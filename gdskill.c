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
 * @@ Objective (gdskill.c)
 *
 * This file contains the skills that are specific to guilds. Players
 * must belong to the required guild to use these skills, and are given
 * (typically) the skills when guilded, or promoted.
 */




#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"



/** Function: is_bonded_to
  * Descr   : Determines if 'to' is bonded to 'ch'
  * Returns : TRUE/FALSE on success
  * Syntax  : is_bonded_to( ch, to )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool is_bonded_to (CHAR_DATA * ch, CHAR_DATA * to)
{
  int i = 0;

  if (IS_NPC (ch) || IS_NPC (to) ||
      ch->pcdata->bonds[0] == 0 || to->pcdata->bonds[0] == 0)
    return FALSE;

  for (i = 1; i < ch->pcdata->bonds[0] + 1; i++)
    if (ch->pcdata->bonds[i] == to->pcdata->id)
      return TRUE;

  return FALSE;
}


/** Function: add_bond
  * Descr   : Adds 'vch' to bonds listing of ch. Expanding size of bonds
  *         : as needed. Also adds ch to vch's bonds list as well.
  * Returns : void
  * Syntax  : add_bond ( bond holder, bonded )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void add_bond (CHAR_DATA * ch, CHAR_DATA * vch)
{
  int *memory = NULL;
  int pos = 0,	  // array pos of new addition
	  iOSize = 0, // original size	(in memory)
	  iNSize = 0; // new size		(in memory)

  if (IS_NPC (ch) || IS_NPC (vch))
    return;

  // the actual size of the array, is ch->pcdata->bonds[0] + 1 * sizeof(bonds)
  // the position we want to place the new bond id in, bonds[0] + 1;
  pos = ch->pcdata->bonds[0] + 1;
  iOSize = sizeof (ch->pcdata->bonds) * pos;
  iNSize = sizeof (ch->pcdata->bonds) * (pos + 1);

  /* Create temporary memory buffer, and copy ch bond list over to it */
  memory = alloc_mem (iNSize);
  memcpy(memory, ch->pcdata->bonds, iOSize);

  /* add vch->id to temporary memory buffer */
  memory[pos] = vch->pcdata->id;

  /* set size of new bond array */
  memory[0] = pos;

  /* free ch bond list, reallocate it to the new size and copy temp buffer
   * over to new ch bond list, freeing temp buffer when done.
   */
  free_mem (ch->pcdata->bonds, iOSize);
  ch->pcdata->bonds = alloc_mem (iNSize);
  memcpy (ch->pcdata->bonds, memory, iNSize);
  free_mem (memory, iNSize);
}


/** Function: delete_bonds
  * Descr   : Removes someone's bond from another. Only sets the id field
  *         : to -1. A field of -1 is not saved in the pfile.
  * Returns : void
  * Syntax  : delete_bonds( delete ch, from ch )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void delete_bonds (CHAR_DATA * ch, CHAR_DATA * from)
{
  int i = 0;

  return;

  if (ch->pcdata->bonds[0] == 0 || IS_NPC(ch) || IS_NPC(from) )
    return;

  for (i = 1; i < ch->pcdata->bonds[0] + 1; i++)
  {
    if (ch->pcdata->bonds[i] == from->pcdata->id)
    {
      ch->pcdata->bonds[i] = -1;
      break;
    }
  }

}


/** Function: rules_of_warfare
  * Descr   : Blightwatch guild skill that adds +5 hit/dam when fighting
  *         : shadowspawn
  * Returns : void
  * Syntax  : void
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_warfare)
{

  if (IS_NPC (ch))
    return;

  if (!is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, _BLIGHT_WATCH))
  {
    send_to_char ("I hear they teach that up in the Borderlands...\n\r", ch);
    return;
  }

  if (ch->fighting == NULL || !IS_EVIL (ch->fighting))
  {
    send_to_char ("Trolloc?!? Where?\n\r", ch);
    return;
  }

  if (IS_FLAG (ch->states.affected_by, _AFF_WARFARE))
  {
    send_to_char ("There is only so much you can teach!\n\r", ch);
    return;
  }

  if (number_percent () > ch->pcdata->learned[ skill_lookup("warfare") ])
  {
    send_to_char ("Rules of War? You heard something about those...\n\r", ch);
    return;
  }

  send_to_char ("Remembering the Rules of War, you attack the Shadowspawn\n\r"
		"at it's weakest points!\n\r", ch);

  act ("$n growls a low oath and closes in combat with the Shadowspawn!",
       ch, NULL, NULL, TO_ROOM, POS_RESTING);

  SET_FLAG (ch->states.affected_by, _AFF_WARFARE);

}


/** Function: Martyrdom
  * Descr   : Blightwatch guild skill that adds +7 hit/dam but wont
  *         : let you flee a fight
  * Returns : void
  * Syntax  : void
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_martyrdom)
{
  AFFECT_DATA af;

  if (IS_NPC (ch))
    return;

  if (!is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, _BLIGHT_WATCH))
  {
    send_to_char ("You? A Martyr? HA!\n\r", ch);
    return;
  }

  if (!ch->fighting)
  {
    send_to_char ("But... there's no one here trying to kill you!\n\r", ch);
    return;
  }

  if (IS_FLAG (ch->states.affected_by, _AFF_MARTYRDOM))
  {
    send_to_char ("Only one cause at a time! Sheesh!\n\r", ch);
    return;
  }

  if (number_percent () > ch->pcdata->learned[ skill_lookup("martyrdom") ])
  {
    send_to_char ("You don't *really* want to be a Martyr do you?\n\r", ch);
    return;
  }

  send_to_char
    ("Duty is as a mountain, and Death as a feather, but here and now\n\r"
     "both are as one!\n\r", ch);

  af.where = TO_AFFECTS;
  af.type = skill_lookup("martyrdom");
  af.level = ch->level;
  af.duration = UMAX (2, ch->level / 10 + 2);
  af.modifier = 7;
  af.location = APPLY_HITROLL;
  af.bitvector = _AFF_MARTYRDOM;
  af.owner = GET_ID(ch);
  affect_to_char (ch, &af);
  af.location = APPLY_DAMROLL;
  affect_to_char (ch, &af);

  act
    ("$n charges into the fight, roaring an oath as the blood begins to fly!",
     ch, NULL, NULL, TO_ROOM, POS_RESTING);
  act ("A man enraged, $n asks no quarter as he charges you!", ch, NULL,
       ch->fighting, TO_VICT, POS_RESTING);
  return;
}



/** Function: do_cloak( CHAR_DATA *ch, char *argument )
  * Descr   : Gaidin skill, involving wrapping their color shifting cloak
  *         : around them, slipping from view.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_cloak)
{
  AFFECT_DATA af;
  int iSkill = skill_lookup("cloak");

  if (IS_NPC (ch) ||
      !is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, _GAIDIN))
  {
    send_to_char ("huh?\n\r", ch);
    return;
  }

  if (get_obj_wear_vnum (ch, OBJ_VNUM_GAIDIN_CLOAK) == NULL)
  {
    send_to_char ("But your not wearing you cloak!\n\r", ch);
    return;
  }

  if (number_percent () < get_skill (ch, iSkill))
  {
    act ("$n dissapears into a color shifting cloak.",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
    check_improve (ch, iSkill, TRUE, 3);

    send_to_char ("You wrap your fancloth cloak around you.\n\r", ch);
    affect_strip (ch, iSkill);

    af.where = TO_AFFECTS;
    af.type = iSkill;
    af.level = (ch->level + ch->level / 4);
    af.duration = ch->level / 2;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.owner = GET_ID(ch);
    af.bitvector = _AFF_CLOAKED;
    affect_to_char (ch, &af);
    return;
  }
  send_to_char ("Your cloak flaps in the breeze.\n\r", ch);

  act ("$n's fancloth cloak flaps in the breeze,"
       " shrouding $m in a shifting pattern.",
       ch, NULL, NULL, TO_ROOM, POS_RESTING);

  check_improve (ch, iSkill, FALSE, 4);
  return;
}


/** Function: tiganza
  * Descr   : Tuatha'an skill. Essentially an imm level 'peace'.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_tiganza)
{
  int iSkill = skill_lookup("tiganza");

  if (!can_fight (ch, TRUE))
    return;

  if (!is_clan (ch) || ch->clan != clan_lookup ("Tinker"))
  {
    send_to_char ("You attempt the Tiganza, and trip over your own feet!\n\r",
		  ch);
    act ("$n attempts the Tiganza and trips over $s own feet!", ch, NULL,
	 NULL, TO_ROOM, POS_RESTING);
    return;
  }

  if (get_skill (ch, iSkill) * 2 > number_percent ())
  {
    CHAR_DATA *rch;

    send_to_char ("You sway wildly as you dance the Tiganza!\n\r", ch);
    act ("$n dances the Tiganza! Holding your gaze spellbound.",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
      if (rch->fighting != NULL)
	    stop_fighting (rch, TRUE);
      if (IS_NPC (rch) && IS_FLAG (rch->states.player, _ACT_AGGRESSIVE))
	    REMOVE_FLAG (rch->states.player, _ACT_AGGRESSIVE);
    }
	check_improve(ch, iSkill, TRUE, skill_table[iSkill].rating);
  }

}


/** Function: creators_wrath
  * Descr   : Whitecloak skill. Adds affects: haste/-armor
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_wrath)
{
  AFFECT_DATA af;
  int sn = 0;

  if (!IS_IMMORTAL (ch) || !IS_FLAG (clan_table[ch->clan].flags, _WHITECLOAK))
  {
    send_to_char ("Huh?\n\r", ch);
    return;
  }

  sn = skill_lookup ("wrath");

  if (number_percent () > get_skill (ch, sn) || is_affected (ch, sn))
  {
    send_to_char
      ("You call upon the Wrath of the Creator and his power almost"
       " burns you to a crisp!\n\r", ch);
    act ("$n calls upon the Wrath of the Creator, who ignores him!", ch, NULL,
	 NULL, TO_ROOM, POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = ch->level;
  af.duration = ch->level / 3;
  af.modifier = ch->level / 6;
  af.bitvector = 0;

  af.location = APPLY_HITROLL;
  affect_to_char (ch, &af);

  af.location = APPLY_DAMROLL;
  affect_to_char (ch, &af);

  af.modifier = 10 * (ch->level / 6);
  af.location = APPLY_AC;
  affect_to_char (ch, &af);

  af.duration = ch->level / 4;
  af.location = APPLY_DEX;
  af.modifier = 1 + (ch->level >= 18) + (ch->level >= 25) + (ch->level >= 32);
  af.bitvector = _AFF_HASTE;
  affect_to_char (ch, &af);

  send_to_char ("You are filled with the Creators wrath!\n\r", ch);
  act ("$n becomes filled with a divine wrath!", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);

  return;

}				/* end: wrath */


/** Function: do_blood
  * Descr   : Seanchan guild skill. Increases all 5 attributes for a 
  *         : short duration
  * Returns : void
  * Syntax  : void
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_blood)
{
  AFFECT_DATA af;
  int sn = -1;

  if (!IS_IMMORTAL (ch) && !IS_FLAG (clan_table[ch->clan].flags, _HAILENE))
  {
    send_to_char ("Huh?\n\r", ch);
    return;
  }

  sn = skill_lookup ("blood");

  if (number_percent () > get_skill (ch, sn) || is_affected (ch, sn))
  {
    send_to_char ("Even your `Rblood`w is only so strong!\n\r", ch);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = ch->level;
  af.duration = ch->level / 2;
  af.modifier = UMAX (1, ch->rank);
  af.bitvector = 0;

  af.location = APPLY_STR;
  affect_to_char (ch, &af);

  af.location = APPLY_INT;
  affect_to_char (ch, &af);

  af.location = APPLY_WIS;
  affect_to_char (ch, &af);

  af.location = APPLY_DEX;
  affect_to_char (ch, &af);

  af.location = APPLY_CON;
  affect_to_char (ch, &af);

  send_to_char ("The purity of your `Rblood`w lends you strength!\n\r", ch);
  act ("The purity of $n's `Bblood`w lends $m strength!",
       ch, NULL, NULL, TO_ROOM, POS_RESTING);

  return;
}


/** Function: do_unbond
  * Descr   : Guild skill for Aes Sedai and Asha'man that releases a bonded
  *         : person from them. Deletes the bond both ways.
  * Returns : (N/A)
  * Syntax  : release (who)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_unbond)
{
  CHAR_DATA *victim = NULL;
  int sn = -1;

  if (!IS_FLAG (clan_table[ch->clan].flags, _SEDAI) && !IS_IMMORTAL (ch))
  {
    send_to_char ("Huh?\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, argument)) != NULL)
  {
    if (victim == ch)
    {
      send_to_char
	("Dont be foolish, you cannot release yourself from a bond!\n\r", ch);
      return;
    }

    if (!victim->desc)
    {
      send_to_char ("Wait for them to connect!\n\r", ch);
      return;
    }

    if (!is_bonded_to (ch, victim))
    {
      act ("You do not hold $S bond.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
      return;
    }

    delete_bonds (victim, ch);
    delete_bonds (ch, victim);

    act ("You release $N from $S bond.",
	 ch, NULL, victim, TO_CHAR, POS_RESTING);

    chprintf (victim, "%s releases you from your bond.\n\r", ch->name);

    if (ch->pcdata->bonds[0] == 0)
      REMOVE_FLAG (ch->states.comm, _BOND);

    if (victim->pcdata->bonds[0] == 0)
      REMOVE_FLAG (victim->states.comm, _BOND);

    sn = skill_lookup ("sense evil");
    if (sn > 0)
    {
      if (ch->pcdata->bonds[0] == 0)
      {
	ch->pcdata->learned[sn] = 0;
	affect_strip (ch, sn);
	chprintf (ch, "With your bond to %s broken, your senses "
			"return to normal.\n\r", victim->name);
      }
      if (victim->pcdata->bonds[0] == 0)
      {
	victim->pcdata->learned[sn] = 0;
	affect_strip (victim, sn);
	chprintf (victim, "With your bond to %s broken, you feel"
			" different, somehow...less.\n\r", ch->name);
      }
    }
  }				/* end: victim != NULL */
  else
    send_to_char ("They are not here.\n\r", ch);

  return;

}				/* end: do_unbond */



/** Function: do_bond
  * Descr   : Aes Sedai skill that bonds Warders(Gaidin) to them
  * Returns : (void)
  * Syntax  : bond <who>
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_bond)
{
  CHAR_DATA *victim;

  if ((!IS_FLAG (clan_table[ch->clan].flags, _SEDAI)
       && !IS_FLAG (clan_table[ch->clan].flags, _ASHAMAN))
      && !IS_IMMORTAL (ch))
  {
    send_to_char ("Whatever puny knowledge you think you have of bonding "
		  "child, will likely destroy you both!\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, argument)) != NULL)
  {

    if (IS_FLAG (ch->states.penalties, _NOBOND))
    {
      send_to_char ("Your ability to bond another has been denied.\n\r", ch);
      return;
    }

    if (victim == ch)
    {
      send_to_char ("Dont be foolish, you cannot bond yourself!\n\r", ch);
      return;
    }

    if (IS_NPC (victim))
    {
      send_to_char ("You cannot bond NPC's!\n\r", ch);
      return;
    }

    if (!victim->desc)
    {
      send_to_char ("Wait for them to connect!\n\r", ch);
      return;
    }

    if (IS_IMMORTAL (victim) && ch->level < victim->level)
    {
      send_to_char ("You would like that, wouldn't you?\n\r", ch);
      chprintf (victim, "%s just tried to bond you!\n\r", ch->name);
      return;
    }

    if (is_bonded_to (ch, victim))
    {
      chprintf (ch, "%s is already bonded to you!\n\r", victim->name);
      return;
    }

    if (victim->pcdata->bonds[0] != 0 && !IS_IMMORTAL (ch))
    {
      chprintf (ch, "%s is already bonded to another.\n\r",
		      victim->name);
      return;
    }

    add_bond (ch, victim);
    add_bond (victim, ch);

    if (!IS_FLAG (ch->states.comm, _BOND))
      SET_FLAG (ch->states.comm, _BOND);

    if (!IS_FLAG (victim->states.comm, _BOND))
      SET_FLAG (victim->states.comm, _BOND);


    if (IS_FLAG (clan_table[ch->clan].flags, _ASHAMAN))
    {
      act ("$n has bonded you!", ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You have bonded $N.", ch, NULL, victim, TO_CHAR, POS_RESTING);
    }
    else
       if (IS_FLAG (clan_table[ch->clan].flags, _SEDAI)
	   && (!IS_AFFECTED (ch, _AFF_SENSE_EVIL)
	       && !is_affected (ch, skill_lookup("sense evil"))))
    {
      int sn;
      AFFECT_DATA af;

      act ("$n has bonded you as $s warder.",
	   ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You bond $N as your warder.",
	   ch, NULL, victim, TO_CHAR, POS_RESTING);

      sn = skill_lookup ("sense evil");
      if (sn > 0)
      {
		ch->pcdata->learned[sn] = 100;
		victim->pcdata->learned[sn] = 100;
      }
      af.where = TO_AFFECTS;
      af.type = sn;
      af.level = 60;
      af.duration = -1;		/* permanent! */
      af.modifier = 0;
      af.location = APPLY_NONE;
      af.bitvector = _AFF_SENSE_EVIL;
      affect_to_char (ch, &af);
      affect_to_char (victim, &af);
      send_to_char
	("Your eyes tingle as you become more aware of your surroundings.\n\r",
	 ch);
      send_to_char
	("Your eyes tingle as you become more aware of your surroundings.\n\r",
	 victim);
    }
    else

    {
      act ("$n has bonded you!", ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You have bonded $N.", ch, NULL, victim, TO_CHAR, POS_RESTING);
    }

    return;

  }				/* found victim */
  else
    send_to_char ("They are not here.\n\r", ch);
  return;

}				/* end: do_bond */



/** Function: do_spear_dance
  * Guild   : Aiel
  * Descr   : Attacks all mobs/pc's in the room who are not currently
  *         : grouped with user.
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.0 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_speardance)
{
  CHAR_DATA *pChar;
  CHAR_DATA *pChar_next;
  OBJ_DATA *wield;
  bool found = FALSE;
  int roll = 0;
  int iSkill = skill_lookup("speardance");
  
  if (!can_fight (ch, TRUE))
	  return;
  
  if (!IS_FLAG (clan_table[ch->clan].flags, _AIEL) && !IS_IMMORTAL (ch))
  {
	  send_to_char ("Think your an Aiel now eh? Heh...\n\r", ch);
	  return;
  }
  
  if (!IS_NPC (ch)
	  && ch->level < skill_table[ iSkill ].skill_level[ch->c_class])
  {
	  send_to_char ("You don't know how to do that...\n\r", ch);
	  return;
  }
  
  if ((wield = get_eq_char (ch, WEAR_WIELD)) == NULL)
	  send_to_char ("Unarmed, the dance will be more exciting!\n\r", ch);
  
  act ("$n draws $s veil across $s face and leaps to the dance!",
	  ch, NULL, NULL, TO_ROOM, POS_RESTING);
  
  send_to_char
	  ("You draw your veil across you face and leap to the dance!\n\r", ch);
  
  for (pChar = ch->in_room->people; pChar; pChar = pChar_next)
  {
	  pChar_next = pChar->next_in_room;
	  if (pChar != ch && !is_same_group (ch, pChar))
	  {
		  found = TRUE;
		  act ("$n turns to dance with you!", ch, NULL, pChar, TO_VICT, POS_DEAD);
		  one_hit (ch, pChar, iSkill, FALSE);
		  check_improve (ch, iSkill, TRUE, skill_table[iSkill].rating);
		  
		  /* hit them again? */
		  roll = get_skill (ch, gsn_third_attack) / 2;
		  
		  if (IS_AFFECTED (ch, _AFF_SLOW))
			  roll /= 2;
		  if (number_percent () < roll)
			  one_hit (ch, pChar, iSkill, FALSE);
		  
		  roll = get_skill (ch, gsn_fifth_attack) / 3;
		  if (IS_AFFECTED (ch, _AFF_SLOW))
			  roll /= 2;
		  if (number_percent () < roll)
		  {
			  one_hit (ch, pChar, iSkill, FALSE);
			  check_improve (ch, iSkill, TRUE,
				  skill_table[iSkill].rating);
				  //skill_table[iSkill].rating[ch->c_class]);
		  }
	  }
  }
  
  if (!found)
	  send_to_char("You appear confused as you realize there's no one to fight!", ch);
  
  WAIT_STATE (ch, skill_table[iSkill].beats);
  
  return;
}




/** Function: do_call_wolf
  * Descr   : Sei'Car guild skill. Summons level/10 wolves to the aid
  *         : of the wolf brother.
  * Returns : Wolves!
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_call_wolf)
{
  int count = 0;
  int i;
  int sn = -1;
  CHAR_DATA *wolf;
  CHAR_DATA *gch;

  AFFECT_DATA af;

  if (IS_NPC (ch) && !IS_IMMORTAL (ch))
  {
    send_to_char ("I dont think so.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL (ch) && !IS_FLAG (clan_table[ch->clan].flags, _WOLF))
  {
    send_to_char ("huh?\n\r", ch);
    return;
  }

  /* Dont let them keep calling wolves! */
  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (IS_NPC (gch) && gch->pIndexData->vnum == MOB_VNUM_WOLF
	&& is_same_group (ch, gch))
    {
      send_to_char ("You have called the wolves to your aid already.\n\r",
		    ch);
      return;
    }
  }

  if (!IS_OUTSIDE (ch))
  {
    send_to_char ("You hear the howl of wolves in the distance, but they\n\r",
		  ch);
    send_to_char ("are reluctant to come indoors.\n\r", ch);
    return;
  }

  /* ok, char is either immortal, or is a member of the 
     Sei'car and is outside */

  count = (int) ch->level / 10;

  if (count <= 0)
    count = 1;

  if (get_mob_index (MOB_VNUM_WOLF) == NULL)
  {
    send_to_char
      ("Warning. Unable to load wolves. Please inform your guild leader.\n\r",
       ch);
    bugf ("[gdskill.c::do_call_wolf] Unable to load wolf. Vnum #%d non-existant.",
	  MOB_VNUM_WOLF);
    return;
  }

  sn = skill_lookup ("compell");

  for (i = 0; i < count; i++)
  {
    wolf = create_mobile (get_mob_index (MOB_VNUM_WOLF));
    char_to_room (wolf, ch->in_room);

    add_follower (wolf, ch, TRUE);
    wolf->leader = ch;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = ch->level;
    af.duration = number_fuzzy (ch->level / 4);
    af.location = 0;
    af.modifier = 0;
    af.bitvector = _AFF_COMPULSION;
    affect_to_char (wolf, &af);

    wolf->leader = ch;
  }

  if (count > 1)
  {
    send_to_char ("A pack of wolves suddenly appear and walk up to you.\n\r",
		  ch);
    act ("A pack of wolves suddenly appear and walk up to $n.", ch, NULL,
	 NULL, TO_ROOM, POS_RESTING);
  }
  else
  {
    send_to_char ("A lone wolf appears and walks up to you.\n\r", ch);
    act ("A single wolf suddenly appears and walks up to $n.",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
  }

  return;
}


/** Function: do_reachout
  * Guild   : Wolfbrother
  * Descr   : tells guild member if another person can "listen to the wolves"
  * Returns : TRUE/FALSE
  * Syntax  : reachout (who)
  * Written : v1.0 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_reachout)
{
  CHAR_DATA *victim;

  if (!IS_FLAG (ch->states.player, _ACT_WOLFBROTHER) && !IS_IMMORTAL (ch))
    return;

  if ((victim = get_char_room (ch, argument)) != NULL)
  {
    if (IS_FLAG (victim->states.player, _ACT_WOLFBROTHER))
    {
      act ("$N acknowledges your touch.", ch, NULL, victim, TO_CHAR,
	   POS_DEAD);
      act ("$N reaches out to you, $S thoughts half wolf, half human.",
	   victim, NULL, ch, TO_CHAR, POS_DEAD);
      return;
    }
    send_to_char ("Your thoughts reach out, and find nothing.\n\r", ch);
  }

  send_to_char ("They're not here.\n\r", ch);
  return;
}

/** Function: do_second
  * Guild   : Gaidin
  * Descr   : Allows ch to wield a second weapon, giving additional
  *         : attacks per round.
  * Returns : (void)
  * Syntax  : second <weapon>
  * Written : v1.0 2/98
  * Author  : Erwin Andreasan, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_second)
{
  OBJ_DATA *obj;
  OBJ_DATA *dual = NULL;

  char buf[MIL];		/* overkill, but what the heck */

  if (!can_fight (ch, TRUE))
    return;

  if (argument[0] == '\0')	/* empty */
  {
    send_to_char ("Wear which weapon in your off-hand?\n\r", ch);
    return;
  }

  obj = get_obj_carry (ch, argument, ch);
  dual = get_eq_char (ch, WEAR_WIELD);

  if (obj == NULL)
  {
    send_to_char ("You have no such thing on you.\n\r", ch);
    return;
  }

  if (GET_SKILL (ch, gsn_dual_wield) < 1)
  {
    act ("You would likely cut off your own arm, trying to wield $p in"
	 " your off hand!\n\r", ch, obj, NULL, TO_CHAR, POS_RESTING);
    return;
  }

  /* check if the char is using a shield or a held weapon */
  if ((get_eq_char (ch, WEAR_SHIELD) != NULL) ||
      (get_eq_char (ch, WEAR_HOLD) != NULL))
  {
    send_to_char ("You cannot use a secondary weapon while using"
		  " a shield or holding an item\n\r", ch);
    return;
  }

  /* Equally important... */
  if (dual && IS_WEAPON_STAT (dual, WEAPON_TWO_HANDS))
  {
    send_to_char ("You cannot wield both a secondary weapon,"
		  " and a two-handed weapon!\n\r", ch);
    return;
  }

  if (ch->level < obj->level)
  {
    sprintf (buf, "You must be level %d to use this object.\n\r", obj->level);
    send_to_char (buf, ch);
    act ("$n tries to use $p, but is too inexperienced.",
	 ch, obj, NULL, TO_ROOM, POS_RESTING);
    return;
  }

  /* check that the character is using a first weapon at all */
  if (get_eq_char (ch, WEAR_WIELD) == NULL)
  {
    send_to_char ("You need to wield a primary weapon, before"
		  " using a secondary one!\n\r", ch);
    return;
  }


  /* check for str - secondary weapons have to be lighter */
  if (get_obj_weight (obj) > (str_app[get_curr_stat (ch,
						     STAT_STR)].wield / 2))
  {
    send_to_char ("This weapon is too heavy to be used as a"
		  " secondary weapon by you.\n\r", ch);
    return;
  }

  /* check if the secondary weapon is at least half
     as light as the primary weapon */

  if ((get_obj_weight (obj) * 2) >
      get_obj_weight (get_eq_char (ch, WEAR_WIELD)))
  {
    send_to_char ("Your secondary weapon has to be considerably"
		  " lighter than the primary one.\n\r", ch);
    return;
  }

  /* at last - the char uses the weapon */
  if (!remove_obj (ch, WEAR_SECONDARY, TRUE))
    return;

  /* char CAN use the item! that didn't take long at aaall */
  act ("$n wields $p in $s off-hand.", ch, obj, NULL, TO_ROOM, POS_RESTING);
  act ("You wield $p in your off-hand.", ch, obj, NULL, TO_CHAR, POS_RESTING);
  equip_char (ch, obj, WEAR_SECONDARY);
  return;
}



/** Function: do_bounty
  * Descr   : set's a bounty of gold on a victims head.
  * Returns : (void)
  * Syntax  : bounty (who) (amount)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_bounty)
{
  char arg1[MIL];
  char arg2[MIL];
  char buf[MSL];
  CHAR_DATA *victim;

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);


  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    chprintf(ch,
      "Place a bounty on who's head?\n\r"
      "Syntax: Bounty <victim> <amount%s\n\r",
      (IS_IMMORTAL(ch) || IS_FLAG(ch->states.player, _ACT_MORTAL_LEADER) ) ? "|remove>" : ">");
    return;
  }
  if ((victim = get_char_world (ch, arg1)) == NULL)
  {
    send_to_char ("They are currently not logged in!", ch);
    return;
  }

  if (IS_NPC (victim))
  {
    send_to_char ("You cannot put a bounty on them!", ch);
    return;
  }

  if ( !str_prefix("remove", arg2) && (IS_IMMORTAL(ch) || IS_FLAG(ch->states.player, _ACT_MORTAL_LEADER)))
  {
    victim->pcdata->bounty = 0;
    sprintf(buf, "The Guild of Thief Takers has revoked the bounty for the head of %s!\n\r", victim->name);
    system_msg(buf);
  }    

  if (is_number (arg2))
  {
    long amount;
    amount = parsebet(0, arg2);

    if (player_silver(ch) < amount)
    {
      send_to_char ("You don't have that much gold!", ch);
      return;
    }

    if ( amount >  0 )
    {
      amt_from_player(ch, amount);
  
      victim->pcdata->bounty += amount;

      sprintf (buf,
  	      "The Guild of Thief Takers is now offering %ld gold for the head of %s!\n\r",
	       victim->pcdata->bounty, victim->name);

      system_msg (buf);
	  victim->pcdata->notoriety[INFAMY] += 5;
      return;
    }
    else
    {
      send_to_char("Positive amounts only, please.", ch);
      return;
    }

  }				/* end: if is_number(arg2) */

}				/* end of do_bounty */

_DOFUN(do_entrance_cue)
{
  char buf[MSL];


  if (!IS_NPC (ch))
  {
    if (ch->clan < 1 || !IS_FLAG (clan_table[ch->clan].flags, _GLEEMAN))
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }

    smash_tilde (argument);

    if (argument[0] == '\0')
    {
      sprintf (buf, "Your Entrance Cue is %s\n\r", ch->pcdata->bamfin);
      send_to_char (buf, ch);
      return;
    }

    if (strstr (argument, ch->name) == NULL)
    {
      send_to_char ("You must include your name.\n\r", ch);
      return;
    }

    free_string (ch->pcdata->bamfin);
    ch->pcdata->bamfin = str_dup (argument);

    sprintf (buf, "Your Entrance Cue is now %s\n\r", ch->pcdata->bamfin);
    send_to_char (buf, ch);
  }
  return;
}

_DOFUN(do_exit_cue)
{
  char buf[MSL];

  if (!IS_NPC (ch))
  {
    if (!IS_FLAG (clan_table[ch->clan].flags, _GLEEMAN))
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }

    smash_tilde (argument);

    if (argument[0] == '\0')
    {
      sprintf (buf, "Your Exit Cue is %s\n\r", ch->pcdata->bamfout);
      send_to_char (buf, ch);
      return;
    }

    if (strstr (argument, ch->name) == NULL)
    {
      send_to_char ("You must include your name.\n\r", ch);
      return;
    }

    free_string (ch->pcdata->bamfout);
    ch->pcdata->bamfout = str_dup (argument);

    sprintf (buf, "Your Exit Cue is now %s\n\r", ch->pcdata->bamfout);
    send_to_char (buf, ch);
  }
  return;
}

/** Function: do_feral
  * Descr   : Wolk kin skill that raises hit/dam but lowers ac
  *         : (works much like berserk)
  * Returns : void
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(do_feral)
{
  int chance, hp_percent;

  if ((chance = get_skill (ch, gsn_feral)) == 0
      || IS_NPC (ch)
      || !IS_FLAG (ch->states.player, _ACT_WOLFBROTHER)
      || (!IS_NPC (ch)
	  && ch->level < skill_table[gsn_feral].skill_level[ch->c_class]))
  {
    send_to_char ("You have a sudden craving for red meat.\n\r", ch);
    return;
  }

  if (IS_AFFECTED (ch, _AFF_FERAL) || is_affected (ch, gsn_frenzy))
  {
    send_to_char
      ("You can't get the taste of hot blood out of your mouth.\n\r", ch);
    return;
  }

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

    if (number_percent () < 5 && !IS_NPC (ch) && ch->level < (LEVEL_HERO - 1))
    {
      ch->pcdata->insanity++;
    }

    /* heal a little damage */
    ch->hit += ch->level * 2;
    ch->hit = UMIN (ch->hit, ch->max_hit);

    send_to_char
      ("Your thoughts turn feral as the wolf in you takes over!\n\r", ch);

    act ("The yellow in $n's eyes suddenly flares brilliantly.",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
    check_improve (ch, gsn_feral, TRUE, 2);

    af.where = TO_AFFECTS;
    af.type = gsn_feral;
    af.level = ch->level;
    af.duration = number_fuzzy (ch->level / 10);
    af.modifier = UMAX (1, ch->level / 5);
    af.bitvector = _AFF_FERAL;

    af.location = APPLY_HITROLL;
    affect_to_char (ch, &af);

    af.location = APPLY_DAMROLL;
    affect_to_char (ch, &af);
  }

  else
  {
    WAIT_STATE (ch, 3 * PULSE_VIOLENCE);

    send_to_char ("You think you hear a faint sound of wolves in "
		  "the distance.\n\r", ch);
    check_improve (ch, gsn_feral, FALSE, 2);
  }
}
