/*
 * Portions Copyright  2000-2007 Sun Microsystems, Inc. All Rights
 * Reserved.  Use is subject to license terms.
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
/*
 */
package gov.nist.javax.sdp.fields;

/**
 * A context sensitive list of keyowrds for SDP Headers.
 */
public interface SDPKeywords {
    /** SDP base64 keyword. */
    public static final String BASE64 = "base64";
    /** SDP prompt keyword. */
    public static final String PROMPT = "prompt";
    /** SDP clear keyword. */
    public static final String CLEAR = "clear";
    /** SDP URI keyword. */
    public static final String URI = "URI";
    /** SDP IPv4 network keyword. */
    public static final String IPV4 = "IP4";
    /** SDP IPV6 network keyword. */
    public static final String IPV6 = "IP6";
    /** SDP Internet type keyword. */
    public static final String INT = "IN";
}
