#ifndef FX_CHAT_H
#define FX_CHAT_H

extern FxChat* fx_chat_new(FxMain *fxmain , Conversation *conv);

extern void fx_chat_add_message(FxChat *fxchat , const char *msg
		, const struct tm *datetime , int issendmsg,
		int issysback);

extern void fx_chat_add_information(FxChat *fxchat , const char *msg);

extern void fx_chat_initialize(FxChat *fxchat);

extern void fx_chat_free(FxChat *fxchat);

extern void fx_chat_destroy(GtkWidget *UNUSED(widget) , gpointer data);

extern void fx_chat_send_message(FxChat *fxchat);

extern void fx_chat_nudge(FxChat *fxchat);

extern void fx_chat_nudge_in_thread(FxChat *fxchat);

extern void fx_chat_update_window(FxChat *fxchat);

#endif
