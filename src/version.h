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

#ifndef _DASM_VERSION_H
#define _DASM_VERSION_H

/**
 * @brief Macro to tag files going into DASM executables
 * with Subversion "Id" keywords.
 *
 * @warning Only .c files should be tagged, for obvious
 * reasons.
 */
/* TODO: add some kind of __unused annotation to make sure they stay? */
#define SVNTAG(id) static const char _svnid[] = id

/**
 * @brief Macro to provide current DASM release as a string.
 */
#define DASM_RELEASE "2.20.12"

/**
 * @brief Macro to provide current DASM headline, including
 * release, as a string; printed first in usage messages.
 */
#define DASM_ID "DASM " DASM_RELEASE

/**
 * @brief Macro to print standard DASM copyright, in case we
 * go back to printing this part on every run again...
 *
 * @todo There are several pieces of code out there that have
 * 1987 as the first copyright year by Matthew Dillon; almost
 * all his later stuff starts at 1988, but ftohex.c is still
 * at 1987 even in his 2.16 release. We should settle this...
 */
#define DASM_PRINT_COPYRIGHT \
  puts("Copyright (c) 1988-2008 by various authors (see file AUTHORS).");

/**
 * @brief Macro to print standard DASM legalese, including
 * copyright, also for usage messages.
 */
#define DASM_PRINT_LEGAL \
  DASM_PRINT_COPYRIGHT \
  puts("License GPLv2+: GNU GPL version 2 or later (see file COPYING)."); \
  puts("DASM is free software: you are free to change and redistribute it."); \
  puts("There is ABSOLUTELY NO WARRANTY, to the extent permitted by law."); \

/**
 * @brief Macro to print bug report message, once again for
 * usage messages.
 */
#define DASM_PRINT_BUGS \
  puts("Report bugs to dasm-dillon-discuss@lists.sf.net please!");

#endif /* _DASM_VERSION_H */

