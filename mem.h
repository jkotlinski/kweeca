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

#ifndef _MEM_H
#define _MEM_H

#include "types.h"

#define CURR_DATA_VERSION 0

// It should be OK to use up to $d000.

#define MEM_START (U8*)0x6fff
#define DATA_VERSION (U8*)0x6fff
#define SONG_START (U8*)0x7000
#define SONG_END (U8*)0xa3a3
#define MEM_SIZE ((unsigned int)SONG_END - (unsigned int)MEM_START + 1)

// ---- SONG DATA START

#define PHRASE_COUNT 0x80u
#define PHRASE_NOTES (U8*)SONG_START
#define PHRASE_INSTRS (U8*)(SONG_START + 0x800u)
#define PHRASE_CMDS (U8*)(SONG_START + 0x1000u)
#define PHRASE_ARGS (U8*)(SONG_START + 0x1800u)

#define CHAIN_COUNT 0x80u
#define CHAIN_PHRASES (U8*)(SONG_START + 0x2000)
#define CHAIN_TRANSPOSES (U8*)(SONG_START + 0x2800)

#define SONG_ROW_COUNT 0x80u
#define SONG_CHAINS (U8*)(SONG_START + 0x3000)

#define INSTR_COUNT 0x20u
#define INSTR_START (SONG_START + 0x3180)

#define PHRASE_ALLOC_TABLE (U8*)(SONG_START + 0x3380)
#define CHAIN_ALLOC_TABLE (U8*)(SONG_START + 0x3390)
#define INSTR_ALLOC_TABLE (U8*)(SONG_START + 0x33a0)

// ---- SONG DATA END

#define SPRITE_OFFSET 0xd80u

void init_mem();

#endif // _MEM_H
