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


/** The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN (spec_cast_mage);
DECLARE_SPEC_FUN (spec_fido);
DECLARE_SPEC_FUN (spec_guard);
DECLARE_SPEC_FUN (spec_janitor);
DECLARE_SPEC_FUN (spec_poison);
DECLARE_SPEC_FUN (spec_thief);
DECLARE_SPEC_FUN (spec_nasty);
DECLARE_SPEC_FUN (spec_patrolman);

/* the function table */
const struct spec_type spec_table[] = {
  {"spec_cast_mage", spec_cast_mage},
  {"spec_fido", spec_fido},
  {"spec_guard", spec_guard},
  {"spec_janitor", spec_janitor},
  {"spec_poison", spec_poison},
  {"spec_thief", spec_thief},
  {"spec_nasty", spec_nasty},
  {"spec_patrolman", spec_patrolman},
  {NULL, NULL}
};

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup (const char *name)
{
  int i;

  for (i = 0; spec_table[i].name != NULL; i++)
  {
    if (LOWER (name[0]) == LOWER (spec_table[i].name[0])
	&& !str_prefix (name, spec_table[i].name))
      return spec_table[i].function;
  }

  return 0;
}

char *spec_name (SPEC_FUN * function)
{
  int i;

  for (i = 0; spec_table[i].function != NULL; i++)
  {
    if (function == spec_table[i].function)
      return spec_table[i].name;
  }

  return NULL;
}


bool spec_patrolman (CHAR_DATA * ch)
{
  CHAR_DATA *vch, *victim = NULL;
  OBJ_DATA *obj;
  char *message;
  int count = 0;

  if (!IS_AWAKE (ch) || IS_AFFECTED (ch, _AFF_CALM) || ch->in_room == NULL
      || IS_AFFECTED (ch, _AFF_COMPULSION) || ch->fighting != NULL)
    return FALSE;

  /* look for a fight in the room */
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch == ch)
      continue;

    if (vch->fighting != NULL)	/* break it up! */
    {
      if (number_range (0, count) == 0)
	victim = (vch->level > vch->fighting->level) ? vch : vch->fighting;
      count++;
    }
  }

  if (victim == NULL || (IS_NPC (victim) && victim->spec_fun == ch->spec_fun))
    return FALSE;

  if (((obj = get_eq_char (ch, WEAR_NECK_1)) != NULL
       && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)
      || ((obj = get_eq_char (ch, WEAR_NECK_2)) != NULL
	  && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE))
  {
    act ("You blow down hard on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$n blows on $p, ***WHEEEEEEEEEEEET***",
	 ch, obj, NULL, TO_ROOM, POS_RESTING);

    for (vch = char_list; vch != NULL; vch = vch->next)
    {
      if (vch->in_room == NULL)
	continue;

      if (vch->in_room != ch->in_room
	  && vch->in_room->area == ch->in_room->area)
	send_to_char ("You hear a shrill whistling sound.\n\r", vch);
    }
  }

  switch (number_range (0, 6))
  {
  default:
    message = NULL;
    break;
  case 0:
    message = "$n yells 'All roit! All roit! break it up!'";
    break;
  case 1:
    message = "$n says 'Society's to blame, but what's a bloke to do?'";
    break;
  case 2:
    message = "$n mumbles 'bloody kids will be the death of us all.'";
    break;
  case 3:
    message = "$n shouts 'Stop that! Stop that!' and attacks.";
    break;
  case 4:
    message = "$n pulls out his billy and goes to work.";
    break;
  case 5:
    message = "$n sighs in resignation and proceeds to break up the fight.";
    break;
  case 6:
    message = "$n says 'Settle down, you hooligans!'";
    break;
  }

  if (message != NULL)
    act (message, ch, NULL, NULL, TO_ALL, POS_RESTING);

  multi_hit (ch, victim, TYPE_UNDEFINED);

  return TRUE;
}


bool spec_nasty (CHAR_DATA * ch)
{
  CHAR_DATA *victim, *v_next;
  long gold;

  if (!IS_AWAKE (ch))
  {
    return FALSE;
  }

  if (ch->position != POS_FIGHTING)
  {
    for (victim = ch->in_room->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (!IS_NPC (victim)
	  && (victim->level > ch->level) && (victim->level < ch->level + 10))
      {
	do_kill (ch, victim->name);
	if (ch->position != POS_FIGHTING)
	  do_murder (ch, victim->name);
	/* should steal some coins right away? :) */
	return TRUE;
      }
    }
    return FALSE;		/*  No one to attack */
  }

  /* okay, we must be fighting.... steal some coins and flee */
  if ((victim = ch->fighting) == NULL)
    return FALSE;		/* let's be paranoid.... */

  switch (number_bits (2))
  {
  case 0:
    act ("$n rips apart your coin purse, spilling your gold!",
	 ch, NULL, victim, TO_VICT, POS_RESTING);
    act ("You slash apart $N's coin purse and gather his gold.",
	 ch, NULL, victim, TO_CHAR, POS_RESTING);
    act ("$N's coin purse is ripped apart!",
	 ch, NULL, victim, TO_NOTVICT, POS_RESTING);
    gold = victim->gold / 10;	/* steal 10% of his gold */
    victim->gold -= gold;
    ch->gold += gold;
    return TRUE;

  case 1:
    do_flee (ch, "");
    return TRUE;

  default:
    return FALSE;
  }
}


bool spec_cast_mage (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  char *spell;
  int sn;

  if (ch->position != POS_FIGHTING)
    return FALSE;

  for (victim = ch->in_room->people; victim != NULL; victim = v_next)
  {
    v_next = victim->next_in_room;
    if (victim->fighting == ch && number_bits (2) == 0)
      break;
  }

  if (victim == NULL)
    return FALSE;

  for (;;)
  {
    int min_level;

    switch (number_bits (4))
    {
    case 0:
      min_level = 0;
      spell = "blindness";
      break;
    case 1:
      min_level = 3;
      spell = "ice shard";
      break;
    case 2:
      min_level = 7;
      spell = "weaken";
      break;
    case 4:
      min_level = 11;
      spell = "sever weave";
      break;
    case 6:
      min_level = 13;
      spell = "energy drain";
      break;
    case 7:
      min_level = 12;
      spell = "sleep";
      break;
    case 8:
      min_level = 10;
      spell = "slow";
      break;
    case 9:
      min_level = 15;
      spell = "fireball";
      break;
    case 10:
      min_level = 20;
      spell = "earthquake";
      break;
    case 12:
      min_level = 0;
      spell = "blindness";
      break;
    case 13:
      min_level = 3;
      spell = "cause serious";
      break;
    case 14:
      min_level = 7;
      spell = "earthquake";
      break;
    case 15:
      min_level = 9;
      spell = "cause critical";
      break;
    case 16:
      min_level = 10;
      spell = "dispel evil";
      break;
    case 17:
      min_level = 12;
      spell = "curse";
      break;
    case 19:
      min_level = 13;
      spell = "flame blast";
      break;
    case 20:
      min_level = 10;
      spell = "scald";
      break;
    case 21:
      min_level = 14;
      spell = "poison";
      break;
    case 22:
      min_level = 15;
      spell = "harm";
      break;
    default:
      min_level = 16;
      spell = "sever weave";
      break;
    }

    if (ch->level >= min_level)
      break;
  }

  if ((sn = skill_lookup (spell)) < 0)
    return FALSE;

  if ( skill_table[sn].weave_fun != NULL )
    (*skill_table[sn].weave_fun) (sn, ch->level, ch, victim, TARGET_CHAR, "", FALSE);
  else
	bugf("[special.c::spec_cast_mage] NULL weave function for weave '%s'!", 
	     skill_table[sn].name);

  return TRUE;
}

bool spec_fido (CHAR_DATA * ch)
{
  OBJ_DATA *corpse;
  OBJ_DATA *c_next;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  if (!IS_AWAKE (ch))
    return FALSE;

  for (corpse = ch->in_room->contents; corpse != NULL; corpse = c_next)
  {
    c_next = corpse->next_content;
    if (corpse->item_type != ITEM_CORPSE_NPC)
      continue;

    act ("$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM,
	 POS_RESTING);
    for (obj = corpse->contains; obj; obj = obj_next)
    {
      obj_next = obj->next_content;
      obj_from_obj (obj);
      obj_to_room (obj, ch->in_room);
    }
    extract_obj (corpse);
    return TRUE;
  }

  return FALSE;
}



bool spec_guard (CHAR_DATA * ch)
{
  char buf[MSL];
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  CHAR_DATA *ech;
  char *crime;
  int max_evil;

  if (!IS_AWAKE (ch) || ch->fighting != NULL)
    return FALSE;

  max_evil = 300;
  ech = NULL;
  crime = "";

  for (victim = ch->in_room->people; victim != NULL; victim = v_next)
  {
    v_next = victim->next_in_room;

    if (!can_see (ch, victim))
      continue;

    if ((!IS_NPC (victim) &&
	 !IS_IMMORTAL (victim)) &&
	((IS_SET (victim->in_room->room_flags, ROOM_MEN_ONLY) &&
	  victim->sex == SEX_FEMALE) ||
	 (IS_SET (victim->in_room->room_flags, ROOM_WOMEN_ONLY) &&
	  victim->sex == SEX_MALE)))
    {
      crime = "INVALIDSEX";
      break;
    }

    if (victim->fighting != NULL
	&& victim->fighting != ch && victim->alignment < max_evil)
    {
      max_evil = victim->alignment;
      ech = victim;
    }
  }

  if (!str_cmp(crime, "INVALIDSEX"))
  {
    ROOM_INDEX_DATA *to;

    REMOVE_FLAG (ch->states.penalties, _NOTELL);
    REMOVE_FLAG (ch->states.penalties, _NOCHANNELS);
    sprintf (buf, "tell %s You are not allowed in here!\n\r", victim->name);
    do_tell (ch, buf);

    if ((to = get_room_index (victim->from_room)) == NULL)
      to = get_room_index (ROOM_VNUM_LOGIN);

    char_from_room (victim);
    char_to_room (victim, to);

    do_look (victim, "");

    return TRUE;
  }

  if (victim != NULL)
  {
    sprintf (buf, "%s is a %s!  Rouse the Watch!", victim->name, crime);
    REMOVE_FLAG (ch->states.penalties, _NOSHOUT);
    SET_FLAG (ch->states.comm, _SHOUTS);
    do_yell (ch, buf);
    multi_hit (ch, victim, TYPE_UNDEFINED);
    return TRUE;
  }

  if (ech != NULL)
  {
    act ("$n screams 'Rouse the Watch!",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
    multi_hit (ch, ech, TYPE_UNDEFINED);
    return TRUE;
  }

  return FALSE;
}



bool spec_janitor (CHAR_DATA * ch)
{
  OBJ_DATA *trash;
  OBJ_DATA *trash_next;

  if (!IS_AWAKE (ch))
    return FALSE;

  for (trash = ch->in_room->contents; trash != NULL; trash = trash_next)
  {
    trash_next = trash->next_content;
    if (!IS_SET (trash->wear_flags, ITEM_TAKE) || !can_loot (ch, trash))
      continue;
    if (trash->item_type == ITEM_DRINK_CON
	|| trash->item_type == ITEM_TRASH || trash->cost < 10)
    {
      act ("$n picks up some trash.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
      obj_from_room (trash);
      obj_to_char (trash, ch);
      return TRUE;
    }
  }

  return FALSE;
}




bool spec_poison (CHAR_DATA * ch)
{
  CHAR_DATA *victim;

  if (ch->position != POS_FIGHTING
      || (victim = ch->fighting) == NULL || number_percent () > 2 * ch->level)
    return FALSE;

  act ("You bite $N!", ch, NULL, victim, TO_CHAR, POS_RESTING);
  act ("$n bites $N!", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
  act ("$n bites you!", ch, NULL, victim, TO_VICT, POS_DEAD);

  if (!IS_AFFECTED (victim, _AFF_POISON))
    weave_poison (skill_lookup("poison"), ch->level, ch, victim, TARGET_CHAR, "", FALSE);
  return TRUE;
}



bool spec_thief (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  long gold, silver;

  if (ch->position != POS_STANDING)
    return FALSE;

  for (victim = ch->in_room->people; victim != NULL; victim = v_next)
  {
    v_next = victim->next_in_room;

    if (IS_NPC (victim)
	|| victim->level >= LEVEL_IMMORTAL
	|| number_bits (5) != 0 || !can_see (ch, victim))
      continue;

    if (IS_AWAKE (victim) && number_range (0, ch->level) == 0)
    {
      act ("You discover $n's hands in your wallet!",
	   ch, NULL, victim, TO_VICT, POS_RESTING);
      act ("$N discovers $n's hands in $S wallet!",
	   ch, NULL, victim, TO_NOTVICT, POS_RESTING);
      return TRUE;
    }
    else
    {
      gold = victim->gold * UMIN (number_range (1, 20), ch->level / 2) / 100;
      gold = UMIN (gold, ch->level * ch->level * 10);
      ch->gold += gold;
      victim->gold -= gold;
      silver =
	victim->silver * UMIN (number_range (1, 20), ch->level / 2) / 100;
      silver = UMIN (silver, ch->level * ch->level * 25);
      ch->silver += silver;
      victim->silver -= silver;
      return TRUE;
    }
  }

  return FALSE;
}
