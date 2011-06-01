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

#include "file.h"

#include <cbm.h>
#include <stdio.h>

#include "myassert.h"
#include "mem.h"
#include "status.h"

const char* filename = "song";

void load_song()
{
    show_status_msg(MSG_LOADING);
    assert(MEM_SIZE == cbm_load(filename, 8, MEM_START));
    assert(*DATA_VERSION == CURR_DATA_VERSION);
    show_status_msg(MSG_OK);
}

void save_song()
{
    U8 status;
    show_status_msg(MSG_SAVING);
    remove(filename);
    *DATA_VERSION = 0;
    status = cbm_save(filename, 8, MEM_START, MEM_SIZE);
    show_status_msg(status ? MSG_ERROR : MSG_OK);
}
