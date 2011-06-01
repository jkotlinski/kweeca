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

#ifndef _CHAIN_H
#define _CHAIN_H

#include "types.h"
#include "box_data.h"

extern U8 VIEW_CHAIN;

void view_chain(U8 chain);

U8 get_prev_phrase();
U8 get_next_phrase();

// Clone VIEW_CHAIN. Return number of new chain, or 0xff if out of mem.
U8 clone_chain();

extern const BoxData k_chain_box_data;

#endif // _CHAIN_H
