project(ofetion)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# library version history
# API      ABI
# 0.0.0 => 0.0.0 (released with OpenFetion 1.0.0 - 1.8.0)
# 1.0.0 => 0.0.0 (released with OpenFetion 1.8.0 - 2.0.x)
#

# program name, version etc
set(OFETION_VERSION_MAJOR "2")
set(OFETION_VERSION_MINER "0")
set(OFETION_VERSION_PATCH "7")

# library API version
set(LIBOFETION_MAJOR_VERSION "1")
set(LIBOFETION_MINOR_VERSION "0")
set(LIBOFETION_PATCH_VERSION "0")

# library ABI version
set(LIBOFETION_SO_CUR_VERSION "0")
set(LIBOFETION_SO_REV_VERSION "0")
set(LIBOFETION_SO_AGE_VERSION "0")

set(LIBOFETION_VERSION_STRING
    "${LIBOFETION_MAJOR_VERSION}.${LIBOFETION_MINOR_VERSION}.${LIBOFETION_PATCH_VERSION}"
)
set(LIBOFETION_SO_VERION_STRING
    "${LIBOFETION_SO_CUR_VERSION}.${LIBOFETION_SO_REV_VERSION}.${LIBOFETION_SO_AGE_VERSION}"
)

set(PACKAGE_VERSION "${OFETION_VERSION_MAJOR}.${OFETION_VERSION_MINER}.${OFETION_VERSION_PATCH}")
set(PACKAGE_NAME "OpenFetion")
set(PACKAGE_TARNAME "openfetion")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "http://basiccoder.com/openfetion")

# compile option switch
option(WITH_LIBNOTIFY "Libnotify support" ON)
option(WITH_GSTREAMER "GStreamer support" ON)
option(WITH_LIBXSS "LibXScrnSaver support" ON)
option(WITH_NETWORKMANAGER "NetworkManager support" ON)

# libofetion build requirements
find_package(LibXml2 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Sqlite REQUIRED)

# openfetion build requirements
if(WITH_LIBNOTIFY)
    find_package(Libnotify)
endif(WITH_LIBNOTIFY)
if(WITH_GSTREAMER)
    find_package(GStreamer)
endif(WITH_GSTREAMER)
if(WITH_LIBXSS)
    find_package(X11)
endif(WITH_LIBXSS)
if(WITH_NETWORKMANAGER)
    find_package(NetworkManager)
endif(WITH_NETWORKMANAGER)
find_package(GTK2 REQUIRED)
find_package(GDKPixBuf REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(Gettext REQUIRED)

# gnu ld version script
set(_link_flags)
if(CMAKE_COMPILER_IS_GNUCXX)
    include(TestCXXAcceptsFlag)
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/lib/libofetion.map")
    check_cxx_accepts_flag("-Wl,--version-script,${_version_script}" LD_ACCEPTS_VERSION_SCRIPT)
    if(LD_ACCEPTS_VERSION_SCRIPT)
        set(_link_flags "${_link_flags} -Wl,--version-script,'${_version_script}'")
    endif(LD_ACCEPTS_VERSION_SCRIPT)
endif(CMAKE_COMPILER_IS_GNUCXX)

##########################################################################
message("==========================================")
if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND AND (HAVE_LIBNOTIFY_0_4 OR HAVE_LIBNOTIFY_0_7))
    set(USE_LIBNOTIFY 1)
    if(HAVE_LIBNOTIFY_0_4)
        set(LIBNOTIFY_OLD 1)
    endif(HAVE_LIBNOTIFY_0_4)
    message(STATUS "Libnotify support:        YES" )
else(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND AND (HAVE_LIBNOTIFY_0_4 OR HAVE_LIBNOTIFY_0_7))
    message(STATUS "Libnotify support:        NO" )
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND AND (HAVE_LIBNOTIFY_0_4 OR HAVE_LIBNOTIFY_0_7))
if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    set(USE_GSTREAMER 1)
    message(STATUS "GStreamer support:        YES" )
else(WITH_GSTREAMER AND GSTREAMER_FOUND)
    message(STATUS "GStreamer support:        NO" )
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)
if(WITH_LIBXSS AND X11_Xscreensaver_FOUND)
    set(USE_LIBXSS 1)
    message(STATUS "Xscreensaver support:     YES" )
else(WITH_LIBXSS AND X11_Xscreensaver_FOUND)
    message(STATUS "Xscreensaver support:     NO" )
endif(WITH_LIBXSS AND X11_Xscreensaver_FOUND)
if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
    set(USE_NETWORKMANAGER 1)
    message(STATUS "NetworkManager support:   YES" )
else(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
    message(STATUS "NetworkManager support:   NO" )
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
message("==========================================")

configure_file(config-ofetion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

##########################################################################
if(NOT DEFINED BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
endif(NOT DEFINED BIN_INSTALL_DIR)
if(NOT DEFINED LIB_INSTALL_DIR)
    if(NOT DEFINED LIB_SUFFIX)
        if((CMAKE_SIZEOF_VOID_P EQUAL 4) OR APPLE)
            set(LIB_SUFFIX "")
        else((CMAKE_SIZEOF_VOID_P EQUAL 4) OR APPLE)
            set(LIB_SUFFIX 64)
        endif((CMAKE_SIZEOF_VOID_P EQUAL 4) OR APPLE)
    endif(NOT DEFINED LIB_SUFFIX)
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
endif(NOT DEFINED LIB_INSTALL_DIR)
if(NOT DEFINED DATA_INSTALL_DIR)
    set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share")
endif(NOT DEFINED DATA_INSTALL_DIR)
if(NOT DEFINED INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
endif(NOT DEFINED INCLUDE_INSTALL_DIR)

##########################################################################
add_definitions(-Wall -Wextra)
add_definitions(-DGETTEXT_PACKAGE="${PACKAGE_TARNAME}")
add_definitions(-DRESOURCE_DIR="${DATA_INSTALL_DIR}/openfetion/resource/")
add_definitions(-DSKIN_DIR="${DATA_INSTALL_DIR}/openfetion/skin/")
add_definitions(-DLOCALE_DIR="${DATA_INSTALL_DIR}/locale/")
# add_definitions(-DDEBUG_FOOTPRINT="//  ")

##########################################################################

include_directories(
    ${LIBXML2_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${SQLITE_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/openfetion
)

add_definitions(${LIBXML2_DEFINITIONS})
add_definitions(${OPENSSL_DEFINITIONS})
add_definitions(${SQLITE_DEFINITIONS})

set(libofetion_SRC
    lib/fetion_buddylist.c
    lib/fetion_config.c
    lib/fetion_connection.c
    lib/fetion_contact.c
    lib/fetion_conversation.c
    lib/fetion_debug.c
    lib/fetion_directsms.c
    lib/fetion_group.c
    lib/fetion_history.c
    lib/fetion_list.c
    lib/fetion_login.c
    lib/fetion_message.c
    lib/fetion_share.c
    lib/fetion_sip.c
    lib/fetion_user.c
)

add_library(ofetion SHARED ${libofetion_SRC})
target_link_libraries(ofetion ${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${SQLITE_LIBRARIES})

set_target_properties(ofetion PROPERTIES
    VERSION ${LIBOFETION_SO_VERION_STRING}
    SOVERSION ${LIBOFETION_SO_CUR_VERSION}
    LINK_FLAGS ${_link_flags}
)

install(TARGETS ofetion LIBRARY DESTINATION ${LIB_INSTALL_DIR})
file(GLOB ofetion_INCLUDES "include/openfetion/*.h")
install(FILES ${ofetion_INCLUDES} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

##########################################################################

include_directories(
    ${GTK2_INCLUDE_DIRS}
    ${GDKPIXBUF_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    include_directories(${LIBNOTIFY_INCLUDE_DIR})
    add_definitions(${LIBNOTIFY_DEFINITIONS})
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)

if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    include_directories(${GSTREAMER_INCLUDE_DIR})
    add_definitions(${GSTREAMER_DEFINITIONS})
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)

if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
    include_directories(${NETWORKMANAGER_INCLUDE_DIRS})
    add_definitions(${NETWORKMANAGER_CFLAGS})
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)

set(openfetion_SRC
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
)

add_executable(openfetion ${openfetion_SRC})
target_link_libraries(openfetion ofetion ${GTK2_LIBRARIES} ${GDKPIXBUF_LIBRARIES} ${GTHREAD2_LIBRARIES})
if(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
    target_link_libraries(openfetion ${LIBNOTIFY_LIBRARIES})
endif(WITH_LIBNOTIFY AND LIBNOTIFY_FOUND)
if(WITH_GSTREAMER AND GSTREAMER_FOUND)
    target_link_libraries(openfetion ${GSTREAMER_LIBRARIES})
endif(WITH_GSTREAMER AND GSTREAMER_FOUND)
if(WITH_LIBXSS AND X11_Xscreensaver_FOUND)
    target_link_libraries(openfetion ${X11_Xscreensaver_LIB})
endif(WITH_LIBXSS AND X11_Xscreensaver_FOUND)
if(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)
    target_link_libraries(openfetion ${NETWORKMANAGER_LIBRARIES})
endif(WITH_NETWORKMANAGER AND NETWORKMANAGER_FOUND)

install(TARGETS openfetion RUNTIME DESTINATION ${BIN_INSTALL_DIR})

##########################################################################
file(GLOB openfetion_SKINIMAGES "skin/*.png" "skin/*.svg" "skin/*.gif")
file(GLOB openfetion_FACEIMAGES "skin/face_images/*.gif")
install(FILES ${openfetion_SKINIMAGES} DESTINATION ${DATA_INSTALL_DIR}/openfetion/skin)
install(FILES ${openfetion_FACEIMAGES} DESTINATION ${DATA_INSTALL_DIR}/openfetion/skin/face_images)
install(
    FILES resource/city.xml resource/province.xml resource/newmessage.wav resource/license.txt
    DESTINATION ${DATA_INSTALL_DIR}/openfetion/resource
)
install(FILES skin/fetion.svg DESTINATION ${DATA_INSTALL_DIR}/pixmaps)
install(FILES resource/openfetion.desktop DESTINATION ${DATA_INSTALL_DIR}/applications)

if(NOT WIN32)
    configure_file(lib/ofetion.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/ofetion.pc)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ofetion.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

file(GLOB POFILES "${CMAKE_CURRENT_SOURCE_DIR}/po/*.po")
gettext_create_translations("${CMAKE_CURRENT_SOURCE_DIR}/po/openfetion.pot" ALL ${POFILES})
