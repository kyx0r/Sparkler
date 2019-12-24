/* Sparkler

Copyright (c) 2003 Christian Reitwiessner (christian@reitwiessner.de)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef GFX_H
#define GFX_H

void redraw(void);
void stay_on_top(void);
int init_gfx(void);
void deinit_gfx(void);
void get_pointer_pos(int *x, int *y);
void handle_events(void);

#endif
