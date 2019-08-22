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
 * @@ Objective (bit_h.c)
 *
 * This file contains the support code for handling bits. Converting them to
 * strings, searching for them in the various bit_tables, etc. Note: The
 * bit tables themselves are contained in tables.c
 */



#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



char *bit_string (const struct flag_type *flag_table, SET bits)
{
	sh_int count = 0;
	static char buf[MIL];
	
	memset (buf, 0, MIL);
	
	for (count = 0; flag_table[count].name != NULL; count++)
	{
		if (!flag_table[count].name || flag_table[count].name[0] == '\0')
			break;
		
		if (IS_FLAG (bits, flag_table[count].bit))
		{
			strcat (buf, flag_table[count].name);
			strcat (buf, " ");
		}
		
	}
	
	return buf;
}

char *bit_flags (const struct flag_type *flag_table, long bit)
{
	SET set;
	
	set_init(set);
	SET_FLAG(set, bit);
	
	
	return bit_string(flag_table, set);
}


int bit_value (const struct flag_type *flag_table, const char *flag)
{
	int i = 0;
	
	for (i = 0; flag_table[i].name != NULL; i++)
		if (!str_prefix (flag_table[i].name, flag))
			return flag_table[i].bit;
		
		return NOT_SET;
}

/*
 * The code below uses a table lookup system that is based on suggestions
 * from Russ Taylor.  There are many routines in handler.c that would benefit
 * with the use of tables.  You may consider simplifying your code base by
 * implementing a system like below with such functions. -Jason Dinkel
 */
extern int flag_lookup (const char *word, const struct flag_type *flag_table);


/** Function: flag_pos
  * Descr   : Returns a flags postion in the array.
  * Returns : (int, -1 on failure)
  * Syntax  : flag_pos( flag_array, flag_name )
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int flag_pos (const struct flag_type *table, char *flag)
{
	int i = -1;
	
	if (table == NULL)
		return i;
	
	for (i = 0; table[i].name != NULL; i++)
	{
		if (!str_prefix (table[i].name, flag))
			return i;
	}
	
	return -1;
}



/*****************************************************************************
  Name:		is_stat( table )
  Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
  Called by:flag_value and flag_string.
  Note:		This function is local and used only in bit.c.
****************************************************************************/
bool is_stat (const struct flag_type * flag_table)
{
	int flag;
	
	for (flag = 0; flag_stat_table[flag].structure; flag++)
	{
		if (flag_stat_table[flag].structure == flag_table
			&& flag_stat_table[flag].stat)
			return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
  Name:		flag_value( table, flag )
  Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
  Called by:olc.c and olc_act.c.
****************************************************************************/
int flag_value (const struct flag_type *flag_table, char *argument)
{
	char word[MIL];
	int bit;
	int marked = 0;
	bool found = FALSE;
	
	if (argument[0] == '\0')
		return NO_FLAG;
	
	if (is_stat (flag_table))
		return flag_lookup (argument, flag_table);
	
	/*
 	 * Accept multiple flags.
	 */
	for (;;)
	{
		argument = one_argument (argument, word);
		
		if (word[0] == '\0')
			break;
		
		if ((bit = flag_lookup (word, flag_table)) != NO_FLAG)
		{
			SET_BIT (marked, bit);
			found = TRUE;
		}
	}
	
	if (found)
		return marked;
	else
		return NO_FLAG;
}



/*****************************************************************************
  Name:		flag_string( table, flags/stat )
  Purpose:	Returns string with name(s) of the flags or stat entered.
  Called by:act_olc.c, olc.c, and olc_save.c.
****************************************************************************/
char *flag_string (const struct flag_type *flag_table, int bits)
{
	static char buf[2][512];
	static int cnt = 0;
	int flag;
	
	if (++cnt > 1)
		cnt = 0;
	
	buf[cnt][0] = '\0';
	
	for (flag = 0; flag_table[flag].name != NULL; flag++)
	{
		if (!is_stat (flag_table) && IS_SET (bits, flag_table[flag].bit))
		{
			strcat (buf[cnt], " ");
			strcat (buf[cnt], flag_table[flag].name);
		}
		else if (flag_table[flag].bit == bits)
		{
			strcat (buf[cnt], " ");
			strcat (buf[cnt], flag_table[flag].name);
			break;
		}
	}
	return (buf[cnt][0] != '\0') ? buf[cnt] + 1 : "none";
}

/** Function: who_flags
  * Descr   : Sets buf to various 'WHO' bits for display in do_who
  *         : and show_char_to_char_0
  * Returns : char *
  * Syntax  : who_flags ( ch )
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *who_flags (CHAR_DATA * vch)
{
	static char buf[30];
	
	buf[0] = '\0';
	
	/* note: can_see is assumed to be true */
	
	if (vch->incog_level >= LEVEL_HERO)
	{
		if (vch->incog_level >= 100)
			sprintf (buf, "`N(`bI00`N)`w");
		else
			sprintf (buf, "`N(`bI%d`N)`w ", vch->incog_level);
	}
	
	if (vch->invis_level >= LEVEL_HERO)
	{
		if (vch->invis_level >= 100)
			sprintf (buf, "`N(`bW00`N)`w ");
		else
			sprintf (buf, "`N(`bW%d`N)`w ", vch->invis_level);
	}
	
	if (IS_FLAG (vch->states.config, _AFK))
		strcat (buf, "`N[`RAFK`N]`w ");
	if (IS_FLAG (vch->states.config, _BUILDING))
		strcat (buf, "[`NBuild`w] ");
	if (IS_FLAG (vch->states.config, _PKQUESTOR))
		strcat (buf, "[`RPkQ`w] ");
	if (IS_FLAG (vch->states.config, _RPQUESTOR))
		strcat (buf, "[`CRpQ`w] ");
	
	if (buf[0] == '\0')
		return "";
	else
		return buf;
}
