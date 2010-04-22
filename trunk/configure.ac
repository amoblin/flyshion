#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([openfetion], [1.4], [http://basiccoder.com/openfetion])
AC_CONFIG_SRCDIR([src/fx_main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

AM_PROG_CC_C_O

AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcrypto':
AC_CHECK_LIB([crypto], [main])
# FIXME: Replace `main' with a function in `-lnotify':
AC_CHECK_LIB([notify], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lssl':
AC_CHECK_LIB([ssl], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# checks for gtk+2.0
AM_PATH_GTK_2_0(,,AC_MSG_ERROR(openfetion 1.4 needs GTK+ 2.0))

# checks for libxml2
AM_PATH_XML2(,,AC_MSG_ERROR(openfetion 1.4 needs LIBXML2))

AC_ARG_ENABLE(debug,
        [ --enable-debug turn on debug],
        CPPFLAGS+="-DFETION_DEBUG_TRACE")

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gethostbyname memset mkdir select socket strstr strtol])

AC_CONFIG_FILES(
				Makefile
				src/Makefile
				skin/Makefile
				skin/face_images/Makefile
				include/Makefile
				include/openfetion/Makefile
				resource/Makefile
				)
AC_OUTPUT
