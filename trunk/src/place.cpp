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
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "video.h"
#include "place.h"
#include "map.h"
#include "pfxopen.h"

IMPLEMENT_PERSISTENCE(Place, "Place");

/*
Place::Place()
{
	viscol = 0;
	m_item = NULL;
	set(0, 0, 16, 16);
}
*/

Place::Place(int x, int y, int w, int h)
{
	viscol = 0;
	m_item = NULL;
	set(x, y, w, h);
}

Place::~Place()
{
	Item *t1 = m_item, *t2 = NULL;
	while (t1 != NULL) {
		t2 = t1->next;
		delete t1;
		t1 = t2;
	}
}

void Place::set(int x, int y, int w, int h)
{
	gx = x; gy = y;
	width = w; height = h;
}

void Place::scroll_left()
{
	if (viscol > 0) viscol--;
}

void Place::scroll_right()
{
	if (viscol < width - 1) viscol++;
}


Item *Place::item_under_mouse()
{
	int xx = (mouse_x - gx) / 16;
	int yy = (mouse_y - gy) / 15;

	if ((xx >= 0) && (xx < width) && (yy >= 0) && (yy < height)) {
		Item *t = m_item;
		while (t != NULL) {
			if (t->inside(xx, yy))
				return t;
			t = t->next;
		}
	}
	return NULL;
}

Item *Place::item(int ix, int iy)
{
	Item * t = m_item;
	while (t != NULL) {
		if ((ix == t->x) && (iy == t->y)) {
			return t;
		}
		t = t->next;
	}
	return NULL;
}

int Place::isfree(int xx, int yy)
{
	if (!outside_belt(xx, yy))
		if ((xx >= 0) && (xx < width) && (yy >= 0) && (yy < height)) {
			Item * t = m_item;
			while (t != NULL) {
				if (t->inside(xx, yy))
					return 0;
				t = t->next;
				//if (t==m_item) return 0;
			}
			return 1;
		}
	return 0;
}

int Place::ishand()
{
	if ((width == 2) && (height == 3))
		return 1;
	return 0;
}

int Place::isfit(Item * it, int xx, int yy)
{
	if (ishand() && (m_item != NULL))
		return 0;

	for (int i = xx; i < xx + it->data()->width; i++)
		for (int j = yy; j < yy + it->data()->height; j++)
			if (!isfree(i, j)) return 0;
	return 1;
}

int Place::put(Item * it, int xx, int yy)
{
	if (m_item == NULL) {
		m_item = it;
		it->setpos(xx, yy);
	} else if (isfree(xx, yy) && isfit(it, xx, yy)) {
		m_item->prev = it;
		it->next = m_item; it->prev = NULL;
		it->setpos(xx, yy);
		m_item = it;
	} else {
		return 0;
	}

	return 1;
}

int Place::put(Item * it)
{
	it->unlink();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (put(it, x, y))
				return 1;
		}
	}
	return 0;
}

Item * Place::get(int xx, int yy)
{
	Item * t;
	t = m_item;
	while (t != NULL) {
		if (ishand() || t->inside(xx, yy)) {
			if (t == m_item)
				m_item = t->next;
			t->unlink();
			return t;
		}
		t = t->next;
	}
	return NULL;
}

int Place::destroy(Item * it)
{
	Item * t;
	t = m_item;
	while (t != NULL) {
		if (t == it) {
			if (t == m_item)
				m_item = t->next;
			t->unlink();
			delete it;
			return 1;
		}
		t = t->next;
	}
	return 0;
}


Item *Place::mselect()
{
	if ((mouse_x > gx) && (mouse_x < gx + width * 16))
		if ((mouse_y > gy) && (mouse_y < gy + height * 15))
			return get((mouse_x -gx) / 16 + viscol, (mouse_y - gy) / 15);
	return NULL;
}

int Place::mdeselect(Item *it)
{
	if ((mouse_x > gx) && (mouse_x < gx + width * 16))
		if ((mouse_y > gy) && (mouse_y < gy + height * 15)) {
			int x2 = (mouse_x - gx - (it->data()->width - 1) * 8) / 16 + viscol;
			int y2 = (mouse_y - gy - (it->data()->height - 1) * 8) / 15;
			//text_mode(0);
			//textprintf(screen, font, 1, 150, 1, "x=%d y=%d", x2, y2);

			if (isfree(x2, y2) && isfit(it, x2, y2)) {
				put(it, x2, y2);
				return 1;
			}
		}
	return 0;
}

int Place::outside_belt(int x, int y)
{
	if ((width == 4) && (height == 2))
		if ((y == 1) && ((x == 1) || (x == 2)))
			return 1;
	return 0;
}

void Place::draw(int gx, int gy)
{
	if (m_item == NULL)
		return ;

	Item *t = m_item, *gt = m_item;

	while (t != NULL) {
		if (t->data()->importance > gt->data()->importance) {
			gt = t;
		}
		t = t->next;
	}

	map->drawitem(gt->data()->pMap, gx, gy);
}
/*
#define P_SHL_RIGHT 0
#define P_SHL_LEFT  1
#define P_ARM_RIGHT 2
#define P_ARM_LEFT  3
#define P_LEG_RIGHT 4
#define P_LEG_LEFT  5
#define P_BACK		6
#define P_BELT		7
#define P_MAP		 8
*/
static char *place_name[10] = {
	"RIGHT SHOULDER",
	"LEFT SHOULDER",
	"RIGHT HAND",
	"LEFT HAND",
	"RIGHT LEG",
	"LEFT LEG",
	"BACK PACK",
	"BELT",
	"GROUND",
	"ARMOURY"
};

void Place::drawgrid(int PLACE_NUM)
{
	assert((PLACE_NUM >= 0) && (PLACE_NUM < 10));

	textout(screen2, small, place_name[PLACE_NUM], gx, gy - 8, 66);

	if (!ishand()) {
		int dx = 0, dy = 0;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (!outside_belt(j, i))
					rect(screen2, gx + dx, gy + dy, gx + dx + 16, gy + dy + 15, 8);      //square
				dx += 16;
				if (j == 19) break;      // for map cell!!!!!!!!!!!!!!!!!!!
			}
			dx = 0;
			dy += 15;
		}
	} else {
		rect(screen2, gx, gy, gx + width * 16, gy + height * 15, 8);
	}

	Item * t = m_item;
	while (t != NULL) {
		if ((t->x >= viscol - 1) && (t->x < viscol + 20)) {
			int x = gx + (t->x - viscol) * 16;
			int y = gy + t->y * 15 + 5;

			if (ishand()) {
				int it_width = t->data()->width;
				int it_height = t->data()->height;
				x = gx + (width - it_width) * 16 / 2;
				y = gy + (height - it_height) * 15 / 2 + 5;
			}

			bigobs->showpck(t->data()->pInv, x, y);
			if (key[KEY_LCONTROL]) {
				t->draw_health(1, x + 1, y - 4);
			}
			if (key[KEY_ALT]) {
				t->draw_health(0, x + 1, y - 4);
			}
		}
		t = t->next;
	}
}

void Place::dropall(int lev, int col, int row)
{
	Item * t = m_item;
	while (t != NULL) {
		//text_mode(0);
		//textprintf(screen, font, 1, 150, 1, "x=%d y=%d t=%2x p=%4s n=%1s   ", t->x, t->y, t->type, t->prev, t->next);
		Item * tn = t->next;
		map->place(lev, col, row)->put(t);
		//textprintf(screen, font, 1, 160, 1, "put t=%2x", t->type);
		//textprintf(screen, font, 1, 170, 1, "x=%d y=%d t=%2x p=%4s n=%1s   ", t->x, t->y, t->type, t->prev, t->next);
		t = tn;
	}
	m_item = NULL;
}

int Place::isthere(Item * it)
{
	Item * t;
	t = m_item;
	while (t != NULL) {
		if (t == it)
			return 1;
		t = t->next;
	}
	return 0;
}

void Place::save_bin(char *fn)
{
	char buf[3 * 1000];
	int buf_size = 0;

	Item * it = m_item;
	while (it != NULL) {
		buf[buf_size++] = it->type;
		buf[buf_size++] = it->x;
		buf[buf_size++] = it->y;
		it = it->next;
	}

	int fh = OPEN_OWN(fn, O_CREAT | O_TRUNC | O_RDWR | O_BINARY);
	assert(fh != -1);
	write(fh, buf, buf_size);
	close(fh);
}

void Place::load_bin(char *fn)
{
	char buf[3 * 1000];
	int buf_size;

	int fh = OPEN_OWN(fn, O_RDONLY | O_BINARY);
	assert(fh != -1);
	buf_size = read(fh, buf, sizeof(buf));
	close(fh);

	for (int i = 0; i < buf_size; ) {
		int type = buf[i++];
		int x = buf[i++];
		int y = buf[i++];
		put(new Item(type), x, y);
	}
}

void Place::build_ITEMDATA(int ip, ITEMDATA * id) //don't save clip rounds
{
	Item * it = m_item;
	while (it != NULL)
	{
		assert(id->num < 100);
		id->place[id->num] = ip;
		id->type[id->num] = it->type;
		id->x[id->num] = it->x;
		id->y[id->num] = it->y;
		id->num++;
		if (it->haveclip()) {
			id->place[id->num] = 0xFF;
			id->type[id->num] = it->cliptype();
			id->num++;
		}
		it = it->next;
	}
}

void Place::build_items_stats(char *buf, int &len)
{
	Item * it = m_item;
	while (it != NULL) {
		buf[len++] = it->type;
		if (it->haveclip()) {
			buf[len++] = it->cliptype();
		}
		it = it->next;
	}
}


int Place::save_items(char *fs, int _z, int _x, int _y, char *txt)
{
	int len = 0;
	Item *t = m_item;

	char format[1000];
	sprintf(format, "%s type=%%d x=%%d y=%%d rounds=%%d\r\n", fs);

	while (t != NULL) {
		len += sprintf(txt + len, format, _z, _x, _y,
		               t->type, t->x, t->y, t->rounds);

		if (t->haveclip()) {
			len += sprintf(txt + len, format, _z, _x, _y,
			               t->cliptype(), -1, -1, t->roundsremain());
		}
		t = t->next;
	}
	return len;
}

int Place::eot_save(int ip, char *txt)
{
	int len = 0;
	Item *t = m_item;

	while (t != NULL) {
		len += sprintf(txt + len, "ip=%d type=%d x=%d y=%d rounds=%d\r\n",
		               ip, t->type, t->x, t->y, t->rounds);
		if (t->haveclip()) //////////////////////
		{
			len += sprintf(txt + len, "ip=%d type=%d x=%d y=%d rounds=%d\r\n",
			               -1, t->cliptype(), -1, -1, t->roundsremain());
		}
		t = t->next;
	}
	return len;
}

void Place::destroy_all_items()
{
	Item *t, *t2;
	t = m_item;
	while (t != NULL) {
		t2 = t->next;
		delete t;
		t = t2;
	}
	m_item = NULL;
}

#include "explo.h"

void Place::damage_items(int dam)
{
	/*Item *it;
	it = m_item;
	while(it != NULL) {
		if (it->damage(dam)) { //destroyed
			Item *t2;
			t2 = it->next;
			if (m_item == it)
				m_item = t2;

			it->unlink();
			elist.remove(it);
			delete it; // explodable

			it = t2;
			continue;			
		}
		it = it->next;
	}*/
}

void Place::check_mine() {
	Item *it;
	it = m_item;
	if (it!=NULL)
		if ((it->is_grenade())&&(it->type == PROXIMITY_GRENADE)&&(it->delay_time()<=0))
			elist->detonate(it);
}


void Place::draw_deselect_time(int PLACE_NUM, int time)
{
	if (time) {
		//place_name[PLACE_NUM]
		//textout(screen2, small, time, gx, gy-8, 66);
		printsmall(gx + 1 + text_length(small, place_name[PLACE_NUM]), gy - 6, 1, time);
	}
}

bool Place::Write(persist::Engine &archive) const
{
	PersistWriteBinary(archive, *this);

	PersistWriteObject(archive, m_item);

	return true;
}

bool Place::Read(persist::Engine &archive)
{
	PersistReadBinary(archive, *this);

	PersistReadObject(archive, m_item);

	return true;
}