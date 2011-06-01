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

#include "song.h"

#include <conio.h>

#include "alloc.h"
#include "border.h"
#include "boxes.h"
#include "chain.h"
#include "cursor.h"
#include "defines.h"
#include "global.h"
#include "mem.h"
#include "kbd.h"
#include "player.h"
#include "print.h"
#include "status.h"

#define SONG_BOX_WIDTH 9
#define SONG_BOX_HEIGHT 17

void set_chain(U8 val);
U8 song_handle_key(U8 key);
void lift_song_col();
void song_tweak_handler(U8 modval);
static void cursor_down();
static void cursor_up();
static void y_pos_validator();
static void draw_song();

static const U8 song_screen_col[] = { SONG_X + 1, SONG_X + 4, SONG_X + 7 };
static const U8 song_col_width[] = { 2, 2, 2 };
const BoxData k_song_box_data =
{
	3, // Column count.
	0, // Min lift col 2.
	song_screen_col,
	song_col_width,
	song_handle_key,
	cursor_down,
	cursor_up,
	song_tweak_handler,
	lift_song_col,
	set_chain,
	cursor_down,
	y_pos_validator,
	0, // refresh_cursor
	draw_song
};

U8 g_song_y_offset;

static void draw_row_number();
static void validate_window_start();
static void y_pos_validator()
{
	if (CUR_ROW_SONG < g_song_y_offset)
		g_song_y_offset = CUR_ROW_SONG;
	if (CUR_ROW_SONG >= g_song_y_offset + 0x10)
		g_song_y_offset = CUR_ROW_SONG - 0x10;
	validate_window_start();
	draw_row_number();
}

U8 song_y_offset()
{
	return g_song_y_offset;
}

static U8 last_chain[3];

static U8 title_color()
{
	return IS_BOX_ACTIVE(SONG_BOX) ? COLOR_TITLE : COLOR_INACTIVE;
}

static void draw_row_number()
{
	gotoxy(SONG_X + 6, SONG_Y);
	textcolor(title_color());
	print_hex(CUR_ROW_SONG);
}

static void draw_song_border()
{
	U8 active = IS_BOX_ACTIVE(SONG_BOX);
	draw_border(SONG_X, SONG_Y, SONG_X + SONG_BOX_WIDTH, SONG_Y + SONG_BOX_HEIGHT, active);

	gotoxy(SONG_X + 1, SONG_Y);
	textcolor(title_color());
	cputs("song");
	draw_row_number();
}

static U8 get_chain(U8 x, U8 y)
{
	return *(U8*)(SONG_CHAINS + y * 3 + x);
}

void set_chain(U8 val)
{
	U8 y = CUR_ROW_SONG - g_song_y_offset;
	*(U8*)(SONG_CHAINS + CUR_ROW_SONG * 3 + CUR_COL_SONG) = val;
	if (y & 0xf0u)
		return;
	TEXTCOLOR(SONG_BOX);
	gotoxy(SONG_X + 1 + CUR_COL_SONG * 3, SONG_Y + 1 + y);
	print_hex_ff(val);
}

static void draw_song_rows()
{
	U8 y = 0;
	TEXTCOLOR(SONG_BOX);
	for (; y < 16; ++y)
	{
		const U8 src_y = y + g_song_y_offset;
		gotoxy(SONG_X + 1, SONG_Y + 1 + y);
		print_hex_ff(get_chain(0, src_y));
		cputc(' ');
		print_hex_ff(get_chain(1, src_y));
		cputc(' ');
		print_hex_ff(get_chain(2, src_y));
	}
}

static void draw_song()
{
	draw_song_border();
	draw_song_rows();
}

static U8 get_cur_chain()
{
	return get_chain(CUR_COL_SONG, CUR_ROW_SONG);
}

static void show_chain()
{
	U8 chain = get_cur_chain();
	if (0xffu == chain)
		return;
	view_chain(chain);
}

static void release_space_handler()
{
	alloc_chain(get_cur_chain());
	release_space_handler_p = 0;
}

static void new_or_clone(U8 do_new)
{
	U8 chain;
	if (do_new)
	{
		// No chain on step, alloc new.
		chain = next_new_chain();
	}
	else
	{
		// Clone existing chain.
		chain = clone_chain();
	}
	if (chain == 0xffu)
		return;
	last_chain[EDIT_CH] = chain;
	alloc_chain(chain);
	set_chain(chain);
	show_chain();
}

static void handle_space()
{
	static U8 s_first_tap_was_on_empty_step;
	U8 chain = get_cur_chain();
	release_space_handler_p = release_space_handler;

	if (g_marking) return;

	switch (g_space_tap_count)
	{
		case 1:
			s_first_tap_was_on_empty_step = (0xffu == chain);

			if (s_first_tap_was_on_empty_step)
			{
				set_chain(last_chain[EDIT_CH]);
				show_chain();
			}
			else 
			{
				last_chain[EDIT_CH] = chain;
			}
			break;

		case 2:
			new_or_clone(s_first_tap_was_on_empty_step);
			break;
	}
}

static void pull_up_below_chains()
{
	U8 prev_row = CUR_ROW_SONG;
	while (++CUR_ROW_SONG < SONG_ROW_COUNT)
	{
		U8 chain = get_cur_chain();
		--CUR_ROW_SONG;
		set_chain(chain);
		++CUR_ROW_SONG;
	}
	CUR_ROW_SONG = prev_row;
	show_chain();
}

static void cut_step()
{
	U8 chain = get_cur_chain();
	if (0xffu == chain)
	{
		pull_up_below_chains();
		return;
	}
	last_chain[EDIT_CH] = chain;
	g_clipboard[0][0] = chain;
	g_clip_x1 = 0;
	g_clip_x2 = 0;
	g_clip_y1 = 0;
	g_clip_y2 = 0;
	g_clip_type = SONG_BOX;
	set_chain(0xffu);
}

// Return true if we stole the key.
static U8 handle_chain_entry(U8 key)
{
	U8 status = get_hex_from_keyboard(key, CHAIN_COUNT - 1);
	switch (status)
	{
		case HEXGET_DONE:
			last_chain[EDIT_CH] = g_hexget_value;
			alloc_chain(g_hexget_value);
			// Fall through...

		case HEXGET_ACTIVE:
			set_chain(g_hexget_value);
			show_chain();
	}
	return status == HEXGET_ACTIVE;
}

void handle_song_box_enter()
{
	if (PLAYMODE != PLAYMODE_SONG)
		player_stop();
	if (!g_playing)
	{
		play_song(CUR_ROW_SONG);
		PLAYMODE = PLAYMODE_SONG;
		g_playing = 1;
	}
	else
		player_stop();
}

U8 song_handle_key(U8 key)
{
	if (handle_chain_entry(key))
		return 1;

	switch (key)
	{
		case CH_REFRESH:
			show_chain();
			break;

		case CH_ENTER:
			handle_song_box_enter();
			break;

		case CH_SCREEN_RIGHT:
			switch_to_box(CHAIN_BOX);
			break;

		case ' ':
			handle_space();
			break;

		case CH_INS:
		case 'v':
			paste();
			break;

		case CH_DEL:
		case 'x':
			if (g_marking)
				cut_marked();
			else
				cut_step();
			break;

		case 'm':
			mark();
			break;

		default:
			return 0;
	}
	return 1;
}

void lift_song_col()
{
	g_clipboard[CUR_COL_SONG][CUR_ROW_SONG] = get_cur_chain();
	set_chain(0xffu);
}

void song_tweak_handler(U8 modval)
{
	U8 chain = get_cur_chain();
	if (0xffu == chain)
		return;
	chain += modval;
	if (chain >= 0xf0u)
		chain = 0;
	else if (chain >= CHAIN_COUNT)
		chain = CHAIN_COUNT - 1;
	set_chain(chain);
	if (!g_marking) show_chain();
	last_chain[EDIT_CH] = chain;
}

static void validate_window_start()
{
#define WINDOW_MARGIN 2
	const U8 offset = CUR_ROW_SONG - g_song_y_offset;
	if (offset >= 0x10 - WINDOW_MARGIN)
	{
#define MAX_g_song_y_offset (SONG_ROW_COUNT - 0x10)
		if (g_song_y_offset < MAX_g_song_y_offset)
		{
			++g_song_y_offset;
			draw_song_rows();
		}
	}
	else if (offset < WINDOW_MARGIN && g_song_y_offset)
	{
		--g_song_y_offset;
		draw_song_rows();
	}
}

static void cursor_down()
{
	if (CUR_ROW_SONG < SONG_ROW_COUNT - 1)
	{
		++CUR_ROW_SONG;
		draw_row_number();
		validate_window_start();
	}
}

static void cursor_up()
{
	if (CUR_ROW_SONG)
	{
		--CUR_ROW_SONG;
		draw_row_number();
		validate_window_start();
	}
}