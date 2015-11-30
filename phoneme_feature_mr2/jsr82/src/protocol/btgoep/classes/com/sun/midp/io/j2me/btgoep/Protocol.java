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
package com.sun.midp.io.j2me.btgoep;

import java.io.IOException;
import javax.microedition.io.Connection;
import javax.microedition.io.StreamConnectionNotifier;
import javax.microedition.io.StreamConnection;
import com.sun.kvem.jsr082.obex.ClientSessionImpl;
import com.sun.kvem.jsr082.obex.SessionNotifierImpl;
import com.sun.midp.security.SecurityToken;
import com.sun.midp.io.BluetoothUrl;
import com.sun.midp.io.BluetoothProtocol;
import com.sun.midp.security.Permissions;
import com.sun.midp.security.ImplicitlyTrustedClass;
import com.sun.midp.jsr082.SecurityInitializer;

/**
 * Provides a wrapper for "btgoep" protocol implementation
 * to answer the GCF style.
 */
public class Protocol extends BluetoothProtocol {

    /**
     * Inner class to request security token from SecurityInitializer.
     * SecurityInitializer should be able to check this inner class name.
     */
    static private class SecurityTrusted
        implements ImplicitlyTrustedClass {};

    /**
     * The internal security token is privileged and allows to skip
     * permissions check for btspp in case ones are allowed for btgoep.
     */
    private static SecurityToken internalSecurityToken =
        SecurityInitializer.requestToken(new SecurityTrusted());
    
    /** Originally requested url saved to reused in btspp transport. */
    String name = null;
    
    /**
     * Constructs an instance.
     */
    public Protocol() {
        super(BluetoothUrl.OBEX);
    }
    
    /**
     * Cheks permissions and opens requested connection.
     * Returns either ClientSession or SessionNotifier for OBEX connections,
     * depending whether client or server URL was specified. Actually all 
     * essential logic is implemented in the superclass, this one only saves
     * resource name to reuse it for opening btspp transport.
     *
     * @param token security token of the calling class
     * @param name the URL for the connection without protocol.
     * @param mode obex supports READ_WRITE mode only.
     * @param timeouts ignored (because it is allowed by spec).
     * @return ClientSession for client url or SessionNotifier for server url.
     * @exception IOException if opening connection fails.
     */
    public Connection openPrim(Object token, String name, int mode, 
            boolean timeouts) throws IOException {
        this.name = name;
        return super.openPrim(token, name, mode, timeouts);
    }

    /** 
     * Ensures URL parameters have valid value. Makes nothing actually
     * for all the required checks will be done durin ntspp transport 
     * creation.
     * @param url URL to check
     */
    protected void checkUrl(BluetoothUrl url) {}
    
    /**
     * Creates ClientSession connection over the rfcomm transport layer 
     * @param token security token of the calling class
     * @param mode       I/O access mode
     * @return ClientSession connection instance
     * @exception IOException if openning connection fails.
     */
    protected Connection clientConnection(SecurityToken token, int mode)
                throws IOException {
        checkForPermission(token, Permissions.OBEX_CLIENT);

        StreamConnection sock = (StreamConnection)
            new com.sun.midp.io.j2me.btspp.Protocol().
            openPrim(getBtsppToken(token), name, mode, false);
        return new ClientSessionImpl(new BTGOEPConnection(sock));
    }

    /**
     * Creates server connection over the rfcomm transport layer 
     * @param token security token of the calling class
     * @param mode       I/O access mode
     * @return server connection instance
     * @exception IOException if openning connection fails.
     */
    protected Connection serverConnection(SecurityToken token, int mode)
                throws IOException {
        checkForPermission(token, Permissions.OBEX_SERVER);
        
        StreamConnectionNotifier sock = (StreamConnectionNotifier)
            new com.sun.midp.io.j2me.btspp.Protocol().
            openPrim(getBtsppToken(token), name, mode, false);
        return new SessionNotifierImpl(new BTGOEPNotifier(sock));
    }

    /** 
     * Retrieves privileged token if there is no defined one.
     * @param token security token on hands or <code>null</code> 
     *        if there is no one 
     * @return privileged security token that allow acces to btspp if
     *     given token is <code>null</code>, the token given otherwise.
     */
    private static SecurityToken getBtsppToken(SecurityToken token) {
        return (token == null) ? internalSecurityToken : token;
    }
}
