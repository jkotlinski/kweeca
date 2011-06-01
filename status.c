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

#include "status.h"

#include <conio.h>

#include "defines.h"
#include "myassert.h"

static const char* const msgs[] =
{
    "\1kweeca " VERSION,
    "\2out of mem. f3 = clean unused data",
    "\1loading",
    "\1saving",
    "\1ok",
    "\2error",
    "\1marking",
    "\1cut ok",
};

static void clear_status_line()
{
    U8 i;
    gotoxy(0, 19);
    for (i = 0; i < 40; ++i)
        cputc(' ');
}

static U8 status_counter;

void show_status_msg(STATUS_MSG_ID id)
{
    const char* msg = msgs[id];
    assert(id < MSG_COUNT);
    clear_status_line();
    textcolor(*msg);
    gotoxy(0, 19);
    cputs(msg + 1);
    status_counter = 255;
}

void status_tick()
{
    if (!status_counter || --status_counter) return;

    clear_status_line();
}
