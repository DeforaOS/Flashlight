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



#ifndef FLASHLIGHT_FLASHLIGHT_H
# define FLASHLIGHT_FLASHLIGHT_H


/* Flashlight */
/* types */
typedef struct _Flashlight Flashlight;


/* functions */
Flashlight * flashlight_new(GtkOrientation orientation);
void flashlight_delete(Flashlight * window);

/* accessors */
gboolean flashlight_get_active(Flashlight * flashlight);
gboolean flashlight_get_keep_lit(Flashlight * flashlight);
GtkWidget * flashlight_get_widget(Flashlight * flashlight);

void flashlight_set_active(Flashlight * flashlight, gboolean active);
void flashlight_set_keep_lit(Flashlight * flashlight, gboolean active);
void flashlight_set_orientation(Flashlight * flashlight,
		GtkOrientation orientation);

/* useful */
void flashlight_toggle(Flashlight * flashlight);

#endif /* !FLASHLIGHT_FLASHLIGHT_H */
