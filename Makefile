INSTALLDIR = /usr/bin

CC = gcc
X_LIBS =  -L/usr/X11R6/lib -lX11 -lXext -lm
X_CFLAGS =  -I/usr/X11R6/include
CFLAGS = -O3 -Wall $(X_CFLAGS)



all:
	@echo "Please specify the version of sparkler you want:"
	@echo "make sparkler-root"
	@echo "or"
	@echo "make sparkler-topwin"

sparkler-root: sparkler.c gfx-root.c
	$(CC) $(CFLAGS) $(X_LIBS) -o sparkler sparkler.c gfx-root.c

sparkler-topwin: sparkler.c gfx-topwin.c
	$(CC) $(CFLAGS) $(X_LIBS) -o sparkler sparkler.c gfx-topwin.c

install:
	cp sparkler $(DESTDIR)/$(INSTALLDIR)

.PHONY: sparkler-root sparkler-topwin all install
