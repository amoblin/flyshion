C_FLAGS = -fPIC -I/usr/include/libxml2 -I~/include/sqlite3 -I. -Ibuild -g -Wall -Wextra

C_DEFINES = -Dofetion_EXPORTS -DRESOURCE_DIR=\"/usr/local/share/libofetion1/resource/\"

LINK_FLAGS = -lxml2 -lsqlite3 -lcrypto -lz -lssl

default:
	gcc $(C_FLAGS) $(C_DEFINES) $(LINK_FLAGS) fetion_buddylist.c fetion_config.c fetion_connection.c fetion_contact.c fetion_conversation.c fetion_debug.c fetion_directsms.c fetion_group.c fetion_history.c fetion_list.c fetion_login.c fetion_message.c fetion_share.c fetion_sip.c fetion_user.c -shared -o libofetion.so
