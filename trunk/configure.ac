#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([OpenFetion], [1.8], [http://basiccoder.com/openfetion])
AC_CONFIG_SRCDIR([src/fx_main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL

# libtools
AC_PROG_RANLIB
AC_PROG_LIBTOOL

#gettext
IT_PROG_INTLTOOL([0.40.0])
AC_SUBST([GETTEXT_PACKAGE], [openfetion])
AM_GNU_GETTEXT_VERSION([0.17])
AM_GLIB_GNU_GETTEXT
AM_GNU_GETTEXT([external])
AC_DEFINE([GETTEXT_PACKAGE], [PACKAGE_TARNAME], [Define to the gettext
package name.])

AC_CHECK_LIB([notify], [notify_init])

AC_CHECK_LIB([ssl], [SSL_library_init])

AC_CHECK_LIB([crypto], [RSA_new])

AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# checks for gtk+2.0
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.16], [
			AC_DEFINE(USE_GTK, 1, [Use GTK])
			AC_SUBST(GTK_CFLAGS)
			AC_SUBST(GTK_LIBS)
		] ,	
			)
# checks for gthread-2.0
PKG_CHECK_MODULES(GTHREAD, [gthread-2.0], [
			AC_DEFINE(USE_GTHREAD, 1, [Use LIBXML])
			AC_SUBST(GTHREAD_CFLAGS)
			AC_SUBST(GTHREAD_LIBS)
		] , )

# checks for libxml2
PKG_CHECK_MODULES(LIBXML, [libxml-2.0], [
			AC_DEFINE(USE_LIBXML, 1, [Use LIBXML])
			AC_SUBST(LIBXML_CFLAGS)
			AC_SUBST(LIBXML_LIBS)
		] , )

# checks for gstreamer-0.10
PKG_CHECK_MODULES(GSTREAMER, [gstreamer-0.10], [
			AC_DEFINE(USE_GSTREAMER, 1, [Use Gstreamer for playing sound])
			AC_SUBST(GSTREAMER_CFLAGS)
			AC_SUBST(GSTREAMER_LIBS)
		] , [enable_gst="no"])

# checks for libnotify
PKG_CHECK_MODULES(LIBNOTIFY, [libnotify], [
			AC_DEFINE(USE_LIBNOTIFY, 1, [Use libnotify for notification])
			AC_SUBST(LIBNOTIFY_CFLAGS)
			AC_SUBST(LIBNOTIFY_LIBS)
		] , [enable_libnotify="no"])

PKG_CHECK_MODULES(LIBSSL, [
		openssl
])

PKG_CHECK_MODULES(GLIB2, [
        glib-2.0 >= 2.14
])

PKG_CHECK_MODULES(GOBJECT2, [
        gobject-2.0 >= 2.14
])

AC_ARG_ENABLE(debug,
        [ --enable-debug turn on debug],
        CPPFLAGS+="-DFETION_DEBUG_TRACE")

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero gethostbyname memset mkdir select socket strstr strtol])
AC_CHECK_FUNCS([inet_ntoa])

AC_CONFIG_FILES( po/Makefile.in
				Makefile
				lib/Makefile
				lib/ofetion.pc
				src/Makefile
				skin/Makefile
				skin/face_images/Makefile
				include/Makefile
				include/openfetion/Makefile
				resource/Makefile
				)
AC_OUTPUT

if test "x$enable_gst" == "xno"; then
	echo "-----------------------------------------------------------"
	echo "|                                                         |"
	echo "|            Welcome To OpenFetion v1.8                   |"
	echo "|                                                         |"
	echo "| You have not installed gstreamer-0.10 on your system    |"
	echo "| for which you can not use the sound reminder function   |"
	echo "| if you want to use this function,just install gstreamer |"
	echo "| and recompile openfetion                                |"
	echo "| NOTE:this message does not mean that you have failed    |"
	echo "|     just type \"make and\" \"make install\" to install      |"
	echo "|     openfetion without sound reminder                   |"
	echo "-----------------------------------------------------------"
fi
