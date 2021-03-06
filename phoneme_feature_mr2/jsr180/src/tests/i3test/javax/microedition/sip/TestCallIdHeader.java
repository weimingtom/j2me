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

package javax.microedition.sip;

import com.sun.midp.i3test.TestCase;

import javax.microedition.io.Connector;
import java.io.IOException;

/**
 * RFC3261, p. 37; BNF: p. 228
 *
 * Call-ID = ( "Call-ID" / "i" ) HCOLON callid
 * callid  = word [ "@" word ]
 *
 * The Call-ID header field acts as a unique identifier to group
 * together a series of messages.  It MUST be the same for all requests
 * and responses sent by either UA in a dialog.  It SHOULD be the same
 * in each registration from a UA.
 *
 * In a new request created by a UAC outside of any dialog, the Call-ID
 * header field MUST be selected by the UAC as a globally unique
 * identifier over space and time unless overridden by method-specific
 * behavior.  All SIP UAs must have a means to guarantee that the Call-
 * ID header fields they produce will not be inadvertently generated by
 * any other UA.  Note that when requests are retried after certain
 * failure responses that solicit an amendment to a request (for
 * example, a challenge for authentication), these retried requests are
 * not considered new requests, and therefore do not need new Call-ID
 * header fields; see Section 8.1.3.5.
 *
 * Use of cryptographically random identifiers (RFC 1750 [12]) in the
 * generation of Call-IDs is RECOMMENDED.  Implementations MAY use the
 * form "localid@host".  Call-IDs are case-sensitive and are simply
 * compared byte-by-byte.
 *
 *    Using cryptographically random identifiers provides some
 *    protection against session hijacking and reduces the likelihood of
 *    unintentional Call-ID collisions.
 *
 * No provisioning or human interface is required for the selection of
 * the Call-ID header field value for a request.
 *
 * For further information on the Call-ID header field, see Section 20.8.
 *
 * Example:
 *
 *    Call-ID: f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com
 *
 */

public class TestCallIdHeader extends SipHeaderBaseTest {

    /** A name of the header that will be tested */
    private final String headerName = "Call-ID";

    /**
     * Body of the test 1.
     *
     * Test for Call-ID header field: setName()/getName().
     */
    void Test1() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test1 started ***");

        SipHeader sh;

        sh = createSipHeader(headerName,
            "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

        if (sh == null) {
            return;
        }

        // Testing getName()...
        String ret_name = sh.getName();
        assertTrue("Invalid header value: " + ret_name,
            ret_name.equals(headerName));

        // Testing setName()...
        try {
           sh.setName(headerName);
        } catch (java.lang.IllegalArgumentException e) {
            fail("setName(" + headerName + ") failed (IAE): " + e);
        } catch (Throwable e) {
            fail("setName(" + headerName + ") failed: " + e);
        }
    }

    /**
     * Body of the test 2.
     *
     * Test for Call-ID header field: getValue()/getHeaderValue().
     */
    void Test2() {
        String val;
        String headerValue = "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com";

        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test2 started ***");

        SipHeader sh = createSipHeader(headerName, headerValue);

        if (sh != null) {
            val = sh.getValue();
            assertTrue("getValue() returned invalid value: '" +
                       val + "'", val.equals(headerValue));

            val = sh.getHeaderValue();
            assertTrue("(1) getHeaderValue() returned invalid " +
                "value: '" + val + "'", val.equals(headerValue));

            // Test if the value can be changed.
            headerValue = "test";
            sh.setValue(headerValue);

            val = sh.getHeaderValue();
            assertTrue("(2) getHeaderValue() returned invalid " +
                "value: '" + val + "'", val.equals(headerValue));
        }
    }

    /**
     * Body of the test 4.
     *
     * Test for Call-ID header field: getParameterNames()/getParameter().
     */
    void Test4() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test4 started ***");

        SipHeader sh = createSipHeader(headerName,
            "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

        if (sh == null) {
            return;
        }

        // Testing getParameterNames()...
        String[] paramList = sh.getParameterNames();

        if (paramList != null) {
            fail("getParameterNames() should return null!");
        }

        // Testing getParameter()...
        String paramVal = sh.getParameter("ttl");
        assertTrue("getParameter() returned '" + paramVal +
            "' for the parameter 'ttl' that doesn't exist.", paramVal == null);
    }

    /**
     * Body of the test 5.
     *
     * Test for Call-ID header field: setParameter()/removeParameter().
     */
    void Test5() {
        // DEBUG:        System.out.println("");
        // DEBUG:        System.out.println("*** Test5 started ***");

        SipHeader sh = createSipHeader(headerName,
            "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

        if (sh == null) {
            return;
        }

        // Testing setParameter()...
        try {
            sh.setParameter("test", "1");
        } catch (Exception e) {
            fail(e + " was thrown.");
        }

        try {
            sh.removeParameter("test");
        } catch (Exception e) {
            fail("removeParameter(): " + e + " was thrown!");
        }

        assertTrue(true); // to avoid error message from the test framework
    }

    /**
     * Run the tests
     */
    public void runTests() {
        declare("setName()/getName()");
        Test1();

        declare("getValue()/getHeaderValue()");
        Test2();

        declare("setValue()");
        testSetValue(headerName,
            "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");

        declare("getParameterNames()/getParameter()");
        Test4();

        declare("setParameter()/removeParameter()");
        Test5();

        declare("toString()");
        testToString(headerName,
            "f81d4fae-7dec-11d0-a765-00a0c91e6bf6@foo.bar.com");
    }
}

