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

#include "cursor.h"

#include <c64.h>
#include <string.h>

#include "boxes.h"
#include "defines.h"
#include "global.h"
#include "mem.h"
#include "myassert.h"
#include "print.h"
#include "song.h"
#include "player.h"
#include "phrase.h"
#include "chain.h"

#define BLINK_DELAY 0x10
static U8 blink_status;
static U8 blink_delay;

static U8 cursor_x;
static U8 cursor_y;
static U8 cursor_w = 1;

static void draw_cursor(U8 invert)
{
	paint_box(cursor_x, cursor_y,
			cursor_x + cursor_w - 1, 
			cursor_y, invert);
}

static void apply_cursor()
{
	draw_cursor(blink_status);
	blink_delay = BLINK_DELAY;
}

static void reset_blink()
{
	blink_status = 1;
	apply_cursor();
}

void move_cursor(U8 col, U8 row, U8 w)
{
	if (cursor_x == col && cursor_y == row && cursor_w == w)
		return;
	draw_cursor(0);
	cursor_x = col;
	cursor_y = row;
	cursor_w = w;
	draw_cursor(1);
}

void cursor_tick()
{
	if (blink_delay--)
		return;
	blink_status ^= 1;
	apply_cursor();
}

// ----------- Marking.

#define TOP_Y 1

static void paint_mark(U8 invert)
{
	U8 col = MIN(g_clip_x1, g_clip_x2);
	const U8 col_max = MAX(g_clip_x1, g_clip_x2);
	const U8 y_offset = cursor_y_offset();
	U8 row_min = MIN(g_clip_y1, g_clip_y2) - y_offset;
	U8 row_max = MAX(g_clip_y1, g_clip_y2) - y_offset;
	if (0xf0u == (row_min & 0xf0u))
		row_min = 0;
	if (row_max & 0xf0u)
		row_max = 0xf;
	row_min += TOP_Y;
	row_max += TOP_Y;

	while (col <= col_max)
	{
		const U8 x = g_cur_box_data->column_x[col];
		const U8 w = g_cur_box_data->column_w[col];
		paint_box(x, row_min, x + w - 1, row_max, invert);
		++col;
	}
}

void clear_mark()
{
	paint_mark(0);
}

void refresh_mark()
{
	if (!g_marking) return;
	paint_mark(0);
	g_clip_x2 = CUR_COL;
	g_clip_y2 = CUR_ROW;
	paint_mark(1);
}

// ----- Play indicators.

#include "player.h"

enum {
	PI_SONG0,
	PI_SONG1,
	PI_SONG2,
	PI_CHAIN,
	PI_PHRASE,
	PI_COUNT
};

#define PI_SONG_X0 VIC.spr0_x
#define PI_SONG_X1 VIC.spr1_x
#define PI_SONG_X2 VIC.spr2_x
#define PI_CHAIN_X VIC.spr3_x
#define PI_PHRASE_X VIC.spr4_x

void init_play_indicators()
{
	U8 it;

	// Change screen X offset, to give some space for sprites to the left of column 0.
	VIC.ctrl2 |= 2; 

	for (it = 0; it < PI_COUNT; ++it)
	{
		(&VIC.spr0_color)[it] = COLOR_YELLOW;
		*(U8*)(0x7f8 + it) = (SPRITE_OFFSET >> 6);
	}	
#define SCREEN_LEFT 0x18
#define SCREEN_TOP 0x32
#define SPRITE_X_FROM_COL(c) ((c) * 8 + SCREEN_LEFT)
	PI_SONG_X0 = SPRITE_X_FROM_COL(0);
	PI_SONG_X1 = SPRITE_X_FROM_COL(3);
	PI_SONG_X2 = SPRITE_X_FROM_COL(6);
	PI_CHAIN_X = SPRITE_X_FROM_COL(CHAIN_X + 1);
	PI_PHRASE_X = SPRITE_X_FROM_COL(PHRASE_X + 1);

	VIC.spr_hi_x = 0;
}

#define PI_SONG_Y0 VIC.spr0_y
#define PI_SONG_Y1 VIC.spr1_y
#define PI_SONG_Y2 VIC.spr2_y
#define PI_CHAIN_Y VIC.spr3_y
#define PI_PHRASE_Y VIC.spr4_y

static U8 song_y_to_screen(U8 ch)
{
	U8 row = (int)(SONG_CHAIN_PTR[ch] - ch) / 3;
	return SCREEN_TOP + 8 * (1 + row - g_song_y_offset);
}

void play_indicator_tick()
{
	VIC.spr_ena = 0;
	if (!g_playing)
		return;
	switch (PLAYMODE)
	{
		case PLAYMODE_SONG:
			if (CH_ACTIVE[0])
			{
				PI_SONG_Y0 = song_y_to_screen(0);
				VIC.spr_ena |= (1 << PI_SONG0);
			}
			if (CH_ACTIVE[1]) 
			{
				PI_SONG_Y1 = song_y_to_screen(1);
				VIC.spr_ena |= (1 << PI_SONG1);
			}
			if (CH_ACTIVE[2]) 
			{
				PI_SONG_Y2 = song_y_to_screen(2);
				VIC.spr_ena |= (1 << PI_SONG2);
			}
		case PLAYMODE_CHAIN:
			if (CH_ACTIVE[EDIT_CH] && ACTIVE_CHAIN[EDIT_CH] == VIEW_CHAIN)
			{
				VIC.spr_ena |= 1 << PI_CHAIN;
				PI_CHAIN_Y = SCREEN_TOP + 8 * (1 + (CHAIN_PHRASE_PTR_LSB[EDIT_CH] & 0xf));
			}
		default: // = phrase
			if (CH_ACTIVE[EDIT_CH] && ACTIVE_PHRASE[EDIT_CH] == VIEW_PHRASE)
			{
				VIC.spr_ena |= 1 << PI_PHRASE;
				PI_PHRASE_Y = SCREEN_TOP + 8 * (1 + (PHRASE_NOTE_PTR_LSB[EDIT_CH] & 0xf));
			}
	}
}
