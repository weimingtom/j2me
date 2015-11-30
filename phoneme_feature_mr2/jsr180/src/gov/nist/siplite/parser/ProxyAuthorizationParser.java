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
package gov.nist.siplite.parser;

import gov.nist.siplite.header.*;
import gov.nist.core.*;

/**
 * Parser for ProxyAuthorization headers.
 *
 * @version JAIN-SIP-1.1
 *
 *
 * <a href="{@docRoot}/uncopyright.html">This code is in the public domain.</a>
 *
 */
public class ProxyAuthorizationParser extends ChallengeParser {
    /** Default constructor. */
    ProxyAuthorizationParser() {}
    
    /**
     * Constructor with initial proxty authorization string.
     * @param proxyAuthorization -- header to parse
     */
    public ProxyAuthorizationParser(String proxyAuthorization) {
        super(proxyAuthorization);
    }
    
    /**
     * Constructor with initial lexer engine.
     * @param lexer initial lexer engine
     */
    protected ProxyAuthorizationParser(Lexer lexer) {
        super(lexer);
    }
    
    /**
     * Parses the String message.
     * @return Header (ProxyAuthenticate object)
     * @throws ParseException if the message does not respect the spec.
     */
    public Header parse() throws ParseException {
        headerName(TokenTypes.PROXY_AUTHORIZATION);
        ProxyAuthorizationHeader proxyAuth =
                new ProxyAuthorizationHeader();
        super.parse(proxyAuth);
        return proxyAuth;
    }
    
}

