lib_LTLIBRARIES = libofetion.la

libofetion_la_CPPFLAGS= $(LIBXML_CFLAGS) $(LIBSSL_CFLAGS) \
						-g -Wall -I../include/openfetion -I ../. \
						-DRESOURCE_DIR=\"$(pkgdatadir)/resource/\"

libofetion_la_LDFLAGS= $(LIBXML_LIBS) $(LIBSSL_LIBS)

libofetion_la_SOURCES = fetion_buddylist.c \
						fetion_connection.c \
						fetion_conversation.c \
						fetion_directsms.c \
						fetion_list.c \
						fetion_message.c \
						fetion_sip.c \
						fetion_config.c \
						fetion_contact.c \
						fetion_debug.c \
						fetion_history.c \
						fetion_login.c \
						fetion_share.c \
						fetion_user.c \
						fetion_group.c
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ofetion.pc
