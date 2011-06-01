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

#ifndef _INSTR_H
#define _INSTR_H

#include "types.h"
#include "box_data.h"

// sizeof(INSTR) must be less than 0x10!
typedef struct
{
    U8 ad;
    U8 h;
    U8 sr;
    U8 pw;
    U8 vctl; // d404

    U8 flt_cutoff; // filter cutoff
    U8 flt_mode; // filter mode + volume
    U8 flt_q; // filter resonance ctl

    unsigned int table : 4;
    unsigned int automate : 1;
} Instr;

extern const BoxData k_instr_box_data;

void view_instr(U8 instr);

#endif // _INSTR_H
