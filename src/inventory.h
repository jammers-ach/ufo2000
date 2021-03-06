/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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
#ifndef INVENTORY_H
#define INVENTORY_H
#include "spk.h"
#include "item.h"
#include "mouse.h"

/**
 * Inventory management in the battlecape. 
 
 * @ingroup battlescape
 * @ingroup gui
 * @relates Editor
 * @todo Try to simplify Inventory and Editor classes and make them 
 *       share common code -serge-
 */
class Inventory
{
private:
	int x, y;	

	Item   *sel_item;
	int    sel_item_place;

	BITMAP *b123, *b4, *b5;
	
	MouseRange *m_mouse_range;

public:
	Inventory();
	~Inventory();

	void draw(int _x, int _y);
	void execute();
	void close();
	void backput();
    Item* get_sel_item() {return sel_item;}
};

#endif
