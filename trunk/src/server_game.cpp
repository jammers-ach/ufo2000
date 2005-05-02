/*
This file is part of "UFO 2000" aka "X-COM: Gladiators"
                    http://ufo2000.sourceforge.net/
Copyright (C) 2000-2001  Alexander Ivanov aka Sanami
Copyright (C) 2002-2003  ufo2000 development team

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

#include <time.h>
#include <math.h>
#include "server_game.h"
#include "server_config.h"

std::map<long int, Server_Game_UFO *> Server_Game_UFO::active_games;

long int Server_Game_UFO::CreateGame(std::string playername1,std::string playername2)
{
//    sqlite3::connection db_conn(DB_FILENAME);
    try {
        db_conn.executenonquery("update ufo2000_sequences set seq_val=seq_val+1 where name='ufo2000_games';");
        long int game_id = db_conn.executeint32("select seq_val from ufo2000_sequences where name='ufo2000_games';");
        db_conn.executenonquery("\
            insert into ufo2000_games\
            (id, last_received_packed, is_finished, errors, result) \
            values \
            (%d, 0, 'N', NULL, 0);", game_id);
        db_conn.executenonquery("\
            insert into ufo2000_game_players\
            (game, player, last_sended_packet, position)\
            values\
            (%d, '%s', 0, 1);", game_id, playername1.c_str());
        db_conn.executenonquery("\
            insert into ufo2000_game_players\
            (game, player, last_sended_packet, position)\
            values\
            (%d, '%s', 0, 2);", game_id, playername2.c_str());
        return game_id;
    }
    catch(std::exception &ex) {
        server_log("Exception Occured: %s",ex.what());
        return 0;
    }
}


void Server_Game_UFO::ActivatePlayer(int game_id,ServerClientUfo* player)
{
    if(player->game)
        DeactivatePlayer(player);
    if (active_games.find(game_id) == active_games.end()) {
        active_games[game_id]=new Server_Game_UFO(game_id);
        try {
            db_conn.executenonquery("commit;");
            db_conn.executenonquery("begin transaction;");
        }
        catch(std::exception &ex) {
            server_log("Exception Occured: %s",ex.what());
        }
        server_log("Game %d activated.\n", game_id);
    } else {
        try {
            db_conn.executenonquery("commit;");
            db_conn.executenonquery("begin transaction;");
        }
        catch(std::exception &ex) {
            server_log("Exception Occured: %s",ex.what());
        }
    }
    try {
        player->position = db_conn.executeint32(
        "select position from ufo2000_game_players where game=%d and player='%s';",
        game_id,player->m_name.c_str());
    }
    catch(std::exception &ex) {
        server_log("Exception Occured: %s",ex.what());
    }
    player->game=active_games[game_id];
    player->game->players[player->position - 1] = player;
}

void Server_Game_UFO::DeactivatePlayer(ServerClientUfo* player)
{
    player->game->players[player->position - 1] = NULL;
    if(!player->game->players[0] && !player->game->players[1])
    {
        active_games.erase(player->game->game_id);
        server_log("Game %d deactivated.\n",player->game->game_id);
        db_conn.executenonquery("commit;");
        db_conn.executenonquery("begin transaction;");
        delete player->game;
    }
    player->game = NULL;
}


void Server_Game_UFO::PacketToServer(ServerClientUfo* sender, int packet_type, const std::string &packet) {
    try {
        if(packet == "result:victory") {
            if(sender -> position == 1)
                db_conn.executenonquery("update ufo2000_games set is_finished='Y', result=1 where id=%d;",game_id);
            else
                db_conn.executenonquery("update ufo2000_games set is_finished='Y', result=2 where id=%d;",game_id);
            std::string winner = db_conn.executestring("select p.player from ufo2000_games g,ufo2000_game_players p where g.id=%d and g.id=p.game and g.result=p.position;", game_id);
            std::string looser = db_conn.executestring("select p.player from ufo2000_games g,ufo2000_game_players p where g.id=%d and g.id=p.game and g.result!=p.position;", game_id);
            double winner_score = db_conn.executedouble("select elo_score from ufo2000_users where name='%s';", winner.c_str());
            double looser_score = db_conn.executedouble("select elo_score from ufo2000_users where name='%s';", looser.c_str());
            double winner_expected = 1 / (1 + pow(10, ((looser_score - winner_score) / 400)));
            double looser_expected = 1 / (1 + pow(10, ((winner_score - looser_score) / 400)));
            double k;
            if (winner_score <= 2400)
                k = 32;
            else
                k = 16;
            db_conn.executenonquery("update ufo2000_users set elo_score=%f where name='%s';", winner_score + (k * (1 - winner_expected)), winner.c_str());
            if (looser_score <= 2400)
                k = 32;
            else
                k = 16;
            db_conn.executenonquery("update ufo2000_users set elo_score=%f where name='%s';", looser_score + (k * (0 - looser_expected)), looser.c_str());
            db_conn.executenonquery("commit; ");
            db_conn.executenonquery("begin transaction;");
        }
        if(packet == "result:draw")
            if(sender -> position == 1)
                db_conn.executenonquery("update ufo2000_games set is_finished='Y', result=3 where id=%d;",game_id);
    }
    catch(std::exception &ex) {
        server_log("Exception Occured: %s",ex.what());
    }
    
    try {
//        db_conn.executenonquery("begin transaction;");
        db_conn.executenonquery("update ufo2000_games set last_received_packed=last_received_packed+1 where id=%d;",game_id);
        long int last_received_packed = db_conn.executeint32("select last_received_packed from ufo2000_games where id=%d;",game_id);
        db_conn.executenonquery("update ufo2000_game_players set last_sended_packet=%d where game=%d and player='%s';",
            last_received_packed, game_id, sender->m_name.c_str());
        //db_conn.executenonquery("commit;");

        time_t now = time(NULL);
        struct tm * t = localtime(&now);
        char timebuf[1000];
        strftime(timebuf, 1000, "%d/%m/%Y %H:%M:%S", t);

		sqlite3::command sql_cmd(db_conn, "\
            insert into ufo2000_game_packets\
            (game, id, sender, date, command, packet_type) values \
            (?, ?, ?, ?, ?, ?);");
        sql_cmd.parameters.push_back(sqlite3::parameter(1, (int)game_id));
        sql_cmd.parameters.push_back(sqlite3::parameter(2, (int)last_received_packed));
        sql_cmd.parameters.push_back(sqlite3::parameter(3, (int)sender->position));
        sql_cmd.parameters.push_back(sqlite3::parameter(4,timebuf,strlen(timebuf)));
		sql_cmd.parameters.push_back(sqlite3::parameter(5, (void*) packet.c_str(), packet.size()));
		sql_cmd.parameters.push_back(sqlite3::parameter(6, (int)packet_type));
		sql_cmd.executenonquery();

        ServerClientUfo* recipient = players[2-sender->position];

        if (packet_type == SRV_GAME_PACKET && recipient) {
            recipient->send_packet_back(SRV_GAME_PACKET, packet);
            db_conn.executenonquery("update ufo2000_game_players set last_sended_packet=%d where game=%d and player='%s';",
            last_received_packed, game_id, recipient->m_name.c_str());
        }
    }
    catch(std::exception &ex) {
        server_log("Exception Occured: %s",ex.what());
    }
}
