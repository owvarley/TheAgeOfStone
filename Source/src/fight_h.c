/** 
 * @@ Copyright (The Age of Stone Project)
 *
 * The Age of Stone (TaoS): Version v0.1 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

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

/*
 * fight_h.c
 *
 * This file contains the various support functions needed in combat. 
 * Generally, functions needed during combat that are not skills, and are
 * not callable functions by the player, are placed here. Although some
 * skill support code is also here, such as check_assist or check_parry.
 */

/* 
 * Edited to remove all old WoTmud combat functions replaced with functions for
 * Taos's new combat system.
 */



#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update (void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *victim;
	
	for (ch = char_list; ch != NULL; ch = ch_next)
	{
		ch_next = ch->next;
		
		if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
			continue;
		
		if (IS_AWAKE (ch) && ch->in_room == victim->in_room && in_same_world(ch, victim) )
		{
				// New initiative check, to see who gets the first attack in.
				int char_int = calculate_initiative( ch );
				int vict_int = calculate_initiative( victim );

				// Stamina penalties of combat
				victim->move--;
				ch->move--;
 
				if (char_int > vict_int)
				{
					make_attack (ch, victim, TYPE_UNDEFINED);
					make_attack (victim, ch, TYPE_UNDEFINED);
					act ("$n gets first attack on $N!", victim, NULL, ch, TO_ROOM, POS_RESTING);
				}
				if (vict_int > char_int)
				{ 
					make_attack (victim, ch, TYPE_UNDEFINED);
					make_attack (ch, victim, TYPE_UNDEFINED);
					act ("$N gets first attack on $n!", victim, NULL, ch, TO_ROOM, POS_RESTING);
				}
					
		}
		else
		stop_fighting (ch, FALSE);
		
		if ((victim = ch->fighting) == NULL)
			continue;
		
		// Fun for the whole family!
		check_assist (ch, victim);
		
		if (IS_NPC (ch))
		{
			if (HAS_TRIGGER (ch, TRIG_FIGHT))
				mp_percent_trigger (ch, victim, NULL, NULL, TRIG_FIGHT);

			if (HAS_TRIGGER (ch, TRIG_HPCNT))
				mp_hprct_trigger (ch, victim);
		}
	}
	
	return;
}


/** Function: valid_level
  * Descr   : Validates that the victim is of suffecient level in relation to
  *         : the killer.
  * Returns : TRUE/FALSE if ch can kill victim
  * Syntax  : valid_level (ch, victim, report yesorno)
  * Written : v1.0 12/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool valid_level (CHAR_DATA * ch, CHAR_DATA * victim, bool report)
{
	return TRUE;
}


/** Function: can_fight
  * Descr   : Determines if ch can move enough to fight.
  * Returns : TRUE/FALSE on success
  * Syntax  : can_fight(ch, announce/report yes or no)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool can_fight (CHAR_DATA * ch, bool announce)
{
	if (IS_AFFECTED (ch, _AFF_BOUND_AIR))
	{
		if (announce)
		{
			send_to_char ("You cant move!\n\r", ch);
			act ("$n struggles against invisible bonds.",
				ch, NULL, NULL, TO_ALL, POS_RESTING);
		}
		return FALSE;
	}
	else
	if (IS_AFFECTED (ch, _AFF_EARTH_SHIELD))
	{
		if (announce)
		{
			send_to_char ("You are encased in earth!\n\r", ch);
		}
		return FALSE;
	}
	
	if (IS_AFFECTED (ch, _AFF_CAPTURED))
	{
		if (announce)
		{
			send_to_char ("Your bonds restrain you!\n\r", ch);
			act ("$n struggles against $s captivity.",
				ch, NULL, NULL, TO_ALL, POS_RESTING);
		}
		return FALSE;
	}
	
	return TRUE;
}


/* for auto assisting 
 * This function was kept in fight_h.c because it's only used by the
 * code and is not relative to any skill that the player can call.
 * (although, can be shut off with 'autoassist' toggle. [GM]
 */
void check_assist (CHAR_DATA * ch, CHAR_DATA * victim)
{
	CHAR_DATA *rch, *rch_next;
	
	for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
	{
		rch_next = rch->next_in_room;
		
		if (IS_AWAKE (rch) && rch->fighting == NULL)
		{
			
			if (RIDDEN (rch) == ch || MOUNTED (rch) == ch)
				multi_hit (rch, victim, TYPE_UNDEFINED);
			
			/* quick check for ASSIST_PLAYER */
			if (!IS_NPC (ch) && IS_NPC (rch)
				&& IS_FLAG (rch->states.off, _ACT_ASSIST_PLAYERS)
				&& rch->level + 6 > victim->level)
			{
				do_emote (rch, "screams and attacks!");
				multi_hit (rch, victim, TYPE_UNDEFINED);
				continue;
			}
			
			/* PCs next */
			if (!IS_NPC (ch) || IS_AFFECTED (ch, _AFF_COMPULSION))
			{
				if (((!IS_NPC (rch)
					&& IS_FLAG (rch->states.config, _AUTOASSIST))
					|| IS_AFFECTED (rch, _AFF_COMPULSION))
					&& is_same_group (ch, rch) && !is_safe (rch, victim))
				{
					multi_hit (rch, victim, TYPE_UNDEFINED);
				}
				continue;
			}
			
			/* now check the NPC cases */
			
			if (IS_NPC (ch) && !IS_AFFECTED (ch, _AFF_COMPULSION))
				
			{
				if ((IS_NPC (rch) && IS_FLAG (rch->states.off, _ACT_ASSIST_ALL))
				|| (IS_NPC (rch) && rch->group && rch->group == ch->group)
				|| (IS_NPC (rch) && rch->race == ch->race
				&& IS_FLAG (rch->states.off, _ACT_ASSIST_RACE))
				|| (IS_NPC (rch) && IS_FLAG (rch->states.off, _ACT_ASSIST_ALIGN)
				&& ((IS_GOOD (rch) && IS_GOOD (ch))
				|| (IS_EVIL (rch) && IS_EVIL (ch))
				|| (IS_NEUTRAL (rch) && IS_NEUTRAL (ch))))
				|| (rch->pIndexData == ch->pIndexData
				&& IS_FLAG (rch->states.off, _ACT_ASSIST_VNUM)))
				{
					CHAR_DATA *vch;
					CHAR_DATA *target;
					int number;
					
					if (number_bits (1) == 0)
						continue;
					
					target = NULL;
					number = 0;
					for (vch = ch->in_room->people; vch; vch = vch->next)
					{
						if (can_see (rch, vch)
							&& is_same_group (vch, victim)
							&& number_range (0, number) == 0)
						{
							target = vch;
							number++;
						}
					}
					
					if (target != NULL)
					{
						do_emote (rch, "screams and attacks!");
						multi_hit (rch, target, TYPE_UNDEFINED);
					}
				}
			}
		}
	}
}



/*
 * Do one group of attacks.
 */
void multi_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
	return;
}


// 
// Make Attack
// New function for making attacks
// Uses new damroll, hitroll and iniative values
//
void make_attack (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
	int roll, dex, weapon;
	int droll, str, mod;
	int sn, dtype;
	int hitroll = 0;
	int damroll = 0;
	int defence = 0;
		
	enum _WEAPON_CLASS wclass = WEAPON_UNDEFINED;

	OBJ_DATA *pObj = NULL;
	OBJ_DATA *shield = NULL;
		
	/* no attacks for not standing -- just a check */
	if (ch->position <= POS_RESTING)
		return;

	//First we roll the attackers hitroll
	//Hitroll is a 1d10 + DEX + 1d(Weapon Skill)

	pObj = get_eq_char(ch, WEAR_WIELD);
	
	if (pObj)
		wclass = (enum _WEAPON_CLASS)pObj->value[0];
	
	if ( dt == TYPE_UNDEFINED )
	{
		switch ( wclass )
		{
			case WEAPON_UNDEFINED:
			case WEAPON_EXOTIC:		sn = gsn_sword;		break;
			case WEAPON_SWORD:		sn = gsn_sword;		break;
			case WEAPON_DAGGER:		sn = gsn_dagger;	break;
			case WEAPON_SPEAR:		sn = gsn_spear;		break;
			case WEAPON_MACE:		sn = gsn_mace;		break;
			case WEAPON_AXE:		sn = gsn_axe;		break;
			case WEAPON_FLAIL:		sn = gsn_flail;		break;
			case WEAPON_WHIP:		sn = gsn_whip;		break;
			case WEAPON_POLEARM:	sn = gsn_polearm;	break;
			case WEAPON_STAFF:		sn = gsn_staff;		break;
			case WEAPON_BOW:		sn = gsn_bow;		break;
			default:				sn = gsn_bash;		break;
		}
	}

	weapon = skill_roll(victim, sn);
	roll = random_dice(1, 10, 0);
	dex = ch->perm_stat[STAT_DEX];
	//Add up the hitroll
	hitroll = roll + dex + weapon;

	//Getting the defences for the defender
	//Defences are: Parry, dodge, shield block
	//Defences are a 1d10 + AGL + 1d(Defence Skill)
	if (victim->pcdata->learned[gsn_parry] > 0
		&& victim->pcdata->learned[gsn_parry] <= 30)
	{
		defence = skill_roll(victim, gsn_parry);
		dtype = 1;
	}
	
	if (victim->pcdata->learned[gsn_dodge] > 0
		&& victim->pcdata->learned[gsn_dodge] <= 30)
	{
		defence = skill_roll(victim, gsn_dodge);
		dtype = 2;
	}
	
	if (victim->pcdata->learned[gsn_shield_block] > 0
		&& victim->pcdata->learned[gsn_shield_block] <= 30)
	{
		if (get_eq_char (victim, WEAR_SHIELD) == NULL)
		{
			defence = skill_roll(victim, gsn_shield_block);
			dtype = 3;
		}
	}

//	if (defence > 0)
//	{
//		defence += number_range(1, 10);
//		defence += victim->perm_stat[STAT_AGL];
//	}
	
	// Here the attack has been blocked
	// So we roll to find out how it was blocked
	if (defence > hitroll)
	{
			switch ( dtype )
			{
			case 1:			
				act ("You parry $N's attack.", victim, NULL, ch, TO_CHAR, POS_RESTING);
				act ("$n parries your attack.", victim, NULL, ch, TO_VICT, POS_RESTING);
				act ("$N parries $n's attack.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				break;
			case 2:
				act ("You dodge $N's attack.", victim, NULL, ch, TO_CHAR, POS_RESTING);
				act ("$n dodges your attack.", victim, NULL, ch, TO_VICT, POS_RESTING);
				act ("$N dodges $n's attack.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				break;
			case 3:
				shield = get_eq_char(victim, WEAR_SHIELD);
				if (shield)
				{
				act ("You block $N's attack with your shield.", victim, NULL, ch, TO_CHAR, POS_RESTING);
				act ("$n blocks your attack with $S shield.", victim, NULL, ch, TO_VICT, POS_RESTING);
				act ("$N blocks $n's attack with $S shield.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				}
				break;
			default:
				break;
			}
	}
	
	// Attack not blocked, time to deal out damage
	// Damroll is 1d10 + STR + MOD
	
	droll = random_dice(1, 10, 0);
	str = ch->perm_stat[STAT_STR];
	damroll = droll + str;

	//Now damage the victim
	apply_damage(ch, victim, damroll);

	return;


}

// 
// Apply_damage
// New function for applying damage to a player
// Uses new damroll with new modifiers
//

bool apply_damage (CHAR_DATA *ch, CHAR_DATA *victim, int damroll)
{
	

	// Apply the damage to the victim
	victim->hit -= damroll;

	if (victim->position > POS_STUNNED)
		{
			if (victim->fighting == NULL)
			{
				set_fighting (victim, ch);
				if (IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_KILL))
					mp_percent_trigger (victim, ch, NULL, NULL, TRIG_KILL);
			}
			if (victim->timer <= 2)
				victim->position = POS_FIGHTING;

			if (ch->fighting == NULL)
				set_fighting (ch, victim);
		}

	// Show the damage message to those present
	show_dammessage(ch, victim, damroll);

	update_pos (victim);
	
	switch (victim->position)
	{
			
		case POS_MORTAL:
			if (victim->wtimer == 0)
			{
				act ("$n falls to the ground, mortally wounded.",
					victim, NULL, NULL, TO_ROOM, POS_RESTING);
				send_to_char ("You crumple to the ground, mortally wounded.\n\r",
					victim);
				
				victim->wtimer = 1;
				stop_fighting (victim, TRUE);
				return TRUE;
			}
			break;
			
		case POS_INCAP:
			act ("$n is incapacitated and will slowly die, if not aided.",
				victim, NULL, NULL, TO_ROOM, POS_RESTING);
			send_to_char
				("You are incapacitated and will slowly die, if not aided.\n\r",
				victim);
			break;
			
		case POS_STUNNED:
			act ("$n is stunned, but will probably recover.",
				victim, NULL, NULL, TO_ROOM, POS_RESTING);
			send_to_char ("You are stunned, but will probably recover.\n\r", victim);
			break;
			
		case POS_DEAD:
			act ("$n is DEAD!!", victim, 0, 0, TO_ROOM, POS_RESTING);
			send_to_char ("You have been KILLED!!\n\r\n\r", victim);
			break;
			
		default:
			if (damroll > victim->max_hit / 4)
				send_to_char ("That really did HURT!\n\r", victim);
			if (victim->hit < victim->max_hit / 4)
				send_to_char ("You sure are BLEEDING!\n\r", victim);
			break;
	}

	return FALSE;
}

//
// Shows a damage message to the fighters
//

void show_dammessage(CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
	char buf1[256], buf2[256], buf3[256];
	// Variable definitions for the different parts of the message
	const char *bonus;
	const char *damage;
	const char *damage2;
	const char *bodypart;
	const char *weapon;
	const char *weapon2;
	// Weapon sn
	int sn;
	int roll, roll2;
	// Weapon data
    OBJ_DATA *wield;
  
 

	// Get the weapon skill
	sn = get_weapon_sn ( ch, FALSE );

	// Check whether they are good enough to get bonus messages
	if (ch->pcdata->learned[sn] >= 5)
	{
	   roll = number_range(1, 10);

	   // Decide on which message to give them, its random.
	   switch (roll)
	   {
	   case 1: bonus = "brutually";
			   break;
	   case 2: bonus = "savagely";
			   break;
	   case 3: bonus = "mercilessly";
			   break;
	   case 4: bonus = "powerfully";
			   break;
	   case 5: bonus = "viciously";
			   break;
	   case 6: bonus = "fiercely";
			   break;
	   case 7: bonus = "ferociously";
			   break;
	   case 8: bonus = "violently";
			   break;
	   case 9: bonus = "aggressively";
			   break;
	   case 10: bonus = "cruelly";
			   break;
	   }
	   
	}

	bonus = "";
	// Get the weapon type of the player
	wield = get_eq_char (ch, WEAR_WIELD);

	// What if they dont have a weapon?
  
	 if (wield != NULL || wield->item_type != ITEM_WEAPON)
	 {
	  // Roll to decide on dam message
	  int roll = number_range(1, ch->pcdata->learned[sn]);

	  switch (roll)
	  {
	  case 1: damage = "slam";
			  damage2 = "slams";
			  break;
	  case 2: damage = "thump";
			  damage2 = "thumps";
			  break;
	  case 3: damage = "smash";
			  damage2 = "smashes";
			  break;
	  case 4: damage = "pummel";
			  damage2 = "pummels";
			  break;
	  case 5: damage = "hook";
			  damage2 = "hooks";
			  break;
	  case 6: damage = "backhand";
			  damage2 = "backhands";
			  break;
	  case 7: damage = "whack";
			  damage2 = "whacks";
			  break;
	  case 8: damage = "wallop";
		      damage2 = "wallops";
			  break;
	  case 9: damage = "uppercut";
		      damage2 = "uppercuts";
			  break;
	 case 10: damage = "roundhouse";
		      damage2 = "roundhouses";
			  break;
	 default: damage = "ERROR";
		      damage2 = "ERROR";
			  break;
	  }
	
	  // Now we decide on the weapon message
	  roll2 = number_range(1, ch->pcdata->learned[sn]);
	  
	  switch (roll2)
	  {
	  case 1: weapon = "with your fists";
			  weapon2 = "with their fists";
			  break;
	  case 2: weapon = "with a swift punch";
			  weapon2 = "with a swift punch";
			  break;
	  case 3: weapon = "as your fist strikes hard!";
			  weapon2 = "as their fist strikes hard!";
			  break;
	  case 4: weapon = "as your fist makes contact";
			  weapon2 = "as their fist makes contact ";
			  break;
	  case 5: weapon = "with a well aimed punch";
			  weapon2 = "with a well aimed punch";
			  break;
	  case 6: weapon = "with a graceful blow";
			  weapon2 = "with a graceful blow";
			  break;
	  case 7: weapon = "as your clenched fist meets bone!";
			  weapon2 = "as their clenched fist meets bone! ";
			  break;
	  case 8: weapon = "with an incredibly powerful blow!";
			  weapon2 = "with an incredibly powerful blow!";
			  break;
	  case 9: weapon = "with a well timed, precise strike!";
			  weapon2 = "with a well timed, precise strike!";
			  break;
	 case 10: weapon = "with ease as your fist strike hard!";
			  weapon2 = "with ease as their fist strikes hard ";
			  break;
	  }


	}

	 if (wield->value[0] == WEAPON_SWORD)
	 {
	  // Roll to decide on dam message
	  int roll = number_range(1, ch->pcdata->learned[sn]);

	  switch (roll)
	  {
	  case 1: damage = "slash";
			  damage2 = "slashes";
			  break;
	  case 2: damage = "stab";
			  damage2 = "stabs";
			  break;
	  case 3: damage = "slice";
			  damage2 = "slices";
			  break;
	  case 4: damage = "jab";
			  damage2 = "jabs";
			  break;
	  case 5: damage = "impale";
			  damage2 = "impales";
			  break;
	  case 6: damage = "decimate";
			  damage2 = "decimates";
			  break;
	  case 7: damage = "devastate";
			  damage2 = "devastates";
			  break;
	  case 8: damage = "mutilate";
		      damage2 = "mutilates";
			  break;
	  case 9: damage = "eradicate";
		      damage2 = "eradicates";
			  break;
	 case 10: damage = "annihilate";
		      damage2 = "annihilates";
			  break;
	 default: damage = "ERROR";
		      damage2 = "ERROR";
			  break;
	  }
	
	  // Now we decide on the weapon message
	  roll2 = number_range(1, ch->pcdata->learned[sn]);
	  
	  switch (roll2)
	  {
	  case 1: weapon = "with your sword";
			  weapon2 = "with their sword";
			  break;
	  case 2: weapon = "with a rapid strike!";
			  weapon2 = "with a rapid strike!";
			  break;
	  case 3: weapon = "as your blade strikes flesh!";
			  weapon2 = "as their blade strikes flesh!";
			  break;
	  case 4: weapon = "as your blade's swinging arc strikes!";
			  weapon2 = "as their blade's swinging arc strikes!";
			  break;
	  case 5: weapon = "with a smooth accurate strike!";
			  weapon2 = "with a smooth accurate strike!";
			  break;
	  case 6: weapon = "as you swing your blade with all your might!";
			  weapon2 = "as they swing their blade with all their might!";
			  break;
	  case 7: weapon = "with a gracefull flowing strike!";
			  weapon2 = "with a gracefull flowing strike! ";
			  break;
	  case 8: weapon = "as you spin with your blade singing!";
			  weapon2 = "as they spin around with their blade singing!";
			  break;
	  case 9: weapon = "with an expertly timed strike!!";
			  weapon2 = "with an expertly timed srike!";
			  break;
	 case 10: weapon = "as you become one with your blade, flowing into attack!";
			  weapon2 = "as they seemly flow into the attack!";
			  break;
	  }


	}
 //   switch (wield->value[0])
 //   {
//		default:				sn = -1;			break;
//		case WEAPON_SWORD:		sn = gsn_sword;		break;
//		case WEAPON_DAGGER:		sn = gsn_dagger;	break;
//		case WEAPON_SPEAR:		sn = gsn_spear;		break;
//		case WEAPON_MACE:		sn = gsn_mace;		break;
//		case WEAPON_AXE:		sn = gsn_axe;		break;
//		case WEAPON_FLAIL:		sn = gsn_flail;		break;
//		case WEAPON_WHIP:		sn = gsn_whip;		break;
//		case WEAPON_POLEARM:	sn = gsn_polearm;	break;
//		case WEAPON_BOW:		sn = gsn_bow;		break;
//		case WEAPON_STAFF:		sn = gsn_staff;		break;
//    }

	if (ch != victim)
	{
		if (!str_cmp(bonus, ""))
		{
			sprintf (buf1, "$n %s $N %s (%d)", damage2, weapon2, dam);
			sprintf (buf2, "You %s $N %s", damage, weapon);
			sprintf (buf3, "$n %s you %s", damage2, weapon2);
		}
		else
		{
			sprintf (buf1, "$n %s %s $N %s (%d)", bonus, damage2, weapon2, dam);
			sprintf (buf2, "You %s %s $N %s", bonus, damage, weapon);
			sprintf (buf3, "$n %s %s you %s", bonus, damage2, weapon2);
		}
	}

//	if (form > -1)
//	{
//		sprintf (buf1, form_table[form].toRoom);
//		sprintf (buf2, form_table[form].toChar);
//		sprintf (buf3, form_table[form].toVict);
//		strcat (buf1, amt);
//		strcat (buf2, amt);
//		strcat (buf3, amt);
//	}
	
	if (ch == victim)
	{
		act (buf1, ch, NULL, NULL, TO_ROOM, POS_RESTING);
		act (buf2, ch, NULL, NULL, TO_CHAR, POS_DEAD);
	}
	else
	{
		act (buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		act (buf2, ch, NULL, victim, TO_CHAR, POS_DEAD);
		act (buf3, ch, NULL, victim, TO_VICT, POS_RESTING);
	}
	
	return;
}


/*
 * Hit one guy once.
 */
void one_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary)
{
	return;
}


/* Inflict damage from a hit. */
bool damage (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
			 int dam_type, bool show, bool check_safe)
{
	return TRUE;
}



bool is_safe (CHAR_DATA * ch, CHAR_DATA * victim)
{
	if (victim->in_room == NULL || ch->in_room == NULL)
		return TRUE;
	
	if (victim->fighting == ch || victim == ch)
		return FALSE;
	
	if (IS_IMMORTAL (ch) && ch->level > LEVEL_IMMORTAL)
		return FALSE;
	
	/* killing mobiles */
	if (IS_NPC (victim))
	{
		
		/* safe room? */
		if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
		{
			send_to_char ("Not in this room.\n\r", ch);
			return TRUE;
		}
		
		if (victim->pIndexData->pShop != NULL)
		{
			send_to_char ("The shopkeeper wouldn't like that.\n\r", ch);
			return TRUE;
		}
		
		/* no killing healers, trainers, etc */
		if (IS_FLAG (victim->states.player, _ACT_TRAIN)
			|| IS_FLAG (victim->states.player, _ACT_STABLEHAND)
			|| IS_FLAG (victim->states.player, _ACT_PRACTICE)
			|| IS_FLAG (victim->states.player, _ACT_HEALER)
			|| IS_FLAG (victim->states.player, _ACT_BANKER))
		{
			send_to_char ("I don't think the Creator would approve.\n\r", ch);
			return TRUE;
		}
		
		if (!IS_NPC (ch))
		{
			/* no pets */
			if (IS_FLAG (victim->states.player, _ACT_PET))
			{
				act ("But $N looks so cute and cuddly...",
					ch, NULL, victim, TO_CHAR, POS_RESTING);
				return TRUE;
			}
		}
	}

	// killing players
	else
	{
		/* NPC doing the killing */
		if (IS_NPC (ch))
		{
			/* safe room check */
			if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
			{
				send_to_char ("Not in this room.\n\r", ch);
				return TRUE;
			}
			
			/* charmed mobs and pets cannot attack players while owned */
			if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master != NULL
				&& ch->master->fighting != victim)
			{
				send_to_char ("Players are your friends!\n\r", ch);
				return TRUE;
			}
		}
		/* player doing the killing */
		else
		{

			/* safe room? */
			if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
			{
				send_to_char ("Not in this room.\n\r", ch);
				return TRUE;
			}

//			if (victim->pcdata->peaceful)
//			{
//				send_to_char ("Not while they're peaceful you wont...\n\r", ch);
//				return TRUE;
//			}
			
			if (ch->level > victim->level + pMud.pk_limit)
			{
				send_to_char ("Pick on someone your own size.\n\r", ch);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/* Set position of a victim. */
// Needs re-writing
void update_pos (CHAR_DATA * victim)
{
	
	if (!IS_NPC (victim) && !IS_IMMORTAL (victim) && victim->hit < 1 && victim->wtimer == 0)
	{
		victim->position = POS_MORTAL;
		victim->hit = 1;
		return;
	}
	else
		if (victim->wtimer > 0)
		{
			victim->position = POS_MORTAL;
			return;
		}
		
		if (victim->hit > 0)
		{
			if (victim->position < POS_STUNNED)
				victim->position = POS_STANDING;
			return;
		}
		
		if (IS_NPC (victim) && victim->hit < 1)
		{
			victim->position = POS_DEAD;
			return;
		}
		
		if (victim->hit <= -11)
		{
			victim->position = POS_DEAD;
			return;
		}
		
		if (victim->hit <= -6)
			victim->position = POS_MORTAL;
		else if (victim->hit <= -3)
			victim->position = POS_INCAP;
		else
			victim->position = POS_STUNNED;
		
		return;
}



/*
 * Start fights.
 */
void set_fighting (CHAR_DATA * ch, CHAR_DATA * victim)
{
	if (IS_AFFECTED (ch, _AFF_SLEEP))
		affect_strip (ch, skill_lookup("sleep"));
	
	ch->fighting = victim;
	
	if (victim != NULL)
		ch->position = POS_FIGHTING;

	// This check is repeated in damage I know... but taking damage does not always
	// set fighting... so....
	if ( victim->vGhostRoom > -1 ) // return the player to their body...
	{
		CHAR_DATA *pCh = NULL, *pNext = NULL, *vCh = NULL;

		for ( pCh = char_list; pCh != NULL; pCh = pNext )
		{
			pNext = pCh->next;
			if ( !IS_NPC(pCh) && GET_ID(pCh) == victim->vGhostRoom )
			  break;
		}


		if ( pCh && ( vCh = get_char_by_id( pCh->vGhostRoom ) ) != NULL )
		{
			int i;

			char_from_room(pCh);
			char_to_room(vCh, pCh->in_room);
			vCh->hit = pCh->hit;
			vCh->move = pCh->move;

			for(i=0; i<5;i++)
				vCh->flow_str.current[i] = pCh->flow_str.current[i];

			vCh->position = pCh->position;
			vCh->fighting = pCh->fighting;
			pCh->fighting->fighting = vCh;
			extract_char(pCh, TRUE); 
			pCh->vGhostRoom = -1;
			victim = pCh;
		}
	}

	return;
}



/*
 * Stop fights.
 */
void stop_fighting (CHAR_DATA * ch, bool fBoth)
{
	CHAR_DATA *fch;
	
	for (fch = char_list; fch != NULL; fch = fch->next)
	{
		if (fch == ch || (fBoth && fch->fighting == ch))
		{
			fch->fighting = NULL;
			fch->position = IS_NPC (fch) ? fch->default_pos : POS_STANDING;
			
			if (IS_AFFECTED (ch, _AFF_WARFARE))
			{
				send_to_char ("With the threat gone, you slowly clean"
					" the gore from your weapon.\n\r", ch);
				
				act ("$n cleans the blood from $s weapon, unwilling to let "
					" Shadowspawn blood etch the steel",
					ch, NULL, NULL, TO_ROOM, POS_RESTING);
				
				REMOVE_FLAG (ch->states.affected_by, _AFF_WARFARE);
			}
			
			if (IS_AFFECTED (ch, _AFF_MARTYRDOM))
			{
				send_to_char ("The Last Embrace will have to wait.\n\r", ch);
				affect_strip (ch, gsn_martyrdom);
			}
			update_pos (fch);
		}
	}
	
	return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse (CHAR_DATA * ch)
{
	char buf[MSL];
	OBJ_DATA *corpse;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	char *name;
	
	if (IS_NPC (ch))
	{
		name = ch->short_descr;
		corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
		
		if (corpse)
		{
			corpse->timer = number_range (3, 6);
			if (ch->gold > 0)
			{
				obj_to_obj (create_money (ch->gold, ch->silver), corpse);
				ch->gold = 0;
				ch->silver = 0;
			}
			corpse->cost = 0;
		}
	}
	else
	{
		name = ch->name;
		corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
		
		if (corpse)
		{
			corpse->timer = number_range (25, 40);
			REMOVE_FLAG (ch->states.player, _CANLOOT);
			if (!is_clan (ch))
				corpse->owner = str_dup (ch->name);
			else
			{
				corpse->owner = NULL;
				if (ch->gold > 1 || ch->silver > 1)
				{
					obj_to_obj (create_money (ch->gold / 2, ch->silver / 2), corpse);
					ch->gold -= ch->gold / 2;
					ch->silver -= ch->silver / 2;
				}
			}
			
			corpse->cost = 0;
		}
	}
	
	if (!corpse)
		return;
	
	corpse->level = ch->level;
	
	sprintf (buf, corpse->short_descr, name);
	free_string (corpse->short_descr);
	corpse->short_descr = str_dup (buf);
	
	sprintf (buf, corpse->description, name);
	free_string (corpse->description);
	corpse->description = str_dup (buf);
	
	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		bool floating = FALSE;
		
		obj_next = obj->next_content;
		if (obj->wear_loc == WEAR_FLOAT)
			floating = TRUE;
		obj_from_char (obj);
		if (obj->item_type == ITEM_POTION)
			obj->timer = number_range (500, 1000);
		if (IS_SET (obj->extra_flags, ITEM_ROT_DEATH) && !floating)
		{
			obj->timer = number_range (5, 10);
			REMOVE_BIT (obj->extra_flags, ITEM_ROT_DEATH);
		}
		REMOVE_BIT (obj->extra_flags, ITEM_VIS_DEATH);
		
		if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
			extract_obj (obj);
		else if (floating)
		{
			if (IS_OBJ_STAT (obj, ITEM_ROT_DEATH))	/* get rid of it! */
			{
				if (obj->contains != NULL)
				{
					OBJ_DATA *in, *in_next;
					
					act ("$p evaporates,scattering its contents.",
						ch, obj, NULL, TO_ROOM, POS_RESTING);
					for (in = obj->contains; in != NULL; in = in_next)
					{
						in_next = in->next_content;
						obj_from_obj (in);
						obj_to_room (in, ch->in_room);
					}
				}
				else
					act ("$p evaporates.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				extract_obj (obj);
			}
			else
			{
				act ("$p falls to the floor.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				obj_to_room (obj, ch->in_room);
			}
		}
		else
			obj_to_obj (obj, corpse);
	}
	
	obj_to_room (corpse, ch->in_room);
	return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry (CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *was_in_room;
	char *msg;
	int door;
	long vnum;
	
	vnum = 0;
	msg = "You hear $n's death cry.";
	
	switch (number_bits (4))
	{
		case 0:
			msg = "$n hits the ground ... DEAD.";
			break;
		case 1:
			if (ch->material == 0)
			{
				msg = "$n splatters blood on your armor.";
				break;
			}
		case 2:
			if (IS_FLAG (ch->states.parts, _GUTS))
			{
				msg = "$n spills $s guts all over the floor.";
				vnum = OBJ_VNUM_GUTS;
			}
			break;
		case 3:
			if (IS_FLAG (ch->states.parts, _HEAD))
			{
				msg = "$n's severed head plops on the ground.";
				vnum = OBJ_VNUM_SEVERED_HEAD;
			}
			break;
		case 4:
			if (IS_FLAG (ch->states.parts, _HEART))
			{
				msg = "$n's heart is torn from $s chest.";
				vnum = OBJ_VNUM_TORN_HEART;
			}
			break;
		case 5:
			if (IS_FLAG (ch->states.parts, _ARMS))
			{
				msg = "$n's arm is sliced from $s dead body.";
				vnum = OBJ_VNUM_SLICED_ARM;
			}
			break;
		case 6:
			if (IS_FLAG (ch->states.parts, _LEGS))
			{
				msg = "$n's leg is sliced from $s dead body.";
				vnum = OBJ_VNUM_SLICED_LEG;
			}
			break;
		case 7:
			if (IS_FLAG (ch->states.parts, _BRAINS))
			{
				msg = "$n's head is shattered, and $s brains splash all over you.";
				vnum = OBJ_VNUM_BRAINS;
			}
	}
	
	act (msg, ch, NULL, NULL, TO_ROOM, POS_RESTING);
	
	snd_trigger (ch,
	       ch->sounds ? ch->sounds :
	IS_NPC (ch) ? ch->pIndexData->sounds : NULL,
	       SOUND_MOBILE, SOUND_MOB_DEATHCRY, TRUE);
	
	
	if (vnum != 0)
	{
		char buf[MSL];
		OBJ_DATA *obj;
		char *name;
		
		name = IS_NPC (ch) ? ch->short_descr : ch->name;
		obj = create_object (get_obj_index (vnum), 0);
		if (obj)
		{
			obj->timer = number_range (4, 7);
			
			sprintf (buf, obj->short_descr, name);
			free_string (obj->short_descr);
			obj->short_descr = str_dup (buf);
			
			sprintf (buf, obj->description, name);
			free_string (obj->description);
			obj->description = str_dup (buf);
			
			if (obj->item_type == ITEM_FOOD)
			{
				if (IS_FLAG (ch->states.form, _POISON))
					obj->value[3] = 1;
				else if (!IS_FLAG (ch->states.form, _EDIBLE))
					obj->item_type = ITEM_TRASH;
			}
			
			obj_to_room (obj, ch->in_room);
		}
	}
	
	if (IS_NPC (ch))
		msg = "You hear something's death cry.";
	else
		msg = "You hear someone's death cry.";
	
	was_in_room = ch->in_room;
	for (door = 0; door <= 5; door++)
	{
		EXIT_DATA *pexit;
		
		if ((pexit = was_in_room->exit[door]) != NULL
			&& pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room)
		{
			ch->in_room = pexit->u1.to_room;
			act (msg, ch, NULL, NULL, TO_ROOM, POS_RESTING);
		}
	}
	ch->in_room = was_in_room;
	
	return;
}


/** Function: return_ch_to_play
  * Descr   : Returns a mortally wounded (via PK) character to a state
  *         : where they can begin playing again.
  * Returns : void
  * Syntax  : return_ch_to_play( who )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void return_ch_to_play (CHAR_DATA * ch)
{
	char buf[MSL];
	ROOM_INDEX_DATA *to = NULL;
	
	if (ch == NULL)
		return;
	
	ch->wtimer = 0;
	
	ch->position = POS_RESTING;
	ch->hit = UMAX (1, ch->hit);
	ch->move = UMAX (1, ch->move);
	affect_strip (ch, skill_lookup("poison"));
	
	ch->flow_str.current[AIR] = UMAX (1, ch->flow_str.current[AIR]);
	ch->flow_str.current[EARTH] = UMAX (1, ch->flow_str.current[EARTH]);
	ch->flow_str.current[WATER] = UMAX (1, ch->flow_str.current[WATER]);
	ch->flow_str.current[FIRE] = UMAX (1, ch->flow_str.current[FIRE]);
	ch->flow_str.current[SPIRIT] = UMAX (1, ch->flow_str.current[SPIRIT]);
	
	if (MOUNTED (ch))
	{
		sprintf (buf, "$n's mount wanders off with it's master.\n\r");
	}
	else
		
		switch (dice (1, 6))
    {
		case 1:
		case 2:
			sprintf (buf, "%s %s.\n\r",
				"A wisdom, on her way to town, stops and takes charge of the"
				" wounded", is_clan (ch) ? clan_table[ch->clan].name : "");
			break;
		case 3:
		case 4:
			sprintf (buf, "%s %s.\n\r",
				"A soldier passes by and carries off the wounded",
				is_clan (ch) ? clan_table[ch->clan].name : "");
			break;
		case 5:
		case 6:
			sprintf (buf, "%s %s.\n\r",
				"A kind merchant stops to tend to ", ch->name);
			break;
    }
	
	act (buf, ch, NULL, NULL, TO_ROOM, POS_RESTING);
	
	if (is_clan (ch) && clan_table[ch->clan].room[2] != 0)
		to = get_room_index (clan_table[ch->clan].room[2]);
	
	if (to == NULL)
		to = get_room_index (ROOM_VNUM_LOGIN);
	
	char_from_room (ch);
	char_to_room (ch, to);
	send_to_char ("You wake up in a healers. Next time, run!\n\r", ch);
}


void raw_kill (CHAR_DATA * victim, bool destroy, char *why_destroyed)
{
	int i;
	OBJ_DATA *tattoo;
	
	stop_fighting (victim, TRUE);
	death_cry (victim);
	
	if (MOUNTED (victim))
		victim->riding = FALSE;
	
	if (victim->master)
	{
		if (victim->master->mount && victim->master->mount == victim)
		{
			send_to_char ("You leap off of your stricken mount!\n\r",
				victim->master);
			victim->master->riding = FALSE;
		}
	}
	
	if (!destroy || IS_NPC (victim))
		make_corpse (victim);
	
	if (IS_NPC (victim))
	{
		victim->pIndexData->killed++;
		kill_table[URANGE (0, victim->level, MAX_LEVEL - 1)].killed++;
		extract_char (victim, TRUE);
		return;
	}
	
	tattoo = get_eq_char (victim, WEAR_TATTOO);	/* keep tattoo */
	
	if (tattoo != NULL)
		obj_from_char (tattoo);
	
	/* GM, transfer the corpse to the clan morgue... */
	if (is_clan (victim) && clan_table[victim->clan].room[1] > 0)
	{
		OBJ_DATA *corpse;
		ROOM_INDEX_DATA *morgue = NULL;
		
		corpse = get_obj_list (victim, "corpse", victim->in_room->contents);
		
		if ( IS_FLAG(pMud.config, _USE_MORGUES) )
			morgue = get_room_index (clan_table[victim->clan].room[1]);
		
		if (corpse != NULL && morgue != NULL)
		{
			obj_from_room (corpse);
			obj_to_room (corpse, morgue);
			if (victim->alignment > 0)
				act ("The Creator protects this one...", victim, NULL, NULL, TO_ROOM,
				POS_RESTING);
			else
				act ("The Lord of the Grave claims his own!", victim, NULL, NULL,
				TO_ROOM, POS_RESTING);
		}
	}
	
	if ( IS_FLAG(pMud.config, _DESTROY_LOSER) && destroy && !IS_NPC (victim))
	{
		char filename[MSL];
		char backup[MSL];
		char arg[MSL];

		sprintf (filename, "%s%c/%s",
			PLAYER_DIR,
			UPPER (victim->name[0]),
			capitalize (fix_name (victim->name, FALSE)));
		
		sprintf (backup, "%s%s", filename, ".dead");
		
		wiznet ("$N has been destroyed.", victim, NULL, 0, 0, 0);
		victim->wtimer = 0;
		
		if ( IS_FLAG(pMud.config, _BAN_LOSER_NAME) )
		{
			sprintf (arg, "%s %s",
				victim->name,
				why_destroyed[0] != '\0' ? why_destroyed : "Killed in a PK.");
		
			ban_name (NULL, arg);
		}		

		do_quit (victim, "");
		rename (filename, backup);
		return;
	}
	else
		extract_char (victim, FALSE);
	
	if (tattoo != NULL)
	{
		obj_to_char (tattoo, victim);
		equip_char (victim, tattoo, WEAR_TATTOO);
	}
	
	while (victim->affected)
		affect_remove (victim, victim->affected);
	
	set_init (victim->states.affected_by);
	add_sets (victim->states.affected_by, race_table[victim->race].aff);
	
	for (i = 0; i < 4; i++)
		victim->armor[i] = 100;
	
	victim->position = POS_RESTING;
	victim->hit = UMAX (1, victim->hit);
	victim->move = UMAX (1, victim->move);
	
	victim->flow_str.current[AIR]	= UMAX (1, victim->flow_str.current[AIR]);
	victim->flow_str.current[EARTH] = UMAX (1, victim->flow_str.current[EARTH]);
	victim->flow_str.current[WATER] = UMAX (1, victim->flow_str.current[WATER]);
	victim->flow_str.current[FIRE]	= UMAX (1, victim->flow_str.current[FIRE]);
	victim->flow_str.current[SPIRIT]= UMAX (1, victim->flow_str.current[SPIRIT]);
	
	
	return;
}

