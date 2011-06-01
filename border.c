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

#include "border.h"

#include <conio.h>

#include "defines.h"

#define HBAR 0xc0u // -
#define VBAR 0xc2u // |
#define TOPLEFT 0x70 + 0x40
#define BOTTOMLEFT 0x6d + 0x40
#define TOPRIGHT 0x6e + 0x40
#define BOTTOMRIGHT 0x7d + 0x40

void draw_border(U8 x1, U8 y1, U8 x2, U8 y2, U8 active)
{
    U8 xi = x1 + 1;
    U8 yi = y1 + 1;
    U8 ycount = 0;
    textcolor(active ? COLOR_LIGHTBLUE : COLOR_INACTIVE);
    while (xi < x2)
    {
        // cputcxy(xi, y1, HBAR);
        cputcxy(xi, y2, HBAR);
        ++xi;
    }
    while (yi <= y2)
    {
        U8 symbol = !(ycount & 3) ? CH_CROSS : 
            !(ycount & 1) ? VBAR : ' ';
        if (x1 < 0xf0u) // In case we write outside left border...
            cputcxy(x1, yi, symbol);
        cputcxy(x2, yi, symbol);
        ++ycount;
        ++yi;
    }
}
