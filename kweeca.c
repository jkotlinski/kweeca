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

#include <c64.h>

#include <conio.h>
#include <string.h>

#include "boxes.h"
#include "cursor.h"
#include "defines.h"
#include "file.h"
#include "global.h"
#include "kbd.h"
#include "mem.h"
#include "player.h"
#include "status.h"

#include "instr.h"

void wait_frame()
{
    while (VIC.rasterline != 0x40);
    while (VIC.rasterline == 0x40);
}

void beep()
{
    SID.v1.ctrl = 0x41;
    SID.v2.ctrl = 0x41;
}

void mute()
{
    SID.v1.ctrl = 0x40;
    SID.v2.ctrl = 0x40;
}

void init()
{
    clrscr();
    player_init();
    init_mem();
    startirq();
    *(U8*)0xd018 = 0x14; // Use upper case + gfx chars.
    VIC.bgcolor0 = COLOR_BLACK;
    VIC.bordercolor = COLOR_BLACK;
    *(U8*)0x28a = 0x80; // Repeat all keys.
    textcolor(COLOR_YELLOW);
    reset_boxes();
    show_status_msg(MSG_HELLO);
    init_play_indicators();
}

#include "print.h"

void handle_key(U8 key)
{
    switch (key)
    {
        case CH_F5:
            load_song();
            reset_boxes();
            return;

        case CH_F7:
            save_song();
            return;
    }

    boxes_handle_key(key);
    reset_blink();
}

void tick()
{
    U8 prev_key_space = KEY_SPACE;
    if (kbhit())
    {
        const U8 ch = cgetc();
        const U8 is_repeated_space = (ch == ' ' && KEY_SPACE);
        if (!is_repeated_space)
        {
            handle_key(ch);
        }
    }
    poll_special_keys();
    if (prev_key_space && !KEY_SPACE && release_space_handler_p)
        release_space_handler_p();
    cursor_tick();
    play_indicator_tick();
    status_tick();
}

int main()
{
    init();

    for (;;)
    {
        while (!g_ticks); // Wait for tick.
        tick();
        g_ticks--; // Only decreased here.
    }
    return 0;
}

