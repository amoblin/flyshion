prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${BIN_INSTALL_DIR}
libdir=${LIB_INSTALL_DIR}
includedir=${INCLUDE_INSTALL_DIR}

Name: ofetion
Description: Fetion Protocol Library
Requires: libxml-2.0 openssl
Version: ${PACKAGE_VERSION}
Libs: -L${LIB_INSTALL_DIR} -lofetion
Cflags: -I${INCLUDE_INSTALL_DIR}
