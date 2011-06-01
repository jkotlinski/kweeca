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

#ifndef _CURSOR_H
#define _CURSOR_H

#include "types.h"
#include "box_data.h"

void blink_cursor();
void reset_blink();

void move_cursor_down();
void move_cursor_up();
void move_cursor(U8 col, U8 row, U8 w);

void cursor_tick();

// Marking.
void setup_marking(const BoxData* box_data);
void refresh_mark();
void clear_mark();

// Play indicators.
void init_play_indicators();
void play_indicator_tick();

#endif // _CURSOR_H
