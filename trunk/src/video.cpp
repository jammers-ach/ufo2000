/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002       ufo2000 development team

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
#include "global.h"
#include <assert.h>
#include <stdio.h>

#ifdef LINUX
#include <stdlib.h>
#include <unistd.h>
#endif

#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "video.h"
#ifdef WIN32
#include <allegro/internal/aintern.h>
#include <allegro/platform/aintwin.h>
#endif
#include "wind.h"
#include "../ufo2000.h"
#include "pfxopen.h"

unsigned long FLAGS = 0;
/*
extern Wind *info;
*/

#ifdef DJGPP
BEGIN_COLOR_DEPTH_LIST
COLOR_DEPTH_8
END_COLOR_DEPTH_LIST
/*
BEGIN_GFX_DRIVER_LIST
	GFX_DRIVER_VGA
	GFX_DRIVER_VESA1
END_GFX_DRIVER_LIST
*/
#endif

//PALLETE gamepal;
RGB *gamepal;
DATAFILE *datafile;

BITMAP *mouser, *selector;
BITMAP *screen2;
BITMAP *screen_text;

char *palettes;
int palettes_size;

int SCREEN2W = 320, SCREEN2H = 200;

volatile int DRAWIT = 0;
void drawit_timer()
{
	DRAWIT++;
}
END_OF_FUNCTION(drawit_timer);

int REDRAW = 0;
#ifdef WIN32
static void switch_in_callback()
{
	/*
		REDRAW++;
		//directdraw->RestoreAllSurfaces();
	//	dd_prim_surface->Restore();
		info->redraw_full();
	*/
	return;
}

static void switch_out_callback()
{
	/*
		info->redraw_full();
	*/
	return;
}

#endif

void initvideo()
{
	LOCK_VARIABLE(DRAWIT);
	LOCK_FUNCTION(drawit_timer);

	selector = (BITMAP*)datafile[DAT_SELECT].dat;
	mouser = (BITMAP*)datafile[DAT_MOUSE].dat;

	screen2 = create_bitmap(SCREEN2W, SCREEN2H);
	clear(screen2);
	screen_text = create_bitmap(640, 400 - 10 - SCREEN2H);
	clear(screen_text);

	initpal("geodata/palettes.dat");

	font = (FONT*)datafile[DAT_UNIFONT_TXT].dat;
	create_small_font();
	create_large_font();

	if (FLAGS & F_LARGEFONT) {
		font = large;
	}
	
	if (FLAGS & F_SMALLFONT) {
		
// crude hack around a some idiot's #define in rpcndr.h file, apparently from WIN32 SDK.
// instead of using typedef these bozos put a '#define small char'  there. 
#undef small		
		font = small;
	}
	
}

void closevideo()
{
	free_large_font();
	free_small_font();

	delete [] palettes;

	destroy_bitmap(screen_text);
	destroy_bitmap(screen2);
}

void change_screen_mode()
{
	if (FLAGS & F_SWITCHVIDEO) {
		if (FLAGS & F_FULLSCREEN) {
			FLAGS &= ~F_FULLSCREEN;
		} else {
			FLAGS |= F_FULLSCREEN;
		}
		set_video_mode();
		reset_video();
		REDRAW = 1;
	}
}

void set_video_mode()
{
#ifdef USE_HICOLOR
	set_color_depth(16);
	set_color_conversion(COLORCONV_TOTAL | COLORCONV_KEEP_TRANS);
#else
	set_color_depth(8);
#endif

#ifdef DJGPP
	set_gfx_mode(GFX_AUTODETECT, 640, 400, 0, 0);
#elif WIN32
	if (FLAGS & F_SAFEVIDEO) {
		if (FLAGS & F_FULLSCREEN)
			set_gfx_mode(GFX_DIRECTX_SAFE, 640, 400, 0, 0);
		else
			set_gfx_mode(GFX_GDI, 640, 400, 0, 0);
	} else {
		if (FLAGS & F_FULLSCREEN)
			set_gfx_mode(GFX_DIRECTX, 640, 400, 0, 0);
		else
			set_gfx_mode(GFX_DIRECTX_WIN, 640, 400, 0, 0);
	}
	set_display_switch_mode(SWITCH_AMNESIA);
	//	set_display_switch_callback(SWITCH_IN, switch_in_callback);
	//	set_display_switch_callback(SWITCH_OUT, switch_out_callback);
#elif LINUX
	int got_display_env = (NULL != getenv("DISPLAY"));


#ifdef GFX_SVGALIB
	int got_root = (0 == geteuid());
	int force_svgalib = (NULL != getenv("UFO2000_FORCE_SVGALIB"));
	// allegro was compiled with svgalib driver
	if (force_svgalib) {
		if (got_display_env) {
			fprintf(stderr, "\nWarning: initializing svgalib while running X11.");
		}
		if (got_root) {
			set_gfx_mode(GFX_SVGALIB, 640, 400, 0, 0);
			return ;
		}
		fprintf(stderr, "\nNeed setuid root to use svgalib, got none.\n");
		exit(1);
	}
#endif

#ifdef GFX_XWINDOWS
	if (got_display_env) {
        if (FLAGS & F_FULLSCREEN) {
            set_gfx_mode(GFX_XWINDOWS_FULLSCREEN, 640, 400, 0, 0);
        } else {
            set_gfx_mode(GFX_XWINDOWS, 640, 400, 0, 0);
        }
		return ;
	} else
#endif
	{
#ifdef GFX_SVGALIB
		if (got_root) {
			set_gfx_mode(GFX_SVGALIB, 640, 400, 0, 0);
		} else {
			fprintf(stderr, "\nNeed setuid root to use svgalib, got none.\n");
			exit(1);
		}
#elif defined GFX_BEOS_FULLSCREEN_SAFE
		set_gfx_mode(GFX_BEOS_FULLSCREEN_SAFE, 640, 400, 0, 0);
#else
	fprintf(stderr, "\nNo $DISPLAY or X11 driver, no svgalib driver... I'm giving up. \n");
	exit(1);
#endif

	}

#ifndef GFX_BEOS_FULLSCREEN_SAFE
#ifndef GFX_XWINDOWS
#ifndef GFX_SVGALIB
#error Neither X11 nor svgalib drivers were compiled into allegro.
#endif
#endif
#endif

#else
#error Use one of -DLINUX -DWIN32 or -DDJGPP in the Makefile.
#endif
}

void reset_video()
{
	set_palette(gamepal);
	position_mouse(160, 100);
	set_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
	set_mouse_sprite(mouser);
	set_mouse_speed(1, 1);
	gui_fg_color = xcom1_color(15);
	gui_bg_color = xcom1_color(1);
	REDRAW = 1;
}

void initpal(char *fname)
{
	int fh = OPEN_ORIG(fname, O_RDONLY | O_BINARY);
	assert(fh != -1);
	palettes_size = filelength(fh);

	palettes = new char[palettes_size];
	palettes_size = read(fh, palettes, palettes_size);
	close(fh);

	gamepal = (RGB *)datafile[DAT_GAMEPAL].dat;
}

void setpal(RGB pal[PAL_SIZE], int pofs)
{
	//RGB pal[PAL_SIZE];
	int j = pofs;     //0xC18;

	for (int i = 0; i < PAL_SIZE; i++) {
		pal[i].r = palettes[j++];
		pal[i].g = palettes[j++];
		pal[i].b = palettes[j++];
		//pal[i].filler = 0;
	}
	set_palette(pal);
}

void savescreen()
{
	BITMAP *scr = create_bitmap(SCREEN_W, SCREEN_H);
	blit(screen, scr, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

	save_pcx("snapshot.pcx", scr, gamepal);
	destroy_bitmap(scr);
}

#define POLY 0x8408
//												  16	12	5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.

unsigned short crc16(char *data_p)
{
	unsigned char i;
	unsigned int data;
	unsigned int crc = 0xffff;
	unsigned short length = strlen(data_p);

	if (length == 0)
		return (~crc);
	do {
		for (i = 0, data = (unsigned int)0xff & *data_p++; i < 8; i++, data >>= 1) {
			if ((crc & 0x0001) ^ (data & 0x0001))
				crc = (crc >> 1) ^ POLY;
			else
				crc >>= 1;
		}
	} while (--length);

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | (data >> 8 & 0xff);

	return (crc);
}

#include "icon.h"

/*
extern Icon *icon;
extern Wind *info;
extern BITMAP *info_background;
extern BITMAP *logo;
*/

void resize_screen2(int vw, int vh)
{
	// shortcut to avoid screen flicker when at maximum/minimum size.
	if (((SCREEN2W <= 320) && (vw < 0)) || ((SCREEN2H <= 200) && (vh < 0)))
		return ;

	if (((SCREEN2W >= 640) && (vw > 0)) || ((SCREEN2H >= 400) && (vh > 0)))
		return ;

	SCREEN2W += vw; SCREEN2H += vh;
	if (SCREEN2W > 640) SCREEN2W = 640;
	if (SCREEN2W < 320) SCREEN2W = 320;
	if (SCREEN2H > 400) SCREEN2H = 400;
	if (SCREEN2H < 200) SCREEN2H = 200;

	destroy_bitmap(screen2);
	screen2 = create_bitmap(SCREEN2W, SCREEN2H);

	if (SCREEN2H < 390) {
		destroy_bitmap(screen_text);
		screen_text = create_bitmap(640, 400 - 10 - SCREEN2H);
		clear(screen_text);
	}
	/*	What are those three lines for?  It crashes if SCREEN2H > 390 without them. */

	clear(info_background);
	draw_sprite(info_background, logo, 640 - logo->w, SCREEN2H);
	blit(info_background, screen, 0, 0, 0, 0, 640, 400);
	delete(info);
	info = new Wind(info_background, 0, SCREEN2H, 640, 400, 1);
	icon->setxy((SCREEN2W - 320) / 2, SCREEN2H - 56);
	set_mouse_range(0, 0, SCREEN2W - 1, SCREEN2H - 1);
	position_mouse(SCREEN2W / 2, SCREEN2H / 2);
}

int askmenu(char *mess)
{
	int omx = mouse_x, omy = mouse_y;
	set_mouse_range(0, 0, 639, 399);

	int sel = alert(mess, "", "", "OK", "Cancel", 0, 0);
	//set_mouse_speed(2, 2);
	position_mouse(omx, omy);
	//set_mouse_range(0, 0, 319, 199);
	//set_mouse_range(0, 0, SCREEN2W-1, SCREEN2H-1);
	set_mouse_range(0, 0, screen2->w - 1, screen2->h - 1);
	if (sel == 1)
		return 1;
	return 0;
}