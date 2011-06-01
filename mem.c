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

