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
 * 
 * This source file is specific for Qt-based configurations.
 */

#include <midp_slavemode_port.h>
#include <qteapp_export.h>

/**
 * Requests that the VM control code schedule a time slice as soon
 * as possible, since Java threads are waiting to be run.
 */
extern "C"
void midp_slavemode_port_schedule_vm_timeslice(void) {
    qteapp_get_mscreen()->setNextVMTimeSlice(0);
}

/**
 * Runs the platform-specific event loop.
 */
extern "C"
void midp_slavemode_port_event_loop(void) {
  qteapp_get_mscreen()->startVM();
 #ifdef FV_USE_QT4
  qteapp_get_application()->exec();
 #else
  qteapp_get_application()->enter_loop();
#endif
  qteapp_get_mscreen()->stopVM();
}
