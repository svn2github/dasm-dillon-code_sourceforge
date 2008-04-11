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

/**
 * @file
 *   util.c
 * @brief
 *   Generic utility functions for string manipulation
 *   and memory allocation.
 */

#include "asm.h"
#include "util.h"

/*@unused@*/
SVNTAG("$Id$");

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void panic(const char *s)
{
    (void) printf("Panic: %s\n", s);
#if !defined(TEST)
/* hack for unit tests where we don't want to exit! */
    exit(EXIT_FAILURE);
#endif /* !defined(TEST) */
}

/*@null@*/
void *checked_malloc(size_t bytes)
{
    void *p;
    assert(bytes > 0); /* size_t usually unsigned, but rule out 0! */

    p = malloc(bytes);
    if (p == NULL)
    {
        panic("Unable to allocate memory!");
    }

    return p;
}

/*@null@*/
void *zero_malloc(size_t bytes)
{
    void *p;
    assert(bytes > 0); /* size_t usually unsigned, but rule out 0! */

    p = checked_malloc(bytes);
    if (p != NULL)
    {
        (void) memset(p, 0, bytes);
    }

    return p;
}

unsigned int hash_string(const char *string, size_t length)
{
    unsigned int hash = 5381;

    /*printf("string=='%s', length==%zu\n", string, length);*/

    assert(string != NULL && length <= strlen(string));

/*
    assert(string != NULL && length > 0 && length <= strlen(string));

    Crazy! findmne (and maybe others) call the hash function with
    empty strings quite a few time, I consider that a bug but I
    can't focus on it until I have the hash functions refactored
    to use this code. :-/ [phf]

    For MNEMONICs we go from 14 to 5 collisons, woohoo. :-)
    For SYMBOLs we go from 29 to 11 collisons, woohoo. :-)
*/

    while (length-- != 0)
    {
        hash = ((hash << 5) + hash) + *string++;
    }

    return hash;
}

char *strlower(char *str)
{
    char *ptr = str;

    while (*ptr != '\0')
    {
        *ptr = tolower(*ptr);
    }

    return str;
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
strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	assert(dst != NULL && src != NULL && siz > 0); /* added [phf] */

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
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
strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	assert(dst != NULL && src != NULL && siz > 0); /* added [phf] */

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

	return(s - src - 1);	/* count does not include NUL */
}

#endif /* !defined(__APPLE__) && !defined(__BSD__) */

#if defined(TEST)
/* unit tests */
int main(void)
{
  char *one;
  char *two;
  union align { long l; long long ll; void *p; void (*fp)(void); };
  printf("sizeof(align)==%zu\n", sizeof(union align));
  one = checked_malloc(1<<31);
  puts("First malloc()ed!");
  two = checked_malloc(1<<31);
  puts("Second malloc()ed! Should have caused a panic?");
  if (two) free(two);
  puts("Second free()d!");
  if (one) free(one);
  puts("First free()d!");
  return EXIT_SUCCESS;
}
#endif /* defined(TEST) */

