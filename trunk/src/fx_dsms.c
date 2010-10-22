/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "fx_include.h"

static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data);

FxConfirm* fx_confirm_new(FxMain *fxmain)
{
	FxConfirm *fxconfirm ;
	
	fxconfirm = (FxConfirm*)malloc(sizeof(FxConfirm));
	fxconfirm->fxmain = fxmain;
	return fxconfirm;
}

static void
fx_confirm_on_ok_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxConfirm *fxconfirm = (FxConfirm*)data;
	gtk_dialog_response(GTK_DIALOG(fxconfirm->dialog)
			, GTK_RESPONSE_OK);
}

static void
fx_confirm_on_cancel_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxConfirm *fxconfirm = (FxConfirm*)data;
	gtk_dialog_response(GTK_DIALOG(fxconfirm->dialog)
			, GTK_RESPONSE_CANCEL);
}

void fx_confirm_initialize(FxConfirm *fxconfirm)
{
	GtkWidget *infoLabel , *noteLabel , *codeLabel;
	GtkWidget *fixed , *okBtn , *cancelBtn;
	GdkPixbuf *pb;

	fxconfirm->dialog = gtk_dialog_new();
	gtk_widget_set_usize(fxconfirm->dialog , 420 , 220);
	gtk_window_set_resizable(GTK_WINDOW(fxconfirm->dialog) , FALSE);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"online.svg" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxconfirm->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(fxconfirm->dialog) , ("SMS directly"));

	fixed = gtk_fixed_new();
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxconfirm->dialog)->vbox)
			, fixed);
	infoLabel = gtk_label_new(_("Fetion has sent the verification code as SMS to your cell phone. Please input it."));
	codeLabel = gtk_label_new(_("Verification code"));
	fxconfirm->codeEntry = gtk_entry_new();
	gtk_widget_set_usize(fxconfirm->codeEntry , 240 , 25);
	noteLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(noteLabel)
			, _("<span color='#808080'>NOTE：A little timelag because of some network reason"
			 ",Please Wait......</span>"));
	gtk_fixed_put(GTK_FIXED(fixed) , infoLabel , 40 , 40);
	gtk_fixed_put(GTK_FIXED(fixed) , codeLabel , 40 , 74);
	gtk_fixed_put(GTK_FIXED(fixed) , fxconfirm->codeEntry , 120 , 70);
	gtk_fixed_put(GTK_FIXED(fixed) , noteLabel , 20 , 120);

	okBtn = gtk_button_new_with_label(_("OK"));
	cancelBtn = gtk_button_new_with_label(_("Cancel"));

	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxconfirm->dialog)->action_area)
			, okBtn);
	g_signal_connect(okBtn , "clicked"
			, G_CALLBACK(fx_confirm_on_ok_clicked) , fxconfirm);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxconfirm->dialog)->action_area)
			, cancelBtn);
	g_signal_connect(cancelBtn , "clicked"
			, G_CALLBACK(fx_confirm_on_cancel_clicked) , fxconfirm);

	gtk_widget_show_all(fxconfirm->dialog);
	gtk_widget_hide(fxconfirm->dialog);

}

FxDSMS* fx_dsms_new(FxMain *fxmain)
{
	FxDSMS *fxdsms = (FxDSMS*)malloc(sizeof(FxDSMS));

	fxdsms->fxmain = fxmain;
	return fxdsms;
}

static void
add_contact(gpointer data)
{
	FxDSMS *fxdsms = (FxDSMS*)data;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxdsms->chooseList));
	GdkPixbuf *pb;
	const char *number;

	GtkTreeIter iter;

	gtk_widget_show(fxdsms->chooseList);
	gtk_widget_hide(fxdsms->msgLabel);
	pb = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"online.svg",
				   22 , 22 , NULL);
	number = gtk_entry_get_text(GTK_ENTRY(fxdsms->numberEntry));
	if(strlen(number) == 0)
		return;
	gtk_tree_store_append(GTK_TREE_STORE(model) , &iter , NULL);
	gtk_tree_store_set(GTK_TREE_STORE(model) , &iter
			, PIXBUF_COL , pb , NUMBER_COL , number , -1);
	gtk_entry_set_text(GTK_ENTRY(fxdsms->numberEntry) , "");
	g_object_unref(pb);
}

static gboolean
fx_dsms_number_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	add_contact(data);
	return TRUE;
}

static void fx_dsms_add_contact(GtkWidget *UNUSED(widget)
		, GdkEventButton *UNUSED(event) , gpointer data){
	add_contact(data);
}

static GtkTreeModel* create_model()
{
	GtkTreeStore *store;
	store = gtk_tree_store_new(COL_SIZE , GDK_TYPE_PIXBUF , G_TYPE_STRING);
	return GTK_TREE_MODEL(store);
}


static void create_column(GtkWidget* tree)
{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn *col , *col0;

	renderer = gtk_cell_renderer_pixbuf_new();
	col = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", PIXBUF_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);

    renderer = gtk_cell_renderer_text_new();
    col0 = gtk_tree_view_column_new(); 
    gtk_tree_view_column_pack_start(col0, renderer , FALSE);
	gtk_tree_view_column_add_attribute(col0 , renderer, "text", NUMBER_COL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col0);

}
static void
fx_dsms_on_delete_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	struct deleteargs *args = (struct deleteargs*)data;
	FxDSMS *fxdsms = args->fxdsms;
	GtkTreeIter iter = args->iter;
	GtkWidget *treeView = fxdsms->chooseList;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));

	gtk_tree_store_remove(GTK_TREE_STORE(model) , &iter);
	if(! gtk_tree_model_get_iter_first(model , &iter)){
		gtk_widget_hide(treeView);
		gtk_widget_show(fxdsms->msgLabel);
	}
}
static gboolean 
fx_dsms_on_rightbutton_click(GtkWidget *UNUSED(tree)
		, GdkEventButton *event , gpointer data)
{
	FxDSMS *fxdsms = (FxDSMS*)data;
	GtkTreeIter iter;
	GtkTreePath* path = NULL;
	GtkTreeModel* model = NULL;
	GtkWidget *menu , *item , *img;
	struct deleteargs *args;

	if(event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		menu = gtk_menu_new();
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(fxdsms->chooseList));
		gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(fxdsms->chooseList) 
				, (gint)event->x , (gint)event->y , &path , NULL , NULL , NULL);
		if(path == NULL)
			return FALSE;

		gtk_tree_model_get_iter(GTK_TREE_MODEL(model) , &iter , path);

		gtk_tree_path_free(path);
		
		item = gtk_image_menu_item_new_with_label(_("Delete this contact"));
		img = gtk_image_new_from_file(SKIN_DIR"delete.png");

		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item) , img);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu) , item);
		args = (struct deleteargs*)malloc(sizeof(struct deleteargs));
		args->fxdsms = fxdsms;
		args->iter = iter;
		g_signal_connect(item , "activate"
				, G_CALLBACK(fx_dsms_on_delete_clicked) , args);
		gtk_widget_show_all(menu);
		gtk_menu_popup(GTK_MENU(menu) , NULL , NULL , NULL , NULL 
				, (event != NULL) ? event->button : 0 , gdk_event_get_time((GdkEvent*)event));
		return TRUE;
	}
	return FALSE;
}
static gboolean
fx_dsms_on_text_buffer_changed(GtkTextBuffer *buffer , gpointer data)
{
	FxDSMS* fxdsms = (FxDSMS*)data;
	GtkTextIter startIter , endIter;
	char text[48];
	const char *res = NULL;
	int count = gtk_text_buffer_get_char_count(buffer);
	int len;
	
	res = gtk_entry_get_text(GTK_ENTRY(fxdsms->sigEntry));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxdsms->checkBtn))){
		len = 179 - g_utf8_strlen(res , -1);
		count += (g_utf8_strlen(res , -1) + 1);
	}else{
		len = 180;
	}

	if(count <= 180)
	{
		bzero(text , sizeof(text));
		sprintf(text , _("%d/180, will split to %d") , count
				, (count % 60) ? (count / 60 + 1) : (count / 60 ));
		gtk_label_set_markup(GTK_LABEL(fxdsms->countLabel) , text);
	}
	else
	{
		gtk_text_buffer_get_start_iter(buffer , &startIter);
		gtk_text_buffer_get_iter_at_offset(buffer , &endIter , len);
		res = gtk_text_buffer_get_text(buffer , &startIter , &endIter , len);
		gtk_text_buffer_set_text(buffer , res , strlen(res));
	}
	return FALSE;
}

static void 
fx_dsms_add_information(FxDSMS* fxdsms , const char* msg)
{
	GtkTextIter iter;
	GtkTextMark *mark;
	GtkTextBuffer* buffer;

 	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->recvText));
	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert(buffer , &iter , "" , -1);
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter, msg , -1 , "grey" , "lm10" , NULL);
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);
	gtk_text_iter_set_line_offset (&iter, 0);
	mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, mark, &iter);
	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxdsms->recvText), mark);
}

static void
fx_dsms_sig_checked(GtkWidget *UNUSED(widget) , gpointer data)
{
	FxDSMS *fxdsms = (FxDSMS*)data;
	GtkTextBuffer *buffer;	

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->sendText));

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxdsms->checkBtn))){
		gtk_widget_set_sensitive(fxdsms->sigEntry , TRUE);
	}else{
		gtk_widget_set_sensitive(fxdsms->sigEntry , FALSE);
	}
	fx_dsms_on_text_buffer_changed(buffer , data);
}

static void*
fx_dsms_send_thread(void *data)
{
	FxDSMS *fxdsms = (FxDSMS*)data;
	FxMain *fxmain = fxdsms->fxmain;
	User *user = fxmain->user;
	GtkTreeView *treeView = GTK_TREE_VIEW(fxdsms->chooseList);
	GtkTreeModel *model = gtk_tree_view_get_model(treeView);
	GtkTreeIter iter;
	GtkTextIter begin , end;
	GtkTextBuffer *buffer;
	char *to , text[1024] , msg[1024] , *error;
	const char *message , *code;
	int ret , ret1 , ret2 , ret3;

	FxCode *fxcode;
	FxConfirm *fxconfirm;

	gtk_tree_model_get_iter_first(model , &iter);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->sendText));
	gtk_text_buffer_get_start_iter(buffer , &begin);
	gtk_text_buffer_get_end_iter(buffer , &end);
	message = gtk_text_buffer_get_text(buffer , &begin , &end , TRUE);
	bzero(msg , sizeof(msg));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxdsms->checkBtn))){
		sprintf(msg , "%s:%s"
				, gtk_entry_get_text(GTK_ENTRY(fxdsms->sigEntry))
				, message);
	}else{
		strcpy(msg , message);
	}
	gtk_text_buffer_delete(buffer , &begin , &end);

	do
	{
		gtk_tree_model_get(model , &iter
				, NUMBER_COL , &to , -1);
		ret = fetion_directsms_send_sms(user , to , msg);
		if(ret == SEND_SMS_SUCCESS){
			bzero(text , sizeof(text));
			sprintf(text , _("Mesage has been send to %s successfully.") , to);
			gdk_threads_enter();
			fx_dsms_add_information(fxdsms , text);
			gdk_threads_leave();
		}else if(ret == SEND_SMS_OTHER_ERROR){
			bzero(text , sizeof(text));
			sprintf(text , _("Mesage didn't send to %s. Please check the phone number of the contact.") , to);
			gdk_threads_enter();
			fx_dsms_add_information(fxdsms , text);
			gdk_threads_leave();
		}else{
picreload:
			generate_pic_code(user);
			fxcode = fx_code_new(fxmain , user->verification->text
					, user->verification->tips , CODE_NOT_ERROR );
			gdk_threads_enter();
			fx_code_initialize(fxcode);
			ret = gtk_dialog_run(GTK_DIALOG(fxcode->dialog));
			if(ret == GTK_RESPONSE_CANCEL){
				bzero(text , sizeof(text));
				strcpy(text , _("Send message failed"));
				fx_dsms_add_information(fxdsms , text);
				gtk_widget_destroy(fxcode->dialog);
				gdk_threads_leave();
				fetion_verification_free(user->verification);
				user->verification = NULL;
				break;
			}
			if(ret == GTK_RESPONSE_OK){
				code = gtk_entry_get_text(GTK_ENTRY(fxcode->codeentry));
				ret1 = fetion_directsms_send_subscribe(user
						, code , &error);
				gtk_widget_destroy(fxcode->dialog);
				if(ret1 == PIC_ERROR){
					debug_info("%s" , error);
					free(error);
					gdk_threads_leave();
					goto picreload;
				}else{
					fxconfirm = fx_confirm_new(fxmain);
					fx_confirm_initialize(fxconfirm);
					ret2 = gtk_dialog_run(GTK_DIALOG(fxconfirm->dialog));
					if(ret2 == GTK_RESPONSE_CANCEL){
						bzero(text , sizeof(text));
						strcpy(text , _("Message sent failed"));
						fx_dsms_add_information(fxdsms , text);
						gtk_widget_destroy(fxconfirm->dialog);
						gdk_threads_leave();
						fetion_verification_free(user->verification);
						user->verification = NULL;
						break;
					}else{
						bzero(user->verification->guid
								, strlen(user->verification->guid) + 1);	
						strcpy(user->verification->guid , user->mobileno);
						code = gtk_entry_get_text(GTK_ENTRY(fxconfirm->codeEntry));
						ret3 = fetion_directsms_send_option(user , code);
						gtk_widget_destroy(fxconfirm->dialog);
						if(ret3 == DSMS_OPTION_SUCCESS){
							gdk_threads_leave();
						}else{
							gdk_threads_leave();
							goto picreload;
						}
					}
				}
			}
		}
		bzero(text , sizeof(text));
		fx_dsms_add_information(fxdsms , text);
		free(to);
	}while(gtk_tree_model_iter_next(model , &iter));

	return NULL;
}

static void
fx_dsms_send_message(FxDSMS *fxdsms)
{
	GtkTextIter iter , begin , end;
	GtkTextBuffer *buffer , *buffer1;
	GtkTextMark *mark;
	GtkTreeModel *model;
	GtkTreeView *treeView;
	GtkTreeIter posIter;
	FxMain* fxmain = fxdsms->fxmain;
	User* user = fxmain->user;
	char text[200] = { 0 };
	char time[30] = { 0 };
	const char *message;
	char msg[1024];
	struct tm *datetime;

	treeView = GTK_TREE_VIEW(fxdsms->chooseList);
	model = gtk_tree_view_get_model(treeView);
	if(! gtk_tree_model_get_iter_first(model , &posIter))
	{
		fx_util_popup_warning(fxmain , _("Please input a contact"));
		return;
	}

	datetime = get_currenttime();
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->recvText));
	buffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->sendText));
	gtk_text_buffer_get_start_iter(buffer1 , &begin);
	gtk_text_buffer_get_end_iter(buffer1 , &end);

	message = gtk_text_buffer_get_text(buffer1 , &begin , &end , TRUE);
	if(strlen(message) == 0)
		return;
	bzero(msg , sizeof(msg));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fxdsms->checkBtn))){
		sprintf(msg , "%s:%s"
				, gtk_entry_get_text(GTK_ENTRY(fxdsms->sigEntry))
				, message);
	}else{
		strcpy(msg , message);
	}
	strftime(time , sizeof(time) , "%H:%M:%S" , datetime);

	sprintf(text , _("%s said (%s):\n") , user->nickname , time);
	gtk_text_buffer_get_end_iter(buffer , &iter );
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter , text , -1 , "blue" , NULL);
	gtk_text_buffer_insert_with_tags_by_name(buffer
					, &iter, msg , strlen(msg) , "lm10" , NULL);
	gtk_text_buffer_insert(buffer , &iter , "\n" , -1);

	gtk_text_iter_set_line_offset (&iter, 0);
	mark = gtk_text_buffer_get_mark (buffer, "scroll");
	gtk_text_buffer_move_mark (buffer, mark, &iter);
	gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW(fxdsms->recvText), mark);

	g_thread_create(fx_dsms_send_thread , fxdsms , FALSE , NULL);
}

static gboolean
fx_dsms_on_key_pressed(GtkWidget *UNUSED(widget) , GdkEventKey *event , gpointer data)
{
	FxDSMS *fxdsms = NULL;
	Config *config = NULL;

	if(event->keyval == GDK_Return || event->keyval == GDK_ISO_Enter || event->keyval == GDK_KP_Enter)
	{
		fxdsms = (FxDSMS*)data;
		config = fxdsms->fxmain->user->config;
		if(config->sendMode == SEND_MODE_ENTER)
		{
			if(event->state & GDK_CONTROL_MASK || event->state & GDK_SHIFT_MASK){
				return FALSE;
			}else{
 				if (gtk_im_context_filter_keypress (GTK_TEXT_VIEW(fxdsms->sendText)->im_context, event)) {
					GTK_TEXT_VIEW (fxdsms->sendText)->need_im_reset = TRUE;
 					return TRUE;
 				}
				fx_dsms_send_message(fxdsms);
				return TRUE;
			}
		}
		else
		{
			if(event->state & GDK_CONTROL_MASK)	{
				fx_dsms_send_message(fxdsms);
				return TRUE;
			}else{
				return FALSE;
			}
		}
	}
	return FALSE;
}

static void
fx_dsms_on_close_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	GtkWidget *dialog = GTK_WIDGET(data);
	gtk_widget_destroy(dialog);
}

static void
fx_dsms_on_send_clicked(GtkWidget *UNUSED(widget) , gpointer data)
{
	fx_dsms_send_message((FxDSMS*)data);
}

void fx_dsms_initialize(FxDSMS *fxdsms)
{
	FxMain *fxmain = fxdsms->fxmain;	
	User *user = fxmain->user;
	Config *config = user->config;
	char path[256];

	GtkWidget *infoLabel;
	GtkWidget *hbox , *lvbox , *rvbox , *rhbox , *okBtn , *cancelBtn;
	GtkWidget *recvScroll , *sendScroll;
	GtkTextBuffer *recvBuffer , *sendBuffer;
	GtkTextIter recvIter ;
	GdkPixbuf *pb;
	GtkWidget *portrait , *portraitFrame;
	GtkWidget *addImage , *addEventBox;
	GtkBox *abox;

	fxdsms->dialog = gtk_dialog_new();
	gtk_widget_set_usize(fxdsms->dialog , 650 , 490);
	gtk_window_set_resizable(GTK_WINDOW(fxdsms->dialog) , FALSE);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"directsms.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxdsms->dialog) , pb);
	g_object_unref(pb);
	gtk_window_set_title(GTK_WINDOW(fxdsms->dialog) , _("SMS directly"));
	g_signal_connect(fxdsms->dialog , "key-press-event"
			, G_CALLBACK(key_press_func) , fxdsms);

	infoLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(infoLabel)
			, _(" You will send SMS to contact directly, cost <span color='red'>0.1 RMB/SMS.</span>"));
	gtk_misc_set_alignment(GTK_MISC(infoLabel) , 0 , 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fxdsms->dialog)->vbox) , infoLabel , FALSE , TRUE , 10);

	hbox = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxdsms->dialog)->vbox)
			, hbox);
	lvbox = gtk_vbox_new(FALSE , 0);
	rvbox = gtk_vbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(hbox) , lvbox , TRUE , TRUE , 10);
	gtk_box_pack_start(GTK_BOX(hbox) , rvbox , FALSE , FALSE , 5);
	/*left box*/
	recvScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_box_pack_start(GTK_BOX(lvbox) , recvScroll , TRUE , TRUE , 5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(recvScroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(recvScroll)
									  , GTK_SHADOW_ETCHED_IN);
	fxdsms->recvText = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxdsms->recvText) , GTK_WRAP_CHAR);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(fxdsms->recvText) , FALSE);
	gtk_container_add(GTK_CONTAINER(recvScroll) , fxdsms->recvText);

	recvBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->recvText));
	gtk_text_buffer_create_tag(recvBuffer , "blue" , "foreground" , "#639900" , NULL);
	gtk_text_buffer_create_tag(recvBuffer , "grey" , "foreground" , "#808080" , NULL);
	gtk_text_buffer_create_tag(recvBuffer , "lm10" , "left_margin" , 10 , NULL);
	gtk_text_buffer_get_end_iter(recvBuffer , &recvIter);
	gtk_text_buffer_create_mark(recvBuffer , "scroll" , &recvIter , FALSE);

	sendScroll = gtk_scrolled_window_new(NULL , NULL);
	gtk_widget_set_usize(sendScroll , 0 , 120);
	gtk_box_pack_start(GTK_BOX(lvbox) , sendScroll , FALSE , TRUE , 5);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sendScroll)
								 , GTK_POLICY_NEVER
								 , GTK_POLICY_AUTOMATIC);

	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sendScroll)
									  , GTK_SHADOW_ETCHED_OUT);
	fxdsms->sendText = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(fxdsms->sendText) , GTK_WRAP_CHAR);
	gtk_container_add(GTK_CONTAINER(sendScroll) , fxdsms->sendText);
	sendBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxdsms->sendText));
	g_signal_connect(sendBuffer , "changed"
			, G_CALLBACK(fx_dsms_on_text_buffer_changed) , fxdsms);
	g_signal_connect(fxdsms->sendText , "key_press_event"
			, G_CALLBACK(fx_dsms_on_key_pressed) , fxdsms);
	/*right box*/
	rhbox = gtk_hbox_new(FALSE , 0);
	gtk_box_pack_start(GTK_BOX(rvbox) , rhbox , FALSE , FALSE , 5);
	fxdsms->numberEntry = gtk_entry_new();
	gtk_widget_set_usize(fxdsms->numberEntry , 140 , 20);
	gtk_box_pack_start(GTK_BOX(rhbox) , fxdsms->numberEntry , FALSE , FALSE , 0);
	g_signal_connect(fxdsms->numberEntry , "activate"
			, G_CALLBACK(fx_dsms_number_clicked) , fxdsms);

	addEventBox = gtk_event_box_new();
	addImage = gtk_image_new_from_file(SKIN_DIR"add.png");
	gtk_container_add(GTK_CONTAINER(addEventBox) , addImage);
	gtk_box_pack_start(GTK_BOX(rhbox) , addEventBox , FALSE , FALSE , 2);
	gtk_widget_set_events(addEventBox
						, GDK_MOTION_NOTIFY
						| GDK_BUTTON_PRESS
						| GDK_BUTTON_RELEASE
						| GDK_ENTER_NOTIFY
						| GDK_LEAVE_NOTIFY);
	g_signal_connect(G_OBJECT(addEventBox)
					 , "button_press_event"
					 , GTK_SIGNAL_FUNC(fx_dsms_add_contact)
					 , fxdsms);

	GtkTreeModel *model = create_model();
	fxdsms->chooseList = gtk_tree_view_new_with_model(model);
	create_column(fxdsms->chooseList);
	gtk_tree_view_set_show_expanders(GTK_TREE_VIEW(fxdsms->chooseList) , FALSE);
	gtk_tree_view_set_level_indentation(GTK_TREE_VIEW(fxdsms->chooseList) , -30);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(fxdsms->chooseList) , FALSE);
 	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (fxdsms->chooseList), TRUE);
	gtk_box_pack_start(GTK_BOX(rvbox) , fxdsms->chooseList , TRUE , TRUE , 0);
	g_signal_connect(fxdsms->chooseList
				   , "button_press_event"
				   , G_CALLBACK(fx_dsms_on_rightbutton_click)
				   , fxdsms);
	
	fxdsms->msgLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(fxdsms->msgLabel) , _("<b>No Contact</b>\n\n"
	"<span color='#808080'>Please input Phone Number\nin the input box above\n"
	"3 contacts allowed at most</span>"));
	gtk_box_pack_start(GTK_BOX(rvbox) , fxdsms->msgLabel , TRUE , FALSE , 0);

	portraitFrame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(portraitFrame) , GTK_SHADOW_ETCHED_IN);
	gtk_widget_set_usize(portraitFrame , 160 , 160);
	bzero(path , sizeof(path));
	sprintf(path , "%s/%s.jpg" , config->iconPath , user->sId);
	pb = gdk_pixbuf_new_from_file_at_size(path , 140 , 140 , NULL);
	portrait = gtk_image_new_from_pixbuf(pb);
	g_object_unref(pb);
	gtk_container_add(GTK_CONTAINER(portraitFrame) , portrait);
	gtk_box_pack_start(GTK_BOX(rvbox) , portraitFrame , FALSE , FALSE , 2);
	/* bottom box */
	abox = GTK_BOX(gtk_hbox_new(FALSE , 2));
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxdsms->dialog)->action_area) , GTK_WIDGET(abox));
	fxdsms->countLabel = gtk_label_new(_("0/180, will split to 0"));

	gtk_misc_set_alignment(GTK_MISC(fxdsms->countLabel) , 0 , 0.5);
	gtk_box_pack_start(abox , fxdsms->countLabel , TRUE , FALSE , 0);
	fxdsms->checkBtn = gtk_check_button_new_with_label(_("Sign"));
	g_signal_connect(fxdsms->checkBtn , "toggled"
			, G_CALLBACK(fx_dsms_sig_checked) , fxdsms);
	gtk_box_pack_start(abox , fxdsms->checkBtn , FALSE , FALSE , 0);
	fxdsms->sigEntry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(fxdsms->sigEntry) , user->nickname);
	gtk_widget_set_sensitive(fxdsms->sigEntry , FALSE);
	gtk_widget_set_usize(fxdsms->sigEntry , 100 , 0);
	gtk_box_pack_start(abox , fxdsms->sigEntry , FALSE , FALSE , 0);

	okBtn = gtk_button_new_with_label(_("Send"));
	gtk_widget_set_usize(okBtn , 100 , 0);
	g_signal_connect(okBtn , "clicked"
			, G_CALLBACK(fx_dsms_on_send_clicked) , fxdsms);
	cancelBtn = gtk_button_new_with_label(_("Close"));
	g_signal_connect(cancelBtn , "clicked"
			, G_CALLBACK(fx_dsms_on_close_clicked) , fxdsms->dialog);
	gtk_widget_set_usize(cancelBtn , 100 , 0);

	gtk_box_pack_start(abox	, cancelBtn , FALSE , FALSE , 0);
	gtk_box_pack_start(abox	, okBtn , FALSE , FALSE , 0);

	gtk_widget_show_all(fxdsms->dialog);
	gtk_widget_hide(fxdsms->chooseList);
}

static gboolean key_press_func(GtkWidget *UNUSED(widget) , GdkEventKey *event
		, gpointer data)
{
	FxDSMS *fxdsms ;
	if(event->keyval == GDK_w){
		if(event->state & GDK_CONTROL_MASK){
			fxdsms = (FxDSMS*)data;
			gtk_widget_destroy(fxdsms->dialog);
			return TRUE;
		}else{
			return FALSE;
		}
	}

	return FALSE;
}
