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

#ifndef SPARKLER_H
#define SPARKLER_H

#define NUM_SPARKLES 100
#define SPARKLES_PER_SECOND 20.0
#define SPARKLE_LIFETIME 0.5
#define SPARKLE_LIFETIME_VARIANCE 0.25
#define SPARKLE_POS_VARIANCE 40
#define GEN_SPARKLES_AFTER_STOP 1.0

struct sparkle {
	int x, y;
	char alive;
	unsigned char r, g, b;
	unsigned char br, bg, bb;
	double death_time;
};

extern struct sparkle sparkles[];

#endif
