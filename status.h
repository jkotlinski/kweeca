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

#ifndef _STATUS_H
#define _STATUS_H

#include "types.h"

typedef enum {
	MSG_HELLO,
	MSG_ALLOC_OUT_OF_RAM,
	MSG_LOADING,
	MSG_SAVING,
	MSG_OK,
	MSG_ERROR,
	MSG_MARKING,
	MSG_LIFT_OK,
	MSG_COUNT
} STATUS_MSG_ID;

void show_status_msg(STATUS_MSG_ID id);
void status_tick();

#endif // _STATUS_H
