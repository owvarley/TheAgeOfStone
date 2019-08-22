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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/** Function: do_lacerate
  * Descr   : For assassins. Confers one of several maladicts to victim
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 11/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>, Phil Shortman
  */
_DOFUN(do_lacerate)
{
	CHAR_DATA *victim = NULL;
	OBJ_DATA *wield = NULL;
	char arg[MIL];

	memset(arg, 0, MIL);
	
	if (!can_fight (ch, TRUE) || !IS_VALID_SKILL(gsn_lacerate) )
	  return;

	if (get_skill(ch, gsn_lacerate) < 1)
	{
		chprintf(ch, "I'm sure that would leave an ugly scar...\n\r");
		return;
	}

	argument = one_argument (argument, arg);

	if ( !IS_NULLSTR(arg) )
	  victim = get_char_room(ch, arg);
	if ( !victim )
	  victim = ch->fighting;

	if (!victim)
	{
		chprintf(ch, "Who? I can't seem to find that one...\n\r");
		return;
	}

	if (is_safe (ch, victim))
	{
		chprintf(ch, "Not here.\n\r");
		return;
	}

	
	wield = get_eq_char(ch, WEAR_WIELD);
	if ( !wield )
		wield = get_eq_char(ch, WEAR_SECONDARY);

	if ( !wield )
	{
		chprintf(ch, "With WHAT, exactly?\n\r");
		return;
	}

	if ( attack_table[ wield->value[3] ].damage != DAM_SLASH )
	{
		chprintf(ch, "Not with that weapon you wont.\n\r");
		return;
	}

	if ( make_skill_roll(gsn_lacerate, ch->fighting == NULL ? 1.0 : 2.0) )
	{
		int dam = 0, roll = number_range(0, 7);
		AFFECT_DATA af;

		af.where = TO_AFFECTS;
		af.type = gsn_lacerate;
		af.level = ch->level;
		af.duration = UMIN(3, ch->level/5);
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.owner = GET_ID(ch);

		switch ( roll )
		{
			case 0:		
				dam = 20;
				af.bitvector = _AFF_WEAKEN;
				act("$N seems to suddenly go slack with fatigue as $n's attack cuts $M open.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("The strength seeps out of your body as $n's attack cuts you open.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("Your attack leaves a deep gouge in $N's $B.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			case 1:		
				dam = 20;
				af.bitvector = _AFF_SLOW; 
				act("$N moves in slow motion as $n's attack lacerates $M.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("You cant seem to move fast enough after $n's attack lacerates you.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("Your attack leaves deep lacerations in $N's $B.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			case 2:		
				dam = 20;
				af.bitvector = _AFF_DEAF; 
				act("$N shakes $S head to clear the blood from $S ears after $n's slashing attack.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("You shake your head to clear the blood from your ears after $n's slashing attack.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("You slice $N's ear deeply, leaving blood pouring into $S ear.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			case 3:		
				dam = 20;
				af.bitvector = _AFF_MUTE; 
				act("Blood flies from $N's mouth after $n's razor sharp attack.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("Blood flies from your mouth after $n's razor sharp attack.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("Your attack cuts deep into $N's mouth.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			case 4:		
				dam = 40;
				af.bitvector = _AFF_WOUNDED; 
				af.duration = -1; // must be 'healed'
				act("$n's blistering attack leaves $N with a gaping wound.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("$n's blistering attack leaves you with a gaping wound.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("Your blistering attack leaves a gaping wound in $N.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			case 5:
			case 6:		
				dam = 40;
				af.bitvector = _AFF_LACERATED; 
				act("$n's razor sharp slice cuts deep into $N's leg.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("$n's razor sharp slice cuts deep into your leg.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("Your razor sharp slice cuts deep into $N's leg.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;
			default:	
				dam = 20;
				af.bitvector = _AFF_BLIND; 
				act("$N shakes $S head to clear the blood from $S eyes after $n's slashing attack.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
				act("You shake your head to clear the blood from your eyes after $n's slashing attack.", ch, NULL, victim, TO_VICT, POS_RESTING);
				act("You slice deep into $N's forehead, blood flows into $S eyes.", ch, NULL, victim, TO_CHAR, POS_RESTING);
				break;

		}


		dam += number_range(1, ch->level);
		dam += (get_skill(ch, gsn_lacerate) + get_weapon_skill(ch, get_weapon_sn(ch, FALSE)))/2;
		affect_to_char(victim, &af);
		damage(ch, victim, dam, gsn_lacerate, DAM_SLASH, TRUE, TRUE);
		check_improve(ch, gsn_lacerate, TRUE, skill_table[gsn_lacerate].rating);
		WAIT_STATE(ch, skill_table[gsn_lacerate].beats);
		return;
	}

	chprintf(ch, "You missed!\n\r");
	WAIT_STATE(ch, skill_table[gsn_lacerate].beats);
}

/** Function: do_treat
  * Descr   : Meant to be a replacement for "heal", for non channelers, 
  *         : applies "first-aid" to character.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 4/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_treat)
{
  AFFECT_DATA *paf;
  CHAR_DATA *victim;
  char arg[MIL];
  sh_int amt = 0;

  if (!can_fight (ch, TRUE))
    return;

  if (IS_NPC (ch))
  {
    send_to_char ("Only PC's can treat.\n\r", ch);
    return;
  }

  if (ch->pcdata->learned[gsn_treat] <= 0)
  {
    send_to_char ("I'm afraid you have no idea how to treat wounds.\n\r", ch);
    return;
  }

  argument = one_argument (argument, arg);

  if ((victim = get_char_room (ch, arg)) == NULL)
  {
    send_to_char ("Treat who for wounds?\n\r", ch);
    return;
  }

  if (victim->hit >= victim->max_hit)
  {
    act ("But... their isn't a scratch on $M!",
	 ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }

  if (victim->position == POS_FIGHTING)
  {
    act ("Better wait for the fight to finish!",
	 ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }

  if ((paf = affect_find (victim->affected, gsn_treat)) != NULL)
  {
//    if (paf->owner == GET_ID(ch))
//    {
      send_to_char ("The wounds have been treated!\n\r", ch);
      return;
//    }
  }

  if (number_percent () > get_skill (ch, gsn_treat) * .75)
  {
    send_to_char
      ("You are unable to determine how to treat these wounds.\n\r", ch);
    act ("$n tries to hide $s concern.", ch, NULL, victim, TO_VICT,
	 POS_MORTAL);
    check_improve (ch, gsn_treat, FALSE,
		   skill_table[gsn_treat].rating);
    return;
  }
  else
    check_improve (ch, gsn_treat, TRUE,
		   skill_table[gsn_treat].rating);

  if (ch != victim)
    act ("$n treats $N's wounds.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
  else
    act ("$n applies first aid to $s wounds.",
	 ch, NULL, victim, TO_NOTVICT, POS_RESTING);

  act ("You apply first aid to $N's wounds",
       ch, NULL, victim, TO_CHAR, POS_RESTING);
  act ("$n does $s best to treat your wounds",
       ch, NULL, victim, TO_VICT, POS_MORTAL);

  if (is_affected (victim, gsn_treat))
    affect_strip (victim, gsn_treat);

  amt = UMIN (victim->hit + 
	          (ch->level + number_range (0, get_skill (ch,gsn_treat))),
	      victim->max_hit);

  // more if treating someone else
  if (ch != victim)
    amt = UMIN (amt + number_range (0, ch->level), victim->max_hit);
  // more if healing talent
  if (!IS_NPC (ch) && ch->pcdata->talents[gtn_healing] > 0)
    amt = UMIN (amt + number_range (0, ch->level), victim->max_hit);

  paf = new_affect ();
  paf->where = TO_AFFECTS;
  paf->type = gsn_treat;
  paf->level = ch->level;
  paf->duration = 60 - ch->level;
  paf->modifier = 0;
  paf->location = APPLY_NONE;
  paf->bitvector = 0;
  paf->owner = GET_ID(ch);

  victim->hit = amt;

  // now add a little moves
  amt = victim->move + 10 + get_skill (ch, gsn_treat) * 
	    number_range (0, ch->level) / 100;
  victim->move = UMIN (victim->max_move, amt);

  affect_to_char (victim, paf);
  victim->wtimer = 0;
  update_pos (victim);

  return;

}				/* end: do_treat */

_DOFUN (do_heal)
{
  CHAR_DATA *mob;
  char arg[MIL];
  int cost, sn;
  WEAVE_FUN *spell;

  /* check for healer */
  for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
  {
    if (IS_NPC (mob) && IS_FLAG (mob->states.player, _ACT_HEALER))
      break;
  }

  if (mob == NULL)
  {
    send_to_char ("You can't do that here.\n\r", ch);
    return;
  }

  one_argument (argument, arg);

  if (arg[0] == '\0')
  {
    /* display price list */
    act ("$N says 'I offer the following curatvies:'",
	 ch, NULL, mob, TO_CHAR, POS_MORTAL);
    send_to_char ("  heal   : healing weave		50 gold\n\r", ch);
    send_to_char ("  blind  : cure blindness		20 gold\n\r", ch);
    send_to_char ("  disease: cure disease		15 gold\n\r", ch);
    send_to_char ("  poison : cure poison		25 gold\n\r", ch);
    send_to_char ("  refresh: restore endurance	5 gold\n\r", ch);
    send_to_char ("  ability: refresh ability	10 gold\n\r", ch);
    send_to_char (" Type heal <type> to be healed.\n\r", ch);
    return;
  }

  else if (!str_prefix (arg, "heal"))
  {
    spell = weave_heal;
    sn = skill_lookup("heal");
    cost = 5000;
  }

  else if (!str_prefix (arg, "blindness"))
  {
    spell = weave_restore_sight;
    sn = skill_lookup ("restore sight");
    cost = 2000;
  }

  else if (!str_prefix (arg, "disease"))
  {
    spell = weave_clear_disease;
    sn = skill_lookup ("clear disease");
    cost = 1500;
  }

  else if (!str_prefix (arg, "poison"))
  {
    spell = weave_clear_poison;
    sn = skill_lookup ("clear poison");
    cost = 2500;
  }

  else if (!str_prefix (arg, "ability") || !str_prefix (arg, "energize"))
  {
    spell = NULL;
    sn = -1;
    cost = 1000;
  }


  else if (!str_prefix (arg, "refresh") || !str_prefix (arg, "moves"))
  {
    spell = weave_refresh;
    sn = skill_lookup ("refresh");
    cost = 500;
  }

  else
  {
    act ("$N says 'Type 'heal' for a list of weaves.'",
	 ch, NULL, mob, TO_CHAR, POS_MORTAL);
    return;
  }

  if (cost > (ch->gold * 100 + ch->silver))
  {
    act ("$N says 'You do not have enough gold for my services.'",
	 ch, NULL, mob, TO_CHAR, POS_MORTAL);
    return;
  }

  WAIT_STATE (ch, PULSE_VIOLENCE);

  deduct_cost (ch, cost);
  mob->gold += cost;
  act ("$n lays $s hands on $N.", mob, NULL, ch, TO_ROOM, POS_SLEEPING);
  act ("$n lays $s hands on you.", mob, NULL, ch, TO_CHAR, POS_MORTAL);

  if (spell == NULL)
  {
    increase_OP (ch, dice (1, 4) + mob->level / 5, FALSE);
    send_to_char ("A warm glow passes through you.\n\r", ch);
    return;
  }

  spell (sn, mob->level, mob, ch, TARGET_CHAR, "", TRUE);
}


_DOFUN (do_search)
{
  OBJ_DATA *obj;
  int iSkill = skill_lookup("search");

  if (IS_NPC (ch))
    return;

  if (!can_fight (ch, TRUE))
    return;

  if (ch->pcdata->learned[iSkill] < 1)
  {
    send_to_char ("You search around clumsily.\n\r", ch);
    return;
  }

  if (number_percent () < ch->pcdata->learned[iSkill])
  {
    check_improve (ch, iSkill, TRUE, 4);
    send_to_char ("You search the room..\n\r", ch);

    for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
    {
      if (IS_SET (obj->extra_flags, ITEM_HIDDEN))
      {
	chprintf (ch, "You reveal %s\n\r", obj->short_descr);
	REMOVE_BIT (obj->extra_flags, ITEM_HIDDEN);
      }
    }

    send_to_char ("You have searched everywhere.\n\r", ch);
    WAIT_STATE (ch, 24);
  }
  else

    send_to_char ("You didn't uncover anything unusual.\n\r", ch);
  check_improve (ch, iSkill, FALSE, 4);
  WAIT_STATE (ch, 24);
  return;
}

bool check_read_stone (CHAR_DATA * ch)
{
  int iSkill = skill_lookup("read");
  CHAR_DATA *vch;

  vch = ch->desc->original ? ch->desc->original : ch->desc->character;

  if (IS_NPC (vch))
    return FALSE;

  if (number_percent () < vch->pcdata->learned[iSkill])
  {
    check_improve (vch, iSkill, TRUE, 4);
    return TRUE;
  }

  check_improve (vch, iSkill, FALSE, 4);
  WAIT_STATE (vch, 24);
  return FALSE;
}

_DOFUN (do_mend)
{
  char arg[MIL], arg1[MIL];
  OBJ_DATA *obj;

  if (get_skill(ch, gsn_mend) <= 0)
  {
    send_to_char ("I'm afraid you have no idea how to mend objects.\n\r", ch);
    return;
  }

  if (IS_NULLSTR (argument))
  {
    send_to_char ("Syntax:\n\r"
		  "\tmend                    - Will show this menu.\n\r"
		  "\tmend <object>           - Will mend <object>.\n\r"
		  "\tmend <object> condition - Will report the current condition of <object>.\n\r",
		  ch);
    return;
  }

  argument = one_argument (argument, arg);
  argument = one_argument (argument, arg1);

  if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
  {
    send_to_char
      ("How do you expect to mend something not in your inventory?\n\r", ch);
    return;
  }

  if (obj->condition >= 90)
  {
    send_to_char
      ("Object is allready in 90% or greater condition, you could only do it more harm.\n\r",
       ch);
    return;
  }

  if (IS_NULLSTR (arg1))
  {
    if (get_skill(ch, gsn_mend) > number_percent ())
    {
      int silver = obj->condition * 10;

      if (player_silver (ch) < silver)
      {
				send_to_char ("You need more coin for materials.\n\r", ch);
				return;
      }

      amt_from_player (ch, silver);

      obj->condition = (short)(obj->max_condition * .9);
      send_to_char ("You easily mend the object.\n\r", ch);
      check_improve (ch, gsn_mend, TRUE, 2);
    }
    else
    {
      send_to_char("Uh-oh, you failed to mend it and damaged it even worse!\n\r", ch);
      obj->max_condition *= .9;
      if (obj->condition >= obj->max_condition)
				obj->condition = obj->max_condition;
      check_improve (ch, gsn_mend, FALSE, 2);
    }
    return;
  }
  if (!str_cmp (arg1, "condition"))
  {
    chprintf (ch, "%s has a condition of %d.\n\r", obj->short_descr,
		    obj->condition);
    return;
  }
  return;
}

_DOFUN (do_hone)
{
  char arg[MIL], arg1[MIL];
  OBJ_DATA *obj;

  if (get_skill(ch, gsn_hone) <= 0)
  {
    send_to_char ("I'm afraid you have no idea how to hone objects.\n\r", ch);
    return;
  }

  if (IS_NULLSTR (argument))
  {
    send_to_char ("Syntax:\n\r"
		  "\thone                    - Will show this menu.\n\r"
		  "\thone <object>           - Will mend <object>.\n\r"
		  "\thone <object> condition - Will report the current condition of <object>.\n\r",
		  ch);
    return;
  }

  argument = one_argument (argument, arg);
  argument = one_argument (argument, arg1);

  if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
  {
    send_to_char
      ("How do you expect to hone something not in your inventory?\n\r", ch);
    return;
  }

	if (IS_OBJ_STAT (obj, ITEM_NONMETAL))
  {
		send_to_char ("You can only hone metal items.\n\r", ch);
		return;
  }

  if (obj->condition < 90)
  {
    send_to_char ("The object needs mending before you can hone it.\n\r", ch);
    return;
  }

  if (IS_NULLSTR (arg1))
  {
    if (get_skill(ch, gsn_hone) > number_percent ())
    {
      int silver = obj->level * 100;

      if (player_silver (ch) < silver)
      {
				send_to_char("You do not have enough cash for the materials.\n\r",ch);
				return;
      }
      if (IS_WEAPON_STAT (obj, WEAPON_SHARP))
      {
				send_to_char("You can't sharpen it any more then it already is.\n\r",ch);
				return;
      }

      amt_from_player (ch, silver);

			obj->value[4] ^= WEAPON_SHARP;
      
			send_to_char ("You hone the object to a razor edge.\n\r", ch);
      check_improve (ch, gsn_hone, TRUE, 2);
    }
    else
    {
      send_to_char("Uh-oh, you failed to hone it and damaged it!\n\r", ch);
      obj->max_condition *= .75;

      if (obj->condition >= obj->max_condition)
				obj->condition = obj->max_condition;

      check_improve (ch, gsn_hone, FALSE, 2);
    }
    return;
  }

  if (!str_cmp (arg1, "condition"))
  {
    chprintf (ch, "%s has a condition of %d.\n\r", obj->short_descr,
		    obj->condition);
		if (IS_WEAPON_STAT (obj, WEAPON_SHARP))
			chprintf (ch, "It is also very sharp.\n\r");
    return;
  }
  return;
}

_DOFUN (do_engrave)
{
  OBJ_DATA *obj;
  char arg[MIL], arg1[MIL], arg2[MIL];
  bool shortd;

  if (get_skill(ch, gsn_engrave) <= 0)
  {
    send_to_char ("You have no idea how to engrave objects.\n\r", ch);
    return;
  }

  if (IS_NULLSTR (argument))
  {
    send_to_char ("Syntax:\n\r"
		  "\tengrave <object> short <text> - Engraves <object>'s short description to be <text>.\n\r"
		  "\tengrave <object> long  <text> - Engraves <object>'s long description to be <text>.\n\r",
		  ch);
    return;
  }

  argument = one_argument (argument, arg);
  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
  {
    send_to_char
      ("You do not carry that object, how do you expect to engrave it?\n\r",
       ch);
    return;
  }

  if (IS_NULLSTR (arg1))
  {
    send_to_char ("Syntax:\n\r"
		  "\tengrave <object> short <text> - Engraves <object>'s short description to be <text>.\n\r"
		  "\tengrave <object> long  <text> - Engraves <object>'s long description to be <text>.\n\r",
		  ch);
    return;
  }

  if (!str_cmp (arg1, "short"))
    shortd = TRUE;
  else if (!str_cmp (arg1, "long"))
    shortd = FALSE;
  else
  {
    send_to_char ("Syntax:\n\r"
		  "\tengrave <object> short <text> - Engraves <object>'s short description to be <text>.\n\r"
		  "\tengrave <object> long  <text> - Engraves <object>'s long description to be <text>.\n\r",
		  ch);
    return;
  }

  if (IS_NULLSTR (arg2))
  {
    send_to_char ("What do you want to engrave it as?\n\r", ch);
    return;
  }

  if (player_silver (ch) < 1000)
  {
    send_to_char
      ("Sorry, but it takes 10 gold to engrave an item, you're broke.\n\r",
       ch);
    return;
  }

	if (IS_OBJ_STAT (obj, ITEM_NONMETAL))
  {
		send_to_char ("You can only engrave metal items.\n\r", ch);
		return;
  }

  if (get_skill (ch, gsn_engrave) > number_percent ())
  {
    amt_from_player (ch, 1000);

    if (shortd == TRUE)
      obj->short_descr = strip_codes (str_dup (arg2));
    else
      obj->description = strip_codes (str_dup (arg2));

    chprintf (ch, "Your object's short description is now, %s.\n\r",
		    obj->short_descr);
    check_improve (ch, gsn_engrave, TRUE, 2);
  }
  else
  {
    send_to_char
      ("You realize that you would have only damaged the object by trying to engrave it at this time, so you stop.\n\r",
       ch);
    check_improve (ch, gsn_engrave, FALSE, 2);
    return;
  }

  return;
}
