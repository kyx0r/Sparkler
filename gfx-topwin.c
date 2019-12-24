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

#include "gfx.h"
#include "sparkler.h"

static Display *display;
static int screen;
static Window window;
static unsigned int depth;
static Pixmap pixmap;
static Pixmap mask;
static GC gc;
static GC mask_gc;
static int width;
static int height;

static void draw_sparkle(int i)
{
	struct sparkle s = sparkles[i];
	XPoint points[16];
	
	if (s.b & 1) {
		XSetForeground(display, gc, 0xffffff);
		XDrawPoint(display, pixmap, gc, s.x, s.y);
		XDrawPoint(display, mask, mask_gc, s.x, s.y);
	
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
		XDrawPoints(display, mask, mask_gc, points, 12,
				CoordModeOrigin);
	} else {
		XSetForeground(display, gc, 0xffffff);
		XDrawPoint(display, pixmap, gc, s.x, s.y);
		XDrawPoint(display, mask, mask_gc, s.x, s.y);
	
		XSetForeground(display, gc, (s.r << 16) | (s.g << 8) | s.b);
		points[0].x = s.x - 1;	points[0].y = s.y;
		points[1].x = s.x;	points[1].y = s.y - 1;
		points[2].x = s.x + 1;	points[2].y = s.y;
		points[3].x = s.x;	points[3].y = s.y + 1;
		XDrawPoints(display, pixmap, gc, points, 4, CoordModeOrigin);
		XDrawPoints(display, mask, mask_gc, points, 4, CoordModeOrigin);
	
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
		XDrawPoints(display, mask, mask_gc, points, 8, CoordModeOrigin);
	}
}

void redraw(void)
{
	int i;
	
	XSetForeground(display, mask_gc, BlackPixel(display, screen));
	XFillRectangle(display, mask, mask_gc, 0, 0, width, height);
	XSetForeground(display, mask_gc, WhitePixel(display, screen));

	XSetForeground(display, gc, BlackPixel(display, screen));
	XFillRectangle(display, pixmap, gc, 0, 0, width, height);
	
	for (i = 0; i < NUM_SPARKLES; i ++)
		if (sparkles[i].alive)
			draw_sparkle(i);
	
	XCopyArea(display, pixmap, window, gc,
			0, 0, width, height,
			0, 0);
	XShapeCombineMask(display, window, ShapeBounding,
			0, 0, mask, ShapeSet);
	
	XFlush(display);
}

static Atom net_wm;
static Atom net_wm_state;
static Atom net_wm_top;

#define _NET_WM_STATE_ADD           1    /* add/set property */

/* tested with kde */
static void net_wm_stay_on_top(void)
{
	XEvent e;

	e.xclient.type = ClientMessage;
	e.xclient.message_type = net_wm_state;
	e.xclient.display = display;
	e.xclient.window = window;
	e.xclient.format = 32;
	e.xclient.data.l[0] = _NET_WM_STATE_ADD;
	e.xclient.data.l[1] = net_wm_top;
	e.xclient.data.l[2] = 0l;
	e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;

	XSendEvent(display, DefaultRootWindow(display), False,
			SubstructureRedirectMask, &e);
}

/* ------------------------------------------------------------------------ */

static Atom gnome;
static Atom gnome_layer;

#define WIN_LAYER_ONTOP                  6

/* tested with icewm + WindowMaker */
static void gnome_stay_on_top(void)
{
	XClientMessageEvent xev;

	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.window = window;
	xev.message_type = gnome_layer;
	xev.format = 32;
	xev.data.l[0] = WIN_LAYER_ONTOP;

	XSendEvent(display, DefaultRootWindow(display), False,
			SubstructureNotifyMask, (XEvent *)&xev);

}

/* ------------------------------------------------------------------------ */

void stay_on_top(void)
{
	Atom            type;
	int             format;
	unsigned long   nitems, bytesafter;
	unsigned char  *args = NULL;
	Window root = DefaultRootWindow(display);

	/* build atoms */
	net_wm       = XInternAtom(display, "_NET_SUPPORTED", False);
	net_wm_state = XInternAtom(display, "_NET_WM_STATE", False);
	net_wm_top   = XInternAtom(display, "_NET_WM_STATE_STAYS_ON_TOP",
			False);
	gnome        = XInternAtom(display, "_WIN_SUPPORTING_WM_CHECK", False);
	gnome_layer  = XInternAtom(display, "_WIN_LAYER", False);

	/* gnome-compilant */
	if (Success == XGetWindowProperty(display, root, gnome,
				0, (65536 / sizeof(long)), False,
				AnyPropertyType, &type, &format, &nitems,
				&bytesafter, &args) && nitems > 0) {
		/* FIXME: check capabilities */
		gnome_stay_on_top();
		XFree(args);
	}
	/* netwm compliant */
	else if (Success == XGetWindowProperty(display, root, net_wm,
				0, (65536 / sizeof(long)), False,
				AnyPropertyType, &type, &format, &nitems,
				&bytesafter, &args) && nitems > 0) {
		net_wm_stay_on_top();
		XFree(args);
	}
	XRaiseWindow(display, window);
}



int init_gfx(void) 
{
	XSetWindowAttributes setwinattr;

	display = XOpenDisplay(getenv("DISPLAY"));
	if (!display) {
		fprintf(stderr, "Cannot oped display.");
		return -1;
	}
	
	screen = XDefaultScreen(display);
	depth = DefaultDepth(display, screen);
	
	width = XDisplayWidth(display, screen); 
	height = XDisplayHeight(display, screen); 

	setwinattr.override_redirect = 1;
	
	window = XCreateWindow(display, XRootWindow(display, screen),
			0, 0, width, height,
			0, depth, CopyFromParent,
			DefaultVisual(display, screen),
			CWOverrideRedirect, &setwinattr);

	pixmap = XCreatePixmap(display, window,
			width, height, depth);
	gc = XCreateGC(display, window, 0, NULL);
	mask = XCreatePixmap(display, window,
			width, height, 1);
	mask_gc = XCreateGC(display, mask, 0, NULL);
	
	XSelectInput(display, window, NoEventMask);
	XSelectInput(display, window, 0xffffff);

	stay_on_top();

	XMapRaised(display, window);
	
	return 0;
}


void deinit_gfx(void)
{
	XFreeGC(display, gc);
	XFreeGC(display, mask_gc);
	XFreePixmap(display, mask);
	XFreePixmap(display, pixmap);
	
	XDestroyWindow(display, window);

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
