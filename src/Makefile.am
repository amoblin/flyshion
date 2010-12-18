bin_PROGRAMS = openfetion

openfetion_CPPFLAGS=$(LIBXML_CFLAGS) $(GTK_CFLAGS) $(GTHREAD_CFLAGS) \
					$(GSTREAMER_CFLAGS) $(LIBNOTIFY_CFLAGS) $(LIBXSS_CFLAGS) \
					$(NETWORKMANAGER_CFLAGS) $(DBUS_CFLAGS) -g -Wall\
					-I../include -I../include/openfetion -I ../. \
					-DSKIN_DIR=\"$(pkgdatadir)/skin/\" \
					-DRESOURCE_DIR=\"$(pkgdatadir)/resource/\" \
					-DLOCALE_DIR=\"$(datadir)/locale/\"

openfetion_LDADD=   $(LIBXML_LIBS) $(GTK_LIBS) $(GTHREAD_LIBS) $(SQLITE3_LIBS)\
				    $(GSTREAMER_LIBS) $(LIBNOTIFY_LIBS) $(LIBXSS_LIBS)\
					$(NETWORKMANAGER_LIBS) $(DBUS_LIBS) ../lib/libofetion.la

openfetion_SOURCES = fx_add_group.c \
					 fx_addbuddy.c \
					 fx_app.c \
					 fx_about.c \
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
					 fx_sound.c \
					 fx_sysmsg.c \
					 fx_util.c \
					 fx_search.c \
					 fx_close.c \
					 fx_pggroup.c \
					 fx_pgprofile.c \
					 fx_smsstat.c \
					 fx_idle.c \
					 fx_conn.c \
					 fx_cli.c \
					 fx_server.c
