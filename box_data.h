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

#ifndef _BOX_DATA_H
#define _BOX_DATA_H

#include "types.h"

typedef void (*geometry_fn)();
typedef U8 (*handler_fn)(U8);
typedef void (*cur_down_handler_fn)(void);
typedef void (*cur_up_handler_fn)(void);
typedef void (*tweak_handler_fn)(U8);
typedef void (*lift_col_fn)();
typedef void (*paste_col_fn)(U8);
typedef void (*paste_advance_row_fn)();
typedef void (*y_pos_validator_fn)();
typedef void (*refresh_cursor_fn)();
typedef void (*redraw_fn)();

typedef struct {
	const U8 column_count;
	const U8 min_allowed_lift_col_2;
	const U8* column_x;
	const U8* column_w;
	const handler_fn key_handler;
	const cur_down_handler_fn cur_down_handler;
	const cur_up_handler_fn cur_up_handler;
	const tweak_handler_fn tweak_handler;
	const lift_col_fn lift_col_handler;
	const paste_col_fn paste_col_handler;
	const paste_advance_row_fn paste_advance_row;
	const y_pos_validator_fn y_pos_validator;
	const refresh_cursor_fn refresh_cursor;
	const redraw_fn redraw;
} BoxData;

#endif
