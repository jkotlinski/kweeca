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

#include "player.h"

#include "instr.h"
#include "types.h"
#include "mem.h"
#include "myassert.h"
#include "print.h"
#include "phrase.h"
#include "chain.h"

U8* SONG_CHAIN_PTR[3];
U8 ACTIVE_PHRASE[3];
U8 ACTIVE_CHAIN[3];

extern U8 PCC7;

static void rewind_song_step();

static void play_pcc_chain()
{
	if (0xffu == *SONG_CHAIN_PTR[PCC])
	{
		rewind_song_step();
	}
	play_chain(*(SONG_CHAIN_PTR[PCC]), 0);
}

void play_song(U8 song_row)
{
	SONG_CHAIN_PTR[0] = SONG_CHAINS + song_row * 3;
	SONG_CHAIN_PTR[1] = SONG_CHAIN_PTR[0] + 1;
	SONG_CHAIN_PTR[2] = SONG_CHAIN_PTR[1] + 1;
	PCC = 0;
	play_pcc_chain();
	++PCC;
	play_pcc_chain();
	++PCC;
	play_pcc_chain();
}

U8 CHAIN_PHRASE_PTR_LSB[3];
static U8 CHAIN_PHRASE_PTR_MSB[3];
static U8 CHAIN_TSP_PTR_LSB[3];
static U8 CHAIN_TSP_PTR_MSB[3];

static U8 fetch_pcc_phrase()
{
	return *(U8*)((CHAIN_PHRASE_PTR_MSB[PCC] << 8) | CHAIN_PHRASE_PTR_LSB[PCC]);
}

U8 CHAIN_TRANSPOSE[3];
static void fetch_curr_chain_transpose()
{
	CHAIN_TRANSPOSE[PCC] = *(U8*)((CHAIN_TSP_PTR_MSB[PCC] << 8) | CHAIN_TSP_PTR_LSB[PCC]);
}

void play_chain(U8 chain, U8 row)
{
	U8 lsn = (chain << 4) + row;
	U8 msn = chain >> 4;
	assert(row < 16);
	ACTIVE_CHAIN[PCC] = chain;
	if (0xffu == chain)
		return;
	CHAIN_PHRASE_PTR_LSB[PCC] = lsn;
	CHAIN_TSP_PTR_LSB[PCC] = lsn;
	CHAIN_PHRASE_PTR_MSB[PCC] = ((U16)CHAIN_PHRASES >> 8) + msn;
	CHAIN_TSP_PTR_MSB[PCC] = ((U16)CHAIN_TRANSPOSES >> 8) + msn;
	fetch_curr_chain_transpose();

	play_phrase(fetch_pcc_phrase());
}

void play_phrase(U8 phrase)
{
	U8 lsn = phrase << 4;
	U8 msn = phrase >> 4;
	ACTIVE_PHRASE[PCC] = phrase;
	if (0xffu == phrase)
		return;
	CH_ACTIVE[PCC] = 1;
	PHRASE_NOTE_PTR_LSB[PCC] = lsn;
	PHRASE_INSTR_PTR_LSB[PCC] = lsn;
	PHRASE_CMD_PTR_LSB[PCC] = lsn;
	PHRASE_ARG_PTR_LSB[PCC] = lsn;
	PHRASE_NOTE_PTR_MSB[PCC] = ((U16)PHRASE_NOTES >> 8) + msn;
	PHRASE_INSTR_PTR_MSB[PCC] = ((U16)PHRASE_INSTRS >> 8) + msn;
	PHRASE_CMD_PTR_MSB[PCC] = ((U16)PHRASE_CMDS >> 8) + msn;
	PHRASE_ARG_PTR_MSB[PCC] = ((U16)PHRASE_ARGS >> 8) + msn;
}

// ---- called from asm

#include "global.h"
#include <conio.h>
#include <stdio.h>

// Return 1 if we hit chain end, otherwise 0.
static U8 move_to_next_chain_step()
{
	U8* phrase_ptr = &CHAIN_PHRASE_PTR_LSB[PCC];
	U8 played_past_end = !(0xf & (++*phrase_ptr));
	U8 new_phrase = fetch_pcc_phrase();
	if (played_past_end || new_phrase == 0xffu)
	{
		return 1;
	}
	++CHAIN_TSP_PTR_LSB[PCC];
	fetch_curr_chain_transpose();
	return 0;
}

static void rewind_song_step()
{
	// TODO Maybe this can be optimized.
	U8* ptr = SONG_CHAIN_PTR[PCC];
	for (;;)
	{
		if (ptr - 3 < SONG_CHAINS) break; // Can't rewind further.
		if (*ptr != 0xffu && *(ptr - 3) == 0xffu) break; // Can't rewind further.
		ptr -= 3;
	}
	SONG_CHAIN_PTR[PCC] = ptr;
}

static void advance_step_song()
{
	// TODO - handle empty chains somewhere in this function.
	if (move_to_next_chain_step())
	{
		// Failed - reached chain end. Must find a new chain.
		
		SONG_CHAIN_PTR[PCC] += 3;
		if (0xffu == *SONG_CHAIN_PTR[PCC])
		{
			rewind_song_step();
		}

		play_pcc_chain();
		return;
	}
	play_phrase(fetch_pcc_phrase());
}

void advance_step()
{
	U8* note_ptr = &PHRASE_NOTE_PTR_LSB[PCC];

	// Advance note ptr.
	++*note_ptr;

	// Did we move past phrase end?
	if (0xf & *note_ptr)
	{
		// No, we're still in the same phrase. Advance the other ptrs accordingly.
		++PHRASE_INSTR_PTR_LSB[PCC];
		++PHRASE_CMD_PTR_LSB[PCC];
		++PHRASE_ARG_PTR_LSB[PCC];
	}
	else
	{
		// Yes, we moved past phrase... do something depending on playmode.
		switch (PLAYMODE)
		{
			case PLAYMODE_PHRASE:
				play_phrase(VIEW_PHRASE);
				break;

			case PLAYMODE_CHAIN:
				if (move_to_next_chain_step())
				{
					// Play from chain start.
					play_chain(VIEW_CHAIN, 0);
				}
				play_phrase(fetch_pcc_phrase());
				break;

			case PLAYMODE_SONG:
				advance_step_song();
				break;
		}
	}
}

extern U8 g_sid_src[3 * 7];
U8 d417;
U8 g_filter_mod[3];
void apply_new_instr(U8 instr_no)
{
	const Instr* instr = (Instr*)(INSTR_START + instr_no * 16);
	U8* sid = g_sid_src + PCC7;
	const U8 flt_vctl_mask = (1 << PCC);
	const U8 flt_mode = instr->flt_mode;
	static U8 d417;
	sid[0] = instr->ad;
	sid[1] = instr->sr;
	sid[2] = instr->h;
	sid[3] = instr->vctl;
	sid[4] = instr->pw;

	// Filter...
	if (flt_mode & 0xf0)
	{
		// Activate filter.
		*(U8*)0xd416 = instr->flt_cutoff + g_filter_mod[PCC];
		d417 &= 0xf;
		d417 |= instr->flt_q | flt_vctl_mask;
		*(U8*)0xd418 = flt_mode;
	}
	else
	{
		// No filter.
		d417 &= ~flt_vctl_mask;
	}
	*(U8*)0xd417 = d417;
}
