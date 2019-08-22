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
 * non-exclusive right to use it.  Any distributions or changes to this code
 * must contain the original copyrights as contained in this header.
 */

/** 
 * @@ Copyright (ROM Consortium)
 *
 *       ROM 2.4 is copyright 1993-1995 Russ Taylor			   
 *       ROM has been brought to you by the ROM consortium		   
 *           Russ Taylor (rtaylor@pacinfo.com)				   
 *           Gabrielle Taylor (gtaylor@pacinfo.com)			   
 *           Brian Moore (rom@rom.efn.org)				   
 *       By using this code, you have agreed to follow the terms of the    
 *       ROM license, in the file Rom24/doc/rom.license                    
 */

/**
 * @@ Copyright (Diku Team)
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   
 *									   
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   
 *  Chastain, Michael Quan, and Mitchell Tse.				   
 *									   
 *  In order to use any part of this Merc Diku Mud, you must comply with   
 *  both the original Diku license in 'license.doc' as well the Merc	   
 *  license in 'license.txt'.  In particular, you may not remove either of 
 *  these copyright notices.						   
 *									   
 *  Much time and thought has gone into this software and you are	   
 *  benefitting.  We hope that you share your changes too.  What goes	   
 *  around, comes around.						   
 */

/**
 * @@ Objective (ansi.c)
 *
 * Contains the code for handling ANSI color conversion as well as the 
 * cursor control commands.
 *
 * Author: Gary McNickle <gary@tarmongaidon.org>
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"

 /* Color code system
 
  This is a simple, 2 character key system that can easily be used by
  the players and should be fairly easy to remember. For use within the
  code, either the key method may be used, or the defines following them.
  If the defines are used instead of the color codes, the character will
  get ansi codes even if they have set ansi to off.
  
   
	Dark Colors		Brights 	Blinking 	Backgrounds   
	----------------------------------------------------------------------
	b = Blue		B = Blue	0 = Blue	! = Blue	
	c = Cyan		C = Cyan	1 = Cyan	# = Cyan
	g = Green		G = Green	2 = Green	$ = Green
	m = Magenta		M = Magenta	3 = Magenta	% = Magenta
	r = Red			R = Red		4 = Red		^ = Red
	w = White  	 	W = White	5 = White	& = White
	y = brown		Y = Yellow 	6 = Yellow	* = Brown
	n = Dark Grey	N = Black		
	
	 - = Tilde		w = Reset code
	 
*/

struct ansi_codes
{
	char code;
	// Add these to be able to convert text to ansi, or to html, or html to ansi,
	// etc...
	// Removed -- now done programatically
	// char *ansi;
	// char *html; 
	//
};

const struct ansi_codes ansi_table[MAX_CODES] = {
	{'b'},
	{'c'},
	{'g'},
	{'m'},
	{'r'},
	{'w'},
	{'y'},
	{'n'},
	{'B'},
	{'C'},
	{'G'},
	{'M'},
	{'R'},
	{'W'},
	{'Y'},
	{'N'},
	{'0'},
	{'1'},
	{'2'},
	{'3'},
	{'4'},
	{'5'},
	{'6'},
	{'!'},
	{'#'},
	{'$'},
	{'%'},
	{'^'},
	{'&'},
	{'*'},
	{'+'},
	{'-'}
};

/** Function: count_codes
  * Descr   : Used to determine the number of mud color codes in a string
  * Returns : Number of codes in string
  * Syntax  : count_codes ( colored string )
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int count_codes (char *str)
{
	int codes = 0;
	bool count;
	
	while (*str != '\0')
	{
		count = FALSE;
		
		if (*str != ANSI_KEY)
		{
			str++;
			continue;
		}
		else
			
		{
			int z;
			
			str++;
			
			for (z = 0; z < MAX_CODES; z++)
				if (*str == ansi_table[z].code)
				{
					codes++;
					continue;
				}
		}
	}
	
	return codes;
}


/** Function: contains_ansi
  * Descr   : Determines if a string contains ANSI codes, OR Mud color codes.
  * Returns : TRUE or FALSE
  * Syntax  : contains_ansi ( string in question )
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool contains_ansi (char *str)
{
	int i;
	
	if (strstr (str, "\x01B["))
		return TRUE;
	
	while (*str != '\0')
	{
		if (*str == ANSI_KEY)
			for (i = 0; i < MAX_CODES; i++)
				if (*str++ == ansi_table[i].code)
					return TRUE;
				str++;
	}
	
	return FALSE;
}


/* move the cursor up a number of rows */
void cursor_up (CHAR_DATA * ch, int rows)
{
	chprintf (ch, "\x01B[%uA", rows);
}

/* move the cursor down a number of rows */
void cursor_down (CHAR_DATA * ch, int rows)
{
	chprintf (ch, "\x01B[%uB", rows);
}

/* move the cursor forward a number of columns */
void cursor_forward (CHAR_DATA * ch, int cols)
{
	chprintf (ch, "\x01B[%uC", cols);
}

/* move the cursor backwards a number of columns */
void cursor_back (CHAR_DATA * ch, int cols)
{
	chprintf (ch, "\x01B[%uD", cols);
}

/* save the current cursor position */
void save_cursor (CHAR_DATA * ch)
{
	send_to_char ("\x01B[s", ch);
}

/* retore the cursor position to a previously saved location */
void restore_cursor (CHAR_DATA * ch)
{
	send_to_char ("\x01B[r", ch);
}

/* erase the current line, beginning from current cursor position, to end of line */
void erase_eol (CHAR_DATA * ch)
{
 /* note: error in DOS documentation 
  * lists K as lowercase 
  */
  send_to_char ("\x01B[K", ch);
}
  
/* go to xy, then erase the line from that cursor position to the end of line */
void erase_xyeol (CHAR_DATA * ch, int col, int row)
{
	chprintf (ch, "\x01B[%u;%uH\x01B[K", row, col);
}

/* beep the character */
void beep_char (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	
	if ((victim = get_char_world (ch, argument)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	chprintf (victim, "\007%s needs your attention.\n\r", PERS (ch, victim));
	chprintf (ch, "You beep %s\n\r", PERS (victim, ch));
}

/* move the cursor to column x, row y */
void goto_xy (CHAR_DATA * ch, int col, int row)
{
	chprintf (ch, "\x01B[%u;%uH", row, col);
}

/* clear the screen and place the cursor in default position */
void clear_screen (CHAR_DATA * ch)
{
	send_to_char ("\x01B[2J", ch);
}

/* at column x, row y, send string "str" to char */
void send_to_char_xy (CHAR_DATA * ch, int col, int row, char *str)
{
	chprintf (ch, "\x01B[%u;%uH%s", row, col, str);
}

/* at row y, send string "str" to char, and center on the screen */
void send_to_char_center_xy (CHAR_DATA * ch, int row, char *str)
{
	chprintf (ch, "\x01B[%u;%uH%s", row, ((80 - (strlen (str) - 1)) / 2),
		str);
}
