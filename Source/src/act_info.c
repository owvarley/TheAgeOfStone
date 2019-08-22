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
 * @@ Objective (act_info.c)
 *
 * This file contains general (non-imm) informational commands for players
 * immortal information commands are found primarily in act_wiz.c
 *
 * Also contains support functions related to information, such as the greet
 * and places code.
 */

#define _GNU_SOURCE

#if defined(WIN32)
  #include <time.h>
#else
  #include <sys/time.h>
  #include <unistd.h>
  #include <time.h>
#endif


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "merc.h"
#include "recycle.h"

#if defined(_WIN32)
  #pragma warning(disable:4273)   // 'symbol' : : inconsistent dll linkage.  dllexport assumed.
#endif


/* for do_count */
int max_on = 0;
WIZ_DATA *wiz_list;



/** Function: do_bounties
  * Descr   : Display bounty amounts of all online players
  * Returns : n/a
  * Syntax  : n/a
  * Written : v1.0 05/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_bounties)
{
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	int count = 0;
	
	if (!is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, _THIEF_TAKER))
	{
		send_to_char ("You'll have to seek out a thieftaker to get"
			" that information.\n\r", ch);
		return;
	}
	
	send_to_char ("The following people have bounties posted:\n\r"
		"------------------------------------------\n\r", ch);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		vch = CH (d);
		
		if (!IS_NPC (vch) && !IS_IMMORTAL (vch) && can_see (ch, vch)
			&& vch->pcdata->bounty > 0)
		{
			count++;
			chprintf (ch,
				"%-20.20s     $%-7ld\n\r", vch->name,
				vch->pcdata->bounty);
			
		}
		
	}
	
	if (count < 1)
		send_to_char ("None\n\r", ch);
	else
		chprintf (ch, "%d bount%s listed.\n\r", count,
		(count > 1) ? "ies" : "y");
	
}

/** Function: do_config
  * Descr   : Displays and allows for editing of various character
  *         : configuration states
  * Returns : n/a
  * Syntax  : config [option|state]
  * Written : v1.0 05/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_config)
{
	char arg1[MIL];
	char arg2[MIL];
	bool turnOn = FALSE;
	int flag = 0;
	
	if (!argument || argument[0] == '\0')
	{
		send_to_char ("Current Configuration\n\r"
			"--------------------------------------------\n\r", ch);
		/**
 		  * Note: the keys (in [] brackets) should match the entries in
		  * the _config_bits table (tables.c)
		  * If this expands much at all beyond the 18 or so original options,
		  * then it would definately be better to table this...
		  */
		chprintf (ch,
			"[Affects  ] Show Affects in 'score'    [%-3.3s]\n\r"
			"[AFK      ] Away From Keyboard         [%-3.3s]\n\r"
			"[Ansi     ] Ansi Color                 [%-3.3s]\n\r"
			"[Brief    ] Brief Mode                 [%-3.3s]\n\r"
			"[Combine  ] Combine Items in 'inv'     [%-3.3s]\n\r"
			"[Compact  ] Compact Display            [%-3.3s]\n\r"
			"[Damage   ] Show Damage                [%-3.3s]\n\r"
			"[Deaf     ] Deaf Mode                  [%-3.3s]\n\r"
			"[Echo     ] Echo text in note editor   [%-3.3s]\n\r"
			"[Empty    ] Show Empty slots in 'eq'   [%-3.3s]\n\r"
			"[Follow   ] Allow Followers            [%-3.3s]\n\r"
			"[Guild    ] Show Guild affiliation     [%-3.3s]\n\r"
			"[Hunger   ] Hunger/Thirst Messages     [%-3.3s]\n\r"
			"[MudFTP   ] MudFTP                     [%-3.3s]\n\r"
			"[Prompt   ] Prompt                     [%-3.3s]\n\r"
			"[Quiet    ] Quiet Mode                 [%-3.3s]\n\r"
			"[Sound    ] Mud Sound Protocol         [%-3.3s]\n\r"
			"[Telnet   ] Telnet Go Ahead Compatible [%-3.3s]\n\r"
			"[Weather  ] Weather Messages           [%-3.3s]\n\r",
			IS_FLAG (ch->states.config, _SHOW_AFFECTS) ?"On" : "Off",
			IS_FLAG (ch->states.config, _AFK) ?			"On" : "Off",
			IS_FLAG (ch->states.config, _ANSI) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _BRIEF) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _COMBINE) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _COMPACT) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _SHOW_DAMAGE) ? "On" : "Off",
			IS_FLAG (ch->states.config, _DEAF) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _ECHO) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _SHOW_EMPTY) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _NOFOLLOW) ?	"Off": "On",
			IS_FLAG (ch->states.config, _SHOWGUILD) ?	"On" : "Off",
			IS_FLAG (ch->states.config, _NOHUNGER) ?	"Off": "On",
			IS_FLAG (ch->states.config, _MUDFTP) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _PROMPT) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _QUIET) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _SOUNDON) ?		"On" : "Off",
			IS_FLAG (ch->states.config, _TELNET_GA) ?	"On" : "Off",
			IS_FLAG (ch->states.config, _NOWEATHER) ?	"Off": "On");
		
		if (IS_IMMORTAL (ch))
		{
			chprintf (ch,
				"----------Immortal Specific States----------\n\r"
				"[Building ] Building Mode              [%-3.3s]\n\r"
				"[Holylight] Holy Light                 [%-3.3s]\n\r"
				"[WhoLevel ] Show Level in who          [%-3.3s]\n\r"
				"[WhoWhere ] Show Where from in who     [%-3.3s]\n\r"
				"[WhoStat  ] Show Sex and Class in who  [%-3.3s]\n\r",
				IS_FLAG (ch->states.config, _BUILDING)  ? "On" : "Off",
				IS_FLAG (ch->states.config, _HOLYLIGHT) ? "On" : "Off",
				IS_FLAG (ch->states.config, _WHO_LEVEL) ? "On" : "Off",
				IS_FLAG (ch->states.config, _WHO_WHERE) ? "On" : "Off",
				IS_FLAG (ch->states.config, _WHO_SEX)   ? "On" : "Off");
		}
		return;
	}
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] != '+' && arg1[0] != '-')
	{
		send_to_char ("Syntax: config +/- <flag>\n\r"
			"Use + to turn a flag On, and - to turn it off.\n\r", ch);
		return;
	}
	
	turnOn = (arg1[0] == '+');
	
	if ((flag = flag_pos (_config_bits, arg2)) != -1
		&& (_config_bits[flag].settable || IS_IMMORTAL (ch)))
	{
		chprintf (ch, "%s flag set.\n\r",
			capitalize (_config_bits[flag].name));
		if (turnOn)
			SET_FLAG (ch->states.config, _config_bits[flag].bit);
		else
			REMOVE_FLAG (ch->states.config, _config_bits[flag].bit);
		
	}
	else
	{
		send_to_char ("Invalid Option.\n\r", ch);
		do_config (ch, "");
	}
	
}


/** Function: do_greet
  * Descr   : Introduces one player to another. Setting the introducee's
  *         : "known" data accordingly.
  * Returns : void
  * Syntax  : greet someone|name
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Credit  : Thanks to Erwin Andreasen for the idea and code skeleton.
  */
_DOFUN (do_greet)
{
	CHAR_DATA *victim = NULL;
	char arg1[MIL];
	
	if (argument[0] == '\0')
	{
		send_to_char ("Greet whom?\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, arg1);
	
	if ((victim = get_char_room (ch, arg1)) == NULL)
	{
		send_to_char ("Greet whom?\n\r", ch);
		return;
	}
	
	if (victim->position < POS_RESTING)
	{
		send_to_char ("They are not paying any attention to you.\n\r", ch);
		return;
	}
	
	act ("You greet $N.", ch, NULL, victim, TO_CHAR, POS_RESTING);
	
	if (knows_char (victim, ch))
		chprintf (ch, "%s smiles knowingly.\n\r", PERS (victim, ch));
	else
		add_known (victim, ch);
	
	chprintf (victim, "%s %s\n\r", ch->name,
		(argument[0] ==
		'\0') ? "nods in greeting to you." : argument);
	
}


/** Function: knows_char
  * Descr   : Determines if victim is a player known by ch.
  * Returns : TRUE/FALSE
  * Syntax  : knows_char ( ch, victim )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool knows_char (CHAR_DATA * ch, CHAR_DATA * victim)
{
	int i;
	
	if (ch == victim || IS_NPC (ch) || IS_NPC (victim) ||
		IS_IMMORTAL (ch) || IS_IMMORTAL (victim))
		return TRUE;
	
	for (i = 1; i < ch->pcdata->known[0] + 1; i++)
		if (ch->pcdata->known[i] == GET_ID(victim))
			return TRUE;
		
		return FALSE;
}


/** Function: nation_id
  * Descr   : Determines (and returns) the proper nation string for a given
  *         : pc.
  * Returns : char *
  * Syntax  : nation_id ( who )
  * Written : v1.0 04/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *nation_id (CHAR_DATA * ch)
{
	sh_int nation = -1;
	
	if (IS_NPC (ch))
		return ch->short_descr;
	
	nation = national_number (ch->pcdata->nation);
	
	if (nation < 0
		|| !nation_table[nation].nationid
		|| nation_table[nation].nationid[0] == '\0')
		return "Someone";
	
	return nation_table[nation].nationid;
}

/** Function: PERS
  * Descr   : Replacment function for old PERS macro.  This uses
  *         : "knows_char" to determine which value to send show to
  *         : viewer when displaying ch. 'Someone' or their name?
  * Returns : Value to show to viewer
  * Syntax  : PERS(ch being looked at, who's looking)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *PERS (CHAR_DATA * ch, CHAR_DATA * viewer)
{
	static char buf[MSL];
	
	if (!can_see (viewer, ch))
		return "Someone";
	
	if (IS_NPC (ch))
		return ch->short_descr;
	
	if (IS_NPC (viewer) ||
		IS_IMMORTAL (ch) || IS_IMMORTAL (viewer) || knows_char (viewer, ch))
		return ch->name;

	buf[0] = '\0';
	
	return buf;
}

/** Function: add_known
  * Descr   : Adds 'victim' to ch's known list
  * Returns : void
  * Syntax  : add_known( to this ch's list, who to add )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void add_known (CHAR_DATA * ch, CHAR_DATA * victim)
{
	int *memory;
	
	if (knows_char (ch, victim))
		return;
	
	memory = alloc_mem (sizeof (int) * (ch->pcdata->known[0] + 1));
	memory[0] = ch->pcdata->known[0] + 1;
	memcpy (memory + 1, ch->pcdata->known + 1,
		ch->pcdata->known[0] * sizeof (int));
	memory[ch->pcdata->known[0] + 1] = GET_ID(victim);
	
	free_mem (ch->pcdata->known, sizeof (int) * ch->pcdata->known[0]);
	
	ch->pcdata->known = memory;
}

/** Function: do_places
  * Descr   : Displays any/all places in the room. Will also display
  *         : Who's at the places, or where a specific person is at.
  * Returns : output only
  * Syntax  : do_places (who|all|people) (here) (persons name)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_places)
{
	CHAR_DATA *vch;
	CHAR_DATA *who = NULL;
	OBJ_DATA *obj;
	int counter = 0;
	bool fAll = FALSE;
	bool here = FALSE;
	
	if (argument[0] != '\0')
	{
		who = get_char_room (ch, argument);
		if (who == NULL)
			fAll = is_name (argument, "who all people");
		if (!fAll)
			here = !str_cmp (argument, "here");
	}
	
	if (here && (!ch->on || !IS_SET (ch->on->value[2], IS_PLACE)))
	{
		send_to_char ("But your not at a specific place!\n\r", ch);
		return;
	}
	
	if (fAll || here || who != NULL)
	{
		for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
		{
			if (can_see (ch, vch) && vch->on
				&& IS_SET (vch->on->value[2], IS_PLACE))
			{
				if (fAll ||
					(here && (vch->on == ch->on)) || (who != NULL && who == vch))
					chprintf (ch, "[%2d] %s is at %s\n\r",
					++counter, vch->name, vch->on->short_descr);
			}
		}
		
		if (counter < 1)
			chprintf (ch, "You didn't find %s at a place.\n\r",
			who == NULL ? "anyone" : "them");
		
		return;
	}
	
	if (argument[0] != '\0')
	{
		chprintf (ch, "You didn't find %s at a place.\n\r",
			who == NULL ? "anyone" : "them");
		
		return;
	}
	
	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
	{
		if (IS_SET (obj->value[2], IS_PLACE) && can_see_obj (ch, obj))
		{
			chprintf (ch, "[%2d] %s\n\r", ++counter, obj->short_descr);
		}
	}
	
	if (counter < 1)
		send_to_char ("There are no special places in this spot.\n\r", ch);
	
	return;
}


/** Function: format_obj_to_char
  * Descr   : Parses various information about an object into a string
  *         : that is then displayed to the person looking at the object.
  * Returns : parsed string
  * Syntax  : obj looked at, who's looking, use short or long descriptions
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
char *format_obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
	static char buf[MSL];
	
	buf[0] = '\0';
	
	if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
		|| (obj->description == NULL || obj->description[0] == '\0'))
		return buf;
	
	if (IS_OBJ_STAT (obj, ITEM_INVIS))
		strcat (buf, "(Inverted) ");
	if (
		(IS_AFFECTED (ch, _AFF_DETECT_EVIL)
		|| IS_AFFECTED (ch, _AFF_SENSE_EVIL)) && IS_OBJ_STAT (obj, ITEM_EVIL))
		strcat (buf, "(Red Aura) ");
	if (IS_OBJ_STAT (obj, ITEM_HIDDEN))
		strcat (buf, "(Hidden)  ");
	
	if (fShort)
	{
		if (obj->short_descr != NULL)
			strcat (buf, obj->short_descr);
	}
	else
	{
		if (obj->description != NULL)
			strcat (buf, obj->description);
	}
	
	return buf;
}


/** Function: show_list_to_char
  * Descr   : Show a list to a character, can coalesce duplicate items
  * Returns : void
  * Syntax  : list of objects to view, who's looking, coaelesce or not, show 'nothing' if the 
  *         : list is empty?
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void show_list_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
						bool fShowNothing)
{
	char buf[MSL];
	BUFFER *output;
	char **prgpstrShow;
	int *prgnShow;
	char *pstrShow;
	OBJ_DATA *obj;
	int nShow;
	int iShow;
	int count;
	bool fCombine;
	
	if (ch->desc == NULL)
		return;
	
	/*
	 * Alloc space for output lines.
	 */
	output = new_buf ();
	
	count = 0;

	for (obj = list; obj != NULL; obj = obj->next_content)
		count++;
		
	prgpstrShow = alloc_mem (count * sizeof (char *));
	prgnShow = alloc_mem (count * sizeof (int));
	nShow = 0;
	
	/*
	* Format the list of objects.
	*/
	for (obj = list; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
		{
			pstrShow = format_obj_to_char (obj, ch, fShort);
			
			fCombine = FALSE;
			
			if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
			{
			/*
			* Look for duplicates, case sensitive.
			* Matches tend to be near end so run loop backwords.
				*/
				for (iShow = nShow - 1; iShow >= 0; iShow--)
				{
					if (!strcmp (prgpstrShow[iShow], pstrShow))
					{
						prgnShow[iShow]++;
						fCombine = TRUE;
						break;
					}
				}
			}
			
			/*
			* Couldn't combine, or didn't want to.
			*/
			if (!fCombine)
			{
				prgpstrShow[nShow] = str_dup (pstrShow);
				prgnShow[nShow] = 1;
				nShow++;
			}
		}
	}
	
	/*
	* Output the formatted list.
	*/
	for (iShow = 0; iShow < nShow; iShow++)
	{
		if (prgpstrShow[iShow][0] == '\0')
		{
			free_string (prgpstrShow[iShow]);
			continue;
		}
		
		if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
		{
			if (prgnShow[iShow] != 1)
			{
				sprintf (buf, "(%2d) ", prgnShow[iShow]);
				add_buf (output, buf);
			}
			else
			{
				add_buf (output, "     ");
			}
		}
		add_buf (output, prgpstrShow[iShow]);
		add_buf (output, "\n\r");
		free_string (prgpstrShow[iShow]);
	}
	
	if (fShowNothing && nShow == 0)
	{
		if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
			send_to_char ("     ", ch);
		
		send_to_char ("Nothing.\n\r", ch);
	}
	page_to_char (buf_string (output), ch);
	
	/*
	* Clean up.
	*/
	free_buf (output);
	free_mem (prgpstrShow, count * sizeof (char *));
	free_mem (prgnShow, count * sizeof (int));
	
	return;
}


/** Function: show_char_to_char_0
  * Descr   : Parses character (pc/npc) information into a descriptive 
  *         : string. Modified to pass that string to calling function
  *			: instead of send it to the char.
  * Returns : Description string
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MSL];
	char message[MSL];
	
	buf[0] = '\0';
	
	if (IS_FLAG (victim->states.config, _AFK))
		strcat (buf, "[AFK] ");
	if (IS_FLAG (victim->states.config, _PKQUESTOR))
		strcat (buf, "[PKQ] ");
	if (IS_FLAG (victim->states.config, _BUILDING))
		strcat (buf, "[Build] ");
	if (IS_AFFECTED (victim, _AFF_INVISIBLE))
		strcat (buf, "(Inverted) ");
	if (victim->invis_level >= LEVEL_HERO)
		strcat (buf, "(Wizi) ");
	if (IS_AFFECTED (victim, _AFF_HIDE))
		strcat (buf, "(Hide) ");
	if (IS_EVIL (victim) && IS_AFFECTED (ch, _AFF_SENSE_EVIL))
		strcat (buf, "(Red Aura) ");
	if (IS_LINKDEAD (victim))
		strcat (buf, "(Linkdead) ");
	if (can_channel (ch))
	{
  	  if (is_affected (victim, gsn_seize) &&
		  is_affected (ch, gsn_seize))
	    strcat (buf, "(SAIDIN) ");
	  if (is_affected (victim, gsn_embrace) &&
		  TRUESEX(ch) == SEX_FEMALE && can_channel(ch))
	    strcat (buf, "(Golden Nimbus) ");
	}
	 
	if (victim->position == victim->start_pos && victim->long_descr[0] != '\0')
	{
		strcat (buf, victim->long_descr);
		send_to_char (buf, ch);
		return;
	}
	
	strcat (buf, PERS (victim, ch));
	
	if (!IS_NPC (victim)		/* && !IS_FLAG(ch->states.config, _BRIEF) */
		&& victim->position == POS_STANDING && ch->on == NULL)
	{
		
		if ( IS_FLAG(pMud.config, _ALLOW_RDESC) )
		{
			if (victim->pcdata->r_desc && victim->pcdata->r_desc[0] != '\0')
			{
				if (victim->pcdata->r_desc[0] != ' ')
					strcat (buf, " ");
				
				strcat (buf, victim->pcdata->r_desc);
			}
		}
		else
 		  strcat (buf, victim->pcdata->title);
	}
	
	switch (victim->position)
	{
	case POS_DEAD:
		strcat (buf, " is DEAD!!");
		break;
	case POS_MORTAL:
		strcat (buf, " is mortally wounded.");
		break;
	case POS_INCAP:
		strcat (buf, " is incapacitated.");
		break;
	case POS_STUNNED:
		strcat (buf, " is lying here stunned.");
		break;
	case POS_SLEEPING:
		
		if (victim->on != NULL)
		{
			if (IS_SET (victim->on->value[2], SLEEP_AT))
			{
				sprintf (message, " is sleeping at %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else if (IS_SET (victim->on->value[2], SLEEP_ON))
			{
				sprintf (message, " is sleeping on %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else
			{
				sprintf (message, " is sleeping in %s.", victim->on->short_descr);
				strcat (buf, message);
			}
		}
		else
			strcat (buf, " is sleeping here.");
		break;
	case POS_RESTING:
		if (victim->on != NULL)
		{
			if (IS_SET (victim->on->value[2], REST_AT))
			{
				sprintf (message, " is resting at %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else if (IS_SET (victim->on->value[2], REST_ON))
			{
				sprintf (message, " is resting on %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else
			{
				sprintf (message, " is resting in %s.", victim->on->short_descr);
				strcat (buf, message);
			}
		}
		else
			strcat (buf, " is resting here.");
		break;
		
	case POS_SITTING:
		if (victim->on != NULL)
		{
			if (IS_SET (victim->on->value[2], SIT_AT))
			{
				sprintf (message, " is sitting at %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else if (IS_SET (victim->on->value[2], SIT_ON))
			{
				sprintf (message, " is sitting on %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else
			{
				sprintf (message, " is sitting in %s.", victim->on->short_descr);
				strcat (buf, message);
			}
		}
		else
			strcat (buf, " is sitting here.");
		break;
		
	case POS_STANDING:
		if (victim->on != NULL)
		{
			if (IS_SET (victim->on->value[2], STAND_AT))
			{
				sprintf (message, " is standing at %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else if (IS_SET (victim->on->value[2], STAND_ON))
			{
				sprintf (message, " is standing on %s.", victim->on->short_descr);
				strcat (buf, message);
			}
			else
			{
				sprintf (message, " is standing in %s.", victim->on->short_descr);
				strcat (buf, message);
			}
		}
		
		else if (MOUNTED (victim))
		{
			strcat (buf, " is here, riding ");
			strcat (buf, MOUNTED (victim)->short_descr);
			strcat (buf, ".");
		}
		else
		{			
			if ( IS_FLAG(pMud.config, _ALLOW_RDESC) )
			{
				if (!IS_NPC (victim) && victim->pcdata->r_desc[0] == '\0')
					strcat (buf, " is here.");
			}
			else
			{
				if (!IS_NPC (victim))
					strcat (buf, " is here.");
			}
		}
		break;
				
	case POS_FIGHTING:
		strcat (buf, " is here, fighting ");
		if (victim->fighting == NULL)
			strcat (buf, "thin air??");
		else if (victim->fighting == ch)
			strcat (buf, "YOU!");
		else if (victim->in_room == victim->fighting->in_room)
		{
			strcat (buf, PERS (victim->fighting, ch));
			strcat (buf, ".");
		}
		else
			strcat (buf, "someone who left??");
		break;
		
	default:
		break;
  }
  
  strcat (buf, "\n\r");
  buf[0] = UPPER (buf[0]);
  send_to_char (buf, ch);
}


char *condition_string (int percent)
{
	static char buf[MSL];
	
	memset (buf, 0, MSL);
	
	if (percent >= 100)
		strcat (buf, " is in excellent condition.\n\r");
	else if (percent >= 90)
		strcat (buf, " has a few scratches.\n\r");
	else if (percent >= 75)
		strcat (buf, " has some small wounds and bruises.\n\r");
	else if (percent >= 50)
		strcat (buf, " has quite a few wounds.\n\r");
	else if (percent >= 30)
		strcat (buf, " has some big nasty wounds and scratches.\n\r");
	else if (percent >= 15)
		strcat (buf, " looks pretty hurt.\n\r");
	else if (percent >= 0)
		strcat (buf, " is in awful condition.\n\r");
	else
		strcat (buf, " is bleeding to death.\n\r");
	
	return buf;
}

/** Function: show_char_to_char_1
  * Descr   : Used when a player looks at another charaters (NPC or PC) to
  *         : display what they are wearing/holding/carrying, and their
  *         : general description.
  * Returns : output only
  * Syntax  : (N/A)
  * Written : v1.0 8/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch, bool showEQ)
{
	char buf[MSL];
	OBJ_DATA *obj;
	int iWear;
	int percent;
	
	if (can_see (victim, ch))
	{
		if (ch == victim)
			act ("$n looks at $mself.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		else
		{
			act ("$n looks at you.", ch, NULL, victim, TO_VICT, POS_RESTING);
			act ("$n looks at $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		}
	}
	
	if (victim->description[0] != '\0')
	{
		send_to_char (victim->description, ch);
	}
	else
	{
		act ("You see nothing special about $M.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
	}
	
	if (MOUNTED (victim))
	{
		sprintf (buf, "%s is riding %s.\n\r", victim->name,
			MOUNTED (victim)->short_descr);
		send_to_char (buf, ch);
	}
	else
		if (RIDDEN (victim))
		{
			sprintf (buf, "%s is being ridden by %s.\n\r",
				victim->short_descr, RIDDEN (victim)->name);
			send_to_char (buf, ch);
		}
		
		if (victim->max_hit > 0)
			percent = (100 * victim->hit) / victim->max_hit;
		else
			percent = -1;
		
		strcpy (buf, PERS (victim, ch));
		
		strcat (buf, condition_string (percent));
		
		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);
		
		if (!showEQ)
			return;
		
		for (iWear = 0; wear_loc_strings[iWear].name != NULL; iWear++)
		{
			if ((obj = get_eq_char (victim, wear_loc_strings[iWear].bit)) != NULL
				&& can_see_obj (ch, obj) && wear_loc_strings[iWear].bit != WEAR_NONE)
			{
				if (iWear == 0)
					chprintf (ch, "\n\r%s is using:\n\r", PERS (victim, ch));
				
				chprintf (ch, "%s %s\n\r",
					wear_loc_strings[iWear].name,
					format_obj_to_char (obj, ch, TRUE));
			}
		}
		
		if (victim != ch
			&& !IS_NPC (ch) && number_percent () < get_skill (ch, skill_lookup("peek")))
		{
			send_to_char ("\n\rYou peek at the inventory:\n\r", ch);
			check_improve (ch, skill_lookup("peek"), TRUE, 4);
			show_list_to_char (victim->carrying, ch, TRUE, TRUE);
		}
		
		return;
}


/** Function: show_char_to_char
  * Descr   : Called by 'do_look' to show all characters in the room
  *         : (except the looker) to the one looking. 
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void show_char_to_char (CHAR_DATA * list, CHAR_DATA * ch)
{
	CHAR_DATA *rch;
	
	for (rch = list; rch != NULL; rch = rch->next_in_room)
	{
		if (rch == ch)
			continue;
		
		if (get_trust (ch) < rch->invis_level)
			continue;
		
		if (can_see (ch, rch))
		{
			show_char_to_char_0 (rch, ch);
		}
		else if (room_is_dark (ch->in_room) && IS_AFFECTED (rch, _AFF_INFRARED))
		{
			send_to_char ("You see glowing red eyes watching YOU!\n\r", ch);
		}
	}
	
	return;
}



bool check_blind (CHAR_DATA * ch)
{
	if (!IS_NPC (ch) && IS_FLAG (ch->states.config, _HOLYLIGHT))
		return TRUE;
	
	if (IS_AFFECTED (ch, _AFF_BLIND))
	{
		send_to_char ("You can't see a thing!\n\r", ch);
		return FALSE;
	}
	
	return TRUE;
}


/* changes your scroll */
_DOFUN (do_scroll)
{
	char arg[MIL];
	char buf[100];
	int lines;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		if (ch->lines == 0)
			send_to_char ("You do not page long messages.\n\r", ch);
		else
		{
			sprintf (buf, "You currently display %d lines per page.\n\r",
				ch->lines + 2);
			send_to_char (buf, ch);
		}
		return;
	}
	
	if (!is_number (arg))
	{
		send_to_char ("You must provide a number.\n\r", ch);
		return;
	}
	
	lines = atoi (arg);
	
	if (lines == 0)
	{
		send_to_char ("Paging disabled.\n\r", ch);
		ch->lines = 0;
		return;
	}
	
	if (lines < 10 || lines > 100)
	{
		send_to_char ("You must provide a reasonable number.\n\r", ch);
		return;
	}
	
	sprintf (buf, "Scroll set to %d lines.\n\r", lines);
	send_to_char (buf, ch);
	ch->lines = lines - 2;
}

/* RT does socials */
_DOFUN (do_socials)
{
	char buf[MSL];
	int iSocial;
	int col;
	BUFFER *buffer;
	
	col = 0;
	buffer = new_buf ();
	
	for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
	{
		sprintf (buf, "%-12s", social_table[iSocial].name);
		add_buf (buffer, buf);
		
		if (++col % 6 == 0)
			add_buf (buffer, "\n\r");
	}
	
	if (col % 6 != 0)
		add_buf (buffer, "\n\r");
	
	page_to_char (buf_string (buffer), ch);
	free_buf (buffer);
	
	return;
}

_DOFUN (do_motd)
{
	BUFFER *output;
	UPDATE_DATA *curr;
	char buf[MSL];
	
	if (motd_list == NULL)	/* Empty MOTD */
	{
		send_to_char ("No updates posted at this time.\n\r", ch);
		return;
	}
	
	output = new_buf ();
	
	add_buf (output, "`WWelcome to `CT`che `CA`cge`W of `CS`ctone`w\n\r");
	add_buf (output, "`cBelow are recent happenings and announcements you should be aware of.\n\r");
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	
	for (curr = motd_list; curr; curr = curr->next)
	{
		if ( curr && !IS_NULLSTR(curr->keyword) && !IS_NULLSTR(curr->information) )
		{
			sprintf(buf, "`N%s `B|`w %s\n\r", curr->keyword, curr->information);
			add_buf(output, buf);
		}
	}
	
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	add_buf (output, "`cPlease read any helps listed above for more information.`w\n\r");
	
	if (!IS_NPC (ch) && ch->desc->connected != CON_PLAYING)
		add_buf (output, "`N-`w=`WHit Enter`w=`N-`w\n\r");
	
	page_to_char (buf_string (output), ch);
	free_buf (output);
	
	return;
}

_DOFUN (do_imotd)
{
	BUFFER *output;
	UPDATE_DATA *curr;
	
	if (imotd_list == NULL)	/* Empty MOTD */
	{
		send_to_char ("No immortal updates posted at this time.\n\r", ch);
		return;
	}
	
	output = new_buf ();
	
	add_buf (output, "`cWelcome, `WAdministrator`c,  to `CT`che `CA`cge`W of `CS`ctone`w\n\r");
	add_buf (output, "`cHere is a list of updates for immortals.\n\r");
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	
	for (curr = imotd_list; curr; curr = curr->next)
		printf_to_buf (output, "`N%s `B|`w %s\n\r", curr->keyword, curr->information);
	
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	add_buf (output, "`cYou know the immortal rules by now, stick to them, or be demoted.`w\n\r");
	
	if (!IS_NPC (ch) && ch->desc->connected != CON_PLAYING)
		add_buf (output, "`N-`w=`WHit Enter`w=`N-`w\n\r");
	
	page_to_char (buf_string (output), ch);
	free_buf (output);
	
	return;
}

_DOFUN (do_jobs)
{
	BUFFER *output;
	UPDATE_DATA *curr;
	
	if (jobs_list == NULL)	/* Empty Job list */
	{
		send_to_char ("No jobs posted at this time.\n\r", ch);
		return;
	}
	
	output = new_buf ();
	
	add_buf (output, "`WT`warm`No`Wn `wGai'`Nd`Wo`wn`c:\n\r");
	add_buf (output, "`cHere is a list of jobs to be done and by who.\n\r");
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	
	for (curr = jobs_list; curr; curr = curr->next)
		printf_to_buf (output, "`N%s `B|`w %s\n\r", curr->keyword, curr->information);
	
	add_buf (output, "`b---------------------------------------------------------------------------\n\r");
	add_buf (output, "`cIf you aren't on this list or have questions, note to us.`w\n\r");
	
	page_to_char (buf_string (output), ch);
	free_buf (output);
	
	return;
}

_DOFUN (do_rules)
{
	do_help (ch, "rules");
}

_DOFUN (do_story)
{
	do_help (ch, "story");
}

void save_wizlist (void)
{
	WIZ_DATA *pwiz;
	FILE *fp;
	bool found = FALSE;
	
	if ((fp = fopen (WIZ_FILE, "w")) == NULL)
	{
		perror (WIZ_FILE);
	}
	
	for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
	{
		found = TRUE;
		
		fprintf (fp, "%s %d %s~ %s~\n", pwiz->name, pwiz->level, pwiz->job, pwiz->email);
	}
	
	fclose (fp);
	
	if (!found)
		unlink (WIZ_FILE);
}

void load_wizlist (void)
{
	FILE *fp;
	WIZ_DATA *wiz_last;
	
	if ((fp = fopen (WIZ_FILE, "r")) == NULL)
	{
		return;
	}
	
	wiz_last = NULL;
	
	for (;;)
	{
		WIZ_DATA *pwiz;
		if (feof (fp))
		{
			fclose (fp);
			return;
		}
		
		pwiz = new_wiz ();
		
		pwiz->name = str_dup (fread_word (fp));
		pwiz->level = fread_number (fp);
		pwiz->job = str_dup (fread_string (fp));
		pwiz->email = str_dup (fread_string (fp));
		fread_to_eol (fp);
		
		if (wiz_list == NULL)
			wiz_list = pwiz;
		else
			wiz_last->next = pwiz;
		wiz_last = pwiz;
	}
}

_DOFUN (do_wizlist)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char arg4[MIL];
	char arg5[MIL];
	char buf[MSL];
	BUFFER *buffer;
	int level;
	WIZ_DATA *pwiz;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = One_argument (argument, arg3);
	argument = One_argument (argument, arg4);
	argument = One_argument (argument, arg5);
	
	if ((arg1[0] != '\0') /* && is_granted_name(ch,"update") */ )
	{
		if (!str_prefix (arg1, "add"))
		{
			if (!is_number (arg2) || (arg3[0] == '\0') || (arg4[0] == '\0')
				|| (arg5[0] == '\0'))
			{
				do_update (ch, "");
				return;
			}
			level = atoi (arg2);
			change_wizlist (ch, TRUE, level, arg3, arg4, arg5);
			return;
		}
		if (!str_prefix (arg1, "delete"))
		{
			if (arg2[0] == '\0')
			{
				send_to_char ("Syntax: wizlist delete <name>\n\r", ch);
				return;
			}
			change_wizlist (ch, FALSE, 0, arg2, arg4, arg5);
			return;
		}
		if (!str_prefix (arg1, "modify"))
		{
			if (IS_NULLSTR (arg2) || IS_NULLSTR (arg3) || IS_NULLSTR (arg4))
			{
				send_to_char ("Syntax: wizlist modify <name> <field> <new value>\n\r",
					ch);
				return;
			}
			modify_wizlist (ch, arg2, arg3, arg4);
			return;
		}
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("       wizlist delete <name>\n\r", ch);
		send_to_char ("       wizlist add <level> <name> <job> <email>\n\r", ch);
		send_to_char ("       wizlist modify <name> <field> <new value>\n\r", ch);
		return;
	}
	
	if (wiz_list == NULL)
	{
		send_to_char ("No immortals listed at this time.\n\r", ch);
		return;
	}
	buffer = new_buf ();
	
	sprintf (buf,
		"              `cThe Administrators of `CT`che `CA`cge`W of `CS`ctone`w\n\r");
	add_buf (buffer, buf);
	sprintf (buf,
		"`N----------`w-----------`W-----------------------------------`w-----------`N-----------`w\n\r");
	add_buf (buffer, buf);
	for (level = IMPLEMENTOR; level > HERO; level--)
	{
		for (pwiz = wiz_list; pwiz != NULL; pwiz = pwiz->next)
		{
			if (pwiz->level == level)
			{
				sprintf (buf,
					"`c-`C=`W%3d`C=`c-`N %-15s `C>`c %-22s `C<`c %-30s`w\n\r",
					pwiz->level, 
					pwiz->name, 
					pwiz->job, 
					pwiz->email);
				
				add_buf (buffer, buf);
			}
		}
	}
	sprintf (buf,
		"`N----------`w-----------`W-----------------------------------`w-----------`N-----------`w\n\r");
	add_buf (buffer, buf);
	
	page_to_char (buf_string (buffer), ch);
	free_buf (buffer);
	return;
}

void update_wizlist (CHAR_DATA * ch, int level, char *job, char *email)
{
	WIZ_DATA *prev;
	WIZ_DATA *curr;
	
	if (IS_NPC (ch))
	{
		return;
	}
	prev = NULL;
	for (curr = wiz_list; curr != NULL; prev = curr, curr = curr->next)
	{
		if (!str_cmp (ch->name, curr->name))
		{
			if (prev == NULL)
				wiz_list = wiz_list->next;
			else
				prev->next = curr->next;
			
			free_wiz (curr);
			save_wizlist ();
		}
	}
	if (level <= HERO)
	{
		return;
	}
	curr = new_wiz ();
	curr->name = str_dup (ch->name);
	curr->level = level;
	curr->job = job;
	curr->email = email;
	curr->next = wiz_list;
	wiz_list = curr;
	save_wizlist ();
	return;
}

void modify_wizlist (CHAR_DATA * ch, char *arg1, char *arg2, char *arg3)
{
	WIZ_DATA *curr;
	
	if (IS_NULLSTR (arg1) || IS_NULLSTR (arg2) || IS_NULLSTR (arg3))
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char
			("		  wizlist modify <name> <field> <new value>\n\r", ch);
		return;
	}
	
	for (curr = wiz_list;; curr = curr->next)
	{
		if (curr == NULL)
		{
			send_to_char ("No such entry.\n\r", ch);
			return;
		}
		
		if (!str_cmp (arg1, curr->name))
			break;
		else
			continue;
	}
	
	if (!str_cmp (arg2, "name"))
	{
		free_string (curr->name);
		curr->name = str_dup (arg3);
	}
	else if (!str_cmp (arg2, "level") && is_number (arg3))
	{
		curr->level = atoi (arg3);
	}
	else if (!str_cmp (arg2, "job"))
	{
		free_string (curr->job);
		curr->job = str_dup (arg3);
	}
	else if (!str_cmp (arg2, "email"))
	{
		free_string (curr->email);
		curr->email = str_dup (arg3);
	}
	else
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("		  wizlist modify <name> <field> <new value>\n\r", ch);
		return;
	}
	
	save_wizlist ();
	return;
}

void change_wizlist (CHAR_DATA * ch, bool add, int level, char *argument,char *job, char *email)
{
	char arg[MIL];
	WIZ_DATA *prev;
	WIZ_DATA *curr;
	
	One_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		if (!add)
			send_to_char ("\twizlist delete <name>\n\r", ch);
		else
			send_to_char ("\twizlist add <level> <name> <job> <email>\n\r", ch);
		return;
	}
	
	if (add)
	{
		if ((level <= HERO) || (level > MAX_LEVEL))
		{
			send_to_char ("Syntax:\n\r", ch);
			send_to_char ("\twizlist add <level> <name> <job> <email>\n\r", ch);
			return;
		}
	}
	if (!add)
	{
		prev = NULL;
		for (curr = wiz_list; curr != NULL; prev = curr, curr = curr->next)
		{
			if (!str_cmp (arg, curr->name))
			{
				if (prev == NULL)
					wiz_list = wiz_list->next;
				else
					prev->next = curr->next;
				
				free_wiz (curr);
				save_wizlist ();
			}
		}
	}
	else
	{
		curr = new_wiz ();
		curr->name = str_dup (arg);
		curr->level = level;
		curr->job = str_dup (job);
		curr->email = str_dup (email);
		curr->next = wiz_list;
		wiz_list = curr;
		save_wizlist ();
	}
	return;
}


_DOFUN (do_gllist)
{
	do_help (ch, "gllist");
}

/** Function: do_autolist
  * Descr   : Displays, or toggles an 'auto' command for the player.
  * Returns : void
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Russ Taylor, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_autolist)
{
	/* lists most player flags */
	if (IS_NPC (ch))
		return;
	
	send_to_char ("   action     status\n\r", ch);
	send_to_char ("---------------------\n\r", ch);
	
	send_to_char ("autoassist     ", ch);
	if (IS_FLAG (ch->states.config, _AUTOASSIST))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("autoexit       ", ch);
	if (IS_FLAG (ch->states.config, _AUTOEXIT))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("autogold       ", ch);
	if (IS_FLAG (ch->states.config, _AUTOGOLD))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("autoloot       ", ch);
	if (IS_FLAG (ch->states.config, _AUTOLOOT))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("autosac        ", ch);
	if (IS_FLAG (ch->states.config, _AUTOSAC))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("autosplit      ", ch);
	if (IS_FLAG (ch->states.config, _AUTOSPLIT))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("compact mode   ", ch);
	if (IS_FLAG (ch->states.config, _COMPACT))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("prompt         ", ch);
	if (IS_FLAG (ch->states.config, _PROMPT))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("combine items  ", ch);
	if (IS_FLAG (ch->states.config, _COMBINE))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	send_to_char ("mudFTP         ", ch);
	if (IS_FLAG (ch->states.config, _MUDFTP))
		send_to_char ("ON\n\r", ch);
	else
		send_to_char ("OFF\n\r", ch);
	
	if (!IS_FLAG (ch->states.player, _CANLOOT))
		send_to_char ("Your corpse is safe from thieves.\n\r", ch);
	else
		send_to_char ("Your corpse may be looted.\n\r", ch);
	
	if (IS_FLAG (ch->states.config, _NOFOLLOW))
		send_to_char ("You do not welcome followers.\n\r", ch);
	else
		send_to_char ("You accept followers.\n\r", ch);
}

void do_mudftp (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _MUDFTP))
	{
		send_to_char ("You will now use the normal editor to "
			"edit strings.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _MUDFTP);
	}
	else
	{
		send_to_char ("You will now use the mudFTP protocol to "
			"edit strings.\n\r", ch);
		SET_FLAG (ch->states.config, _MUDFTP);
	}
}


_DOFUN (do_autoassist)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOASSIST))
	{
		send_to_char ("Autoassist removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOASSIST);
	}
	else
	{
		send_to_char ("You will now assist when needed.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOASSIST);
	}
}

_DOFUN (do_autoexit)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOEXIT))
	{
		send_to_char ("Exits will no longer be displayed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOEXIT);
	}
	else
	{
		send_to_char ("Exits will now be displayed.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOEXIT);
	}
}

_DOFUN (do_autogold)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOGOLD))
	{
		send_to_char ("Autogold removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOGOLD);
	}
	else
	{
		send_to_char ("Automatic gold looting set.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOGOLD);
	}
}

_DOFUN (do_autoloot)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOLOOT))
	{
		send_to_char ("Autolooting removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOLOOT);
	}
	else
	{
		send_to_char ("Automatic corpse looting set.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOLOOT);
	}
}

_DOFUN (do_autosac)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOSAC))
	{
		send_to_char ("Autosacrificing removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOSAC);
	}
	else
	{
		send_to_char ("Automatic corpse sacrificing set.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOSAC);
	}
}

_DOFUN (do_autosplit)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _AUTOSPLIT))
	{
		send_to_char ("Autosplitting removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AUTOSPLIT);
	}
	else
	{
		send_to_char ("Automatic gold splitting set.\n\r", ch);
		SET_FLAG (ch->states.config, _AUTOSPLIT);
	}
}

_DOFUN (do_brief)
{
	if (IS_FLAG (ch->states.config, _BRIEF))
	{
		send_to_char ("Full descriptions activated.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _BRIEF);
	}
	else
	{
		send_to_char ("Short descriptions activated.\n\r", ch);
		SET_FLAG (ch->states.config, _BRIEF);
	}
}

_DOFUN (do_compact)
{
	if (IS_FLAG (ch->states.config, _COMPACT))
	{
		send_to_char ("Compact mode removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _COMPACT);
	}
	else
	{
		send_to_char ("Compact mode set.\n\r", ch);
		SET_FLAG (ch->states.config, _COMPACT);
	}
}

_DOFUN (do_show)
{
	if (IS_FLAG (ch->states.config, _SHOW_AFFECTS))
	{
		send_to_char ("Affects will no longer be shown in score.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _SHOW_AFFECTS);
	}
	else
	{
		send_to_char ("Affects will now be shown in score.\n\r", ch);
		SET_FLAG (ch->states.config, _SHOW_AFFECTS);
	}
}

_DOFUN (do_prompt)
{
	char buf[MSL];
	
	if (argument[0] == '\0')
	{
		if (IS_FLAG (ch->states.config, _PROMPT))
		{
			send_to_char ("You will no longer see prompts.\n\r", ch);
			REMOVE_FLAG (ch->states.config, _PROMPT);
		}
		else
		{
			send_to_char ("You will now see prompts.\n\r", ch);
			SET_FLAG (ch->states.config, _PROMPT);
		}
		return;
	}
	
	if (!strcmp (argument, "all"))
		strcpy (buf, "<%hhp %ma %vmv> ");
	else
	{
		if (strlen (argument) > 50)
			argument[50] = '\0';
		
		strcpy (buf, argument);
		smash_tilde (buf);
		
		if (str_suffix ("%c", buf))
			strcat (buf, " ");
		
	}
	
	free_string (ch->prompt);
	ch->prompt = str_dup (buf);
	sprintf (buf, "Prompt set to %s\n\r", ch->prompt);
	send_to_char (buf, ch);
	return;
}

_DOFUN (do_combine)
{
	if (IS_FLAG (ch->states.config, _COMBINE))
	{
		send_to_char ("Long inventory selected.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _COMBINE);
	}
	else
	{
		send_to_char ("Combined inventory selected.\n\r", ch);
		SET_FLAG (ch->states.config, _COMBINE);
	}
}

_DOFUN (do_fixnote)
{
	if (!IS_FLAG (ch->states.config, _ECHO))
	{
		send_to_char ("Lines will be echoed to you while writing a note.\n\r",
			ch);
		SET_FLAG (ch->states.config, _ECHO);
	}
	else
	{
		send_to_char
			("Lines will nolonger be echoed to you while writing notes.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _ECHO);
	}
}


_DOFUN (do_nofollow)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _NOFOLLOW))
	{
		send_to_char ("You now accept followers.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _NOFOLLOW);
	}
	else
	{
		send_to_char ("You no longer accept followers.\n\r", ch);
		SET_FLAG (ch->states.config, _NOFOLLOW);
		die_follower (ch);
	}
}

_DOFUN (do_glance)
{
	CHAR_DATA *victim = NULL;
	
	if (ch->desc == NULL)
		return;
	
	if (ch->position < POS_SLEEPING)
	{
		send_to_char ("You can't see anything but stars!\n\r", ch);
		return;
	}
	
	if (ch->position == POS_SLEEPING)
	{
		send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
		return;
	}
	
	if (!check_blind (ch))
		return;
	
	if ((victim = get_char_room (ch, argument)) != NULL)
	{
		show_char_to_char_1 (victim, ch, FALSE);
		return;
	}
	
	send_to_char ("Glance at who?\n\r", ch);
}

_DOFUN (do_look)
{
	char buf[MSL];
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	char *pdesc;
	int door;
	int number, count;
	
	if (ch->desc == NULL)
		return;
	
	if (ch->position < POS_SLEEPING)
	{
		send_to_char ("You can't see anything but stars!\n\r", ch);
		return;
	}
	
	if (ch->position == POS_SLEEPING)
	{
		send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
		return;
	}
	
	if (!check_blind (ch))
		return;
	
	if (!IS_NPC (ch)
		&& !IS_FLAG (ch->states.config, _HOLYLIGHT)
		&& !IS_AFFECTED (ch, _AFF_DARK_VISION) && room_is_dark (ch->in_room))
	{
		send_to_char ("It is pitch black ... \n\r", ch);
		show_char_to_char (ch->in_room->people, ch);
		return;
	}
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	number = number_argument (arg1, arg3);
	count = 0;
	
	if (arg1[0] == '\0' || !str_cmp (arg1, "auto"))
	{
		/* 'look' or 'look auto' */
		send_to_char (TITLECOLOR, ch);
		send_to_char (ch->in_room->name, ch);
		send_to_char ("`w", ch);
		
		if (
			(IS_IMMORTAL (ch)
			&& (IS_NPC (ch) || IS_FLAG (ch->states.config, _HOLYLIGHT)))
			|| IS_BUILDER (ch, ch->in_room->area))
		{
			sprintf (buf, " [`cRoom %ld`w]", ch->in_room->vnum);
			send_to_char (buf, ch);
		}
		
		send_to_char ("\n\r", ch);
		
		if (arg1[0] == '\0'
			|| (!IS_NPC (ch) && !IS_FLAG (ch->states.config, _BRIEF)))
		{
			send_to_char ("  ", ch);
			send_to_char (ch->in_room->description, ch);
		}
		
		if (!IS_NPC (ch) && IS_FLAG (ch->states.config, _AUTOEXIT))
		{
			send_to_char ("\n\r", ch);
			do_exits (ch, "auto");
		}
		
		if ( ch->in_room->contents != NULL )
		  show_list_to_char (ch->in_room->contents, ch, FALSE, FALSE);
		
		if ( ch->in_room->people != NULL )
		  show_char_to_char (ch->in_room->people, ch);
		
		return;
	}
	
	if (!str_prefix (arg1, "in") || !str_cmp (arg1, "on"))
	{
		/* 'look in' */
		if (arg2[0] == '\0')
		{
			send_to_char ("Look in what?\n\r", ch);
			return;
		}
		
		if ((obj = get_obj_here (ch, arg2)) == NULL)
		{
			send_to_char ("You do not see that here.\n\r", ch);
			return;
		}
		
		switch (obj->item_type)
		{
		default:
			send_to_char ("That is not a container.\n\r", ch);
			break;
			
		case ITEM_QUIVER:
			if (obj->value[0] <= 0)
			{
				send_to_char ("The quiver is out of arrows.\n\r", ch);
				break;
			}
			
			if (obj->value[0] == 1)
			{
				send_to_char ("The quiver has 1 arrow remaining in it.\n\r", ch);
				break;
			}
			
			if (obj->value[0] > 1)
				chprintf (ch, "The quiver holds %d arrows.\n\r", obj->value[0]);
			break;
			
		case ITEM_DRINK_CON:
			if (obj->value[1] <= 0)
			{
				send_to_char ("It is empty.\n\r", ch);
				break;
			}
			
			sprintf (buf, "It's %sfilled with  a %s liquid.\n\r",
				obj->value[1] < obj->value[0] / 4
				? "less than half-" :
			obj->value[1] < 3 * obj->value[0] / 4
				? "about half-" : "more than half-",
				liq_table[obj->value[2]].liq_color);
			
			send_to_char (buf, ch);
			break;
			
		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			if (IS_SET (obj->value[1], CONT_CLOSED))
			{
				send_to_char ("It is closed.\n\r", ch);
				break;
			}
			
			act ("$p holds:", ch, obj, NULL, TO_CHAR, POS_RESTING);
			show_list_to_char (obj->contains, ch, TRUE, TRUE);
			break;
		}
		return;
	}
	
	if ((victim = get_char_room (ch, arg1)) != NULL)
	{
		show_char_to_char_1 (victim, ch, TRUE);
		return;
	}
	
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (can_see_obj (ch, obj))
		{	/* player can see object */
			pdesc = get_extra_descr (arg3, obj->extra_descr);
			if (pdesc != NULL)
			{
				if (++count == number)
				{
					send_to_char (pdesc, ch);
					return;
				}
				else
					continue;
			}
			pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
			if (pdesc != NULL)
			{
				if (++count == number)
				{
					send_to_char (pdesc, ch);
					return;
				}
				else
					continue;
			}
			if (is_name (arg3, obj->name))
				if (++count == number)
				{
					send_to_char (obj->description, ch);
					send_to_char ("\n\r", ch);
					return;
				}
		}
	}
	
	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
	{
		if (can_see_obj (ch, obj))
		{
			if (obj->item_type == ITEM_GATEMARKER)
			{
				if (!check_read_stone (ch))
				{
					send_to_char ("You were not able to make anything out.\n\r", ch);
					return;
				}
			}
			
			pdesc = get_extra_descr (arg3, obj->extra_descr);
			if (pdesc != NULL)
				if (++count == number)
				{
					send_to_char (pdesc, ch);
					return;
				}
				
				pdesc = get_extra_descr (arg3, obj->pIndexData->extra_descr);
				if (pdesc != NULL)
					if (++count == number)
					{
						send_to_char (pdesc, ch);
						return;
					}
					
					if (is_name (arg3, obj->name))
						if (++count == number)
						{
							send_to_char (obj->description, ch);
							send_to_char ("\n\r", ch);
							return;
						}
		}
	}
	
	pdesc = get_extra_descr (arg3, ch->in_room->extra_descr);
	if (pdesc != NULL)
	{
		if (++count == number)
		{
			send_to_char (pdesc, ch);
			return;
		}
	}
	
	if (count > 0 && count != number)
	{
		if (count == 1)
			sprintf (buf, "You only see one %s here.\n\r", arg3);
		else
			sprintf (buf, "You only see %d of those here.\n\r", count);
		
		send_to_char (buf, ch);
		return;
	}
	
	if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
		door = 0;
	else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
		door = 1;
	else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
		door = 2;
	else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
		door = 3;
	else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
		door = 4;
	else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
		door = 5;
	else
	{
		send_to_char ("You do not see that here.\n\r", ch);
		return;
	}
	
	/* 'look direction' */
	if ((pexit = ch->in_room->exit[door]) == NULL)
	{
		send_to_char ("Nothing special there.\n\r", ch);
		return;
	}
	
	if (pexit->description != NULL && pexit->description[0] != '\0')
	{
		send_to_char (pexit->description, ch);
	}
	else
		send_to_char ("Nothing special there.\n\r", ch);
	
	
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	{
		bool bFound = FALSE;
		
		send_to_char("Beyond the door, you see...\n\r", ch);
		
		
		if ( pexit->u1.to_room->people != NULL )
		{
			show_char_to_char(pexit->u1.to_room->people, ch);
			bFound = TRUE;
		}
		
		if ( pexit->u1.to_room->contents != NULL )
		{
			show_list_to_char(pexit->u1.to_room->contents, ch, TRUE, FALSE);
			bFound = TRUE;
		}
		
		
		if ( !bFound )
			send_to_char("nothing!\n\r", ch);
	}
	
	
	if ( !IS_NULLSTR(pexit->keyword) && pexit->keyword[0] != ' ')
	{
		if (!IS_SET (pexit->exit_info, EX_SECRET))
		{
			if (IS_SET (pexit->exit_info, EX_CLOSED))
			{
				act ("The $d is closed.",
					ch, NULL, pexit->keyword, TO_CHAR, POS_RESTING);
			}
			else if (IS_SET (pexit->exit_info, EX_ISDOOR))
			{
				act ("The $d is open.",
					ch, NULL, pexit->keyword, TO_CHAR, POS_RESTING);
			}
		}
	}
	
	return;
}

/* RT added back for the hell of it */
_DOFUN (do_read)
{
	do_look (ch, argument);
}

_DOFUN (do_examine)
{
	char buf[MSL];
	char arg[MIL];
	OBJ_DATA *obj;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Examine what?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "box"))
	{
		if (ch->in_room->vnum != ROOM_VNUM_RENTBOX)
		{
			send_to_char ("Do you see a box here?\n\r", ch);
			return;
		}
		
		send_to_char ("You got following in your box:\n\r", ch);
		show_list_to_char (ch->pcdata->box, ch, TRUE, TRUE);
		return;
	}
	
	
	do_look (ch, arg);
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		switch (obj->item_type)
		{
		default:
			break;
			
		case ITEM_JUKEBOX:
			do_play (ch, "list");
			break;
			
		case ITEM_MONEY:
			if (obj->value[0] == 0)
			{
				if (obj->value[1] == 0)
					sprintf (buf, "Odd...there's no coins in the pile.\n\r");
				else if (obj->value[1] == 1)
					sprintf (buf, "Wow. One gold coin.\n\r");
				else
					sprintf (buf, "There are %d gold coins in the pile.\n\r",
					obj->value[1]);
			}
			else if (obj->value[1] == 0)
			{
				if (obj->value[0] == 1)
					sprintf (buf, "Wow. One silver coin.\n\r");
				else
					sprintf (buf, "There are %d silver coins in the pile.\n\r",
					obj->value[0]);
			}
			else
				sprintf (buf,
				"There are %d gold and %d silver coins in the pile.\n\r",
				obj->value[1], obj->value[0]);
			send_to_char (buf, ch);
			break;
			
		case ITEM_DRINK_CON:
		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			sprintf (buf, "in %s", argument);
			do_look (ch, buf);
		}
	}
	
	return;
}



/** Function: do_exits
  * Descr   : Display room exits to player, according to their state/and
  *         : any exit flags.
  * Returns : output only
  * Syntax  : (N/A)
  * Written : v1.0 8/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_exits)
{
	extern char *const dir_name[];
	char buf[MSL];
	EXIT_DATA *pexit;
	bool found;
	bool fAuto;
	int door;
	
	fAuto = !str_cmp (argument, "auto");
	
	if (!check_blind (ch))
		return;
	
	if (fAuto)
		sprintf (buf, "[%sExits:", AEXITC);
	else if (IS_IMMORTAL (ch))
		sprintf (buf, "Obvious exits from room %ld:\n\r", ch->in_room->vnum);
	else
		sprintf (buf, "Obvious exits:\n\r");
	
	found = FALSE;
	for (door = 0; door <= 5; door++)
	{
		if ((pexit = ch->in_room->exit[door]) != NULL
			&& pexit->u1.to_room != NULL
			&& !IS_SET (pexit->exit_info, EX_SECRET)
			&& can_see_room (ch, pexit->u1.to_room))
		{
			found = TRUE;
			if (fAuto)
			{
				strcat (buf, IS_SET (pexit->exit_info, EX_CLOSED) ? " (" : " ");
				strcat (buf, dir_name[door]);
				if (IS_SET (pexit->exit_info, EX_CLOSED))
					strcat (buf, ")");
			}
			else
			{
				sprintf (buf + strlen (buf), "%-5s - %s",
					capitalize (dir_name[door]),
					room_is_dark (pexit->u1.to_room)
					? "Too dark to tell" : pexit->u1.to_room->name);
				if (IS_IMMORTAL (ch))
					sprintf (buf + strlen (buf),
					" (room %ld)\n\r", pexit->u1.to_room->vnum);
				else
					sprintf (buf + strlen (buf), "\n\r");
			}
		}
	}
	
	if (!found)
		strcat (buf, fAuto ? " none" : "None.\n\r");
	
	if (fAuto)
		strcat (buf, "`w]\n\r");
	
	send_to_char (buf, ch);
	return;
}

_DOFUN (do_worth)
{
	char buf[MSL];
	
	if (IS_NPC (ch))
	{
		sprintf (buf, "You have %ld gold and %ld silver.\n\r",
			ch->gold, ch->silver);
		send_to_char (buf, ch);
		return;
	}
	
	sprintf (buf,
		"You have %ld gold, %ld silver, and %ld experience (%ld exp to level).\n\r",
		ch->gold, ch->silver, ch->exp,
		(ch->level + 1) * exp_per_level (ch,
		ch->pcdata->points) - ch->exp);
	
	send_to_char (buf, ch);
	
	return;
}

/** Function: do_score
  * Descr   : Displays a PC's score to them, as well as some important
  *         : information not available elsewhere to the PC.
  * Returns : void
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_score)
{
	char buf[MSL];
	char cond[MSL];
	char posit[MSL];
	char size[MSL];
	
	if (MOUNTED (ch))
		sprintf (posit, "Mounted");
	
	else
		switch (ch->position)
    {
    case POS_DEAD:
		sprintf (posit, "DEAD!!");
		break;
    case POS_MORTAL:
		sprintf (posit, "mortally wounded");
		break;
    case POS_INCAP:
		sprintf (posit, "incapacitated");
		break;
    case POS_STUNNED:
		sprintf (posit, "stunned");
		break;
    case POS_SLEEPING:
		sprintf (posit, "sleeping");
		break;
    case POS_RESTING:
		sprintf (posit, "resting");
		break;
    case POS_SITTING:
		sprintf (posit, "sitting");
		break;
    case POS_STANDING:
		sprintf (posit, "standing");
		break;
    case POS_FIGHTING:
		sprintf (posit, "fighting");
		break;
    default:
		break;
    }
	
	if (ch->alignment > 900)
		sprintf (buf, "%s", "angelic");
	else if (ch->alignment > 700)
		sprintf (buf, "%s", "saintly");
	else if (ch->alignment > 350)
		sprintf (buf, "%s", "good");
	else if (ch->alignment > 100)
		sprintf (buf, "%s", "kind");
	else if (ch->alignment > -100)
		sprintf (buf, "%s", "neutral");
	else if (ch->alignment > -350)
		sprintf (buf, "%s", "mean");
	else if (ch->alignment > -700)
		sprintf (buf, "%s", "evil");
	else if (ch->alignment > -900)
		sprintf (buf, "%s", "demonic");
	else
		sprintf (buf, "%s", "satanic");
	
	
	if (!IS_NPC (ch) && ch->pcdata->condition[_DRUNK] > 10)
		sprintf (cond, "Drunk");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_HUNGER] <= -50)
    sprintf (cond, "Starving");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_THIRST] <= -50)
    sprintf (cond, "Parched");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_THIRST] <= 0)
		sprintf (cond, "Thirsty");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_HUNGER] <= 0)
		sprintf (cond, "Hungry");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_HUNGER] +
						   ch->pcdata->condition[_THIRST] > 74)
    sprintf (cond, "Full");
  else if (!IS_NPC (ch) && ch->pcdata->condition[_HUNGER] +
						   ch->pcdata->condition[_THIRST] > 99)
    sprintf (cond, "Stuffed");
	else
		sprintf (cond, "Not hungry");
	
	chprintf (ch, "\n\r\n\r"
		"  `G*`B-----------------------------------------------------------------------`G*\n\r"
		"  `B|`w Name: %-13.13s  %23s, %-23s `B|\n\r"
		"  |`w Title: %-62.62s `B|\n\r",
		ch->name,
		IS_NPC(ch) ? "" : ch->pcdata->city,
		IS_NPC(ch) ? "" : ch->pcdata->nation,
		IS_NPC (ch) ? ch->short_descr : strip_codes (ch->pcdata->title));

	if ( IS_FLAG(pMud.config, _ALLOW_RDESC) )
	  chprintf (ch, "  |`w Room Desc: %-58.58s `B|\n\r",
		IS_NPC (ch) ? "(none)" : strip_codes (ch->pcdata->r_desc));

		if ( is_clan(ch) )
			chprintf(ch, "  |`w A %-19.19s of the: %-38.38s  `B|\n\r",
			strip_codes (clan_table[ch->clan].rank[ch->rank].rankname),
			strip_codes (clan_table[ch->clan].who_name));


		chprintf (ch,
		"  |`w A level %-2d %-9.9s  Required experience needed to level is %-8ld `B|\n\r"
		"  |`w You are %-3d years old (%-4d hrs). Your alignment is %-7.7s (%-5d)   `B|\n\r",
		ch->level,
		class_table[ch->c_class].name,  
		(IS_NPC (ch) || ch->level >= LEVEL_HERO) ? -1
		  : ((ch->level + 1) * exp_per_level (ch, ch->pcdata->points) - ch->exp),
								 get_age (ch),
								 (ch->played + (int) (current_time - ch->logon)) / 3600, buf,
								 ch->alignment);

	if (IS_IMMORTAL (ch) && !IS_SWITCHED (ch))
	{
		chprintf (ch,
			"  |`w Holyight is %-3s. WizInvis Level: %-3d"
			" Incognitio Level: %-3d            `B|\n\r",
			IS_FLAG (ch->states.config, _HOLYLIGHT) ? "On" : "Off",
			ch->invis_level, ch->incog_level);
		
		chprintf (ch, "  |`w Quest Points %-4d                        "
			"                             `B|\n\r",
			ch->pcdata->quest_points);
	}
	if (get_trust (ch) != ch->level)
		chprintf (ch,
		"  |`w You are trusted at level %-2d, please"
		" do not abuse this priveledge      `B|\n\r",
		get_trust (ch));
	
	chprintf (ch,
		"  |`w a %-7s %-9s  You have %-3d Training and %-3d practice"
		" sessions  `B|\n\r",
		ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
		IS_NPC (ch) ? "Mobile" : class_table[ch->c_class].name,
		ch->train, ch->practice);
	
	if (!IS_NPC (ch) && ch->pcdata->bounty > 0)
		chprintf (ch,
		"  |`w -- You have a bounty of %-7ld that has been placed on your"
		" head! -- `B|\n\r", ch->pcdata->bounty);
	
	chprintf (ch,
		"  |-----------------------------------------------------------------------|\n\r"
		"  |`w HP  %5d|%-5d  AC Pierce %-6d Gold    %-9ld Max Weight %-7d`B|\n\r"
		"  |`w EN  %5d|%-5d     Bash   %-6d in Bank %-9ld Cur Weight %-7ld`B|\n\r"
		"  |`w H/D %5d|%-5d     Slash  %-6d Silver  %-9ld Max Items  %-7d`B|\n\r"
		"  |`w Peace: [%-3.3s]        Weaves %-6d Exp     %-9ld Cur Items  %-7d`B|\n\r"
		"  |-----------------------------------------------------------------------|\n\r",
		ch->hit,
		ch->max_hit,
		GET_AC (ch, AC_PIERCE),
		ch->gold,
		can_carry_w (ch) / 10,
		ch->move,
		ch->max_move,
		GET_AC (ch, AC_BASH),
		IS_NPC (ch) ? 0 : ch->pcdata->balance,
		(get_carry_weight (ch) / 10),
		GET_HITROLL (ch),
		GET_DAMROLL (ch),
		GET_AC (ch, AC_SLASH),
		ch->silver,
		can_carry_n (ch),
		IS_NPC (ch) ? "Off" : (ch->pcdata->peaceful) ? "On" : "Off",
		GET_AC (ch, AC_EXOTIC),
		ch->exp,
		ch->carry_number);

	if (can_channel (ch))
	  chprintf (ch,
		"  |`w Ability %2d|%2d   `CAir`w %1d|%1d  `NEarth`w %1d|%1d  `BWater`w %1d|%1d"
		"  `RFire`w %1d |%1d  `WSpirit`w %1d|%1d  `B|\n\r"
		"  |-----------------------------------------------------------------------|\n\r",
		TAU (ch),
		OPR (ch),
		ch->flow_str.current[AIR], ch->flow_str.perm[AIR],
		ch->flow_str.current[EARTH], ch->flow_str.perm[EARTH],
		ch->flow_str.current[WATER], ch->flow_str.perm[WATER],
		ch->flow_str.current[FIRE], ch->flow_str.perm[FIRE],
		ch->flow_str.current[SPIRIT], ch->flow_str.perm[SPIRIT]);

	 switch (ch->size)
	 {
	 case 1:
		 sprintf (size, "Small");
		 break;
	 case 2:
		 sprintf (size, "Medium");
		 break;
	 case 3:
		 sprintf (size, "Large");
		 break;
	 }

	chprintf (ch,
		"  |`w Str: %2d Int: %2d Agl: %2d Dex: %2d Con:"
		" %2d Sen: %2d     `B|\n\r"
		"  |-----------------------------------------------------------------------|\n\r"
		"  |`w Notoriety:  Fame[%3.3f] Infamy[%3.3f]   Size: %-10.11s              `B|\n\r"
		"  |-----------------------------------------------------------------------|\n\r"
		"  |`w You flee at: %-4d hp. You're %-10s and are %-13.13s         `B|`w\n\r"
		"  `G*`B-----------------------------------------------------------------------"
		"`G*`w\n\r\n\r",
		ch->perm_stat[STAT_STR],
		ch->perm_stat[STAT_INT],
		ch->perm_stat[STAT_AGL],
		ch->perm_stat[STAT_DEX],
		ch->perm_stat[STAT_CON],
		ch->perm_stat[STAT_SEN],
		(IS_NPC(ch)) ? 0.0 : ch->pcdata->notoriety[FAME],
		(IS_NPC(ch)) ? 0.0 : ch->pcdata->notoriety[INFAMY],
		size,
		ch->wimpy, cond, posit);
	
	if (IS_FLAG (ch->states.config, _SHOW_AFFECTS))
		do_affects (ch, "");
	
	return;
}

_DOFUN (do_affects)
{
	AFFECT_DATA *paf, *paf_last = NULL;
	char buf[MSL];
	
	if (ch->affected != NULL)
	{
		send_to_char ("You are affected by the following:\n\r", ch);
		for (paf = ch->affected; paf != NULL; paf = paf->next)
		{
			if (paf_last != NULL && paf->type == paf_last->type)
				if (ch->level >= 20)
					sprintf (buf, "                    ");
				else
					continue;
				else
					sprintf (buf, "%-20.20s", skill_table[paf->type].name);
				
				send_to_char (buf, ch);
				
				if (ch->level >= 20)
				{
					chprintf (ch,
						": modifies %s by %d ",
                        flag_string(apply_flags, paf->location),
						paf->modifier);
					
					if (paf->duration == -1)
						sprintf (buf, "permanently");
					else
						sprintf (buf, "for %d hours", paf->duration);
					send_to_char (buf, ch);
					
				}
				
				send_to_char ("\n\r", ch);
				paf_last = paf;
		}
	}
	else
		send_to_char ("You are not affected by any weaves.\n\r", ch);
	
	return;
}


_DOFUN (do_talents)
{
	int i;
	bool tFound = FALSE;
	
	if (IS_NPC (ch))
	{
		send_to_char ("NPC's dont have talents.\n\r", ch);
		return;
	}
	
	for (i = 0; i < MAX_TALENTS; i++)
		if (ch->pcdata->talents[i])
		{
			if (!tFound)
				send_to_char ("You have the following Talents/Specialties:\n\r"
				"-------------------------------------------\n\r", ch);
			
			tFound = TRUE;
			chprintf (ch, "%-20.20s [%s]\n\r",
				talent_table[i].name,
				IS_FLAG (talent_table[i].flags, _IS_TALENT) ?
				"Talent" : "Specialty");
		}
		
		if (!tFound)
			send_to_char ("None found.\n\r", ch);
		
		if (IS_FLAG (ch->states.player, _ACT_BLADEMASTER))
			send_to_char ("You've reached Blademaster Status.\n\r", ch);
}


char *const day_name[] = {
	"the Wheel", "the Dragon", "the Pattern", "Reflection", "the High King",
		"Understanding", "the Flame", "Effort", "the Serpent", "the Light"
};

char *const month_name[] = {
	"Taisham", "Jumara", "Saban", "Aine",
		"Adar", "Saven", "Amadaine", "Tammaz", "Maigdhal",
		"Choren", "Shaldine", "Nesan", "Danu"
};


_DOFUN (do_time)
{
	extern char str_boot_time[];
	char lc_time[MIL];
	char time_now[MIL];
	char buf[MIL];
	char suf[3];
	int day;
	int month;
	
	day = time_info.day + 1;
	month = time_info.month;
	
	if (month > 12)
		month -= 12;
	
	memset (buf, 0, MIL);
	memset (suf, 0, 3);
	
	if (day > 4 && day < 20)
		sprintf (suf, "%s", "th");
	else if (day % 10 == 1)
		sprintf (suf, "%s", "st");
	else if (day % 10 == 2)
		sprintf (suf, "%s", "nd");
	else if (day % 10 == 3)
		sprintf (suf, "%s", "rd");
	else
		sprintf (suf, "%s", "th");
	
	sprintf (buf,
		"It is %d o'clock %s, Day of %s, %d%s of the Month of %s.\n\r",
		(time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
		(time_info.hour >= 12) ? "pm" : "am",
		day_name[day % 10], day, suf, month_name[month]);
	
	send_to_char (buf, ch);
	
	sprintf (lc_time, "%s", (pMud.last_cover < 1) ? "(n/a)" :
	   (char *) ctime (&pMud.last_cover));
	   sprintf (time_now, "%s", (char *) ctime (&current_time));
	   
	   sprintf (buf,
		   "%s started up at %s\n\r"
		   "Last copyover was at: %s"
		   "The system time is  : %s",
		   pMud.mud_name, str_boot_time, lc_time, time_now);
	   
	   send_to_char (buf, ch);
	   return;
}



_DOFUN (do_weather)
{
	char buf[MSL];
	
	static char *const sky_look[4] = {
		"cloudless",
			"cloudy",
			"rainy",
			"lit by flashes of lightning"
	};
	
	if (!IS_OUTSIDE (ch))
	{
		send_to_char ("You can't see the weather indoors.\n\r", ch);
		return;
	}
	
	sprintf (buf, "The sky is %s and %s.\n\r",
		sky_look[weather_info.sky],
		weather_info.change >= 0
		? "a warm southerly breeze blows" : "a cold northern gust blows");
	send_to_char (buf, ch);
	tornado ();
	lightning ();
    weather_change(2, 0);
	return;
}

_DOFUN (do_help)
{
	HELP_DATA *pHelp;
	BUFFER *output;
	bool found = FALSE;
	bool keyOnly = FALSE;
	char buf[MSL], argall[MIL], argone[MIL];
	
	int level;
	
	output = new_buf ();
	
	memset (buf, 0, MSL);
	memset (argall, 0, MIL);
	memset (argone, 0, MIL);
	
	sprintf (buf, "%s", argument);
	
	if (argument[0] == '\0')
		argument = "summary";
	
	/* this parts handles help a b so that it returns help 'a b' */
	argall[0] = '\0';
	
	while (argument[0] != '\0')
	{
		argument = one_argument (argument, argone);
		
		if (argall[0] != '\0')
			strcat (argall, " ");
		
		strcat (argall, argone);
		
		keyOnly = (strlen (argall) == 1);
	}
	
	for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
	{
		level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;
		
		if (level > get_trust (ch))
			continue;
		
		if (is_name (argall, pHelp->keyword))
		{
			if (output->size == MAX_BUF_SIZE / 2)
			{
				add_buf (output,
					"Too many helps were found that matched your input.\n\r"
					"Please narrow your search.\n\r");
				break;
			}
			
			/* add seperator if found */
			if (found)
				add_buf (output, "\n\r======================================="
				"=====================\n\r\n\r");
			
			if (pHelp->level >= 0 && str_cmp (argall, "imotd"))
			{
				add_buf (output, pHelp->keyword);
				add_buf (output, "\n\r");
			}
			
			/*
			* Strip leading '.' to allow initial blanks.
			*/
			if (keyOnly)
				;
			else
			{
				if (pHelp->text[0] == '.')
					add_buf (output, pHelp->text + 1);
				else
					add_buf (output, pHelp->text);
			}
			found = TRUE;
			if (ch->desc != NULL
				&& ch->desc->connected != CON_PLAYING
				&& ch->desc->connected != CON_GEN_GROUPS)
			{
				break;
			}
		}
	}
	
	if (!found)
	{
		logf ("Request for help [%s] not found.", buf);
		send_to_char ("No help on that word.\n\r", ch);
	}
	else
		page_to_char (buf_string (output), ch);
	
	free_buf (output);
}


/** Function: do_whois
  * Descr   : Displays various information about victim to (ch)
  *	    : Possible output determined by defines in config.h
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_whois)
{
	CHAR_DATA* victim = NULL;

	if ((victim = get_char_world (ch, argument)) == NULL)
	{
		chprintf(ch,"They aren't here.\n\r");
		return;
	}

	if ( IS_NPC(victim) )
	{
		chprintf(ch,"NPCs dont have profiles.\n\r");
		return;
	}

	chprintf(ch, "%s\n\r", IS_NULLSTR(victim->pcdata->profile) ? "Empty profile." : 
						   victim->pcdata->profile);
}


/** Function: do_who
  * Descr   : Display to (ch) who's online at the moment. Uses various
  *         : Defines (located in config.h)
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_who)
{
	
	sh_int slvl = 1;
	sh_int elvl = ML;
	char buf[MSL];
    int iClass = -1;
    int iGuild = -1;
    int iRace = -1;
    int iNation = -1;
	
	BUFFER *buffer;
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	
	memset (buf, 0, MSL);
	buffer = new_buf ();
	
	if (argument[0] != '\0')
	{
		char arg[MIL];
		
		memset (arg, 0, MIL);
		
		/* check for levels */
		argument = one_argument (argument, arg);

		if ( IS_FLAG(pMud.config, _ALLOW_WHO_LEVEL) && is_number (arg) )
		{
			slvl = atoi (arg);
			argument = one_argument (argument, arg);
			
			if (is_number (arg))
				elvl = atoi (arg);
			
			if (elvl < slvl)
				elvl = ML;
			
			if (slvl < 1 || elvl > ML)
			{
				send_to_char ("Invalid level range.\n\r", ch);
				free_buf (buffer);
				return;
			}
			
		}
		else
		
		if (!str_prefix (arg, "immortals"))
		{
			slvl = IM;
			elvl = ML;
		}
		else
			
		{

			if ( IS_FLAG(pMud.config, _ALLOW_WHO_CLASS) )
				iClass = class_lookup (arg);
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_GUILD) )
				if (iClass == -1)
					iGuild = clan_lookup (arg);
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_RACE) )
				if (iGuild <= 0)
					iRace = race_lookup (arg);
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_NATION) )
				if (iGuild <= 0)
					iNation = nation_lookup (arg);
				
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_CLASS) ||
				 IS_FLAG(pMud.config, _ALLOW_WHO_GUILD) ||
				 IS_FLAG(pMud.config, _ALLOW_WHO_RACE)  ||
				 IS_FLAG(pMud.config, _ALLOW_WHO_NATION))
			{
				
				if (iClass < 0 && iGuild == 0 && iRace < 0 && iNation < 0)
				{
					send_to_char ("That's not a valid class, race or guild.\n\r", ch);
					free_buf (buffer);
					return;
				}
			}
		}
	}
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		
		if (d->connected < CON_PLAYING)
			continue;
		
		vch = (d->original != NULL) ? d->original : d->character;
		
		if (vch == NULL)
			continue;
		
		if (!can_see (ch, vch))
			continue;

		if (IS_FLAG(pMud.config, _ALLOW_WHO_LEVEL) && 
			(vch->level < slvl || vch->level > elvl) )
			continue;

		if ( IS_FLAG(pMud.config, _ALLOW_WHO_RACE) &&
			(iRace != -1 && vch->race != iRace) )
			continue;

		if ( IS_FLAG(pMud.config, _ALLOW_WHO_GUILD) && 
			(iGuild != -1))
			if (vch->clan != iGuild || 
				 (!IS_FLAG (vch->states.config, _SHOWGUILD) &&
				  !IS_IMMORTAL (ch)))
			  continue;

		if ( IS_FLAG(pMud.config, _ALLOW_WHO_CLASS) &&
			(iClass != -1 && vch->c_class == iClass) )
      continue;
		
		if ( IS_FLAG(pMud.config, _ALLOW_WHO_NATION) && 
			(iNation != -1 && nation_lookup (vch->pcdata->nation) != iNation) )
			continue;

		if ( IS_FLAG(pMud.config, _GUILD_ONLY_IN_WHO) )
		{
			if (is_clan (vch) && IS_FLAG (vch->states.config, _SHOWGUILD))
				sprintf (buf, "%s] ", pad (clan_table[vch->clan].who_name, 15, 'L'));
			else
				if (IS_IMMORTAL (ch) && is_clan (vch))
					sprintf (buf, "`NGuild`w is `NSecret`w] ");
				else
					sprintf (buf, "%s] ", pad (NULL, 15, 'L'));
			add_buf (buffer, buf);


			if (IS_IMMORTAL (ch) && 
					vch->pcdata->points > pMud.MaxCreationPoints)
			{
				sprintf (buf, "`R[cp=%d]`w ", vch->pcdata->points);
				add_buf (buffer, buf);
			}

			if (IS_IMMORTAL (ch) && IS_FLAG (ch->states.config, _WHO_LEVEL))
			{
				sprintf (buf, "%-2d ", vch->level);
				add_buf (buffer, buf);
			}
			
			if (IS_IMMORTAL (ch) && IS_FLAG (ch->states.config, _WHO_SEX))
			{
				if (TRUESEX (vch) == SEX_MALE)
				  sprintf (buf, "M");
				else
				  sprintf (buf, "F");
				add_buf (buffer, buf);
				if (can_channel(vch))
				  sprintf (buf, "C");
				else
				  sprintf (buf, " ");
				add_buf (buffer, buf);
				if (vch->c_class == CHANNELER)
				  sprintf (buf, "  ");
				else if (vch->c_class == ROGUE)
				  sprintf (buf, "R ");
				else if (vch->c_class == WARRIOR)
				  sprintf (buf, "W ");
				add_buf (buffer, buf);
			}

			if (IS_IMMORTAL (ch) && IS_FLAG (ch->states.config, _WHO_WHERE))
			{
				sprintf (buf, "%s%s `Nfrom `w%s, %s`w\n\r",
					who_flags (vch),
					PERS (vch, ch),
					vch->pcdata->city,
					vch->pcdata->nation);
				add_buf (buffer, buf);
			}
			else
			{
				sprintf (buf, "%s%s%s`w\n\r", who_flags (vch),
					PERS (vch, ch), vch->pcdata->title);
				add_buf (buffer, buf);
			}
		}
		else
		{
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_LEVEL) 
			||   IS_FLAG(pMud.config, _ALLOW_WHO_CLASS)
			||	 IS_FLAG(pMud.config, _ALLOW_WHO_RACE) )
			{
				sprintf (buf, "%s", "[");
				add_buf (buffer, buf);
			}

			if ( IS_FLAG(pMud.config, _ALLOW_WHO_LEVEL) )
			{
				sprintf (buf, "%-2d ", vch->level);
				add_buf (buffer, buf);
			}
			
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_CLASS) )
			{
				sprintf (buf, "%-3s ", class_table[vch->c_class].who_name);
				add_buf (buffer, buf);
			}

			if ( IS_FLAG(pMud.config, _ALLOW_WHO_RACE) )
			{
				sprintf (buf, "%-3s", pc_race_table[vch->race].who_name);
				add_buf (buffer, buf);
			}		

			if ( IS_FLAG(pMud.config, _ALLOW_WHO_LEVEL) 
			||   IS_FLAG(pMud.config, _ALLOW_WHO_CLASS)
			||	 IS_FLAG(pMud.config, _ALLOW_WHO_RACE) )
			{
				sprintf (buf, "%s", "] ");
				add_buf (buffer, buf);
			}		
			if ( IS_FLAG(pMud.config, _ALLOW_WHO_GUILD) && is_clan(vch) &&
					 IS_FLAG (vch->states.config, _SHOWGUILD))
			{
				sprintf (buf, "[%s] ", clan_table[vch->clan].who_name);
				add_buf (buffer, buf);
			}
			
			sprintf (buf, "%s%s%s\n\r", who_flags (vch), vch->name,
				vch->pcdata->title);
			add_buf (buffer, buf);
		}
		
	} /* end: for through descriptors */
	
	add_buf (buffer, "\n\r");
	page_to_char (buf_string (buffer), ch);
	free_buf (buffer);
	
	do_count (ch, "");
	return;
	
}


_DOFUN (do_count)
{
	int count;
	DESCRIPTOR_DATA *d;
	char buf[MSL];
	
	count = 0;
	
	for (d = descriptor_list; d != NULL; d = d->next)
		if (d->connected == CON_PLAYING && can_see (ch, d->character))
			count++;
		
		max_on = UMAX (count, max_on);
		
		if (max_on == count)
			sprintf (buf, "There are %d characters on, the most so far today.\n\r",
			count);
		else
			sprintf (buf, "There are %d characters on, the most on today was %d.\n\r",
			count, max_on);
		
		send_to_char (buf, ch);
}

_DOFUN (do_inventory)
{
	send_to_char ("You are carrying:\n\r", ch);
	
	if ( !ch->carrying )
	  send_to_char("Nothing.\n\r", ch);
	else
	  show_list_to_char (ch->carrying, ch, TRUE, TRUE);
	
	return;
}



_DOFUN (do_equipment)
{
	OBJ_DATA *obj;
	int iWear;
	bool found;
	
	send_to_char ("You are using:\n\r", ch);
	found = FALSE;
	for (iWear = 0; wear_loc_strings[iWear].name != NULL; iWear++)
	{
		if ((obj = get_eq_char (ch, wear_loc_strings[iWear].bit)) == NULL
			&& (wear_loc_strings[iWear].bit != WEAR_NONE
			&& wear_loc_strings[iWear].bit != WEAR_LODGED_A
			&& wear_loc_strings[iWear].bit != WEAR_LODGED_L
			&& wear_loc_strings[iWear].bit != WEAR_LODGED_R)
			&& IS_FLAG (ch->states.config, _SHOW_EMPTY))
		{
			chprintf (ch, "%s %s\n\r",
				wear_loc_strings[iWear].name, "<nothing>");
		}
		else
			if (obj)
				chprintf (ch, "%s %s\n\r",
				wear_loc_strings[iWear].name,
				can_see_obj (ch, obj) ?
				format_obj_to_char (obj, ch, TRUE) : "something.");
			found = TRUE;
	}
	
	
	if (!found)
		send_to_char ("Nothing.\n\r", ch);
	
	return;
}



_DOFUN (do_compare)
{
	char arg1[MIL];
	char arg2[MIL];
	OBJ_DATA *obj1;
	OBJ_DATA *obj2;
	int value1;
	int value2;
	char *msg;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0')
	{
		send_to_char ("Compare what to what?\n\r", ch);
		return;
	}
	
	if ((obj1 = get_obj_carry (ch, arg1, ch)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	if (arg2[0] == '\0')
	{
		for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
		{
			if (obj2->wear_loc != WEAR_NONE
				&& can_see_obj (ch, obj2)
				&& obj1->item_type == obj2->item_type
				&& (obj1->wear_flags & obj2->wear_flags & ITEM_TAKE) != 0)
				break;
		}
		
		if (obj2 == NULL)
		{
			send_to_char ("You aren't wearing anything comparable.\n\r", ch);
			return;
		}
	}
	
	else if ((obj2 = get_obj_carry (ch, arg2, ch)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	msg = NULL;
	value1 = 0;
	value2 = 0;
	
	if (obj1 == obj2)
	{
		msg = "You compare $p to itself.  It looks about the same.";
	}
	else if (obj1->wear_loc != obj2->wear_loc)
	{
		msg = "You can't compare $p and $P.";
	}
	else
	{
		switch (obj1->item_type)
		{
		default:
			msg = "You can't compare $p and $P.";
			break;
			
		case ITEM_ARMOR:
			value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
			value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
			break;
			
		case ITEM_WEAPON:
			value1 = (1 + obj1->value[2]) * obj1->value[1];
			value2 = (1 + obj2->value[2]) * obj2->value[1];
			break;
		}
	}
	
	if (msg == NULL)
	{
		if (value1 == value2)
			msg = "$p and $P look about the same.";
		else if (value1 > value2)
			msg = "$p looks better than $P.";
		else
			msg = "$p looks worse than $P.";
	}
	
	act (msg, ch, obj1, obj2, TO_CHAR, POS_RESTING);
	return;
}



_DOFUN (do_credits)
{
	do_help (ch, "diku");
	return;
}

_DOFUN (do_range)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *vch;
	int count = 0;
	
	if (IS_NPC (ch))
	{
		send_to_char ("Everyone is in range of YOU!\n\r", ch);
		return;
	}
	
	if (IS_IMMORTAL (ch))
	{
		send_to_char ("Your an immortal...DOH!\n\r", ch);
		return;
	}
	
	if (ch->pcdata->peaceful)
	{
		send_to_char ("Your still peaceful, you've nothing to worry about.\n\r",
			ch);
		return;
	}
	
	send_to_char ("The Players in PK range of you are:\n\r", ch);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected >= CON_PLAYING
			&& (vch = d->character) != NULL
			&& !IS_NPC (vch)
			&& ch != vch
			&& (vch->level < IM && !vch->pcdata->peaceful)
			&& vch->in_room != NULL
			&& !IS_SET (vch->in_room->room_flags, ROOM_NOWHERE)
			&& can_see (ch, vch)
			&& ((ch->level - vch->level <= pMud.pk_limit &&
			ch->level - vch->level > -1) ||
			(vch->level - ch->level <= pMud.pk_limit &&
			vch->level - ch->level > -1)))
		{
			count++;
			chprintf (ch, "%s\n\r", PERS (vch, ch));
		}
		
	}
	
	if (count == 0)
		send_to_char ("(none)\n\r", ch);
	
}

_DOFUN (do_where)
{
	char arg[MIL];
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	bool found;
	
	memset(arg, 0, MIL);
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		chprintf(ch, "Players in %s:\n\rArea Credits: %s\n\r\n\r", 
				 ch->in_room->area->name, ch->in_room->area->credits);

		found = FALSE;
		for (d = descriptor_list; d; d = d->next)
		{
			if (d->connected == CON_PLAYING
				&& (victim = d->character) != NULL
				&& !IS_NPC (victim)
				&& victim->in_room != NULL
				&& !IS_SET (victim->in_room->room_flags, ROOM_NOWHERE)
				&& (is_room_owner (ch, victim->in_room)
				|| !room_is_private (victim->in_room))
				&& victim->in_room->area == ch->in_room->area
				&& can_see (ch, victim))
			{
				found = TRUE;
				chprintf(ch, "%-28s %s\n\r",
					PERS (victim, ch), victim->in_room->name);
			}
		}
		if (!found)
			chprintf(ch, "None\n\r");
	}
	else
	{
		found = FALSE;
		for (victim = char_list; victim != NULL; victim = victim->next)
		{
			if (victim->in_room != NULL
				&& victim->in_room->area == ch->in_room->area
				&& can_see (ch, victim) && is_name (arg, victim->name))
			{
				found = TRUE;
				chprintf(ch, "%-28s %s\n\r",
					PERS (victim, ch), victim->in_room->name);
				break;
			}
		}
		if (!found)
			act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR, POS_RESTING);
	}
	
	return;
}




_DOFUN (do_consider)
{
	char arg[MIL];
	CHAR_DATA *victim;
	char *msg;
	int diff;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Consider killing whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They're not here.\n\r", ch);
		return;
	}
	
	if (is_safe (ch, victim))
	{
		send_to_char ("Don't even think about it.\n\r", ch);
		return;
	}
	
	diff = victim->level - ch->level;
	
	if (diff <= -10)
		msg = "You can kill $N naked and weaponless.";
	else if (diff <= -5)
		msg = "$N is no match for you.";
	else if (diff <= -2)
		msg = "$N looks like an easy kill.";
	else if (diff <= 1)
		msg = "The perfect match!";
	else if (diff <= 4)
		msg = "$N says 'Do you feel lucky, punk?'.";
	else if (diff <= 9)
		msg = "$N laughs at you mercilessly.";
	else
		msg = "Death will thank you for your gift.";
	
	act (msg, ch, NULL, victim, TO_CHAR, POS_RESTING);
	return;
}



void set_title (CHAR_DATA * ch, char *title)
{
	char buf[MSL];
	
	if (IS_NPC (ch))
	{
		bugf ("[act_info.c::set_title] Set_title: NPC.");
		return;
	}
	
	memset(buf, 0, MSL);
	
	if (title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?')
	{
		buf[0] = ' ';
		strcpy (buf + 1, title);
	}
	else
	{
		strcpy (buf, title);
	}
	
	free_string (ch->pcdata->title);
	ch->pcdata->title = str_dup (buf);
	return;
}



_DOFUN (do_title)
{
	if (IS_NPC (ch))
		return;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Change your title to what?\n\r", ch);
		return;
	}
	
	smash_tilde (argument);
	set_title (ch, argument);
	send_to_char ("Ok.\n\r", ch);
}

_DOFUN (do_rdesc)
{
	char buf[MIL];
	
	if (IS_NPC (ch) || !IS_FLAG(pMud.config, _ALLOW_RDESC) )
		return;
	
	if (argument[0] == '\0')
	{
		chprintf (ch, "Your rdesc is: %s\n\r",
			(ch->pcdata->r_desc[0] !=
			'\0') ? ch->pcdata->r_desc : "(none)");
		return;
	}
	
	if (argument[0] == '.')
	{
		send_to_char ("Room description erased.\n\r", ch);
		if (ch->pcdata->r_desc)
			free_string (ch->pcdata->r_desc);
		
		ch->pcdata->r_desc = str_dup ("");
		return;
	}
	
	sprintf (buf, "%s", argument);
	
	if (ch->pcdata->r_desc)
		free_string (ch->pcdata->r_desc);
	
	ch->pcdata->r_desc = str_dup (buf);
	send_to_char ("Ok.\n\r", ch);
}


_DOFUN (do_description)
{
	if ( IS_NULLSTR(argument) )
	{
		chprintf(ch,"Your description is:\n\r%s\n\r", ch->description);
		return;
	}
	
	send_to_char ("Editing character description.\n\r", ch);
	string_append (ch, &ch->description);
	return;
}

_DOFUN (do_profile)
{
	if ( IS_NULLSTR(argument) )
	{
		chprintf(ch,"Your profile is:\n\r%s\n\r", ch->pcdata->profile);
		return;
	}
	
	send_to_char ("Editing character profile.\n\r", ch);
	string_append (ch, &ch->pcdata->profile);
	return;
}


_DOFUN (do_report)
{
	char buf[MIL];
	
	sprintf (buf,
		"You say 'I have %d/%d Hp %d/%d Ab %d/%d En %ld xp.'\n\r",
		ch->hit, ch->max_hit,
		TAU (ch), OPR (ch), ch->move, ch->max_move, ch->exp);
	
	send_to_char (buf, ch);
	
	sprintf (buf, "$n says 'I have %d/%d Hp %d/%d Ab %d/%d En %ld xp.'",
		ch->hit, ch->max_hit,
		TAU (ch), OPR (ch), ch->move, ch->max_move, ch->exp);
	
	act (buf, ch, NULL, NULL, TO_ROOM, POS_RESTING);
	
	return;
}



_DOFUN (do_practice)
{
	char buf[MSL];
	int sn = 1;
	
	if (IS_NPC (ch))
		return;
	
	if (argument[0] == '\0')
	{
		int col;
		BUFFER *buffer;
		
		buffer = new_buf ();
		
		col = 0;
	
		if (col % 3 != 0)
			add_buf (buffer, "\n\r");
		
		sprintf (buf, "You have %d practice sessions left.\n\r", ch->practice);
		add_buf (buffer, buf);
		
		page_to_char (buf_string (buffer), ch);
		free_buf (buffer);
		
	}
	else
	{
		CHAR_DATA *mob;
		int adept;
		
		if (!IS_AWAKE (ch))
		{
			send_to_char ("In your dreams, or what?\n\r", ch);
			return;
		}
		
		for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
		{
			if (IS_NPC (mob) && IS_FLAG (mob->states.player, _ACT_PRACTICE))
				break;
		}
		
		if (mob == NULL)
		{
			send_to_char ("You can't do that here.\n\r", ch);
			return;
		}
		
		if (ch->practice <= 0)
		{
			send_to_char ("You have no practice sessions left.\n\r", ch);
			return;
		}
		
		adept = IS_NPC (ch) ? 100 : class_table[ch->c_class].skill_adept;
		
		if (ch->pcdata->learned[sn] >= adept)
		{
			sprintf (buf, "You are already learned at %s.\n\r",
				skill_table[sn].name);
			send_to_char (buf, ch);
		}
		else
		{
			ch->practice--;
			ch->pcdata->learned[sn] +=
				int_app[get_curr_stat (ch, STAT_INT)].learn;
//				skill_table[sn].rating[ch->c_class];
			if (ch->pcdata->learned[sn] < adept)
			{
				act ("You practice $T.",
					ch, NULL, skill_table[sn].name, TO_CHAR, POS_RESTING);
				act ("$n practices $T.",
					ch, NULL, skill_table[sn].name, TO_ROOM, POS_RESTING);
			}
			else
			{
				ch->pcdata->learned[sn] = adept;
				act ("You are now learned at $T.",
					ch, NULL, skill_table[sn].name, TO_CHAR, POS_RESTING);
				act ("$n is now learned at $T.",
					ch, NULL, skill_table[sn].name, TO_ROOM, POS_RESTING);
			}
		}
	}
	return;
}



/*
* 'Wimpy' originally by Dionysos.
*/
_DOFUN (do_wimpy)
{
	char buf[MSL];
	char arg[MIL];
	int wimpy;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
		wimpy = ch->max_hit / 5;
	else
		wimpy = atoi (arg);
	
	if (wimpy < 0)
	{
		send_to_char ("Your courage exceeds your wisdom.\n\r", ch);
		return;
	}
	
	if (wimpy > ch->max_hit / 2)
	{
		send_to_char ("Such cowardice ill becomes you.\n\r", ch);
		return;
	}
	
	ch->wimpy = wimpy;
	sprintf (buf, "Wimpy set to %d hit points.\n\r", wimpy);
	send_to_char (buf, ch);
	return;
}



_DOFUN (do_password)
{
	char arg1[MIL];
	char arg2[MIL];
	char *pArg;
	char *pwdnew;
	char *p;
	char cEnd;
	
	if (IS_NPC (ch))
		return;
	
		/*
		* Can't use one_argument here because it smashes case.
		* So we just steal all its code.  Bleagh.
	*/
	pArg = arg1;
	while (isspace (*argument))
		argument++;
	
	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;
	
	while (*argument != '\0')
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';
	
	pArg = arg2;
	while (isspace (*argument))
		argument++;
	
	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;
	
	while (*argument != '\0')
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*pArg++ = *argument++;
	}
	*pArg = '\0';
	
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: password <old> <new>.\n\r", ch);
		return;
	}
	
	if (strcmp (crypt (arg1, ch->pcdata->pwd), ch->pcdata->pwd))
	{
		WAIT_STATE (ch, 40);
		send_to_char ("Wrong password.  Wait 10 seconds.\n\r", ch);
		return;
	}
	
	if (strlen (arg2) < 5)
	{
		send_to_char ("New password must be at least five characters long.\n\r",
			ch);
		return;
	}
	
	/*
	* No tilde allowed because of player file format.
	*/
	pwdnew = crypt (arg2, ch->name);
	for (p = pwdnew; *p != '\0'; p++)
	{
		if (*p == '~')
		{
			send_to_char ("New password not acceptable, try again.\n\r", ch);
			return;
		}
	}
	
	free_string (ch->pcdata->pwd);
	ch->pcdata->pwd = str_dup (pwdnew);
	save_char_obj (ch);
	send_to_char ("Ok.\n\r", ch);
	return;
}


/* 
* returns everything the character can see at that location in buf
* returns number of creatures seen 
*/

int scan_room (CHAR_DATA * ch, const ROOM_INDEX_DATA * room, char *buf)
{
	CHAR_DATA *target = room->people;
	int number_found = 0;
	
	while (target != NULL)	/* repeat as long more peple in the room */
	{
		if (can_see (ch, target))	/* show only if the character can see the target */
		{
			strcat (buf, " - ");
			strcat (buf, PERS (target, ch));
			strcat (buf, "\n\r");
			number_found++;
		}
		target = target->next_in_room;
	}
	
	return number_found;
}

_DOFUN (do_scan)
{
	EXIT_DATA *pexit;
	ROOM_INDEX_DATA *room;
	extern char *const dir_name[];
	char buf[MSL];
	int dir;
	int distance;
	
	sprintf (buf, "Right here you see:\n\r");
	if (scan_room (ch, ch->in_room, buf) == 0)
		strcat (buf, "Noone\n\r");
	
	send_to_char (buf, ch);
	
	for (dir = 0; dir < 6; dir++)	/* look in every direction */
	{
		room = ch->in_room;		/* starting point */
		
		for (distance = 1; distance < 4; distance++)
		{
			
			if (distance > 1 && (IS_NPC (ch) || !is_clan (ch) ||
				!IS_FLAG (clan_table[ch->clan].flags, _REDHAND)))
				break;
			
			pexit = room->exit[dir];	/* find the door to the next room */
			
			if ((pexit == NULL)
				|| (pexit->u1.to_room == NULL)
				|| (IS_SET (pexit->exit_info, EX_CLOSED))
				|| (IS_SET (pexit->exit_info, EX_SECRET)))
				break;
			
			/* char can see the room */
			sprintf (buf, "%d %s from here you see:\n\r", distance, dir_name[dir]);
			if (scan_room (ch, pexit->u1.to_room, buf))
				send_to_char (buf, ch);
			
			room = pexit->u1.to_room;	/* go to the next room */
		}				/* for distance */
	}				/* for dir */
}



/** Function: do_lore
  * Descr   : A non-channelers replacemnt for 'delve item'. Gives some
  *         : information about the item in question.
  * Returns : info only
  * Syntax  : lore ( ch, item_name )
  * Written : v1.0 01/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_lore)
{
	char arg[MIL];
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	int cnt = 0;
	
	one_argument (argument, arg);
	
	if (IS_NPC (ch))
	{
		send_to_char ("PC's Only!\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		send_to_char ("You can't seem to find that...\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, argument)) == NULL)
	{
		send_to_char ("You can't seem to find that...\n\r", ch);
		return;
	}
	
	/* make 3 skill checks, each revealing more information than
	* the last. 
	*/
	
	if (number_percent () <= ch->pcdata->learned[skill_lookup("lore")])
	{
		const char *level_text[] = { "any woolhead could use it.",	/*  0 -  5 */
			"anyone past the age of 10 could use it.",	/*  6 - 14 */
			"requires a bit of understanding to use.",	/* 15 - 24 */
			"someone with a bit of education could use it.",	/* 25 - 35 */
			"is meant for those with experience.",	/* 36 - 44 */
			"you would have to be Ogier, or Aes Sedai trained to use it!"	/*46+ */
		};
		
		
		chprintf (ch,
			"This is %s\n\rAnd %s\n\r",
			obj->short_descr,
			level_text[UMIN (5, (int) obj->level / 10)]);
		
		if (obj->extra_descr)
		{
			EXTRA_DESCR_DATA *ed;
			
			send_to_char ("Some of the well known names of this object are:\n\r",
				ch);
			
			for (ed = obj->extra_descr; ed; ed = ed->next)
			{
				chprintf (ch, "%s%s", ed->keyword, (ed->next) ? ", " : "\n\r");
			}
		}
		
		if (obj->pIndexData->extra_descr)
		{
			EXTRA_DESCR_DATA *ed;
			
			send_to_char ("\n\rand is also known as:\n\r", ch);
			
			for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next)
			{
				chprintf (ch, "%s%s", ed->keyword, (ed->next) ? ", " : "\n\r");
			}
		}
	}
	else
		
	{
		send_to_char ("You've never heard of one of these before.\n\r", ch);
		return;
	}
	
	if (number_percent () <= ch->pcdata->learned[skill_lookup("lore")])
	{
		
		const char *rare_text[] = {
			"A commonly found",
				"An uncommon",
				"A fairly rare",
				"A Rare",
				"An extremely rare"
		};
		
		const char *condition_text[] = {
			"useless",		/*  0 -  4  */
				"virtually destroyed",	/*  5 -  14 */
				"nearly worthless",	/* 15 -  24 */
				"in bad condition",	/* 25 -  34 */
				"in poor shape",		/* 35 -  44 */
				"in ok condition",	/* 45 -  54 */
				"in good shape",		/* 55 -  64 */
				"in really good condition",	/* 65 -  74 */
				"in great shape",		/* 75 -  84 */
				"in virtually new condition",	/* 85 -  94 */
				"in brand new condition"
		};				/* 95 - 100 */
		
		chprintf (ch,
			"%s type of %s it,\n\r"
			"is either carried or worn by most. [%s]\n\r",
			rare_text[UMIN (4, eq_cost (obj) / 10)],
			item_name (obj->item_type),
			flag_string(wear_flags, obj->wear_flags));
		
		if (obj->extra_flags > 0)
			chprintf (ch, "With the properties of %s\n\r\n\r",
			flag_string(extra_flags, obj->extra_flags));
		
		chprintf (ch, "Hefting the object, you believe it to be around"
			" %d pounds%s",
			get_obj_weight (obj),
			(obj->contains) ?
			", including anything it might contain.\n\r" : ".\n\r");
		
		chprintf (ch, "It appears to be %s and\n\r"
			"you estimate it's value to be %d silver.\n\r",
			condition_text[UMIN (10, obj->condition / 10)],
			obj->cost);
		
	}
	
	
	if (number_percent () <= ch->pcdata->learned[skill_lookup("lore")] / 2)
	{
		switch (obj->item_type)
		{
			
		case ITEM_DRINK_CON:
			chprintf (ch, "It holds %s-colored %s.\n\r",
				liq_table[obj->value[2]].liq_color,
				liq_table[obj->value[2]].liq_name);
			break;
			
		case ITEM_CONTAINER:
			chprintf (ch, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
				obj->value[0], obj->value[3],
				flag_string(container_flags, obj->value[1]));
			
			if (obj->value[4] != 100)
			{
				chprintf (ch, "Weight multiplier: %d%%\n\r", obj->value[4]);
			}
			break;
			
		case ITEM_WEAPON:
			send_to_char ("Weapon type is ", ch);
			switch (obj->value[0])
			{
				case (WEAPON_EXOTIC):
					send_to_char ("exotic.\n\r", ch);
					break;
				case (WEAPON_SWORD):
					send_to_char ("sword.\n\r", ch);
					break;
				case (WEAPON_DAGGER):
					send_to_char ("dagger.\n\r", ch);
					break;
				case (WEAPON_SPEAR):
					send_to_char ("spear.\n\r", ch);
					break;
				case (WEAPON_STAFF):
					send_to_char ("staff.\n\r", ch);
					break;
				case (WEAPON_MACE):
					send_to_char ("mace/club.\n\r", ch);
					break;
				case (WEAPON_AXE):
					send_to_char ("axe.\n\r", ch);
					break;
				case (WEAPON_FLAIL):
					send_to_char ("flail.\n\r", ch);
					break;
				case (WEAPON_WHIP):
					send_to_char ("whip.\n\r", ch);
					break;
				case (WEAPON_POLEARM):
					send_to_char ("polearm.\n\r", ch);
					break;
				default:
					send_to_char ("unknown.\n\r", ch);
					break;
			}
			
			chprintf (ch, "Damage is %dd%d (average %d).\n\r",
				obj->value[1], obj->value[2],
				(1 + obj->value[2]) * obj->value[1] / 2);
			
			if (obj->value[4])	/* weapon flags */
			{
				chprintf (ch, "Weapons flags: %s\n\r",
					flag_string(weapon_type2, obj->value[4]));
			}
			break;
			
			case ITEM_ARMOR:
				chprintf (ch,
					"Armor class is %d pierce, %d bash, %d slash"
					", and %d vs. weaves.\n\r",
					obj->value[0], obj->value[1], obj->value[2],
					obj->value[3]);
				break;
				
			default:
				break;
				
		}				/* end switch */
		
		for (cnt = 0, paf = obj->affected; paf; paf = paf->next)
		{
			if (cnt == 0)
			{
				send_to_char ("\n\rNumber Modifier Affects\n\r", ch);
				send_to_char ("------ -------- -------\n\r", ch);
			}
			chprintf (ch, "[%4d] %-8d %s\n\r",
				cnt++,
				paf->modifier,
				flag_string (apply_flags, paf->location));
			
		}
		
		if (!obj->enchanted)
			for (paf = obj->pIndexData->affected; paf; paf = paf->next)
			{
				if (cnt == 0)
				{
					send_to_char ("\n\rNumber Modifier Affects\n\r", ch);
					send_to_char ("------ -------- -------\n\r", ch);
				}
				chprintf (ch, "[%4d] %-8d %s\n\r",
					cnt++,
					paf->modifier,
					flag_string (apply_flags, paf->location));
				
			}
  }
  
  return;
}
