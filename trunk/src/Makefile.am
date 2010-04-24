bin_PROGRAMS = openfetion

openfetion_CPPFLAGS=`xml2-config --cflags` `pkg-config --cflags gtk+-2.0 gthread-2.0` \
					-I../include -I../include/openfetion \
					-DSKIN_DIR=\"$(pkgdatadir)/skin/\" \
					-DRESOURCE_DIR=\"$(pkgdatadir)/resource/\"
					-g

openfetion_LDFLAGS=`xml2-config --libs` `pkg-config --libs gtk+-2.0 gthread-2.0` -lssl -lcrypto

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
					 fetion_sip.c \
					 fetion_user.c \
					 fx_add_group.c \
					 fx_addbuddy.c \
					 fx_app.c \
					 fx_bottom.c \
					 fx_chat.c \
					 fx_code.c \
					 fx_edit.c \
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
					 fx_util.c 
