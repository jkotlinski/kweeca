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

#include "alloc.h"
#include "mem.h"
#include "myassert.h"
#include "status.h"

static U8 get_index(U8* bits, U8 index)
{
    U8* byte = &bits[index / 8];
    U8 mask = 1 << (index % 8);
    return *byte & mask;
}

static void set_index(U8* bits, U8 index)
{
    U8* byte = &bits[index / 8];
    U8 mask = 1 << (index % 8);
    *byte |= mask;
}

static U8 first_unset_bit(U8* bits, U8 count)
{
    U8 i;
    for (i = 0; i < count; ++i)
        if (!get_index(bits, i))
            return i;
    show_status_msg(MSG_ALLOC_OUT_OF_RAM);
    return 0xffu;
}

U8 next_new_phrase()
{
    return first_unset_bit(PHRASE_ALLOC_TABLE, PHRASE_COUNT);
}

void alloc_phrase(U8 phrase)
{
    if (phrase == 0xffu) return;
    assert(phrase < PHRASE_COUNT);
    set_index(PHRASE_ALLOC_TABLE, phrase);
}

void alloc_chain(U8 chain)
{
    if (chain == 0xffu) return;
    assert(chain < CHAIN_COUNT);
    set_index(CHAIN_ALLOC_TABLE, chain);
}

U8 next_new_chain()
{
    return first_unset_bit(CHAIN_ALLOC_TABLE, CHAIN_COUNT);
}

void alloc_instr(U8 instr)
{
    if (instr == 0xffu) return;
    assert(instr < INSTR_COUNT);
    set_index(INSTR_ALLOC_TABLE, instr);
}

U8 next_new_instr()
{
    return first_unset_bit(INSTR_ALLOC_TABLE, INSTR_COUNT);
}

// Cloning.
#include <string.h>
#include "mem.h"

// Assumes that src, dst are binary or:able with base.
#define MOVE16(base, dst, src) \
    memcpy((void*)((U16)base | dst), \
            (void*)((U16)base | src), \
            16);

U8 clone_phrase(U8 src_phrase)
{
    U16 src_offset = src_phrase * 16;
    U16 dst_offset;
    U8 new_phrase = next_new_phrase();
    if (new_phrase == 0xffu)
        return 0xffu;
    alloc_phrase(new_phrase);
    dst_offset = new_phrase * 16;

    MOVE16(PHRASE_NOTES, dst_offset, src_offset);
    MOVE16(PHRASE_INSTRS, dst_offset, src_offset);
    MOVE16(PHRASE_CMDS, dst_offset, src_offset);
    MOVE16(PHRASE_ARGS, dst_offset, src_offset);

    return new_phrase;
}
