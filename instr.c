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

#include "instr.h"

#include <conio.h>

#include "myassert.h"
#include "screens.h"
#include "cursor.h"
#include "kbd.h"
#include "math.h"
#include "mem.h"
#include "print.h"

static Instr* s_curr_instr = (Instr*)INSTR_START;
U8 VIEW_INSTR;

typedef enum
{
    EtBool,
    EtNibble,
    EtByte,
    EtCount
} EntryType;

typedef enum {
    CLeft,
    CRight,
    CBoth
} Column;

typedef U8 (*get_fn)(void);
typedef void (*set_fn)(U8);
typedef struct 
{
    U8* desc;
    get_fn get;
    set_fn set;
    int x : 4;
    int y : 4;
    int column : 2;
    int type : 2;
    int enabled : 1;
} Field;

#define COL2X 6

// Byte getters + setters.

static U8 get_ad() { return s_curr_instr->ad; }
static U8 get_sr() { return s_curr_instr->sr; }
static U8 get_h() { return s_curr_instr->h; }
static U8 get_pw() { return s_curr_instr->pw; }
static U8 get_cf() { return s_curr_instr->flt_cutoff; }

static void set_ad(U8 val) { s_curr_instr->ad = val; }
static void set_sr(U8 val) { s_curr_instr->sr = val; }
static void set_h(U8 val) { s_curr_instr->h = val; }
static void set_pw(U8 val) { s_curr_instr->pw = val; }
static void set_cf(U8 val) { s_curr_instr->flt_cutoff = val; }

// Nibble getters + setters.

static U8 get_res() { return s_curr_instr->flt_q / 16; }
static U8 get_flt_mode() { return s_curr_instr->flt_mode; }
static U8 get_vol() { return get_flt_mode() & 0xf; }

static void set_res(U8 val) { s_curr_instr->flt_q = val * 16; }
static void set_flt_mode(U8 val) { s_curr_instr->flt_mode = val; }

static void set_vol(U8 val) {
    s_curr_instr->flt_mode &= 0xf0u;
    s_curr_instr->flt_mode |= val;
}

// bool getters + setters.

static U8 get_wf() {
    return s_curr_instr->vctl;
}
static void set_wf(U8 val) {
    s_curr_instr->vctl = val;
}
static U8 get_noi() {
    return get_wf() & 0x80u;
}
static void set_noi(U8 set) {
    U8 val = get_wf() & ~0x80u;
    set_wf(val | (set ? 0x80u : 0));
}
static U8 get_sqr() {
    return get_wf() & 0x40;
}
static void set_sqr(U8 set) {
    U8 val = get_wf() & ~0x40;
    set_wf(val | (set ? 0x40u : 0));
}
static U8 get_saw() {
    return get_wf() & 0x20;
}
static void set_saw(U8 set) {
    U8 val = get_wf() & ~0x20;
    set_wf(val | (set ? 0x20u : 0));
}
static U8 get_tri() {
    return get_wf() & 0x10;
}
static void set_tri(U8 set) {
    U8 val = get_wf() & ~0x10;
    set_wf(val | (set ? 0x10u : 0));
}
static U8 get_ring() {
    return get_wf() & 4;
}
static void set_ring(U8 set) {
    U8 val = get_wf() & ~4;
    set_wf(val | (set ? 4 : 0));
}
static U8 get_sync() {
    return get_wf() & 2;
}
static void set_sync(U8 set) {
    U8 val = get_wf() & ~2;
    set_wf(val | (set ? 2 : 0));
}
static U8 get_hp() {
    return get_flt_mode() & 0x40;
}
static void set_hp(U8 set) {
    U8 val = get_flt_mode() & ~0x40;
    set_flt_mode(val | (set ? 0x40 : 0));
}
static U8 get_bp() {
    return get_flt_mode() & 0x20;
}
static void set_bp(U8 set) {
    U8 val = get_flt_mode() & ~0x20;
    set_flt_mode(val | (set ? 0x20 : 0));
}
static U8 get_lp() {
    return get_flt_mode() & 0x10;
}
static void set_lp(U8 set) {
    U8 val = get_flt_mode() & ~0x10;
    set_flt_mode(val | (set ? 0x10 : 0));
}

static U8 dummy_get() { return 0; }

// ----------

#define FIELD_COUNT 18
#define MAX_Y 15
#define COL1_Y 1
static const Field fields[FIELD_COUNT] =
{
    // { "",            0, 0, 0, 0, CBoth, EtString, 0 },
    { "ad", get_ad, set_ad, 3, COL1_Y, CLeft, EtByte, 1 },
    { "h", get_h, set_h, 3, COL1_Y + 1, CLeft, EtByte, 1 },
    { "sr", get_sr, set_sr, 3, COL1_Y + 2, CLeft, EtByte, 1 },
    { "saw", get_saw, set_saw, 4, COL1_Y + 4, CLeft, EtBool, 1 },
    { "tri", get_tri, set_tri, 4, COL1_Y + 5, CLeft, EtBool, 1 },
    { "noi", get_noi, set_noi, 4, COL1_Y + 6, CLeft, EtBool, 1 },
    { "pul", get_sqr, set_sqr, 4, COL1_Y + 7, CLeft, EtBool, 1 },
    { "pw", get_pw, set_pw, 3, COL1_Y + 8, CLeft, EtByte, 1 },
    { "ring", get_ring, set_ring, 5,  COL1_Y + 10, CLeft, EtBool, 1 },
    { "sync", get_sync, set_sync, 5, COL1_Y + 11, CLeft, EtBool, 1 },
    { "automate", dummy_get, 0, 9, MAX_Y - 1, CBoth, EtBool, 0 },
    { "table", dummy_get, 0, 6, MAX_Y, CBoth, EtNibble, 0 },

    { "hp ", get_hp, set_hp, COL2X + 4, 3, CRight, EtBool, 1 },
    { "bp ", get_bp, set_bp, COL2X + 4, 4, CRight, EtBool, 1 },
    { "lp ", get_lp, set_lp, COL2X + 4, 5, CRight, EtBool, 1 },
    { "cf", get_cf, set_cf, COL2X + 3, 7, CRight, EtByte, 1 },
    { "res", get_res, set_res, COL2X + 4, 8, CRight, EtNibble, 1 },
    { "vol", get_vol, set_vol, COL2X + 4, 9, CRight, EtNibble, 1 },
};

enum
{
    FieldName,
    FieldAd,
    FieldSr
};

static U8 s_curr_field = FieldAd;
static U8 s_curr_column = 0;
static const Field* get_curr_field() { return &fields[s_curr_field]; }

static void cur_left();
static void cur_right();

#define XPOS 11
#define YPOS 1

static void print_value(const Field* field)
{
    assert(field->get);
    gotoxy(XPOS + field->x, YPOS + field->y);
    switch (field->type)
    {
        case EtBool:
#define BOOL_OFF 0x77
#define BOOL_ON 0x71
            cputc(field->get() ? BOOL_ON : BOOL_OFF);
            break;

        case EtNibble:
            print_nibble(field->get());
            break;

        case EtByte:
            print_hex(field->get());
            break;
    }
}

static void set(U8 value)
{
    const Field* field = get_curr_field();
    field->set(value);
    print_value(field);
}

// Return true if we stole the key.
static U8 handle_num_entry(U8 key)
{
    static const U8 k_max_input_values[] = { 0, 0xf, 0xff, 0 };
    const U8 max_input_value = k_max_input_values[fields[s_curr_field].type];
    U8 status;

    if (!max_input_value)
        return 0;

    status = get_hex_from_keyboard(key, max_input_value);
    switch (status)
    {
        case HEXGET_DONE:
        case HEXGET_ACTIVE:
            set(g_hexget_value);
    }
    return status == HEXGET_ACTIVE;
}

static void handle_space()
{
    const Field* field = get_curr_field();
    if (field->type != EtBool)
        return;
    field->set(!field->get());
    print_value(field);
}

static U8 instr_handle_key(U8 key)
{
    if (handle_num_entry(key))
        return 1;

    switch (key)
    {
        case CH_CURS_LEFT:
            if (KEY_LSHIFT) {
                switch_screen(PHRASE_SCREEN);
            } else {
                cur_left();
            }
            return 1;

        case CH_CURS_RIGHT:
            cur_right();
            return 1;

        case ' ':
            handle_space();
            return 1;
    }
    return 0;
}

static void refresh_cursor()
{
    static const U8 width[4] = { 1, 1, 2, 6 };
    const Field* field = get_curr_field();
    move_cursor(
        XPOS + field->x, 
        YPOS + field->y, 
        width[field->type]);
}

static void draw_instr();
static void cur_down();
static void cur_up();
static void handle_tweak(U8 diff);

static const ScreenData k_instr_screen_data =
{
    2, // Column count.
    0,
    0,
    0,
    instr_handle_key,
    cur_down,
    cur_up,
    handle_tweak,
    0,
    0,
    0,
    0,
    refresh_cursor,
    draw_instr
};

#include "global.h"

static U8 match(U8 field_it, U8 y)
{
    const Field* field = &fields[field_it];
    U8 column = field->column;
    if (!field->enabled)
        return 0;
    if (field->y != y)
        return 0;
    return column == CBoth || column == s_curr_column;
}

static U8 goto_row(U8 y)
{
    U8 field_it;

    for (field_it = 0; field_it < FIELD_COUNT; ++field_it)
    {
        if (match(field_it, y))
        {
            s_curr_field = field_it;
            refresh_cursor();
            return 1;
        }
    }
    return 0;
}

static U8 curr_y()
{
    return fields[s_curr_field].y;
}

static void move_cursor_vertically(char step)
{
    U8 y = curr_y();

    for (;;)
    {
        y += step;
        
        if (y > MAX_Y) break; // Hit top or bottom.
        if (goto_row(y)) break; // Changed field OK.
    }
}

static void cur_down()
{
    move_cursor_vertically(1);
}

static void cur_up()
{
    move_cursor_vertically(-1);
}

static void adjust_row_after_column_change()
{
    if (!goto_row(curr_y())) 
        cur_up();
}

static void cur_left()
{
    s_curr_column = 0;
    adjust_row_after_column_change();
}

static void cur_right()
{
    s_curr_column = 1;
    adjust_row_after_column_change();
}

static void draw_field(U8 field_it)
{
    const Field* field = &fields[field_it];
    U8 col_x = (field->column == CRight) ? XPOS + COL2X : XPOS;

    textcolor(field->enabled ? COLOR_WHITE : COLOR_GRAY1);

    print_value(field);
    gotox(col_x);
    cputs(field->desc);
}

static void draw_instr()
{
    U8 field_it = 0;
    gotoxy(XPOS, 0);
    textcolor(COLOR_GREEN);
    cputs("instr ");
    print_hex(VIEW_INSTR);
    gotoxy(XPOS + 6, YPOS + 1);
    cputs("filtr");
    while (field_it < FIELD_COUNT)
    {
        draw_field(field_it++);
    }
}

static void handle_tweak(U8 diff)
{
    const Field* field = get_curr_field();
    switch (field->type)
    {
        case EtNibble:
            set(clamp_to_nibble(field->get() + diff));
            break;

        case EtByte:
            set(clamp_to_byte(field->get() + (S8)diff));
            break;
    }
}

void view_instr(U8 instr)
{
    STATIC_ASSERT(!(sizeof(Instr) & 0xf0u)); // sizeof(Instr) < 0x10
    assert(instr < INSTR_COUNT);
    s_curr_instr = (Instr*)(INSTR_START + 0x10 * instr);
    VIEW_INSTR = instr;
    draw_instr();
}
