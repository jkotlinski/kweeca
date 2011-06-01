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
