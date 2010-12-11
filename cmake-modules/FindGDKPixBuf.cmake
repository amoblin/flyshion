# - Try to find GDK-PixBuf
# Once done this will define
#
#  GDKPIXBUF_FOUND - system has GDK-PixBuf
#  GDKPIXBUF_INCLUDE_DIR - the GDK-PixBuf include directory
#  GDKPIXBUF_LIBRARIES - Link these to use GDK-PixBuf
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Copyright (c) 2010, Ni Hui <shuizhuyuanluo@126.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(GDKPIXBUF_INCLUDE_DIR AND GDKPIXBUF_LIBRARIES)
    # in cache already
    set(GDKPIXBUF_FOUND_QUIETLY TRUE)
endif(GDKPIXBUF_INCLUDE_DIR AND GDKPIXBUF_LIBRARIES)

find_path(GDKPIXBUF_INCLUDE_DIR NAMES gdk-pixbuf.h
    PATHS
    /usr/include/gdk-pixbuf-2.0
    /usr/include/gtk-2.0
    /usr/local/include/gtk-2.0
    /sw/include/gtk-2.0
    PATH_SUFFIXES gdk-pixbuf
)

find_library(GDKPIXBUF_LIBRARIES NAMES gdk_pixbuf-2.0)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GDK-PixBuf DEFAULT_MSG GDKPIXBUF_INCLUDE_DIR GDKPIXBUF_LIBRARIES)

mark_as_advanced(GDKPIXBUF_INCLUDE_DIR GDKPIXBUF_LIBRARIES)
