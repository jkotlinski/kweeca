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
