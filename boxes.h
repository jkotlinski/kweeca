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

#ifndef _BOXES_H
#define _BOXES_H

#include "types.h"
#include "box_data.h"

typedef enum {
	SONG_BOX,
	CHAIN_BOX,
	PHRASE_BOX,
	INSTR_BOX,
	BOX_COUNT
} BOX_ID;
extern U8 g_cur_box;

#define IS_BOX_ACTIVE(a) (a == g_cur_box)

void switch_to_box(BOX_ID id);

void boxes_handle_key(U8 key);

extern U8 cur_rows[BOX_COUNT];
#define CUR_ROW_SONG cur_rows[SONG_BOX]
#define CUR_ROW_PHRASE cur_rows[PHRASE_BOX]
#define CUR_ROW_CHAIN cur_rows[CHAIN_BOX]
extern U8 cur_cols[BOX_COUNT];
#define CUR_COL_SONG cur_cols[SONG_BOX]
#define CUR_COL_PHRASE cur_cols[PHRASE_BOX]
#define CUR_COL_CHAIN cur_cols[CHAIN_BOX]

#define CUR_COL cur_cols[g_cur_box]
#define CUR_ROW cur_rows[g_cur_box]
#define COLUMN_COUNT box_data[g_cur_box]->column_count

U8 cursor_y_offset();

extern const BoxData* g_cur_box_data;

// Get a hex from keyboard. Return status.
U8 get_hex_from_keyboard(U8 key, U8 max_value);
extern U8 g_hexget_value;
#define HEXGET_ACTIVE 0
#define HEXGET_DONE 1
#define HEXGET_IDLE 2

void mark();
void paste();
void cut_marked();

void reset_boxes();

#endif // _BOXES_H
