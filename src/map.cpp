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
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <allegro.h>
#include "video.h"
#include "sound.h"
#include "pck.h"
#include "map.h"
#include "multiplay.h"
#include "platoon.h"
#include "pfxopen.h"

#define SCANGSIZE 4

//extern GEODATA mapdata;

unsigned short *Map::m_loftemp = NULL;
int Map::m_loftemp_num = 0;
char *Map::m_scang = NULL;
SPK *Map::scanbord = NULL;
PCK *Map::smoke = NULL, *Map::cursor = NULL, *Map::floorob = NULL;

//			  dirs		0  1  2  3  4  5  6  7
int Map::dir2ofs[8] = {1, 1, 0, -1, -1, -1, 0, 1};
//  y  x	  -1  0  1	x
char Map::ofs2dir[3][3] = {{5, 6, 7},
                           {4, 8, 0},
                           {3, 2, 1}};

IMPLEMENT_PERSISTENCE(Map, "Map");

void Map::initpck()
{
	floorob  = new PCK("units/floorob.pck");
	cursor	 = new PCK("ufograph/cursor.pck");
	scanbord = new SPK("ufograph/scanbord.pck");
	smoke	 = new PCK("ufograph/smoke.pck");

	int fh = OPEN_ORIG("geodata/scang.dat", O_RDONLY | O_BINARY);
	assert(fh != -1);
	int fl = filelength(fh);
	m_scang = new char[fl];
	read(fh, m_scang, fl);
	close(fh);

	fh = OPEN_ORIG("geodata/loftemps.dat", O_RDONLY | O_BINARY);
	assert(fh != -1);
	fl = filelength(fh);
	m_loftemp = new unsigned short[fl / 2];
	read(fh, m_loftemp, fl);
	close(fh);
	m_loftemp_num = fl / 32;
}

void Map::freepck()
{
	delete floorob;
	delete cursor;
	delete scanbord;
	delete smoke;

	delete [] m_scang;
	delete [] m_loftemp;
}

Map::Map(GEODATA &mapdata)
{
	sel_row = 0; sel_col = 0; sel_lev = 0;

	memcpy(&gd, &mapdata, sizeof(gd));
	//cprintf("gd.x_size=%d gd.y_size=%d ", gd.x_size, gd.y_size);
	create(gd.z_size, gd.x_size, gd.y_size);

	//cprintf("gd.terrain=%d ", gd.terrain);
	loadterrain(gd.terrain);
	m_terrain_set = gd.terrain;
	loadmaps(gd.mapdata);

	//terrain = cultivat;
	//textout(screen, font, "Map::Map press key", 0,0, 1); readkey();
	build_visi();

	m_minimap_area = new MinimapArea(this, screen->w - SCREEN2W, SCREEN2H);
}

Map::Map(int l, int c, int r, int _ter, unsigned char *_map)
{
	sel_row = 0; sel_col = 0; sel_lev = 0;
	//memcpy(&gd, &mapdata, sizeof(gd));
	memset(&gd, 0, sizeof(gd));
	gd.z_size = l; gd.x_size = c; gd.y_size = r;
	gd.terrain = _ter;
	memcpy(&gd.mapdata, _map, sizeof(gd.mapdata));

	create(gd.z_size, gd.x_size, gd.y_size);

	loadterrain(gd.terrain);
	m_terrain_set = gd.terrain;
	loadmaps(gd.mapdata);

	m_minimap_area = new MinimapArea(this, screen->w - SCREEN2W, SCREEN2H);
}

Map::~Map()
{
	destroy();
}

void Map::create(int l, int w, int h)
{
	int i, j, k;
	level = l; width = w; height = h;

	m_cell = new Cell***[level];
	for (i = 0; i < level; i++) {
		m_cell[i] = new Cell ** [10 * width];
		for (j = 0; j < 10 * width; j++) {
			m_cell[i][j] = new Cell * [10 * height];
			for (k = 0; k < 10 * height; k++) {
				m_cell[i][j][k] = new Cell();
			}
		}
	}
}

void Map::destroy()
{
	int i, j, k;
	for (i = 0; i < level; i++) {
		for (j = 0; j < 10 * width; j++) {
			for (k = 0; k < 10 * width; k++) {
				delete m_cell[i][j][k];
			}
			delete [] m_cell[i][j];
		}
		delete [] m_cell[i];
	}

	delete [] m_cell;
	delete m_terrain;

	delete m_minimap_area;
}


void Map::loadterrain(int tid)
{
	static char *pcksets[10][4] =
    {
        { "jungle.pck",   NULL,           NULL,         NULL           },
        { "cultivat.pck", "barn.pck",     NULL,         NULL           },
        { "forest.pck",   NULL,           NULL,         NULL           },
        { "xbase1.pck",   "xbase2.pck",   NULL,         NULL           },
        { "u_base.pck",   "u_wall02.pck", "u_pods.pck", "brain.pck"    },
        { "roads.pck",    "urbits.pck",   "urban.pck",  "frniture.pck" },
        { "desert.pck",   NULL,           NULL,         NULL           },
        { "mount.pck",    NULL,           NULL,         NULL           },
        { "polar.pck",    NULL,           NULL,         NULL           },
        { "mars.pck",     "u_wall02.pck", NULL,         NULL           }
    };
	static int pcks_num[10] = { 1, 2, 1, 2, 4, 4, 1, 1, 1, 2 };

	m_terrain_set = tid;
	int terrain_num = pcks_num[tid] + 1;

	char pckname[100];
	strcpy(pckname, "terrain/blanks.pck");
	m_terrain = new TerraPCK(pckname);
	for (int i = 1; i < terrain_num; i++) {
		sprintf(pckname, "terrain/%s", pcksets[tid][i - 1]);
		m_terrain->add(pckname);
	}
}

void Map::loadmaps(unsigned char *_map)
{
	static char *bname[10] =
	{
		"jungle", "culta",  "forest", "xbase_", "ubase_",
		"urban",  "desert", "mount",  "polar",  "mars"
	};
	char mname[100];
	int i = 0;

	for (int col = width - 1; col >= 0; col--) {
		for (int row = height - 1; row >= 0; row--) {
			if (_map[i] != 0xFE) {
				sprintf(mname, "maps/%s%02d.map", bname[m_terrain_set], _map[i]);
				loadmap(mname, row * 10, col * 10);
			}
			i++;
		}
	}
}

int Map::loadmap(const char *fname, int _r, int _c)
{
	char mbuf[10000];
	int fh = OPEN_ORIG(fname, O_RDONLY | O_BINARY);
	if (fh == -1) return 0;
	read(fh, mbuf, 10000);
	close(fh);

	int w = mbuf[0];
	int h = mbuf[1];
	int l = mbuf[2];

	for (int lev = 0; lev < l; lev++) {
		int i = 3 + (l - lev - 1) * 4 * w * h;

		for (int col = _c + w - 1 - (w - 10); col >= _c - (w - 10); col--) {
			for (int row = _r - (h - 10); row < _r + h - (h - 10); row++) {
				m_cell[lev][col][row]->type[0] = mbuf[i + 0];
				m_cell[lev][col][row]->type[1] = mbuf[i + 1];
				m_cell[lev][col][row]->type[2] = mbuf[i + 2];
				m_cell[lev][col][row]->type[3] = mbuf[i + 3];
				m_cell[lev][col][row]->set_soldier(NULL);
				i += 4;
			}
		}
	}
	return 1;
}

void Map::drawitem(int itype, int gx, int gy)
{
	floorob->showpck(itype, gx, gy);
}


void Map::draw_cell_pck(int _x, int _y, int _lev, int _col, int _row, int _type, int _seen)
{
	int i = m_cell[_lev][_col][_row]->type[_type];
	assert(m_terrain->mcdstart(i) >= 0);
	int frame = m_terrain->m_mcd[i].Frame[0]; // + m_terrain->mcdstart(i);

	assert(frame < m_terrain->m_imgnum);

	if (frame && frame < m_terrain->m_imgnum) {
		if (_seen)
			m_terrain->showpck(frame, _x, _y);
		else
			m_terrain->showblackpck(frame, _x, _y);
	}
}


void Map::draw()
{
	int sx, sy;
	int mtype = 0;

	m_cell[sel_lev][sel_col][sel_row]->MOUSE = 1;

	int r1 = 0, r2 = height * 10 - 1;
	int c1 = 0, c2 = width * 10 - 1;
	int l1 = 0, l2;
	if (FLAGS & F_SHOWLEVELS)
		l2 = level - 1;
	else
		l2 = sel_lev;


	for (int lev = l1; lev <= l2; lev++) {
		for (int row = r1; row <= r2; row++) {
			for (int col = c2; col >= c1; col--) {
				sx = x + CELL_SCR_X * col + CELL_SCR_X * row;
				sy = y - (col) * CELL_SCR_Y + CELL_SCR_Y * row - 26 - lev * CELL_SCR_Z - 1;

				if ((sx > -32) && (sx < SCREEN2W) && (sy >= -34) && (sy < SCREEN2H)) {

					draw_cell_pck(sx, sy, lev, col, row, 0, seen(lev, col, row));

					if (m_cell[sel_lev][col][row]->MOUSE) {
						if (lev == sel_lev) {
							if ((m_cell[lev][col][row]->soldier_here()) && (visible(lev, col, row)))
								mtype = 1;
							else
								mtype = 0;
							if ((!TARGET) || (target.action == THROW))
								cursor->showpck(mtype, sx, sy);
						} else if (lev < sel_lev) {
							cursor->showpck(2, sx, sy);
						}
					}

					if (seen(lev, col, row)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 1);
						draw_cell_pck(sx, sy, lev, col, row, 2, 1);
					} else if ((mcd(lev, col, row, 1)->Door || mcd(lev, col, row, 1)->UFO_Door) &&
					        (row > 0) && seen(lev, col, row - 1)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 1);
						draw_cell_pck(sx, sy, lev, col, row, 2, 0);
					} else if ((mcd(lev, col, row, 2)->Door || mcd(lev, col, row, 2)->UFO_Door) &&
					        (col < width * 10 - 1) && seen(lev, col + 1, row)) {
						draw_cell_pck(sx, sy, lev, col, row, 1, 0);
						draw_cell_pck(sx, sy, lev, col, row, 2, 1);
					} else {
						draw_cell_pck(sx, sy, lev, col, row, 1, 0);
						draw_cell_pck(sx, sy, lev, col, row, 2, 0);
					}

					draw_cell_pck(sx, sy, lev, col, row, 3, seen(lev, col, row));

					if (seen(lev, col, row)) {
						int gy = sy + mcd(lev, col, row, 0)->T_Level;
						gy += mcd(lev, col, row, 3)->T_Level;
						m_cell[lev][col][row]->get_place()->draw(sx, gy);
					}

					if (visible(lev, col, row)) {
						if (m_cell[lev][col][row]->soldier_here())
							m_cell[lev][col][row]->get_soldier()->draw();

						if (lev < level - 1) {
							if (m_cell[lev + 1][col][row]->soldier_here()) {
								if (isStairs(lev, col, row)) {
									m_cell[lev + 1][col][row]->get_soldier()->draw();
								}
							}
						}
					}

					if (m_cell[sel_lev][col][row]->MOUSE) {
						if (lev == sel_lev) {
							if ((TARGET) && (target.action == THROW))
								cursor->showpck(15, sx, sy);
							if ((TARGET) && (target.action != THROW))
								mtype += 6;
							else
								mtype += 3;
							cursor->showpck(mtype, sx, sy);
						} else if (lev < sel_lev) {
							cursor->showpck(5, sx, sy);
						}
					}

					if (seen(lev, col, row)) {
 						int s = fire_state(lev, col, row);
 						if (fire_time(lev,col,row)>0) {
 							smoke->showpck(8-s, sx, sy);
 						} else {
 							s = smog_state(lev, col, row);
 							if (smog_time(lev,col,row)>0)
 								smoke->showpck(s-1, sx, sy);
 						}
 					}
				}
			}
		}
	}

	m_cell[sel_lev][sel_col][sel_row]->MOUSE = 0;
}


void Map::step()
{
 	for(int k=0; k<level;k++)
 		for(int i=0; i<10*width;i++)
 			for(int j=0; j<10*height;j++)
 				if (fire_time(k,i,j)>0) {
 					dec_fire_time(k,i,j);
 					for (int h=0; h<4; h++)
 						damage_cell_part(k,i,j,h,PISTOL_CLIP);
 					if (man(k, i, j) != NULL)
 						man(k, i, j)->hit(10);
 					if (fire_time(k,i,j) > 1)
 						set_fire_state(k,i,j,1);
 					if (fire_time(k,i,j) == 1)
 						set_fire_state(k,i,j,5);
 					if (fire_time(k,i,j) <= 0)
 						set_fire_state(k,i,j,0);
 				} else {
 					if (smog_time(k, i, j)>0) {
 						dec_smog_time(k, i, j);
 						if (smog_time(k, i, j) > 2)
 							set_smog_state(k, i, j, 17);
 						if (smog_time(k, i, j) == 2)
 							set_smog_state(k, i, j, 13);
 						if (smog_time(k, i, j) == 1)
 							set_smog_state(k, i, j, 9);
 						if (smog_time(k, i, j) == 0)
 							set_smog_state(k, i, j, 0);
 					}
				}
}

void Map::smoker()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10 * width; i++)
			for (int j = 0; j < 10 * height; j++)
				cell(k, i, j)->cycle_smoke();
}


#define Cx 1
#define Cy -8

void Map::set_sel(int mx, int my)
{
	my += sel_lev * CELL_SCR_Z;      //!!
	sel_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx ;
	sel_row = (my - 3) - y + 8 - Cy + sel_col / 4;
	sel_row /= CELL_SCR_Y; sel_col /= CELL_SCR_X * 2; // 32;

	if (sel_col < 0) sel_col = 0;
	if (sel_row < 0) sel_row = 0;
	if (sel_col >= 10 * width) sel_col = 10 * width - 1;
	if (sel_row >= 10 * height) sel_row = 10 * height - 1;
//	text_mode(0); textprintf(screen, font, SCREEN2W + 8, 0, xcom1_color(1), "x=%02d, y=%02d", sel_col, sel_row);
}


void Map::move(int ofs_x, int ofs_y)
{
	int sx = x, sy = y;
	x += ofs_x;
	y += ofs_y;

	int mx = SCREEN2W / 2;
	int my = SCREEN2H / 2;

	int center_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx ;
	int center_row = (my - 3) - y + 8 - Cy + center_col / 4;
	center_row >>= 3; center_col >>= 5;

	if ((center_col < 0) || (center_row < 0) ||
	        (center_col >= 10 * width) || (center_row >= 10 * height)) {
		x = sx; y = sy;
	}
}

#define S 2

void Map::svga2d()
{
	m_minimap_area->redraw(screen, SCREEN2W, 0);
}

void Map::draw2d()
{
	char scang4x4[16];

	int cx = SCREEN2W / 2;
	int cy = SCREEN2H / 2;

	set_sel(cx, cy);

	int r1 = sel_row - 18; if (r1 < 0) r1 = 0;
	int r2 = sel_row + 18; if (r2 >= height * 10) r2 = height * 10 - 1;

	int c1 = sel_col - 27; if (c1 < 0) c1 = 0;
	int c2 = sel_col + 27; if (c2 >= width * 10) c2 = width * 10 - 1;

	for (int lev = 0; lev <= sel_lev; lev++)
		for (int row = r1; row <= r2; row++)
			for (int col = c1; col <= c2; col++) {
				if (seen(lev, col, row)) {
					memset(scang4x4, 0, 16);
					for (int j = 0; j < 4; j++) {
						int ct = m_cell[lev][col][row]->type[j];
						int mt = m_terrain->m_mcd[ct].ScanG;
						if (mt > 0) {
							mt += 35;

							for (int i = 0; i < 16; i++)
								if (m_scang[mt * 16 + i])
									//putpixel(screen2, X+col*4+(3-i/4), Y+row*4+i%4, m_scang[mt*16+i]);
									scang4x4[i] = m_scang[mt * 16 + i];
						}
					}

					for (int i = 0; i < 16; i++)
						if (scang4x4[i])
							putpixel(screen2,
							         cx + ( -sel_col + col) * 4 + (3 - i / 4),
							         cy + ( -sel_row + row) * 4 + i % 4, scang4x4[i]);

					if (man(lev, col, row) != NULL) {
						if (platoon_local->belong(man(lev, col, row)))
							rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
							         cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
							         xcom1_color(144));      //yellow
						else
							if (visible(lev, col, row))
								rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
								         cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
								         xcom1_color(32));      //red
					}
				}
			}

	scanbord->show(screen2, cx - 160 + 4, cy - 100 + 12);
	text_mode( -1);
	textprintf(screen2, large, cx - 160 + 281 + 4, cy - 100 + 74 + 12, xcom1_color(66), "%d", sel_lev);
}


BITMAP *Map::create_bitmap_of_map()
{
	char scang4x4[16];
	BITMAP *bmp = create_bitmap(width * 10 * 4, height * 10 * 4);

	for (int lev = 0; lev <= sel_lev; lev++) {
		for (int row = 0; row < height*10; row++) {
			for (int col = 0; col < width*10; col++) {
				memset(scang4x4, 0, 16);
				for (int j = 0; j < 4; j++) {
					int mt = mcd(lev, col, row, j)->ScanG;
					if (mt > 0) {
						mt += 35;
						for (int i = 0; i < 16; i++)
							if (m_scang[mt * 16 + i])
								scang4x4[i] = m_scang[mt * 16 + i];
					}
				}

				for (int i = 0; i < 16; i++)
					if (scang4x4[i])
						putpixel(bmp, col * 4 + (3 - i / 4), row * 4 + i % 4, xcom1_color(scang4x4[i]));
			}
		}
	}
	return bmp;
}


int Map::center2d(int xx, int yy)
{
	int cx = SCREEN2W / 2;
	int cy = SCREEN2H / 2;
	xx -= cx - 160 + 4;
	yy -= cy - 100 + 11;

	//if ((xx<X)||(xx>X+width*10*SCANGSIZE) || (yy<Y)||(yy>Y+height*10*SCANGSIZE)) {
	if ((xx > 278) && (yy > 148) && (xx < 302) && (yy < 173)) { //OK
		return 1;
	}
	if ((xx > 24) && (yy > 62) && (xx < 40) && (yy < 81)) { //Up
		if (sel_lev < level - 1)
			sel_lev++;
		return 0;
	}
	if ((xx > 24) && (yy > 88) && (xx < 40) && (yy < 107)) { //Down
		if (sel_lev > 0)
			sel_lev--;
		return 0;
	}

	//	return 0;
	//}
	int col, row;
	col = sel_col - (160 - xx - 4) / SCANGSIZE;
	row = sel_row - (100 - yy - 11) / SCANGSIZE;
	center(0, col, row);
	return 0;
}

void Map::clearvis()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10*width; i++)
			for (int j = 0; j < 10*height; j++) {
				set_visible(k, i, j, 0);
			}
}

void Map::clearseen()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10*width; i++)
			for (int j = 0; j < 10*height; j++)
				set_seen(k, i, j, 0);
}

void Map::unhide()
{
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10*width; i++)
			for (int j = 0; j < 10*height; j++)
				set_seen(k, i, j, 1);
}

void Map::center(int lev, int col, int row)
{
	sel_lev = lev;
	x = SCREEN2W / 2 - CELL_SCR_X - CELL_SCR_X * col - CELL_SCR_X * row;     	  //320	 320/2-20
	y = SCREEN2H / 2 + CELL_SCR_Y * (col + 1) - CELL_SCR_Y * row + CELL_SCR_Z * lev;     	  //200	 200/2-8
}

int Map::stopLOS_level(int dx, int dy, int lev, int col, int row)
{
	if ((col + dx < 0) || (col + dx >= width * 10) ||
	        (row + dy < 0) || (row + dx >= height * 10)
	   )
		return 1;
	int ld = ofs2dir[dy + 1][dx + 1];
	if (viewable(lev, col, row, ld))
		return 0;
	return 1;
}


int Map::haveGROUND(int lev, int col, int row)
{
	if ((lev < 0) || (lev >= level) ||
	        (col < 0) || (col >= width * 10) ||
	        (row < 0) || (row >= height * 10)
	   )
		return 0;

	int ct = m_cell[lev][col][row]->type[0];
	if (ct != 0)
		return 1;
	return 0;
}


void Map::build_visi_cell(int lev, int col, int row)
{
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				int dz = k - 1;
				int dx = i - 1;
				int dy = j - 1;

				switch (dz) {
					case 0:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
					case - 1:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						if ((lev > 0) && stopLOS_level(dx, dy, lev - 1, col, row))
							continue;
						if (haveGROUND(lev, col, row))
							continue;
						if (haveGROUND(lev, col + dx, row + dy))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
					case 1:
						if (stopLOS_level(dx, dy, lev, col, row))
							continue;
						if ((lev < level - 1) && stopLOS_level(dx, dy, lev + 1, col, row))
							continue;
						if ((lev < level - 1) && haveGROUND(lev + 1, col, row))
							continue;
						if ((lev < level - 1) && haveGROUND(lev + 1, col + dx, row + dy))
							continue;
						m_cell[lev][col][row]->visi[k][i][j] = 1;
						break;
				}
			}
		}
	}

	if (stopLOS(lev, col, row, 3))
		m_cell[lev][col][row]->visi[1][1][1] = 0;

}

void Map::build_visi()
{
	for (int lev = 0; lev < level; lev++) {
		for (int col = 0; col < width*10; col++) {
			for (int row = 0; row < height*10; row++) {
				build_visi_cell(lev, col, row);
			}
		}
	}
}

extern int RECALC_VISIBILITY;

void Map::rebuild_visi(int z, int x, int y)
{
	RECALC_VISIBILITY = 1;
	for (int lev = z - 1; lev <= z + 1; lev++) {
		if ((lev < 0) || (lev >= level))
			continue;
		for (int col = x - 1; col <= x + 1; col++) {
			if ((col < 0) || (col >= width * 10))
				continue;
			for (int row = y - 1; row <= y + 1; row++) {
				if ((row < 0) || (row >= height * 10))
					continue;
				build_visi_cell(lev, col, row);
			}
		}
	}
}

int Map::stopLOS(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 1;

	int ct = cell(oz, ox, oy)->type[part];
	return m_terrain->m_mcd[ct].Stop_LOS;
}

int Map::viewable(int oz, int ox, int oy, int dir)
{
	switch (dir) {
		case 0:
			if (stopLOS(oz, ox, oy, 2))
				return 0;
			break;
		case 1:
			if (stopLOS(oz, ox, oy, 2) || stopLOS(oz, ox + 1, oy + 1, 1) ||
			        stopLOS(oz, ox, oy + 1, 2) || stopLOS(oz, ox, oy + 1, 1))
				return 0;
			break;
		case 2:
			if (stopLOS(oz, ox, oy + 1, 1))
				return 0;
			break;
		case 3:
			if (stopLOS(oz, ox - 1, oy, 2) || stopLOS(oz, ox, oy + 1, 1) ||
			        stopLOS(oz, ox - 1, oy + 1, 2) || stopLOS(oz, ox - 1, oy + 1, 1))
				return 0;
			break;
		case 4:
			if (stopLOS(oz, ox - 1, oy, 2))
				return 0;
			break;
		case 5:
			if (stopLOS(oz, ox - 1, oy - 1, 2) || stopLOS(oz, ox, oy, 1) ||
			        stopLOS(oz, ox - 1, oy, 2) || stopLOS(oz, ox - 1, oy, 1))
				return 0;
			break;
		case 6:
			if (stopLOS(oz, ox, oy, 1))
				return 0;
			break;
		case 7:
			if (stopLOS(oz, ox, oy - 1, 2) || stopLOS(oz, ox + 1, oy, 1) ||
			        stopLOS(oz, ox, oy, 2) || stopLOS(oz, ox, oy, 1))
				return 0;
			break;
	}
	return 1;
}

int Map::viewable_further(int vz, int vx, int vy)
{
	if (stopLOS(vz, vx, vy, 0) || stopLOS(vz, vx, vy, 3))
		return 0;
	if (isStairs(vz, vx, vy))
		return 0;

	return 1;
}


int Map::stopWALK(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 1;

	int ct = cell(oz, ox, oy)->type[part];

	if (m_terrain->m_mcd[ct].TU_Walk == 255) return 1;

	if ((part == 0 || part == 3) && pfval(oz, ox, oy) != 0)	{
		if (man(oz, ox, oy)) return 1;
		if (isStairs(oz, ox, oy) && man(oz + 1, ox, oy)) return 1;
		for (int z = oz; z > 0 && mcd(z, ox, oy, 0)->No_Floor; z--) {
			if (man(z - 1, ox, oy)) return 1;
		}
	}

	return 0;
}

int Map::stopDOOR(int oz, int ox, int oy, int part)
{
	if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
		return 0;

	int ct = cell(oz, ox, oy)->type[part];
	if (m_terrain->m_mcd[ct].Door == 1)
		return 1;
	return 0;
}

int Map::passable(int oz, int ox, int oy)
{
	if (stopWALK(oz, ox, oy, 0) || stopWALK(oz, ox, oy, 3))
		return 0;
		return 1;
}

int Map::passable(int oz, int ox, int oy, int dir)
{
	if (!passable(oz, ox, oy)) return 0;

	switch (dir) {
		case DIR_EAST:
			if (stopWALK(oz, ox, oy, 2))
				return 0;
			if (stopDOOR(oz, ox, oy, 2))
				return 0;
			break;
		case DIR_NORTHEAST:
			if (stopWALK(oz, ox, oy, 2) || stopWALK(oz, ox + 1, oy + 1, 1) ||
			        stopWALK(oz, ox, oy + 1, 2) || stopWALK(oz, ox, oy + 1, 1)) {
				return 0;
			}
			break;
		case DIR_NORTH:
			if (stopWALK(oz, ox, oy + 1, 1))
				return 0;
			if (stopDOOR(oz, ox, oy + 1, 1))
				return 0;
			break;
		case DIR_NORTHWEST:
			if (stopWALK(oz, ox - 1, oy, 2) || stopWALK(oz, ox, oy + 1, 1) ||
			        stopWALK(oz, ox - 1, oy + 1, 2) || stopWALK(oz, ox - 1, oy + 1, 1)) {
				return 0;
			}
			break;
		case DIR_WEST:
			if (stopWALK(oz, ox - 1, oy, 2))
				return 0;
			if (stopDOOR(oz, ox - 1, oy, 2))
				return 0;
			break;
		case DIR_SOUTHWEST:
			if (stopWALK(oz, ox - 1, oy - 1, 2) || stopWALK(oz, ox, oy, 1) ||
			        stopWALK(oz, ox - 1, oy, 2) || stopWALK(oz, ox - 1, oy, 1)) {
				return 0;
			}
			break;
		case DIR_SOUTH:
			if (stopWALK(oz, ox, oy, 1))
				return 0;
			if (stopDOOR(oz, ox, oy, 1))
				return 0;
			break;
		case DIR_SOUTHEAST:
			if (stopWALK(oz, ox, oy - 1, 2) || stopWALK(oz, ox + 1, oy, 1) ||
			        stopWALK(oz, ox, oy, 2) || stopWALK(oz, ox, oy, 1)) {
				return 0;
			}
			break;
	}

	return 2;
}


extern int mapscroll;

int Map::scroll(int mx, int my)
{
	int r = 0;
	if (mx == 0) {
		move(mapscroll, 0); r = 1;
	}
	if (mx >= SCREEN2W - 1) {
		move( -mapscroll, 0); r = 1;
	}
	if (my == 0) {
		move(0, mapscroll); r = 1;
	}
	if (my >= SCREEN2H - 1) {
		move(0, -mapscroll); r = 1;
	}
	return r;
}


int Map::open_door(int z, int x, int y, int dir)
{
	int door = 0, doorx = 0, doory = 0, doorz = z;

	switch (dir) {
		case 0:
			if (stopDOOR(z, x, y, 2)) {
				doorx = x;
				doory = y;
				door = 2;
			}
			break;
		case 2:
			if (stopDOOR(z, x, y + 1, 1)) {
				doorx = x;
				doory = y + 1;
				door = 1;
			}
			break;
		case 4:
			if (stopDOOR(z, x - 1, y, 2)) {
				doorx = x - 1;
				doory = y;
				door = 2;
			}
			break;
		case 6:
			if (stopDOOR(z, x, y, 1)) {
				doorx = x;
				doory = y;
				door = 1;
			}
			break;
	}
	if (door > 0) {
		//m_cell[doorz][doorx][doory].type[door] = 0;

		int ct = cell(doorz, doorx, doory)->type[door];
		int mcd = m_terrain->m_mcd[ct].Alt_MCD;

		if (mcd > 0) {
			int ms = m_terrain->mcdstart(ct);
			if (ms != -1) {
				mcd = mcd + ms;

				m_cell[doorz][doorx][doory]->type[door] = 0;
				if (door == 2) {
					m_cell[doorz][doorx][doory + 1]->type[1] = mcd;
					rebuild_visi(doorz, doorx, doory + 1);
				} else {
					m_cell[doorz][doorx][doory - 1]->type[2] = mcd;
					rebuild_visi(doorz, doorx, doory - 1);
				}
			}

			rebuild_visi(doorz, doorx, doory);
		}

		return 1;
	}
	return 0;
}


void Map::build_lof_cell(int _z, int _x, int _y, unsigned short *lof_cell)
{
	memset(lof_cell, 0, 2 * 16 * 12);

	for (int lev = 0; lev < 12; lev++) {
		for (int part = 0; part < 4; part++) {
			int ct = m_cell[_z][_x][_y]->type[part];
			if (!ct)
				continue;

			int mcd = m_terrain->m_mcd[ct].LOFT[lev];

			for (int i = 0; i < 16; i++) {
				lof_cell[lev * 16 + i] |= m_loftemp[mcd * 16 + i];
			}
		}
	}
}

void Map::build_lof_cell_part(int _z, int _x, int _y, int _part, unsigned short *lof_cell)
{
	memset(lof_cell, 0, 2 * 16 * 12);

	for (int lev = 0; lev < 12; lev++) {
		int ct = m_cell[_z][_x][_y]->type[_part];
		if (!ct)
			continue;

		int mcd = m_terrain->m_mcd[ct].LOFT[lev];

		for (int i = 0; i < 16; i++) {
			lof_cell[lev * 16 + i] |= m_loftemp[mcd * 16 + i];
		}

	}
}

BITMAP *Map::create_lof_bitmap(int lev, int col, int row)
{
	unsigned short lof_cell[16 * 12];
	build_lof_cell(lev, col, row, lof_cell);

	BITMAP *bmp = create_bitmap(20 * 4, 20 * 3);
	clear_to_color(bmp, xcom1_color(0));

	int dir = -1, s = 0;     //, tl = 0;
	if (visible(lev, col, row))
		if (man(lev, col, row) != NULL) {
			dir = man(lev, col, row)->dir;
			if (man(lev, col, row)->m_state == SIT)
				s = 1;
			//tl = mcd(lev, col, row, 0)->T_Level;
			//tl += mcd(lev, col, row, 3)->T_Level;
		}

	for (int j = 0; j < 12; j++) {
		/*int Tlev = lev - tl/2;
		if (Tlev > 11) Tlev = 11;
		if (Tlev < 0) Tlev = 0;*/

		for (int i = 0; i < 16; i++) {
			unsigned short l = lof_cell[j * 16 + i];
			//textprintf(screen, font, SCREEN2W, SCREEN2H-80+i*8, 1, "%04x ", l);
			for (int k = 0; k < 16; k++) {
				/*if (l & 0x8000) {
					putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 1);
				} else {
					if ((dir != -1) && (Soldier::bof[s][dir][j][i][15-k]))
						putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 52);
					else
						putpixel(bmp, 2+k+(j/3)*20, 2+i+(2-j%3)*20, 12);
				}*/

				if (dir != -1) {
					if (Soldier::m_bof[s][dir][j][i][15 - k])
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, 52);
					else if (l & 0x8000)
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, 1);
					else
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, 12);
				} else {
					if (l & 0x8000)
						//putpixel(screen, SCREEN2W+k  +(j/3)*20, SCREEN2H+i  +(2 - j%3 )*20, 1);
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, 1);
					else
						//putpixel(screen, SCREEN2W+k  +(j/3)*20, SCREEN2H+i  +(2 - j%3 )*20, 12);
						putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, 12);
				}
				l <<= 1;
			}
		}

	}
	return bmp;
}


void Map::show_lof_cell()
{
	BITMAP *bmp = create_lof_bitmap(sel_lev, sel_col, sel_row);
	blit(bmp, screen, 0, 0, SCREEN2W, SCREEN2H - 70, bmp->w, bmp->h);
	destroy_bitmap(bmp);

	bmp = create_bitmap(30, 90); clear_to_color(bmp, xcom1_color(0));
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < 3; i++) {
			textprintf(bmp, font, 0, 80 - (k * 30 + i * 8), xcom1_color(1),
			           "%d%d%d",
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][0],
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][1],
			           m_cell[sel_lev][sel_col][sel_row]->visi[k][i][2]
			          );
		}
	}
	blit(bmp, screen, 0, 0, SCREEN2W + 90, SCREEN2H - 100, bmp->w, bmp->h);
	destroy_bitmap(bmp);
}


int Map::pass_lof_cell(int _z, int _x, int _y)
{
	int lev = _z / 12;
	int col = _x / 16;
	int row = _y / 16;

	unsigned short lof_cell[16 * 12];
	build_lof_cell(lev, col, row, lof_cell);

	lev = _z % 12;
	col = _x % 16;
	row = _y % 16;

	int v = lof_cell[lev * 16 + (15 - col)] << row;
	if (v & 0x8000)
		return 0;

	return 1;
}

int Map::pass_lof_cell_part(int _z, int _x, int _y, int _part)
{
	int lev = _z / 12;
	int col = _x / 16;
	int row = _y / 16;

	unsigned short lof_cell[16 * 12];
	build_lof_cell_part(lev, col, row, _part, lof_cell);

	lev = _z % 12;
	col = _x % 16;
	row = _y % 16;

	int v = lof_cell[lev * 16 + (15 - col)] << row;
	if (v & 0x8000)
		return 0;

	return 1;
}

void Map::destroy_cell_part(int lev, int col, int row, int _part)
{
	int ct = m_cell[lev][col][row]->type[_part];
	if (ct > 0) {
		int mcd = m_terrain->m_mcd[ct].Die_MCD;

		if (mcd > 0) {
			if ((_part == 0) && (lev > 0)) {
				m_cell[lev][col][row]->type[_part] = 0;
				return ;
			}


			int ms = m_terrain->mcdstart(ct);
			if (ms != -1) {
				mcd = mcd + ms;
				m_cell[lev][col][row]->type[_part] = mcd;
			} else
				m_cell[lev][col][row]->type[_part] = 0;
		} else
			m_cell[lev][col][row]->type[_part] = 0;

		rebuild_visi(lev, col, row);
	}
}

void Map::damage_cell_part(int lev, int col, int row, int _part, int _wtype)
{
	int dam = Item::obdata[_wtype].damage;
	if (mcd(lev, col, row, _part)->Armour < dam) {
		destroy_cell_part(lev, col, row, _part);
	}
}


void Map::apply_hit(int _z, int _x, int _y, int _wtype)
{
	if (!pass_lof_cell(_z, _x, _y)) {
		for (int i = 0; i < 4; i++)
			if (!pass_lof_cell_part(_z, _x, _y, i)) {
				int lev = _z / 12;
				int col = _x / 16;
				int row = _y / 16;
				damage_cell_part(lev, col, row, i, _wtype);
			}
	}
}


void Map::drawline(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d)
{
	int z0, x0, y0;
	int zd, xd, yd;
	int xg, yg;
	int xg2, yg2;

	x0 = x_s * 16 + 8; y0 = y_s * 16 + 8; z0 = z_s * 12 + 8;
	xd = x_d * 16 + 8; yd = y_d * 16 + 8; zd = z_d * 12 + 8;

	xg = x + x0 + y0;
	//yg = y - (x0+1.0)/2.0 + y0/2.0 - z0*23.5/12.0;
	yg = (int)(y - (x0 + 1) / 2.0 + y0 / 2.0 - z0 * 2.0 - 2);

	xg2 = x + xd + yd;
	//yg2 = y - (xd+1.0)/2.0 + yd/2.0 - zd*23.5/12.0;
	yg2 = (int)(y - (xd + 1) / 2.0 + yd / 2.0 - zd * 2.0 - 2);

	line(screen2, xg, yg, xg2, yg2, xcom1_color(144));
}



Place *Map::find_item(Item *it, int &lev, int &col, int &row)
{
	for (lev = 0; lev < level; lev++)
		for (col = 0; col < width*10; col++)
			for (row = 0; row < height*10; row++)
				if (m_cell[lev][col][row]->get_place()->isthere(it))
					return m_cell[lev][col][row]->get_place();
	return NULL;
}

int Map::find_place_num(Place *pl, int lev, int col, int row)
{
	if (man(lev, col, row) != NULL)
		return man(lev, col, row)->place(pl);
	if (pl == place(lev, col, row))
		return P_MAP;
	return -1;
}


int Map::inside(int z, int x, int y)
{
	if ((x < 0) || (x >= width * 10 * 16) ||
	        (y < 0) || (y >= height * 10 * 16) ||
	        (z < 0) || (z >= level * 12))
		return 0;
	return 1;
}

int Map::cell_inside(int z, int x, int y)
{
	if ((x < 0) || (x >= width * 10) ||
	        (y < 0) || (y >= height * 10) ||
	        (z < 0) || (z >= level))
		return 0;
	return 1;
}

int Map::find_ground(int lev, int col, int row)
{
	while (lev > 0) {
		if (!mcd(lev, col, row, 0)->No_Floor) {
			break;
		}
		lev--;
	}
	return lev;
}

void Map::update_visible_cells(char *visible_cells)
{
	int n = 0, k, i, j, width_10 = 10 * width, height_10 = 10 * height;
	for (k = 0; k < level; k++)
		for (i = 0; i < width_10; i++)
			for (j = 0; j < height_10; j++)
				set_visible(k, i, j, visible_cells[n++]);

	if (FLAGS & F_CLEARSEEN) {
		n = 0;
		for (k = 0; k < level; k++)
			for (i = 0; i < width_10; i++)
				for (j = 0; j < height_10; j++)
					set_seen(k, i, j, visible_cells[n++]);
	}
}

#define FI_STEP (PI / 64.0)
#define TE_STEP (PI / 64.0)

static char field[8 * 6*10 * 6*10];

int Map::calc_visible_cells(int z, int x, int y, int dir, char *visicells, int *ez, int *ex, int *ey)
{
	//fixed 3d
	set_seen(z, x, y, 1);
	visicells[z * width * 10 * height * 10 + x * width * 10 + y] = 1;
	if (z > 0) {
		if (isStairs(z - 1, x, y)) {
			set_seen(z - 1, x, y, 1);
			visicells[(z - 1) * width * 10 * height * 10 + x * width * 10 + y] = 1;
		}
	}

	memset(field, 0, level * width * 10 * height * 10);
	int en = 0;
	int ang = dir * 32;

	for (int fi = 32; fi <= 128 - 32; fi += 4) {
		fixed cos_fi = fcos(itofix(fi));
		fixed sin_fi = fsin(itofix(fi));

		for (int te = ang - 32; te <= ang + 32; te += 1) {
			fixed cos_te = fcos(itofix(te));
			fixed sin_te = fsin(itofix(te));

			int oz = z, ox = x, oy = y;
			int l;
			int vz, vx, vy;
			int smokeway = 0;

			for (l = 1; l < 18 - smokeway*3; l++) { /////////////from smoke

				vx = x + fixtoi(fmul(itofix(l), fmul(cos_te, sin_fi)));
				vy = y + fixtoi(fmul(itofix(l), fmul(sin_te, sin_fi)));
				vz = z + fixtoi(fmul(itofix(l), cos_fi));

				if (!cell_inside(vz, vx, vy))
					break;
				if ((vz == oz) && (vx == ox) && (vy == oy))
					continue;

				if (!m_cell[oz][ox][oy]->visi[vz - oz + 1][vx - ox + 1][vy - oy + 1]) break;

				visicells[vz * width * 10 * height * 10 + vx * width * 10 + vy] = 1;
				set_seen(vz, vx, vy, 1);

				if (field[vz * width * 10 * height * 10 + vx * width * 10 + vy] == 0)
					if (man(vz, vx, vy) != NULL) {
						if (!platoon_local->belong(man(vz, vx, vy))) {
							ez[en] = vz;
							ex[en] = vx;
							ey[en] = vy;
							en++;
						}
					}
				field[vz * width * 10 * height * 10 + vx * width * 10 + vy] = 1;

				oz = vz; ox = vx; oy = vy;

				if (!viewable_further(vz, vx, vy)) break;

				if (smog_state(vz, vx, vy) != 0) if (++smokeway > 2) break;
			}
		}
	}

	return en;
}


#define TE_STEP (PI / 64.0)

int Map::explode(int lev, int col, int row, int type, int maxrange, int damage)
{
	soundSystem::getInstance()->play(SS_CV_GRENADE_BANG);

	char *field = new char[level * width * 10 * height * 10];
	memset(field, 0, level * width * 10 * height * 10);

	for (int ll = lev - 1; ll <= lev + 1; ll++) {
		if ((ll < 0) || (ll >= level))
			continue;
		int range = maxrange - abs(lev - ll);
		if (range <= 0)
			continue;
		//break;

		//REAL fi = PI/2.0;
		for (REAL te = -PI; te < PI; te += TE_STEP) {
			int dam = damage;

			for (int l = 0; l < range; l++) {
				int nz = ll;
				int nx = col + (int)floor(l * cos(te) + 0.5);
				int ny = row + (int)floor(l * sin(te) + 0.5);

				if (!cell_inside(nz, nx, ny))
					break;

				dam -= (damage / (range + range / 2)) * l;
				if (dam < 1) dam = 1;

				if (field[nz * width * 10 * height * 10 + nx * width * 10 + ny] != 0)
					continue;
				field[nz * width * 10 * height * 10 + nx * width * 10 + ny] = 1;

				explocell(nz, nx, ny, dam, type);
			}
		}
	}
	delete (field);
	net->send_explode(lev, col, row, type, maxrange, damage);
	return 1;
}

bool Map::check_mine(int lev, int col, int row) 
{
	for (int c = col - 1; c <= col + 1; c++) {
		for (int r = row - 1; r <= row + 1; r++) {
			if (!cell_inside(lev, c, r)) continue;
			if (place(lev, c, r)->check_mine())
				return true;
		}
	}
	return false;
}

void Map::explocell(int lev, int col, int row, int damage, int type)
{
	set_smog_state(lev, col, row, 8);
	set_smog_time(lev, col, row, 0);

 	for(int i=0; i<4; i++) {
 		if (mcd(lev, col, row, i)->Fuel > smog_time(lev, col, row)) {
 			set_smog_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
 			if (mcd(lev, col, row, i)->Armour < damage) {

 				if ((type==INCENDIARY_ROCKET)|(type==AUTO_CANNON_I_AMMO)|(type==CANNON_I_AMMO)) {
 					set_fire_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
 					set_fire_state(lev, col, row, 4);
 				}
 			}
 		} else {
 			set_smog_time(lev,col,row,2);
 		}
 		if ((mcd(lev, col, row, i)->Armour < damage)&&(type!=STUN_MISSILE)) {
 			destroy_cell_part(lev, col, row, i);
 		}
 	}
	//place(_z, _x, _y)->damage_items(Item::obdata[_wtype].damage);
	place(lev, col, row)->damage_items(damage);

	if (man(lev, col, row) != NULL)
		man(lev, col, row)->explo_hit(damage);
}


int Map::eot_save(char *buf, int & buf_size)
{
	buf_size += sprintf(buf + buf_size, "\r\nmap level=%d, width=%d, height=%d\r\n", level, width, height);

	for (int k = 0; k < level; k++) {
		for (int i = 0; i < 10 * width; i++) {
			for (int j = 0; j < 10 * height; j++) {
				buf_size += sprintf(buf + buf_size, " %d_%d_%d_%d ", cell(k, i, j)->type[0], cell(k, i, j)->type[1], cell(k, i, j)->type[2], cell(k, i, j)->type[3]);
			}
			buf_size += sprintf(buf + buf_size, "\r\n");
		}
		buf_size += sprintf(buf + buf_size, "\r\n\r\n\r\n");
	}
	buf_size += saveitems(buf + buf_size);
	return buf_size;
}

int Map::saveitems(char *txt)
{
	int len = 0;
	for (int k = 0; k < level; k++)
		for (int i = 0; i < 10 * width; i++)
			for (int j = 0; j < 10 * height; j++)
				len += place(k, i, j)->save_items("(%d,%d,%d)", k, i, j, txt + len);

	return len;
}

//void Map::send_GEODATA() {
//	net->send_map_data(&gd);
//}

//void Map::recv_GEODATA(GEODATA *gd) {
//}
extern int MAP_WIDTH, MAP_HEIGHT;

void Map::new_GEODATA(GEODATA *md)
{
	if (MAP_WIDTH * MAP_HEIGHT > 36) {
		MAP_WIDTH = 5;
		MAP_HEIGHT = 5;
	}

	terrain_set->create_geodata(
		terrain_set->get_random_terrain_id(), MAP_WIDTH, MAP_HEIGHT, *md);
/*
	memset(md, 0, sizeof(*md));
	md->x_size = MAP_WIDTH;
	md->y_size = MAP_HEIGHT;
	md->z_size = 4;
	md->terrain = TERRAIN_INDEX;
	for (int i = 0; i < MAP_WIDTH*MAP_HEIGHT; i++) {
		if (rand() % 5 == 0)
			md->mapdata[i] = DEFAULT_TERRAIN;
		else
			md->mapdata[i] = rand() % TERRAIN_COUNT;
	}
*/
}

int Map::valid_GEODATA(GEODATA *md)
{
	if ((md->x_size * md->y_size > 36) || (md->x_size > 6) || (md->y_size > 6) ||
	        (md->z_size != 4) || (md->terrain != TERRAIN_INDEX)) return 0;
	return 1;
}

int Map::walk_time(int _z, int _x, int _y)
{
	return mcd(_z, _x , _y, 0)->TU_Walk + mcd(_z, _x , _y, 3)->TU_Walk;
}

void Map::set_map_data(int c, int r, char ter)
{
	//int i = 35 - (c * height + (height - r));
	int i = (width - c - 1) * height + (height - r - 1);
	gd.mapdata[i] = ter;
	//for(int col=width-1; col>=0; col--) {
	//	for(int row=height-1; row>=0; row--) {
}

void Map::save(char *fname)
{
	int fh = open(fname, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, S_IRUSR | S_IWUSR);
	assert(fh != -1);
	write(fh, &gd, sizeof(gd));
	close(fh);
}

void Map::load(char *fname)
{
	int fh = open(fname, O_RDONLY | O_BINARY);
	assert(fh != -1);
	read(fh, &gd, sizeof(gd));
	close(fh);

	loadmaps(gd.mapdata);
}

bool Map::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	for (int lev = 0; lev < level; lev++)
		for (int col = 0; col < 10 * width; col++)
			for (int row = 0; row < 10 * height; row++)
				PersistWriteObject(archive, m_cell[lev][col][row]);

	return true;
}

bool Map::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	m_cell = new Cell***[level];
	for (int i = 0; i < level; i++) {
		m_cell[i] = new Cell ** [10 * width];
		for (int j = 0; j < 10 * width; j++) {
			m_cell[i][j] = new Cell * [10 * height];
		}
	}

	for (int lev = 0; lev < level; lev++)
		for (int col = 0; col < 10 * width; col++)
			for (int row = 0; row < 10 * height; row++)
				PersistReadObject(archive, m_cell[lev][col][row]);

    loadterrain(m_terrain_set);

	m_minimap_area = new MinimapArea(this, screen->w - SCREEN2W, SCREEN2H);

	return true;
}

//////////////////////////////////////////////////////////////////////////////

#undef map

Terrain::Terrain(const char *fileprefix, const char *name, int rand_weight):
	m_name(name), m_rand_weight(rand_weight), m_blocks_count(0)
{
	while (true) {
		assert(m_blocks_count < 100);
		char fname[256];
		sprintf(fname, "maps/%s%02d.map", fileprefix, m_blocks_count);
		int fh = OPEN_ORIG(fname, O_RDONLY | O_BINARY);
		if (fh == -1) break;
		unsigned char tmp[3];
		read(fh, &tmp, sizeof(tmp));
		m_blocks.resize(m_blocks_count + 1);
		assert(tmp[0] % 10 == 0);
		assert(tmp[1] % 10 == 0);
		m_blocks[m_blocks_count].x_size      = tmp[0] / 10;
		m_blocks[m_blocks_count].y_size      = tmp[1] / 10;
		m_blocks[m_blocks_count].z_size      = tmp[2];
		m_blocks[m_blocks_count].rand_weight = 100;
		m_blocks_count++;
		close(fh);
	}
	assert(m_blocks_count > 0);
}

Terrain::~Terrain()
{
}

int Terrain::get_random_block()
{
	int randmax = 0, i;

	for (i = 0; i < m_blocks_count; i++)
		randmax += m_blocks[i].rand_weight;

    int randval = rand() % randmax;

	for (i = 0; i < m_blocks_count; i++) {
		randval -= m_blocks[i].rand_weight;
		if (randval < 0) return i;
	}
	assert(false);
	return -1;
}

bool Terrain::create_geodata(GEODATA &gd)
{
	char map[6][6];
	memset(map, 0xFF, 36);
	for (int y = 0; y < gd.y_size; y++)
		for (int x = 0; x < gd.x_size; x++) {
			if (map[y][x] != 0xFF) continue;

			int i;
			while (true) {
				i = get_random_block();
				if (x + m_blocks[i].x_size > gd.x_size || y + m_blocks[i].y_size > gd.y_size)
					continue;
				int no_overlap = true;
				for (int by = 0; by < m_blocks[i].y_size; by++)
					for (int bx = 0; bx < m_blocks[i].x_size; bx++)
						if (map[y + by][x + bx] != 0xFF)
							no_overlap = false;
				if (no_overlap) break;
			}

			for (int by = 0; by < m_blocks[i].y_size; by++)
				for (int bx = 0; bx < m_blocks[i].x_size; bx++) {
					map[y + by][x + bx] = 0xFE;
					gd.mapdata[(y + by) * gd.x_size + x + bx] = 0xFE;
				}

			map[y][x] = i;
			gd.mapdata[y * gd.x_size + x] = i;
		}
	return true;
}

//////////////////////////////////////////////////////////////////////////////

TerrainSet::TerrainSet()
{
	terrain[0] = new Terrain("jungle", "", 100);
	terrain[1] = new Terrain("culta", "", 200);
	terrain[2] = new Terrain("forest", "", 0);
	terrain[3] = new Terrain("xbase_", "", 0);
	terrain[4] = new Terrain("ubase_", "", 0);
	terrain[5] = new Terrain("urban", "", 200);
	terrain[6] = new Terrain("desert", "", 0);
	terrain[7] = new Terrain("mount", "", 0);
	terrain[8] = new Terrain("polar", "", 100);
	terrain[9] = new Terrain("mars", "", 100);
}

TerrainSet::~TerrainSet()
{
	std::map<int, Terrain *>::iterator it;

	for (it = terrain.begin(); it != terrain.end(); ++it)
		delete it->second;
}

bool TerrainSet::create_geodata(int terrain_index, int x_size, int y_size, GEODATA &gd)
{
	if (terrain.find(terrain_index) == terrain.end()) return false;

	memset(&gd, 0, sizeof(gd));
	gd.terrain = terrain_index;
	gd.x_size  = x_size;
	gd.y_size  = y_size;
	gd.z_size  = 4;

	return terrain[terrain_index]->create_geodata(gd);
}

int TerrainSet::get_random_terrain_id()
{
	int randmax = 0;
	std::map<int, Terrain *>::iterator it;

	for (it = terrain.begin(); it != terrain.end(); ++it)
		randmax += it->second->get_rand_weight();

    int randval = rand() % randmax;

	for (it = terrain.begin(); it != terrain.end(); ++it) {
		randval -= it->second->get_rand_weight();
		if (randval < 0) return it->first;
	}
	assert(false);
	return -1;
}

#define map ufo2000_map
