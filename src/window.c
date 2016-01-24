/* $Id$ */
/* Copyright (c) 2016 Pierre Pronchery <khorben@defora.org> */
/* This file is part of DeforaOS Desktop Flashlight */
/* All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */



#include <System.h>
#include <Desktop.h>
#include "flashlight.h"
#include "window.h"
#include "../config.h"


/* FlashlightWindow */
/* private */
/* types */
struct _FlashlightWindow
{
	GtkWidget * window;
	Flashlight * flashlight;
	guint source;
};


/* prototypes */
/* callbacks */
static gboolean _flashlightwindow_on_closex(void);
static gboolean _flashlightwindow_on_idle(gpointer data);


/* public */
/* functions */
/* flashlightwindow_new */
FlashlightWindow * flashlightwindow_new(void)
{
	const int width = 300;
	const int height = 400;
	FlashlightWindow * window;
	GtkWidget * widget;
	GtkOrientation orientation = (height >= width)
		? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL;

	if((window = object_new(sizeof(*window))) == NULL)
		return NULL;
	window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window->window), width, height);
	gtk_window_set_title(GTK_WINDOW(window->window), PACKAGE);
	g_signal_connect(window->window, "delete-event", G_CALLBACK(
				_flashlightwindow_on_closex), NULL);
	window->flashlight = flashlight_new(orientation);
	window->source = 0;
	/* check for errors */
	if(window->flashlight == NULL)
	{
		flashlightwindow_delete(window);
		return NULL;
	}
	widget = flashlight_get_widget(window->flashlight);
	gtk_container_add(GTK_CONTAINER(window->window), widget);
	gtk_widget_show_all(window->window);
	window->source = g_idle_add(_flashlightwindow_on_idle, window);
	return window;
}


/* flashlightwindow_delete */
void flashlightwindow_delete(FlashlightWindow * window)
{
	if(window->source != 0)
		g_source_remove(window->source);
	if(window->flashlight != NULL)
		flashlight_delete(window->flashlight);
	gtk_widget_destroy(window->window);
	object_delete(window);
}


/* prototypes */
/* callbacks */
/* flashlightwindow_on_closex */
static gboolean _flashlightwindow_on_closex(void)
{
	gtk_main_quit();
	return TRUE;
}


/* flashlightwindow_on_idle */
static gboolean _flashlightwindow_on_idle(gpointer data)
{
	FlashlightWindow * window = data;

	flashlight_toggle(window->flashlight);
	window->source = 0;
	return FALSE;
}
