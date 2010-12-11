# - Try to find NetworkManager
# Once done this will define
#
#  NETWORKMANAGER_FOUND - system has NetworkManager
#  NETWORKMANAGER_INCLUDE_DIRS - the NetworkManager include directories
#  NETWORKMANAGER_LIBRARIES - the libraries needed to use NetworkManager
#  NETWORKMANAGER_CFLAGS - Compiler switches required for using NetworkManager
#  NETWORKMANAGER_VERSION - version number of NetworkManager

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2007, Will Stephenson, <wstephenson@kde.org>
# Copyright (c) 2010, Ni Hui <shuizhuyuanluo@126.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(NETWORKMANAGER_INCLUDE_DIRS AND NM-UTIL_INCLUDE_DIRS)
   # in cache already
   set(NetworkManager_FIND_QUIETLY TRUE)
endif(NETWORKMANAGER_INCLUDE_DIRS AND NM-UTIL_INCLUDE_DIRS)

if(NOT WIN32)
   find_package(PkgConfig)
   pkg_search_module(NETWORKMANAGER NetworkManager)
   pkg_search_module(NM-UTIL libnm-util)
endif(NOT WIN32)

if(NETWORKMANAGER_FOUND AND NM-UTIL_FOUND)
      message(STATUS "Found NetworkManager ${NETWORKMANAGER_VERSION}: ${NETWORKMANAGER_INCLUDE_DIRS}")
      message(STATUS "Found libnm-util: ${NM-UTIL_LIBRARY_DIRS}")
endif(NETWORKMANAGER_FOUND AND NM-UTIL_FOUND)

mark_as_advanced(NETWORKMANAGER_INCLUDE_DIRS NM-UTIL_INCLUDE_DIRS)
