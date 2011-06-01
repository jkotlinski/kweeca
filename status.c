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
