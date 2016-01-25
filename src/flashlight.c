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


/* Flashlight */
/* private */
/* types */
struct _Flashlight
{
	/* widgets */
	GtkWidget * box;
	/* image */
	GtkWidget * image;
	/* controls */
	GtkWidget * co_main;
};


/* prototypes */
/* callbacks */
static void _flashlight_on_toggled(GtkWidget * widget, gpointer data);


/* public */
/* functions */
/* flashlight_new */
Flashlight * flashlight_new(GtkOrientation orientation)
{
	Flashlight * flashlight;
	GtkWidget * widget;

	if((flashlight = object_new(sizeof(*flashlight))) == NULL)
		return NULL;
	flashlight->box = gtk_box_new(orientation, 0);
	gtk_box_set_homogeneous(GTK_BOX(flashlight->box), TRUE);
	/* image */
	flashlight->image = gtk_image_new_from_icon_name("gtk-dialog-info",
			GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(flashlight->box), flashlight->image, TRUE,
			TRUE, 0);
	/* controls */
	widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	flashlight->co_main = gtk_toggle_button_new_with_mnemonic("_Switch");
	g_signal_connect(flashlight->co_main, "toggled", G_CALLBACK(
				_flashlight_on_toggled), flashlight);
	gtk_box_pack_start(GTK_BOX(widget), flashlight->co_main, FALSE, TRUE,
			0);
	gtk_box_pack_start(GTK_BOX(flashlight->box), widget, FALSE, TRUE, 0);
	flashlight_set_active(flashlight, flashlight_get_active(flashlight));
	return flashlight;
}


/* flashlight_delete */
void flashlight_delete(Flashlight * flashlight)
{
	gtk_widget_destroy(flashlight->box);
	object_delete(flashlight);
}


/* accessors */
/* flashlight_get_active */
gboolean flashlight_get_active(Flashlight * flashlight)
{
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
				flashlight->co_main));
}


/* flashlight_get_widget */
GtkWidget * flashlight_get_widget(Flashlight * flashlight)
{
	return flashlight->box;
}


/* flashlight_set_active */
void flashlight_set_active(Flashlight * flashlight, gboolean active)
{
	gtk_widget_set_sensitive(flashlight->image, active);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(flashlight->co_main),
			active);
}


/* flashlight_set_orientation */
void flashlight_set_orientation(Flashlight * flashlight,
		GtkOrientation orientation)
{
#if GTK_CHECK_VERSION(2, 16, 0)
	gtk_orientable_set_orientation(GTK_ORIENTABLE(flashlight->box),
			orientation);
#else
# warning Switching orientation is not supported (needs Gtk+ >= 2.16)
#endif
}


/* useful */
/* flashlight_toggle */
void flashlight_toggle(Flashlight * flashlight)
{
	gboolean active;

	active = flashlight_get_active(flashlight);
	flashlight_set_active(flashlight, !active);
}


/* prototypes */
/* callbacks */
/* flashlight_on_toggled */
static void _flashlight_on_toggled(GtkWidget * widget, gpointer data)
{
	Flashlight * flashlight = data;

	flashlight_set_active(flashlight, gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON(widget)));
}
