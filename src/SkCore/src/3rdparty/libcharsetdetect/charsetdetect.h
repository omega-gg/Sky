//=================================================================================================
/*
    Copyright (C) 2015-2020 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of Sky kit.

    - GNU Lesser General Public License Usage:
    This file may be used under the terms of the GNU Lesser General Public License version 3 as
    published by the Free Software Foundation and appearing in the LICENSE.md file included in the
    packaging of this file. Please review the following information to ensure the GNU Lesser
    General Public License requirements will be met: https://www.gnu.org/licenses/lgpl.html.
*/
//=================================================================================================

#ifndef charsetdetect_
#define charsetdetect_

/* The classes below are exported */
#pragma GCC visibility push(default)

#ifdef __cplusplus
extern "C" {
#endif

// Opaque type of character set detectors
typedef void* csd_t;
    
// Create a new character set detector. Must be freed by csd_close.
// If creation fails, returns (csd_t)-1.
csd_t csd_open(void);
// Feeds some more data to the character set detector. Returns 0 if it
// needs more data to come to a conclusion and a positive number if it has enough to say what
// the character set is. Returns a negative number if there is an error.
int csd_consider(csd_t csd, const char *data, int length);
// Closes the character set detector and returns the detected character set name as an ASCII string.
// Returns NULL if detection failed.
const char *csd_close(csd_t csd);

// Sky implementation
const char *csd_codec(const char *data, int length);
#ifdef __cplusplus
}
#endif

#pragma GCC visibility pop
#endif
