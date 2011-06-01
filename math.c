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

#include "math.h"

U8 clamp_to_nibble(U8 byte)
{
	if (byte & 0x80) return 0;
	else if (byte & 0x10u) return 0xf;
	return byte;
}

U8 clamp_to_byte(U16 word)
{
	if (word & 0x8000u) return 0;
	else if (word & 0xff00u) return 0xff;
	return word;
}
