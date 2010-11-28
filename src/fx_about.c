#include <fx_include.h>

#define BUFLEN 4096

static void
follow_if_link (GtkWidget   *text_view, 
                GtkTextIter *iter)
{
	GSList *tags = NULL, *tagp = NULL;

	tags = gtk_text_iter_get_tags (iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		gchar *url = (gchar*)(g_object_get_data (G_OBJECT (tag), "url"));

		if (url)
		{
			if(fork() == 0)
				execlp("xdg-open" , "xdg-open" , url , (char**)NULL);
			break;
		}
	}

	if (tags) 
		g_slist_free (tags);
}

static gboolean event_after (GtkWidget *text_view,
             		GdkEvent  *ev)
{
	GtkTextIter start, end, iter;
	GtkTextBuffer *buffer;
	GdkEventButton *event;
	gint x, y;

	if (ev->type != GDK_BUTTON_RELEASE)
		return FALSE;

	event = (GdkEventButton *)ev;

	if (event->button != 1)
		return FALSE;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	if (gtk_text_iter_get_offset (&start) != gtk_text_iter_get_offset (&end))
		return FALSE;

	gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), 
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

	gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (text_view), &iter, x, y);

	follow_if_link (text_view, &iter);
	
	return FALSE;
}
static void 
insert_link (GtkTextBuffer *buffer, 
             GtkTextIter   *iter, 
             gchar         *text)
{
  GtkTextTag *tag;
  
  tag = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "foreground", "blue", 
                                    "underline", PANGO_UNDERLINE_SINGLE, 
                                    NULL);
  g_object_set_data (G_OBJECT (tag), "url", text);
  gtk_text_buffer_insert_with_tags (buffer, iter, text, -1, tag, NULL);
}

static gboolean hovering_over_link = FALSE;
static GdkCursor *hand_cursor = NULL;
static GdkCursor *regular_cursor = NULL;

static void
set_cursor_if_appropriate (GtkTextView    *text_view,
                           gint            x,
                           gint            y)
{
	GSList *tags = NULL, *tagp = NULL;
	GtkTextIter iter;
	gboolean hovering = FALSE;

	gtk_text_view_get_iter_at_location (text_view, &iter, x, y);
  
	tags = gtk_text_iter_get_tags (&iter);
	for (tagp = tags;  tagp != NULL;  tagp = tagp->next)
	{
		GtkTextTag *tag = tagp->data;
		gchar *url = (gchar*)(g_object_get_data (G_OBJECT (tag), "url"));


		if (url) 
		{
			hovering = TRUE;
			break;
		}
	}

	if (hovering != hovering_over_link)
    {
		hovering_over_link = hovering;

		if (hovering_over_link)
			gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), hand_cursor);
		else
			gdk_window_set_cursor (gtk_text_view_get_window (text_view, GTK_TEXT_WINDOW_TEXT), regular_cursor);
	}

	if (tags) 
		g_slist_free (tags);
}

static gboolean
motion_notify_event (GtkWidget      *text_view,
                     GdkEventMotion *event)
{
	gint x, y;

	gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (text_view), 
                                         GTK_TEXT_WINDOW_WIDGET,
                                         event->x, event->y, &x, &y);

	set_cursor_if_appropriate (GTK_TEXT_VIEW (text_view), x, y);

	gdk_window_get_pointer (text_view->window, NULL, NULL, NULL);
	return FALSE;
}

static void create_intro(GtkTextView *view)
{
	GtkTextTag    *tag;
	GtkTextTag    *tag1;
	GtkTextIter    iter;
	const char title[] = "Openfetion 2.0.7\n\n";
	const char intro[] = "OpenFetion is a Fetion client for linux based on GTK+2.0,"
						"implementing Fetion Protocol Version 4.\n\n"
						"It supports most features of Fetion. \n\n"
						"What's more, it's lightweight and efficient with intuitive interface.\n\n"
						"Project homepage: ";

	gtk_text_view_set_editable(view,FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
	g_signal_connect (view, "motion-notify-event", 
			G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect(view, "event-after", 
                     G_CALLBACK (event_after), NULL);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(view);

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "size", 15 * PANGO_SCALE, 
									"justification", GTK_JUSTIFY_CENTER,
									"weight", PANGO_WEIGHT_BOLD,
                                    NULL);

	tag1 = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "left-margin", 25, 
                                    "right-margin", 25, 
                                    NULL);


	gtk_text_buffer_get_start_iter(buffer, &iter);
  
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	title, -1, tag, NULL);
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);
	insert_link(buffer, &iter, "http://code.google.com/p/ofetion/");
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"\nPage on author's blog: ", -1, tag1, NULL);
	insert_link(buffer, &iter, "http://basiccoder.com/openfetion");
}

static void create_author(GtkTextView *view)
{
	GtkTextTag    *tag;
	GtkTextTag    *tag1;
	GtkTextIter    iter;
	const char title[] = "Levin\n\n";
	const char *intro = "Graduate student at Beijing University of Post & Telecommunication."
						"Love Linux,Love OpenSource.Started this project in order to help "
						"linux users including myself to get a better way to use fetion.\n\n"
						"Blog:";

	gtk_text_view_set_editable(view,FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
	g_signal_connect (view, "motion-notify-event", 
			G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect(view, "event-after", 
                     G_CALLBACK (event_after), NULL);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(view);

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "size", 15 * PANGO_SCALE, 
									"justification", GTK_JUSTIFY_CENTER,
									"weight", PANGO_WEIGHT_BOLD,
                                    NULL);

	tag1 = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "left-margin", 25, 
                                    "right-margin", 25, 
                                    NULL);


	gtk_text_buffer_get_start_iter(buffer, &iter);
  
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	title, -1, tag, NULL);
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);
	insert_link(buffer, &iter, "http://basiccoder.com");
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"\nE-mail: levin108@gmail.com\n", -1, tag1, NULL);
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"Twitter: ", -1, tag1, NULL);
	insert_link(buffer, &iter, "http://twitter.com/levin108");
}

static void create_gpl(GtkTextView *view)
{
	GtkTextTag    *tag;
	GtkTextIter    iter;
	FILE          *fd;
	char           buf[BUFLEN];

	fd = fopen(RESOURCE_DIR"license.txt", "r");
	if(!fd){
		perror("read license.txt failed");
		return;
	}

	gtk_text_view_set_editable(view,FALSE);
	g_signal_connect (view, "motion-notify-event", 
			G_CALLBACK (motion_notify_event), NULL);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(view);

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "left-margin", 5, 
                                    "right-margin", 5, 
                                    NULL);

	gtk_text_buffer_get_start_iter(buffer, &iter);
  
	memset(buf, 0, sizeof(buf));
	while(fgets(buf, sizeof(buf), fd)){
		gtk_text_buffer_insert_with_tags(buffer, &iter,
		   		buf, -1, tag, NULL);
		memset(buf, 0, sizeof(buf));
	}
	fclose(fd);
}

static void create_contri(GtkTextView *view)
{
	GtkTextTag    *tag;
	GtkTextTag    *tag1;
	GtkTextTag    *tag2;
	GtkTextIter    iter;
	const char title[] = "The Main Contributors\n\n";
	const char *intro;

	gtk_text_view_set_editable(view,FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
	g_signal_connect (view, "motion-notify-event", 
			G_CALLBACK (motion_notify_event), NULL);
	g_signal_connect(view, "event-after", 
                     G_CALLBACK (event_after), NULL);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(view);

	tag = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "size", 15 * PANGO_SCALE, 
									"justification", GTK_JUSTIFY_CENTER,
									"weight", PANGO_WEIGHT_BOLD,
                                    NULL);

	tag1 = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "left-margin", 25, 
                                    "right-margin", 25, 
                                    NULL);

	tag2 = gtk_text_buffer_create_tag (buffer, NULL, 
                                    "left-margin", 25, 
									"weight", PANGO_WEIGHT_BOLD,
                                    NULL);


	gtk_text_buffer_get_start_iter(buffer, &iter);
  
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	title, -1, tag, NULL);

	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"happyaron: ", -1, tag2, NULL);
	intro = "Make dep package for debian/ubuntu users,"
			"maintainer of the PPA of openfetion.\nBlog:";
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);
	insert_link(buffer, &iter, "http://blogs.gnome.org/happyaron/");

	
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"\n\nwzssyqa: ", -1, tag2, NULL);
	intro = "Main translater of openfetion,accomplish"
		" the internationalisation(i18n) of openfetion";
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);

	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"\n\nxhacker: ", -1, tag2, NULL);
	intro = "Contributed to the UI development,and fixed some "
			"bugs of the UI.\nFollow him:";
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);
	insert_link(buffer, &iter, "http://twitter.com/xhacker");

	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	"\n\nriku: ", -1, tag2, NULL);
	intro = "A good designer,designed a logo and some icons for "
			"openfetion,and also contributed to the UI design.\nFollow him:";
	gtk_text_buffer_insert_with_tags(buffer, &iter,
		   	intro, -1, tag1, NULL);
	insert_link(buffer, &iter, "http://twitter.com/riku");
}

void close_about(GtkWidget *widget, gpointer data)
{
	GtkWidget *window = GTK_WIDGET(data);
	gtk_widget_destroy(window);
}

void show_about()
{
	GtkWidget *window;
	GtkWidget *image;
	GdkPixbuf *pixbuf;
	GtkWidget *vbox;
	GtkWidget *notebook;
	GtkWidget *scroll;
	GtkWidget *textview;
	GtkWidget *tablabel;
	GtkWidget *action_area;
	GtkWidget *button;

	hand_cursor = gdk_cursor_new (GDK_HAND2);
	regular_cursor = gdk_cursor_new (GDK_XTERM);
      

	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg", 40, 40, NULL);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), pixbuf);
	g_object_unref(pixbuf);
	gtk_window_set_title(GTK_WINDOW(window), _("About OpenFetion"));
	gtk_widget_set_usize(window, 500, 400);

	vbox = gtk_vbox_new(FALSE, 5);
	pixbuf = gdk_pixbuf_new_from_file_at_size(SKIN_DIR"fetion.svg", 98, 98, NULL);
	image = gtk_image_new_from_pixbuf(pixbuf);
	g_object_unref(pixbuf);
	gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);

	notebook = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 5);

	tablabel = gtk_label_new(_("Introduction"));
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
								 GTK_POLICY_NEVER,
								 GTK_POLICY_AUTOMATIC);
	textview = gtk_text_view_new();
	create_intro(GTK_TEXT_VIEW(textview));
	gtk_container_add(GTK_CONTAINER(scroll), textview);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			scroll, tablabel);

	tablabel = gtk_label_new(_("About the author"));
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
								 GTK_POLICY_NEVER,
								 GTK_POLICY_AUTOMATIC);
	textview = gtk_text_view_new();
	create_author(GTK_TEXT_VIEW(textview));
	gtk_container_add(GTK_CONTAINER(scroll), textview);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			scroll, tablabel);

	tablabel = gtk_label_new(_("Contributor"));
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
								 GTK_POLICY_NEVER,
								 GTK_POLICY_AUTOMATIC);
	textview = gtk_text_view_new();
	create_contri(GTK_TEXT_VIEW(textview));
	gtk_container_add(GTK_CONTAINER(scroll), textview);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			scroll, tablabel);
	
	tablabel = gtk_label_new(_("License"));
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
								 GTK_POLICY_AUTOMATIC,
								 GTK_POLICY_AUTOMATIC);
	textview = gtk_text_view_new();
	create_gpl(GTK_TEXT_VIEW(textview));
	gtk_container_add(GTK_CONTAINER(scroll), textview);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			scroll, tablabel);

	action_area = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox), action_area, FALSE, FALSE, 5);
	button = gtk_button_new_with_label(_("Close"));
	gtk_widget_set_usize(button, 90, 0);
	gtk_box_pack_end(GTK_BOX(action_area), button, FALSE, FALSE, 5);
	g_signal_connect(button, "clicked", G_CALLBACK(close_about), window);

	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show_all(window);
}
