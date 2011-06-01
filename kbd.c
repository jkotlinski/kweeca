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

#include "kbd.h"

#include <cbm.h>

#include "global.h"

U8 KEY_SHIFT;
U8 KEY_SPACE;

void handle_key(U8 key); // defined in kweeca.c

#define ROW(i) CIA1.pra = 0xffu - (1 << i)
#define COL(i) !(CIA1.prb & (1 << i))

void poll_special_keys()
{
	U8 right_shift;
	static U8 s_last_tick_was_tweak;
	
	ROW(6);
	right_shift = COL(4);

	ROW(1);
	KEY_SHIFT = right_shift || COL(7); // left shift
	
	ROW(7);
	KEY_SPACE = COL(4);

	if (KEY_SPACE)
	{
		U8 tweak_key = 0;
		ROW(5);
		if (COL(5)) tweak_key = CH_TWEAK_LEFT; // :
		else if (COL(6)) tweak_key = CH_TWEAK_UP; // @
		else {
			ROW(6);
			if (COL(2)) tweak_key = CH_TWEAK_DOWN; // ;
			else if (COL(5)) tweak_key = CH_TWEAK_RIGHT; // =
		}
      
		if (tweak_key)
		{
			if (!s_last_tick_was_tweak) // No key repeat for now, change this later?
			{
				handle_key(tweak_key);
				s_last_tick_was_tweak = 1;
			}
		}
		else
			s_last_tick_was_tweak = 0;
	}
	else
		s_last_tick_was_tweak = 0;
	
	CIA1.pra = 0;
}


