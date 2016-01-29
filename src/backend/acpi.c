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



#include <sys/param.h>
#include <sys/sysctl.h>
#include <string.h>
#include <errno.h>
#include <System.h>
#include "../backend.h"

#ifndef PROGNAME
# define PROGNAME "flashlight"
#endif


/* flashlightbackend_get */
FlashlightBackendActive flashlightbackend_get(void)
{
	char const sysctl[] = "hw.acpi.acpiout0.brightness";
	size_t s;
	int level;

	if(sysctlbyname(sysctl, &level, &s, NULL, 0) != 0)
	{
		error_set_print(PROGNAME, -errno, "%s: %s", sysctl,
				strerror(errno));
		return FBA_UNKNOWN;
	}
	else if(level < 0)
	{
		error_set_print(PROGNAME, -ERANGE, "%s: %s", sysctl,
				strerror(ERANGE));
		return FBA_UNKNOWN;
	}
	return (level > 0) ? FBA_ACTIVE : FBA_INACTIVE;
}


/* flashlightbackend_set */
int flashlightbackend_set(FlashlightBackendActive active)
{
	char const sysctl[] = "hw.acpi.acpiout0.brightness";
	int level = active ? 100 : 0;

	if(sysctlbyname(sysctl, NULL, NULL, &level, sizeof(level)) != 0)
	{
		error_set_print(PROGNAME, -errno, "%s: %s", sysctl,
				strerror(errno));
		return FBA_UNKNOWN;
	}
	return 0;
}
