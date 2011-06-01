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

#include "mem.h"

#include <string.h>

#include "instr.h"

void init_mem()
{
    // Init phrases.
    memset(PHRASE_NOTES, 0xff, PHRASE_COUNT * 16);
    memset(PHRASE_INSTRS, 0xff, PHRASE_COUNT * 16);
    memset(PHRASE_CMDS, 0xff, PHRASE_COUNT * 16);
    memset(PHRASE_ARGS, 0, PHRASE_COUNT * 16);

    // Init chains. 
    memset(CHAIN_PHRASES, 0xff, CHAIN_COUNT * 16);
    memset(CHAIN_TRANSPOSES, 0, CHAIN_COUNT * 16);

    // Init song.   
    memset(SONG_CHAINS, 0xff, SONG_ROW_COUNT * 3);

    // Alloc tables.
    memset(PHRASE_ALLOC_TABLE, 0, PHRASE_COUNT / 8);
    memset(CHAIN_ALLOC_TABLE, 0, CHAIN_COUNT / 8);
    memset(INSTR_ALLOC_TABLE, 0, INSTR_COUNT / 8);

    {
        static const Instr k_default_instr = {
            0, // ad
            0, // hold
            0x80u, // sr
            0x80u, // pw
            0x40, // vctl
            0, // flt cutoff
            0xf, // flt volume
            0, // flt q
        };
        U8* dst = INSTR_START;
        U8 it = 0;
        for (; it < INSTR_COUNT; ++it)
        {
            memcpy(dst, &k_default_instr, sizeof(k_default_instr));
            dst += 0x10;
        }
    }
}

