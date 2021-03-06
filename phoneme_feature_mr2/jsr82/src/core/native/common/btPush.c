/*
 *   
 *
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#include "btPush.h"

#include "btBCC.h"
#include "btSDDB.h"
#include "btStack.h"
#include "btL2CAPConnection.h"
#include "btRFCOMMConnection.h"

#include <stdio.h>
#include <string.h>

#include <midp_libc_ext.h>
#include <midpStorage.h>
#include <pcsl_memory.h>
#include <pcsl_string.h>
#include <midp_logging.h>
#include <push_server_export.h>

#define DEFAULT_MTU 672

/** Filename to save Bluetooth push data. */
PCSL_DEFINE_STATIC_ASCII_STRING_LITERAL_START(BT_PUSH_FILENAME)
    {'B', 't', 'P', 'u', 's', 'h', '\0'} /* "BtPush" */
PCSL_DEFINE_STATIC_ASCII_STRING_LITERAL_END(BT_PUSH_FILENAME);

/** First entry in the linked list representing the internal registry. */
static bt_push_t *g_registry;
static int g_count;
static bt_bdaddr_t inp_bdaddr;

static int get_hex(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

static int test_prefix(const char **pstring, const char *prefix)
{
    int len = strlen(prefix);
    if (!midp_strncasecmp(*pstring, prefix, len)) {
        *pstring += len;
        return 1;
    }
    return 0;
}

static bt_bool_t read_bool(const char **pstring)
{
    if (test_prefix(pstring, "true")) {
        return BT_BOOL_TRUE;
    }
    if (test_prefix(pstring, "false")) {
        return BT_BOOL_FALSE;
    }
    /* unexpected boolean value */
    REPORT_ERROR(LC_PUSH, "Invalid boolean value.");
    return BT_BOOL_FALSE;
}

static unsigned short read_short(const char **pstring)
{
    unsigned short retval = 0;
    while (**pstring >= '0' && **pstring <= '9') {
        retval *= 10;
        retval += *(*pstring++) - '0';
    }
    return retval;
}

static bt_push_t *find_push(const bt_port_t *port, bt_push_t **prev)
{
    bt_push_t *push = g_registry;
    if (prev != NULL) {
        *prev = NULL;
    }
    while (push != NULL) {
        if (push->port.protocol == port->protocol &&
                !memcmp(push->port.uuid, port->uuid, sizeof(bt_uuid_t))) {
            return push;
        }
        if (prev != NULL) {
            *prev = push;
        }
        push = push->next;
    }
    return NULL;
}

static bt_result_t close_handle(bt_protocol_t protocol, bt_handle_t handle)
{
    bt_result_t result = BT_RESULT_FAILURE;
    if (handle == BT_INVALID_HANDLE) {
        return BT_RESULT_SUCCESS;
    }
    switch (protocol) {
        case BT_L2CAP:
            result = bt_l2cap_close(handle);
            break;
        case BT_SPP:
        case BT_GOEP:
            result = bt_rfcomm_close(handle);
            break;
        default:
            break;
    }
    return result;
}

static void close_all(bt_push_t *push)
{
    bt_client_t *client = push->client;
    close_handle(push->port.protocol, push->server);
    while (client != NULL) {
        bt_client_t *next = client->next;
        close_handle(push->port.protocol, client->handle);
        pcsl_mem_free(client);
        client = next;
    }
    push->client = NULL;
}

static void push_save()
{
    char *error;
    bt_push_t *push = g_registry;
    pcsl_string full_name = PCSL_STRING_NULL;
    int storage;
    pcsl_string_cat(storage_get_root(INTERNAL_STORAGE_ID),
                    &BT_PUSH_FILENAME, &full_name);
    storage = storage_open(&error, &full_name, OPEN_READ_WRITE_TRUNCATE);
    pcsl_string_free(&full_name);
    if (error != NULL) {
        REPORT_ERROR1(LC_PUSH, "Error opening `BtPush' file: %s", error);
        storageFreeError(error);
        return;
    }
    storageWrite(&error, storage, (char *)&g_count, sizeof(g_count));
    while (push != NULL && error != NULL) {
        bt_push_t *next = push->next;
        storageWrite(&error, storage, (char *)&push->port, sizeof(bt_port_t));
        if (error != NULL) {
            break;
        }
        storageWrite(&error, storage, (char *)&push->params,
                sizeof(bt_params_t));
        if (error != NULL) {
            break;
        }
        storageWrite(&error, storage, (char *)&push->record.classes,
            sizeof(push->record.classes));
        if (error != NULL) {
            break;
        }
        storageWrite(&error, storage, (char *)&push->record.size,
            sizeof(push->record.size));
        if (error != NULL) {
            break;
        }
        storageWrite(&error, storage, (char *)push->record.data,
                push->record.size);
        if (error != NULL) {
            break;
        }
        push = next;
    }
    if (error != NULL) {
        REPORT_ERROR1(LC_PUSH, "Error writing `BtPush' file: %s", error);
        storageFreeError(error);
    }
    storageClose(&error, storage);
    storageFreeError(error);
}

bt_result_t bt_push_startup()
{
    int i;
    char *error;
    pcsl_string full_name = PCSL_STRING_NULL;
    int storage;
    REPORT_INFO(LC_PUSH, "Bluetooth PushRegistry is now starting.");
    bt_sddb_startup();
    pcsl_string_cat(storage_get_root(INTERNAL_STORAGE_ID),
                    &BT_PUSH_FILENAME, &full_name);
    if (!storage_file_exists(&full_name)) {
        pcsl_string_free(&full_name);
        return BT_RESULT_SUCCESS;
    }
    storage = storage_open(&error, &full_name, OPEN_READ);
    pcsl_string_free(&full_name);
    if (error != NULL) {
        REPORT_ERROR1(LC_PUSH, "Failed to open `BtPush' file: %s", error);
        storageFreeError(error);
        return BT_RESULT_FAILURE;
    }
    storageRead(&error, storage, (char *)&g_count, sizeof(g_count));
    for (i = 0; error != NULL && i < g_count; i++) {
        bt_push_t *push = (bt_push_t *)pcsl_mem_malloc(sizeof(bt_push_t));
        if (push == NULL) {
            REPORT_ERROR(LC_PUSH, "Failed to allocate memory.");
            storageClose(&error, storage);
            storageFreeError(error);
            return BT_RESULT_FAILURE;
        }
        storageRead(&error, storage, (char *)&push->port, sizeof(push->port));
        if (error != NULL) {
            pcsl_mem_free(push);
            break;
        }
        storageRead(&error, storage, (char *)&push->params,
                sizeof(push->params));
        if (error != NULL) {
            pcsl_mem_free(push);
            break;
        }
        push->record.id = BT_INVALID_SDDB_HANDLE;
        storageRead(&error, storage, (char *)&push->record.classes,
                sizeof(push->record.classes));
        if (error != NULL) {
            pcsl_mem_free(push);
            break;
        }
        storageRead(&error, storage, (char *)&push->record.size,
                sizeof(push->record.size));
        if (error != NULL) {
            pcsl_mem_free(push);
            break;
        }
        push->record.data = pcsl_mem_malloc(push->record.size);
        if (push->record.data == NULL) {
            pcsl_mem_free(push);
            REPORT_ERROR(LC_PUSH, "Failed to allocate memory.");
            storageClose(&error, storage);
            storageFreeError(error);
            return BT_RESULT_FAILURE;
        }
        storageRead(&error, storage, (char *)push->record.data,
                push->record.size);
        if (error != NULL) {
            pcsl_mem_free(push->record.data);
            pcsl_mem_free(push);
            break;
        }
        push->server = BT_INVALID_HANDLE;
        push->client = NULL;
        push->next = g_registry;
        g_registry = push;
    }
    if (error != NULL) {
        REPORT_ERROR1(LC_PUSH, "Error reading `BtPush' file: %s", error);
        storageFreeError(error);
        storageClose(&error, storage);
        storageFreeError(error);
        return BT_RESULT_FAILURE;
    }
    REPORT_INFO1(LC_PUSH, "%d record(s) read.", g_count);
    storageClose(&error, storage);
    storageFreeError(error);
    if (g_count > 0) {
        // attempt to enable Bluetooth radio, if it is not already on
        bt_bool_t enabled;
        bt_stack_initialize();
        if (bt_stack_is_enabled(&enabled) == BT_RESULT_SUCCESS &&
                enabled == BT_BOOL_FALSE) {
            bt_stack_enable();
        }
    }
    return BT_RESULT_SUCCESS;
}

void bt_push_shutdown()
{
    bt_push_t *push = g_registry;
    REPORT_INFO(LC_PUSH, "Shutting down Bluetooth PushRegistry.");
    while (push != NULL) {
        bt_push_t *next = push->next;
        bt_sddb_remove_record(push->record.id);
        pcsl_mem_free(push->record.data);
        close_all(push);
        pcsl_mem_free(push);
        push = next;
    }
    g_registry = NULL;
    bt_sddb_shutdown();
}

bt_bool_t bt_is_bluetooth_url(const char *url)
{
    if (test_prefix(&url, "btl2cap://")) {
        return BT_BOOL_TRUE;
    }
    if (test_prefix(&url, "btspp://")) {
        return BT_BOOL_TRUE;
    }
    if (test_prefix(&url, "btgoep://")) {
        return BT_BOOL_TRUE;
    }
    return BT_BOOL_FALSE;
}

bt_result_t bt_push_parse_url(const char *url, bt_port_t *port,
        bt_params_t *params)
{
    int i;
    if (test_prefix(&url, "btl2cap://")) {
        port->protocol = BT_L2CAP;
    } else if (test_prefix(&url, "btspp://")) {
        port->protocol = BT_SPP;
    } else if (test_prefix(&url, "btgoep://")) {
        port->protocol = BT_GOEP;
    } else {
        return BT_RESULT_FAILURE;
    }
    if (!test_prefix(&url, "localhost:")) {
        return BT_RESULT_FAILURE;
    }
    for (i = 0; i < 16; i++) {
        int hex1, hex2;
        hex1 = get_hex(*url++);
        if (hex1 < 0) {
            return BT_RESULT_FAILURE;
        }
        hex2 = get_hex(*url++);
        if (hex2 < 0) {
            return BT_RESULT_FAILURE;
        }
        port->uuid[i] = hex1 << 4 | hex2;
    }
    if (params == NULL) {
        /* params parsing is not needed */
        return BT_RESULT_SUCCESS;
    }
    params->authenticate = BT_BOOL_FALSE;
    params->authorize = BT_BOOL_FALSE;
    params->encrypt = BT_BOOL_FALSE;
    params->rmtu = DEFAULT_MTU;
    params->tmtu = -1;
    url = strchr(url, ';');
    while (url != NULL) {
        if (test_prefix(&url, ";authenticate=")) {
            params->authenticate = read_bool(&url);
        } else if (test_prefix(&url, ";authorize=")) {
            params->authorize = read_bool(&url);
        } else if (test_prefix(&url, ";encrypt=")) {
            params->encrypt = read_bool(&url);
        } else if (test_prefix(&url, ";master=")) {
            params->master = read_bool(&url);
        } else if (test_prefix(&url, ";receiveMTU=")) {
            params->rmtu = read_short(&url);
        } else if (test_prefix(&url, ";transmitMTU=")) {
            params->tmtu = read_short(&url);
        }
        url = strchr(url + 1, ';');
    }
    if (params->authorize || params->encrypt) {
        params->authenticate = BT_BOOL_TRUE;
    }
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_push_register_url(const char *url, const void *data,
        size_t size)
{
    bt_port_t port;
    bt_params_t params;
    bt_push_t *push;
    REPORT_INFO(LC_PUSH, "Bluetooth PushRegistry URL registration:");
    REPORT_INFO1(LC_PUSH, "%s", url);
    bt_push_parse_url(url, &port, &params);
    push = find_push(&port, NULL);
    if (push != NULL) {
        /* found existing entry with the same protocol/uuid, can not proceed */
        REPORT_ERROR(LC_PUSH, "Entry already exists, registration failed.");
        return BT_RESULT_FAILURE;
    }
    /* save the entry in the registry */
    push = (bt_push_t *)pcsl_mem_malloc(sizeof(bt_push_t));
    if (push == NULL) {
        REPORT_ERROR(LC_PUSH, "Failed to allocate memory.");
        return BT_RESULT_FAILURE;
    }
    memcpy(&push->port, &port, sizeof(bt_port_t));
    memcpy(&push->params, &params, sizeof(bt_params_t));
    push->record.id = BT_INVALID_SDDB_HANDLE;
    push->record.classes = 0;
    if (data != NULL) {
        push->record.data = pcsl_mem_malloc(size);
        if (push->record.data == NULL) {
            pcsl_mem_free(push);
            return BT_RESULT_FAILURE;
        }
        memcpy(push->record.data, data, size);
    } else {
        push->record.data = NULL;
    }
    push->record.size = size;
    push->server = BT_INVALID_HANDLE;
    push->client = NULL;
    push->next = g_registry;
    g_registry = push;
    g_count++;
    push_save();
    REPORT_INFO(LC_PUSH, "Registration successful.");
    return BT_RESULT_SUCCESS;
}

bt_result_t bt_push_unregister_url(const char *url)
{
    bt_port_t port;
    bt_push_t *push, *prev;
    REPORT_INFO(LC_PUSH, "Bluetooth PushRegistry URL un-registration:");
    REPORT_INFO1(LC_PUSH, "%s", url);
    bt_push_parse_url(url, &port, NULL);
    push = find_push(&port, &prev);
    if (push == NULL) {
        return BT_RESULT_FAILURE;
    }
    /* remove the service record */
    bt_sddb_remove_record(push->record.id);
    /* close server and client connections */
    close_all(push);
    /* remove the entry */
    if (prev != NULL) {
        prev->next = push->next;
    } else {
        g_registry = push->next;
    }
    g_count--;
    pcsl_mem_free(push->record.data); /* NULL is valid */
    pcsl_mem_free(push);
    push_save();
    bt_stack_set_service_classes(bt_sddb_get_service_classes(0));
    REPORT_INFO(LC_PUSH, "Un-registration successful.");
    return BT_RESULT_SUCCESS;
}


/**
 * Gets friendly mode of bluetooth address .
 *
 * @param btaddr input bluetooth address
 * @param frrAddr (OUT) array which is filled by
 *        address in friendly mode 
 */
void getFriendlyAddr(const bt_bdaddr_t bdaddr, unsigned char address[]) {
    int i,j;
    uint8_t* bt_ptr;
    unsigned char sym;
    bt_ptr = (uint8_t*)bdaddr;
    j = 0;
    for (i = 0; i < BT_ADDRESS_SIZE; i++) {
        sym = (*bt_ptr >> 4) & 0xF;
        if (sym < 10) {
            sym += '0';
        } else {
            sym += 'A' - 1;
        }
        address[j++] = sym;
        sym = *bt_ptr & 0xF;
        if (sym < 10) {
            sym += '0';
        } else {
            sym += 'A' - 1;
        }
        address[j++] = sym;
    }
    address[j] = 0;
}

bt_bool_t bt_push_test_filter(const bt_bdaddr_t bdaddr, const char *filter)
{
    unsigned char address[2 * BT_ADDRESS_SIZE + 1];
    bt_bool_t auth;

    /* Transform bluetooth address to friendly mode */
    getFriendlyAddr(bdaddr, address);
    if (!wildComp(filter, address)) {
        return BT_BOOL_FALSE;
    }
    if (bt_bcc_is_trusted(bdaddr, &auth) != BT_RESULT_SUCCESS) {
        return BT_BOOL_FALSE;
    }
    if (auth == BT_BOOL_TRUE) {
        if (!test_prefix(&filter, ";authorized")) {
            return BT_BOOL_FALSE;
        }
    } else {
        if (bt_bcc_is_authenticated(bdaddr, &auth) != BT_RESULT_SUCCESS) {
            return BT_BOOL_FALSE;
        }
        if (auth == BT_BOOL_TRUE) {
            if (!test_prefix(&filter, ";authenticated")) {
                return BT_BOOL_FALSE;
            }
        }
    }
    if (test_prefix(&filter, ";blacklist=")) {
        while (*filter != '\0') {
            if (wildComp(filter, address)) {
                return BT_BOOL_FALSE;
            }
            if (*filter == ';') {
                filter++;
            } else if (*filter != '\0') {
                return BT_BOOL_FALSE;
            }
        }
    }
    return BT_BOOL_TRUE;
}

bt_pushid_t bt_push_find_server(bt_handle_t server)
{
    bt_push_t *push = g_registry;
    while (push != NULL) {
        if (push->server == server) {
            return (bt_pushid_t)push;
        }
        push = push->next;
    }
    return BT_INVALID_PUSH_HANDLE;
}

bt_pushid_t bt_push_find_client(bt_handle_t handle)
{
    bt_push_t *push = g_registry;
    while (push != NULL) {
        bt_client_t *client = push->client;
        while (client != NULL) {
            if (client->handle == handle) {
                return (bt_pushid_t)push;
            }
            client = client->next;
        }
        push = push->next;
    }
    return BT_INVALID_PUSH_HANDLE;
}

bt_bool_t bt_push_accept(bt_pushid_t pushid, const char *filter,
        bt_handle_t *handle)
{
    void *context;
    bt_push_t *push = (bt_push_t *)pushid;
    bt_client_t *client;
    if (push == NULL) {
        return BT_BOOL_FALSE;
    }
    client = (bt_client_t *)pcsl_mem_malloc(sizeof(bt_client_t));
    switch (push->port.protocol) {
        case BT_L2CAP:
            if (bt_l2cap_accept_start(push->server,
                    &client->handle, client->bdaddr,
                    &client->rmtu, &client->tmtu,
                    &context) != BT_RESULT_SUCCESS) {
                pcsl_mem_free(client);
                return BT_BOOL_FALSE;
            }
            break;
        case BT_SPP:
        case BT_GOEP:
            if (bt_rfcomm_accept_start(push->server,
                    &client->handle, client->bdaddr,
                    &context) != BT_RESULT_SUCCESS) {
                pcsl_mem_free(client);
                return BT_BOOL_FALSE;
            }
            break;
        default:
            pcsl_mem_free(client);
            return BT_BOOL_FALSE;
    }
    if (bt_push_test_filter(client->bdaddr, filter) == BT_BOOL_TRUE) {
        bt_client_t **client_ptr = &push->client;
        while (*client_ptr != NULL) {
            client_ptr = &(*client_ptr)->next;
        }
        *client_ptr = client;
        client->next = NULL;
        *handle = client->handle;
        memcpy(inp_bdaddr, client->bdaddr, sizeof(BT_ADDRESS_SIZE));
        return BT_BOOL_TRUE;
    }
    close_handle(push->port.protocol, client->handle);
    pcsl_mem_free(client);
    return BT_BOOL_FALSE;
}

uint8_t* getInpAddress() {
    return inp_bdaddr;
}

bt_pushid_t bt_push_checkout_server(const bt_port_t *port, bt_handle_t *server,
        bt_sddbid_t *sddbid)
{
    bt_push_t *push = find_push(port, NULL);
    if (push == NULL || push->server == BT_INVALID_HANDLE) {
        return BT_INVALID_PUSH_HANDLE;
    }
    *server = push->server;
    if (sddbid != NULL) {
        *sddbid = push->record.id;
    }
    push->server = BT_INVALID_HANDLE;
    return (bt_pushid_t)push;
}

bt_result_t bt_push_checkout_client(bt_pushid_t pushid, bt_handle_t *handle,
        bt_bdaddr_t bdaddr, int *rmtu, int *tmtu)
{
    bt_push_t *push = (bt_push_t *)pushid;
    bt_client_t *client;
    if (push == NULL || push->client == NULL) {
        return BT_RESULT_FAILURE;
    }
    client = push->client;
    *handle = client->handle;
    memcpy(bdaddr, client->bdaddr, sizeof(bt_bdaddr_t));
    if (rmtu != NULL) {
        *rmtu = client->rmtu;
    }
    if (tmtu != NULL) {
        *tmtu = client->tmtu;
    }
    push->client = client->next;
    pcsl_mem_free(client);
    return BT_RESULT_SUCCESS;
}

bt_handle_t bt_push_start_server(const bt_port_t *port)
{
    int psm, cn;
    bt_params_t *params;
    bt_push_t *push = find_push(port, NULL);
    if (push == NULL || push->server != BT_INVALID_HANDLE) {
        return BT_INVALID_HANDLE;
    }
    if (bt_sddb_update_record(&push->record) != BT_RESULT_SUCCESS) {
        return BT_INVALID_HANDLE;
    }
    params = &push->params;
    switch (port->protocol) {
        case BT_L2CAP:
            if (bt_l2cap_create_server(params->rmtu, params->tmtu,
                    params->authenticate, params->authorize, params->encrypt,
                    params->master, &push->server, &psm) !=
                    BT_RESULT_SUCCESS) {
                return BT_INVALID_HANDLE;
            }
            bt_sddb_update_psm(push->record.id, psm);
            bt_l2cap_listen(push->server);
            break;
        case BT_SPP:
        case BT_GOEP:
            if (bt_rfcomm_create_server(params->authenticate, params->authorize,
                    params->encrypt, params->master, &push->server, &cn) !=
                    BT_RESULT_SUCCESS) {
                return BT_INVALID_HANDLE;
            }
            bt_sddb_update_channel(push->record.id, cn);
            bt_rfcomm_listen(push->server);
            break;
        default:
            return BT_INVALID_HANDLE;
    }
    bt_stack_set_service_classes(bt_sddb_get_service_classes(0));
    return push->server;
}

bt_result_t bt_push_reject(bt_pushid_t pushid)
{
    bt_push_t *push = (bt_push_t *)pushid;
    bt_client_t *client;
    if (push == NULL) {
        return BT_RESULT_FAILURE;
    }
    client = push->client;
    while (client != NULL) {
        bt_client_t *next = client->next;
        close_handle(push->port.protocol, client->handle);
        pcsl_mem_free(client);
        client = next;
    }
    push->client = NULL;
    return BT_RESULT_SUCCESS;
}

bt_sddbid_t bt_push_get_record(const bt_port_t *port)
{
    bt_push_t *push = find_push(port, NULL);
    return push != NULL ? push->record.id : BT_INVALID_SDDB_HANDLE;
}

bt_result_t bt_push_update_record(bt_sddbid_t sddbid,
        const bt_record_t *record)
{
    bt_push_t *push = g_registry;
    while (push != NULL) {
        bt_record_t *rec = &push->record;
        if (rec->id == sddbid) {
            rec->id = record->id;
            rec->classes = record->classes;
            if (rec->size != record->size) {
                void *data = pcsl_mem_realloc(rec->data, record->size);
                if (data == NULL) {
                    return BT_RESULT_FAILURE;
                }
                rec->data = data;
                rec->size = record->size;
            }
            memcpy(rec->data, record->data, record->size);
            push_save();
            return BT_RESULT_SUCCESS;
        }
        push = push->next;
    }
    return BT_RESULT_FAILURE;
}
