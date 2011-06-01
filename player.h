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
