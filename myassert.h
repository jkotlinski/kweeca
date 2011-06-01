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

#ifndef _MYASSERT_H
#define _MYASSERT_H

//#define SLIM_ASSERT

#define STATIC_ASSERT(expr) switch(0) { case 0: case (expr): ; }

#ifdef SLIM_ASSERT
#define assert(a) if(!(a)) while(1) ++*(char*)0xd020u
#else
#include <assert.h>
#endif

#endif
