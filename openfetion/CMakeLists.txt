project(openfetion)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(TARNAME openfetion)

# Program name, version, etc
set(PACKAGE_VERSION "2.1.0")
set(PACKAGE_NAME "OpenFetion")
set(PACKAGE_TARNAME "${TARNAME}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "http://code.google.com/p/ofetion/issues/list")

##########################################################################

if(NOT DEFINED BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
endif(NOT DEFINED BIN_INSTALL_DIR)
if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif(NOT DEFINED LIB_INSTALL_DIR)
if(NOT DEFINED DATA_INSTALL_DIR)
    set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share")
endif(NOT DEFINED DATA_INSTALL_DIR)
if(NOT DEFINED MAN_INSTALL_DIR)
    set(MAN_INSTALL_DIR "${DATA_INSTALL_DIR}/man")
endif(NOT DEFINED MAN_INSTALL_DIR)
if(NOT DEFINED RESOURCE_INSTALL_DIR)
    set(RESOURCE_INSTALL_DIR "${DATA_INSTALL_DIR}/openfetion/resource/")
endif(NOT DEFINED RESOURCE_INSTALL_DIR)
if(NOT DEFINED SKIN_INSTALL_DIR)
    set(SKIN_INSTALL_DIR "${DATA_INSTALL_DIR}/openfetion/skin/")
endif(NOT DEFINED SKIN_INSTALL_DIR)
if(NOT DEFINED LOCALE_INSTALL_DIR)
    set(LOCALE_INSTALL_DIR "${DATA_INSTALL_DIR}/locale/")
endif(NOT DEFINED LOCALE_INSTALL_DIR)

##########################################################################

add_definitions(-Wall -Wextra)
add_definitions(-DRESOURCE_DIR="${RESOURCE_INSTALL_DIR}")
add_definitions(-DSKIN_DIR="${SKIN_INSTALL_DIR}")
add_definitions(-DLOCALE_DIR="${LOCALE_INSTALL_DIR}")

##########################################################################

# compile option switch
option(NLS "Native language support" ON)
option(WITH_LIBNOTIFY "Libnotify support" ON)
option(WITH_GSTREAMER "GStreamer support" ON)
option(WITH_LIBXSS "LibXScrnSaver support" ON)
option(WITH_NETWORKMANAGER "NetworkManager support" ON)

# openfetion build requirements
find_package(PkgConfig REQUIRED)
if(NLS)
	find_package(Gettext)
endif(NLS)

# openfetion build requirements
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)
pkg_check_modules(GLIB2 REQUIRED glib-2.0)
#if use system ofetion, need check ofetion, 
#if build locally, no need to check
pkg_check_modules(OFETION REQUIRED ofetion)

if(WITH_LIBNOTIFY)
    pkg_check_modules(LIBNOTIFY libnotify)
endif(WITH_LIBNOTIFY)
if(WITH_GSTREAMER)
    pkg_check_modules(GSTREAMER gstreamer-0.10)
endif(WITH_GSTREAMER)
if(WITH_LIBXSS)
    pkg_check_modules(LIBXSS xscrnsaver)
endif(WITH_LIBXSS)
if(WITH_NETWORKMANAGER)
    pkg_check_modules(NETWORKMANAGER NetworkManager)
    pkg_check_modules(DBUSGLIB dbus-glib-1)
endif(WITH_NETWORKMANAGER)

##########################################################################

message("==========================================")
if(NLS AND GETTEXT_FOUND)
    set(ENABLE_NLS 1)
    message(STATUS "Native language support:  YES" )
else(NLS AND GETTEXT_FOUND)
    message(STATUS "Native language support:  NO" )
endif(NLS AND GETTEXT_FOUND)
if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    set(USE_LIBNOTIFY 1)
    if(LIBNOTIFY_VERSION LESS 0.7)
    	set(LIBNOTIFY_OLD 1)
    	message(STATUS "Libnotify support:        OLD" )
    else(LIBNOTIFY_VERSION LESS 0.7)
    	message(STATUS "Libnotify support:        NEW" )
    endif(LIBNOTIFY_VERSION LESS 0.7)
else(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    message(STATUS "Libnotify support:        NO" )
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    set(USE_GSTREAMER 1)
    message(STATUS "GStreamer support:        YES" )
else(WITH_GSTREAMER AND GSTREAMER_FOUND)
    message(STATUS "GStreamer support:        NO" )
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)
if(WITH_LIBXSS AND LIBXSS_FOUND)
    set(USE_LIBXSS 1)
    message(STATUS "Xscreensaver support:     YES" )
else(WITH_LIBXSS AND LIBXSS_FOUND)
    message(STATUS "Xscreensaver support:     NO" )
endif(WITH_LIBXSS AND LIBXSS_FOUND)
if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND AND DBUSGLIB_FOUND)
    set(USE_NETWORKMANAGER 1)
    message(STATUS "NetworkManager support:   YES" )
else(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND AND DBUSGLIB_FOUND)
    message(STATUS "NetworkManager support:   NO" )
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND AND DBUSGLIB_FOUND)
message("==========================================")

configure_file(config-ofetion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

##########################################################################

include_directories(
    ${GTK2_INCLUDE_DIRS}
    ${GDKPIXBUF_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${OFETION_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    include_directories(${LIBNOTIFY_INCLUDE_DIRS})
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)

if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    include_directories(${GSTREAMER_INCLUDE_DIRS})
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)

if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
    include_directories(${NETWORKMANAGER_INCLUDE_DIRS})
    include_directories(${DBUSGLIB_INCLUDE_DIRS})
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)

set(OPENFETION_SRC
    src/fx_about.c
    src/fx_addbuddy.c
    src/fx_add_group.c
    src/fx_app.c
    src/fx_bottom.c
    src/fx_chat.c
    src/fx_close.c
    src/fx_code.c
    src/fx_conn.c
    src/fx_dsms.c
    src/fx_edit.c
    src/fx_emotion.c
    src/fx_gedit.c
    src/fx_head.c
    src/fx_history.c
    src/fx_idle.c
    src/fx_login.c
    src/fx_lookup.c
    src/fx_lookupres.c
    src/fx_main.c
    src/fx_many.c
    src/fx_myself.c
    src/fx_pggroup.c
    src/fx_pgprofile.c
    src/fx_profile.c
    src/fx_proxy.c
    src/fx_search.c
    src/fx_set.c
    src/fx_share.c
    src/fx_smsstat.c
    src/fx_sound.c
    src/fx_sysmsg.c
    src/fx_tree.c
    src/fx_util.c
    src/fx_server.c
    src/fx_cli.c
)

add_executable(openfetion ${OPENFETION_SRC})
target_link_libraries(openfetion 
    ${GTK2_LIBRARIES} ${GDKPIXBUF_LIBRARIES} ${GTHREAD2_LIBRARIES}
    ${OFETION_LIBRARIES})
if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    target_link_libraries(openfetion ${LIBNOTIFY_LIBRARIES})
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    target_link_libraries(openfetion ${GSTREAMER_LIBRARIES})
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)
if(WITH_LIBXSS AND LIBXSS_FOUND)
    target_link_libraries(openfetion ${LIBXSS_LIBRARIES})
endif(WITH_LIBXSS AND LIBXSS_FOUND)
if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND AND DBUSGLIB_FOUND)
    target_link_libraries(openfetion ${NETWORKMANAGER_LIBRARIES})
    target_link_libraries(openfetion ${DBUSGLIB_LIBRARIES})
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND AND DBUSGLIB_FOUND)

##########################################################################

install(TARGETS openfetion RUNTIME DESTINATION ${BIN_INSTALL_DIR})

if(NLS AND GETTEXT_FOUND)
	file(GLOB POFILES "${CMAKE_CURRENT_SOURCE_DIR}/po/*.po")
	gettext_create_translations("${CMAKE_CURRENT_SOURCE_DIR}/po/openfetion.pot" ALL ${POFILES})
endif(NLS AND GETTEXT_FOUND)

file(GLOB OPENFETION_SKINIMAGES "skin/*.png" "skin/*.svg" "skin/*.gif")
file(GLOB OPENFETION_FACEIMAGES "skin/face_images/*.gif")
install(FILES ${OPENFETION_SKINIMAGES} DESTINATION ${SKIN_INSTALL_DIR})
install(FILES ${OPENFETION_FACEIMAGES} DESTINATION ${SKIN_INSTALL_DIR}/face_images/)

if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    install(FILES resource/newmessage.wav DESTINATION ${RESOURCE_INSTALL_DIR})
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)
install(FILES resource/openfetion.desktop DESTINATION ${DATA_INSTALL_DIR}/applications)
install(FILES skin/fetion.svg DESTINATION ${DATA_INSTALL_DIR}/pixmaps)
install(FILES openfetion.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
