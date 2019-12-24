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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/Xatom.h>

#include "vroot.h"

#include "gfx.h"
#include "sparkler.h"

static Display *display;
static int screen;
static Window window;
static unsigned int depth;
static Pixmap orig_background;
static Pixmap pixmap;
static GC gc;
static int width;
static int height;


static void draw_sparkle(int i)
{
	struct sparkle s = sparkles[i];
	XPoint points[16];

	if (s.b & 1) {
		XSetForeground(display, gc, 0xffffff);
		XDrawPoint(display, pixmap, gc, s.x, s.y);
	
		XSetForeground(display, gc, (s.r << 16) | (s.g << 8) | s.b);
		points[ 0].x = s.x - 1;	points[ 0].y = s.y;
		points[ 1].x = s.x - 2;	points[ 1].y = s.y;
		points[ 2].x = s.x - 3;	points[ 2].y = s.y;
		points[ 3].x = s.x + 1;	points[ 3].y = s.y;
		points[ 4].x = s.x + 2;	points[ 4].y = s.y;
		points[ 5].x = s.x + 3;	points[ 5].y = s.y;
		points[ 6].x = s.x;	points[ 6].y = s.y - 1;
		points[ 7].x = s.x;	points[ 7].y = s.y - 2;
		points[ 8].x = s.x;	points[ 8].y = s.y - 3;
		points[ 9].x = s.x;	points[ 9].y = s.y + 1;
		points[10].x = s.x;	points[10].y = s.y + 2;
		points[11].x = s.x;	points[11].y = s.y + 3;
		XDrawPoints(display, pixmap, gc, points, 12, CoordModeOrigin);
	} else {
		XSetForeground(display, gc, 0xffffff);
		XDrawPoint(display, pixmap, gc, s.x, s.y);
	
		XSetForeground(display, gc, (s.r << 16) | (s.g << 8) | s.b);
		points[0].x = s.x - 1;	points[0].y = s.y;
		points[1].x = s.x;	points[1].y = s.y - 1;
		points[2].x = s.x + 1;	points[2].y = s.y;
		points[3].x = s.x;	points[3].y = s.y + 1;
		XDrawPoints(display, pixmap, gc, points, 4, CoordModeOrigin);
	
		XSetForeground(display, gc, ((s.r / 2) << 16) |
				((s.g / 2) << 8) | (s.b / 2));
		points[0].x = s.x - 2;	points[0].y = s.y;
		points[1].x = s.x;	points[1].y = s.y - 2;
		points[2].x = s.x + 2;	points[2].y = s.y;
		points[3].x = s.x;	points[3].y = s.y + 2;
		points[4].x = s.x - 1;	points[4].y = s.y - 1;
		points[5].x = s.x + 1;	points[5].y = s.y - 1;
		points[6].x = s.x + 1;	points[6].y = s.y + 1;
		points[7].x = s.x - 1;	points[7].y = s.y + 1;	
		XDrawPoints(display, pixmap, gc, points, 8, CoordModeOrigin);
	}
	
}

void redraw(void)
{
	int i;
	
	XCopyArea(display, orig_background, pixmap, gc,
			0, 0, width, height,
			0, 0);
	
	for (i = 0; i < NUM_SPARKLES; i ++)
		if (sparkles[i].alive)
			draw_sparkle(i);
	
	XCopyArea(display, pixmap, window, gc,
			0, 0, width, height,
			0, 0);
	
	XFlush(display);
}

int init_gfx(void) 
{
	display = XOpenDisplay(getenv("DISPLAY"));
	if (!display) {
		fprintf(stderr, "Cannot oped display.");
		return -1;
	}
	
	screen = XDefaultScreen(display);
	depth = DefaultDepth(display, screen);
	
	width = XDisplayWidth(display, screen); 
	height = XDisplayHeight(display, screen); 
	window = RootWindow(display, screen);

	orig_background = XCreatePixmap(display, window,
			width, height, depth);
	pixmap = XCreatePixmap(display, window,
			width, height, depth);
	gc = XCreateGC(display, window, 0, NULL);

	XSelectInput(display, window, NoEventMask);

	XSetSubwindowMode(display, gc, ClipByChildren);
	
	XCopyArea(display, window, orig_background, gc,
			0, 0, width, height,
			0, 0);
	
	return 0;
}


void deinit_gfx(void)
{
	/* restore original background */
	XCopyArea(display, orig_background, window, gc,
			0, 0, width, height,
			0, 0);
	
	XFreeGC(display, gc);
	XFreePixmap(display, orig_background);
	XFreePixmap(display, pixmap);

	XCloseDisplay(display);
}

void get_pointer_pos(int *x, int *y)
{
	Window root_return, child_return;
	int root_x_return, root_y_return;
	unsigned int mask_return;

	XQueryPointer(display, window, &root_return, &child_return,
			&root_x_return, &root_y_return,
			x, y, &mask_return);
}

void handle_events(void)
{
	while (XQLength(display) > 0) {
		XEvent event;
		XNextEvent(display, &event);
	}
}

void stay_on_top(void)
{
}
