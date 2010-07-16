bin_PROGRAMS = openfetion

openfetion_CPPFLAGS= \
		     @LIBXML_CFLAGS@ \
		     @GLIB2_CFLAGS@ \
		     @GOBJECT2_CFLAGS@ \
		     @GTK_CFLAGS@ \
		     @GSTREAMER_CFLAGS@ \
		     @LIBNOTIFY_CFLAGS@ \
		     @SSL_CFLAGS@ \
		     -g -Wall -O2\
		     -I../include -I../include/openfetion -I ../. \
		     -DSKIN_DIR=\"$(pkgdatadir)/skin/\" \
		     -DRESOURCE_DIR=\"$(pkgdatadir)/resource/\"

openfetion_LDADD= \
		  @LIBXML_LIBS@ \
		  @GLIB2_LIBS@ \
		  @GOBJECT2_LIBS@ \
		  @GTK_LIBS@ \
		  @GSTREAMER_LIBS@ \
		  @LIBNOTIFY_LIBS@ \
		  @SSL_LIBS@

openfetion_SOURCES = fetion_buddylist.c \
					 fetion_config.c \
					 fetion_connection.c \
					 fetion_contact.c \
					 fetion_conversation.c \
					 fetion_debug.c \
					 fetion_history.c \
					 fetion_list.c \
					 fetion_login.c \
					 fetion_message.c \
					 fetion_share.c \
					 fetion_directsms.c \
					 fetion_sip.c \
					 fetion_user.c \
					 fx_add_group.c \
					 fx_addbuddy.c \
					 fx_app.c \
					 fx_bottom.c \
					 fx_chat.c \
					 fx_code.c \
					 fx_edit.c \
					 fx_dsms.c \
					 fx_gedit.c \
					 fx_head.c \
					 fx_history.c \
					 fx_proxy.c \
					 fx_login.c \
					 fx_lookup.c \
					 fx_lookupres.c \
					 fx_main.c \
					 fx_many.c \
					 fx_myself.c \
					 fx_profile.c \
					 fx_set.c \
					 fx_tree.c \
					 fx_emotion.c \
					 fx_share.c \
					 fx_sound.c \
					 fx_sysmsg.c \
					 fx_util.c \
					 fx_close.c 
