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

/**
 * @@ Objective (combat.c)
 *
 * This file contains most of the combat skill support functions, such as
 * can_dodge, etc.  As well as the more generic combat skills, such as parry
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/** Function: check_form
  * Descr   : Given a form and affect type, searches through the form table to see if
  *         : it can find a match. (both types must match)
  * Returns : Entry in form table of matched form, if any. Or -1 if not found.
  * Syntax  : check_form( form affect type, form type )
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int check_form (enum _FORM_TYPE aType, enum _FORM_AFFECTS fType)
{
	int i = -1;
	int count = 0;
	int pos = 0;
	
	for (i = 0; i < maxForm; i++)
		if ((aType == SF_EITHER || (aType == form_table[i].aType))
			&& fType == form_table[i].fType)
			count++;
		
		if (count == 0)
			return -1;
		
		pos = number_range (1, count);
		count = 0;
		
		for (i = 0; i < maxForm; i++)
			if ((aType == SF_EITHER || (aType == form_table[i].aType))
				&& fType == form_table[i].fType)
			{
				count++;
				if (count == pos)
					return i;		/* return the position of the form in the form_table */
			}
			
			return -1;
}


/** Function: disarm
  * Descr   : Disarm a character (pc/npc)
  * Note    : Caller must check for success prior to calling disarm
  * Returns : void
  * Syntax  : disarm( attacker, defender )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void disarm (CHAR_DATA * ch, CHAR_DATA * victim)
{
	OBJ_DATA *obj;
	
	if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
		if ((obj = get_eq_char (victim, WEAR_SECONDARY)) == NULL)
			return;
		
		if (!can_fight (victim, TRUE))
			return;
		
		if (IS_OBJ_STAT (obj, ITEM_NOREMOVE))
		{
			act ("$S weapon won't budge!", ch, NULL, victim, TO_CHAR, POS_RESTING);
			act ("$n tries to disarm you, but your weapon won't budge!",
				ch, NULL, victim, TO_VICT, POS_RESTING);
			act ("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT,
				POS_RESTING);
			return;
		}
		
		if (is_clan (ch) &&
			IS_FLAG (clan_table[ch->clan].flags, _THIEF_TAKER) &&
			!IS_SET (obj->extra_flags, ITEM_NOBREAK))
		{
			act ("$n disarms you and in the process, snaps your weapon in two!",
				ch, NULL, victim, TO_VICT, POS_RESTING);
			act ("You break $N's weapon!", ch, NULL, victim, TO_CHAR, POS_RESTING);
			act ("$n snaps $N's weapon in two!",
				ch, NULL, victim, TO_NOTVICT, POS_RESTING);
			obj->condition = 0;
		}
		else
			
		{
			int form = -1;
			
			if (form >= 0)
			{
				act (form_table[form].toChar, ch, NULL, victim, TO_CHAR, POS_RESTING);
				act (form_table[form].toVict, ch, NULL, victim, TO_VICT, POS_RESTING);
				act (form_table[form].toRoom, ch, NULL, victim, TO_NOTVICT,
					POS_RESTING);
			} // forms should not be a silent disarm, they should be a preamble on how
			act ("$n DISARMS you and sends your weapon flying!",
				ch, NULL, victim, TO_VICT, POS_RESTING);
			act ("You disarm $N!", ch, NULL, victim, TO_CHAR, POS_RESTING);
			act ("$n disarms $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
			if ( !IS_SET(obj->extra_flags, ITEM_NOBREAK) )
				obj->condition--;
		}
		
		obj_from_char (obj);
		
		if (IS_OBJ_STAT (obj, ITEM_NODROP) || IS_OBJ_STAT (obj, ITEM_INVENTORY))
			obj_to_char (obj, victim);
		else
		{
			obj_to_room (obj, victim->in_room);
			if (IS_NPC (victim) && victim->wait == 0 && can_see_obj (victim, obj))
				get_obj (victim, obj, NULL);
		}
		
		return;
}


/** Function: do_berserk
  * Descr   : Warrior skill that raises hit/dam but lowers ac
  * Returns : void
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_berserk)
{
	int chance, hp_percent;
	
	if ((chance = get_skill (ch, gsn_berserk)) == 0
		|| (IS_NPC (ch) && !IS_FLAG (ch->states.off, _OFF_BERSERK))
		|| (!IS_NPC (ch)))
	{
		send_to_char ("You turn red in the face, but nothing happens.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_BERSERK) || is_affected (ch, gsn_berserk)
		|| is_affected (ch, gsn_frenzy))
	{
		send_to_char ("You get a little madder.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_CALM))
	{
		send_to_char ("You're feeling too mellow to berserk.\n\r", ch);
		return;
	}
	
	
	/* modifiers */
	
	/* fighting */
	if (ch->position == POS_FIGHTING)
		chance += 10;
	
	/* damage -- below 50% of hp helps, above hurts */
	hp_percent = 100 * ch->hit / ch->max_hit;
	chance += 25 - hp_percent / 2;
	
	if (number_percent () < chance)
	{
		AFFECT_DATA af;
		
		WAIT_STATE (ch, PULSE_VIOLENCE);
		
		/* heal a little damage */
		ch->hit += ch->level * 2;
		ch->hit = UMIN (ch->hit, ch->max_hit);
		
		send_to_char ("Your pulse races as you are consumed by rage!\n\r", ch);
		act ("$n gets a wild look in $s eyes.", ch, NULL, NULL, TO_ROOM,
			POS_RESTING);
		check_improve (ch, gsn_berserk, TRUE, 2);
		
		af.where = TO_AFFECTS;
		af.type = gsn_berserk;
		af.level = ch->level;
		af.duration = number_fuzzy (ch->level / 8);
		af.modifier = UMAX (1, ch->level / 5);
		af.bitvector = _AFF_BERSERK;
		
		af.location = APPLY_HITROLL;
		affect_to_char (ch, &af);
		
		af.location = APPLY_DAMROLL;
		affect_to_char (ch, &af);
		
		af.modifier = UMAX (10, 10 * (ch->level / 5));
		af.location = APPLY_AC;
		affect_to_char (ch, &af);
	}
	
	else
	{
		WAIT_STATE (ch, 3 * PULSE_VIOLENCE);

		send_to_char ("Your pulse speeds up, but nothing happens.\n\r", ch);
		check_improve (ch, gsn_berserk, FALSE, 2);
	}
}

/** Function: do_bash
  * Descr   : Warrior skill to knock an opponet down by bashing into them
  *         : Takes into account size/str&dex of attacker and defender
  * Returns : void
  * Syntax  : bash ( who )
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_bash)
{
	char arg[MIL];
	CHAR_DATA *victim;
	int chance, dam;
	
	one_argument (argument, arg);
	
	if (!can_fight (ch, TRUE))
		return;
	
	if ((chance = get_skill (ch, gsn_bash)) == 0
		|| (IS_NPC (ch) && !IS_FLAG (ch->states.off, _OFF_BASH))
		|| (!IS_NPC (ch)))
	{
		send_to_char ("Bashing? What's that?\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		victim = ch->fighting;
		if (victim == NULL)
		{
			send_to_char ("But you aren't fighting anyone!\n\r", ch);
			return;
		}
	}
	
	else if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (!valid_level (ch, victim, TRUE))
		return;
	
	if (victim->position < POS_FIGHTING)
	{
		act ("You'll have to let $M get back up first.", ch, NULL, victim,
			TO_CHAR, POS_RESTING);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("You try to bash your brains out, but fail.\n\r", ch);
		return;
	}
	
	if (is_safe (ch, victim))
		return;
	
	if (MOUNTED (victim) && !MOUNTED (ch))
	{
		send_to_char ("But they're mounted!\n\r", ch);
		return;
	}
	
	if (!IS_FLAG(pMud.config, _ALLOW_KILL_STEALING) && IS_NPC (victim) &&
		victim->fighting != NULL && !is_same_group (ch, victim->fighting))
	{
		send_to_char ("Kill stealing is not permitted.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master == victim)
	{
		act ("But $N is your friend!", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	/* modifiers */
	
	/* size  and weight */
	chance += ch->carry_weight / 250;
	chance -= victim->carry_weight / 200;
	
	if (ch->size < victim->size)
		chance += (ch->size - victim->size) * 15;
	else
		chance += (ch->size - victim->size) * 10;
	
	
	/* stats */
	chance += get_curr_stat (ch, STAT_STR);
	chance -= (get_curr_stat (victim, STAT_DEX) * 4) / 3;
	chance -= GET_AC (victim, AC_BASH) / 25;
	
	/* speed */
	if (IS_FLAG (ch->states.off, _OFF_FAST) || IS_AFFECTED (ch, _AFF_HASTE))
		chance += 10;
	
	if (IS_FLAG (victim->states.off, _OFF_FAST)
		|| IS_AFFECTED (victim, _AFF_HASTE))
		chance -= 30;
	
	/* level */
	chance += (ch->level - victim->level);
	
	if (!IS_NPC (victim) && chance < get_skill (victim, gsn_dodge))
	{
		chance -= 3 * (get_skill (victim, gsn_dodge) - chance);
	}
	
	/* now the attack */
	if (number_percent () <= chance)
	{
		int form = -1;
		
		if (ch->size > victim->size)
			dam = ((get_curr_stat(ch,STAT_STR) + get_curr_stat(ch,STAT_CON))/2 * 
			(ch->size))*(get_skill(ch, gsn_bash)/100);
		else
			dam = (get_curr_stat(ch,STAT_STR) + get_curr_stat(ch,STAT_CON))/2 * (get_skill(ch, gsn_bash)/100);
		
		if (form < 0)
		{ 
			if (get_skill (ch, gsn_bash) == 100)
			{
				dam *= 2;
				act ("$n slams $s head into yours.\n\rThe headbutt stuns you!",
					ch, NULL, victim, TO_VICT, POS_RESTING);
				act ("You slam your head into $N.  The headbutt stuns $M!",
					ch, NULL, victim, TO_CHAR, POS_RESTING);
				act ("$n slams $s head into $N.  The headbutt stuns $N.",
					ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				DAZE_STATE (victim, 4 * PULSE_VIOLENCE);
			}
			else
			{
				act ("$n slams into you with their body, sending you sprawling!",
					ch, NULL, victim, TO_VICT, POS_RESTING);
				act ("You slam your body into $N, sending $M sprawling!",
					ch, NULL, victim, TO_CHAR, POS_RESTING);
				act ("$n slams into $N with thier body, sending $M sprawling!",
					ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				DAZE_STATE (victim, 2 * PULSE_VIOLENCE);
			}
		}
		else
		{
			act (form_table[form].toChar, ch, NULL, victim, TO_CHAR, POS_RESTING);
			act (form_table[form].toVict, ch, NULL, victim, TO_VICT, POS_RESTING);
			act (form_table[form].toRoom, ch, NULL, victim, TO_NOTVICT,
				POS_RESTING);
			DAZE_STATE (victim, 2 * PULSE_VIOLENCE);
		}
		
		check_improve (ch, gsn_bash, TRUE, 3);
		
		WAIT_STATE (ch, skill_table[gsn_bash].beats);
		
		damage (ch, victim, dam, gsn_bash, DAM_BASH, TRUE, TRUE);
		
		victim->position = POS_STUNNED;
		
	}
	else
	{
		damage (ch, victim, 0, gsn_bash, DAM_BASH, FALSE, TRUE);
		act ("You fall flat on your face!",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		act ("$n falls flat on $s face.",
			ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		act ("You evade $n's bash, causing $m to fall flat on $s face.",
			ch, NULL, victim, TO_VICT, POS_RESTING);
		
		check_improve (ch, gsn_bash, FALSE, 2);
		ch->position = POS_RESTING;
		WAIT_STATE (ch, skill_table[gsn_bash].beats * 3 / 2);
	}
}

/** Function: do_trip
  * Descr   : Warrior skill to trip an opponent
  * Returns : void
  * Syntax  : trip who?
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_trip)
{
	char arg[MIL];
	CHAR_DATA *victim;
	int chance;
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't trip someone while your riding!\n\r", ch);
		return;
	}
	
	if (!IS_FLAG (ch->states.parts, _LEGS))
	{
		send_to_char
			("You don't have any legs, how are you going to trip someone?\n\r", ch);
		return;
	}
	
	one_argument (argument, arg);
	
	if (!can_fight (ch, TRUE))
		return;
	
	if ((chance = get_skill (ch, gsn_trip)) == 0
		|| (IS_NPC (ch) && !IS_FLAG (ch->states.off, _OFF_TRIP))
		|| (!IS_NPC (ch)))
	//	&& ch->level < skill_table[gsn_trip].skill_level[ch->c_class]))
	{
		send_to_char ("Tripping?  What's that?\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		victim = ch->fighting;
		if (victim == NULL)
		{
			send_to_char ("But you aren't fighting anyone!\n\r", ch);
			return;
		}
	}
	
	else if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (!valid_level (ch, victim, TRUE))
		return;
	
	if (is_safe (ch, victim))
		return;
	
	if (!IS_FLAG(pMud.config, _ALLOW_KILL_STEALING) && IS_NPC (victim) &&
		victim->fighting != NULL && !is_same_group (ch, victim->fighting))
	{
		send_to_char ("Kill stealing is not permitted.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (victim, _AFF_FLYING))
	{
		act ("$S feet aren't on the ground.", ch, NULL, victim, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (victim->position < POS_FIGHTING)
	{
		act ("$N is already down.", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (MOUNTED (victim))
	{
		send_to_char ("Explain to me how your going to trip a horse?!\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("You fall flat on your face!\n\r", ch);
		WAIT_STATE (ch, 2 * skill_table[gsn_trip].beats);
		act ("$n trips over $s own feet!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master == victim)
	{
		act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	/* modifiers */
	
	/* size */
	if (ch->size < victim->size)
		chance += (ch->size - victim->size) * 10;	/* bigger = harder to trip */
	
	/* dex */
	chance += get_curr_stat (ch, STAT_DEX);
	chance -= get_curr_stat (victim, STAT_DEX) * 3 / 2;
	
	/* speed */
	if (IS_FLAG (ch->states.off, _OFF_FAST) || IS_AFFECTED (ch, _AFF_HASTE))
		chance += 10;
	if (IS_FLAG (victim->states.off, _OFF_FAST)
		|| IS_AFFECTED (victim, _AFF_HASTE))
		chance -= 20;
	
	/* level */
	chance += (ch->level - victim->level) * 2;
	
	/* now the attack */
	if (number_percent () < chance)
	{
		act ("$n trips you and you go down!", ch, NULL, victim, TO_VICT,
			POS_RESTING);
		act ("You trip $N and $N goes down!", ch, NULL, victim, TO_CHAR,
			POS_RESTING);
		act ("$n trips $N, sending $M to the ground.", ch, NULL, victim,
			TO_NOTVICT, POS_RESTING);
		check_improve (ch, gsn_trip, TRUE, 1);
		
		DAZE_STATE (victim, 2 * PULSE_VIOLENCE);
		WAIT_STATE (ch, skill_table[gsn_trip].beats);
		victim->position = POS_RESTING;
		damage (ch, victim, number_range (2, 2 + 2 * victim->size), gsn_trip,
			DAM_BASH, TRUE, TRUE);
	}
	else
	{
		damage (ch, victim, 0, gsn_trip, DAM_BASH, TRUE, TRUE);
		WAIT_STATE (ch, skill_table[gsn_trip].beats * 2 / 3);
		check_improve (ch, gsn_trip, FALSE, 1);
	}
}

/** Function: do_kill
  * Descr   : Starts a fight, 'throws the first punch'
  * Note    : Only 'kills' PC's if called after they have been beaten into
  *         : a state of "incapactation" first.
  * Returns : void
  * Syntax  : kill who?
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  * Edited  : Owen Varley <owen@interactiontraining.com>
  */
_DOFUN (do_kill)
{
	char arg[MIL];
	int char_int;
	int vict_int;
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Kill whom?\n\r", ch);
		return;
	}
	
	if (!can_fight (ch, TRUE))
		return;
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("You hit yourself.  Ouch!\n\r", ch);
		multi_hit (ch, ch, TYPE_UNDEFINED);
		return;
	}
	
	if (!can_fight (victim, TRUE))
	{
		if (IS_AFFECTED (victim, _AFF_BOUND_AIR))
			chprintf (ch,
			"Your weapon can't break through the bands of air surrounding %s!\n\r",
			PERS (victim, ch));
		else if (IS_AFFECTED (victim, _AFF_EARTH_SHIELD))
			chprintf (ch,
			"You can't break through the dome of earth surrounding %s!\n\r",
			PERS (victim, ch));
		else
			chprintf (ch,
			"That person is a bound captive, pick on someone else!\n\r");
		
		return;
	}
	
	if (is_safe (ch, victim))
		return;
	
	if (!IS_FLAG(pMud.config, _ALLOW_KILL_STEALING) && victim->fighting != NULL && !is_same_group (ch, victim->fighting))
	{
		send_to_char ("Kill stealing is not permitted.\n\r", ch);
		return;
	}
	
	if (!valid_level (ch, victim, TRUE))
		return;
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master == victim)
	{
		act ("$N is your beloved master.", ch, NULL, victim, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (ch->position == POS_FIGHTING)
	{
		send_to_char ("You do the best you can!\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch) && !IS_NPC (victim)
		&& IS_FLAG (ch->states.penalties, _NOPK))
	{
		send_to_char ("Your PK priveledges have been revoked.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch) && !IS_NPC (victim) && victim->wtimer > 0)
	{
		chprintf (ch, "You must type 'permakill %s' to permakill %s.\n\r",
			victim->name, victim->name);
		return;
	}
	
	WAIT_STATE (ch, 1 * PULSE_VIOLENCE);

	// New initiative check, to see who gets the first attack in.
	char_int = calculate_initiative( ch );
	vict_int = calculate_initiative( victim );

	if (char_int > vict_int)
	{
		make_attack (ch, victim, TYPE_UNDEFINED);
	    act ("$n gets first hit on $N!", ch, NULL, victim, TO_ROOM, POS_RESTING);
	}
	if (vict_int > char_int)
	{
		make_attack (victim, ch, TYPE_UNDEFINED);
		act ("$N gets first hit on $n!", ch, NULL, victim, TO_ROOM, POS_RESTING);
	}

	// Weapon lock, not completed yet
	if (vict_int == char_int)
	{
		send_to_char("Your weapons lock as you leap into combat!\n\r", ch);
		send_to_char("Your weapons lock as you leap into combat!\n\r", victim);
		return;
	}
	return;
}

/** Function: calculate_initiative
  * Descr   : Calculates the initiative of a character
  *         : used to decide who fights first
  * Returns : initiative value
  * Syntax  : calculate_intiative ( player )
  * Written : v0.1a 4/02
  * Author  : Owen Varley <owen@interactiontraining.com>
  */
bool calculate_initiative (CHAR_DATA * player)
{
	int value = 0;
	int hp, stam;
	int wg =0;
	int i;


	// Initiative is calculated as 1d10 + AGL + MOD
	// Calculating the base values

	// First value, 1d10
	value = random_dice(1, 10, 0);

	// Next we add their Agility stat to this
	value += player->perm_stat[STAT_AGL];

	// Calculating the modifiers to their initiative roll

	// +3 bonus if they have Fast Feet ability
	if (player->ability[ABILITY_FASTFEET] == 1)
		value += 3;

	// Health level modifiers
	// Calculate % of player's hp left
	hp = ((player->hit / player->max_hit) * 100);

	// Between 50% and 75% -1
	if (hp >= 50 && hp < 75)
		value -= 1;

	// Between 25% and 50% -2
	if (hp >= 25 && hp < 50)
		value -= 2;

	// Less than 25% -4
	if (hp < 25)
		value -=4;

	// Stamina level modifiers
	// Calculate % of player's stamina left
	stam = ((player->move / player->max_move) * 100);

	// Between 50% and 75% -1
	if (stam >= 50 && stam < 75)
		value -= 1;

	// Between 25% and 50% -2
	if (stam >= 25 && stam < 50)
		value -= 2;

	// Less than 25% -4
	if (stam < 25)
		value -=4;

	// Now we calculate the weight modifiers
	// For ever 5 WD( Water Day/Liter of Water/Kg) they loose one Intiative
	for (i = 0; i < player->carry_weight; i+= 5)
		wg++;

	value -= wg;


	return value;
}


/** Function: do_murder
  * Descr   : see do_kill. Shell that calls do_kill
  * Returns : void
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_murder)
{
	do_kill (ch, argument);
	return;
}


/** Function: do_rescue
  * Descr   : Removes someone from the thick of the fight, putting the 
  *         : caller in jeopardy instead.
  * Returns : void
  * Syntax  : rescue who?
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_rescue)
{
	char arg[MIL];
	CHAR_DATA *victim;
	CHAR_DATA *fch;
	bool bDenied = FALSE;
	
	one_argument (argument, arg);
	
	if (!can_fight (ch, TRUE))
		return;
	
	
	if (arg[0] == '\0')
	{
		send_to_char ("Rescue whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("What about fleeing instead?\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch) && IS_NPC (victim))
	{
		send_to_char ("Doesn't need your help!\n\r", ch);
		return;
	}
	
	if (ch->fighting == victim)
	{
		send_to_char ("Too late.\n\r", ch);
		return;
	}
	
	if ((fch = victim->fighting) == NULL)
	{
		send_to_char ("That person is not fighting right now.\n\r", ch);
		return;
	}
	
	if (!IS_FLAG(pMud.config, _ALLOW_KILL_STEALING) && IS_NPC (fch) && !is_same_group (ch, victim))
	{
		send_to_char ("Kill stealing is not permitted.\n\r", ch);
		return;
	}
	
	WAIT_STATE (ch, skill_table[gsn_rescue].beats);
	if (number_percent () > get_skill (ch, gsn_rescue))
	{
		send_to_char ("You fail the rescue.\n\r", ch);
		check_improve (ch, gsn_rescue, FALSE, 2);
		return;
	}

	if ( check_maneuver(fch, CANCEL_RESCUE) )
	{
		int iMan = -1;
		float fMod = 0.000f;

		if ( get_skill(ch, gsn_rescue) > get_skill(fch, gsn_maneuvers) )
			fMod = .500f;
		else
			fMod = 0.000f;

		iMan = lookup_maneuver("spearhead");
		fMod += maneuver_table[iMan].difficulty;

		if ( iMan > -1 && make_skill_roll(get_skill(fch, gsn_maneuvers), fMod) )
		{
			if ( validate_requirements(fch, ch, iMan, FALSE) )
			  bDenied = execute_maneuver(fch, ch, iMan, "");
		}
	}

	if ( bDenied )
	{
		_trace(TRUE, "[combat.c::do_rescue] Rescue was canceled by a maneuver.");
		set_fighting(victim, fch);
		set_fighting(fch, victim);
		return;
	}

	act ("You rescue $N!", ch, NULL, victim, TO_CHAR, POS_RESTING);
	act ("$n rescues you!", ch, NULL, victim, TO_VICT, POS_RESTING);
	act ("$n rescues $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
	check_improve (ch, gsn_rescue, TRUE, 2);
	
	stop_fighting (fch, FALSE);
	stop_fighting (victim, FALSE);
	
	set_fighting (ch, fch);
	set_fighting (fch, ch);
	return;
}



/** Function: do_kick
  * Descr   : Kick an opponet
  * Returns : void
  * Syntax  : who's kicking who
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_kick)
{
	CHAR_DATA *victim;
	
	if (!can_fight (ch, TRUE))
		return;
	
	if (MOUNTED (ch)
		&& (!is_clan (ch)
		|| !IS_FLAG (clan_table[ch->clan].flags, _WHITECLOAK)))
	{
		send_to_char ("You can't kick while riding!\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch))
	{
		send_to_char ("You better leave the martial arts to fighters.\n\r", ch);
		return;
	}
	
	if (IS_NPC (ch) && !IS_FLAG (ch->states.off, _OFF_KICK))
		return;
	
	if ((victim = ch->fighting) == NULL)
	{
		send_to_char ("You aren't fighting anyone.\n\r", ch);
		return;
	}
	
	WAIT_STATE (ch, skill_table[gsn_kick].beats);
	
	/* mounted kicks use players skill%, not mounts */
	if ( make_skill_roll( get_skill(ch, gsn_kick), 1.0 ) )
	{
		if (MOUNTED (ch))
			damage (ch, victim, number_range (1, ch->level * 2),
			gsn_kick, DAM_BASH, TRUE, TRUE);
		else
			damage (ch, victim, number_range (1, ch->level),
			gsn_kick, DAM_BASH, TRUE, TRUE);

		check_improve (ch, gsn_kick, TRUE, 2);
	}
	else
	{
		damage (ch, victim, 0, gsn_kick, DAM_BASH, TRUE, TRUE);
		check_improve (ch, gsn_kick, FALSE, 2);
	}
	return;
}




/** Function: do_disarm
  * Descr   : Disarm a character, potentially weaken or damage their weapon.
  * Returns : void
  * Syntax  : disarmer, who to disarm
  * Written : v1.0 9/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_disarm)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;
	
	hth = 0;
	
	if ((chance = get_skill (ch, gsn_disarm)) == 0)
	{
		send_to_char ("You don't know how to disarm opponents.\n\r", ch);
		return;
	}
	
	if (get_eq_char (ch, WEAR_WIELD) == NULL)
		if (get_eq_char (ch, WEAR_SECONDARY) == NULL
			&& ((hth = get_skill (ch, gsn_hand_to_hand)) == 0
			|| (IS_NPC (ch) && !IS_FLAG (ch->states.off, _OFF_DISARM))))
		{
			send_to_char ("You must wield a weapon to disarm.\n\r", ch);
			return;
		}
		
		if ((victim = ch->fighting) == NULL)
		{
			send_to_char ("You aren't fighting anyone.\n\r", ch);
			return;
		}
		
		if ((obj = get_eq_char (victim, WEAR_WIELD)) == NULL)
			if ((obj = get_eq_char (victim, WEAR_SECONDARY)) == NULL)
			{
				send_to_char ("Your opponent is not wielding a weapon.\n\r", ch);
				return;
			}
			
			if (IS_FLAG (victim->states.imm, _DISARM))
			{
				send_to_char ("Your opponet is too strong to be disarmed.\n\r", ch);
				return;
			}
			
			/* find weapon skills */
			if (get_eq_char (ch, WEAR_WIELD) == NULL)
			  ch_weapon = get_weapon_skill (ch, get_weapon_sn (ch, TRUE));
			else
			  ch_weapon = get_weapon_skill (ch, get_weapon_sn (ch, FALSE));
			if (get_eq_char (victim, WEAR_WIELD) == NULL)
			{
              vict_weapon = get_weapon_skill (victim, get_weapon_sn (victim, FALSE));
   			  ch_vict_weapon = get_weapon_skill (ch, get_weapon_sn (victim, FALSE));
			}
			else
			{
			  vict_weapon = get_weapon_skill (victim, get_weapon_sn (victim, TRUE));
   			  ch_vict_weapon = get_weapon_skill (ch, get_weapon_sn (victim, TRUE));
			}
			
			/* modifiers */
			
			/* skill */
			if (get_eq_char (ch, WEAR_WIELD) == NULL)
			{
				if (get_eq_char (ch, WEAR_SECONDARY) == NULL)
					chance = chance * hth / 150;
				else
					chance = chance * ch_weapon / 100;
			}
			
			chance += (ch_vict_weapon / 2 - vict_weapon) / 2;
			
			/* dex vs. strength */
			chance += get_curr_stat (ch, STAT_DEX);
			chance -= 2 * get_curr_stat (victim, STAT_STR);
			
			/* level */
			chance += (ch->level - victim->level) * 2;
			
			if (IS_FLAG (victim->states.res, _DISARM))
				chance /= 2;
			if (IS_FLAG (victim->states.vuln, _DISARM))
				chance = chance * 1.5;
			
			/* and now the attack */
			if (number_percent () < chance)
			{
				WAIT_STATE (ch, skill_table[gsn_disarm].beats);
				disarm (ch, victim);
				check_improve (ch, gsn_disarm, TRUE, 1);
			}
			else
			{
				WAIT_STATE (ch, skill_table[gsn_disarm].beats);
				act ("You fail to disarm $N.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				act ("$n tries to disarm you, but fails.", ch, NULL, victim, TO_VICT,
					POS_RESTING);
				act ("$n tries to disarm $N, but fails.", ch, NULL, victim, TO_NOTVICT,
					POS_RESTING);
				check_improve (ch, gsn_disarm, FALSE, 1);
			}
			return;
}

/** Function: mob_chase
  * Descr   : Lets a mob, damaged by a character from another room, chase
  *         : that character to the room they were damaged from
  * Returns : void
  * Syntax  : mob_chase( ch who was hit )
  * Written : v1.0 01/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void mob_chase (CHAR_DATA * ch)
{
	EXIT_DATA *pExit;
	sh_int door;
	
	if (ch->position < POS_FIGHTING || !ch->fighting || ch->attacked_from == 0)
		return;
	
	for (door = 0; door < 6; door++)
	{
		pExit = ch->in_room->exit[door];
		
		if (pExit
			&& pExit->u1.to_room->vnum == ch->attacked_from
			&& can_see_room (ch, pExit->u1.to_room))
		{
			move_char (ch, door, FALSE);
			WAIT_STATE (ch, PULSE_VIOLENCE);
			return;
		}
	}
	
}


_DOFUN (do_flee)
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	CHAR_DATA *victim;
	int attempt;
	
	if ((victim = ch->fighting) == NULL)
	{
		if (ch->position == POS_FIGHTING)
			ch->position = POS_STANDING;
		send_to_char ("You aren't fighting anyone.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch) && IS_AFFECTED (ch, _AFF_MARTYRDOM))
	{
		send_to_char ("Martyrs never flee.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED(ch, _AFF_LACERATED) && number_range(0, 1) == 1)
	{
		send_to_char("The wound in your leg hurts too much. You cant flee!\n\r", ch);
		return;
	}

	was_in = ch->in_room;
	for (attempt = 0; attempt < 6; attempt++)
	{
		EXIT_DATA *pexit;
		int door;
		
		door = number_door ();
		if ((pexit = was_in->exit[door]) == 0
			|| pexit->u1.to_room == NULL
			|| IS_SET (pexit->exit_info, EX_CLOSED)
			|| number_range (0, ch->daze) != 0
			|| (IS_NPC (ch)
			&& IS_SET (pexit->u1.to_room->room_flags, ROOM_NO_MOB)))
			continue;
		
		move_char (ch, door, FALSE);
		if ((now_in = ch->in_room) == was_in)
			continue;
		
		ch->in_room = was_in;
		act ("$n has fled!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		ch->in_room = now_in;
		
		if (!IS_NPC (ch))
		{
			send_to_char ("You flee from combat!\n\r", ch);
			if ((ch->c_class == 2) && (number_percent () < 3 * (ch->level / 2)))
				send_to_char ("You snuck away safely.\n\r", ch);
			else
			{
				send_to_char ("You lost 10 exp.\n\r", ch);
				gain_exp (ch, -10);
			}
		}
		
		stop_fighting (ch, TRUE);
		return;
	}
	
	send_to_char ("PANIC! You couldn't escape!\n\r", ch);
	return;
}


_DOFUN (do_surrender)
{
	CHAR_DATA *mob;
	if ((mob = ch->fighting) == NULL)
	{
		send_to_char ("But you're not fighting!\n\r", ch);
		return;
	}
	act ("You surrender to $N!", ch, NULL, mob, TO_CHAR, POS_RESTING);
	act ("$n surrenders to you!", ch, NULL, mob, TO_VICT, POS_RESTING);
	act ("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT,
		POS_RESTING);
	stop_fighting (ch, TRUE);
	
	if (!IS_NPC (ch) && IS_NPC (mob)
		&& (!HAS_TRIGGER (mob, TRIG_SURR)
		|| !mp_percent_trigger (mob, ch, NULL, NULL, TRIG_SURR)))
	{
		act ("$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR,
			POS_RESTING);
		multi_hit (mob, ch, TYPE_UNDEFINED);
	}
}

/* Bowfire code 
 *  Originally from Tch at Feudal Realms
 *  Used with permission.
 */
_DOFUN (do_fire)
{
	char arg[MIL];
	CHAR_DATA *victim = NULL;
	OBJ_DATA *arrow;
	OBJ_DATA *bow;
	ROOM_INDEX_DATA *was_in_room = NULL;
	EXIT_DATA *pexit;
	int dam=0, door=0, chance=0;
	
	
	bow = get_eq_char (ch, WEAR_WIELD);
	if (bow == NULL)
	{
		send_to_char ("What are you going to do, throw the arrow at them?\n\r",
			ch);
		return;
	}
	
	if (bow->value[0] != WEAPON_BOW)
	{
		send_to_char ("You might want to use a bow to fire with.\n\r",
			ch);
		return;
	}
	
	one_argument (argument, arg);
	if (arg[0] == '\0' && ch->fighting == NULL)
	{
		send_to_char ("Fire at...?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "none") || !str_cmp (arg, "self") || victim == ch)
	{
		send_to_char
			("How exactly did you plan on firing at yourself?\n\r", ch);
		return;
	}
	
	if ((arrow = get_eq_char (ch, WEAR_HOLD)) == NULL)
	{
		do_draw (ch, "");
		arrow = get_eq_char (ch, WEAR_HOLD);
	}
	
	if (arrow == NULL)
	{
		send_to_char ("You hold nothing in your hand.\n\r", ch);
		return;
	}
	
	if (arrow->item_type != ITEM_ARROW)
	{
		send_to_char ("You can only a fire arrows or quarrels.\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		if (ch->fighting != NULL)
		{
			victim = ch->fighting;
		}
		else
		{
			send_to_char ("Fire at...?\n\r", ch);
			return;
		}
	}
	else
	{
		
		/* See if who you are trying to shoot at is nearby... */
		if ((victim = get_char_room (ch, arg)) == NULL)
		{
			was_in_room = ch->in_room;
			
			for (door = 0; door <= 5 && victim == NULL; door++)
			{
				if ((pexit = was_in_room->exit[door]) != NULL
					&& pexit->u1.to_room != NULL
					&& pexit->u1.to_room != was_in_room
					&& !strcmp (pexit->u1.to_room->area->name,
					was_in_room->area->name))
				{
					ch->in_room = pexit->u1.to_room;
					victim = get_char_room (ch, arg);
				}
				
			}
			
			ch->in_room = was_in_room;
			
			if (victim == NULL)
			{
				send_to_char ("Where exactly are they?\n\r", ch);
				return;
			}
			else
			{
				if (pexit && IS_SET (pexit->exit_info, EX_CLOSED))
				{ // same fail msg so player doesnt learn where target is
					send_to_char ("Where exactly are they?", ch);
					return;
				}
			}
		}
	}
	
	// just to make sure..
	if (victim == NULL ||
			!can_see (ch, victim))
	{
		send_to_char ("Where exactly are they?\n\r", ch);
		return;
	}
	
	if ((ch->in_room) == (victim->in_room))
	{
		send_to_char
			("Don't you think that standing a bit further away would be wise?\n\r",
			ch);
		return;
	}
	
	/* Lag the bowman... */
	WAIT_STATE (ch, PULSE_VIOLENCE);
	
	/* Fire the damn thing finally! */
	if (arrow->item_type == ITEM_ARROW)
	{
		
		/* Valid target? */
		if (victim != NULL)
		{
			act ("$n fires $p at $N.", ch, arrow, victim, TO_NOTVICT, POS_RESTING);
			act ("You fire $p at $N.", ch, arrow, victim, TO_CHAR, POS_RESTING);
			act ("$n fires $p at you!", ch, arrow, victim, TO_VICT, POS_RESTING);
		}
		
		/* Did it hit? */
		if (ch->level < arrow->level
			|| number_percent () >= 20 + get_skill (ch, gsn_bow) * 4 / 5)
		{
			/* denied... */
			act ("You missed!", ch, arrow, NULL, TO_CHAR, POS_RESTING);
			act ("$n missed!", ch, arrow, NULL, TO_ROOM, POS_RESTING);
			obj_from_char (arrow);
			obj_to_room (arrow, victim->in_room);
			check_improve (ch, gsn_bow, FALSE, 2);
		}
		else
		{
			chance = dice (1, 10);
			if (chance < 10 && 
				  get_skill (victim, gsn_juggling) > number_percent() &&
					victim->daze < 1 &&
					victim->position >= POS_RESTING)
			{
				/* blocked...*/
				act ("Your shot was knocked out of the air!", ch, arrow, NULL, TO_CHAR, POS_RESTING);
				act ("$N knocks the $p out of the air!", ch, arrow, victim, TO_NOTVICT, POS_RESTING);
				act ("You knock the $p out of the air!", ch, arrow, victim, TO_VICT, POS_RESTING);		
				obj_from_char (arrow);
				obj_to_room (arrow, victim->in_room);
				check_improve (ch, gsn_bow, FALSE, 2);
				check_improve (victim, gsn_juggling, TRUE, 6);
			}
			else
			{
				switch (chance)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
						dam = dice (arrow->value[1], arrow->value[2]);
						break;
					case 7:
					case 8:
					case 9:
						dam = 3 * (dice (arrow->value[1], arrow->value[2])) / 2;
						break;
					case 10:
						dam = 2 * (dice (arrow->value[1], arrow->value[2]));
				}  // end of switch
				obj_from_char (arrow);
				obj_to_char (arrow, victim);
				arrow->wear_flags = ITEM_TAKE | ITEM_WEAR_LODGED;
				equip_char (victim, arrow, WEAR_LODGED_R);
				SET_BIT (arrow->extra_flags, ITEM_LODGED);
				damage (ch, victim, dam, gsn_bow, DAM_PIERCE, TRUE, TRUE);
				check_improve (ch, gsn_bow, TRUE, 2);
			} // end of else
		}
	}
	return;
}

// hurling code, roughly taken from bowfire code above 
_DOFUN (do_hurl)
{
	char arg[MIL];
	CHAR_DATA *victim = NULL;
	OBJ_DATA *blade;
	ROOM_INDEX_DATA *was_in_room = NULL;
	EXIT_DATA *pexit;
	int dam, door, chance, skill, hit_where;
		
	one_argument (argument, arg);
	if (arg[0] == '\0' && ch->fighting == NULL)
	{
		send_to_char ("Hurl it at...?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "none") || !str_cmp (arg, "self"))
	{
		send_to_char
			("How exactly did you plan on hurling it at yourself?\n\r", ch);
		return;
	}
	
	if ((blade = get_eq_char (ch, WEAR_WIELD)) == NULL)
		blade = get_eq_char (ch, WEAR_SECONDARY);

	if (blade == NULL)
	{
		send_to_char ("You have nothing in your hands to hurl.\n\r", ch);
		return;
	}
	
	if (blade->item_type != ITEM_WEAPON)
	{
		chprintf (ch, "You can't hurl %s!  It's not a weapon!\n\r", blade->short_descr);
		return;
	}

	if (IS_OBJ_STAT (blade, ITEM_NODROP) ||
			IS_OBJ_STAT (blade, ITEM_NOREMOVE))
	{
		chprintf (ch, "You can't hurl %s!  No Way!\n\r", blade->short_descr);
		return;
	}

	if (blade->weight > get_curr_stat (ch, STAT_STR))
	{
		chprintf (ch, "You are not strong enough to hurl %s!\n\r", blade->short_descr);
		return;
	}

	if (arg[0] == '\0')
	{
		if (ch->fighting != NULL)
		{
			victim = ch->fighting;
		}
		else
		{
			send_to_char ("Hurl it at...?\n\r", ch);
			return;
		}
	}
	else
	{
		/* See if who you are trying to shoot at is nearby... */
		if ((victim = get_char_room (ch, arg)) == NULL)
		{
			was_in_room = ch->in_room;
			
			for (door = 0; door <= 5 && victim == NULL; door++)
			{
				if ((pexit = was_in_room->exit[door]) != NULL
					&& pexit->u1.to_room != NULL
					&& pexit->u1.to_room != was_in_room
					&& !strcmp (pexit->u1.to_room->area->name,
					was_in_room->area->name))
				{
					ch->in_room = pexit->u1.to_room;
					victim = get_char_room (ch, arg);
				}
				
			}
			
			ch->in_room = was_in_room;
			
			if (victim == NULL)
			{
				send_to_char ("Where exactly are they?\n\r", ch);
				return;
			}
			else
			{
				if (pexit && IS_SET (pexit->exit_info, EX_CLOSED))
				{
					// make same msg because otherwise char knows they are here
					send_to_char ("Where exactly are they?", ch);
					return;
				}
			}
		}
	}
	
	// just to make sure..
	if (victim == NULL ||
			!can_see (ch, victim))
	{
		send_to_char ("Where exactly are they?\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char
			("How exactly did you plan on hurling it at yourself?\n\r", ch);
		return;
	}

	/* Lag the hurler... */
	if (blade->value[0] == WEAPON_DAGGER &&
		  ch->pcdata->talents[gtn_knives] > 0 &&
		  number_percent() < get_curr_stat(ch, STAT_DEX) * 3)
	{
		act ("In a blur, $n hurls $p at $N.", ch, blade, victim, TO_NOTVICT, POS_RESTING);
		act ("In a blur, you hurl $p at $N.", ch, blade, victim, TO_CHAR, POS_RESTING);
		act ("In a blur, $n hurls $p at you!", ch, blade, victim, TO_VICT, POS_RESTING);
	}
	else
	{
  	WAIT_STATE (ch, PULSE_VIOLENCE);
		act ("$n hurls $p at $N.", ch, blade, victim, TO_NOTVICT, POS_RESTING);
		act ("You hurl $p at $N.", ch, blade, victim, TO_CHAR, POS_RESTING);
		act ("$n hurls $p at you!", ch, blade, victim, TO_VICT, POS_RESTING);
	}
	
	// low rolls are better
	chance = number_percent() +
					 GET_AC (victim, AC_EXOTIC)/10;
	dam = dice (blade->value[1], blade->value[2]);
	skill = get_skill (ch, gsn_hurl) / 2;
	if (skill > blade->weight)
		skill -= blade->weight;
	else
		skill = 1;
	if ((ch->in_room) == (victim->in_room))
	{
		// in the room is much easier and hits harder
		dam *= 2;
		skill += 20;	
	}
	else
	{
		// extra weight penalty for further
		if (skill > blade->weight)
			skill -= blade->weight;
		else
			skill = 1;
	}

	if (blade->value[0] == WEAPON_DAGGER &&
		  ch->pcdata->talents[gtn_knives] > 0)
	{
		skill += 25;
		dam *= 2;
	}

	/* Did it hit? */
	if (ch->level < blade->level ||
		  chance > skill ||
			chance > 95)
	{
		/* denied... */
		act ("You missed!", ch, blade, NULL, TO_CHAR, POS_RESTING);
		act ("$n missed!", ch, blade, NULL, TO_ROOM, POS_RESTING);
		obj_from_char (blade);
		obj_to_room (blade, victim->in_room);
		check_improve (ch, gsn_hurl, FALSE, 2);
	}
	else
	{  // reuse chance
		chance = URANGE (1, (skill-chance) / 10, 10);
		if (!IS_NPC(victim) &&
			  chance < 10 && 
			  get_skill (victim, gsn_juggling) > number_percent() &&
				victim->daze < 1 &&
				victim->position >= POS_RESTING)
		{
			/* blocked...*/
			act ("$N knocked $p out of the air!", ch, blade, victim, TO_CHAR, POS_RESTING);
			act ("$N knocks $p out of the air!", ch, blade, victim, TO_NOTVICT, POS_RESTING);
			act ("You knock $p out of the air!", ch, blade, victim, TO_VICT, POS_RESTING);		
			obj_from_char (blade);
			obj_to_room (blade, victim->in_room);
			check_improve (ch, gsn_hurl, FALSE, 2);
			check_improve (victim, gsn_juggling, TRUE, 6);
		}
		else
		{
			hit_where=WEAR_LODGED_A;
			switch (chance)
			{
				case 1:
				case 2:
				case 3:
					break;
				case 4:
				case 5:
				case 6:
					hit_where=WEAR_LODGED_L;
					dam += 10;
					break;
				case 7:
				case 8:
				case 9:
					hit_where=WEAR_LODGED_R;
					dam = (dam+10)*2;
					break;
				case 10:
					hit_where=WEAR_LODGED_R;
					dam = (dam+10)*3;
			}  // end of switch
			obj_from_char (blade);
			obj_to_char (blade, victim);
			blade->wear_flags = ITEM_TAKE | ITEM_WEAR_LODGED;
			equip_char (victim, blade, hit_where);
			SET_BIT (blade->extra_flags, ITEM_LODGED);
			damage (ch, victim, dam, gsn_hurl,
			attack_table[blade->value[3]].damage, TRUE, TRUE);
			check_improve (ch, gsn_hurl, TRUE, 2);
		} // end of not blocked
	} // end of not missed
	return;
}
