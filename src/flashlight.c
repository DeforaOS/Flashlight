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



#include <libintl.h>
#include <System.h>
#include <Desktop.h>
#include "backend.h"
#include "flashlight.h"
#define _(string) gettext(string)


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
	gboolean control;
	GtkWidget * co_press;
	GtkWidget * co_toggle;
};


/* prototypes */
/* callbacks */
static gboolean _flashlight_on_button_pressed(gpointer data);
static gboolean _flashlight_on_button_released(gpointer data);
static void _flashlight_on_toggled(gpointer data);


/* public */
/* functions */
/* flashlight_new */
Flashlight * flashlight_new(GtkOrientation orientation)
{
	Flashlight * flashlight;
	GtkWidget * widget;
	GtkIconTheme * icontheme;
	GdkPixbuf * pixbuf;
	const unsigned int size = 256;
	const unsigned int flags = 0;

	if((flashlight = object_new(sizeof(*flashlight))) == NULL)
		return NULL;
	flashlight->box = gtk_box_new(orientation, 0);
	gtk_box_set_homogeneous(GTK_BOX(flashlight->box), TRUE);
	/* image */
	icontheme = gtk_icon_theme_get_default();
	pixbuf = gtk_icon_theme_load_icon(icontheme, "gtk-dialog-info", size,
			flags, NULL);
	flashlight->image = gtk_image_new_from_pixbuf(pixbuf);
	gtk_box_pack_start(GTK_BOX(flashlight->box), flashlight->image, TRUE,
			TRUE, 0);
	/* controls */
	widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	/* press switch */
	flashlight->co_press = gtk_button_new_with_mnemonic(_("_Press"));
	g_signal_connect_swapped(flashlight->co_press, "button-press-event",
			G_CALLBACK(_flashlight_on_button_pressed), flashlight);
	g_signal_connect_swapped(flashlight->co_press, "button-release-event",
			G_CALLBACK(_flashlight_on_button_released), flashlight);
	gtk_widget_set_no_show_all(flashlight->co_press, TRUE);
	gtk_box_pack_start(GTK_BOX(widget), flashlight->co_press, TRUE, TRUE,
			0);
	/* toggle switch */
#if GTK_CHECK_VERSION(3, 0, 0)
	flashlight->co_toggle = gtk_switch_new();
	g_signal_connect_swapped(flashlight->co_toggle, "notify::active",
			G_CALLBACK(_flashlight_on_toggled), flashlight);
#else
# warning Switch widget is not available (needs Gtk+ >= 3.0)
	flashlight->co_toggle = gtk_toggle_button_new_with_mnemonic(
			_("_Switch"));
	g_signal_connect_swapped(flashlight->co_toggle, "toggled", G_CALLBACK(
				_flashlight_on_toggled), flashlight);
#endif
	gtk_widget_set_no_show_all(flashlight->co_toggle, TRUE);
	gtk_box_pack_start(GTK_BOX(widget), flashlight->co_toggle, TRUE, TRUE,
			0);
	gtk_box_pack_start(GTK_BOX(flashlight->box), widget, FALSE, TRUE, 0);
	flashlight_set_active(flashlight, flashlight_get_active(flashlight));
	flashlight_set_keep_lit(flashlight, TRUE);
	return flashlight;
}


/* flashlight_delete */
void flashlight_delete(Flashlight * flashlight)
{
	flashlightbackend_set(FALSE);
	gtk_widget_destroy(flashlight->box);
	object_delete(flashlight);
}


/* accessors */
/* flashlight_get_active */
gboolean flashlight_get_active(Flashlight * flashlight)
{
	switch(flashlightbackend_get())
	{
		case FBA_ACTIVE:
			return TRUE;
		case FBA_INACTIVE:
			return FALSE;
		default:
#if GTK_CHECK_VERSION(3, 0, 0)
			return gtk_switch_get_active(GTK_SWITCH(
						flashlight->co_toggle));
#else
			return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
						flashlight->co_toggle));
#endif
	}
}


/* flashlight_get_keep_lit */
gboolean flashlight_get_keep_lit(Flashlight * flashlight)
{
	return flashlight->control;
}


/* flashlight_get_widget */
GtkWidget * flashlight_get_widget(Flashlight * flashlight)
{
	return flashlight->box;
}


/* flashlight_set_active */
void flashlight_set_active(Flashlight * flashlight, gboolean active)
{
	flashlightbackend_set(active);
	gtk_widget_set_sensitive(flashlight->image, active);
#if GTK_CHECK_VERSION(3, 0, 0)
	gtk_switch_set_active(GTK_SWITCH(flashlight->co_toggle), active);
#else
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(flashlight->co_toggle),
			active);
#endif
}


/* flashlight_set_keep_lit */
void flashlight_set_keep_lit(Flashlight * flashlight, gboolean active)
{
	flashlight->control = active;
	gtk_widget_hide(active ? flashlight->co_press : flashlight->co_toggle);
	gtk_widget_show(active ? flashlight->co_toggle : flashlight->co_press);
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
static gboolean _flashlight_on_button_pressed(gpointer data)
{
	Flashlight * flashlight = data;

	flashlight_set_active(flashlight, TRUE);
	return FALSE;
}


static gboolean _flashlight_on_button_released(gpointer data)
{
	Flashlight * flashlight = data;

	flashlight_set_active(flashlight, FALSE);
	return FALSE;
}


/* flashlight_on_toggled */
static void _flashlight_on_toggled(gpointer data)
{
	Flashlight * flashlight = data;

#if GTK_CHECK_VERSION(3, 0, 0)
	flashlight_set_active(flashlight, gtk_switch_get_active(
				GTK_SWITCH(flashlight->co_toggle)));
#else
	flashlight_set_active(flashlight, gtk_toggle_button_get_active(
				GTK_TOGGLE_BUTTON(flashlight->co_toggle)));
#endif
}
