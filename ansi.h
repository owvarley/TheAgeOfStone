/**
 * @@ Copyright (Wheel of Time Project)
 *
 * Wheel of Time (WoT) Mud System v0.11a
 * Copyright (c) 1998, by Gary McNickle <gary@tarmongaidon.org>
 * Based on ROM Mud, v2.4b4 by Russ Taylor
 *
 * By using any of the WoT source code, you agree to follow the terms of
 * usage and liscensing detailed in the file "liscense.txt"
 *
 * The author(s) retain full copyright to this package and are granting you a
 * non-exclusive right to use it.  Any distrobutions or changes to this code
 * must contain the original copyrights as contained in this header.
 */


/*
 * ANSI_KEY  This is the character you want the ANSI code parser to
 * interpret as the signal to process the next character(s) as ANSI
 * color
 */

#define ANSI_KEY '`'


/* Color Definitions */
#define BLACK			30
#define RED				31
#define GREEN			32
#define YELLOW			33
#define BLUE			34
#define MAGENTA			35
#define CYAN			36
#define WHITE			37

/* Background colors */
#define RED_BG			"\x01B[41m"
#define BLUE_BG			"\x01B[44m"
#define GREEN_BG		"\x01B[42m"
#define BLACK_BG		"\x01B[40m"
#define WHITE_BG		"\x01B[47m"
#define MAGENTA_BG		"\x01B[45m"
#define YELLOW_BG		"\x01B[43m"
#define CYAN_BG			"\x01B[46m"
#define HRED_BG 		"\x01B[1;41m"


/* Color Modes */
#define NORMAL 			"\x01B[0;%dm"
#define BOLD			"\x01B[1;%dm"
#define NO_COLOR		"\x01B[0;37m"
#define BLINK           "\x01B[5;%dm"
#define REVERSE			"\x01B[7;%dm"
#define TILDE           "~"

#define MAX_CODES 32

/* Room color config
 * Lets ya pick a different color for the exits displayed in autoexits
 */
#define TITLECOLOR      "`B"
#define AEXITC          "`B"

/* Channel color codes
 * To change the colors of your channels from the defaults to customized
 * simply change the `color below
 */
#define GOSSIPC         "`y"
#define AUCTIONC        "`W"
#define OOCC            "`Y"
#define QUESTIONC       "`m"
#define ANSWERC         "`B"
#define QUOTEC          "`C"
#define GRATSC          "`g"
#define IMMORTALC       "`W"
#define SHOUTSC         "`N"
#define TELLSC          "`r"
#define REPLIESC        "`g"
#define SAYC            "`c"
#define OSAYC           "`c"
#define PRAYC           "`y"
#define BONDC			"`c"
#define GTALKC          "`c"

/* Channel header colors
 * these defines let you set the color on the [channel name] which gives
 * you much more options on configuring your mud to your own tastes.
 * the defines below are done as channel name with the color codes in it.
 * an example might be Immortal = `NIm`wm`Wor`wt`Nal`w
 */

#define OOCH                    "`YOOC`w"
#define GOSSIPH                 "Rumor sweeps across the land"
#define AUCTIONH                "`WAuction`w"
#define QUESTIONH               "`mQuestion`w"
#define ANSWERH                 "`BAnswer`w"
#define QUOTEH                  "`CQuote`w"
#define GRATSH                  "`GGrats`w"
#define IMMORTALH               "`NI`wm`Wm`w"
#define SHOUTSH                 "`bShouts`w"
#define OSAYH					"`Coocly`w"
#define BONDH					"`RBond`w"
