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
#ifdef USE_GSTREAMER
#include <gst/gst.h>
#endif

#ifdef USE_GSTREAMER
static void
add_pad (GstElement *element , GstPad *pad , gpointer data){

	gchar *name;
	GstElement *sink = (GstElement*)data;

	name = gst_pad_get_name(pad);
	gst_element_link_pads(element , name , sink , "sink");
	g_free(name);
}
#endif /* USE_GSTREAMER */

void
fx_sound_play_file(const char *filename){

#ifdef USE_GSTREAMER
	GstElement *pipeline;
	GstElement *source , *parser , *sink;

	pipeline = gst_pipeline_new("audio-player");

	source = gst_element_factory_make("filesrc" , "source");
	if(source == NULL){
		g_warning("make filesrc element failed");
		return;
	}
	parser = gst_element_factory_make("wavparse" , "parser");
	if(parser == NULL){
		g_warning("make wavparse element failed");
		return;
	}
	sink = gst_element_factory_make("alsasink" , "output");
	if(sink == NULL){
		g_warning("make alsasink element failed");
		return;
	}

	g_object_set(G_OBJECT(source) , "location"
			, filename , NULL);

	gst_bin_add_many(GST_BIN(pipeline)
			, source , parser , sink , NULL);

	g_signal_connect(parser
			, "pad-added" , G_CALLBACK(add_pad) , sink);

	if(! gst_element_link(source , parser)){
		g_warning("linke source to parser failed");
	}

	gst_element_set_state(pipeline , GST_STATE_PLAYING);
	sleep(1);
	gst_element_set_state(pipeline , GST_STATE_NULL);
	g_object_unref(pipeline);
#endif /* USE_GSTREAMER */
}
