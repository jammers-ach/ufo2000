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
#ifndef TERRAPCK_H
#define TERRAPCK_H

#include "pck.h"

struct MCD
{
	unsigned char Frame[8];      //Each frame is an index into the ____.TAB file; it rotates between the frames constantly.
	unsigned char LOFT[12];      //The 12 levels of references into GEODATA\LOFTEMPS.DAT
	short int ScanG;      //A reference into the GEODATA\SCANG.DAT
	unsigned char u23;
	unsigned char u24;
	unsigned char u25;
	unsigned char u26;
	unsigned char u27;
	unsigned char u28;
	unsigned char u29;
	unsigned char u30;
	unsigned char UFO_Door;      //If it's a UFO door it uses only Frame[0] until it is walked through, then it animates once and becomes Alt_MCD.  It changes back at the end of the turn
	unsigned char Stop_LOS;      //You cannot see through this tile.
	unsigned char No_Floor;      //If 1, then a non-flying unit can't stand here
	unsigned char Big_Wall;      //It's an object (tile type 3), but it acts like a wall
	unsigned char Gravlift;
	unsigned char Door;      //It's a human style door--you walk through it and it changes to Alt_MCD
	unsigned char Block_Fire;       //If 1, fire won't go through the tile
	unsigned char Block_Smoke;      //If 1, smoke won't go through the tile
	unsigned char u39;
	unsigned char TU_Walk;       //The number of TUs require to pass the tile while walking.  An 0xFF (255) means it's unpassable.
	unsigned char TU_Fly;        // remember, 0xFF means it's impassable!
	unsigned char TU_Slide;      // sliding things include snakemen and silacoids
	unsigned char Armour;        //The higher this is the less likely it is that a weapon will destroy this tile when it's hit.
	unsigned char HE_Block;      //How much of an explosion this tile will block
	unsigned char Die_MCD;       //If the terrain is destroyed, it is set to 0 and a tile of type Die_MCD is added
	unsigned char Flammable;     //How flammable it is (the higher the harder it is to set aflame)
	unsigned char Alt_MCD;       //If "Door" or "UFO_Door" is on, then when a unit walks through it the door is set to 0 and a tile type Alt_MCD is added.
	unsigned char u48;
	signed char T_Level;      //When a unit or object is standing on the tile, the unit is shifted by this amount
	unsigned char P_Level;      //When the tile is drawn, this amount is subtracted from its y (so y position-P_Level is where it's drawn)
	unsigned char u51;
	unsigned char Light_Block;     //The amount of light it blocks, from 0 to 10
	unsigned char Footstep;     	//The Sound Effect set to choose from when footsteps are on the tile
	unsigned char Tile_Type;       //This is the type of tile it is meant to be -- 0=floor, 1=west wall, 2=north wall, 3=object .  When this type of tile is in the Die_As or Open_As flags, this value is added to the tile coordinate to determine the byte in which the tile type should be written.
	unsigned char HE_Type;         //0=HE  1=Smoke
	unsigned char HE_Strength;     //The strength of the explosion caused when it's destroyed.  0 means no explosion.
	unsigned char Smoke_Blockage;      //? Not sure about this ...
	unsigned char Fuel;      //The number of turns the tile will burn when set aflame
	unsigned char Light_Source;      //The amount of light this tile produces
	unsigned char Target_Type;       //The special properties of the tile
	unsigned char u61;
	unsigned char u62;

};

class TerraPCK : public PCK
{
private:
	std::vector<BITMAP *> m_blackbmp;

	static unsigned char m_tbb[0xFFFF];
	static unsigned short m_tbs[0xFFF];

	void loadmcd(int start, int size);
	void create_blackbmp(int start, int size);

	int m_mcdstart[10];
	int m_mcdnum;
public:
	std::vector<MCD> m_mcd;
	MCD empty;

	TerraPCK(char *pckfname);
	~TerraPCK();

	void add(char *pckfname);
	void showblackpck(int num, int xx, int yy);

	int mcdstart(int mcd_num);
};

#endif