project(libofetion)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(TARNAME libofetion)


# program name, version etc
set(LIBOFETION_API_VERSION "1.1.0")

set(LIBOFETION_ABI_VERSION "1.0.0")
set(LIBOFETION_ABI_MAJOR_VERSION "1")

set(PACKAGE_VERSION "2.1.0")
set(PACKAGE_NAME "${TARNAME}")
set(PACKAGE_TARNAME "${TARNAME}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "http://code.google.com/p/ofetion/issues/list")

find_package(PkgConfig REQUIRED)

if(NOT DEFINED BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
endif(NOT DEFINED BIN_INSTALL_DIR)
if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif(NOT DEFINED LIB_INSTALL_DIR)
if(NOT DEFINED DATA_INSTALL_DIR)
    set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share")
endif(NOT DEFINED DATA_INSTALL_DIR)
if(NOT DEFINED INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/${PACKAGE_TARNAME}")
endif(NOT DEFINED INCLUDE_INSTALL_DIR)
if(NOT DEFINED MAN_INSTALL_DIR)
    set(MAN_INSTALL_DIR "${DATA_INSTALL_DIR}/man")
endif(NOT DEFINED MAN_INSTALL_DIR)
if(NOT DEFINED RESOURCE_INSTALL_DIR)
    set(RESOURCE_INSTALL_DIR "${DATA_INSTALL_DIR}/libofetion${LIBOFETION_ABI_MAJOR_VERSION}/resource/")
endif(NOT DEFINED RESOURCE_INSTALL_DIR)
if(NOT DEFINED LOCALE_INSTALL_DIR)
    set(LOCALE_INSTALL_DIR "${DATA_INSTALL_DIR}/locale/")
endif(NOT DEFINED LOCALE_INSTALL_DIR)

add_definitions(-DRESOURCE_DIR="${RESOURCE_INSTALL_DIR}")
install(
	FILES resource/city.xml resource/province.xml
	DESTINATION ${RESOURCE_INSTALL_DIR}
)
pkg_check_modules(LIBXML2 REQUIRED libxml-2.0)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
# gnu ld version script
set(_link_flags " ")
if(CMAKE_COMPILER_IS_GNUCXX)
    include(TestCXXAcceptsFlag)
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/./libofetion.map")
    check_cxx_accepts_flag("-Wl,--version-script,${_version_script}" LD_ACCEPTS_VERSION_SCRIPT)
    if(LD_ACCEPTS_VERSION_SCRIPT)
        set(_link_flags "${_link_flags} -Wl,--version-script,'${_version_script}'")
    endif(LD_ACCEPTS_VERSION_SCRIPT)
endif(CMAKE_COMPILER_IS_GNUCXX)


##########################################################################
add_definitions(-Wall -Wextra)
##########################################################################

include_directories(
    ${LIBXML2_INCLUDE_DIRS} 
    ${OPENSSL_INCLUDE_DIRS} 
    ${SQLITE3_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)


set(libofetion_SRC
    ./fetion_buddylist.c
    ./fetion_config.c
    ./fetion_connection.c
    ./fetion_contact.c
    ./fetion_conversation.c
    ./fetion_debug.c
    ./fetion_directsms.c
    ./fetion_group.c
    ./fetion_history.c
    ./fetion_list.c
    ./fetion_login.c
    ./fetion_message.c
    ./fetion_share.c
    ./fetion_sip.c
    ./fetion_user.c
)

add_library(ofetion SHARED ${libofetion_SRC})
target_link_libraries(ofetion ${LIBXML2_LIBRARIES} ${OPENSSL_LIBRARIES} ${SQLITE3_LIBRARIES})

set_target_properties(ofetion PROPERTIES
    VERSION ${LIBOFETION_ABI_VERSION}
    SOVERSION ${LIBOFETION_ABI_MAJOR_VERSION}
    LINK_FLAGS ${_link_flags}
)

add_library(ofetion-static STATIC ${libofetion_SRC})
SET_TARGET_PROPERTIES(ofetion-static PROPERTIES OUTPUT_NAME "ofetion")
SET_TARGET_PROPERTIES(ofetion-static PROPERTIES PREFIX "lib")

install(TARGETS ofetion ofetion-static
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
file(GLOB ofetion_INCLUDES "./*.h")
install(FILES ${ofetion_INCLUDES} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

##########################################################################

configure_file(./ofetion.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/ofetion.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ofetion.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
