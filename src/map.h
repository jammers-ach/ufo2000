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
#ifndef MAP_H
#define MAP_H
#include <assert.h>
#include <vector>
#include <map>
#include "cell.h"
#include "pck.h"
#include "spk.h"
#include "terrapck.h"
#include "place.h"
#include "soldier.h"
#include "platoon.h"
#include "minimap.h"

enum DIRECTION
{
	DIR_EAST      = 0,
	DIR_NORTHEAST = 1,
	DIR_NORTH     = 2,
	DIR_NORTHWEST = 3,
	DIR_WEST      = 4,
	DIR_SOUTHWEST = 5,
	DIR_SOUTH     = 6,
	DIR_SOUTHEAST = 7
};

#define DIR_DELTA_X(dir) dir2ofs[dir]
#define DIR_DELTA_Y(dir) dir2ofs[(dir+6)%8]
#define DIR_REVERSE(dir) ((dir+4)%8)

class Map: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Map)
private: 
	static PCK *cursor;
	static SPK *scanbord;
	static char *m_scang;
	static unsigned short *m_loftemp;
	static int m_loftemp_num;

	GEODATA gd;
//	int m_terrain_num;
	int m_terrain_set;

	static int dir2ofs[8];
	static char ofs2dir[3][3];

	Cell ****m_cell;
	TerraPCK *m_terrain;

public:
	MinimapArea *m_minimap_area;

	static PCK *floorob, *smoke;
	static void initpck();
	static void freepck();
	static int valid_GEODATA(GEODATA *gd);
	static void new_GEODATA(GEODATA *mapdata);

	int level, width, height;
	int sel_lev, sel_row, sel_col;
	int x, y;

	Map() { }
	Map(GEODATA &mapdata);
	virtual ~Map();
	void create(int l, int w, int h);
	void destroy();

	//void loadgeodata(char *fname);
	void loadterrain(int tid);
	void loadmaps(unsigned char *_map);
	int loadmap(const char *fname, int _x, int _y);

	void drawitem(int itype, int gx, int gy);
	void draw_cell_pck(int _x, int _y, int _lev, int _col, int _row, int _type, int _seen);
	void draw();
	void draw2d();
	void svga2d();
	BITMAP *create_bitmap_of_map();

	void set();
	void move(int ofs_x, int ofs_y);
	void set_sel(int mx, int my);

	int ofs;

	void center(int lev, int col, int row);
	int center2d(int xx, int yy);

	void step();
	void smoker();
	int explode(int lev, int col, int row, int type, int range, int damage);
	void explocell(int lev, int col, int row, int damage, int type);
	bool check_mine(int lev, int col, int row);
	int stopWALK(int oz, int ox, int oy, int part);
	int stopDOOR(int oz, int ox, int oy, int part);
	int passable(int oz, int ox, int oy);
	int passable(int oz, int ox, int oy, int dir);

	int stopLOS(int oz, int ox, int oy, int part);
	int viewable(int oz, int ox, int oy, int dir);
	int viewable_further(int vz, int vx, int vy);

	int calc_visible_cells(int z, int x, int y, int dir, char *visicells, int *ez, int *ex, int *ey);
	void build_visi();
	void rebuild_visi(int z, int x, int y);
	void build_visi_cell(int lev, int col, int row);
	int stopLOS_level(int dx, int dy, int lev, int col, int row);
	int haveGROUND(int lev, int col, int row);
	void update_visible_cells(char *visible_cells);

	void clearvis();
	void clearseen();

	int scroll(int mx, int my);

	void build_lof_cell(int _z, int _x, int _y, unsigned short *lof_cell);
	void build_lof_cell_part(int _z, int _x, int _y, int _part, unsigned short *lof_cell);
	BITMAP *create_lof_bitmap(int lev, int col, int row);
	void show_lof_cell();
	int pass_lof_cell(int _z, int _x, int _y);
	int pass_lof_cell_part(int _z, int _x, int _y, int _part);

	int open_door(int z, int x, int y, int part);
	void destroy_cell_part(int lev, int col, int row, int _part);
	void damage_cell_part(int lev, int col, int row, int _part, int _type);
	void apply_hit(int _z, int _x, int _y, int _type);

	int findneibo(int &dz, int &dx, int &dy, int value);
	int setneibos(int oz, int ox, int oy, int value);
	int pathfind(int sz, int sx, int sy, int dz, int dx, int dy, char *way);
	void path_show(int _z, int _x, int _y, char *way, int waylen);
	void draw_path_from(Soldier *s);
	int walk_time(int z, int x, int y);

	void drawline(int z_s, int x_s, int y_s, int z_d, int x_d, int y_d);

	Place *find_item(Item *it, int &lev, int &col, int &row);
	int find_place_num(Place *pl, int lev, int col, int row);

	int inside(int z, int x, int y);
	int cell_inside(int z, int x, int y);
	int find_ground(int lev, int col, int row);

	//int savestate(char *txt);
	//void load(char *mapfname, TerraPCK *pck2);
	int eot_save(char *buf, int &buf_size);
	int saveitems(char *txt);

	MCD *mcd(int lev, int col, int row, int type)
	{
		assert((lev >= 0) && (lev < level) &&
		       (col >= 0) && (col < width * 10) &&
		       (row >= 0) && (row < height * 10) &&
		       (type >= 0) && (type < 4));
		MCD *m = &m_terrain->m_mcd[m_cell[lev][col][row]->type[type]];
		if (m->Tile_Type != type)
			return & m_terrain->empty;
		return m;
		//return &m_terrain->m_mcd[  m_cell[lev][col][row].type[type]	];
	}

	Cell *cell(int lev, int col, int row)
	{
		assert((lev >= 0) && (lev < level) &&
		       (col >= 0) && (col < width * 10) &&
		       (row >= 0) && (row < height * 10));

		return m_cell[lev][col][row];
	}

	Place *place(int lev, int col, int row)
	{
		assert((lev >= 0) && (lev < level) &&
		       (col >= 0) && (col < width * 10) &&
		       (row >= 0) && (row < height * 10));

		return m_cell[lev][col][row]->get_place();
	}

	Soldier *man(int lev, int col, int row)
	{
		assert((lev >= 0) && (lev < level) &&
		       (col >= 0) && (col < width * 10) &&
		       (row >= 0) && (row < height * 10));

		return m_cell[lev][col][row]->get_soldier();
	}

	Soldier *sel_man()
	{
		return m_cell[sel_lev][sel_col][sel_row]->get_soldier();
	}


	inline void set_man(int lev, int col, int row, Soldier *man)
	{
		assert((lev >= 0) && (lev < level) &&
		       (col >= 0) && (col < width * 10) &&
		       (row >= 0) && (row < height * 10));
		m_cell[lev][col][row]->set_soldier(man);
	}

	int visible(int lev, int col, int row)
	{
		return platoon_local->is_visible(lev, col, row);
	}
	void set_visible(int lev, int col, int row, int value)
	{
		platoon_local->set_visible(lev, col, row, value);
	}

	int seen(int lev, int col, int row)
	{
		return platoon_local->is_seen(lev, col, row);
	}
	void set_seen(int lev, int col, int row, int value)
	{
		platoon_local->set_seen(lev, col, row, value);
	}

 	int fire_state(int lev, int col, int row)
 	{
 		return m_cell[lev][col][row]->m_fire_state; 
 	}
 	void set_fire_state(int lev, int col, int row, int value)
 	{
 		m_cell[lev][col][row]->m_fire_state = value; 
 	}
 	void dec_fire_state(int lev, int col, int row)
 	{
 		m_cell[lev][col][row]->m_fire_state--;
 	}
 	void inc_fire_state(int lev, int col, int row) 
 	{
 		m_cell[lev][col][row]->m_fire_state++; 
 	}
 
 	int fire_time(int lev, int col, int row) 
 	{
 		return m_cell[lev][col][row]->m_fire_time; 
 	}
 	void set_fire_time(int lev, int col, int row, int value)
 	{
 		m_cell[lev][col][row]->m_fire_time = value; 
 	}
 	void dec_fire_time(int lev, int col, int row) 
 	{ 
 		m_cell[lev][col][row]->m_fire_time--;
 	}


	int smog_state(int lev, int col, int row)
	{
		return m_cell[lev][col][row]->m_smog_state;
	}
	void set_smog_state(int lev, int col, int row, int value)
	{
		m_cell[lev][col][row]->m_smog_state = value;
	}
	void dec_smog_state(int lev, int col, int row)
	{
		m_cell[lev][col][row]->m_smog_state--;
	}
	void inc_smog_state(int lev, int col, int row)
	{
		m_cell[lev][col][row]->m_smog_state++;
	}

	int smog_time(int lev, int col, int row)
	{
		return m_cell[lev][col][row]->m_smog_time;
	}
	void set_smog_time(int lev, int col, int row, int value)
	{
		m_cell[lev][col][row]->m_smog_time = value;
	}
	void dec_smog_time(int lev, int col, int row)
	{
		m_cell[lev][col][row]->m_smog_time--;
	}

	int pfval(int lev, int col, int row)
	{
		return m_cell[lev][col][row]->m_pfval;
	}
	void set_pfval(int lev, int col, int row, int value)
	{
		m_cell[lev][col][row]->m_pfval = value;
	}
	int isStairs(int lev, int col, int row)
	{
		return (mcd(lev, col, row, 3)->T_Level < -15);
	}

	void center(Soldier *s)
	{
		assert(s != NULL);
		sel_lev = s->z;
		center(s->z, s->x, s->y);
	}
	void unhide();
	Map(int l, int c, int r, int _ter, unsigned char *_map);
	void set_map_data(int c, int r, char ter);

	void save(char *fname);
	void load(char *fname);

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

//////////////////////////////////////////////////////////////////////////////
/// Every map is constructed from several blocks which are different for   ///
/// various terrain types. Each block has size divisible by 10 on X and Y  ///
/// axes. There may be different number of such preconstructed blocks for 
/// for different terrain types
//////////////////////////////////////////////////////////////////////////////

class Terrain
{
	struct block_info { int x_size; int y_size; int z_size; int rand_weight; };

	std::string                      m_name;
	std::vector<block_info> m_blocks;
	int                              m_rand_weight;
	int                              m_blocks_count;

	int get_random_block();

public:
	Terrain(const char *fileprefix, const char *name, int rand_weight = 100);
	virtual ~Terrain();

	int get_rand_weight() { return m_rand_weight; }

	bool create_geodata(GEODATA &gd);
	bool check_geodata(const GEODATA &gd);
};

//////////////////////////////////////////////////////////////////////////////
/// Information container and map generator for all the terrains used in   ///
/// the game                                                               ///
//////////////////////////////////////////////////////////////////////////////

#undef map

class TerrainSet
{
	std::map<int, Terrain *> terrain;
public:
	TerrainSet();
	virtual ~TerrainSet();

	int get_random_terrain_id();
	int get_random_terrain_size_x();
	int get_random_terrain_size_y();

	bool create_geodata(int terrain_index, int size_x, int size_y, GEODATA &gd);
	bool check_geodata(const GEODATA &gd);
};

#define map ufo2000_map

#endif