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
 * @@ Objective (gain.c)
 *
 * Contains support code related to player advancement. The checks to see
 * if a player gains experience, as well as how much, etc, are located here.
 * the "do_gain" function is located elsewhere. (skill_h.c)
 */




#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "music.h"



/*
 * Advancement stuff.
 */
void advance_level (CHAR_DATA * ch, bool hide, bool gainLevel)
{
  char buf[MSL];
  int add_hp;
  int add_move;
  int add_prac;

  ch->pcdata->last_level =
    (ch->played + (int) (current_time - ch->logon)) / 3600;

  add_hp =
    con_app[get_curr_stat (ch, STAT_CON)].hitp +
    number_range (class_table[ch->c_class].hp_min, 
		  class_table[ch->c_class].hp_max);

  add_move = number_range (1, (get_curr_stat (ch, STAT_CON)
			       + get_curr_stat (ch, STAT_DEX)) / 6);

  add_prac = wis_app[get_curr_stat (ch, STAT_AGL)].practice;

  add_hp = add_hp * 9 / 10;
  add_move = add_move * 9 / 10;
  add_hp = UMAX (2, add_hp);
  add_move = UMAX (6, add_move);

  if (!gainLevel)
  {
    add_hp /= 2;
    add_move /= 2;
    add_prac /= 2;
  }

  ch->max_hit += add_hp;
  ch->max_move += add_move;
  ch->practice += add_prac;
  ch->train += 1;
  ch->pcdata->perm_hit += add_hp;
  ch->pcdata->perm_move += add_move;

  if (!hide)
  {
    sprintf (buf,
	     "You gain %d hit point%s, %d Endurance,"
	     " 1 train, and %d practice%s.\n\r",
	     add_hp, add_hp == 1 ? "" : "s", add_move,
	     add_prac, add_prac == 1 ? "" : "s");
    send_to_char (buf, ch);
  }
  return;
}



void gain_exp (CHAR_DATA * ch, int gain)
{
	int loops = 0;
	char buf[MSL];
	long xpl;
	
	if (IS_NPC (ch) || ch->level >= LEVEL_HERO)
		return;
	
	xpl = exp_per_level (ch, ch->pcdata->points);
	
	ch->exp = UMAX (xpl, ch->exp + gain);
	
	while (ch->level < HE - 1 && ch->exp >= xpl * (ch->level + 1))
	{
		if (++loops == 1)
		{
			snd_trigger (ch, ch->sounds, SOUND_MOBILE, SOUND_MOB_LEVEL, TRUE);
			send_to_char ("You raise a level!!  ", ch);
		}
		
		ch->level += 1;
		logf("%s gained level %d", ch->name, ch->level);

		sprintf (buf, "$N has attained level %d!", ch->level);
		wiznet (buf, ch, NULL, WIZ_LEVELS, 0, 0);
		advance_level (ch, FALSE, TRUE);
		
		if (ch->level >= pMud.level_no_peace && ch->pcdata->peaceful)
		{
			send_to_char ("Your peaceful days are over!\n\r", ch);
			send_to_char ("Your corpse can now be looted, and also\n\r", ch);
			send_to_char ("you can now kill and be killed, by players.\n\r", ch);
			
			ch->pcdata->peaceful = FALSE;
			if (!IS_FLAG (ch->states.player, _CANLOOT))
				SET_FLAG (ch->states.player, _CANLOOT);
		}
		
		save_char_obj (ch);
	}
	
	return;
}

/** Function: hit_gain
  * Descr   : Regenerates a characters hit points.
  * Returns : (void)
  * Syntax  : (void/automatic)
  * Written : v1.1 3/98
  * Author  : ROM Consortium. updated by Gary McNickle <gary@tarmongaidon.org>
  */
int hit_gain (CHAR_DATA * ch)
{
	int gain;
	int number;
	
	if (ch->in_room == NULL)
		return 0;
	
	if (IS_NPC (ch))
	{
		gain = 5 + ch->level;
		if (IS_AFFECTED (ch, _AFF_REGENERATION))
			gain *= 2;
		
		switch (ch->position)
		{
			default:
				gain /= 2;
				break;
			case POS_SLEEPING:
				gain = 3 * gain / 2;
				break;
			case POS_RESTING:
				break;
			case POS_FIGHTING:
				gain /= 3;
				break;
		}
		
		
	}
	else
	{
		gain = UMAX (10, get_curr_stat (ch, STAT_CON) + ch->level);
		gain += class_table[ch->c_class].hp_max - 10;
		number = number_percent ();
		if (number < get_skill (ch, gsn_fast_healing))
		{
			gain += get_curr_stat (ch, STAT_CON) + number * gain / 100;
			
			/* Bonded Warders Heal much faster... */
			if (ch != NULL
				&& !IS_NPC (ch)
				&& IS_FLAG (clan_table[ch->clan].flags, _GAIDIN)
				&& ch->pcdata->bonds[0] != 0)
			{
				gain += UMAX (3, get_curr_stat (ch, STAT_CON) - 3 + ch->level / 4);
				send_to_char ("You gain strength from your bond.\n\r", ch);
			}
			
			if (ch->hit < ch->max_hit)
				check_improve (ch, gsn_fast_healing, TRUE, 8);
		}
		
		switch (ch->position)
		{
			default:
				gain /= 4;
				break;
			case POS_SLEEPING:
				break;
			case POS_RESTING:
				gain /= 2;
				break;
			case POS_FIGHTING:
				gain /= 6;
				break;
		}
		
		if (ch->pcdata->condition[_HUNGER] == 0)
			gain /= 2;
		
		if (ch->pcdata->condition[_THIRST] == 0)
			gain /= 2;
		
	}
	
	gain = gain * ch->in_room->heal_rate / 100;
	
	if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[3] / 100;
	
	if (IS_AFFECTED (ch, _AFF_POISON))
		gain /= 4;
	
	if (IS_AFFECTED (ch, _AFF_PLAGUE))
		gain /= 8;
	
	if (IS_AFFECTED (ch, _AFF_SLOW))
		gain /= 2;
	
	return UMIN (gain, ch->max_hit - ch->hit);
}



int opu_gain (CHAR_DATA * ch)
{
	int gain;
	int number;
	
	if (ch->in_room == NULL)
		return 0;
	
	if (IS_NPC (ch))
	{
		gain = 5 + ch->level;
		switch (ch->position)
		{
			default:
				gain /= 2;
				break;
			case POS_SLEEPING:
				gain = 3 * gain / 2;
				break;
			case POS_RESTING:
				break;
			case POS_FIGHTING:
				gain /= 3;
				break;
		}
	}
	else
	{
		gain = (get_curr_stat (ch, STAT_AGL)
			+ get_curr_stat (ch, STAT_INT) + ch->level) / 2;
		number = number_percent ();
		if (number < get_skill (ch, gsn_meditation))
		{
			gain += get_curr_stat (ch, STAT_CON) + number * gain / 100;
			check_improve (ch, gsn_meditation, TRUE, 8);
		}
		
		switch (ch->position)
		{
			default:
				gain /= 4;
				break;
			case POS_SLEEPING:
				break;
			case POS_RESTING:
				gain /= 2;
				break;
			case POS_FIGHTING:
				gain /= 6;
				break;
		}
		
		if (ch->pcdata->condition[_HUNGER] == 0)
			gain /= 2;
		
		if (ch->pcdata->condition[_THIRST] == 0)
			gain /= 2;
		
	}
	
	gain = gain * ch->in_room->opu_rate / 100;
	
	if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[4] / 100;
	
	if (IS_AFFECTED (ch, _AFF_POISON))
		gain /= 4;
	
	if (IS_AFFECTED (ch, _AFF_PLAGUE))
		gain /= 8;
	
	if (IS_AFFECTED (ch, _AFF_HASTE) || IS_AFFECTED (ch, _AFF_SLOW))
		gain /= 2;
	
	return UMIN (gain, OPR (ch) - TAU (ch)) / 10;
}



int move_gain (CHAR_DATA * ch)
{
	int gain;
	int number;
	
	if (ch->in_room == NULL)
		return 0;
	
	if (IS_NPC (ch))
	{
		gain = ch->level;
	}
	else
	{
		gain = UMAX (15, ch->level) + get_curr_stat (ch, STAT_CON);
		
		switch (ch->position)
		{
			case POS_SLEEPING:
				gain += get_curr_stat (ch, STAT_DEX);
				break;
			case POS_RESTING:
				gain += get_curr_stat (ch, STAT_DEX) / 2;
				break;
			default:
				break;
		}
		
		number = number_percent ();
		if (number < get_skill (ch, gsn_meditation))
		{
			gain += number * gain / 100;
			check_improve (ch, gsn_meditation, TRUE, 8);
		}
		
		if (ch->pcdata->condition[_HUNGER] == 0)
			gain /= 2;
		
		if (ch->pcdata->condition[_THIRST] == 0)
			gain /= 2;
	}
	
	gain = gain * ch->in_room->heal_rate / 100;
	
	if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
		gain = gain * ch->on->value[3] / 100;
	
	if (IS_AFFECTED (ch, _AFF_POISON))
		gain /= 4;
	
	if (IS_AFFECTED (ch, _AFF_PLAGUE))
		gain /= 8;
	
	if (IS_AFFECTED (ch, _AFF_HASTE) || IS_AFFECTED (ch, _AFF_SLOW))
		gain /= 2;
	
	return UMIN (gain, ch->max_move - ch->move);
}


void gain_condition (CHAR_DATA * ch, int iCond, int value)
{
	int condition;
	
	if (value == 0 || IS_NPC (ch) || ch->level >= LEVEL_IMMORTAL)
		return;
	
	condition = ch->pcdata->condition[iCond];
	
	if (condition == -1)
		return;
	
	ch->pcdata->condition[iCond] = URANGE (0, condition + value, 48);
	
	
	if (IS_AFFECTED (ch, _AFF_HASTE)
		&& (iCond == _HUNGER || iCond == _THIRST))
	{
		ch->pcdata->condition[iCond] *= .4;
	}
	
	if (ch->pcdata->condition[iCond] == 0)
	{
		switch (iCond)
		{
			case _HUNGER:
				if (!IS_FLAG (ch->states.config, _NOHUNGER))
					send_to_char ("You are hungry.\n\r", ch);
				break;
				
			case _THIRST:
				if (!IS_FLAG (ch->states.config, _NOHUNGER))
					send_to_char ("You are thirsty.\n\r", ch);
				break;
				
			case _DRUNK:
				if (condition != 0)
					send_to_char ("You are sober.\n\r", ch);
				break;
		}
	}
	
	return;
}
