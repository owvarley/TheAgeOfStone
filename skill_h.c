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

#include <sys/types.h>
//#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"


bool make_skill_roll(int iRating, float mod)
{

  int roll = -1;
 // int chance = iRating;

  if ( mod > 0.01 && iRating > 0)
	  iRating = UMAX(1, (int)(iRating * mod));

  if ( iRating < 1 )
	  return FALSE;

  // should always be a 5% chance of failure, no matter how good you are.
  if ( iRating > 94 )
	  iRating = 94;

  roll = number_percent();

//
//  _trace(FALSE,"[fight_h.c::make_skill_roll] iRating: %d mod: %2.2f Roll: %d Return %s\n",
//	  iRating, mod, roll, roll <= iRating ? "TRUE" : "FALSE");

  return ( roll <= iRating );

}


/*
 * Used by both meets_reqs and by bad_selection. Moved here to avoid
 * having to write this 4 times...
 */
bool meets_flags (CHAR_DATA * ch, SET flags)
{

  sh_int talents[MAX_TALENTS];
  
  if ( !IS_NPC(ch) )
    memcpy(talents, ch->pcdata->talents, sizeof(ch->pcdata->talents));
  else
    memset(talents, 0, sizeof(talents));

  if (flags == 0)
    return TRUE;

  if (
      (IS_FLAG (flags, _REQ_RACE_HUMAN) && ch->race != race_lookup ("human"))
      || (IS_FLAG (flags, _REQ_RACE_TROLLOC)
	  && ch->race != race_lookup ("trolloc"))
      || (IS_FLAG (flags, _REQ_RACE_OGIER)
	  && ch->race != race_lookup ("ogier"))
      || (IS_FLAG (flags, _REQ_RACE_FADE) && ch->race != race_lookup ("fade"))
      || (IS_FLAG (flags, _REQ_RACE_DRAGHKAR)
	  && ch->race != race_lookup ("draghkar"))
      || (IS_FLAG (flags, _REQ_SPEC_AIR) && talents[gtn_air] < 1)
      || (IS_FLAG (flags, _REQ_SPEC_EARTH) && talents[gtn_earth] < 1)
      || (IS_FLAG (flags, _REQ_SPEC_WATER) && talents[gtn_water] < 1)
      || (IS_FLAG (flags, _REQ_SPEC_SPIRIT) && talents[gtn_spirit] < 1)
      || (IS_FLAG (flags, _REQ_SPEC_FIRE) && talents[gtn_fire] < 1)
      || (IS_FLAG (flags, _REQ_TALENT_DREAMING)
	  && talents[gtn_dreaming] < 1)
      || (IS_FLAG (flags, _REQ_TALENT_TRAVELLING)
	  && talents[gtn_travelling] < 1)
      || (IS_FLAG (flags, _REQ_TALENT_HEALING)
	  && talents[gtn_healing] < 1)
      || (IS_FLAG (flags, _REQ_TALENT_SINGING)
	  && talents[gtn_tree_singing] < 1)
      || (IS_FLAG (flags, _REQ_CHANNEL) && !can_channel(ch)))
    return FALSE;

	if (IS_FLAG (flags, _REQ_ROGUE) && IS_FLAG (flags, _REQ_WARRIOR) &&
		  (ch->c_class == ROGUE || ch->c_class == WARRIOR))
		return TRUE;
	
	if	((IS_FLAG (flags, _REQ_CHANNELER) && ch->c_class != CHANNELER) ||
		 (IS_FLAG (flags, _REQ_ROGUE) && ch->c_class != ROGUE) ||
		 (IS_FLAG (flags, _REQ_WARRIOR) && ch->c_class != WARRIOR))
		return FALSE;

  return TRUE;
}

/** Function: meets_reqs
  * Descr   : Determines if a character meets all the necessary requirements
  *         : for a particular skill/weave.
  * Returns : TRUE/FALSE if requirements are met
  * Syntax  : meets_reqs(who, skill#, include check for skill level?)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool meets_reqs (CHAR_DATA * ch, int sn, bool check_level)
{
  sh_int rating;
  sh_int sex;
  unsigned long guild;
  SET flags;

  rating = skill_table[sn].rating;

  set_init (flags);
  memcpy (flags, skill_table[sn].flags, sizeof (flags));

  if (IS_NPC (ch))
    sex = ch->sex;
  else
    sex = ch->pcdata->true_sex;

  guild = 0;

  if (check_level && (rating < 1 || rating > ch->level))
    return FALSE;

  if ((sn == gsn_seize && sex == SEX_FEMALE) ||
      (sn == gsn_embrace && sex == SEX_MALE))
    return FALSE;

  if (guild == _ANY && !is_clan (ch))
    return FALSE;
  else
    if ((guild > 0 && guild != _ANY) &&
	(!is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, guild)))
    return FALSE;


  return meets_flags (ch, flags);

}

/** Function: bad_selection
  * Descr   : Determines if a skill/talent selection is valid or not
  *         : Differs from 'meets_reqs' in that this checks to see if
  *         : the skill has already been selected and also prints some
  *         : reasons why it may not be a valid selection.
  * Returns : TRUE/FALSE is invalid
  * Syntax  : bad_selection table#, selection# yes_report_to_user
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool bad_selection (CHAR_DATA * ch, int table, int sn, bool report)
{
	  return FALSE;
}


/** Function: over_limit
  * Descr   : Determines if a talent or specialty chosen will place
  *         : the player over the defined limits or not.
  * Returns : TRUE/FALSE if player would go over the limit
  * Syntax  : over_limit(who, talent gsn, report yes or no)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool over_limit (CHAR_DATA * ch, int gn, bool report)
{

	if (IS_FLAG (talent_table[gn].flags, _IS_TALENT))
	{
		int x;
		int count = 0;
		
		for (x = 0; x < MAX_TALENTS; x++)
		{
			if (ch->gen_data->talent_chosen[x] == TRUE &&
				IS_FLAG (talent_table[x].flags, _IS_TALENT) &&
				!IS_FLAG (talent_table[x].flags, _NO_CHARGE))
			
			count++;
		}
		
		if (count >= pMud.MaxPCTalents)
		{
			if (report)
				chprintf (ch, "Only %d talent(s) may be selected.\n\r",
				pMud.MaxPCTalents);
			return TRUE;
		}
	}
	else
		if (IS_FLAG (talent_table[gn].flags, _IS_SPECIALTY))
		{
			int x;
			int count = 0;
			
			/* Only allow one talent or specialty */
			for (x = 0; x < MAX_TALENTS; x++)
			{
				if (ch->gen_data->talent_chosen[x] == TRUE &&
					IS_FLAG (talent_table[x].flags, _IS_SPECIALTY) &&
					!IS_FLAG (talent_table[x].flags, _NO_CHARGE))
					count++;
			}
				
			if (count >= pMud.MaxPCSpecialties)
			{
				if (report)
					chprintf (ch, "Only %d specialties may be selected.\n\r",
					pMud.MaxPCSpecialties);
				return TRUE;
			}
		}
		
		return FALSE;
		
}



/* RT spells and skills show the players spells (or skills) */
_DOFUN (do_spells)
{
	BUFFER *buffer;
	char arg[MIL];
	char spell_list[LEVEL_HERO + 1][MSL];
	char spell_columns[LEVEL_HERO + 1];
	int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
	bool fAll = FALSE, found = FALSE;
	char buf[MSL];
	
	if (IS_NPC (ch))
		return;
	
	if (argument[0] != '\0')
	{
		fAll = TRUE;
		
		if (str_prefix (argument, "all"))
		{
			argument = one_argument (argument, arg);
			if (!is_number (arg))
			{
				send_to_char ("Arguments must be numerical or all.\n\r", ch);
				return;
			}
			max_lev = atoi (arg);
			
			if (max_lev < 1 || max_lev > LEVEL_HERO)
			{
				sprintf (buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
				send_to_char (buf, ch);
				return;
			}
			
			if (argument[0] != '\0')
			{
				argument = one_argument (argument, arg);
				if (!is_number (arg))
				{
					send_to_char ("Arguments must be numerical or all.\n\r", ch);
					return;
				}
				min_lev = max_lev;
				max_lev = atoi (arg);
				
				if (max_lev < 1 || max_lev > LEVEL_HERO)
				{
					sprintf (buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
					send_to_char (buf, ch);
					return;
				}
				
				if (min_lev > max_lev)
				{
					send_to_char ("That would be silly.\n\r", ch);
					return;
				}
			}
		}
	}
	
	if (max_lev > ch->level)
		max_lev = ch->level;

	/* initialize data */
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		spell_columns[level] = 0;
		spell_list[level][0] = '\0';
	}
	
	for (sn = 0; sn < maxSkill; sn++)
	{
		if (skill_table[sn].name == NULL)
			continue;
		
//		if (!meets_reqs (ch, sn, FALSE))
//			continue;
// can now learn any weave
		
		if ((level = skill_table[sn].skill_level[ch->c_class]) < LEVEL_HERO
			&& (fAll)
			&& level >= min_lev && level <= max_lev
			&& skill_table[sn].weave_fun != NULL && GET_SKILL (ch, sn) > 0)
		{
			found = TRUE;
			level = skill_table[sn].skill_level[ch->c_class];
			
		
			if (spell_list[level][0] == '\0')
				sprintf (spell_list[level], "\n\r%2d] %s", level, buf);
			
			else			/* append */
			{
				if (++spell_columns[level] % 2 == 0)
					strcat (spell_list[level], "\n\r    ");
				strcat (spell_list[level], buf);
			}
		}
	}
	
	/* return results */
	
	if (!found)
	{
		send_to_char ("No weaves known.\n\r", ch);
		return;
	}
	
	buffer = new_buf ();
	
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		if (spell_list[level][0] != '\0')
			add_buf (buffer, spell_list[level]);
	}
		
	add_buf (buffer, "\n\r");
	page_to_char (buf_string (buffer), ch);
	free_buf (buffer);
}

/** Function: do_skills
  * Descr   : show all skills known by ch
  *         : (including guild specific skills)
  * Returns : (void)
  * Syntax  : [all/level range]
  * Written : v1.1 2/98
  * Author  : Rom Consortium. Last update by Gary McNickle
  */
_DOFUN (do_skills)
{
	BUFFER *buffer;
	char arg[MIL];
	char skill_list[LEVEL_HERO + 1][MSL];
	char skill_columns[LEVEL_HERO + 1];
	int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
	bool fAll = FALSE, found = FALSE;
	char buf[MSL];
	
	if (IS_NPC (ch))
		return;
	
	if (argument[0] != '\0')
	{
		fAll = TRUE;
		
		if (str_prefix (argument, "all"))
		{
			argument = one_argument (argument, arg);
			if (!is_number (arg))
			{
				send_to_char ("Arguments must be numerical or all.\n\r", ch);
				return;
			}
			max_lev = atoi (arg);
			
			if (max_lev < 1 || max_lev > LEVEL_HERO)
			{
				sprintf (buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
				send_to_char (buf, ch);
				return;
			}
			
			if (argument[0] != '\0')
			{
				argument = one_argument (argument, arg);
				if (!is_number (arg))
				{
					send_to_char ("Arguments must be numerical or all.\n\r", ch);
					return;
				}
				min_lev = max_lev;
				max_lev = atoi (arg);
				
				if (max_lev < 1 || max_lev > LEVEL_HERO)
				{
					sprintf (buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
					send_to_char (buf, ch);
					return;
				}
				
				if (min_lev > max_lev)
				{
					send_to_char ("That would be silly.\n\r", ch);
					return;
				}
			}
		}
	}
	
	
	/* initialize data */
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		skill_columns[level] = 0;
		skill_list[level][0] = '\0';
	}
	
	for (sn = 0; sn < maxSkill; sn++)
	{
		if (skill_table[sn].name == NULL)
			continue;
		
		if (!meets_reqs (ch, sn, FALSE))
			continue;
		
		if ((level = skill_table[sn].skill_level[ch->c_class]) < LEVEL_HERO + 1
			&& (fAll || level <= ch->level)
			&& level >= min_lev && level <= max_lev
			&& skill_table[sn].weave_fun == NULL && GET_SKILL (ch, sn) > 0)
		{
			found = TRUE;
			level = skill_table[sn].skill_level[ch->c_class];
			
			if (ch->level < level)
				sprintf (buf, "%-18s n/a      ", skill_table[sn].name);
			else
				sprintf (buf, "%-18s %3d%%      ",
				skill_table[sn].name, GET_SKILL (ch, sn));
			
			if (skill_list[level][0] == '\0')
				sprintf (skill_list[level], "\n\rLevel %2d: %s", level, buf);
			else			/* append */
			{
				if (++skill_columns[level] % 2 == 0)
					strcat (skill_list[level], "\n\r          ");
				strcat (skill_list[level], buf);
			}
		}
	}
	
	/* return results */
	
	if (!found)
	{
		send_to_char ("No skills found.\n\r", ch);
		return;
	}
	
	buffer = new_buf ();
	for (level = 0; level < LEVEL_HERO + 1; level++)
	{
		if (skill_list[level][0] != '\0')
			add_buf (buffer, skill_list[level]);
	}

	add_buf (buffer, "\n\r");
	page_to_char (buf_string (buffer), ch);
	free_buf (buffer);
}

/*
 * shows skills, talents, groups and costs (only if not bought)
 */
void list_group_costs (CHAR_DATA * ch)
{
	char buf[MSL];
	int gn, sn, col;
	
	if (IS_NPC (ch))
		return;
	
	if (can_channel(ch))
	{
		if (ch->sex == SEX_MALE)
		{
			send_to_char ("\n\rfemale spheres     cp    male spheres       cp    neutral spheres    cp\n\r", ch);
			sprintf (buf, "sphere `Cair`w         %-5d sphere `Nearth`w       %-5d sphere `Wspirit`w      %-5d\n\r",
				(ch->flow_str.perm[AIR]+1)*2+1,
				(ch->flow_str.perm[EARTH]+1)*2-1,
				(ch->flow_str.perm[SPIRIT]+1)*2);
			send_to_char (buf, ch);
			sprintf (buf, "sphere `Bwater`w       %-5d sphere `Rfire`w        %-5d\n\r\n\r",
				(ch->flow_str.perm[WATER]+1)*2+1,
				(ch->flow_str.perm[FIRE]+1)*2-1);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("\n\rfemale spheres     cp    male spheres       cp    neutral spheres    cp\n\r", ch);
			sprintf (buf, "sphere `Cair`w         %-5d sphere `Nearth`w       %-5d sphere `Wspirit`w      %-5d\n\r",
				(ch->flow_str.perm[AIR]+1)*2-1,
				(ch->flow_str.perm[EARTH]+1)*2+1,
				(ch->flow_str.perm[SPIRIT]+1)*2);
			send_to_char (buf, ch);
			sprintf (buf, "sphere `Bwater`w       %-5d sphere `Rfire`w        %-5d\n\r\n\r",
				(ch->flow_str.perm[WATER]+1)*2-1,
				(ch->flow_str.perm[FIRE]+1)*2+1);
			send_to_char (buf, ch);
		}
	}
	col = 0;
	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
		"talent", "cp", "talent", "cp", "talent", "cp");
	
	send_to_char (buf, ch);
		
	for (gn = 0; gn < MAX_TALENTS; gn++)
	{
		if (talent_table[gn].name == NULL)
			break;
		
		if (bad_selection (ch, TALENT_TABLE, gn, FALSE))
			continue;
		
		sprintf (buf, "%-18s %-5d ",
			talent_table[gn].name, talent_table[gn].rating);
		send_to_char (buf, ch);
		if (++col % 3 == 0)
			send_to_char ("\n\r", ch);
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	col = 0;
	
	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r", "group", "cp",
		"group", "cp", "group", "cp");
	send_to_char (buf, ch);
	
	for (gn = 0; gn < maxGroup; gn++)
	{
		if (group_table[gn].name == NULL)
			break;
		
		if (bad_selection (ch, GROUP_TABLE, gn, FALSE))
			continue;
		
		sprintf (buf, "%-18s %-5d ",
			group_table[gn].name, group_table[gn].rating);
		
		send_to_char (buf, ch);
		if (++col % 3 == 0)
			send_to_char ("\n\r", ch);
		
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	col = 0;
	
	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r", "skill", "cp",
		"skill", "cp", "skill", "cp");
	send_to_char (buf, ch);
	
	for (sn = 0; sn < maxSkill; sn++)
	{
		if ( IS_NULLSTR(skill_table[sn].name) )
		{
			if ( sn == 0 )
				sn = 1;
			else
				break;
		}
		
		if (bad_selection (ch, SKILL_TABLE, sn, FALSE) ||
			skill_table[sn].weave_fun != NULL)
			continue;
		
		sprintf (buf, "%-18s %-5d ",
			skill_table[sn].name, skill_table[sn].rating);
		
		send_to_char (buf, ch);
		if (++col % 3 == 0)
			send_to_char ("\n\r", ch);
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	sprintf (buf, "Creation points: %d\n\r", ch->pcdata->points);
	send_to_char (buf, ch);
	sprintf (buf, "Experience per level: %ld\n\r",
		exp_per_level (ch, ch->gen_data->points_chosen));

	send_to_char (buf, ch);
	return;
}

/** Function: list_group_chosen
  * Descr   : display the list of skills chosen so far by ch.
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.1 2/98
  * Author  : Rom Consortium. Last Update by Gary McNickle
  */
void list_group_chosen (CHAR_DATA * ch)
{
	char buf[MSL];
	int gn, sn, col;
	
	if (IS_NPC (ch))
		return;
	
	if (can_channel(ch))
	{
		send_to_char ("spheres\n\r",ch);
		sprintf (buf, "`Cair`w %-2d  `Nearth`w %-2d  `Rfire`w %-2d  `Bwater`w %-2d  `Wspirit`w %-2d\n\r\n\r",
			ch->flow_str.perm[AIR],
			ch->flow_str.perm[EARTH],
			ch->flow_str.perm[FIRE],
			ch->flow_str.perm[WATER],
			ch->flow_str.perm[SPIRIT]);
		send_to_char (buf, ch);
	}

	col = 0;

	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s",
		"talent", "cp", "talent", "cp", "talent", "cp\n\r");
	
	send_to_char (buf, ch);
	
	for (gn = 0; gn < MAX_TALENTS; gn++)
	{
		if (talent_table[gn].name == NULL)
			break;
		
		if (ch->gen_data->talent_chosen[gn])
		{
			sprintf (buf, "%-18s %-5d ",
				talent_table[gn].name, talent_table[gn].rating);
			
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	
	col = 0;
	
	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s",
		"group", "cp", "group", "cp", "group", "cp\n\r");
	
	send_to_char (buf, ch);
	
	for (gn = 0; gn < maxGroup; gn++)
	{
		if (group_table[gn].name == NULL)
			break;
		
		if (ch->gen_data->group_chosen[gn])
		{
			sprintf (buf, "%-18s %-5d ",
				group_table[gn].name, group_table[gn].rating);
			
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	col = 0;
	
	sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s",
		"skill", "cp", "skill", "cp", "skill", "cp\n\r");
	
	send_to_char (buf, ch);
	
	for (sn = 0; sn < maxSkill; sn++)
	{
		if (skill_table[sn].name == NULL)
			break;
		
		if (GET_SKILL (ch, sn) > 0)
		{
			sprintf (buf, "%-18s %-5d ",
				skill_table[sn].name, skill_table[sn].rating);
			
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
	}
	
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
	
	send_to_char ("\n\r", ch);
	
	sprintf (buf, "Creation points: %d\n\r", ch->gen_data->points_chosen);
	send_to_char (buf, ch);
	sprintf (buf, "Experience per level: %ld\n\r",
		exp_per_level (ch, ch->gen_data->points_chosen));
	send_to_char (buf, ch);
	return;
}

long exp_per_level (CHAR_DATA * ch, int points)
{
	long expl, inc;
	
	if (IS_NPC (ch))
		return 1000;
	
	if (IS_IMMORTAL (ch))
		return -1;
	
	expl = 1000;
	inc = 500;
	
	if (points < 40)
		return 1000 * (pc_race_table[ch->race].class_mult[ch->c_class] ?
		pc_race_table[ch->race].class_mult[ch->c_class] / 100 : 1);
	
	/* processing */
	points -= 40;
	
	while (points > 9)
	{
		expl += inc;
		points -= 10;
		if (points > 9)
		{
			expl += inc;
			inc *= 2;
			points -= 10;
		}
	}
	
	expl += points * inc / 10;
	
	return expl * pc_race_table[ch->race].class_mult[ch->c_class] / 100;
}

/** Function: parse_gen_groups
  * Descr   : Handles the input (from nanny) parsing for the skill
  *         : generator.
  * Returns : TRUE/FALSE on valid command/argument
  * Syntax  : parse_gen_groups [add/drop/list/info/done/help/premise/learned] 
  *         : (skill/group)
  * Written : v1.2 02/1998
  * Author  : ROM consortium. v1.2 Update by Gary McNickle
  */
bool parse_gen_groups (CHAR_DATA * ch, char *argument)
{
	return FALSE;
}

/* shows all groups, or the sub-members of a group */
_DOFUN (do_groups)
{
	char buf[100];
	int gn, sn, col;
	
	if (IS_NPC (ch))
		return;
	
	col = 0;
	
	if ( IS_NULLSTR(argument) || !str_cmp(argument,"all") )
	{				/* show all groups */
		
		for (gn = 0; gn < maxGroup; gn++)
		{
			if (group_table[gn].name == NULL)
				break;

			if (knows_group(ch, gn))
			{
				sprintf (buf, "%-20s ", group_table[gn].name);
				send_to_char (buf, ch);
				if (++col % 3 == 0)
					send_to_char ("\n\r", ch);
			}
		}
		
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
		
		sprintf (buf, "Creation points: %d\n\r", ch->pcdata->points);
		send_to_char (buf, ch);
		return;
	}
	
	if (!str_cmp (argument, "all"))	/* show all groups */
	{
		for (gn = 0; gn < maxGroup; gn++)
		{
			if (group_table[gn].name == NULL)
				break;
			sprintf (buf, "%-20s ", group_table[gn].name);
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
		return;
	}
	
	
	/* show the sub-members of a group */
	gn = group_lookup (argument);
	if (gn == -1)
	{
		send_to_char ("No group of that name exist.\n\r", ch);
		send_to_char ("Type 'groups all' or 'info all' for a full listing.\n\r",
			ch);
		return;
	}
	
	for (sn = 0; sn < MAX_IN_GROUP; sn++)
	{
		if (group_table[gn].spells[sn] == NULL)
			break;
		sprintf (buf, "%-20s ", group_table[gn].spells[sn]);
		send_to_char (buf, ch);
		if (++col % 3 == 0)
			send_to_char ("\n\r", ch);
	}
	if (col % 3 != 0)
		send_to_char ("\n\r", ch);
}

/* checks for skill improvement */
void check_improve (CHAR_DATA * ch, int sn, bool success, int multiplier)
{
	int chance;
	char buf[100];
	
	if (IS_NPC (ch))
		return;
	
	if (ch->level < skill_table[sn].skill_level[ch->c_class]
		|| skill_table[sn].rating < 1
		|| GET_SKILL (ch, sn) == 0 || GET_SKILL (ch, sn) == 100)
		return;			/* skill is not known */
	
	/* check to see if the character has a chance to learn */
	chance = 10 * int_app[get_curr_stat (ch, STAT_INT)].learn;
	chance /= (multiplier * skill_table[sn].rating * 4);
	chance += ch->level;
	
	if (number_range (1, 1000) > chance)
		return;
	
	/* now that the character has a CHANCE to learn, see if they really have */
	
	if (success)
	{
		chance = URANGE (5, 100 - GET_SKILL (ch, sn), 95);
		if (number_percent () < chance)
		{
			int gain = 0;
			sprintf (buf, "You have become better at %s!\n\r",
				skill_table[sn].name);
			send_to_char (buf, ch);
			INCREASE_SKILL (ch, sn, 1);
			gain = 2 * skill_table[sn].rating;
			if ( pMud.exp_mod > 0.50 && gain > 0 )
				gain *= pMud.exp_mod;		
			gain_exp (ch, gain);
		}
	}
	
	else
	{
		chance = URANGE (5, GET_SKILL (ch, sn) / 2, 30);
		if (number_percent () < chance)
		{
			int gain = 0;
			sprintf (buf,
				"You learn from your mistakes, and your %s skill improves.\n\r",
				skill_table[sn].name);
			send_to_char (buf, ch);
			INCREASE_SKILL (ch, sn, number_range (1, 3));
			SET_SKILL (ch, sn, UMIN (GET_SKILL (ch, sn), 100));
			gain = 2 * skill_table[sn].rating;
			if ( pMud.exp_mod > 0.50 && gain > 0 )
				gain *= pMud.exp_mod;		
			gain_exp (ch, gain);
		}
	}
	
}


/* recursively adds a group given its number -- uses group_add */
void gn_add (CHAR_DATA * ch, int gn)
{
	int i;

	if ( gn > ch->pcdata->iMemSize[MEM_MAXGROUP] || gn < 0 )
		return;

	ch->pcdata->group_known[gn] = TRUE;
	
	for (i = 0; i < MAX_IN_GROUP; i++)
	{
		if (group_table[gn].spells[i] == NULL)
			break;
		
		group_add (ch, group_table[gn].spells[i], FALSE);
	}
	
}

/* recusively removes a group given its number -- uses group_remove */
void gn_remove (CHAR_DATA * ch, int gn)
{
	int i;
	
	if ( gn > ch->pcdata->iMemSize[MEM_MAXGROUP] || gn < 0 )
		return;

	ch->pcdata->group_known[gn] = FALSE;
	
	for (i = 0; i < MAX_IN_GROUP; i++)
	{
		if (group_table[gn].spells[i] == NULL)
			break;
		group_remove (ch, group_table[gn].spells[i]);
	}
}

/* use for processing a skill or group for addition  */
void group_add (CHAR_DATA * ch, const char *name, bool deduct)
{
	int sn, gn, tn;
	
	if (IS_NPC (ch))		/* NPCs do not have skills */
		return;
	
	sn = skill_lookup (name);
	
	if (sn == gsn_seize && ch->pcdata->true_sex == SEX_FEMALE)
		sn = gsn_embrace;
	else if (sn == gsn_embrace && ch->pcdata->true_sex == SEX_MALE)
		sn = gsn_seize;
	
	if (!str_prefix (country_lookup (ch->pcdata->nation), "Threefold Land") 
	&& sn == skill_lookup("sword"))
	{
		sn = skill_lookup("spear");
	}
	
	if (IS_VALID_SKILL(sn))
	{
		if (GET_SKILL (ch, sn) < 1)	/* i.e. not known */
		{
			SET_SKILL (ch, sn, 1);
			if (deduct)
				ch->pcdata->points += skill_table[sn].rating;
		}
		return;
	}
	
	/* now check groups */
	
	gn = group_lookup (name);
	
	if (gn != -1 && gn <= ch->pcdata->iMemSize[MEM_MAXGROUP] )
	{
		if (!knows_group(ch, gn))
		{
			ch->pcdata->group_known[gn] = TRUE;

		}
		gn_add (ch, gn);		/* make sure all skills in the group are known */
	}
	
	/* now talents */
	
	tn = talent_lookup (name);
	
	if (tn != -1)
	{
		if (ch->pcdata->talents[gn] == FALSE)
		{
			ch->pcdata->talents[tn] = TRUE;
		}
	}
	
}

/* used for processing a skill or group for deletion -- no points back! */

void group_remove (CHAR_DATA * ch, const char *name)
{
	int sn, gn;
	
	sn = skill_lookup (name);
	
	if (IS_VALID_SKILL(sn))
	{
		SET_SKILL (ch, sn, 0);
		return;
	}
	
	/* now check groups */
	
	gn = group_lookup (name);
	
	if ( knows_group(ch, gn) )
	{
		ch->pcdata->group_known[gn] = FALSE;
		gn_remove (ch, gn);		/* be sure to call gn_add on all remaining groups */
	}
}

/** Function: do_train
  * Descr   : function to let player spend trains on stats
  * Returns : void
  * Syntax  : train ( [stat] )
  * Written : v1.0 9/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_train)
{
	char buf[MSL];
	CHAR_DATA *mob;
	sh_int stat = -1;
	char *pOutput = NULL;
	int cost;
	
	if (IS_NPC (ch))
		return;
	
		/*
		* Check for trainer.
	*/
	for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
	{
		if (IS_NPC (mob) && IS_FLAG (mob->states.player, _ACT_TRAIN))
			break;
	}
	
	if (mob == NULL)
	{
		send_to_char ("You can't do that here.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		sprintf (buf, "You have %d training sessions.\n\r", ch->train);
		send_to_char (buf, ch);
		argument = "foo";
	}
	
	cost = 1;
	
	if (!str_cmp (argument, "str"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_STR)
			cost = 1;
		stat = STAT_STR;
		pOutput = "strength";
	}
	
	else if (!str_cmp (argument, "int"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_INT)
			cost = 1;
		stat = STAT_INT;
		pOutput = "intelligence";
	}
	
	else if (!str_cmp (argument, "agl"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_AGL)
			cost = 1;
		stat = STAT_AGL;
		pOutput = "agility";
	}
	
	else if (!str_cmp (argument, "dex"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_DEX)
			cost = 1;
		stat = STAT_DEX;
		pOutput = "dexterity";
	}
	
	else if (!str_cmp (argument, "con"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_CON)
			cost = 1;
		stat = STAT_CON;
		pOutput = "constitution";
	}

	else if (!str_cmp (argument, "sen"))
	{
		if (class_table[ch->c_class].attr_prime == STAT_SEN)
			cost = 1;
		stat = STAT_SEN;
		pOutput = "senses";
	}
	
	else if (!str_cmp (argument, "hp") || !str_cmp (argument, "move"))
		cost = 1;
	
	else if (is_name (argument, "air earth water fire spirit"))
		cost = 10;
	/// change here	at some time to flex cost so high points are not cheap
	else
	{
		strcpy (buf, "You can train:");
		strcat (buf, " hp");
		strcat (buf, " move");
		if (ch->perm_stat[STAT_STR] < get_max_train (ch, STAT_STR))
			strcat (buf, " str");
		if (ch->perm_stat[STAT_INT] < get_max_train (ch, STAT_INT))
			strcat (buf, " int");
		if (ch->perm_stat[STAT_AGL] < get_max_train (ch, STAT_AGL))
			strcat (buf, " wis");
		if (ch->perm_stat[STAT_DEX] < get_max_train (ch, STAT_DEX))
			strcat (buf, " dex");
		if (ch->perm_stat[STAT_CON] < get_max_train (ch, STAT_CON))
			strcat (buf, " con");
		if (can_channel (ch))
		{
			if (ch->flow_str.perm[AIR] < MAXFLOWSTR (ch))
				strcat (buf, " air");
			if (ch->flow_str.perm[EARTH] < MAXFLOWSTR (ch))
				strcat (buf, " earth");
			if (ch->flow_str.perm[FIRE] < MAXFLOWSTR (ch))
				strcat (buf, " fire");
			if (ch->flow_str.perm[WATER] < MAXFLOWSTR (ch))
				strcat (buf, " water");
			if (ch->flow_str.perm[SPIRIT] < MAXFLOWSTR (ch))
				strcat (buf, " spirit");
		}
		if (buf[strlen (buf) - 1] != ':')
		{
			strcat (buf, ".\n\r");
			send_to_char (buf, ch);
		}
		else
		{
			act ("There is nothing more you can improve.",
				ch, NULL, NULL, TO_CHAR, POS_RESTING);
		}
		
		return;
	}
	
	if (!str_cmp ("hp", argument))
	{
		if (cost > ch->train)
		{
			send_to_char ("You don't have enough training sessions.\n\r", ch);
			return;
		}
		
		ch->train -= cost;
		ch->pcdata->perm_hit += 10;
		ch->max_hit += 10;
		ch->hit += 10;
		act ("Your durability increases!", ch, NULL, NULL, TO_CHAR, POS_RESTING);
		act ("$n's durability increases!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		return;
	}

	if (!str_cmp ("move", argument))
	{
		if (cost > ch->train)
		{
			send_to_char ("You don't have enough training sessions.\n\r", ch);
			return;
		}
		
		ch->train -= cost;
		ch->pcdata->perm_move += 20;
		ch->move += 20;
		ch->max_move += 20;
		act ("Your stamina increases!", ch, NULL, NULL, TO_CHAR, POS_RESTING);
		act ("$n's stamina increases!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if (!str_prefix ("air", argument) || !str_prefix ("earth", argument) ||
		!str_prefix ("water", argument) || !str_prefix ("fire", argument) ||
		!str_prefix ("spirit", argument))
	{
		enum _FLOW flow = -1;
		
		if (OPR (ch) < 1)
		{
			send_to_char ("But you can't channel!\n\r", ch);
			return;
		}
		
		if (cost > ch->train)
		{
			send_to_char ("You don't have enough training sessions.\n\r", ch);
			return;
		}
		
		switch (*argument)
		{
		case 'A':
		case 'a':
			flow = AIR;
			break;
		case 'E':
		case 'e':
			flow = EARTH;
			break;
		case 'W':
		case 'w':
			flow = WATER;
			break;
		case 'F':
		case 'f':
			flow = FIRE;
			break;
		case 'S':
		case 's':
			flow = SPIRIT;
			break;
		}
		
		if (ch->flow_str.perm[flow] >= MAXFLOWSTR (ch))
		{
			chprintf (ch, "Your have already reached your maximum"
				" potential in that flow!");
			return;
		}
		
		ch->flow_str.perm[flow] += 1;
		ch->flow_str.current[flow] += 1;
		
		ch->train -= cost;
		
		act ("Your strength in The Power increases!",
			ch, NULL, NULL, TO_CHAR, POS_RESTING);
		act ("$n's strength in the Power increases!",
			ch, NULL, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if (ch->perm_stat[stat] >= get_max_train (ch, stat))
	{
		act ("Your $T is already at maximum.",
			ch, NULL, pOutput, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (cost > ch->train)
	{
		send_to_char ("You don't have enough training sessions.\n\r", ch);
		return;
	}
	
	ch->train -= cost;
	
	ch->perm_stat[stat] += 1;
	act ("Your $T increases!", ch, NULL, pOutput, TO_CHAR, POS_RESTING);
	act ("$n's $T increases!", ch, NULL, pOutput, TO_ROOM, POS_RESTING);
	return;
}

/* used to get new skills */
_DOFUN (do_gain)
{
	char buf[MSL];
	char arg[MIL];
	CHAR_DATA *trainer;
	int gn = 0, sn = 0;
	
	if (IS_NPC (ch))
		return;
	
	/* find a trainer */
	for (trainer = ch->in_room->people;
	trainer != NULL; trainer = trainer->next_in_room)
		if (IS_NPC (trainer) && IS_FLAG (trainer->states.player, _ACT_GAIN))
			break;
		
		if (trainer == NULL || !can_see (ch, trainer))
		{
			send_to_char ("You can't do that here.\n\r", ch);
			return;
		}
		
		one_argument (argument, arg);
		
		if (arg[0] == '\0')
		{
			do_say (trainer, "Pardon me?");
			return;
		}
		
		if (!str_prefix (arg, "list"))
		{
			int col;
			int found=0;
			
			col = 0;
			
			for (gn = 0; gn < maxGroup; gn++)
			{
				if (group_table[gn].name == NULL)
					break;
				
				if (!knows_group(ch, gn) &&
					!bad_selection (ch, GROUP_TABLE, gn, FALSE))
				{
					if (found == 0);
					{
						found=1;
						sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
							"group", "cost", "group", "cost", "group", "cost");
						send_to_char (buf, ch);
					}
					sprintf (buf, "%-18s %-5d ",
						group_table[gn].name, group_table[gn].rating);
					send_to_char (buf, ch);
					if (++col % 3 == 0)
						send_to_char ("\n\r", ch);
				}
			}
			if (col % 3 != 0)
				send_to_char ("\n\r", ch);
			send_to_char ("\n\r", ch);
			
			col = 0;
			found = 0;

			for (sn = 1; sn < maxSkill; sn++)
			{
				if (skill_table[sn].name == NULL)
					break;
				
				if (GET_SKILL (ch, sn) < 1
					&& skill_table[sn].weave_fun == NULL
					&& !bad_selection (ch, SKILL_TABLE, sn, FALSE))
				{
				  if (found == 0)
					{
						found = 1;
						sprintf (buf, "%-18s %-5s %-18s %-5s %-18s %-5s\n\r",
							"skill", "cost", "skill", "cost", "skill", "cost");
						send_to_char (buf, ch);
					}
					sprintf (buf, "%-18s %-5d ",
						skill_table[sn].name, skill_table[sn].rating);
					send_to_char (buf, ch);
					if (++col % 3 == 0)
						send_to_char ("\n\r", ch);
				}
			}
			if (col % 3 != 0)
				send_to_char ("\n\r", ch);
			if (found==0)
				send_to_char ("There are no skills available to gain at your current level.\n\r\n\r", ch);
			else
				send_to_char ("\n\r", ch);
			return;
		} /* end: list */
		
		if (!str_prefix (arg, "convert"))
		{
			if (ch->practice < 10)
			{
				act ("$N tells you 'You are not yet ready.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			act ("$N helps you apply your practice to training",
				ch, NULL, trainer, TO_CHAR, POS_RESTING);
			ch->practice -= 10;
			ch->train += 1;
			return;
		}
		
		if (!str_prefix (arg, "practice"))
		{
			if (ch->train < 1)
			{
				act ("$N tells you 'You are not yet ready.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			act ("$N helps you apply your training to your skills",
				ch, NULL, trainer, TO_CHAR, POS_RESTING);
			ch->practice += 10;
			ch->train -= 1;
			return;
		}
		
		if (!str_prefix (arg, "points"))
		{
			if (ch->train < 2)
			{
				act ("$N tells you 'You are not yet ready.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			if (ch->pcdata->points <= 40)
			{
				act ("$N tells you 'There would be no point in that.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			act ("$N trains you, and you feel more at ease with your skills.",
				ch, NULL, trainer, TO_CHAR, POS_RESTING);
			
			ch->train -= 2;
			ch->pcdata->points -= 1;
			ch->exp = exp_per_level (ch, ch->pcdata->points) * ch->level;
			return;
		}
		
		/* else add a group/skill */
		
		gn = talent_lookup (argument);
		if (gn != -1)
		{
			send_to_char ("Talent and specialties are something learned at a"
				"very young age. They cannot be taught.\n\r", ch);
			return;
		}
		
		gn = group_lookup (argument);
		if (gn > 0)
		{

			if (bad_selection (ch, GROUP_TABLE, gn, TRUE))
				return;
			
			if (group_table[gn].rating < 1)
			{
				act ("$N tells you 'That group is beyond your powers.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			if (ch->train < group_table[gn].rating)
			{
				act ("$N tells you 'You are not yet ready for that group.'",
					ch, NULL, trainer, TO_CHAR, POS_RESTING);
				return;
			}
			
			/* add the group */
			gn_add (ch, gn);
			act ("$N trains you in the art of $t",
				ch, group_table[gn].name, trainer, TO_CHAR, POS_RESTING);
			return;
		}
		
		sn = skill_lookup (argument);
		if ( IS_VALID_SKILL(sn) )
		{
				
				if (bad_selection (ch, SKILL_TABLE, sn, TRUE))
					return;


				if (skill_table[sn].rating < 1)
				{
					act ("$N tells you 'That skill is beyond your powers.'",
						ch, NULL, trainer, TO_CHAR, POS_RESTING);
					return;
				}
				
				if (ch->train < skill_table[sn].rating)
				{
					act ("$N tells you 'You are not yet ready for that skill.'",
						ch, NULL, trainer, TO_CHAR, POS_RESTING);
					return;
				}
				
				/* add the skill */
				SET_SKILL (ch, sn, 1);
				act ("$N trains you in the art of $t",
					ch, skill_table[sn].name, trainer, TO_CHAR, POS_RESTING);
				ch->train -= skill_table[sn].rating;
				return;
		}
		
		act ("$N tells you 'I do not understand...'",
			ch, NULL, trainer, TO_CHAR, POS_RESTING);
}
