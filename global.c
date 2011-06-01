/** Copyright (c) 2011, Johan Kotlinski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

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
