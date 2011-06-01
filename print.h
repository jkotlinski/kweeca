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

#ifndef _PRINT_H
#define _PRINT_H

#include "types.h"

void print_nibble(U8 val);
void print_hex(U8 val);
void print_note(U8 note);
void print_hex_ff(U8 val); // Print "--" if val == 0xff, otherwise hex.

void paint_box(U8 x1, U8 y1, U8 x2, U8 y2, U8 inverse);

#endif // _PRINT_H

