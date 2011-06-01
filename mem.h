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
