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
