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
#ifndef UNITS_H
#define UNITS_H

enum PanPos {POS_LEFT, POS_RIGHT};

class Units
{
	friend class Connect;
private:
	int size;
	char name[20][26];
	int cost[20];
	int x[20], y[20];
	int lev[20], col[20], row[20];
	int selected;
	int mx, my, mx1, my1, mx2, my2;
	int gx, gy, gmx, gmw, gmy, gmh;
	PanPos pos;
public:
	int SEND;
	int START;

	Units();
	void reset();
	void set_pos(PanPos pos, int gx, int gy, int gmx, int gmw, int gmy, int gmh);
	void reset_selections();

	int add(int num, char *nm, int ct);
	void set_mouse_range(int mx, int my, int mx1, int my1, int mx2, int my2);
	void limit_mouse_range();
	void restore_mouse_range();

	void print(int gcol);
	void print_simple(int gcol);

	void build_items_stats(ITEMDATA *id, char *buf, int &len);
	int draw_items_stats(int gx, int gy, char *buf, int len);

	void execute(Map *map, int map_change_allowed = 1);
	void execute_right();
	void deselect();
	void draw_lines(int gcol);
	void draw_text();
	int select_unit(int num, int mx, int my);
	int deselect_unit(int num);

	friend class Platoon;
};

#endif