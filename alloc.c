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
