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
 * @@ Objective (alias.c)
 * 
 * This file contains the 'alias' command, and support code. Aliases
 * are used to allow a player to 'alias' one word for a command.
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* does aliasing and other fun stuff */
void substitute_alias (DESCRIPTOR_DATA * d, char *argument)
{
	CHAR_DATA *ch;
	char buf[MSL], 
		 prefix[MIL],
		 name[MIL];
	char *point;
	int	 alias;
	
	ch = d->original ? d->original : d->character;
	
	if (ch == NULL)
		return;
	
	/* check for prefix */
	if (ch->prefix[0] != '\0' && str_prefix ("prefix", argument))
	{
		if (strlen (ch->prefix) + strlen (argument) > MIL)
			send_to_char ("Line to long, prefix not processed.\r\n", ch);
		else
		{
			sprintf (prefix, "%s %s", ch->prefix, argument);
			argument = prefix;
		}
	}
	
	if (IS_NPC (ch) 
		|| IS_NULLSTR(ch->pcdata->alias[0]) 
		|| !str_prefix ("alias", argument) 
		|| !str_prefix ("prefix", argument))
	{
		interpret (d->character, argument);
		return;
	}
	
	strcpy (buf, argument);
	
	for (alias = 0; alias < MAX_ALIAS; alias++)	/* go through the aliases */
	{
		if (ch->pcdata->alias[alias] == NULL || ch->pcdata->alias[alias][0] == '\0' )
			break;
		
		if (!str_prefix (ch->pcdata->alias[alias], argument))
		{
			point = one_argument (argument, name);
			if (!strcmp (ch->pcdata->alias[alias], name))
			{
				buf[0] = '\0';
				strcat (buf, ch->pcdata->alias_sub[alias]);
				strcat (buf, " ");
				strcat (buf, point);
				break;
			}
			if (strlen (buf) > MIL)
			{
				send_to_char ("Alias substitution too long. Truncated.\r\n", ch);
				buf[MIL - 1] = '\0';
			}
		}
	}
	interpret (d->character, buf);
}

_DOFUN (do_alias)
{
	CHAR_DATA *rch;
	char arg[MIL], buf[MSL];
	int pos;
	
	smash_tilde (argument);
	
	if (ch->desc == NULL)
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;
	
	if (IS_NPC (rch))
		return;
	
	argument = one_argument (argument, arg);
	
	
	if (arg[0] == '\0')
	{
		
		if (rch->pcdata->alias[0] == NULL)
		{
			send_to_char ("You have no aliases defined.\n\r", ch);
			return;
		}
		send_to_char ("Your current aliases are:\n\r", ch);
		
		for (pos = 0; pos < MAX_ALIAS; pos++)
		{
			if (rch->pcdata->alias[pos] == NULL
				|| rch->pcdata->alias_sub[pos] == NULL)
				break;
			
			sprintf (buf, "    %s:  %s\n\r", rch->pcdata->alias[pos],
				rch->pcdata->alias_sub[pos]);
			send_to_char (buf, ch);
		}
		return;
	}
	
	if (!str_prefix ("una", arg) || !str_cmp ("alias", arg))
	{
		send_to_char ("Sorry, that word is reserved.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		for (pos = 0; pos < MAX_ALIAS; pos++)
		{
			if (rch->pcdata->alias[pos] == NULL
				|| rch->pcdata->alias_sub[pos] == NULL)
				break;
			
			if (!str_cmp (arg, rch->pcdata->alias[pos]))
			{
				sprintf (buf, "%s aliases to '%s'.\n\r", rch->pcdata->alias[pos],
					rch->pcdata->alias_sub[pos]);
				send_to_char (buf, ch);
				return;
			}
		}
		
		send_to_char ("That alias is not defined.\n\r", ch);
		return;
	}
	
	if (!str_prefix (argument, "delete") || !str_prefix (argument, "prefix"))
	{
		send_to_char ("That shall not be done!\n\r", ch);
		return;
	}
	
	for (pos = 0; pos < MAX_ALIAS; pos++)
	{
		if (rch->pcdata->alias[pos] == NULL)
			break;
		
		if (!str_cmp (arg, rch->pcdata->alias[pos]))	/* redefine an alias */
		{
			free_string (rch->pcdata->alias_sub[pos]);
			rch->pcdata->alias_sub[pos] = str_dup (argument);
			sprintf (buf, "%s is now realiased to '%s'.\n\r", arg, argument);
			send_to_char (buf, ch);
			return;
		}
	}
	
	if (pos >= MAX_ALIAS)
	{
		send_to_char ("Sorry, you have reached the alias limit.\n\r", ch);
		return;
	}
	
	/* make a new alias */
	rch->pcdata->alias[pos] = str_dup (arg);
	rch->pcdata->alias_sub[pos] = str_dup (argument);
	sprintf (buf, "%s is now aliased to '%s'.\n\r", arg, argument);
	send_to_char (buf, ch);
}


_DOFUN (do_unalias)
{
	CHAR_DATA *rch;
	char arg[MIL];
	int pos;
	bool found = FALSE;
	
	if (ch->desc == NULL)
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;
	
	if (IS_NPC (rch))
		return;
	
	argument = one_argument (argument, arg);
	
	if (arg == '\0')
	{
		send_to_char ("Unalias what?\n\r", ch);
		return;
	}
	
	for (pos = 0; pos < MAX_ALIAS; pos++)
	{
		if (rch->pcdata->alias[pos] == NULL)
			break;
		
		if (found)
		{
			rch->pcdata->alias[pos - 1] = rch->pcdata->alias[pos];
			rch->pcdata->alias_sub[pos - 1] = rch->pcdata->alias_sub[pos];
			rch->pcdata->alias[pos] = NULL;
			rch->pcdata->alias_sub[pos] = NULL;
			continue;
		}
		
		if (!strcmp (arg, rch->pcdata->alias[pos]))
		{
			send_to_char ("Alias removed.\n\r", ch);
			free_string (rch->pcdata->alias[pos]);
			free_string (rch->pcdata->alias_sub[pos]);
			rch->pcdata->alias[pos] = NULL;
			rch->pcdata->alias_sub[pos] = NULL;
			found = TRUE;
		}
	}
	
	if (!found)
		send_to_char ("No alias of that name to remove.\n\r", ch);
}
