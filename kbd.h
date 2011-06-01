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

#ifndef _KBD_H
#define _KBD_H

#include "types.h"

extern U8 KEY_LSHIFT;
extern U8 KEY_RSHIFT;
extern U8 KEY_SHIFT;
extern U8 KEY_SPACE;

enum special_keys {
    CH_CTRL = 1,
    CH_TWEAK_UP,
    // CH_LEFTARROW = 3
    CH_TWEAK_DOWN = 4,
    CH_TWEAK_RIGHT,
    CH_TWEAK_LEFT,
    CH_REFRESH, // Dummy key to refresh view.
    CH_SHIFT = 0x80u
};

#define CH_MYLEFT ':'
#define CH_MYDOWN ';'
#define CH_MYUP '@'
#define CH_MYRIGHT '='

#define CH_SCREEN_LEFT 'p'
#define CH_SCREEN_RIGHT '*'

#define CH_MYDOWN_SHIFTED ']'
#define CH_MYUP_SHIFTED 0xbau

void poll_special_keys();

#endif // _KBD_H
