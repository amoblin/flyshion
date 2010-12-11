# - Try to find GLIB2
# Once done this will define
#
#  GLIB2_FOUND - system has glib2
#  GLIB2_INCLUDE_DIR - the glib2 include directory
#  GLIB2_LIBRARIES - Link these to use glib2
#  GTHREAD2_LIBRARIES - Link these to use gthread2
#  GLIB2_DEFINITIONS - Compiler switches required for using glib2
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Copyright (c) 2010, Ni Hui <shuizhuyuanluo@126.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)
    # in cache already
    set(GLIB2_FOUND_QUIETLY TRUE)
endif(GLIB2_INCLUDE_DIR AND GLIB2_LIBRARIES)

if(NOT WIN32)
   find_package(PkgConfig)
   pkg_check_modules(PKG_GLIB2 glib-2.0)
   set(GLIB2_DEFINITIONS ${PKG_GLIB2_CFLAGS})
endif(NOT WIN32)

find_path(GLIB2_MAIN_INCLUDE_DIR NAMES glib.h
    PATHS
    ${PKG_GLIB2_INCLUDE_DIRS}
    PATH_SUFFIXES glib-2.0
)

find_library(GLIB2_LIBRARIES NAMES glib-2.0
    PATHS
    ${PKG_GLIB2_LIBRARY_DIRS}
)

find_library(GTHREAD2_LIBRARIES NAMES gthread-2.0
    PATHS
    ${PKG_GLIB2_LIBRARY_DIRS}
)

set(GLIB2_INCLUDE_DIR ${GLIB2_MAIN_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLIB2 DEFAULT_MSG GLIB2_INCLUDE_DIR GLIB2_LIBRARIES GTHREAD2_LIBRARIES GLIB2_MAIN_INCLUDE_DIR)

mark_as_advanced(GLIB2_INCLUDE_DIR GLIB2_LIBRARIES GTHREAD2_LIBRARIES GLIB2_MAIN_INCLUDE_DIR)
