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

#include "print.h"

#include <conio.h>
#include <stdio.h>

#include "myassert.h"

void print_nibble(U8 val)
{
	static const U8 hextab[] = {
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46
	};
	cputc(hextab[val]);
}

void print_hex(U8 val)
{
	print_nibble(val >> 4);
	print_nibble(val & 0xf);
}

static const char* notes = 	"ccddeffggaab";
static const char* sharps = " # #  # # # ";

void print_note(U8 note)
{
	U8 octave;

	if (note == 0xff)
	{
		cputs("-- ");
		return;
	}
	else if (note == 0xfe)
	{
		cputs("++ ");
		return;
	}

   	octave = note / 12;
	note = note % 12;
	cputc(notes[note]);
	print_nibble(octave);
	cputc(sharps[note]);
}

void print_hex_ff(U8 val)
{
	if (val == 0xff)
		cputs("--");
	else
		print_hex(val);
}

void paint_box(U8 x1, U8 y1, U8 x2, U8 y2, U8 inverse)
{
	assert(x1 <= x2);
	assert(y1 <= y2);
	while (y1 <= y2)
	{
		U8 x = x1;
		while (x <= x2)
		{
			U8* it = (U8*)0x400u + x + 40 * y1;
			if (inverse)
				*it |= 0x80u;
			else
				*it &= 0x7fu;
			++x;
		}
		++y1;
	}
}

