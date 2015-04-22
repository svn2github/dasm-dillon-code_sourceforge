/*
    $Id$

    the DASM macro assembler (aka small systems cross assembler)

    Copyright (c) 1988-2002 by Matthew Dillon.
    Copyright (c) 1995 by Olaf "Rhialto" Seibert.
    Copyright (c) 2003-2008 by Andrew Davie.
    Copyright (c) 2008 by Peter H. Froehlich.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/**
 * @file
 */

/*
    The functions strlcat() and strlcpy() are not distributed under the GNU
    General Public License but a custom license reproduced below. Although
    not strictly necessary, inclusion of these functions in a GPLed project
    has been explicitly allowed by their author, Todd C. Miller.

    The functions were downloaded from these URLs on 2008/04/07 and had the
    indicated versioning information attached:

        ftp://ftp.openbsd.org/pub/OpenBSD/src/lib/libc/string/strlcat.c
          $OpenBSD: strlcat.c,v 1.13 2005/08/08 08:05:37 espie Exp $
        ftp://ftp.openbsd.org/pub/OpenBSD/src/lib/libc/string/strlcpy.c
          $OpenBSD: strlcpy.c,v 1.11 2006/05/05 15:27:38 millert Exp $

    The code was modified to include assertions and to make splint happier
    but is otherwise unchanged.
    The documentation comments were moved into the util.h header file for
    consistency and were reformatted for Doxygen.

    The function hash_string() is based on code fragments posted by Daniel
    Bernstein to comp.lang.c on 1990/12/04. As far as we know, the code is
    in the public domain.
*/

#include "util.h"

#include "asm.h"
#include "dalloc.h"
#include "errors.h"
#include "version.h"

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

unsigned int hash_string(const char *string, size_t length)
{
    /*
        Why is this a better hash function than Matt's original?
        For MNEMONICs we go from 14 to 5 collisons, woohoo. :-)
        For SYMBOLs we go from 29 to 11 collisons, woohoo. :-)
    */

    unsigned int hash = 5381;

    assert(string != NULL);
    assert(length > 0);
    assert(length <= strlen(string));

    while (length-- != 0)
    {
        hash = ((hash << 5) + hash) + (unsigned int) *string++;
    }

    return hash;
}

char *checked_strdup(const char *s)
{
  assert(s != NULL);
  /* v_seg() doesn't enforce this so we can't either for now [phf] */
  /*assert(strlen(s) > 0);*/
  return strcpy(dalloc(strlen(s)+1), s); /* [phf] was regular */
}

size_t strlower(/*@out@*/ char *dst, const char *src, size_t size)
{
    /* strlcpy checks the assertions anyway */
    size_t result = strlcpy(dst, src, size);

    for (/* blank */; *dst != '\0'; dst++) {
        *dst = (char) tolower((int)*dst);
    }

    return result;
}

size_t strupper(/*@out@*/ char *dst, const char *src, size_t size)
{
    /* strlcpy checks the assertions anyway */
    size_t result = strlcpy(dst, src, size);

    for (/* blank */; *dst != '\0'; dst++) {
        *dst = (char) toupper((int)*dst);
    }

    return result;
}

size_t strip_whitespace(/*@out@*/ char *dst, const char *src, size_t size)
{
    /* TODO: there must be a simpler way to write this... */
    size_t n = size;
    size_t needed = 0;
    const char *t;

    assert(dst != NULL);
    assert(src != NULL);
    assert(size > 0);

    /* pre-count how many non-space characters we have */
    for (t = src; *t != '\0'; t++) {
        if (isspace((int)*t) == 0) {
            needed++;
        }
    }

    /* copy while still room and src not over */
    while (n > 1 && *src != '\0') {
        if (isspace((int)*src) != 0) {
            src++;
        }
        else {
            *dst++ = *src++;
            n--;
        }
    }

    *dst = '\0';
    n--;

    return needed;
}

bool match_either_case(const char *string, const char *either)
{
    char buffer[MAX_SYM_LEN];
    size_t res;

    assert(string != NULL);
    assert(either != NULL);

    res = strlower(buffer, either, sizeof(buffer));
    assert(res < sizeof(buffer));

    if (strcmp(string, buffer) == 0) {
        return true;
    }

    res = strupper(buffer, either, sizeof(buffer));
    assert(res < sizeof(buffer));

    if (strcmp(string, buffer) == 0) {
        return true;
    }

    return false;
}

#if !defined(__APPLE__) && !defined(__BSD__)

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

size_t
strlcat(/*@in@*/ /*@out@*/ char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* added [phf] */
	assert(dst != NULL);
	assert(src != NULL);
	assert(siz > 0);

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
    assert(d - dst >= 0); /* added [phf] */
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

size_t
strlcpy(/*@out@*/ char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* added [phf] */
	assert(dst != NULL);
	assert(src != NULL);
	assert(siz > 0);

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++ != '\0') /* splinted [phf] */
			;
	}

    assert(s - src - 1 >= 0); /* added [phf] */
	return(s - src - 1);	/* count does not include NUL */
}

/*@null@*/
static const char *__dasm_progname = NULL;

/*@temp@*/
const char *getprogname(void)
{
    return (__dasm_progname != NULL) ? __dasm_progname : "(unknown progname)";
}

void setprogname(const char *name)
{
    char *slash = NULL;
    assert(name != NULL);

    slash = strrchr(name, DASM_PATH_SEPARATOR);
    if (slash != NULL)
    {
        name = slash+1;
    }
    __dasm_progname = name;
}

#endif /* !defined(__APPLE__) && !defined(__BSD__) */

/* vim: set tabstop=4 softtabstop=4 expandtab shiftwidth=4 autoindent: */
