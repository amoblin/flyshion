#include "fx_include.h"

struct emotion emotions[] = {

	{ "微笑" , ":)", 1 } ,
	{ "大笑" , ":-d", 2 } ,
	{ "眨眼" , ";)", 3 } ,
	{ "惊讶" , ":-o", 4 } ,
	{ "吐舌笑脸" , ":-p", 5 } ,
	{ "热烈的笑脸" , "(h)", 6 } ,
	{ "生气" , ":-@", 7 } ,
	{ "悲伤" , ":(", 8 } ,
	{ "哭泣" , ":'(", 9 } ,
	{ "尴尬" , ":\"&gt;", 10 } ,
	{ "讽刺" , "^o)", 11 } ,
	{ "生病" , ":&", 12 } ,
	{ "咬牙切齿" , "8o|", 13 } ,
	{ "困" , "|-)", 14 } ,
	{ "保密" , ":-#", 15 } ,
	{ "转动的眼睛" , "8-)", 16 } ,
	{ "沉睡的月亮" , "(s)", 17 } ,
	{ "下雨" , "(st)", 18 } ,
	{ "时钟" , "(o)", 19 } ,
	{ "红心" , "(l)", 20 } ,
	{ "破碎的心" , "(u)", 21 } ,
	{ "猫脸" , "(@)", 22 } ,
	{ "狗脸" , "(&)", 23 } ,
	{ "蜗牛" , "(sn)", 24 } ,
	{ "星星" , "(*)", 25 } ,
	{ "太阳" , "(#)", 26 } ,
	{ "彩虹" , "(r)", 27 } ,
	{ "左侧拥抱" , "(})", 28 } ,
	{ "右侧拥抱" , "({)", 29 } ,
	{ "红唇" , "(k)", 30 } ,
	{ "红玫瑰" , "(f)", 31 } ,
	{ "凋谢的玫瑰" , "(w)", 32 } ,
	{ "礼品盒" , "(g)", 33 } ,
	{ "生日蛋糕" , "(^)", 34 } ,
	{ "音乐" , "-8", 35 } ,
	{ "灯泡" , "(i)", 36 } ,
	{ "想法" , "*-:)", 37 } ,
	{ "咖啡" , "(c)", 38 } ,
	{ "雨伞" , "(um)", 39 } ,
	{ "手机" , "(mp)", 40 } ,
	{ "计算机" , "(co)", 41 } ,
	{ "失望" , ":-|", 42 } ,
	{ "困惑" , ":-/", 43 } ,
	{ "担心" , ":-s", 44 } ,
	{ "饮料" , ")-|", 45 } ,
	{ "高脚杯" , "(d)", 46 } ,
	{ "天使" , "O:)", 47 } ,
	{ "深思" , ":-?", 48 } ,
	{ "太棒了" , "(y)", 49 } ,
	{ "淘气" , "&gt;:)", 50 } ,
	{ "笨蛋，呆子" , ":-b", 51 } ,
	{ "戴着太阳镜" , "b)", 52 } ,
	{ "" , "" , 0}

};

FxEmotion *fx_emotion_new(FxChat *fxchat)
{
	FxEmotion *fxemotion = (FxEmotion*)malloc(sizeof(FxEmotion));

	DEBUG_FOOTPRINT();

	fxemotion->fxchat = fxchat;
}

gboolean fx_emotion_focus_out(GtkWidget *widget 
		, GdkEventFocus *event , gpointer data)
{
	FxEmotion *fxemotion = (FxEmotion*)data;
	gtk_widget_destroy(fxemotion->dialog);
	free(fxemotion);
	return TRUE;
}
gboolean fx_emotion_ok_clicked(GtkWidget *widget
		, GdkEventButton *event , gpointer data)
{
	struct args{
		FxEmotion *fxemotion;
		int id;
	} *emotionArgs = (struct args *)data;

	FxEmotion *fxemotion = emotionArgs->fxemotion;
	FxChat *fxchat = fxemotion->fxchat;
	GtkTextBuffer* buffer;
	GtkTextIter iter;

	DEBUG_FOOTPRINT();
	
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(fxchat->send_text));

	gtk_text_buffer_get_end_iter(buffer , &iter );

	gtk_text_buffer_insert(buffer , &iter , emotions[emotionArgs->id - 1].symbol , -1);

	free(emotionArgs);
	gtk_widget_destroy(fxemotion->dialog);
	free(fxemotion);

	return FALSE;
}
void fx_emotion_initialize(FxEmotion *fxemotion , int x , int y)
{
	GtkWidget *table;
	GtkWidget *frame , *subframe;
	GtkWidget *img , *eventbox;
	GdkPixbuf *pb;
	int i = 0 , j = 0 , k = 0;
	char path[1024];
	struct args{
		FxEmotion *fxemotion;
		int id;
	} *emotionArgs;

	DEBUG_FOOTPRINT();

	fxemotion->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(fxemotion->dialog) , FALSE);
	gtk_window_set_type_hint(GTK_WINDOW(fxemotion->dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_title(GTK_WINDOW(fxemotion->dialog) , "表情盒子");
	pb = gdk_pixbuf_new_from_file(SKIN_DIR"face_images/3.gif" , NULL);
	gtk_window_set_icon(GTK_WINDOW(fxemotion->dialog) , pb);
	gtk_widget_set_name(fxemotion->dialog , "mainwindow");
	gtk_window_set_default_size(GTK_WINDOW(fxemotion->dialog) , 300 , 180);
	gtk_window_move(GTK_WINDOW(fxemotion->dialog) , x , y);

	table = gtk_table_new(10 , 6 , FALSE);
	gtk_widget_set_usize(table , 300 , 180);

	frame = gtk_frame_new(NULL);

	gtk_widget_set_events(fxemotion->dialog , GDK_ALL_EVENTS_MASK);
	g_signal_connect(fxemotion->dialog , "focus-out-event"
			, GTK_SIGNAL_FUNC(fx_emotion_focus_out) , fxemotion);
	for( j = 0 ; j < 6 ; j ++){
		for( i = 0 ; i < 10 ; i ++){
			subframe = gtk_frame_new(NULL);
			gtk_frame_set_shadow_type(GTK_FRAME(subframe) , GTK_SHADOW_ETCHED_IN);
			bzero(path , sizeof(path));
			if( k < 52 ){
				sprintf(path , SKIN_DIR"face_images/%d.gif" , (k++) + 1);
				eventbox = gtk_event_box_new();
				gtk_widget_set_tooltip_markup(eventbox , emotions[k - 1].name);
				img = gtk_image_new_from_file(path);
				gtk_container_add(GTK_CONTAINER(eventbox) , img);
				emotionArgs = (struct args *)malloc(sizeof(struct args));
				emotionArgs->fxemotion = fxemotion;
				emotionArgs->id = k;
				g_signal_connect(eventbox , "button-press-event" 
						, GTK_SIGNAL_FUNC(fx_emotion_ok_clicked) , emotionArgs);
				gtk_container_add(GTK_CONTAINER(subframe) , eventbox);
			}
			gtk_table_attach_defaults(GTK_TABLE(table) , subframe , i , i + 1 , j , j + 1);
		}
	}
	gtk_container_add(GTK_CONTAINER(frame) , table);
	gtk_container_add(GTK_CONTAINER(fxemotion->dialog) , frame);

	gtk_widget_show_all(fxemotion->dialog);
}
