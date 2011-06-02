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

#include "song.h"

#include <conio.h>

#include "alloc.h"
#include "screens.h"
#include "chain.h"
#include "cursor.h"
#include "defines.h"
#include "global.h"
#include "mem.h"
#include "kbd.h"
#include "player.h"
#include "print.h"
#include "status.h"

#define SONG_BOX_WIDTH 9
#define SONG_BOX_HEIGHT 17

void set_chain(U8 val);
U8 song_handle_key(U8 key);
void lift_song_col();
void song_tweak_handler(U8 modval);
static void cursor_down();
static void cursor_up();
static void y_pos_validator();
static void draw_song();

static const U8 song_screen_col[] = { SONG_X, SONG_X + 3, SONG_X + 6 };
static const U8 song_col_width[] = { 2, 2, 2 };
const ScreenData k_song_screen_data =
{
    3, // Column count.
    0, // Min lift col 2.
    song_screen_col,
    song_col_width,
    song_handle_key,
    cursor_down,
    cursor_up,
    song_tweak_handler,
    lift_song_col,
    set_chain,
    cursor_down,
    y_pos_validator,
    0, // refresh_cursor
    draw_song
};

U8 g_song_y_offset;

static void draw_row_number();
static void validate_window_start();
static void y_pos_validator()
{
    if (CUR_ROW_SONG < g_song_y_offset)
        g_song_y_offset = CUR_ROW_SONG;
    if (CUR_ROW_SONG >= g_song_y_offset + 0x10)
        g_song_y_offset = CUR_ROW_SONG - 0x10;
    validate_window_start();
    draw_row_number();
}

U8 song_y_offset()
{
    return g_song_y_offset;
}

static U8 last_chain[3];

static void draw_row_number()
{
    gotoxy(SONG_X + 5, SONG_Y);
    textcolor(COLOR_TITLE);
    print_hex(CUR_ROW_SONG);
}

static void draw_song_border()
{
    gotoxy(SONG_X, SONG_Y);
    textcolor(COLOR_TITLE);
    cputs("song");
    draw_row_number();
}

static U8 get_chain(U8 x, U8 y)
{
    return *(U8*)(SONG_CHAINS + y * 3 + x);
}

void set_chain(U8 val)
{
    U8 y = CUR_ROW_SONG - g_song_y_offset;
    *(U8*)(SONG_CHAINS + CUR_ROW_SONG * 3 + CUR_COL_SONG) = val;
    if (y & 0xf0u)
        return;
    TEXTCOLOR(SONG_BOX);
    gotoxy(SONG_X + CUR_COL_SONG * 3, SONG_Y + 1 + y);
    print_hex_ff(val);
}

static void draw_song_rows()
{
    U8 y = 0;
    TEXTCOLOR(SONG_BOX);
    for (; y < 16; ++y)
    {
        const U8 src_y = y + g_song_y_offset;
        gotoxy(SONG_X, SONG_Y + 1 + y);
        print_hex_ff(get_chain(0, src_y));
        cputc(' ');
        print_hex_ff(get_chain(1, src_y));
        cputc(' ');
        print_hex_ff(get_chain(2, src_y));
    }
}

static void draw_song()
{
    draw_song_border();
    draw_song_rows();
}

static U8 get_cur_chain()
{
    return get_chain(CUR_COL_SONG, CUR_ROW_SONG);
}

static void release_space_handler()
{
    alloc_chain(get_cur_chain());
    release_space_handler_p = 0;
}

static void new_or_clone(U8 do_new)
{
    U8 chain;
    if (do_new)
    {
        // No chain on step, alloc new.
        chain = next_new_chain();
    }
    else
    {
        // Clone existing chain.
        chain = clone_chain();
    }
    if (chain == 0xffu)
        return;
    last_chain[EDIT_CH] = chain;
    alloc_chain(chain);
    set_chain(chain);
}

static void handle_space()
{
    static U8 s_first_tap_was_on_empty_step;
    U8 chain = get_cur_chain();
    release_space_handler_p = release_space_handler;

    if (g_marking) return;

    switch (g_space_tap_count)
    {
        case 1:
            s_first_tap_was_on_empty_step = (0xffu == chain);

            if (s_first_tap_was_on_empty_step) {
                set_chain(last_chain[EDIT_CH]);
            } else {
                last_chain[EDIT_CH] = chain;
            }
            break;

        case 2:
            new_or_clone(s_first_tap_was_on_empty_step);
            break;
    }
}

static void pull_up_below_chains()
{
    U8 prev_row = CUR_ROW_SONG;
    while (++CUR_ROW_SONG < SONG_ROW_COUNT)
    {
        U8 chain = get_cur_chain();
        --CUR_ROW_SONG;
        set_chain(chain);
        ++CUR_ROW_SONG;
    }
    CUR_ROW_SONG = prev_row;
}

static void cut_step()
{
    U8 chain = get_cur_chain();
    if (0xffu == chain)
    {
        pull_up_below_chains();
        return;
    }
    last_chain[EDIT_CH] = chain;
    g_clipboard[0][0] = chain;
    g_clip_x1 = 0;
    g_clip_x2 = 0;
    g_clip_y1 = 0;
    g_clip_y2 = 0;
    g_clip_type = SONG_SCREEN;
    set_chain(0xffu);
}

// Return true if we stole the key.
static U8 handle_chain_entry(U8 key)
{
    U8 status = get_hex_from_keyboard(key, CHAIN_COUNT - 1);
    switch (status)
    {
        case HEXGET_DONE:
            last_chain[EDIT_CH] = g_hexget_value;
            alloc_chain(g_hexget_value);
            // Fall through...

        case HEXGET_ACTIVE:
            set_chain(g_hexget_value);
    }
    return status == HEXGET_ACTIVE;
}

void song_startstop()
{
    if (PLAYMODE != PLAYMODE_SONG)
        player_stop();
    if (!g_playing)
    {
        play_song(CUR_ROW_SONG);
        PLAYMODE = PLAYMODE_SONG;
        g_playing = 1;
    }
    else
        player_stop();
}

U8 song_handle_key(U8 key)
{
    if (handle_chain_entry(key))
        return 1;

    switch (key) {
        case CH_ENTER:
            song_startstop();
            break;

        case CH_CURS_RIGHT:
            if (KEY_LSHIFT) {
                switch_screen(CHAIN_SCREEN);
                break;
            } else {
                return 0;
            }

        case ' ':
            handle_space();
            break;

        case CH_INS:
        case 'v':
            paste();
            break;

        case CH_DEL:
        case 'x':
            if (g_marking)
                cut_marked();
            else
                cut_step();
            break;

        case 'm':
            mark();
            break;

        default:
            return 0;
    }
    return 1;
}

void lift_song_col()
{
    g_clipboard[CUR_COL_SONG][CUR_ROW_SONG] = get_cur_chain();
    set_chain(0xffu);
}

void song_tweak_handler(U8 modval)
{
    U8 chain = get_cur_chain();
    if (0xffu == chain)
        return;
    chain += modval;
    if (chain >= 0xf0u)
        chain = 0;
    else if (chain >= CHAIN_COUNT)
        chain = CHAIN_COUNT - 1;
    set_chain(chain);
    last_chain[EDIT_CH] = chain;
}

static void validate_window_start()
{
#define WINDOW_MARGIN 2
    const U8 offset = CUR_ROW_SONG - g_song_y_offset;
    if (offset >= 0x10 - WINDOW_MARGIN)
    {
#define MAX_g_song_y_offset (SONG_ROW_COUNT - 0x10)
        if (g_song_y_offset < MAX_g_song_y_offset)
        {
            ++g_song_y_offset;
            draw_song_rows();
        }
    }
    else if (offset < WINDOW_MARGIN && g_song_y_offset)
    {
        --g_song_y_offset;
        draw_song_rows();
    }
}

static void cursor_down()
{
    if (CUR_ROW_SONG < SONG_ROW_COUNT - 1)
    {
        ++CUR_ROW_SONG;
        draw_row_number();
        validate_window_start();
    }
}

static void cursor_up()
{
    if (CUR_ROW_SONG)
    {
        --CUR_ROW_SONG;
        draw_row_number();
        validate_window_start();
    }
}
