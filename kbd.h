/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
