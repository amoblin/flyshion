#include "fx_include.h"

struct emotion emotions[] = {

	{ N_("Smile") , ":)", 1 } ,
	{ N_("Laugh") , ":-d", 2 } ,
	{ N_("Wink") , ";)", 3 } ,
	{ N_("Surprised") , ":-o", 4 } ,
	{ N_("Tongue smile") , ":-p", 5 } ,
	{ N_("Warm smile") , "(h)", 6 } ,
	{ N_("Angry") , ":-@", 7 } ,
	{ N_("Sad") , ":(", 8 } ,
	{ N_("Cry") , ":'(", 9 } ,
	{ N_("Awkward") , ":\">", 10 } ,
	{ N_("Irony") , "^o)", 11 } ,
	{ N_("Illed") , ":&", 12 } ,
	{ N_("Gritting my teeth") , "8o|", 13 } ,
	{ N_("Tired") , "|-)", 14 } ,
	{ N_("Secrecy") , ":-#", 15 } ,
	{ N_("Googly eyes") , "8-)", 16 } ,
	{ N_("Sleeping Moon") , "(s)", 17 } ,
	{ N_("Rain") , "(st)", 18 } ,
	{ N_("Clock") , "(o)", 19 } ,
	{ N_("Red heart") , "(l)", 20 } ,
	{ N_("Break heart") , "(u)", 21 } ,
	{ N_("Face of cat") , "(@)", 22 } ,
	{ N_("Face of dog") , "(&)", 23 } ,
	{ N_("Snail") , "(sn)", 24 } ,
	{ N_("Star") , "(*)", 25 } ,
	{ N_("Sun") , "(#)", 26 } ,
	{ N_("Rainbow") , "(r)", 27 } ,
	{ N_("Hug left") , "(})", 29 } ,
	{ N_("Hug right") , "({)", 28 } ,
	{ N_("Red lips") , "(k)", 30 } ,
	{ N_("Red rose") , "(f)", 31 } ,
	{ N_("Withered rose") , "(w)", 32 } ,
	{ N_("Gift Box") , "(g)", 33 } ,
	{ N_("Birthday cake") , "(^)", 34 } ,
	{ N_("Music") , "(8)", 35 } ,
	{ N_("Bulb") , "(i)", 36 } ,
	{ N_("Idea") , "*-:)", 37 } ,
	{ N_("Coffee") , "(c)", 38 } ,
	{ N_("Umbrella") , "(um)", 39 } ,
	{ N_("Mobile Phone") , "(mp)", 40 } ,
	{ N_("Computer") , "(co)", 41 } ,
	{ N_("Disappointed") , ":-|", 42 } ,
	{ N_("Confused") , ":-/", 43 } ,
	{ N_("Worried") , ":s", 44 } ,
	{ N_("Drinks") , ")-|", 45 } ,
	{ N_("Goblet") , "(d)", 46 } ,
	{ N_("Angel") , "O:)", 47 } ,
	{ N_("Thinking") , ":-?", 48 } ,
	{ N_("Great") , "(y)", 49 } ,
	{ N_("Naughty") , "&gt;:)", 50 } ,
	{ N_("Idiot") , ":-b", 51 } ,
	{ N_("Sunglasses") , "b)", 52 } ,
	{ "" , "" , 0}

};

FxEmotion *fx_emotion_new(FxChat *fxchat)
{
	FxEmotion *fxemotion = (FxEmotion*)malloc(sizeof(FxEmotion));

	DEBUG_FOOTPRINT();

	fxemotion->fxchat = fxchat;

	return fxemotion;
}

static gboolean fx_emotion_focus_out(GtkWidget *UNUSED(widget)
		, GdkEventFocus *UNUSED(event) , gpointer data)
{
	FxEmotion *fxemotion = (FxEmotion*)data;
	gtk_widget_destroy(fxemotion->dialog);
	free(fxemotion);
	return TRUE;
}
static gboolean fx_emotion_ok_clicked(GtkWidget *UNUSED(widget)
		, GdkEventButton *UNUSED(event) , gpointer data)
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

	gtk_text_buffer_get_end_iter(buffer , &iter);

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
	gtk_widget_set_name(fxemotion->dialog , "mainwindow");
	gtk_window_set_default_size(GTK_WINDOW(fxemotion->dialog) , 300 , 180);
        gtk_window_set_skip_taskbar_hint (GTK_WINDOW(fxemotion->dialog), TRUE);
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
				gtk_widget_set_tooltip_markup(eventbox , _(emotions[k - 1].name));
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
