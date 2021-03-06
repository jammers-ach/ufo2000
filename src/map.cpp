/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2005  ufo2000 development team

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

#include "stdafx.h"

#include "global.h"

#include "video.h"
#include "colors.h"
#include "sound.h"
#include "pck.h"
#include "map.h"
#include "multiplay.h"
#include "platoon.h"
#include "crc32.h"
#include "scenario.h"
#include "text.h"
#include "random.h"
#include "script_api.h"

#define SCANGSIZE 4

SPK *Map::scanbord = NULL;
PCK *Map::smoke = NULL;
std::vector<ALPHA_SPRITE *> Map::fire_small, Map::fire_large;
std::vector<ALPHA_SPRITE *> Map::smoke_small, Map::smoke_medium, Map::smoke_large;
std::vector<ALPHA_SPRITE *> Map::selectbox, Map::aimbox, Map::throwbox;
int Map::m_animation_cycle = 0;

//            dirs      0  1  2  3  4  5  6  7
int Map::dir2ofs[8] = {1, 1, 0, -1, -1, -1, 0, 1};
//  y  x      -1  0  1  x
char Map::ofs2dir[3][3] = {{5, 6, 7},
                           {4, 8, 0},
                           {3, 2, 1}};

IMPLEMENT_PERSISTENCE(Map, "Map");

void load_terrain_pck(const std::string &tid, TerraPCK *&terrain_pck)
{
    terrain_pck = NULL;

    int stack_top = lua_gettop(L);
    // Enter 'TerrainTable' table
    lua_pushstring(L, "TerrainTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [tid] table
    lua_pushstring(L, tid.c_str());
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    // Enter 'Tiles' table
    lua_pushstring(L, "Tiles");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    int tiles_table = lua_gettop(L);
    // Enter 'Palettes' table
    lua_pushstring(L, "Palettes");
    lua_gettable(L, -3);
    ASSERT(lua_istable(L, -1));
    int palettes_table = lua_gettop(L);

    // Load all tiles from 'Tiles' table
    int index = 1;
    while (1) {
        lua_pushnumber(L, index);
        lua_gettable(L, tiles_table);
        if (!lua_isstring(L, -1)) break;
        const char *pckname = lua_tostring(L, -1);

        lua_pushnumber(L, index);
        lua_gettable(L, palettes_table);
        ASSERT(lua_isnumber(L, -1));
        int tftd_flag = (int)lua_tonumber(L, -1);

        if (terrain_pck == NULL)
            terrain_pck = new TerraPCK(pckname, tftd_flag);
        else
            terrain_pck->add(pckname, tftd_flag);

        lua_pop(L, 2);

        index++;
    }

    lua_settop(L, stack_top);
}

void Map::initpck()
{
    scanbord = new SPK("$(xcom)/ufograph/scanbord.pck");
    smoke    = new PCK("$(xcom)/ufograph/smoke.pck");
    fire_small = lua_table_image_vector("fire_small");
    fire_large = lua_table_image_vector("fire_large");
    smoke_small = lua_table_image_vector("smoke_small");
    smoke_medium = lua_table_image_vector("smoke_medium");
    smoke_large = lua_table_image_vector("smoke_large");
    selectbox = lua_table_image_vector("selectbox");
    aimbox = lua_table_image_vector("aimbox");
    throwbox = lua_table_image_vector("throwbox");
}

void Map::freepck()
{
    delete scanbord;
    delete smoke;
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
                m_cell[i][j][k] = new Cell(Position(i, j, k));
            }
        }
    }
    m_level_offset = height * 10 * width * 10;
    m_width_offset = height * 10;
    m_height_offset = 1;
    m_size = l * 10 * w * 10 * h;
}

Map::Map(GEODATA &mapdata)
{
    sel_row = 0; sel_col = 0; sel_lev = 0;
    m_changed_visicells = new std::vector<Position>;
    create(mapdata.z_size, mapdata.x_size, mapdata.y_size);

    std::string terrain_name = terrain_set->get_terrain_name(mapdata.terrain);
    ASSERT(terrain_name.size() < sizeof(m_terrain_name));
    strcpy(m_terrain_name, terrain_name.c_str());
    load_terrain_pck(m_terrain_name, m_terrain);
    loadmaps(mapdata.mapdata);
	
    build_visi();

	build_lights();
	
    m_minimap_area = new MinimapArea(this, SCREEN_W - SCREEN2W, SCREEN2H);
    
    explo_spr_list = new effect_vector;
}

Map::~Map()
{
    int i, j, k;
    for (i = 0; i < level; i++) {
        for (j = 0; j < 10 * width; j++) {
            for (k = 0; k < 10 * height; k++) {
                delete m_cell[i][j][k];
            }
            delete [] m_cell[i][j];
        }
        delete [] m_cell[i];
    }

    delete [] m_cell;
    delete m_terrain;

    delete m_minimap_area;
    
    delete explo_spr_list;
    delete m_changed_visicells;
}

void Map::loadmaps(unsigned char *_map)
{
	char map_name[512];
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "TerrainTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1));
    lua_pushstring(L, m_terrain_name);
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    lua_pushstring(L, "Maps");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1));
    int i = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            if (_map[i] != 0xFE) {
                lua_pushnumber(L, _map[i]);
                lua_gettable(L, -2);
                // Todo: fix ufo200 crashing
                // when loading invalid map-number
                // ?? maybe just replace map with "00" ??
				if(lua_isstring(L, -1)){
					loadmap(lua_tostring(L, -1), col * 10, row * 10);
					lua_pop(L, 1);
				}else if(lua_istable(L, -1)) {
					lua_pushstring(L, "Name");
					lua_gettable(L, -2);
					ASSERT(lua_isstring(L, -1)); 
					sprintf(map_name, "%s", lua_tostring(L, -1));
					lua_pop(L, 1);
					load_lua_map(map_name, col * 10, row * 10);
					lua_pop(L, 1);
				}else{
					//This should not happen - check Terrain::terrain(str)
					ASSERT(false);
				}
            }
            i++;
        }
    }

    lua_settop(L, stack_top);
}

/**
 * @brief : Loads a lua map, which is contained at the top of the lua stack. Called by Map::Loadmaps
 */
 int Map::load_lua_map(const char *mapname, int _x, int _y)
{
	//Debug purpose
	//char tmp[1024];
	
	int lua_stack_top = lua_gettop(L);
	int map_x;
	int map_y;
	int map_z;
	
	int mcol;
	int mrow;
	int mlev;
	int mindex;
	//Get dimensions
	lua_pushstring(L, "X");
	lua_gettable(L, -2);
	ASSERT(lua_isnumber(L, -1));
	map_x = (unsigned int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "Y");
	lua_gettable(L, -2);
	ASSERT(lua_isnumber(L, -1));
	map_y = (unsigned int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "Z");
	lua_gettable(L, -2);
	ASSERT(lua_isnumber(L, -1));
	map_z = (unsigned int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	//Get data
	lua_pushstring(L, "MapData");
	lua_gettable(L, -2);
	ASSERT(lua_istable(L, -1));
	//sprintf(tmp, "Load Lua Map size %d,%d,%d\n", map_x, map_y, map_z); 
	//lua_message(tmp);

	for (mlev = 0; mlev < map_z; mlev++) {
		lua_pushnumber(L, mlev + 1);
		lua_gettable(L, -2);
		ASSERT(lua_istable(L, -1));
		// exec
        for (mindex = map_x * map_y; mindex > 0; mindex--) {

				mcol = (int)((mindex - 1) % map_x) + _x;
				mrow = (int)((mindex - 1) / map_x) + _y;
				
				lua_pushnumber(L, mindex);
				lua_gettable(L, -2);
				ASSERT(lua_istable(L, -1));
				//Set tiles
				lua_pushnumber(L, 1);
				lua_gettable(L, -2);
				ASSERT(lua_isnumber(L, -1));
			    assign_type(mlev, mcol, mrow, 0, (unsigned int)lua_tonumber(L, -1));
				lua_pop(L, 1);

				lua_pushnumber(L, 2);
				lua_gettable(L, -2);
				ASSERT(lua_isnumber(L, -1));
                assign_type(mlev, mcol, mrow, 1, (unsigned int)lua_tonumber(L, -1));
				lua_pop(L, 1);

				lua_pushnumber(L, 3);
				lua_gettable(L, -2);
				ASSERT(lua_isnumber(L, -1));
                assign_type(mlev, mcol, mrow, 2, (unsigned int)lua_tonumber(L, -1));
				lua_pop(L, 1);

				lua_pushnumber(L, 4);
				lua_gettable(L, -2);
				ASSERT(lua_isnumber(L, -1));
                assign_type(mlev, mcol, mrow, 3, (unsigned int)lua_tonumber(L, -1));
				lua_pop(L, 1);
				
				m_cell[mlev][mcol][mrow]->set_soldier(NULL);
				//Go back to MapData
				lua_pop(L, 1);
		}
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	lua_settop(L, lua_stack_top);
	return 1;
}

int Map::loadmap(const char *fname, int _c, int _r)
{
    int fh = open(fname, O_RDONLY | O_BINARY);
    if (fh == -1) return 0;
    long size = filelength(fh);
    char *mbuf = new char[size];
    read(fh, mbuf, size);
    close(fh);

    int w = mbuf[0];
    int h = mbuf[1];
    int l = mbuf[2];

    _r = _r - 10 + h;
    _c = _c - 10 + w;

    for (int lev = 0; lev < l; lev++) {
        int i = 3 + (l - lev - 1) * 4 * w * h;
        if (i < 3 || i >= size) continue;
        if (lev >= level) continue;

        for (int col = _c + w - 1 - (w - 10); col >= _c - (w - 10); col--) {
            for (int row = _r - (h - 10); row < _r + h - (h - 10); row++, i += 4) {
                if (i < 3 || i + 4 > size) continue;
                if (col < 0 || col >= width * 10) continue;
                if (row < 0 || row >= height * 10) continue;
                assign_type(lev, col, row, 0, mbuf[i + 0]);
                assign_type(lev, col, row, 1, mbuf[i + 1]);
                assign_type(lev, col, row, 2, mbuf[i + 2]);
                assign_type(lev, col, row, 3, mbuf[i + 3]);
                m_cell[lev][col][row]->set_soldier(NULL);
            }
        }
    }
    delete [] mbuf;
    return 1;
}

void Map::assign_type(int lev, int col, int row, int part, int type)
{
    m_cell[lev][col][row]->type[part] = type;
}

void Map::drawitem(ALPHA_SPRITE *itype, int gx, int gy)
{
    PCK::showpck(itype, gx, gy);
}


void Map::draw_cell_pck(int _x, int _y, int _lev, int _col, int _row, int _type, int _seen, BITMAP *_dest)
{
    Cell &cell = *m_cell[_lev][_col][_row];
    int i = cell.type[_type];
    if (i == 0) return;
    
    ASSERT(i < (int)m_terrain->m_mcd.size());

    MCD &mcd = m_terrain->m_mcd[i];
    _y -= mcd.P_Level;

    ALPHA_SPRITE *frame;
    if (!mcd.UFO_Door)
        frame = mcd.FrameBitmap[m_animation_cycle];
    else
        frame = mcd.FrameBitmap[7];

    ASSERT(frame);

    /*int light_level = _seen ? cell.m_light * 16 : 0;*/
	int light_level = 0;
	if (_seen) {
		light_level = ((cell.m_light < cell.m_visi)) ? cell.m_visi * 16 : cell.m_light * 16;
	}else {
		light_level = 0;
	}	
    draw_alpha_sprite(_dest, frame, _x, _y - CELL_SCR_Y, light_level);
}

extern volatile unsigned int ANIMATION;

/**
 * Draw "3D" Battlescape-Map
 *
 * @todo optimize performance, some caching is needed in order to 
 *       avoid doing extra blit operations
 */
void Map::draw(int show_cursor, int battleview_width, int battleview_height)
{
    m_animation_cycle = (ANIMATION / 3) % 8;

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
        // Draw floor sprites first (otherwise they can overlap unit animation)
        for (int row = r1; row <= r2; row++) {
            for (int col = c2; col >= c1; col--) {
                sx = x + CELL_SCR_X * col + CELL_SCR_X * row;
                sy = y - (col) * CELL_SCR_Y + CELL_SCR_Y * row - CELL_SCR_Z - lev * CELL_SCR_Z - 1;
                if ((sx > -32) && (sx < battleview_width) && (sy >= -34) && (sy < battleview_height)) {
                    draw_cell_pck(sx, sy, lev, col, row, 0, platoon_local->is_seen(lev, col, row), screen2);
                }
            }
        }
        // Draw all the remaining sprites
        for (int row = r1; row <= r2; row++) {
            for (int col = c2; col >= c1; col--) {
                sx = x + CELL_SCR_X * col + CELL_SCR_X * row;
                sy = y - (col) * CELL_SCR_Y + CELL_SCR_Y * row - CELL_SCR_Z - lev * CELL_SCR_Z - 1;

                if ((sx > -32) && (sx < battleview_width) && (sy >= -34) && (sy < battleview_height)) {
                    if (m_cell[sel_lev][col][row]->MOUSE && show_cursor) {
                        if (lev == sel_lev) {
                            if ((!TARGET) || (target.action != THROW)) {
                                if ((m_cell[lev][col][row]->soldier_here()) && (platoon_local->is_visible(lev, col, row)))
                                    mtype = TARGET ? 1 + ((int)aimbox.size() / 2) + ((ANIMATION / 3) % (((int)aimbox.size() - 2) / 2)) : 4;
                                else
                                    mtype = TARGET ? (int)aimbox.size() / 2 : 3;
                            } else {
                                mtype = ((int)throwbox.size() / 2) + ((ANIMATION / 3) % ((int)throwbox.size() / 2));
                            }
                                
                            if (!TARGET)
                                draw_alpha_sprite(screen2, selectbox[mtype], sx, sy - CELL_SCR_Y);
                            else if (target.action != THROW)
                                draw_alpha_sprite(screen2, aimbox[mtype], sx, sy - CELL_SCR_Y);
                            else
                                draw_alpha_sprite(screen2, throwbox[mtype], sx, sy - CELL_SCR_Y);
                        } else if (lev < sel_lev) {
                            draw_alpha_sprite(screen2, selectbox[5], sx, sy - CELL_SCR_Y);
                        }
                    }

                    if (platoon_local->is_seen(lev, col, row)) {
                        draw_cell_pck(sx, sy, lev, col, row, 1, 1, screen2);
                        draw_cell_pck(sx, sy, lev, col, row, 2, 1, screen2);
                    } else if ((mcd(lev, col, row, 1)->Door || mcd(lev, col, row, 1)->UFO_Door) &&
                               (row > 0) && platoon_local->is_seen(lev, col, row - 1)) {
                        draw_cell_pck(sx, sy, lev, col, row, 1, 1, screen2);
                        draw_cell_pck(sx, sy, lev, col, row, 2, 0, screen2);
                    } else if ((mcd(lev, col, row, 2)->Door || mcd(lev, col, row, 2)->UFO_Door) &&
                               (col < width * 10 - 1) && platoon_local->is_seen(lev, col + 1, row)) {
                        draw_cell_pck(sx, sy, lev, col, row, 1, 0, screen2);
                        draw_cell_pck(sx, sy, lev, col, row, 2, 1, screen2);
                    } else {
                        draw_cell_pck(sx, sy, lev, col, row, 1, 0, screen2);
                        draw_cell_pck(sx, sy, lev, col, row, 2, 0, screen2);
                    }

                    draw_cell_pck(sx, sy, lev, col, row, 3, platoon_local->is_seen(lev, col, row), screen2);

                    // draw item
                    if (platoon_local->is_seen(lev, col, row)) {
                        int gy = sy;
                        if ((lev > 0) && mcd(lev, col, row, 0)->No_Floor && isStairs(lev - 1, col, row)) {
                            gy += CELL_SCR_Z + mcd(lev - 1, col, row, 3)->T_Level; 
                        } else {
                            gy += mcd(lev, col, row, 0)->T_Level;
                            gy += mcd(lev, col, row, 3)->T_Level;
                        }

                        int it = platoon_local->get_seen_item_index(lev, col, row);
                        if (it != -1)
                            drawitem(Item::obdata_get_bitmap(it, "pMap"), sx, gy);
                            
                        if (lev < level - 1) {
                            if (mcd(lev + 1, col, row, 0)->No_Floor && isStairs(lev, col, row)) {
                                gy = sy + mcd(lev, col, row, 3)->T_Level;
                                
                                it = platoon_local->get_seen_item_index(lev + 1, col, row);
                                if(it != -1)
                                    drawitem(Item::obdata_get_bitmap(it, "pMap"), sx, gy);
                            }
                        }
                    }

                    // draw soldier
                    if (platoon_local->is_visible(lev, col, row)) {
                        if (m_cell[lev][col][row]->soldier_here())
                            m_cell[lev][col][row]->get_soldier()->draw();

                        if (lev < level - 1) {
                            if (m_cell[lev + 1][col][row]->soldier_here()) {
                                if (isStairs(lev, col, row) && mcd(lev + 1, col, row, 0)->No_Floor) {
                                    m_cell[lev + 1][col][row]->get_soldier()->draw();
                                }
                            }
                        }
                    }

                    if (m_cell[sel_lev][col][row]->MOUSE && show_cursor) {
                        if (lev == sel_lev) {
                            if ((!TARGET) || (target.action != THROW)) {
                                if ((m_cell[lev][col][row]->soldier_here()) && (platoon_local->is_visible(lev, col, row)))
                                    mtype = TARGET ? 1 + ((ANIMATION / 3) % (((int)aimbox.size() - 2) / 2)) : 1;
                                else
                                    mtype = 0;
                            } else {
                                mtype = (ANIMATION / 3) % ((int)throwbox.size() / 2);
                            }
                                
                            if (!TARGET)
                                draw_alpha_sprite(screen2, selectbox[mtype], sx, sy - CELL_SCR_Y);
                            else if (target.action != THROW)
                                draw_alpha_sprite(screen2, aimbox[mtype], sx, sy - CELL_SCR_Y);
                            else
                                draw_alpha_sprite(screen2, throwbox[mtype], sx, sy - CELL_SCR_Y);
                        } else if (lev < sel_lev) {
                            draw_alpha_sprite(screen2, selectbox[2], sx, sy - CELL_SCR_Y);
                        }
                    }
                    // Draw smoke
                    if (platoon_local->is_seen(lev, col, row)) {
                        if (fire_time(lev, col, row) > 0) {
                            if (fire_time(lev, col, row) > 1) {
                                int frame = (ANIMATION / 3) % (int)fire_large.size();
                                draw_alpha_sprite(screen2, fire_large[frame], sx, sy - CELL_SCR_Y);
                            } else {
                                int frame = (ANIMATION / 3) % (int)fire_small.size();
                                draw_alpha_sprite(screen2, fire_small[frame], sx, sy - 6);
                            }
                        } else {
                            int st = smog_time(lev, col, row);
                            if (st > 0) {
                                switch (st) {
                                    case 1: {
                                        int frame = (ANIMATION / 3) % (int)smoke_small.size();
                                        draw_alpha_sprite(screen2, smoke_small[frame], sx, sy - CELL_SCR_Y);
                                        break;
                                    }
                                    case 2: {
                                        int frame = (ANIMATION / 3) % (int)smoke_medium.size();
                                        draw_alpha_sprite(screen2, smoke_medium[frame], sx, sy - CELL_SCR_Y);
                                        break;
                                    }
                                    default: {
                                        int frame = (ANIMATION / 3) % (int)smoke_large.size();
                                        draw_alpha_sprite(screen2, smoke_large[frame], sx, sy - CELL_SCR_Y);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
                                   
    //explosions have to be drawn over all other sprites
    //explosions have to be weapon specific
    //The center of an explosion sprite is defined as 1/2 of it's width and 1/3 of it's height starting from the bottom
    //This center is placed above the middle of the tile that explodes.
    std::vector<effect>::iterator exp;  
    for (exp = explo_spr_list->begin(); exp != explo_spr_list->end(); exp++) {
        int l = exp->lev, r = exp->row, c = exp->col;
        int it_type = exp->type;    
        if (!(platoon_local->is_seen(l, c, r)) ||
            !(l >= l1 && l <= l2) ||
            !(r >= r1 && r <= r2) ||
            !(c >= c1 && c <= c2)) continue;
                
        sx = x + CELL_SCR_X * c + CELL_SCR_X * r;
        sy = y - (c) * CELL_SCR_Y + CELL_SCR_Y * r - CELL_SCR_Z - l * CELL_SCR_Z - 1;
                
        int e = exp->state;
        if (e >= 0) {
            int stack_top = lua_gettop(L);
            lua_pushstring(L, "ExplosionAnimation");
            lua_gettable(L, LUA_GLOBALSINDEX);
            ASSERT(lua_istable(L, -1)); 
            // Enter [tid] table
            lua_pushnumber(L, e / 2 + 1);
            lua_gettable(L, -2);
            if (lua_isuserdata(L, -1)) {
                ASSERT(lpcd_isuserdatatype(L, -1, "ALPHA_SPRITE"));
                if (it_type > 0) {
                    ALPHA_SPRITE *spr = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
                    ALPHA_SPRITE *exp_frame = Item::obdata_get_bitmap(it_type, "hitAnim", e / 2 + 1);
                    if (exp_frame) {
                        draw_alpha_sprite(screen2, exp_frame, (sx + 16) - (exp_frame->w / 2), (sy + 12) - (exp_frame->h * 2 / 3));
                    }else if( !Item::obdata_get_bitmap(it_type, "hitAnim", 1) ) {
                        draw_alpha_sprite(screen2, spr, (sx + 16) - (spr->w / 2), (sy + 12) - (spr->h * 2 / 3));
                    }
                } else {
                    ALPHA_SPRITE *spr = (ALPHA_SPRITE *)lua_unboxpointer(L, -1);
                    draw_alpha_sprite(screen2, spr, (sx + 16) - (spr->w / 2), (sy + 12) - (spr->h * 2 / 3));
                } 
            }
            lua_settop(L, stack_top);
        }
    }

    m_cell[sel_lev][sel_col][sel_row]->MOUSE = 0;
}


void Map::step()
{
    int width_10 = 10 * width;
    int height_10 = 10 * height;
    for (int k = 0; k < level; k++)
        for(int i = 0; i < width_10;i++)
            for(int j = 0; j < height_10; j++) {
                if (fire_time(k, i, j) > 0) {
                    dec_fire_time(k,i,j);
                    for (int h = 0; h < 4; h++)
                        damage_cell_part(k, i, j, h, 25);
                    if (man(k, i, j) != NULL)
                        man(k, i, j)->hit(0, 10, DT_INC, 8); //DAMAGEDIR_UNDER
                } else if (smog_time(k, i, j) > 0) {
                    dec_smog_time(k, i, j);
                }
            }
}

void Map::smoker()
{
    std::vector<effect>::iterator exp = explo_spr_list->begin();
    while (exp != explo_spr_list->end()) {
        exp->state++;
        if (exp->state > 30)
            exp = explo_spr_list->erase(exp);
        else
            exp++;
    }
}


#define Cx 1
#define Cy -8

/**
 * Set position of selection-'cage' on map
 */
void Map::set_sel(int mx, int my)
{
    if (FLAGS & F_SCALE2X) {
        mx /= 2;
        my /= 2;
    }
    my += sel_lev * CELL_SCR_Z;      //!!
    sel_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx ;
    sel_row = (my - 3) - y + 8 - Cy + sel_col / 4;
    sel_row /= CELL_SCR_Y; sel_col /= CELL_SCR_X * 2; // 32;

    if (sel_col < 0) sel_col = 0;
    if (sel_row < 0) sel_row = 0;
    if (sel_col >= 10 * width) sel_col = 10 * width - 1;
    if (sel_row >= 10 * height) sel_row = 10 * height - 1;
//  text_mode(0); textprintf(screen, font, SCREEN2W + 8, 0, COLOR_WHITE, "x=%02d, y=%02d", sel_col, sel_row);
}

/**
 * Scroll the map by specified x and y offsets
 */
void Map::move(int ofs_x, int ofs_y)
{
    int sx = x, sy = y;
    x += ofs_x;
    y += ofs_y;
	//Consider the middle to be on current level
    int mx = SCREEN2W / 2;
    int my = SCREEN2H / 2 + sel_lev * CELL_SCR_Z;
    if (FLAGS & F_SCALE2X) {
        mx /= 2;
        my /= 2;
    }

    int old_center_col = mx - sx - 2 * (my - 3) + 2 * sy - 16 + 2 * Cy - Cx;
    int old_center_row = (my - 3) - sy + 8 - Cy + old_center_col / 4;
    old_center_row >>= 3; old_center_col >>= 5;

    int center_col = mx - x - 2 * (my - 3) + 2 * y - 16 + 2 * Cy - Cx;
    int center_row = (my - 3) - y + 8 - Cy + center_col / 4;
    center_row >>= 3; center_col >>= 5;

    // When the center of view is already outside of the map, it should be
    // possible to scroll in the direction of the map.
    if (((center_col < 0) && (center_col < old_center_col)) ||
        ((center_row < 0) && (center_row < old_center_row)) ||
        ((center_col >= 10 * width) && (center_col > old_center_col)) ||
        ((center_row >= 10 * height) && (center_row > old_center_row))) {
        x = sx; y = sy;
    }
}

#define S 2

/**
 * Redraw minimap
 */
void Map::svga2d()
{
    m_minimap_area->redraw(screen, SCREEN2W, 0);
}

/**
 * Minimap in "xcom-mapviewer"-look
 */
void Map::draw2d()
{
    int cx = SCREEN2W / 2;
    int cy = SCREEN2H / 2;

    set_sel(cx, cy);

    BITMAP *tmp = create_bitmap_of_map(sel_lev);

    int r1 = sel_row - 18; if (r1 < 0) r1 = 0;
    int r2 = sel_row + 18; if (r2 >= height * 10) r2 = height * 10 - 1;

    int c1 = sel_col - 27; if (c1 < 0) c1 = 0;
    int c2 = sel_col + 27; if (c2 >= width * 10) c2 = width * 10 - 1;
    
    rectfill(screen2, cx - 160 + 4 + 48, cy - 100 + 12 + 16, 
            cx - 160 + 4 + 268, cy - 100 + 12 + 163, COLOR_GRAY15);

    for (int lev = 0; lev <= sel_lev; lev++)
        for (int row = r1; row <= r2; row++)
            for (int col = c1; col <= c2; col++) {
                if (!platoon_local->is_seen(lev, col, row)) continue;

                blit(tmp, screen2, 
                    col * 4 + 3, 
                    row * 4,
                    cx + ( -sel_col + col) * 4 + 3,
                    cy + ( -sel_row + row) * 4,
                    4, 4);

                if (man(lev, col, row) == NULL) continue;
                
                if (platoon_local->belong(man(lev, col, row)))
                    rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
                             cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
                             COLOR_YELLOW);
                else if (platoon_local->is_visible(lev, col, row))
                    rectfill(screen2, cx + ( -sel_col + col) * 4 + 1, cy + ( -sel_row + row) * 4 + 1,
                             cx + ( -sel_col + col) * 4 + SCANGSIZE - 1, cy + ( -sel_row + row) * 4 + SCANGSIZE - 1,
                             COLOR_RED00);
            }

    destroy_bitmap(tmp);
    scanbord->show(screen2, cx - 160 + 4, cy - 100 + 12);
    text_mode(-1);
    textprintf(screen2, large, cx - 160 + 281 + 4, cy - 100 + 74 + 12, COLOR_LT_OLIVE, "%d", sel_lev);
}


BITMAP *Map::create_bitmap_of_map(int max_lev)
{
    BITMAP *bmp = create_bitmap(width * 10 * 4, height * 10 * 4);
    clear_to_color(bmp, COLOR_BLACK1);

    for (int lev = 0; lev <= max_lev; lev++) {
        for (int row = 0; row < height*10; row++) {
            for (int col = 0; col < width*10; col++) {
                for (int j = 0; j < 4; j++) {
                    int mcd_index = m_cell[lev][col][row]->type[j];
                    if (mcd_index == 0) continue;
                    draw_sprite(bmp, m_terrain->m_mcd[mcd_index].ScangBitmap, col * 4, row * 4);
                }
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

    //  return 0;
    //}
    int col, row;
    col = sel_col - (160 - xx - 4) / SCANGSIZE;
    row = sel_row - (100 - yy - 11) / SCANGSIZE;
    center(0, col, row);
    return 0;
}


void Map::clearseen()
{
    for (int k = 0; k < level; k++)
        for (int i = 0; i < 10*width; i++)
            for (int j = 0; j < 10*height; j++)
                platoon_local->set_seen(k, i, j, 0);
}

void Map::unhide()
{
    for (int k = 0; k < level; k++)
        for (int i = 0; i < 10*width; i++)
            for (int j = 0; j < 10*height; j++)
                platoon_local->set_seen(k, i, j, 1);
}

void Map::center(int lev, int col, int row)
{
    sel_lev = lev;
    if (FLAGS & F_SCALE2X) {
        x = SCREEN2W / 4 - CELL_SCR_X - CELL_SCR_X * col - CELL_SCR_X * row;
        y = SCREEN2H / 4 + CELL_SCR_Y * (col + 1) - CELL_SCR_Y * row + CELL_SCR_Z * lev;
    } else {
        x = SCREEN2W / 2 - CELL_SCR_X - CELL_SCR_X * col - CELL_SCR_X * row;
        y = SCREEN2H / 2 + CELL_SCR_Y * (col + 1) - CELL_SCR_Y * row + CELL_SCR_Z * lev;
    }
}

int Map::stopLOS_level(int dx, int dy, int lev, int col, int row)
{
    if ((col + dx < 0) || (col + dx >= width * 10) || (row + dy < 0) || (row + dy >= height * 10))
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

                m_cell[lev][col][row]->visi[k][i][j] = 0; //so below we check if the cell becomes visible, but what about if it becomes INvisible?
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

void Map::build_lights()
{
    for (int lev = 0; lev < level; lev++) {
        for (int col = 0; col < width*10; col++) {
            for (int row = 0; row < height*10; row++) {
				if (m_terrain->m_mcd[m_cell[lev][col][row]->type[0]].Light_Source)
					m_cell[lev][col][row]->islight = TILE_LIGHT;
				if (m_terrain->m_mcd[m_cell[lev][col][row]->type[1]].Light_Source)
					m_cell[lev][col][row]->islight = TILE_LIGHT;
				if (m_terrain->m_mcd[m_cell[lev][col][row]->type[2]].Light_Source)
					m_cell[lev][col][row]->islight = TILE_LIGHT;
				if (m_terrain->m_mcd[m_cell[lev][col][row]->type[3]].Light_Source)
					m_cell[lev][col][row]->islight = TILE_LIGHT;
            }
        }
    }
	update_lights();
}

void Map::update_lights()
{
	/* set all to default value from scenario rules */
    for (int i = 0; i < level; i++)
        for (int j = 0; j < width * 10; j++)
            for (int k = 0; k < height * 10; k++)
				m_cell[i][j][k]->m_light = scenario->rules[0];

	/* and then add all known light sources */
    for (int lev = 0; lev < level; lev++) {
        for (int col = 0; col < width*10; col++) {
            for (int row = 0; row < height*10; row++) {
				if (m_cell[lev][col][row]->islight > 0)
					show_light_source(lev, col, row);
            }
        }
    }
}

void Map::rebuild_visi(int z, int x, int y)
{
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
                cell_visibility_changed(lev, col, row);
            }
        }
    }
}

/**
 * Can a soldier walk through part "part" of cell (ox,oy,oz)?
 */
int Map::stopLOS(int oz, int ox, int oy, int part)
{
    if ((oz < 0) || (oz >= level) || (ox < 0) || (ox >= width * 10) || (oy < 0) || (oy >= height * 10))
        return 1;

    //A "big" diagonal ufo's wall acts like it consists of inner object (part 3)
    //and 2 "normal" walls (parts 1 and 2)
    int central_part_type = cell(oz, ox, oy)->type[3];
    if(part && m_terrain->m_mcd[central_part_type].Big_Wall)
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

    //A "big" diagonal ufo's wall acts like it consists of inner object (part 3)
    //and 2 "normal" walls (parts 1 and 2)
    int central_part_type = cell(oz, ox, oy)->type[3];
    if(part && m_terrain->m_mcd[central_part_type].Big_Wall)
        return 1;

    int ct = cell(oz, ox, oy)->type[part];

    if (m_terrain->m_mcd[ct].TU_Walk == 255) return 1;

    if (part == 0 || part == 3) {
        switch (m_pathfind_mode) {
            case PF_TRUE:
                if (man(oz, ox, oy)) return 1;
                break;
            case PF_DISPLAY:
                if (man(oz, ox, oy) && platoon_local->is_visible(oz, ox, oy)) return 1;
                break;
        }
        if (isStairs(oz, ox, oy) && ((oz + 1 >= level) || !passable(oz + 1, ox, oy))) return 1;

        // Pathfindig version 1, this condition is in the method Map::step_dest now
        /*for (int z = oz; z > 0 && mcd(z, ox, oy, 0)->No_Floor && !isStairs(z - 1, ox, oy); z--) {
            if (!passable(z - 1, ox, oy)) return 1;*
        }*/
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
//  Check if the destination point is too high
    int dx = ox + DIR_DELTA_X(dir);
    int dy = oy + DIR_DELTA_Y(dir);
    int dz = oz;

    if (dir == DIR_UP)
        dz++;

   if (dir == DIR_DOWN)
        dz--;

    if (!cell_inside(oz, ox, oy) || !cell_inside(dz, dx, dy)) return 0;

    int height_o = -mcd(oz, ox, oy, 3)->T_Level;
    int height_d = -mcd(oz, dx, dy, 3)->T_Level;

    if (height_d - height_o > 10) return 0;

//  Do the rest of checks
    if (!passable(dz, dx, dy)) return 0;

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

        case DIR_UP:
            if (!mcd(oz + 1, ox, oy, 0)->No_Floor && !mcd(oz + 1, ox, oy, 0) -> Gravlift)  {
                return 0;
            }
            break;

        case DIR_DOWN:
            if (!mcd(oz, ox, oy, 0)->No_Floor && !mcd(oz, ox, oy, 0) -> Gravlift)  {
                return 0;
            }
            break;
    }

    return 2;
}


extern int mapscroll;

/**
 * Scroll the map by putting mouse on the edge of its movement area
 */
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

        int ct = cell(doorz, doorx, doory)->type[door];
        int mcd = m_terrain->m_mcd[ct].Alt_tile;
        ASSERT(mcd < (int)m_terrain->m_mcd.size());

        m_cell[doorz][doorx][doory]->type[door] = 0;
        m_cell[doorz][doorx][doory]->type[3 - door] = mcd;
        rebuild_visi(doorz, doorx, doory);

        return 1;
    }
    return 0;
}

/**
 * Prepare bitmap with a cell shape image (used for precise aiming)
 */
BITMAP *Map::create_lof_bitmap(int lev, int col, int row)
{
    BITMAP *bmp = create_bitmap(20 * 4, 20 * 3);
    clear_to_color(bmp, COLOR_BLACK1);

    if (!platoon_local->is_seen(lev, col, row) && !platoon_local->is_visible(lev, col, row))
        return bmp;

    int dir = -1, s = 0;     //, tl = 0;
    if (platoon_local->is_visible(lev, col, row) && man(lev, col, row) != NULL) {
        dir = man(lev, col, row)->dir;
        if (man(lev, col, row)->m_state == SIT)
            s = 1;
    }

    for (int j = 0; j < 12; j++) {
        for (int i = 0; i < 16; i++) {
            for (int k = 0; k < 16; k++) {
                if (dir != -1) {
                    if (Skin::m_bof[s][dir][j][i][15 - k])
                        putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GREEN04);
                    else if (!pass_lof_cell(lev * 12 + j, col * 16 + (15 - i), row * 16 + k)) {
                        putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_WHITE);
                    } else {
                        putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GRAY10);
                    }
                } else {
                    if (!pass_lof_cell(lev * 12 + j, col * 16 + (15 - i), row * 16 + k)) {
                        putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_WHITE);
                    } else {
                        putpixel(bmp, 2 + k + (j / 3) * 20, 2 + i + (2 - j % 3) * 20, COLOR_GRAY10);
                    }
                }
            }
        }

    }
    return bmp;
}

/**
 * For debugging (enabled by setting F_SHOWLOFCELL flag in ufo2000.ini), 
 * shows cell shape in the top right corner of the battlescape view.
 */
void Map::show_lof_cell()
{
    BITMAP *bmp = create_lof_bitmap(sel_lev, sel_col, sel_row);
    blit(bmp, screen2, 0, 0, screen2->w - bmp->w, 0, bmp->w, bmp->h);
    destroy_bitmap(bmp);
}

/**
 * Tests if a voxel at (_z, _x, _y) is not solid and does not 
 * stop bullets
 */
int Map::pass_lof_cell(int _z, int _x, int _y)
{
    Cell *cell = m_cell[_z / 12][_x / 16][_y / 16];

    int z = _z % 12;
    int x = _x % 16;
    int y = _y % 16;

    for (int part = 0; part < 4; part++) {
        int ct = cell->type[part];
        if (ct != 0 && m_terrain->is_solid_voxel(ct, z, x, y))
            return 0;
    }

    return 1;
}

int Map::pass_lof_cell_part(int _z, int _x, int _y, int _part)
{
    Cell *cell = m_cell[_z / 12][_x / 16][_y / 16];

    int z = _z % 12;
    int x = _x % 16;
    int y = _y % 16;

    int ct = cell->type[_part];
    if (ct != 0 && m_terrain->is_solid_voxel(ct, z, x, y))
        return 0;

    return 1;
}

void Map::destroy_cell_part(int lev, int col, int row, int _part)
{
    int ct = m_cell[lev][col][row]->type[_part];
    if (ct > 0) {
        int mcd = m_terrain->m_mcd[ct].Die_tile;
        ASSERT(mcd < (int)m_terrain->m_mcd.size());

        if (m_terrain->m_mcd[ct].Light_Source)
            remove_light_source(lev, col, row, TILE_LIGHT);

        if ((_part == 0) && (lev > 0)) {
            m_cell[lev][col][row]->type[_part] = 0;
            place(lev, col, row)->dropall(lev, col, row);
            return ;
        }                                        
        
        if (_part == 0 && lev == 0 && mcd == 0)
            mcd = 1;                    //"scorched earth" instead of blank space on the ground level

        if (mcd == 0 || mcd == 1)
            m_cell[lev][col][row]->type[_part] = mcd;
        else {
            if (_part != 0)
                m_cell[lev][col][row]->type[_part] = 0;
            m_cell[lev][col][row]->type[m_terrain->m_mcd[mcd].Tile_Type] = mcd;
        }
                    
        if (m_terrain->m_mcd[ct].HE_Strength > 0)
            explode(lev * 12 - 6, col * 16 + 8, row * 16 + 8, m_terrain->m_mcd[ct].HE_Strength);

        rebuild_visi(lev, col, row);
    }
}

void Map::damage_cell_part(int lev, int col, int row, int _part, int dam, int dam_dev)
{
    // damage is randomized as +/- dam_dev %
    if (dam_dev > 0)
        dam = (int) cur_random->getUniform(dam * (1.0 - (dam_dev / 100.0)), dam * (1.0 + (dam_dev / 100.0)));
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
				if (!Item::obdata_isMed(_wtype)) {
					damage_cell_part(lev, col, row, i, Item::obdata_damage(_wtype), Item::obdata_dDeviation(_wtype));
				}
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

    line(screen2, xg, yg, xg2, yg2, COLOR_YELLOW);
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

int Map::find_place_coords(Place *pl, int &lev, int &col, int &row)
{
    for (lev = 0; lev < level; lev++)
        for (col = 0; col < width * 10; col++)
            for (row = 0; row < height * 10; row++)
                if (m_cell[lev][col][row]->get_place() == pl)
                    return 1;
    
    return 0;
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
    if (isStairs(lev, col, row)) lev++;
    return lev;
}

#define FI_STEP (PI / 64.0)
#define TE_STEP (PI / 64.0)

static char field[MAP_LEVEL_LIMIT * 2 * 6*10 * 6*10];

/**
 * Store position of cells whose visibility has changed in a queue. 
 * Queue is cleared when we call update_vision for the platoon. 
 * Smoke, lighting changes, terrain destruction, and doors all changed
 * cell visibility.
 */ 
void Map::cell_visibility_changed(int lev, int col, int row)
{
    Position pos(lev,col,row);
    std::vector<Position>::iterator iter = m_changed_visicells->begin();
    while(iter != m_changed_visicells->end()) {
        if (*iter == pos)
            return;
        iter++;
    }
    m_changed_visicells->push_back(pos);
}

/**
 * Clear a soldiers vision. Called when soldier dies, or vision is updated.
 */ 
void Map::clear_vision_matrix(Soldier *watcher)
{
    int32 vision_mask = watcher->get_vision_mask();
    int32* pMask = watcher->get_platoon()->get_vision_matrix();    
    int32* pEnd = pMask + size();
    while(pMask < pEnd)
        *(pMask++) &= (~vision_mask);
}

/**
 * Calculate viewable cells and write a soldiers vision mask to vision 
 * matrix.  A discrete pseudo-raytracing algorithm is used.
 */
void Map::update_vision_matrix(Soldier *watcher)
{
    clear_vision_matrix(watcher);
    Position pos(watcher->z, watcher->x, watcher->y);
    int dir = watcher->get_dir();
    int32 vision_mask = watcher->get_vision_mask();
    Platoon* pplatoon = watcher->get_platoon();
    Soldier* target;
    int32 visible_enemies = 0;
    int32 *vision_matrix = pplatoon->get_vision_matrix();
    int32 index = pos.index();
    
    vision_matrix[index] |= vision_mask;
    pplatoon->set_seen(pos.level(), pos.column(), pos.row(), 1);
        
    if (pos.level() > 0 && isStairs(pos.level(), pos.column(), pos.row()))  {
        vision_matrix[index - m_level_offset] |= vision_mask;
        pplatoon->set_seen(pos.level() - 1, pos.column(), pos.row(), 1);
    }
    
    memset(field, 0, size());
	/*255 directions possible : 1 to 8, times 32 = 255. 0 is 12 O'clock, 64 is 3 O'clock, 128 is 6 O'clock, 192 is 9 O'clock*/
    int ang = dir * 32;
/* Sweep vertical... should be : up above to 45 degrees underneath*/
    for (int fi = 8; fi <= 128 - 32; fi += 1) {
        fixed cos_fi = fixcos(itofix(fi));
        fixed sin_fi = fixsin(itofix(fi));
/* Sweep horizontal */
        for (int te = ang - 32; te <= ang + 32; te += 1) {
            fixed cos_te = fixcos(itofix(te));
            fixed sin_te = fixsin(itofix(te));

            int oz = pos.level(), ox = pos.column(), oy = pos.row();
            int l;
            int vz, vx, vy;
            int smokeway = 0;
            int lightway = 0;
            int lw_delta = 0;
/* Sweep distance */
            for (l = 1; l < 18 - smokeway * 3; l++) { /////////////from smoke

                vz = pos.level() + fixtoi(fixmul(itofix(l), cos_fi));                
                vx = pos.column() + fixtoi(fixmul(itofix(l), fixmul(cos_te, sin_fi)));
                vy = pos.row() + fixtoi(fixmul(itofix(l), fixmul(sin_te, sin_fi)));
                
                
                if (!cell_inside(vz, vx, vy))
                    break;
                if ((vz == oz) && (vx == ox) && (vy == oy))
                    continue;
                    
                lw_delta = 16 - m_cell[vz][vx][vy]->m_light;
                if (lw_delta < 0) lw_delta = 0;
                    
                if (m_cell[vz][vx][vy]->m_light < (lightway + lw_delta) / 3)
                    continue;

                if (!m_cell[oz][ox][oy]->visi[vz - oz + 1][vx - ox + 1][vy - oy + 1]) break;

                vision_matrix[vz * width * 10 * height * 10 + vx * height * 10 + vy] |= vision_mask;
                pplatoon->set_seen(vz, vx, vy, 1);
                
                Item* item = place(vz, vx, vy)->top_item();
                int itemtype = (item != NULL) ? item->itemtype() : -1;
                pplatoon->set_seen_item_index(vz, vx, vy,itemtype);
    
                if (field[vz * width * 10 * height * 10 + vx * height * 10 + vy] == 0) {
                    if (((target = man(vz, vx, vy)) != NULL) && (!pplatoon->belong(target))) {
                        visible_enemies |= target->get_vision_mask();
                    }
                }
                field[vz * width * 10 * height * 10 + vx * height * 10 + vy] = 1;

                oz = vz; ox = vx; oy = vy;

                if (!viewable_further(vz, vx, vy)) break;

                if (smog_time(vz, vx, vy) != 0) if (++smokeway > 2) break;
                lightway += lw_delta; 
            }
        }
    }
    watcher->set_visible_enemies(visible_enemies);
}

/**
 * Loop through platoon and update vision only for soldiers 
 * with changed vision.
 */
int32 Map::update_vision_matrix(Platoon* platoon)
{
    int32* vision_matrix = platoon->get_vision_matrix();
    int32 soldiers_affected = 0;
    std::vector<Position>::iterator iter = m_changed_visicells->begin();
    // Build a list of soldiers whose vision has changed
    while( iter != m_changed_visicells->end() ) {
        soldiers_affected |= vision_matrix[iter->index()];
        iter++;
    }
    if (soldiers_affected != 0) {
        Soldier *ss = platoon->findnum(0); 
        // Recalculate visibile cells for soldiers whose vision has changed.
        while (ss != NULL) {
            if ( (ss->is_active()) && (ss->get_vision_mask() & soldiers_affected) )
                update_vision_matrix(ss);
            ss = ss->next();
        }
    }
    return platoon->update_visible_enemies();
}

/**
 * When a new item is placed in a voxel, inform all soldiers who can see
 * that voxel.
 */
void Map::update_seen_item(Position p)
{
    Item* item = place(p.level(), p.column(), p.row())->top_item();
    int itemtype = (item != NULL) ? item->itemtype() : -1;
    
    if( platoon_local != NULL)
        if( platoon_local->get_vision_matrix()[p.index()] )
            platoon_local->set_seen_item_index(p.level(), p.column(), p.row(), itemtype);
    
    if( platoon_remote != NULL)
        if( platoon_remote->get_vision_matrix()[p.index()] )
            platoon_remote->set_seen_item_index(p.level(), p.column(), p.row(), itemtype);
}

double distance_3d(double v1, double v2, double v3)
{
    // this procedure probably should be moved somewhere else
    double r2=v1*v1 + v2*v2 + v3*v3;
    return fixtof(fixsqrt(ftofix(r2)));
}

/*
Is point (v1, v2, v3) inside the sphere with center in (0, 0, 0) and radius "range"?
*/
bool InsideSphere(double v1, double v2, double v3, double range)
{
/* Try to use only integer arithmetic. TODO: rewrite this code if it solves
problem with different results on the windows and unix clients (crc errors). */
    long long int x=v1*10000;
    long long int y=v2*10000;
    long long int z=v3*10000;
    long long int r=range*10000;
    return x*x + y*y + z*z <= r*r;
//    return v1*v1 + v2*v2 + v3*v3 <= range*range;
}

int calculate_hitdir(double dz, double dx, double dy)
{
    // Disabled since linux and windows clients get different results
    // Need more debug and testing
    return DAMAGEDIR_UNDER;
/*
    double ALLEGRO_PI=128; // PI=128 in the allegro trigonometry
    double UNDER_ARMOR_ANGLE = ALLEGRO_PI/3; //to rules.h ?
    
    int dir = DAMAGEDIR_UNDER;
    double hor_dist = distance_3d(dx, dy, 0);
    if (fixtof(fixatan((ftofix(dz / hor_dist)))) >= -UNDER_ARMOR_ANGLE) {
        dir = int(round( - 2 + (4.0/ALLEGRO_PI)*fixtof(fixatan2(ftofix(dx), ftofix(dy))) ));
        if (hor_dist==0) dir = 0; // this should not really happen
        if (dir < 0) dir += 8;
    }
    //printf("dz, dx, dy: %f, %f, %f, %f, %d \n", dz, dx, dy, atan(dz/hor_dist), dir );
    return dir;
*/
}

const double EXPL_BORDER_DAMAGE = 0.5; // how much damage does explosion on its border
const double HEIGHT_RATIO = 2; // how high is one level in squares 

/*
 *Map object explosion
 */
int Map::explode(int z, int x, int y, int max_damage)
{
    int damage_type = 2;    //HE damage type
    int damage, hit_dir = 0;
    
    int DEFAULT_SMOKE_TIME = 2; // how many half-turns will the smoke cloud exist
    
    effect eff;
    eff.lev = z / 12; eff.col = x / 16; eff.row = y / 16; eff.type = 0;
    eff.state = 0 - explo_spr_list->size();
    explo_spr_list->push_back(eff);
    
    soundSystem::getInstance()->play(SS_CV_GRENADE_BANG);
    
    // convert to coords relative to center of a cell
    double lev = double(z)/12 - 0.5;
    double col = double(x)/16 - 0.5;
    double row = double(y)/16 - 0.5;
    
    double range = (double(max_damage) / 15);
    for (int l = 0; l <= level; l++)
        for (int r = int(floor(row - range)); r <= int(ceil(row + range)); r++)
            for (int c = int(floor(col - range)); c <= int(ceil(col + range)); c++) {
                if (InsideSphere(row - double(r), col - double(c), (lev - double(l)) * HEIGHT_RATIO, range)) {
                    damage = int ( ( EXPL_BORDER_DAMAGE + (1 - EXPL_BORDER_DAMAGE)*(range - distance_3d(row - double(r), col - double(c), (lev - double(l)) * HEIGHT_RATIO))/range )*double(max_damage) );
                    if (damage > 0 && cell_inside(l, c, r)) {
                        if (man(l, c, r) != NULL) {
                            hit_dir = calculate_hitdir((lev - double(l)) * HEIGHT_RATIO, double(c) - col, double(r) - row);
                        }
                        explocell(-1, l, c, r, damage, damage_type, hit_dir);
                        smokecell(l, c, r, DEFAULT_SMOKE_TIME);
                    }
                }
            }
    return 1;
} 
  
/*
 *weapon or item explosion
 */
int Map::explode(int sniper, int z, int x, int y, int type)
{
    int damage, hit_dir = 0;
    int max_damage = Item::obdata_damage(type);
    int dam_dev = Item::obdata_dDeviation(type);
    int damage_type = Item::obdata_damageType(type);
    double explo_range = Item::obdata_exploRange(type);
    double smoke_range = Item::obdata_smokeRange(type);
    int smoke_time = Item::obdata_smokeTime(type);
    double range = explo_range < smoke_range ? smoke_range : explo_range;
    if (dam_dev > 0)
        max_damage = (int) cur_random->getUniform(max_damage * (1.0 - (dam_dev / 100.0)), max_damage * (1.0 + (dam_dev / 100.0)));
    
    effect eff;
    eff.lev = z / 12; eff.col = x / 16; eff.row = y / 16; eff.type = type;
    eff.state = 0 - explo_spr_list->size();
    explo_spr_list->push_back(eff);
    //Play object's sound or the default sound if the latter is not found
    if (Item::obdata_get_sound(type)) {
        soundSystem::getInstance()->play(Item::obdata_get_sound(type));
    }else {
        soundSystem::getInstance()->play(SS_CV_GRENADE_BANG);
    }
    
    // convert to coords relative to center of a cell
    double lev = double(z)/12 - 0.5;
    double col = double(x)/16 - 0.5;
    double row = double(y)/16 - 0.5;
    
    for (int l = 0; l <= level; l++)
        for (int r = int(floor(row - range)); r <= int(ceil(row + range)); r++)
            for (int c = int(floor(col - range)); c <= int(ceil(col + range)); c++) {
                double distance = distance_3d(row - double(r), col - double(c), (lev - double(l)));
                if (InsideSphere(row - double(r), col - double(c), (lev - double(l)) * HEIGHT_RATIO, range)) {
                    damage = int ( ( EXPL_BORDER_DAMAGE + (1 - EXPL_BORDER_DAMAGE)*(range - distance)/range )*double(max_damage) );
                    if (damage >= 0 && cell_inside(l, c, r)) {
                        if (man(l, c, r) != NULL) {
                            hit_dir = calculate_hitdir((lev - double(l)) * HEIGHT_RATIO, double(c) - col, double(r) - row);
                        }
                        
                        if (distance <= explo_range) {
                            explocell(sniper, l, c, r, damage, damage_type, hit_dir);
                            if (man(l, c, r))
                                man(l, c, r)->change_morale(((int)(distance - explo_range) - 1) * 2);
                        }
                        if (distance <= smoke_range)
                            smokecell(l, c, r, smoke_time);                    
                    }
                }
            }
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

void Map::smokecell(int lev, int col, int row, int time)
{ 
    set_smog_time(lev, col, row, time);              

    for (int i = 0; i < 4; i++)
        if (mcd(lev, col, row, i)->Fuel > smog_time(lev, col, row))
            set_smog_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
}

void Map::explocell(int sniper, int lev, int col, int row, int damage, int damage_type, int hitdir)
{
    for (int i = 3; i >= 0; i--) {  //reversed order is used to avoid a bug with tile destruction
        if (mcd(lev, col, row, i)->Fuel > smog_time(lev, col, row)) {
            if (mcd(lev, col, row, i)->Armour < damage && damage_type == DT_INC) {
                set_fire_time(lev, col, row, mcd(lev, col, row, i)->Fuel);
            }
        }
        
        if ((mcd(lev, col, row, i)->Armour < damage) && (damage_type != DT_STUN)) {
            destroy_cell_part(lev, col, row, i);
        }
    }

    if (damage_type != DT_STUN)
       place(lev, col, row)->damage_items(damage);

    if (man(lev, col, row) != NULL) {
        if (hitdir != DAMAGEDIR_UNDER)
            hitdir = (man(lev, col, row)->get_dir() + (hitdir + 4)) % 8;
        man(lev, col, row)->explo_hit(sniper, damage, damage_type, hitdir);
    }
}
                        
const double POWER_TO_RANGE = 2.5;
                     

void Map::Init_visi_platoon(Platoon* platoon)
{
	if (platoon == platoon_local) {
		reset_visi();
		Soldier *ss = platoon->findnum(0);
		while (ss != NULL) {
			add_visi(ss->z, ss->x, ss->y, 15);
			ss = ss->next();
        }
	}
}
					 
void Map::add_visi(int lev, int col, int row, int pow)
{   
	if (scenario->rules[0] != 16) {
		int power = (pow - scenario->rules[0]);
		double range = power / POWER_TO_RANGE;
	    for (int i = int(floor(lev - range)); i <= int(ceil(lev + range)); i++) {
	        for (int j = int(floor(col - range)); j <= int(ceil(col + range)); j++) {
	            for (int k = int(floor(row - range)); k <= int(ceil(row + range)); k++) {
	                double dst = distance_3d(row - k, col - j, (lev - i) * HEIGHT_RATIO); 
	                if (i < 0 || j < 0 || k < 0 ||
	                    i >= level || j >= width * 10 || k >= height * 10 ||
	                    dst > range)
	                        continue;
					m_cell[i][j][k]->m_visi += power - (int)((double)(power - 1) / range * dst);
	            }
	        }
	    }
	}
}

void Map::remove_visi(int lev, int col, int row, int pow)
{   
	if (scenario->rules[0] != 16) {
		int power = (pow - scenario->rules[0]); 
		double range = power / POWER_TO_RANGE;
		for (int i = int(floor(lev - range)); i <= int(ceil(lev + range)); i++) {
			for (int j = int(floor(col - range)); j <= int(ceil(col + range)); j++) {
				for (int k = int(floor(row - range)); k <= int(ceil(row + range)); k++) {
					double dst = distance_3d(row - k, col - j, (lev - i) * HEIGHT_RATIO); 
					if (i < 0 || j < 0 || k < 0 ||
						i >= level || j >= width * 10 || k >= height * 10 ||
						dst > range)
							continue;
					m_cell[i][j][k]->m_visi -= power - (int)((double)(power - 1) / range * dst);
					if (m_cell[i][j][k]->m_visi < scenario->rules[0])
						m_cell[i][j][k]->m_visi = scenario->rules[0];
				}
			}
		}
	}
}

//Resets visibility
void Map::reset_visi()
{
    for (int i = 0; i < level; i++)
        for (int j = 0; j < width * 10; j++)
            for (int k = 0; k < height * 10; k++)
				m_cell[i][j][k]->m_visi = scenario->rules[0];
}


void Map::add_light_source(int lev, int col, int row, int power)
{	
	m_cell[lev][col][row]->islight = power;
	show_light_source(lev, col, row);
}

void Map::show_light_source(int lev, int col, int row)
{
	int power = m_cell[lev][col][row]->islight;

	double range = power / POWER_TO_RANGE;
	//We know this cell is the LightSource and should be blocked by the same object.
    for (int i = int(floor(lev - range)); i <= int(ceil(lev + range)); i++) {
        for (int j = int(floor(col - range)); j <= int(ceil(col + range)); j++) {
            for (int k = int(floor(row - range)); k <= int(ceil(row + range)); k++) {
			    double dst = distance_3d(row - k, col - j, (lev - i) * HEIGHT_RATIO);
			    if (i < 0 || j < 0 || k < 0 || i >= level || j >= width * 10 || k >= height * 10 || dst > range)
                    continue;
				if (i == lev && j == col && k == row) {
					m_cell[i][j][k]->m_light += power;
					cell_visibility_changed(i, j, k);
					continue;
				}
				if (ray_visi(lev,col,row,i,j,k)) {
					m_cell[i][j][k]->m_light += power - (int)((double)(power - 1) / range * dst);
					cell_visibility_changed(i, j, k);
					continue;
				}
            }
		}
    }
}
/*
 * o stands for Origin, t stands for Target
 * Function should check for light blocking
*/
bool Map::ray_visi(int oz, int ox, int oy, int tz, int tx, int ty)
{
	return true;
}

void Map::remove_light_source(int lev, int col, int row, int power)
{
	m_cell[lev][col][row]->islight -= power;
	update_lights();
}

/**
 * End-of-turn - save game state into a buffer as a large block of text
 */                                        
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

int MAP_WIDTH, MAP_HEIGHT;

void Map::new_GEODATA(GEODATA *md)
{
    std::string terrain_name = terrain_set->get_random_terrain_name();

    if (net->is_network_game()) {
        ASSERT(g_net_allowed_terrains.size() > 0);
        while (g_net_allowed_terrains.find(terrain_name) == g_net_allowed_terrains.end()) {
            terrain_name = terrain_set->get_random_terrain_name();
        }
    }

    new_GEODATA(md, terrain_name);
}

void Map::new_GEODATA(GEODATA *md, const std::string &terrain_name)
{
    // Check map size settings
    if (MAP_WIDTH < 2) MAP_WIDTH = 2;
    if (MAP_HEIGHT < 2) MAP_HEIGHT = 2;

    if (MAP_WIDTH > 6) MAP_WIDTH = 6;
    if (MAP_HEIGHT > 6) MAP_HEIGHT = 6;

    // Try to use map generator function defined in terrain description
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "MapGenerator");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_isfunction(L, -1)); 
    // Enter [terrain_name] table
    lua_pushstring(L, terrain_name.c_str());
    lua_pushnumber(L, MAP_WIDTH);
    lua_pushnumber(L, MAP_HEIGHT);
    lua_safe_call(L, 3, 1);
    if (!lua_istable(L, -1)) {
        lua_settop(L, stack_top);
        // Fallback to default random shuffle map generation algorithm
        terrain_set->create_geodata(terrain_name, MAP_WIDTH, MAP_HEIGHT, *md);
        return;
    }
    // We should have map description returned at the top of stack here
    load_map_from_top_of_lua_stack(md);
    lua_settop(L, stack_top);
}

/**
 * Checks if GEODATA structure contains correct and usable map
 */
int Map::valid_GEODATA(GEODATA *md)
{
    std::string terrain_name = terrain_set->get_terrain_name(md->terrain);

    if ((md->x_size > 6) || (md->y_size > 6) ||
        (md->x_size < 2) || (md->y_size < 2) ||
        (md->z_size > MAP_LEVEL_LIMIT) || terrain_name == "") return 0;
        
    if (net->is_network_game() && 
        g_net_allowed_terrains.find(terrain_name) == g_net_allowed_terrains.end()) return 0;

    return 1;
}

/**
 * Supplementary function that interprets a value at the top of lua stack as
 * a map description and tries to load it
 */
bool Map::load_map_from_top_of_lua_stack(GEODATA *mapdata)
{
    memset(mapdata, 0, sizeof(GEODATA));
    mapdata->z_size = MAP_LEVEL_LIMIT; // !!! Hack
    
    int stack_top = lua_gettop(L);

    // we have a table with map data returned at the top of the stack
    if (!lua_istable(L, -1)) {
        lua_settop(L, stack_top);
        return false;
    }

    lua_pushstring(L, "Name");
    lua_gettable(L, -2);
    if (!lua_isstring(L, -1)) { lua_settop(L, stack_top); return false; }
    int tid = terrain_set->get_terrain_id(lua_tostring(L, -1));
    if (tid < 0) { lua_settop(L, stack_top); return false; }
    mapdata->terrain = (uint16)tid;
    lua_pop(L, 1);

    lua_pushstring(L, "SizeX");
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
    mapdata->x_size = (uint16)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "SizeY");
    lua_gettable(L, -2);
    if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
    mapdata->y_size = (uint16)lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_pushstring(L, "Scenario");
    lua_gettable(L, -2);
    int mission = lua_isstring(L, -1);
    lua_pop(L, 1);
    
    if (mission) {
        lua_pushstring(L, "Scenario");
        lua_gettable(L, -2);
        if (!lua_isstring(L, -1)) { lua_settop(L, stack_top); return false; }
        if (!scenario->new_scenario(lua_tostring(L, -1))) { lua_settop(L, stack_top); return false; }
        lua_pop(L, 1);
    
        lua_pushstring(L, "TargetX1");
        lua_gettable(L, -2);
        if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
        scenario->x1 = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
        
        lua_pushstring(L, "TargetY1");
        lua_gettable(L, -2);
        if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
        scenario->y1 = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    
        lua_pushstring(L, "TargetX2");
        lua_gettable(L, -2);
        if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
        scenario->x2 = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    
        lua_pushstring(L, "TargetY2");
        lua_gettable(L, -2);
        if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }
        scenario->y2 = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    lua_pushstring(L, "Mapdata");
    lua_gettable(L, -2);
    if (!lua_istable(L, -1)) { lua_settop(L, stack_top); return false; }

    int x = 0;

    for (int row = 1; row <= mapdata->y_size; row++) {
        lua_pushnumber(L, row);
        lua_gettable(L, -2);
        if (!lua_istable(L, -1)) { lua_settop(L, stack_top); return false; }

        for (int col = 1; col <= mapdata->x_size; col++) {
            lua_pushnumber(L, col);
            lua_gettable(L, -2);
            if (!lua_isnumber(L, -1)) { lua_settop(L, stack_top); return false; }

            mapdata->mapdata[x++] = lua_tonumber(L, -1) == -1 ? 0xFE : (uint8)lua_tonumber(L, -1);
            lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }

    lua_settop(L, stack_top);
    return true;
}

/**
 * Loads GEODATA structure from lua-file
 */
bool Map::load_GEODATA(const char *filename, GEODATA *mapdata)
{
    int stack_top = lua_gettop(L);
    lua_safe_dofile(L, F(filename), "restricted_sandbox");
    bool result = load_map_from_top_of_lua_stack(mapdata);
    lua_settop(L, stack_top);
    return result;
}

/**
 * Saves GEODATA structure to lua-file
 */
bool Map::save_GEODATA(const char *filename, GEODATA *mapdata)
{
    FILE *fh = fopen(F(filename), "wt");
    if (fh == NULL) return false;

    fprintf(fh, "return {\n");
    fprintf(fh, "\tName = \"%s\",\n", 
        lua_escape_string(terrain_set->get_terrain_name(mapdata->terrain)).c_str());
    fprintf(fh, "\tSizeX = %d, SizeY = %d,\n", mapdata->x_size, mapdata->y_size);
    
    fprintf(fh, "\tMapdata = {\n");

    int x = 0;
    for (int i = 0; i < mapdata->y_size; i++) {
        fprintf(fh, "\t\t{ ");
        for (int j = 0; j < mapdata->x_size; j++) {
            int ch = mapdata->mapdata[x++];
            if (ch == 0xFE) fprintf(fh, "-1"); else fprintf(fh, "%02d", ch);
            if (j + 1 < mapdata->x_size) fprintf(fh, ", ");
        }
        fprintf(fh, " },\n");
    }

    fprintf(fh, "\t}\n");
    fprintf(fh, "}\n");

    fclose(fh);
    return true;
}

int Map::walk_time(int _z, int _x, int _y)
{
    int time;
    time = (int)mcd(_z, _x, _y, 0)->TU_Walk;
    if (cell(_z, _x, _y)->type[3] != 0)
        time += (int)mcd(_z, _x, _y, 3)->TU_Walk;
    if (time < 4) time = 4;
    return time;
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

    load_terrain_pck(m_terrain_name, m_terrain);

    m_minimap_area = new MinimapArea(this, SCREEN_W - SCREEN2W, SCREEN2H);
    
    explo_spr_list = new effect_vector;
    m_changed_visicells = new std::vector<Position>;

    return true;
}

//////////////////////////////////////////////////////////////////////////////

#undef map

#define MAP_BLOCKS_LIMIT 100

Terrain::Terrain(const std::string &terrain_name)
{
	int current_max = 1;
    int stack_top = lua_gettop(L);
    // Enter 'TerrainTable' table
    lua_pushstring(L, "TerrainTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 
    // Enter [terrain_name] table
    lua_pushstring(L, terrain_name.c_str());
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1)); 
    // Extract terrain crc32
    lua_pushstring(L, "Crc32");
    lua_gettable(L, -2);
    ASSERT(lua_isnumber(L, -1)); 
    m_crc32 = (unsigned long)lua_tonumber(L, -1);
    lua_pop(L, 1);
    m_name = terrain_name;
    // Enter 'Maps' table
    lua_pushstring(L, "Maps");
    lua_gettable(L, -2);
    ASSERT(lua_istable(L, -1)); 
    m_rand_weight = 1;

    m_blocks.resize(MAP_BLOCKS_LIMIT);
    std::vector<block_info>::size_type index;
    for (index = 0; index < m_blocks.size(); index++) {
        lua_pushnumber(L, index);
        lua_gettable(L, -2);
		
		//Check if we have a lua map entry
		if (lua_istable(L, -1)){
			// Extract map definition
			lua_pushstring(L, "X");
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1)); 
			m_blocks[index].x_size = (unsigned int)lua_tonumber(L, -1) / 10;
			lua_pop(L, 1);
			
			lua_pushstring(L, "Y");
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1)); 
			m_blocks[index].y_size = (unsigned int)lua_tonumber(L, -1) / 10;
			lua_pop(L, 1);		
			
			lua_pushstring(L, "Z");
			lua_gettable(L, -2);
			ASSERT(lua_isnumber(L, -1)); 
			m_blocks[index].z_size = (unsigned int)lua_tonumber(L, -1);
			if (m_blocks[index].z_size + 1 > current_max) {
				if (m_blocks[index].z_size + 1 <= MAP_LEVEL_LIMIT) {
					current_max = m_blocks[index].z_size + 1;
				}else {
					current_max = MAP_LEVEL_LIMIT;
				}
			}
			lua_pop(L, 1);
			
			m_blocks[index].rand_weight = 1;
			lua_pop(L, 1);
			continue;
		}
		//Check if we have a map file entry
        if (!lua_isstring(L, -1)) {
            m_blocks[index].rand_weight = 0;
            lua_pop(L, 1);
            continue;
        }

        const char *fname = lua_tostring(L, -1);
		
		//Open the file
        int fh = open(fname, O_RDONLY | O_BINARY);
        if (fh == -1)
        {
            m_blocks[index].rand_weight = 0;
            lua_pop(L, 1);
            continue;
        }

        unsigned char buffer[3];
        read(fh, buffer, 3);
        close(fh);

        ASSERT(buffer[0] % 10 == 0);
        ASSERT(buffer[1] % 10 == 0);
        m_blocks[index].x_size      = buffer[0] / 10;
        m_blocks[index].y_size      = buffer[1] / 10;
        m_blocks[index].z_size      = buffer[2];
		if (m_blocks[index].z_size + 1 > current_max) {
			if (m_blocks[index].z_size + 1 <= MAP_LEVEL_LIMIT) {
				current_max = m_blocks[index].z_size + 1;
			}else {
				current_max = MAP_LEVEL_LIMIT;
			}
		}
        m_blocks[index].rand_weight = 1;
        lua_pop(L, 1);
    }
	if (current_max > 4) {
		m_max_levels = current_max;
	}else {
		m_max_levels = 4;
	}
    lua_settop(L, stack_top);
}

Terrain::~Terrain()
{
}

int Terrain::get_random_block()
{
    int randmax = 0;
    std::vector<block_info>::size_type i;

    for (i = 0; i < m_blocks.size(); i++)
        randmax += m_blocks[i].rand_weight;

    ASSERT(randmax > 0);

    int randval = rand() % randmax;

    for (i = 0; i < m_blocks.size(); i++) {
        randval -= m_blocks[i].rand_weight;
        if (randval < 0) return i;
    }
    ASSERT(false);
    return -1;
}

bool Terrain::create_geodata(GEODATA &gd)
{
    unsigned char map[6][6];
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
			gd.z_size = m_max_levels;
        }
    return true;
}

//////////////////////////////////////////////////////////////////////////////

TerrainSet::TerrainSet()
{
    int terrain_id = 0;
    int stack_top = lua_gettop(L);
    lua_pushstring(L, "TerrainTable");
    lua_gettable(L, LUA_GLOBALSINDEX);
    ASSERT(lua_istable(L, -1)); 

    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        ASSERT(lua_isstring(L, -2));
        std::string terrain_name = lua_tostring(L, -2);

        terrain[terrain_id++] = new Terrain(terrain_name);

        lua_pop(L, 1);
    }

    lua_settop(L, stack_top);

    // ?? gettext:
    if (terrain.empty()) {
        display_error_message(
            "Terrain data initialization failed." 
            "At least one valid terrain required." );
    }
}

TerrainSet::~TerrainSet()
{
    std::map<int, Terrain *>::iterator it;

    for (it = terrain.begin(); it != terrain.end(); ++it)
        delete it->second;
}

bool TerrainSet::create_geodata(const std::string &terrain_name, int x_size, int y_size, GEODATA &gd)
{
    int terrain_index = get_terrain_id(terrain_name);
    if (terrain_index < 0) return false;

    memset(&gd, 0, sizeof(gd));
    gd.terrain   = terrain_index;
    gd.x_size    = x_size;
    gd.y_size    = y_size;
    gd.z_size    = MAP_LEVEL_LIMIT;
    
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
    ASSERT(false);
    return -1;
}

/**
 * Displays dialog asking the user to select terrain type from the list of 
 * available terrains, additional requirement for network games is
 * that remote user should have these maps installed too.
 *
 * @param default_choice  terrain name that is active by default
 * @returns               terrain name, selected by user
 */
std::string TerrainSet::select_terrain_gui_dialog(
    const std::string &default_choice)
{
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    int default_index = 0;
    std::vector<std::string> gui_list;
    std::map<int, Terrain *>::iterator it;
    for (it = terrain.begin(); it != terrain.end(); ++it) {
        if (net->is_network_game()) {
            ASSERT(g_net_allowed_terrains.size() > 0);
            if (g_net_allowed_terrains.find(it->second->get_name()) == g_net_allowed_terrains.end())
                continue;
        }
        gui_list.push_back(it->second->get_name());
    }
    std::sort(gui_list.begin(), gui_list.end());
    for (int i = 0; i < (int)gui_list.size(); i++)
        if (gui_list[i] == default_choice) {
            default_index = i;
            break;
        }
    
    int result = gui_select_from_list(
        300, 200,
        _("Select terrain type"), 
        gui_list,
        default_index);
    
    return gui_list[result];
}
