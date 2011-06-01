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

#ifndef _ALLOC_H
#define _ALLOC_H

#include "types.h"

// Return chain number if there is an unused chain, otherwise 0xff.
U8 next_new_chain();
void alloc_chain(U8 chain);

// Return phrase number if there is an unused phrase, otherwise 0xff.
U8 next_new_phrase();
void alloc_phrase(U8 phrase);

U8 next_new_instr();
void alloc_instr(U8 instr);

// -- Cloning

// Return number of new chain, or 0xff if out of mem.
U8 clone_phrase(U8 phrase);

#endif // _ALLOC_H
