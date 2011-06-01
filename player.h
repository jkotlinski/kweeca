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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "types.h"

#define PLAYMODE_SONG 0
#define PLAYMODE_CHAIN 1
#define PLAYMODE_PHRASE 2

extern U8 PLAYMODE;
extern U8 CH_ACTIVE[3];
extern U8 ACTIVE_PHRASE[3];
extern U8 ACTIVE_CHAIN[3];
extern U8 PCC;

extern U8 CHAIN_TRANSPOSE[3];

extern U8 PHRASE_INSTR_PTR_MSB[3];
extern U8 PHRASE_INSTR_PTR_LSB[3];
extern U8 PHRASE_NOTE_PTR_MSB[3];
extern U8 PHRASE_NOTE_PTR_LSB[3];
extern U8 PHRASE_CMD_PTR_MSB[3];
extern U8 PHRASE_CMD_PTR_LSB[3];
extern U8 PHRASE_ARG_PTR_MSB[3];
extern U8 PHRASE_ARG_PTR_LSB[3];

extern U8 CHAIN_PHRASE_PTR_LSB[3];

extern U8* SONG_CHAIN_PTR[3];

void startirq();

void player_init();

void player_stop();

void play_song(U8 song_row);
void play_chain(U8 chain, U8 row); // ch = PCC
void play_phrase(U8 phrase); // ch = PCC

extern U8 g_playing;
extern U8 g_ticks;

extern U8 PLAYMODE;

#endif // _PLAYER_H
