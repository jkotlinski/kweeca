/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "types.h"

extern U8 g_octave[3];

extern void (*release_space_handler_p)();

// Clipboard.
extern U8 g_marking;
extern U8 g_clip_x1;
extern U8 g_clip_y1;
extern U8 g_clip_x2;
extern U8 g_clip_y2;
extern U8 g_clip_type;
#define CLIPBOARD_HEIGHT 16
extern U8 g_clipboard[4][CLIPBOARD_HEIGHT];
extern U8 g_space_tap_count;

// Make sure a is less than b.
void sort(U8* a, U8* b);

void so(U8 val); // Print one byte in top left - used for debugging.

#endif // _GLOBAL_H
