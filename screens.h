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

#ifndef _SCREENES_H
#define _SCREENES_H

#include "types.h"
#include "screen_data.h"

typedef enum {
    SONG_SCREEN,
    CHAIN_SCREEN,
    PHRASE_SCREEN,
    INSTR_SCREEN,
    SCREEN_COUNT
} SCREEN_ID;
extern U8 g_cur_screen;

#define IS_SCREEN_ACTIVE(a) (a == g_cur_screen)

void switch_screen(SCREEN_ID id);

void screens_handle_key(U8 key);

extern U8 cur_rows[SCREEN_COUNT];
#define CUR_ROW_SONG cur_rows[SONG_SCREEN]
#define CUR_ROW_PHRASE cur_rows[PHRASE_SCREEN]
#define CUR_ROW_CHAIN cur_rows[CHAIN_SCREEN]
extern U8 cur_cols[SCREEN_COUNT];
#define CUR_COL_SONG cur_cols[SONG_SCREEN]
#define CUR_COL_PHRASE cur_cols[PHRASE_SCREEN]
#define CUR_COL_CHAIN cur_cols[CHAIN_SCREEN]

#define CUR_COL cur_cols[g_cur_screen]
#define CUR_ROW cur_rows[g_cur_screen]
#define COLUMN_COUNT screen_data[g_cur_screen]->column_count

U8 cursor_y_offset();

extern const ScreenData* g_cur_screen_data;

// Get a hex from keyboard. Return status.
U8 get_hex_from_keyboard(U8 key, U8 max_value);
extern U8 g_hexget_value;
#define HEXGET_ACTIVE 0
#define HEXGET_DONE 1
#define HEXGET_IDLE 2

void mark();
void paste();
void cut_marked();

void reset_screens();

#endif // _SCREENES_H
