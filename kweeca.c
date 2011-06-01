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

#include <c64.h>

#include <conio.h>
#include <string.h>

#include "boxes.h"
#include "cursor.h"
#include "defines.h"
#include "file.h"
#include "global.h"
#include "kbd.h"
#include "mem.h"
#include "player.h"
#include "status.h"

#include "instr.h"

void wait_frame()
{
	while (VIC.rasterline != 0x40);
	while (VIC.rasterline == 0x40);
}

void beep()
{
	SID.v1.ctrl = 0x41;
	SID.v2.ctrl = 0x41;
}

void mute()
{
	SID.v1.ctrl = 0x40;
	SID.v2.ctrl = 0x40;
}

void init()
{
	clrscr();
	player_init();
	init_mem();
	startirq();
	*(U8*)0xd018 = 0x14; // Use upper case + gfx chars.
	VIC.bgcolor0 = COLOR_BLACK;
	VIC.bordercolor = COLOR_BLACK;
	*(U8*)0x28a = 0x80; // Repeat all keys.
	textcolor(COLOR_YELLOW);
	reset_boxes();
	show_status_msg(MSG_HELLO);
	init_play_indicators();
}

#include "print.h"

void handle_key(U8 key)
{
	switch (key)
	{
		case CH_F5:
			load_song();
			reset_boxes();
			return;

		case CH_F7:
			save_song();
			return;
	}

	boxes_handle_key(key);
	reset_blink();
}

void tick()
{
	U8 prev_key_space = KEY_SPACE;
	if (kbhit())
	{
		const U8 ch = cgetc();
		const U8 is_repeated_space = (ch == ' ' && KEY_SPACE);
		if (!is_repeated_space)
		{
			handle_key(ch);
		}
	}
	poll_special_keys();
	if (prev_key_space && !KEY_SPACE && release_space_handler_p)
		release_space_handler_p();
	cursor_tick();
	play_indicator_tick();
	status_tick();
}

int main()
{
	init();

	for (;;)
	{
		while (!g_ticks); // Wait for tick.
		tick();
		g_ticks--; // Only decreased here.
	}
	return 0;
}

