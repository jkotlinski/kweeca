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

#include "boxes.h"

#include <cbm.h>
#include <conio.h>
#include <string.h>

#include "cursor.h"
#include "defines.h"
#include "kbd.h"
#include "global.h"
#include "myassert.h"
#include "status.h"
#include "song.h"
#include "chain.h"
#include "phrase.h"
#include "instr.h"

U8 g_cur_box = SONG_BOX;
U8 cur_rows[BOX_COUNT];
U8 cur_cols[BOX_COUNT];

const BoxData* g_cur_box_data;

static handler_fn key_handler;

static const BoxData* const box_data[] =
{
	&k_song_box_data,
	&k_chain_box_data,
	&k_phrase_box_data,
	&k_instr_box_data
};

void redraw_all_boxes()
{
	U8 it;
	for (it = 0; it < BOX_COUNT; ++it)
		if (g_cur_box != it)
			box_data[it]->redraw();
	box_data[g_cur_box]->redraw(); // Draw active box last.
}

void refresh_cursor()
{
	if (g_cur_box_data->refresh_cursor)
		g_cur_box_data->refresh_cursor();
	else
		move_cursor(g_cur_box_data->column_x[CUR_COL], 1 + CUR_ROW - cursor_y_offset(), g_cur_box_data->column_w[CUR_COL]);
}

static void stop_marking()
{
	if (!g_marking) return;
	g_marking = 0;
	CUR_COL = g_clip_x1;
	CUR_ROW = g_clip_y1;
	if (g_cur_box_data->y_pos_validator)
		g_cur_box_data->y_pos_validator();
	refresh_cursor();
	clear_mark();
}

#include <stdio.h>

void switch_to_box(BOX_ID box)
{
	g_cur_box_data = box_data[box];

	stop_marking();
	g_cur_box = box;
	key_handler = g_cur_box_data->key_handler;
	redraw_all_boxes();
	refresh_cursor();
}

static void move_cursor_up()
{
	if (g_marking && 
			g_clip_y2 < g_clip_y1 && 
			(g_clip_y1 - g_clip_y2 >= CLIPBOARD_HEIGHT - 1))
	{
		return;
	}
	assert(g_cur_box_data->cur_up_handler);
	g_cur_box_data->cur_up_handler();
	refresh_cursor();
	refresh_mark();
}

static void move_cursor_down()
{
	if (g_marking && 
			g_clip_y2 > g_clip_y1 && 
			(g_clip_y2 - g_clip_y1 >= CLIPBOARD_HEIGHT - 1))
	{
		return;
	}
	assert(g_cur_box_data->cur_down_handler);
	g_cur_box_data->cur_down_handler();
	refresh_cursor();
	refresh_mark();
}

static void screen_down()
{
	U8 i;
	for (i = 0; i < 16; ++i)
		move_cursor_down();
}

static void screen_up()
{
	U8 i;
	for (i = 0; i < 16; ++i)
		move_cursor_up();
}

// Expand vertically. Return 0 if selection was already expanded, otherwise 1.
static U8 mark_vertical_expand()
{
	const U8 min_y = cursor_y_offset();
	const U8 max_y = min_y + 0xf;
	if (MAX(g_clip_y1, g_clip_y2)
			- MIN(g_clip_y1, g_clip_y2) 
			>= CLIPBOARD_HEIGHT - 1)
		return 0;
	g_clip_y1 = min_y;
	g_clip_y2 = max_y;
	return 1;
}

static void mark()
{
	if (g_marking)
	{
		// Tap m twice for vertical expand, thrice for horizontal...
		if (!mark_vertical_expand())
		{
			U8 horizontally_expanded = !g_clip_x1 && g_clip_x2 == COLUMN_COUNT - 1;
			if (horizontally_expanded)
			{
				stop_marking();
				return;
			}
			// Expand horizontally.
			g_clip_x1 = 0;
			g_clip_x2 = COLUMN_COUNT - 1;
		}
		CUR_COL = g_clip_x2;
		CUR_ROW = g_clip_y2;
	}
	else
	{
		// Start mark.
		g_marking = 1;
		g_clip_type = g_cur_box;
		g_clip_x1 = CUR_COL;
		g_clip_x2 = CUR_COL;
		g_clip_y1 = CUR_ROW;
		g_clip_y2 = CUR_ROW;
		show_status_msg(MSG_MARKING);
	}
	refresh_cursor();
	refresh_mark();
};

void cut_marked()
{
	assert(g_clip_type == g_cur_box);

	sort(&g_clip_x1, &g_clip_x2);
	sort(&g_clip_y1, &g_clip_y2);

	g_clip_x2 = MAX(g_cur_box_data->min_allowed_lift_col_2, g_clip_x2);

	CUR_ROW = g_clip_y1;

	while (CUR_ROW <= g_clip_y2)
	{
		CUR_COL = g_clip_x1;
		while (CUR_COL <= g_clip_x2)
		{
			g_cur_box_data->lift_col_handler();
			++CUR_COL;
		}
		++CUR_ROW;
	}
	CUR_COL = g_clip_x1;
	CUR_ROW = g_clip_y1;
	stop_marking();
	show_status_msg(MSG_LIFT_OK);
}

static void paste()
{
	U8 src_y = g_clip_y1;
	U8 prev_col = CUR_COL;
	const paste_col_fn paster = g_cur_box_data->paste_col_handler;

	if (g_marking || g_clip_type != g_cur_box || !paster)
		return;

	while (src_y <= g_clip_y2)
	{
		CUR_COL = g_clip_x1;
		while (CUR_COL <= g_clip_x2)
		{
			const U8 val = g_clipboard[CUR_COL][src_y];

			paster(val);

			++CUR_COL;
		}
		if (g_cur_box_data->paste_advance_row)
			g_cur_box_data->paste_advance_row();
		else
		{
			++CUR_ROW;
			CUR_ROW &= 0xf;
		}
		++src_y;
	}

	CUR_COL = prev_col;
	refresh_cursor();
}

static void multi_tweak(U8 val)
{
	U8 prev_col = CUR_COL;
	U8 prev_row = CUR_ROW;
	U8 col1 = MIN(g_clip_x1, g_clip_x2);
	U8 col2 = MAX(g_clip_x1, g_clip_x2);
	U8 row1 = MIN(g_clip_y1, g_clip_y2);
	U8 row2 = MAX(g_clip_y1, g_clip_y2);
	tweak_handler_fn tweak_handler = g_cur_box_data->tweak_handler;

	assert(tweak_handler);

	if (!g_marking)
	{
		tweak_handler(val);
		return;
	}

	for (CUR_ROW = row1; CUR_ROW <= row2; ++CUR_ROW)
	{
		for (CUR_COL = col1; CUR_COL <= col2; ++CUR_COL)
		{
			tweak_handler(val);
		}
	}
	CUR_COL = prev_col;
	CUR_ROW = prev_row;
	refresh_mark();
}

void boxes_handle_key(U8 key)
{
	if (key == ' ')
		++g_space_tap_count;
	else
		g_space_tap_count = 0;

	textcolor(COLOR_WHITE);

	if (key_handler(key))
		return;

	switch (key)
	{
		case CH_LEFTARROW:
			stop_marking();
			break;

		case CH_TWEAK_DOWN:
			multi_tweak(0xf0u);
			break;

		case CH_TWEAK_UP:
			multi_tweak(0x10u);
			break;

		case CH_TWEAK_RIGHT:
			multi_tweak(1);
			break;

		case CH_TWEAK_LEFT:
			multi_tweak(0xffu);
			break;

		case CH_MYDOWN_SHIFTED:
			screen_down();
			break;

		case CH_MYDOWN:
			move_cursor_down();
			break;

		case CH_MYUP_SHIFTED:
			screen_up();
			break;

		case CH_MYUP:
			move_cursor_up();
			break;

		case CH_MYLEFT:
			if (CUR_COL)
			{
				--CUR_COL;
				refresh_cursor();
				refresh_mark();
			}
			break;

		case CH_MYRIGHT:
			if (CUR_COL < COLUMN_COUNT - 1)
			{
				++CUR_COL;
				refresh_cursor();
				refresh_mark();
			}
			break;
	}
	key_handler(CH_REFRESH);
}

U8 g_hexget_value;
static U8 s_hexget_active;
U8 get_hex_from_keyboard(U8 key, U8 max_value)
{
	const char* const str = "0123456789abcdef";
	const char* const pos = strchr(str, key);

	if (!pos)
	{
		// key is not a hexdec.
		if (s_hexget_active)
		{
			s_hexget_active = 0;
			return HEXGET_DONE;
		}
		return HEXGET_IDLE;
	}
	if (!s_hexget_active)
		g_hexget_value = 0;
	s_hexget_active = 1;

	g_hexget_value <<= 4;
	g_hexget_value |= pos - str;
	if (max_value < g_hexget_value)
		g_hexget_value &= 0xf;
	return HEXGET_ACTIVE;
}

U8 cursor_y_offset()
{
	return g_cur_box == SONG_BOX ? g_song_y_offset : 0;
}

void reset_boxes()
{
	U8 i = 0;
	for (; i < BOX_COUNT; ++i)
	{
		cur_cols[i] = 0;
		cur_rows[i] = 0;
	}
	g_song_y_offset = 0;
	switch_to_box(SONG_BOX);
	s_hexget_active = 0;
}