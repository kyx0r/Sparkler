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

Author: Christian Reitwiessner

With contribution by Erik OShaughnessy <eriko@schwa.central.sun.com>
	(circle distribution)

*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>


#include "gfx.h"
#include "sparkler.h"

static double time_accumulator = 0;

struct sparkle sparkles[NUM_SPARKLES];


static int gen_sparkles(double last, double now)
{
	int pointer_x, pointer_y;
	int i, change = 0;
	
	int new_sparkles;
	
	time_accumulator += now - last;
	new_sparkles = time_accumulator * SPARKLES_PER_SECOND;
	time_accumulator -= (double) new_sparkles / SPARKLES_PER_SECOND;

	get_pointer_pos(&pointer_x, &pointer_y);

	for (i = 0; new_sparkles > 0 && i < NUM_SPARKLES; i ++) {
		int r, g, b;
		if (sparkles[i].alive) continue;

		/* XXX this generates a square around the mouse pointer, should
		 * be a circle */

		sparkles[i].alive = 1;
		r = (rand() & 0xff);// | 0x70;
		g = (rand() & 0xff);// | 0x70;
		b = (rand() & 0xff);// | 0x70;
		sparkles[i].r = r;
		sparkles[i].g = g;
		sparkles[i].b = b;
		sparkles[i].br = r + 0x40 > 0xff ? 0xff : r + 0x40;
		sparkles[i].bg = g + 0x40 > 0xff ? 0xff : g + 0x40;
		sparkles[i].bb = b + 0x40 > 0xff ? 0xff : b + 0x40;
#ifdef SQUARE_DISTRIBUTION
		sparkles[i].x = pointer_x +
			(rand() % (SPARKLE_POS_VARIANCE * 2)) -
			SPARKLE_POS_VARIANCE;
		sparkles[i].y = pointer_y +
			(rand() % (SPARKLE_POS_VARIANCE * 2)) -
			SPARKLE_POS_VARIANCE;
#else /* CIRCLE DISTRIBUTION */
		{
			int r = rand() % SPARKLE_POS_VARIANCE;
			double t = random();

			sparkles[i].x = pointer_x + r * sin(t);
			sparkles[i].y = pointer_y + r * cos(t);
		}
#endif
			
		sparkles[i].death_time = now + SPARKLE_LIFETIME -
			SPARKLE_LIFETIME_VARIANCE +
			(rand() / (double) RAND_MAX *
				(SPARKLE_LIFETIME_VARIANCE * 2));

		change = 1;
		new_sparkles --;
	}
	return change;
}

static int kill_sparkles(double now)
{
	int i, change = 0;
	for (i = 0; i < NUM_SPARKLES; i ++) {
		if (now > sparkles[i].death_time &&
				sparkles[i].alive) {
			sparkles[i].alive = 0;
			change = 1;
		}
	}
	return change;
}

static void init_sparkles(void)
{
	int i;
	
	srand(time(NULL));

	for (i = 0; i < NUM_SPARKLES; i ++)
		sparkles[i].alive = 0;
}

static void deinit_sparkles(void)
{
}

static double get_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main(void)
{
	double last_frame, now, last_time_moved = 0;
	
	int stay_on_top_counter = 0;
	int old_x = 0, old_y = 0;
	
	if (init_gfx())
		return 1;

	init_sparkles();
	
	last_frame = get_time();
	while (1) {
		int x, y;
		int change = 0;
		
		handle_events();
		
		now = get_time();
		
		change = kill_sparkles(now);
		
		get_pointer_pos(&x, &y);
		if (x != old_x || y != old_y)
			last_time_moved = now;
		if (now < last_time_moved + GEN_SPARKLES_AFTER_STOP)
			change |= gen_sparkles(last_frame, now);
		if (change)
			redraw();

		last_frame = now;

		if (++ stay_on_top_counter > 10) {
			stay_on_top_counter = 0;
			stay_on_top();
		}

		old_x = x;
		old_y = y;
		
		usleep(10000);
	}

	deinit_sparkles();
	
	deinit_gfx();

	return 0;
}
