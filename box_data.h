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
