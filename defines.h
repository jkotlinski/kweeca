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

#ifndef _DEFINES_H
#define _DEFINES_H

// View locations - start.
#define PHRASE_X 14 
#define PHRASE_Y 0

#define CHAIN_X 8
#define CHAIN_Y 0

#define SONG_X -1
#define SONG_Y 0
// View locations - end.

#define EDIT_CH CUR_COL_SONG

#define MAX_NOTE 95

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

#define CH_LEFTARROW 0x5f

#define COLOR_TITLE COLOR_GREEN
#define COLOR_INACTIVE COLOR_GRAY1
#define TEXTCOLOR(box) textcolor((g_cur_box == box) ? COLOR_WHITE : COLOR_INACTIVE)

#define VERSION "0.0.0"

#endif // _DEFINES_H
