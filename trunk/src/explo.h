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
#ifndef EXPLO_H
#define EXPLO_H

#include "global.h"
#include "item.h"

#define EXPLOITEMS 100
class Explosive: public persist::BaseObject
{
	DECLARE_PERSISTENCE(Explosive);
private:
	Item *item[EXPLOITEMS];
	int delaytime[EXPLOITEMS];

public:
	Explosive();
	void reset();

	void add(Item *it, int delay_time);
	void remove(Item *it);

	void step(int crc);      // -1 if local
	int detonate(Item *it);
	int detonate(int lev, int col, int row, int iplace, int ix, int iy);
	void check_for_detonation(Item *it);
	int on_hand(Item *it);

	virtual bool Write(persist::Engine &archive) const;
	virtual bool Read(persist::Engine &archive);
};

#endif