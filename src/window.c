/* $Id$ */
static char const _flashlightwindow_copyright[] =
"Copyright © 2016 Pierre Pronchery <khorben@defora.org>";
/* This file is part of DeforaOS Desktop Flashlight */
/* All rights reserved. */
static char const _flashlightwindow_license[] =
"Redistribution and use in source and binary forms, with or without\n"
"modification, are permitted provided that the following conditions are\n"
"met:\n"
"\n"
"1. Redistributions of source code must retain the above copyright\n"
"   notice, this list of conditions and the following disclaimer.\n"
"\n"
"2. Redistributions in binary form must reproduce the above copyright\n"
"   notice, this list of conditions and the following disclaimer in the\n"
"   documentation and/or other materials provided with the distribution.\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS\n"
"IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED\n"
"TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A\n"
"PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n"
"HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
"SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED\n"
"TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR\n"
"PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF\n"
"LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING\n"
"NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
"SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n";



#include <libintl.h>
#include <gdk/gdkkeysyms.h>
#include <System.h>
#include <Desktop.h>
#include "flashlight.h"
#include "window.h"
#include "../config.h"
#define _(string) gettext(string)


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
static void _flashlightwindow_on_about(gpointer data);
static gboolean _flashlightwindow_on_closex(void);
static gboolean _flashlightwindow_on_configure(gpointer data);
static gboolean _flashlightwindow_on_idle(gpointer data);
static void _flashlightwindow_on_preferences(gpointer data);


/* constants */
static const DesktopAccel _flashlightwindow_accel[] =
{
	{ G_CALLBACK(_flashlightwindow_on_closex), GDK_CONTROL_MASK,
		GDK_KEY_W },
	{ NULL, 0, 0 }
};

static char const * _flashlightwindow_authors[] =
{
	"Pierre Pronchery <khorben@defora.org>",
	NULL
};


/* public */
/* functions */
/* flashlightwindow_new */
FlashlightWindow * flashlightwindow_new(void)
{
	const int width = 300;
	const int height = 400;
	FlashlightWindow * window;
	GtkAccelGroup * group;
	GtkWidget * box;
	GtkWidget * bbox;
	GtkWidget * widget;
	GtkOrientation orientation = (height >= width)
		? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL;

	if((window = object_new(sizeof(*window))) == NULL)
		return NULL;
	/* window */
	group = gtk_accel_group_new();
	desktop_accel_create(_flashlightwindow_accel, window, group);
	window->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_add_accel_group(GTK_WINDOW(window->window), group);
	g_object_unref(group);
	gtk_container_set_border_width(GTK_CONTAINER(window->window), 4);
	gtk_window_set_default_size(GTK_WINDOW(window->window), width, height);
	gtk_window_set_icon_name(GTK_WINDOW(window->window), "gtk-dialog-info");
	gtk_window_set_title(GTK_WINDOW(window->window), _("Flashlight"));
	g_signal_connect_swapped(window->window, "configure-event", G_CALLBACK(
				_flashlightwindow_on_configure), window);
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
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	/* flashlight */
	widget = flashlight_get_widget(window->flashlight);
	gtk_box_pack_start(GTK_BOX(box), widget, TRUE, TRUE, 0);
	/* about */
	bbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
	gtk_box_set_spacing(GTK_BOX(bbox), 4);
	widget = gtk_button_new_from_stock(GTK_STOCK_PREFERENCES);
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(
				_flashlightwindow_on_preferences), window);
	gtk_container_add(GTK_CONTAINER(bbox), widget);
	widget = gtk_button_new_from_stock(GTK_STOCK_ABOUT);
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(
				_flashlightwindow_on_about), window);
	gtk_container_add(GTK_CONTAINER(bbox), widget);
	gtk_box_pack_start(GTK_BOX(box), bbox, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window->window), box);
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
/* flashlightwindow_on_about */
static void _flashlightwindow_on_about(gpointer data)
{
	FlashlightWindow * window = data;
	GtkWidget * dialog;

	dialog = desktop_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(dialog),
			GTK_WINDOW(window->window));
	desktop_about_dialog_set_authors(dialog, _flashlightwindow_authors);
	desktop_about_dialog_set_comments(dialog,
			_("Flashlight application for the DeforaOS desktop"));
	desktop_about_dialog_set_copyright(dialog, _flashlightwindow_copyright);
	desktop_about_dialog_set_license(dialog, _flashlightwindow_license);
	desktop_about_dialog_set_logo_icon_name(dialog, "gtk-dialog-info");
	desktop_about_dialog_set_program_name(dialog, PACKAGE);
	desktop_about_dialog_set_version(dialog, VERSION);
	desktop_about_dialog_set_website(dialog, "http://www.defora.org/");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


/* flashlightwindow_on_closex */
static gboolean _flashlightwindow_on_closex(void)
{
	gtk_main_quit();
	return TRUE;
}


/* flashlightwindow_on_configure */
static gboolean _flashlightwindow_on_configure(gpointer data)
{
	FlashlightWindow * window = data;
	gint width;
	gint height;
	GtkOrientation orientation;

	gtk_window_get_size(GTK_WINDOW(window->window), &width, &height);
	orientation = (height >= width)
		? GTK_ORIENTATION_VERTICAL : GTK_ORIENTATION_HORIZONTAL;
	flashlight_set_orientation(window->flashlight, orientation);
	return TRUE;
}


/* flashlightwindow_on_idle */
static gboolean _flashlightwindow_on_idle(gpointer data)
{
	FlashlightWindow * window = data;

	flashlight_set_active(window->flashlight, TRUE);
	window->source = 0;
	return FALSE;
}


/* flashlightwindow_on_preferences */
static void _flashlightwindow_on_preferences(gpointer data)
{
	FlashlightWindow * window = data;
	GtkWidget * dialog;
	const unsigned int flags = GTK_DIALOG_MODAL
		| GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget * content;
	GtkWidget * widget;
	gboolean active;

	dialog = gtk_dialog_new_with_buttons(_("Preferences"),
			GTK_WINDOW(window->window), flags,
			_("_Close"), GTK_RESPONSE_CLOSE, NULL);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
#if GTK_CHECK_VERSION(2, 14, 0)
	content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
#else
	content = GTK_DIALOG(dialog)->content_area;
#endif
	widget = gtk_check_button_new_with_mnemonic(
			_("Stay lit when released"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
			flashlight_get_keep_lit(window->flashlight));
	gtk_widget_show(widget);
	gtk_box_pack_start(GTK_BOX(content), widget, FALSE, TRUE, 0);
	gtk_dialog_run(GTK_DIALOG(dialog));
	active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	flashlight_set_keep_lit(window->flashlight, active);
	gtk_widget_destroy(dialog);
}
