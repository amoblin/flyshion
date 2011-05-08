project(cliofetion)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(TARNAME cliofetion)


# program name, version etc

set(PACKAGE_VERSION "2.2.0")
set(PACKAGE_NAME "cliofetion")
set(PACKAGE_TARNAME "${TARNAME}")
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "http://code.google.com/p/ofetion/issues/list")

find_package(PkgConfig REQUIRED)

##########################################################################
if(NOT DEFINED BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
endif(NOT DEFINED BIN_INSTALL_DIR)
if(NOT DEFINED DATA_INSTALL_DIR)
    set(DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share")
endif(NOT DEFINED DATA_INSTALL_DIR)
if(NOT DEFINED MAN_INSTALL_DIR)
    set(MAN_INSTALL_DIR "${DATA_INSTALL_DIR}/man")
endif(NOT DEFINED MAN_INSTALL_DIR)
if(NOT DEFINED RESOURCE_INSTALL_DIR)
    set(RESOURCE_INSTALL_DIR "${DATA_INSTALL_DIR}/clifetion/resource/")
endif(NOT DEFINED RESOURCE_INSTALL_DIR)
if(NOT DEFINED LOCALE_INSTALL_DIR)
    set(LOCALE_INSTALL_DIR "${DATA_INSTALL_DIR}/locale/")
endif(NOT DEFINED LOCALE_INSTALL_DIR)

add_definitions(-DRESOURCE_DIR="${RESOURCE_INSTALL_DIR}")
pkg_check_modules(OFETION REQUIRED ofetion)

include_directories(
    ${OFETION_INCLUDE_DIRS}
)

set(cliofetion_SRC
    src/cliofetion.c
)

add_executable(cliofetion ${cliofetion_SRC})
target_link_libraries(cliofetion ${OFETION_LIBRARIES})
install(TARGETS cliofetion RUNTIME DESTINATION ${BIN_INSTALL_DIR})

install(FILES cliofetion.1 DESTINATION ${MAN_INSTALL_DIR}/man1)
