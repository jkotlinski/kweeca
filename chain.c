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

#include "chain.h"

#include <cbm.h>
#include <conio.h>
#include <string.h>

#include "alloc.h"
#include "boxes.h"
#include "cursor.h"
#include "defines.h"
#include "global.h"
#include "kbd.h"
#include "mem.h"
#include "myassert.h"
#include "phrase.h"
#include "player.h"
#include "print.h"
#include "song.h"
#include "status.h"

#define CHAIN_BOX_WIDTH 6
#define CHAIN_BOX_HEIGHT 17

#define PHRASE_COL 0
#define TRANSPOSE_COL 1

U8 chain_handle_key(U8 key);
void lift_chain_col();
void chain_tweak_handler(U8 modval);
void chain_paster(U8 val);
static void cursor_down();
static void cursor_up();
static void draw_chain();

static const U8 chain_screen_col[] = 
{ 
    CHAIN_X + 1 + 0, 
    CHAIN_X + 1 + 3
};
static const U8 chain_col_width[] = { 2, 2 };
const BoxData k_chain_box_data =
{
    2, // Column count.
    1, // Min lift col 2.
    chain_screen_col,
    chain_col_width,
    chain_handle_key,
    cursor_down,
    cursor_up,
    chain_tweak_handler,
    lift_chain_col,
    chain_paster,
    0, // Paste advance row fn.
    0, // Y pos validator.
    0, // refresh_cursor
    draw_chain
};

U8 VIEW_CHAIN;
static U16 CHAIN_OFFSET;

static U8 last_input_phrase[3];
static U8 last_input_transpose[3];

static void draw_chain_border()
{
    gotoxy(CHAIN_X + 1, CHAIN_Y);
    textcolor(COLOR_GREEN);
    cputs("ch.");
    print_hex(VIEW_CHAIN);
}

static U8 get_phrase_on_row(U8 row)
{
    return *(U8*)(CHAIN_PHRASES + CHAIN_OFFSET + row);
}

static U8 get_phrase()
{
    return get_phrase_on_row(CUR_ROW_CHAIN);
}

U8 get_prev_phrase()
{
    U8 retval;
    if (!CUR_ROW_CHAIN)
        return 0xffu;
    --CUR_ROW_CHAIN;
    retval = get_phrase();
    ++CUR_ROW_CHAIN;
    return retval;
}

U8 get_next_phrase()
{
    U8 retval;
    if (CUR_ROW_CHAIN == 0xf)
        return 0xffu;
    ++CUR_ROW_CHAIN;
    retval = get_phrase();
    --CUR_ROW_CHAIN;
    return retval;
}

static U8 get_transpose()
{
    return *(U8*)(CHAIN_TRANSPOSES + CHAIN_OFFSET + CUR_ROW_CHAIN);
}

#define CUR_SCREEN_ROW (CHAIN_Y + 1 + CUR_ROW_CHAIN)

static void set_phrase(U8 val)
{
    assert(val < PHRASE_COUNT || val == 0xffu);
    *(U8*)(CHAIN_PHRASES + CHAIN_OFFSET + CUR_ROW_CHAIN) = val;
    gotoxy(chain_screen_col[PHRASE_COL], CUR_SCREEN_ROW);
    print_hex_ff(val);
}

static void set_transpose(U8 val)
{
    *(U8*)(CHAIN_TRANSPOSES + CHAIN_OFFSET + CUR_ROW_CHAIN) = val;
    gotoxy(chain_screen_col[TRANSPOSE_COL], CUR_SCREEN_ROW);
    print_hex(val);
}

static void draw_chain()
{
    U8 y;
    U8* phrase_ptr = CHAIN_PHRASES + CHAIN_OFFSET;
    U8* transpose_ptr = CHAIN_TRANSPOSES + CHAIN_OFFSET;
    draw_chain_border();
    textcolor(g_cur_box == CHAIN_BOX ? COLOR_WHITE : COLOR_INACTIVE);
    for (y = 0; y < 16; ++y)
    {
        gotoxy(CHAIN_X + 1, CHAIN_Y + 1 + y);
        print_hex_ff(phrase_ptr[y]);
        cputc(' ');
        print_hex(transpose_ptr[y]);
    }
}

static void show_phrase()
{
    U8 phrase = get_phrase();
    if (0xffu == phrase)
        return;
    view_phrase(phrase);
}

void view_chain(U8 chain)
{
    VIEW_CHAIN = chain;
    CHAIN_OFFSET = chain * 16;
    draw_chain();
    show_phrase();
}

static void release_space_handler()
{
    alloc_phrase(get_phrase());
    release_space_handler_p = 0;
}

static void new_or_clone(U8 do_new)
{
    U8 phrase;
    if (do_new)
    {
        // No phrase on step, alloc new.
        phrase = next_new_phrase();
    }
    else
    {
        // Clone existing chain.
        phrase = clone_phrase(get_phrase());
    }
    if (phrase == 0xffu)
        return;
    last_input_phrase[EDIT_CH] = phrase;
    alloc_phrase(phrase);
    set_phrase(phrase);
    show_phrase();
}

static void handle_phrase_space()
{
    static U8 s_first_tap_was_on_empty_step;
    U8 phrase = get_phrase();
    release_space_handler_p = release_space_handler;

    if (g_marking) return;

    switch (g_space_tap_count)
    {
        case 1:
            s_first_tap_was_on_empty_step = (0xffu == phrase);

            if (s_first_tap_was_on_empty_step)
            {
                set_phrase(last_input_phrase[EDIT_CH]);
                show_phrase();
            }
            else 
            {
                last_input_phrase[EDIT_CH] = phrase;
            }
            break;

        case 2:
            new_or_clone(s_first_tap_was_on_empty_step);
            break;
    }
}

static void handle_space()
{
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            handle_phrase_space();
            break;

        case TRANSPOSE_COL:
            {
                U8 tsp = get_transpose();
                if (!tsp)
                    set_transpose(last_input_transpose[EDIT_CH]);
                else
                    last_input_transpose[EDIT_CH] = tsp;
            }
    }
}

static U8 handle_dec_entry(U8 key)
{
    U8 status;
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            status = get_hex_from_keyboard(key, PHRASE_COUNT - 1);
            switch (status)
            {
                case HEXGET_DONE:
                    last_input_phrase[EDIT_CH] = g_hexget_value;
                    alloc_phrase(g_hexget_value);
                    // Fall through!
                    
                case HEXGET_ACTIVE:
                    set_phrase(g_hexget_value);
                    show_phrase();
            }
            break;

        case TRANSPOSE_COL:
            status = get_hex_from_keyboard(key, 0xffu);
            if (status != HEXGET_IDLE)
            {
                set_transpose(g_hexget_value);
                last_input_transpose[EDIT_CH] = g_hexget_value;
            }
            break;
    }
    return status == HEXGET_ACTIVE;
}

static void delete()
{
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            {
                U8 phrase = get_phrase();
                if (phrase != 0xffu)
                {
                    last_input_phrase[EDIT_CH] = phrase;
                    set_phrase(0xffu);
                }
            }
            // Fall through!
    
        case TRANSPOSE_COL:
            {
                U8 tsp = get_transpose();
                if (tsp)
                {
                    last_input_transpose[EDIT_CH] = tsp;
                    set_transpose(0);
                }
            }
    }
}

U8 chain_handle_key(U8 key)
{
    if (handle_dec_entry(key))
        return 1;
    switch (key)
    {
        case CH_REFRESH:
            show_phrase();
            break;

        case CH_ENTER | CH_SHIFT:
            handle_song_box_enter();
            break;

        case CH_ENTER:
            if (PLAYMODE != PLAYMODE_CHAIN)
                player_stop();
            if (!g_playing)
            {
                PCC = EDIT_CH;
                play_chain(VIEW_CHAIN, CUR_ROW_CHAIN);
                PLAYMODE = PLAYMODE_CHAIN;
                g_playing = 1;
            }
            else
                player_stop();
            break;

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
            {
                delete();
                move_cursor_down();
            }
            break;

        case 'm':
            mark();
            break;

        case CH_SCREEN_RIGHT:
            switch_to_box(PHRASE_BOX);
            break;
        case CH_SCREEN_LEFT:
            switch_to_box(SONG_BOX);
            break;
            
        default:
            return 0;
    }
    return 1;
}

void lift_chain_col()
{
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            g_clipboard[PHRASE_COL][CUR_ROW_CHAIN] = get_phrase();
            set_phrase(0xffu);
            break;
        case TRANSPOSE_COL:
            g_clipboard[TRANSPOSE_COL][CUR_ROW_CHAIN] = get_transpose();
            set_transpose(0);
            break;
    }
}

void chain_tweak_handler(U8 modval)
{
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            {
                U8 phrase = get_phrase();
                if (0xffu == phrase)
                    return;
                phrase += modval;
                if (phrase >= 0xf0u)
                    phrase = 0;
                else if (phrase >= PHRASE_COUNT)
                    phrase = PHRASE_COUNT - 1;
                set_phrase(phrase);
                last_input_phrase[EDIT_CH] = phrase;
            }
            break;

        case TRANSPOSE_COL:
            {
                U8 tsp = get_transpose();
                if (modval == 0x10) modval = 0xc;
                else if (modval == 0xf0u) modval = 0xf4;
                tsp += modval;
                last_input_transpose[EDIT_CH] = tsp;
                set_transpose(tsp);
            }
            break;
    }
}

void chain_paster(U8 val)
{
    switch (CUR_COL_CHAIN)
    {
        case PHRASE_COL:
            set_phrase(val);
            break;

        case TRANSPOSE_COL:
            set_transpose(val);
            break;
    }
}

static void set_chain_phrase(U8 chain, U8 row, U8 phrase)
{
    *(U8*)(CHAIN_PHRASES + chain * 16 + row) = phrase;
}

static U8 get_chain_phrase(U8 chain, U8 row)
{
    return *(U8*)(CHAIN_PHRASES + chain * 16 + row);
}

U8 clone_chain()
{
    U8 row = 0;
    U8 new_chain = next_new_chain();
    if (new_chain == 0xffu)
        return 0xffu;
    alloc_chain(new_chain);

    // Copy transposes.
    memcpy(CHAIN_TRANSPOSES + new_chain * 16,
            CHAIN_TRANSPOSES + VIEW_CHAIN * 16,
            16);

    // Clone phrases.
    for (; row < 16; ++row)
    {
        U8 src_phrase = get_phrase_on_row(row);
        U8 prev_rows_it = 0;
        U8 new_phrase;
        if (src_phrase == 0xffu)
            continue;

        // OK, we got a phrase we want to clone. Is it already cloned?
        for (prev_rows_it; prev_rows_it < row; ++prev_rows_it)
        {
            if (src_phrase == get_phrase_on_row(prev_rows_it))
            {
                // Yes.
                break;
            }
        }

        if (prev_rows_it != row)
        {
            // Above loop interrupted prematurely. Thus:
            // This phrase has already been cloned, let's reuse the clone.
            new_phrase = get_chain_phrase(new_chain, prev_rows_it);
        }
        else
        {
            // Clone a new phrase.
            new_phrase = clone_phrase(src_phrase);
        }

        set_chain_phrase(new_chain, row, new_phrase);
    }

    return new_chain;
}

static void cursor_down()
{
    if (CUR_ROW_CHAIN < 15)
        ++CUR_ROW_CHAIN;
}

static void cursor_up()
{
    if (CUR_ROW_CHAIN)
        --CUR_ROW_CHAIN;
}
