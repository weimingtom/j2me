#
#   
#
# Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License version
# 2 only, as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License version 2 for more details (a copy is
# included at /legal/license.txt).
# 
# You should have received a copy of the GNU General Public License
# version 2 along with this work; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA
# 
# Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
# Clara, CA 95054 or visit www.sun.com if you need additional
# information or have any questions.
#
######################################################################
#
# Makefile for building and packaging.
#
######################################################################

include Platform.gmk

# Workspace directory
MIDP_DIR		= $(CURDIR)/../..

# Build output directory
ifndef MIDP_OUTPUT_DIR
MIDP_OUTPUT_DIR		= $(CURDIR)/output
endif

# Build options should be include before shared gmk
include Options.gmk

# All rules are defined in shared makefiles
include $(MIDP_DIR)/build/common/Top.gmk

# DirectFB specific defines
ifeq ($(USE_DIRECTFB), true)
    include directfb.gmk
endif

ifeq ($(TARGET_ANDROID),true)
    include android.gmk
else
    include hifb.gmk.$(ME_SDK)
endif

ifeq ($(USE_JSR_135),true)
    ifeq ($(TARGET_ANDROID),true)
        include jsr135.gmk.android
    else
        include jsr135.gmk.$(ME_SDK)
    endif
endif

