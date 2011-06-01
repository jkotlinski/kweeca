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

#include "phrase.h"

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "boxes.h"
#include "border.h"
#include "chain.h"
#include "cursor.h"
#include "defines.h"
#include "global.h"
#include "instr.h"
#include "kbd.h"
#include "mem.h"
#include "myassert.h"
#include "print.h"
#include "player.h"
#include "song.h"
#include "types.h"

#define COL_COUNT 4

#define NOTE_COL 0
#define INSTR_COL 1
#define CMD_COL 2
#define ARG_COL 3

#define PHRASE_BOX_WIDTH 10
#define PHRASE_BOX_HEIGHT 17

const char commands[] = "dfg";
#define CMD_COUNT 3

const U8 k_cmd_max_arg[] = {
	0xf, // delay
	0xff, // filter cutoff
	0xff // groove
};

static void print_cmd(U8 cmd)
{
	assert(cmd < strlen(commands));
	putchar(commands[cmd]);
}

U8 phrase_handle_key(U8 key);
void lift_phrase_col();
void paste_phrase_col(U8 val);
void phrase_tweak_handler(U8 modval);
void phrase_cursor_oob_down_handler();
void phrase_cursor_oob_up_handler();
static void cursor_down();
static void cursor_up();
static void draw_phrase();

static const U8 phrase_screen_col[] = 
{ 
	PHRASE_X + 1 + 0, 
	PHRASE_X + 1 + 3, 
	PHRASE_X + 1 + 6, 
	PHRASE_X + 1 + 7 
};
static const U8 phrase_col_width[] = { 2, 2, 1, 2 };
const BoxData k_phrase_box_data =
{
	4, // Column count.
	1, // Min lift col 2.
	phrase_screen_col,
	phrase_col_width,
	phrase_handle_key,
	cursor_down,
	cursor_up,
	phrase_tweak_handler,
	lift_phrase_col,
	paste_phrase_col,
	0, // Paste advance row fn.
	0, // Y pos validator.
	0, // refresh_cursor
	draw_phrase
};

U8 VIEW_PHRASE;
static U16 PHRASE_OFFSET;

#define C3 0x24
#define B7 0x5f
static U8 last_input_note[3] = { C3, C3, C3 };
static U8 last_input_instr[3];
static U8 last_input_cmd[3];
static U8 last_input_arg[3];
// static U8 last_input_cmd[3];
// static U8 last_input_arg[3];

static U8 get_note_from_row(U8 y)
{
	return *(U8*)(PHRASE_NOTES + PHRASE_OFFSET + y);
}

static U8 get_note()
{
	return get_note_from_row(CUR_ROW_PHRASE);
}

static U8 get_instr_on_row(U8 y)
{
	return *(U8*)(PHRASE_INSTRS + PHRASE_OFFSET + y);
}

static U8 get_instr()
{
	return get_instr_on_row(CUR_ROW_PHRASE);
}

static U8 get_cmd_on_row(U8 y)
{
	return *(U8*)(PHRASE_CMDS + PHRASE_OFFSET + y);
}

static U8 get_cmd()
{
	return get_cmd_on_row(CUR_ROW_PHRASE);
}

static U8 get_arg_on_row(U8 y)
{
	return *(U8*)(PHRASE_ARGS + PHRASE_OFFSET + y);
}

static U8 get_arg()
{
	return get_arg_on_row(CUR_ROW_PHRASE);
}

static U8 cur_screen_row()
{
	return PHRASE_Y + 1 + CUR_ROW_PHRASE;
}

static void draw_phrase_border()
{
	U8 active = IS_BOX_ACTIVE(PHRASE_BOX);
	draw_border(PHRASE_X, PHRASE_Y, PHRASE_X + PHRASE_BOX_WIDTH, PHRASE_Y + PHRASE_BOX_HEIGHT, active);

	gotoxy(PHRASE_X + 1, PHRASE_Y);
	if (active) textcolor(COLOR_GREEN);
	cputs("phrase ");
	print_hex(VIEW_PHRASE);
}

static void print_cmd_with_arg(U8 row)
{
	const U8 cmd = get_cmd_on_row(row);

	gotoxy(phrase_screen_col[CMD_COL], PHRASE_Y + 1 + row);

	if (cmd == 0xffu)
	{
		puts("---");
		return;
	}
	print_cmd(cmd);
	{
		const U8 arg = get_arg_on_row(row);
		print_hex(arg);
	}
}

static void draw_phrase()
{
	U8 y;
	draw_phrase_border();
	TEXTCOLOR(PHRASE_BOX);
	for (y = 0; y < 16; ++y)
	{
		U8 instr = get_instr_on_row(y);
		gotoxy(PHRASE_X + 1, PHRASE_Y + 1 + y);
		print_note(get_note_from_row(y));
		print_hex_ff(instr);
		putchar(' ');
		print_cmd_with_arg(y);
	}
}

static void show_instr()
{
	U8 instr = get_instr();
	if (0xffu == instr)
		return;
	view_instr(instr);
}

void view_phrase(U8 phrase)
{
	VIEW_PHRASE = phrase;
	PHRASE_OFFSET = phrase * 16;
	draw_phrase();
	show_instr();
}

// Set note at CUR_ROW_PHRASE.
static void set_note(U8 note)
{
	assert(note >= 0xfe || note <= MAX_NOTE);
	*(U8*)(PHRASE_NOTES + PHRASE_OFFSET + CUR_ROW_PHRASE) = note;
	gotoxy(phrase_screen_col[NOTE_COL], cur_screen_row());
	print_note(get_note());
}

static void set_instr(U8 instr)
{
	*(U8*)(PHRASE_INSTRS + PHRASE_OFFSET + CUR_ROW_PHRASE) = instr;
	gotoxy(phrase_screen_col[INSTR_COL], cur_screen_row());
	print_hex_ff(instr);
}

static void set_cmd(U8 cmd)
{
	*(U8*)(PHRASE_CMDS + PHRASE_OFFSET + CUR_ROW_PHRASE) = cmd;
	print_cmd_with_arg(CUR_ROW_PHRASE);
}

static void set_arg(U8 arg)
{
	*(U8*)(PHRASE_ARGS + PHRASE_OFFSET + CUR_ROW_PHRASE) = arg;
	print_cmd_with_arg(CUR_ROW_PHRASE);
}

static void delete()
{
	U8 tmp;
	switch (CUR_COL_PHRASE)
	{
		case NOTE_COL:
			tmp = get_note();
			if (tmp != 0xff)
				last_input_note[EDIT_CH] = tmp;
			set_note(0xff);
			// Fall through.
			
		case INSTR_COL:
			tmp = get_instr();
			if (tmp != 0xff)
				last_input_instr[EDIT_CH] = tmp;
			set_instr(0xff);
			break;

		case CMD_COL:
			tmp = get_cmd();
			if (tmp != 0xff)
				last_input_cmd[EDIT_CH] = tmp;
			set_cmd(0xff);
			// Fall through.

		case ARG_COL:
			tmp = get_arg();
			if (tmp != 0xff)
				last_input_arg[EDIT_CH] = tmp;
			set_arg(0);
			break;

		default:
			assert(0);
	}
}

const U8* note_keys = "q2w3er5t6y7ui9o0";
const U8* lower_note_keys = "zsxdcvgbhnjm,l.";

// Return 1 if we stole key.
static U8 handle_cmd_key(U8 key)
{
	U8* ptr = strchr(commands, key);
	if (!ptr)
		return 0;
	set_cmd(ptr - commands);
	return 1;
}

// Return 1 if we stole key.
static U8 handle_note_key(U8 key)
{
	U8* ptr;
	U8 note;

	if (ptr = strchr(note_keys, key))
	{
		note = ptr - note_keys;
	}
	else if (ptr = strchr(lower_note_keys, key))
	{
		note = ptr - lower_note_keys - 12;
	}
	else
		return 0;

	note += g_octave[EDIT_CH] * 12;
	if (note > MAX_NOTE) 
		return 1;

	set_note(note);

	if (get_instr() == 0xff)
	{
		U8 instr = last_input_instr[EDIT_CH];
		set_instr(instr);
	}

	move_cursor_down();
	return 1;
}

static void touch()
{
	if (g_marking) return;

	switch (CUR_COL_PHRASE)
	{
		case NOTE_COL:
			{
				U8 note = get_note();
				if (0xffu == note)
					set_note(last_input_note[EDIT_CH]);
				else
				{
					last_input_note[EDIT_CH] = note;
					set_note(0xffu);
				}
			}
			// Fall through!

		case INSTR_COL:
			{
				U8 instr = get_instr();
				if (0xffu == instr)
				{
					set_instr(last_input_instr[EDIT_CH]);
					show_instr();
				}
				else
				{
					if ((CUR_COL_PHRASE == NOTE_COL) && (0xffu != get_note()))
						break; // Don't clear instr when inserting a note on blank step.
					last_input_instr[EDIT_CH] = instr;
					set_instr(0xffu);
				}
			}
			break;

		case CMD_COL:
			if (0xffu == get_cmd())
			{
				set_cmd(last_input_cmd[EDIT_CH]);
				set_arg(last_input_arg[EDIT_CH]);
			}
			break;

		case ARG_COL:
			break;
		
		default:
			assert(0); // TODO
	}
}

void phrase_tweak_handler(U8 modval)
{
	switch (CUR_COL_PHRASE)
	{
		case NOTE_COL:
			{
				U8 note = get_note();

				if (modval == 0x10) modval = 0xc;
				else if (modval == 0xf0u) modval = 0xf4u;

				if (0xffu == note)
				{
					if (g_marking) return;
					note = last_input_note[EDIT_CH];
					set_instr(last_input_instr[EDIT_CH]);
				}
				note += modval;
				if (note >= 0xf0u)
					note += MAX_NOTE + 1;
				else if (note > MAX_NOTE)
					note -= MAX_NOTE + 1;
				set_note(note);
				last_input_note[EDIT_CH] = note;

				gotoxy(0, 0);
				print_hex(note);

				g_octave[EDIT_CH] = note / 12;
			}
			break;

		case INSTR_COL:
			{
				U8 instr = get_instr();
				if (0xffu == instr)
					instr = last_input_instr[EDIT_CH];
				instr += modval;
				if (instr >= INSTR_COUNT)
					break;
				set_instr(instr);
				last_input_instr[EDIT_CH] = instr;
			}
			break;

		case CMD_COL:
			{
				S8 cmd = get_cmd();
				if (0xffu == cmd)
					cmd = last_input_cmd[EDIT_CH];
				cmd += modval;
				if (cmd < 0) cmd = 0;
				else if (cmd >= CMD_COUNT)
					cmd = CMD_COUNT - 1;
				set_cmd(cmd);
				last_input_cmd[EDIT_CH] = cmd;
			}
			break;

		case ARG_COL:
			{
				U8 arg = get_arg();
				arg += modval;
				arg &= k_cmd_max_arg[get_cmd()];
				set_arg(arg);
				last_input_arg[EDIT_CH] = arg;
			}
			break;

		default:
			assert(0); // TODO
	}
}

static U8 handle_instr_entry(U8 key)
{
	U8 status = get_hex_from_keyboard(key, INSTR_COUNT - 1);
	if (status != HEXGET_IDLE)
	{
		last_input_instr[EDIT_CH] = g_hexget_value;
		set_instr(g_hexget_value);
		show_instr();
	}
	return status == HEXGET_ACTIVE;
}

static U8 handle_arg_entry(U8 key)
{
	U8 status = get_hex_from_keyboard(key, k_cmd_max_arg[get_cmd()]);
	STATIC_ASSERT(sizeof(k_cmd_max_arg) == CMD_COUNT);
	if (status != HEXGET_IDLE)
	{
		last_input_arg[EDIT_CH] = g_hexget_value;
		set_arg(g_hexget_value);
	}
	return status == HEXGET_ACTIVE;
}

// Return 1 if key was handled.
static U8 column_key_handler(U8 key)
{
	U8 did_handle_key;
	switch (CUR_COL_PHRASE)
	{
		case INSTR_COL:
			did_handle_key = handle_instr_entry(key);
			break;
		case NOTE_COL:
			did_handle_key = handle_note_key(key);
			break;
		case CMD_COL:
			did_handle_key = handle_cmd_key(key);
			break;
		case ARG_COL:
			did_handle_key = handle_arg_entry(key);
			break;
		default:
			assert(0);
	}
	return did_handle_key;
}

U8 phrase_handle_key(U8 key)
{
	if (column_key_handler(key))
		return 1;

	switch (key)
	{
		case CH_REFRESH:
			show_instr();
			break;

		case CH_ENTER | CH_SHIFT:
			handle_song_box_enter();
			break;

		case CH_ENTER:
			if (PLAYMODE != PLAYMODE_PHRASE)
				player_stop();
			if (!g_playing)
			{
				player_init();
				PCC = EDIT_CH;
				play_phrase(VIEW_PHRASE);
				PLAYMODE = PLAYMODE_PHRASE;
				g_playing = 1;
			}
			else
				player_stop();
			break;

		case CH_SCREEN_LEFT:
			switch_to_box(CHAIN_BOX);
			break;

		case CH_SCREEN_RIGHT:
			switch_to_box(INSTR_BOX);
			break;

		case ' ':
			touch();
			break;

		case CH_INS:
		case 'V':
			paste();
			break;

		case 'M':
			mark();
			break;

		case '+':
			if (g_octave[EDIT_CH] < 7)
				++g_octave[EDIT_CH];
			break;

		case '-':
			if (g_octave[EDIT_CH] > 1)
				--g_octave[EDIT_CH];
			break;

		case CH_DEL:
		case 'X':
			if (g_marking)
				cut_marked();
			else
			{
				delete();
				move_cursor_down();
			}
			break;

		default:
			return 0;
	}
	return 1;
}

void lift_phrase_col()
{
	switch (CUR_COL_PHRASE)
	{
		case NOTE_COL:
			g_clipboard[NOTE_COL][CUR_ROW_PHRASE] = get_note();
			set_note(0xffu);
			break;
		case INSTR_COL:
			g_clipboard[INSTR_COL][CUR_ROW_PHRASE] = get_instr();
			set_instr(0xffu);
			break;
		case CMD_COL:
			g_clipboard[CMD_COL][CUR_ROW_PHRASE] = get_cmd();
			set_cmd(0xffu);
			break;
		case ARG_COL:
			g_clipboard[ARG_COL][CUR_ROW_PHRASE] = get_arg();
			set_arg(0);
	}
}

void paste_phrase_col(U8 val)
{
	switch (CUR_COL_PHRASE)
	{
		case NOTE_COL:
			set_note(val);
			break;
		case INSTR_COL:
			set_instr(val);
			break;
		case CMD_COL:
			set_cmd(val);
			break;
		case ARG_COL:
			set_arg(val);
	}
}

static void handle_oob_down()
{
	U8 next_phrase = get_next_phrase();
	if (0xffu == next_phrase)
		return;
	view_phrase(next_phrase);
	++CUR_ROW_CHAIN;
	CUR_ROW_PHRASE = 0;
}

static void handle_oob_up()
{
	U8 prev_phrase = get_prev_phrase();
	if (0xffu == prev_phrase)
		return;
	view_phrase(prev_phrase);
	--CUR_ROW_CHAIN;
	CUR_ROW_PHRASE = 0xf;
}

static void cursor_down()
{
	if (CUR_ROW_PHRASE < 15)
		++CUR_ROW_PHRASE;
	else
		handle_oob_down();
}

static void cursor_up()
{
	if (CUR_ROW_PHRASE)
		--CUR_ROW_PHRASE;
	else
		handle_oob_up();
}
