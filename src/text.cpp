/*
This file is part of UFO2000 (http://ufo2000.sourceforge.net)

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

/**
 * Ufo2000: Text for messages and help-system
 */

#include "stdafx.h"

#include "global.h"
#include "colors.h"
#include "text.h"

#include "video.h"
#include "icon.h"

#include "soldier.h"
#include "mouse.h"

/**
 * Return string with current date+time,
 * formatted as YYYY-MM-DD HH:MM:SS.
 */
const char *datetime()
{
    time_t now = time(NULL);
    struct tm * t = localtime(&now);
    static char timebuf[128];
    strftime(timebuf, 128, "%Y-%m-%d %H:%M:%S", t);

    return timebuf;
}

/**
 * Call Message() from main.lua to write a message 
 * into the logfile init-scripts.log
 * This logfile is mostly for debugging.
 */
void lua_message(const std::string &str1)
{
    lua_pushstring(L, "Message");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    lua_pushstring(L, "# %s : %s");
    lua_pushstring(L, datetime());
    lua_pushstring(L, str1.c_str());
    lua_safe_call(L, 3, 0);
};

/** 
 * Write message into file battlereport.txt.
 * This logfile is for game-events relevant to the player, and the endgame-stats.
 */
void battle_report( const char *format, ... )
{
    va_list arglist;

    FILE *logFile;
    logFile = fopen( F("$(home)/battlereport.txt"), "a" );

    va_start( arglist, format );
  //vfprintf( stderr, format, arglist );
    vfprintf( logFile, format, arglist );
    va_end( arglist );

  //fflush( stderr );
    fclose( logFile );
}


/**
 * Translate the input-string to a foreign language.
 * Instead of the full gnu-gettext-package, a LUA-script is used
 * to read the texts and translations from a .po - file.
 * The language-file to use is configured in ufo2000.ini.
 *
 * Todo: investigate performance ...
 */
#undef map
std::map<const char *, const char *> translation_cache;

const char *ufo2k_gettext(const char *str)
{ 
    if (L == NULL)   // lua is not yet working 
        return str;  // return input-string unchanged

    if (translation_cache.find(str) != translation_cache.end())
        return translation_cache[str];

    int stack_top = lua_gettop(L);
	lua_pushstring(L, "TranslatedMessages");
	lua_gettable(L, LUA_GLOBALSINDEX);
	if (!lua_istable(L, -1)) {
	    lua_settop(L, stack_top);
        return str;   // no translation-table available
	}
  //lua_message( std::string("gettext: '") + str );  // debug

	lua_pushstring(L, str);
	lua_gettable(L, -2);
	if (!lua_isstring(L, -1)) {
	    lua_settop(L, stack_top);
		return str;   // ?
	}

    // Let's hope that the translated strings are never deleted from
    // the 'TranslatedMessages' table
	const char *translated_str = lua_tostring(L, -1);
    lua_settop(L, stack_top);

  //char test[1000];
  //sprintf( test, "gettext: '%s'='%s'", str, translated_str );
  //lua_message( test );
    translation_cache[str] = translated_str;
    return translated_str;
};

/**
 * Get a line of text from the textfile tips-xx.txt via lua-script,
 * where xx is the language-code (en=english, de=german, etc.)
 * Tip #0 contains the total number of tips in the file.
 * If no file exists, SetLanguage() puts one tip into the table.
 */
const char *get_tip( const int tip_nr )
{ 
    if (L == NULL)   // lua is not yet working 
        return "";

    int stack_top = lua_gettop(L);
    lua_pushstring(L, "TipsOfTheDay");
    lua_gettable(L, LUA_GLOBALSINDEX);
    if (!lua_istable(L, -1)) {
        lua_settop(L, stack_top);
        return "-";   // no tips-table available
    }
    lua_pushnumber(L, tip_nr);
    lua_gettable(L, -2);
    if (!lua_isstring(L, -1)) {
        lua_settop(L, stack_top);
        return "?";
    }
    const char *tip_str = lua_tostring(L, -1);
    lua_settop(L, stack_top);
    return tip_str;
};

//! dialog color for help-index + tip-of-the-day 
#define _FG COLOR_WHITE
//! dialog background color
#define _BG COLOR_BLACK1

/**
 * Show a tip-of-the-day:
 * get a line of text from textfile tips-xx.txt via lua-script, 
 * show it with alert3(), 
 * browse with buttons 'next-tip' and 'random-tip'.
 */
void showtip()
{
    int b1       =   0;    //<! button-number the user pressed
    int nr       =   1;    //<! current Tip-Number
    int maxtips  =   9;    //<! number of tips read from file
    int lasttip  =   0;    //<! number of last tip shown

  //int kp_0     =   0;    //<! Keypress: 0 : default
  //int kp_ok    =  27;    //<! Keypress for ok=close  :  13:ENTER / 27:ESC
  //int kp_rnd   = 114;    //<! Keypress for random tip: 114:"r"
  //int kp_next  = 110;    //<! Keypress for next tip  : 110:"n"

    char rndtip[32], next[32], ok[32];
    char title[64];
    char line1[256];

    maxtips = atoi( get_tip(0) );

// Todo: Calculate keycode from "&R" in button-text, 
// support hotkey in custom dialog.
    sprintf(ok,     "%s", _("  CLOSE  ")   );
    sprintf(rndtip, "%s", _("&RANDOM TIP") ); // with hotkey-marker
    sprintf(next,   "%s", _("&NEXT TIP")   );
    sprintf(rndtip, "%s", _("RANDOM TIP")  ); // without hotkey
    sprintf(next,   "%s", _("NEXT TIP")    );

    int tw =     8, th =     8;  // Textwidth, height
    int xM =  1*tw, yM =    th;  // Margin
    int x0 =    75, y0 =   150,    w0 =   512,   h0 = 12*th;  // Whole box
    int x1 = x0+xM, y1 = y0+yM,    w1 = w0-2*xM, h1 =    th;  // Title
    int x2 = x0+xM, y2 = y1+2*th,  w2 = w1,      h2 =  4*th;  // Textbox
    int x3 = x0+xM, y3 = y2+h2+th, w3 = w1/3-xM, h3 =  2*th;  // Button1
    int p1 = x3,    p2 = p1+w3+xM, p3 = x0+w0-xM-w3;          // Button-positions
    p2 = x0 + (w0/2) - w3/2;

    static DIALOG the_dialog[] =
    {
        // (dialog proc)     (x) (y) (w) (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3)
        { d_shadow_box_proc, x0, y0, w0, h0, _FG, _BG,   0,  0,       0,   0,  NULL,            NULL, NULL }, 
        { d_text_proc,       x1, y1, w1, h1, _FG, _BG,   0,  0,       0,   0,  (void *) title,  NULL, NULL },
        { d_textbox_proc,    x2, y2, w2, h2, _FG, _BG,   0,  0,       0,   0,  (void *) line1,  NULL, NULL },
        { d_button_proc,     p1, y3, w3, h3, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *) rndtip, NULL, NULL },
        { d_button_proc,     p2, y3, w3, h3, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *) ok,     NULL, NULL },
        { d_button_proc,     p3, y3, w3, h3, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *) next,   NULL, NULL },
        { NULL,               0,  0,  0,  0, _FG, _BG,   0,  0,       0,   0,  NULL,            NULL, NULL }
    };

/*
    sprintf(title,  "%50s #%d/%d", _("Tip"), nr, lasttip );
    b1 = alert3( title,
                 _("Be nice !"),
                 _("Have fun !"),
                 rndtip, ok,    next,
                 kp_rnd, kp_ok, kp_next );
*/
    lasttip = get_config_int("General", "LastTip", 0);
    nr = lasttip + 1;

  //while ( b1 != 2 ) {  // alert3() : ok=close
    while ( b1 != 4 ) {  // dialog   : Button 2 is dialog-element #4

        if ( nr > maxtips ) 
           nr = 1;
        if ( nr < 1 ) 
           nr = maxtips;

        sprintf(title,  "%s #%d/%d", _("Tip"), nr, maxtips );
      //sprintf(line2,  "%s", _("Have fun!") );
        sprintf(line1,  "%s", get_tip(nr) );

    b1 = do_dialog(the_dialog, -1);
  //sprintf(title,  "%s: %d", "Tip-Button", b1 );
  //lua_message( title );
/*
        sprintf(title,  "%50s #%d/%d", _("Tip"), nr, maxtips );
        b1 = alert3( title,
                 get_tip(nr),
                 "",
               //line1, line2,
                 rndtip, ok,    next,
                 kp_rnd, kp_ok, kp_next );
*/
      //if ( b1 == 1 ) {  // Random Tip
        if ( b1 == 3 ) {  // Random Tip
            nr = rand() % maxtips;
        }
      //if ( b1 == 3 ) {  // Next Tip
        if ( b1 == 5 ) {  // Next Tip
            nr++;
        }
    }
    set_config_int("General", "LastTip", nr);
};

// moved here from place.cpp :
//static char place_name[11][20];
const char *place_name[11];

/**
 * Translations for placenames used in the inventory
 */
void Init_place_names()
{
  //lua_message( "Enter: Init_place_names" );
    place_name[P_SHL_RIGHT  ] = _("RIGHT SHOULDER");
    place_name[P_SHL_LEFT   ] = _("LEFT SHOULDER");
    place_name[P_ARM_RIGHT  ] = _("RIGHT HAND");
    place_name[P_ARM_LEFT   ] = _("LEFT HAND");
    place_name[P_LEG_RIGHT  ] = _("RIGHT LEG");
    place_name[P_LEG_LEFT   ] = _("LEFT LEG");
    place_name[P_BACK       ] = _("BACK PACK");
    place_name[P_BELT       ] = _("BELT");
    place_name[P_MAP        ] = _("GROUND");
    place_name[P_ARMOURY    ] = _("ARMOURY");
    place_name[P_COMMON_POOL] = _("COMMON POOL");
}

/**
 * Text for the tooltips of the buttons on the control-panel.
 *
 * The texts must be short, otherwise they will be cut off
 * at the right edge of the battlescape-window.
 */
const char *icontext( const int icon_nr )
{
    static std::string txt;

    switch (icon_nr) {
        //                            ....+....1....+....2....+....3
        case B_MAN_UP      : txt = _("Move up one level");   break;
        case B_MAN_DOWN    : txt = _("Move down one level"); break;

        case B_VIEW_UP     : txt = _("View up one level");   break;
        case B_VIEW_DOWN   : txt = _("View down one level"); break;

        case B_MAP         : txt = _("Show Map");            break;
        case B_CROUCH      : txt = _("Stand up / Crouch");   break;

        case B_INVENTORY   : txt = _("Inventory");           break;
        case B_CENTER_VIEW : txt = _("CenterView");          break;

        case B_NEXT_MAN    : txt = _("Jump to Next Man");    break;
        case B_NEXT_MAN_2  : txt = _("Done with this man");  break;

        case B_TOGGLE_ROOF : txt = _("Toggle Roof");         break;
        case B_OPTIONS     : txt = _("Options");             break;

        // when screen-width is set to minimum, "Exit Game" barely fits
        case B_DONE        : txt = _("Done with turn");      break;
        case B_EXIT        : txt = _("Exit Game");           break;

        case B_MAN_STATS   : txt = _("Show Stats of man");   break;
        case B_BARCHART    : txt = _("Switch Barchart");     break;

        case R_TIME_FREE + BUTTON_NUMBER : 
                             txt = _("Reserve Time: None");
                             break;
        case R_TIME_AIM  + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Aimed shot");
                             break;
        case R_TIME_SNAP + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Snapshot");
                             break;
        case R_TIME_AUTO + BUTTON_NUMBER : 
                             txt = _("Reserve Time: Autoshot");
                             break;

        default            : txt = "icon ??"; 
    }
    return txt.c_str();
};


//! Definitions and Procedures for GUI help-index:
/*
static const char *help_chapters[] = {
    "Introduction",
    "Battlescape",
    "Mapview",
    "Inventory",
    "Stats-View",
    "Endgame-Stats",
    "Network",
    "Mission-Planner",
    "Scenarios",
    NULL
};
*/

/**
 * Callback function to specify the contents of the listbox,
 * e.g. the help-chapters
 */
const char *help_chapters(int index, int *list_size)
{
    static char listbox_text[10][32];

    if (index < 0) {
       sprintf( listbox_text[0], _("Introduction") );
       sprintf( listbox_text[1], _("Battlescape") );
       sprintf( listbox_text[2], _("Mapview") );
       sprintf( listbox_text[3], _("Inventory") );
       sprintf( listbox_text[4], _("Stats-View") );
       sprintf( listbox_text[5], _("Endgame-Stats") );
       sprintf( listbox_text[6], _("Network") );
       sprintf( listbox_text[7], _("Mission-Planner") );
       sprintf( listbox_text[8], _("Scenarios") );
       *list_size = 9;
       return NULL;
    }
    else
       return listbox_text[index];
};

/**
 * Help-Index: show GUI-dialog to let user choose a help-chapter
 */
int select_help()
{
    int ret, sel;
    int x0 =    24, y0 =     8, w0 =   280, h0 = 140;
    int x1 = x0+20, y1 = y0+16, w1 = w0-32, h1 =  16;
    int x2 = x0+20, y2 = y0+32, w2 = w0-32, h2 =  96;
    static DIALOG the_dialog[] =
    {
        /* (dialog proc)     (x)   (y)   (w)   (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)              (dp2) (dp3) */
        { d_shadow_box_proc, x0,   y0,   w0,   h0, _FG, _BG,   0,  0,       0,   0,  NULL,              NULL, NULL }, 
        { d_text_proc,       x1,   y1,   w1,   h1, _FG, _BG,   0,  0,       0,   0,  (void *)_("Select Help-Page"), NULL, NULL },
        { d_list_proc,       x2,   y2,   w2,   h2, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *)help_chapters, NULL, NULL },
      //{ d_button_proc,     20,  200,  100,   20, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *)_("OK"),   NULL, NULL },
      //{ d_button_proc,    180,  200,  100,   20, _FG, _BG,   0,  D_EXIT,  0,   0,  (void *)_("Cancel"), NULL, NULL },
        { NULL,               0,    0,    0,    0, _FG, _BG,   0,  0,       0,   0,  NULL,              NULL, NULL }
    };

  //ret = do_dialog(the_dialog, -1);
	centre_dialog(the_dialog);

	set_dialog_color(the_dialog, COLOR_BLACK1, COLOR_WHITE);
	//ret = popup_dialog(the_dialog, -1);
	popup_dialog(the_dialog, -1);

    sel = the_dialog[ 2 ].d1;   // from listbox-entry
  //return sel;

    if (sel == 0) return HELP_INTRO;
    if (sel == 1) return HELP_BATTLESCAPE;
    if (sel == 2) return HELP_MAPVIEW;
    if (sel == 3) return HELP_INVENTORY;
    if (sel == 4) return HELP_STATS;
    if (sel == 5) return HELP_ENDGAME;
    if (sel == 6) return HELP_NET;
    if (sel == 7) return HELP_PLANNER;
    if (sel == 8) return HELP_SCENARIO;
    return HELP_U2K_INDEX;
};

/**
 * Show a single help-page when KEY_F1 is pressed.
 * Each page of help can have only 3 lines of text.
 * The user can browse between pages with the prev & next-buttons,
 * the ok-button exits help.
 *
 * There must be a gap between the pages of different help-chapters.
 * The number 'startpage-1' of each help-chapter is reserved for a jump to the index-page.
 */
void help( const int helppage )
// Page    0   : Help-Index (Todo)
// Page    1..8: Reserved
// Pages  10+ 3: UFO2000-Introduction (extended ABOUT-Box)
// Pages  20+15: Help for Battlescape
// Pages  40+ 1: Help for Mapviewer
// Pages  50+ 1: Help for Mindprobe (Todo)
// Pages  60+ 1: Help for Motion-Scanner (Todo)
// Pages  70+ 1: Help for Medi-kit (Todo)
// Pages  80+ 1: Help for PSI-Amp  (Todo)
// Pages  90+ 1: Help for Blaster-Launcher (Todo)
// Pages 100+ 9: Inventory Help
// Pages 110+ 9: Stats Help
// Pages 130+ 1: Endgame-Stats Help
// Pages 140+ 4: Network Help
//
// Pages 160+10: Help for Mission-Planner and Rules
// Pages 171++: Scenario-Help (Todo)
{
    int b1       =   0;    //<! button-number the user pressed
    int kp_0     =   0;    //<! Keypress: 0 : default
    int hp       =  10;
    int kp_index = 105;    //<! Keypress for indexpage: 105:"i"
    int kp_prev  = 112;    //<! Keypress for prev.page: 112:"p"
    int kp_ok    =  27;    //<! Keypress for ok=close :  13:ENTER / 27:ESC
    int kp_next  = 110;    //<! Keypress for nextpage : 110:"n"
    char index[32], prev[32], next[32], ok[32];
    char esc2exit[64];

// Todo: Calculate keycode from "&I" in button-text
    sprintf(index, "%s", _("HELP-&INDEX") );
    sprintf(prev,  "%s", _("&PREV PAGE")  );
    sprintf(next,  "%s", _("&NEXT PAGE")  );
    sprintf(ok,    "%s", _("  CLOSE  ")   );

    sprintf( esc2exit, "%s", _("To leave, press ESC.") );
  //char test[128];
  //sprintf( test,  "Charset-Test: %s", 
  //         _("DE: ae=�  oe=�  ue=�  Ae=�  Oe=�  Ue=�  sz=�") 
  //);
/*
    FILE *f1 = fopen( "gettext.log", "at");
    fprintf(f1, "#: '%s'\n", prev);
    fclose(f1);
 */
    MouseRange temp_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    rest(1); // Don't eat all CPU resources

    switch (helppage) {
        case HELP_U2K_INDEX + 0 :
            hp = select_help();
          //sprintf( test,  "Selected: %d", hp );
          //b1 = alert3( "", test, "",  NULL, ok, NULL,  kp_prev, kp_ok, kp_0 );
            help( hp );
            break;

        // UFO2000 - Introduction :
        case HELP_INTRO - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_INTRO + 0 :
            b1 = alert3( _("UFO2000 is a multiplayer game in the spirit of 'X-COM: UFO Defense'."),
                         _("At the moment, only the tactical part of the game is playable,"),
                         _("e.g. two teams of soldiers and/or aliens fighting each other."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_INTRO + 1 :
            b1 = alert3( _("UFO2000 needs data (graphics, sound, maps) from an original XCOM-game."),
                         _("Files from the XCOM-Demo can be used if you don't own XCOM-1,"),
                         _("but this demo only has a few of the maps and aliens."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INTRO + 2 :
            b1 = alert3( _("If you also have Terror-from-the-Deep (aka UFO-2) or the TFTD-demo,"),
                         _("some additional maps from this game can be used."),
                         _("(But XCOM-1 is always required)"),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INTRO + 3 : 
            b1 = alert3( _("UFO2000 can be played hotseat (solo, or 2 players on 1 computer),"),
                         _("or via LAN or internet.  The offical server runs on lxnt.info:2001."),
                         _("There is no AI implemented yet."),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Battlescape:
        case HELP_BATTLESCAPE - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_BATTLESCAPE + 0 :
            b1 = alert3( _("UFO2000 is controlled mainly with the mouse, and these keys:"),
                         _("F1: Help, F2+F3: Save+Load, F9: Keymap, F10: Window/Fullscreen, F12: Screenshot"),
                         _("Keypad+/-: adjust Music-Volume, Shift+space: Pause, ESC: Quit."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 1 :
            b1 = alert3( _("The active soldier is highlighted by a yellow arrow above him."),
                         _("Click on any of your soldiers to make him active."),
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 2 :
            b1 = alert3( _("To move the active soldier, left-click on the map."),
                         _("Right-click turns him towards the cursor, opens doors,"),
                         _("and stops movement (e.g. in case of a mis-click)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 3 :
            b1 = alert3( _("To fire a gun, click on the weapon in the left or right hand,"),
                         _("select the fire-mode (aimed, snapshot, autoshot), then click"),
                         _("on the target.  Use the PgUp&&PgDown-buttons to aim at other levels."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 4 :
            b1 = alert3( _("Grenades should be primed before they are thrown."),
                         _("The delay is the number of rounds before they go off."),
                         _("Zero delay means they explode at the end of your turn."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 5 :
            b1 = alert3( _("While you hold a grenade in your hand, it does not countdown,"),
                         _("but a high-explosive does !"),
                         _("Primed proxy-grenades get hot when thrown or put on the ground."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 6 :
            //              ....+....1....+....2....+....3....+....4....+....5....+....6
            b1 = alert3( _("The control-panel at the bottom of the screen:"),
                         _("The boxes at the left and right show the items in hand, e.g. weapons."),
                         _("Between them are some buttons and a small status-display."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 7 :
            b1 = alert3( _("Control-panel:  Buttons in column 1:"),
                         _("Climb up && down a level."),
                         _("This works only when standing on a gravlift."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 8 :
            b1 = alert3( _("Control-panel:  Buttons in column 2 (Ladder-symbol):"),
                         _("Change level up && down: Look (and aim) at other levels."),
                         _("Maps have 4 levels.  Keyboard-shortcut: PgUp && PgDn-Keys."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 9 :
            b1 = alert3( _("Control-panel:  Buttons in column 3:"),
                         _("Map-Display: Invoke the map-viewer for a small overhead-view."),
                         _("Stand up / Kneel:  Kneeling improves aiming (10%)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 10 :
            b1 = alert3( _("Control-panel:  Buttons in column 4:"),
                         _("Inventory: access your belt, backpack etc., and items on the ground."),
                         _("Center view: scroll map-display to center around the current man."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 11 :
            b1 = alert3( _("Control-panel:  Buttons in column 5:"),
                         _("Switch to next man, who still has TUs left.      "),
                         _("Switch to next man, and mark current man as done."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 12 :
            b1 = alert3( _("Control-panel:  Buttons in column 6:"),
                         _("Show/hide other levels (e.g. show roof of buildings etc.)"),
                         _("Config-Dialog: adjust movement speed, scrolling speed etc."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 13 :
            b1 = alert3( _("Control-panel:  Buttons in column 7:"),
                         _("End your turn."),
                         _("Quit: abort mission (same as ESC)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 14 :
            b1 = alert3( _("Control-panel:  green&&red buttons at the lower left:"),
                         _("Green: Reserve no time, move freely.                   "),
                         _("Red:   Reserve time for aimed shot, snapshot, autofire."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_BATTLESCAPE + 15 :
            b1 = alert3( _("Control-panel:  big brown button in the center"),
                         _("(shows turn-number):"),
                         _("Invoke statistics-display for current man."),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mapviewer:
        case HELP_MAPVIEW - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MAPVIEW + 0 :
            b1 = alert3( _("The map-viewer shows a small overhead-view of the map."),
                         _("To look at other levels, use the up&&down-buttons at the left."),
                         _("To leave, click on the red OK-button, or press ESC."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MAPVIEW + 1 :
            b1 = alert3( _("A click on the map-area centers the view to this spot."),
                         "",
                         _("Unexplored areas are shown as black."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_MAPVIEW + 2 : 
            b1 = alert3( _("Yellow dots: your men.             "),
                         _("Red dots   : enemy soldiers.       "),
                         _("Blue dots  : map-tiles with damage."),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mindprobe:
        case HELP_MINDPROBE - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MINDPROBE + 0 :
            b1 = alert3( "With a mindprobe you can look at the stats of enemy soldiers.",
                         "(Todo)",
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MINDPROBE + 1 : 
            b1 = alert3( "Mindprobe:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Motion-Scanner:
        case HELP_SCANNER - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_SCANNER + 0 :
            b1 = alert3( "A Motion-Scanner indicates motion during the last enemy turn.",
                         "(Todo)",
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_SCANNER + 1 : 
            b1 = alert3( "Motion-Scanner:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Medi-kit:
        case HELP_MEDIKIT - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_MEDIKIT + 0 :
            b1 = alert3( "A Medi-kit can heal wounded soldiers.",
                         "(Todo)",
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_MEDIKIT + 1 : 
            b1 = alert3( "Medi-kit:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for PSI-amp :
        case HELP_PSI_AMP - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_PSI_AMP + 0 :
            b1 = alert3( "To use a Psi-Amp, a soldiers needs high psi-strength.",
                         "(Todo)",
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_PSI_AMP + 1 : 
            b1 = alert3( "Psi-amp",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Blaster-Launcher:
        case HELP_BLASTER - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_BLASTER + 0 :
            b1 = alert3( "The Blaster-Launcher is an advanced alien rocket-launcher.",
                         "(Todo)",
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_BLASTER + 1 : 
            b1 = alert3( "Blaster-Launcher:",
                         "(Todo)",
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Inventory:
        case HELP_INVENTORY - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_INVENTORY + 0 :
            b1 = alert3( _("Inventory: here you can manage the weapons"),
                         _("and equipment of your soldier."),
                         _("To leave, press the OK-button, or goto the next/previous man."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 1 : // Todo: save+load single soldiers
            b1 = alert3( _("In the mission-planner, you can use F2 && F3"),
                         _("to save && load a team of soldiers."),
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 2 :
            b1 = alert3( _("Use the mouse to move items between the slots"),
                         _("for hands, belt, backpack etc."),
                         _("Note: during battle, each such move costs time."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 3 :
            b1 = alert3( _("To unload a weapon, move it over the clip-button"),
                         _("(top right, below the arrow-buttons) and click there."),
                         _("You must have your second hand free."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 4 :
            b1 = alert3( _("To load an empty weapon, hold it in one hand,"),
                         _("and move ammo of the matching type into the weapon-slot."),
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 5 : 
            // Todo: Flying Armor
            b1 = alert3( _("The armor cannot be changed during a battle."),
                         _("Flying armor does not work yet (it only has stronger armor)."),
                         _("Click on the picture of the soldier to get to his stats."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;

        // Todo: Extra chapter for Quick-Setup, connected to Inventory ??
        case HELP_INVENTORY + 6 : 
            b1 = alert3( _("Quick-Setup: (experimental feature)"),
                         _("With F5..F8 a predefined soldier-type can be selected."),
                         _("Shift-F5..F8 set up these keys."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 7 : 
            b1 = alert3( _("Quick-Setup:"),
                         _("DEL: delete equipment of the current soldier."),
                         _("TAB/Shift-TAB: jump to next/previous soldier."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 8 : 
            b1 = alert3( _("Quick-Setup:"),
                         _("F9: generate a name for the current soldier."),
                         _("Shift-F9: configure the name-generator."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_INVENTORY + 9 : 
            b1 = alert3( _("Quick-Setup:"),
                         _("F11: cycle thru apperances of the current soldier. "),
                         _("F12: cycle thru armor-types of the current soldier."),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Stats-Display:
        case HELP_STATS - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_STATS + 0 :
            b1 = alert3( _("Stats-Display: the current attributes of the soldier."),
                         "",
                         _("To leave, press ESC or click a mousebutton."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_STATS + 1 :
            b1 = alert3( _("Stats:"),
                         _("Time-Units: every action costs time."),
                         _("Energy: some actions also cost energy (e.g. walking)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 2 :
            b1 = alert3( _("Stats:"),
                         _("Health: when this reaches zero, the soldier dies."),
                         _("Fatal Wounds: amount of health-loss each round."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 3 :
            b1 = alert3( _("Stats:"),
                         _("Stun: shown as an overlay to health. When the stun-value"),
                         _("reaches the value of health, the soldier gets unconscious."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 4 :
            b1 = alert3( _("Stats:"),
                         _("Bravery: (not yet implemented - same for all man)"),
                         _("Morale: current morale. Goes down when own men die."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 5 :
            b1 = alert3( _("Stats:"),
                         _("Reactions: chance for firing at an enemy during their round."),
                         _("This needs time for at least a snapshot."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 6 :  // Todo: Knife
            b1 = alert3( _("Stats:"),
                         _("Firing accuracy: for pistols and guns."),
                         _("Throwing accuracy: for grenades."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 7 :
            b1 = alert3( _("Stats:"),
                         _("Strength: this determines how much the soldier can carry."),
                         _("When overloaded, he gets less TU each round."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 8 : // Todo: Psi-Amp
            b1 = alert3( _("Stats:"),
                         _("PSI-Strength: (not yet implemented)"), "",
                         //"PSI-Skill:",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_STATS + 9 :
            b1 = alert3( _("Stats:"),
                         _("Armor: protection-value on all sides of the soldier."),
                         _("Under-armor protects against nearby explosions."),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Endgame-Stats:
        case HELP_ENDGAME - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_ENDGAME + 0 :
            b1 = alert3( _("Endgame-Statistics:"),
                         "",
                         _("A summary of kills and damage for both teams."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_ENDGAME + 1 :
            b1 = alert3( _("Endgame-Statistics:"),
                         _("You can stay here to chat in private with your partner."),
                         esc2exit,
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Network - Connect to Server / Chat:
        case HELP_NET - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_NET + 0 :
            b1 = alert3( _("Connect to an UFO2000-Server on your LAN (e.g. 192.168.1.2:2000)"),
                         _("or on the Internet (e.g. the offical server lxnt.info:2001)."),
                         esc2exit,
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_NET + 1 :
            b1 = alert3( _("To get a valid login-name at the official server,"),
                         _("register at http://ufo2000.lxnt.info/admin.php"),
                         _("Registration is free, and no personal data is required."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 2 :
            b1 = alert3( _("To chat, just type your message, and press ENTER."),
                         _("To challenge another player, or accept his/her challenge,"),
                         _("click on his/her name in the player-list at the right."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 3 :
            b1 = alert3( _("Colors of the playernames:"),
                         _("White: You.                             "),
                         _("Gray : Someone else, available for chat."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_NET + 4 :
            b1 = alert3( _("Red   : He/she is busy, playing with someone else."),
                         _("Yellow: You have sent a challenge to him/her.     "),
                         _("Green : You can accept a challenge from him/her.  "),
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;

        // Help for Mission-Planner:
        case HELP_PLANNER - 1 :
            help( HELP_U2K_INDEX );
            break;
        case HELP_PLANNER + 0 :
            b1 = alert3( _("Mission-Planner: here you can select a map and a scenario,"),
                         _("set the game-rules, and select soldiers for a mission."),
                         _("When finished, press SEND, then START. To leave, press ESC."),
                         index, ok, next, kp_index, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 1 :
            b1 = alert3( _("To edit one of your soldiers, CTRL-click on his name."),
                         _("Left-click to place a soldier on the map (your side only),"),
                         _("right-click to remove him."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 2 :  // Todo: access to Load,Save,Edit from mission-planner
            b1 = alert3( _("SAVE && LOAD for a team can be found on the soldier-edit-screen."),
                         "",
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 3 :
            b1 = alert3( _("To change the scenario, click on the text below 'MATCH SETTINGS'."),
                         _("To generate a new map, click on the terrain-name,"),
                         _("then click on LOAD, NEW or select a new size (4*4, 5*5, 6*6)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 4 :
            b1 = alert3( _("Click on 'Game rules' for fine-tuning the settings:"),
                         _("Explosives, Pointlimit, Turnlimit, Timelimit, FOW, Items on Ground."),
                         _("You should negotiate them via chat with your partner."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 5 :
            b1 = alert3( _("Gamerules - Explosives:"),
                         _("What kind of grenades and rockets are allowed."),
                         _("(0=no explosives .. 3=everything allowed)"),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 6 :
            b1 = alert3( _("Gamerules - Pointlimit:"),
                         _("How many points you can spend on your soldiers,"),
                         _("their attributes, armor, weapons and equipment."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 7 :  // ?? explain turn vs. half-turn
            b1 = alert3( _("Gamerules - Turnlimit:"),
                         _("How many turns the match will last."),
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 8 :
            b1 = alert3( _("Gamerules - Timelimit:"),
                         _("How many seconds each turn lasts."),
                         _("This is for fast, blitz-type games."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 9 :
            b1 = alert3( _("Gamerules - Map Exploration Level at start of game:"),
                         _("0: map not explored, 1: deployment-area explored, 2: whole map explored."),
                         _("Unexplored (not yet seen) parts of the map are shown as black."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        case HELP_PLANNER + 10 :
            b1 = alert3( _("Gamerules - Ground"),
                         _("If this is off, you cannot leave the editor while items"),
                         _("are on the ground (e.g. heavy rockets to take on the mission)."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
                         // kp_next: continue with scenario-help
            break;

        // Scenario-Help:
        case HELP_SCENARIO + 0 :
            b1 = alert3( _("There are several scenario-types available:"),
                         _("Deathmatch, Escape, Control, Sabotage, Capture ..."),
                         "",
                         prev, ok, next, kp_prev, kp_ok, kp_next);
                         // kp_prev: continue with help for mission-planner
            break;
        case HELP_SCENARIO + 1 :
            b1 = alert3( _("Deathmatch - the most basic scenario:"),
                         _("To win, you just have to survive,"),
                         _("and kill or stun everybody else."),
                         prev, ok, next, kp_prev, kp_ok, kp_next);
            break;
        // TODO - Scenario-developers, please help yourself !!!
        case HELP_SCENARIO + 2 :
            b1 = alert3( "", 
                         _("(to be continued)"),
                         "",
                         prev, ok, NULL, kp_prev, kp_ok, kp_0);  // no "next"-button
            break;


        default:
            // no help page: just exit
            break;
    }
    if (b1 == 1)
        help( helppage - 1 );  // Previous help-page
    if (b1 == 3)
        help( helppage + 1 );  // Next help-page
}
END_OF_FUNCTION(help);

/**
 * Show a large dialog with formatted text. It is useful for showing help
 * pages with long descriptions of something.
 */
void show_help(const char *text)
{
    int w = SCREEN_W * 3 / 4;
    int h = SCREEN_H * 3 / 4;
    DIALOG help_dialog[] = {
        //(dialog proc)           (x)  (y) (w) (h) (fg)(bg) (key) (flags) (d1) (d2) (dp) (dp2) (dp3)
        { d_agup_shadow_box_proc,  0,   0, w, h, 0,  1,  0, 0,  0, 0, NULL, NULL, NULL },
        { d_agup_textbox_proc,    10,  10, w - 20, h - 40, 0,  1,  0, 0,  0, 0, (void *)text, NULL, NULL },
        { d_agup_button_proc,     w - 70, h - 25, 60,  18,  0,  1, 13, D_EXIT, 0, 0, (void *)_("OK"), NULL, NULL },
        { d_yield_proc,           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL},
        { NULL }
    };
    FONT *old_font = font;
    font = large;
    centre_dialog(help_dialog);
    set_dialog_color(help_dialog, COLOR_BLACK1, COLOR_WHITE);
    popup_dialog(help_dialog, 2);
    font = old_font;
}

std::string string_vformat(const char *fmt, va_list arglist)
{
    int bufsize = 1024;
    char *buf = new char[bufsize];
    while (true) {
        int result = vsnprintf(buf, bufsize, fmt, arglist);
        if (result >= 0 && result < bufsize)
            break;
        delete [] buf;
        bufsize *= 2;
        buf = new char[bufsize];
    }
    std::string text = buf;
    delete [] buf;
    return text;
}

std::string string_format(const char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    std::string text = string_vformat(fmt, arglist);
    va_end(arglist);
    return text;
}
