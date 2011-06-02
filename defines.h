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

#ifndef _DEFINES_H
#define _DEFINES_H

// View locations - start.
#define PHRASE_X 10
#define PHRASE_Y 0

#define CHAIN_X 9
#define CHAIN_Y 0

#define SONG_X 8
#define SONG_Y 0
// View locations - end.

#define EDIT_CH CUR_COL_SONG

#define MAX_NOTE 95

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define CH_LEFTARROW 0x5f

#define COLOR_TITLE COLOR_GREEN

// In the future, maybe use different colors for different screens?
// If so, compare box with g_cur_box.
#define TEXTCOLOR(box) textcolor(COLOR_WHITE)

#define VERSION "0.0.0"

#endif // _DEFINES_H
