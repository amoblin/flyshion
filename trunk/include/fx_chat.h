#ifndef FX_CHAT_H
#define FX_CHAT_H

extern FxChat* fx_chat_new(FxMain* fxmain , Conversation* conv);

extern void fx_chat_add_message(FxChat* fxchat , const char* msg , const struct tm* datetime , const int issendmsg);

extern void fx_chat_add_information(FxChat* fxchat , const char* msg);

extern void fx_chat_initialize(FxChat* fxchat);

extern void fx_chat_free(FxChat* fxchat);

extern void fx_chat_destroy(GtkWidget* widget , gpointer data);

extern void fx_chat_send_message(FxChat* fxchat);

extern void fx_chat_nudge(FxChat* fxchat);

extern void fx_chat_nudge_in_thread(FxChat* fxchat);

extern void fx_chat_on_tophone_clicked(GtkWidget* widget , gpointer data);

extern void fx_chat_on_close_clicked(GtkWidget* widget , gpointer data);

extern void fx_chat_on_send_clicked(GtkWidget* widget , gpointer data);

extern void fx_chat_on_history_clicked(GtkWidget* widget , gpointer data);

extern gboolean fx_chat_on_key_pressed(GtkWidget* widget , GdkEventKey* event , gpointer data);

extern gboolean fx_chat_on_key_released(GtkWidget* widget , GdkEventKey* event , gpointer data);

extern void fx_chat_on_nudge_clicked(GtkWidget* widget , gpointer data);

/*private*/

extern gboolean fx_chat_on_text_buffer_changed(GtkTextBuffer* buffer , gpointer data);
#endif
