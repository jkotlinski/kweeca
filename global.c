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

#include "global.h"

#include <conio.h>

#include "boxes.h"
#include "cursor.h"
#include "myassert.h"
#include "phrase.h"
#include "print.h"

U8 g_octave[3] = { 3, 3, 3 };

void (*release_space_handler_p)();

// Clipboard.
U8 g_marking;
U8 g_clip_x1;
U8 g_clip_y1;
U8 g_clip_x2;
U8 g_clip_y2;
U8 g_clip_type;
U8 g_clipboard[4][16];
U8 g_space_tap_count;

void sort(U8* a, U8* b)
{
	if (*a > *b)
	{
		U8 tmp = *a;
		*a = *b;
		*b = tmp;
	}
	assert(*a <= *b);
}

void so(U8 val)
{
	gotoxy(0, 0);
	print_hex(val);
}
