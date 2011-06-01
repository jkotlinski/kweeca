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
