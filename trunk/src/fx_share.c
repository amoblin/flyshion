#include "fx_include.h"

FxShare *fx_share_new(FxMain *fxmain , const char *sipuri)
{
	FxShare *fxshare = (FxShare*)malloc(sizeof(FxShare));
	User *user = fxmain->user;

	DEBUG_FOOTPRINT();

	memset(fxshare , 0 , sizeof(FxShare));

	fxshare->fxmain = fxmain;
	fxshare->contact = fetion_contact_list_find_by_sipuri(user->contactList , sipuri);
	if(fxshare->contact == NULL){
		return NULL;
	}
	fxshare->share = fetion_share_new(sipuri);

	return fxshare;
}

void fx_share_initialize(FxShare *fxshare)
{
	GtkBox *vbox;
	GtkWidget *fixed;
	GtkWidget *pLabel;
	GtkWidget *okButton;
	GtkWidget *cancelButton;
	GdkPixbuf *pb;
	char text[1024];

	DEBUG_FOOTPRINT();

	fxshare->dialog = gtk_dialog_new();

	gtk_window_set_default_size(GTK_WINDOW(fxshare->dialog) , 400 , 180);
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"sendfile.png" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxshare->dialog) , pb);
	gtk_window_set_title(GTK_WINDOW(fxshare->dialog) , "飞信文件传输");

	vbox = GTK_BOX(GTK_DIALOG(fxshare->dialog)->vbox);

	fixed = gtk_fixed_new();
	gtk_box_pack_start_defaults(GTK_BOX(vbox) , fixed);

	fxshare->uLabel = gtk_label_new(NULL);
	bzero(text , sizeof(text));

	sprintf(text , "正在与好友 <b>%s</b> 传送文件 <b>阿凡达.mkv </b>"
			, fxshare->contact->nickname);

	gtk_label_set_markup(GTK_LABEL(fxshare->uLabel) , text);
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->uLabel , 20 , 20);

	pLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(pLabel) , "<b>进度：</b>");
	gtk_fixed_put(GTK_FIXED(fixed) , pLabel , 10 , 65);

	fxshare->progress = gtk_progress_bar_new();
	gtk_widget_set_usize(fxshare->progress , 300 , 30);
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->progress , 60 , 60);
	gtk_progress_set_percentage(GTK_PROGRESS(fxshare->progress) , 0.8);

	fxshare->iLabel = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(fxshare->iLabel)
			, "<span color='#838383'>正在建立连接...</span>");
	gtk_fixed_put(GTK_FIXED(fixed) , fxshare->iLabel , 60 , 100);

	okButton = gtk_button_new_with_label("确定");
	gtk_widget_set_sensitive(okButton , FALSE);
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxshare->dialog)->action_area) , okButton);

	cancelButton = gtk_button_new_with_label("取消");
	gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(fxshare->dialog)->action_area) , cancelButton);
	

	gtk_widget_show_all(fxshare->dialog);
	gtk_widget_hide(fxshare->dialog);
}

FxShare *fx_share_find_by_sipuri(FxList *fxlist , const char *sipuri)
{
	FxShare *fxshare;
	FxList *pos = fxlist;

	DEBUG_FOOTPRINT();

	while(pos != NULL){
		fxshare = (FxShare*)(pos->data);
		if(fxshare->contact == NULL){
			pos = pos->next;
			continue;
		}
		if(strcmp(fxshare->contact->sipuri , sipuri) == 0){
			return fxshare;
		}
		pos = pos->next;
	}
	return NULL;
}

void fx_share_start_transfer(FxShare *fxshare)
{
	Share *share = fxshare->share;

	DEBUG_FOOTPRINT();

	gtk_label_set_markup(GTK_LABEL(fxshare->iLabel)
			, "<span color='#838383'>对方同意了您的文件传输请求，传输进行中...</span>");

}
