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
 * @@ Objective (handler.c)
 * 
 * This file is a bit of a 'catchall' for support functions. I've thought
 * of splitting this up into 'char_h.c', 'obj_h.c', etc, and may yet. Until
 * then, this is the place to look when you cant find the function your 
 * looking for.
 */


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"



void set_init (SET s) /* initialize set to empty */
{
  memset (s, 0, MAXELEMS / UL_BITS);
}

void add_sets (SET dest, SET source)
{
  unsigned long flag;

  for (flag = 0; flag < MAXELEMS; flag++)
    if (IS_FLAG (source, flag) && !IS_FLAG (dest, flag))
      SET_FLAG (dest, flag);
}

void assign_set(SET dest, SET source)
{
  unsigned long flag;
  
  set_init(dest);
  for (flag = 0; flag < MAXELEMS; flag++)
    if ( IS_FLAG (source, flag) )
      SET_FLAG (dest, flag);
}

bool can_channel (CHAR_DATA * ch)
{

  if (IS_NPC (ch))
    return IS_FLAG (ch->states.player, _ACT_CHANNELER);
  else
    return (ch->pcdata->learned[(ch->sex == SEX_FEMALE) ?
				gsn_embrace : gsn_seize] > 0);
}

bool in_same_world(CHAR_DATA *ch, CHAR_DATA *vch)
{

	if ( IS_AFFECTED(ch, _AFF_DREAMING) && !IS_AFFECTED(vch, _AFF_DREAMING) )
		return FALSE;
	else
	if ( !IS_AFFECTED(ch, _AFF_DREAMING) && IS_AFFECTED(vch, _AFF_DREAMING) )
		return FALSE;

	return TRUE;
}

CHAR_DATA *get_char_by_id(long id)
{
	CHAR_DATA *vch = NULL;
	CHAR_DATA *vch_next = NULL;

	if (id <= 0)
		return NULL;

	for (vch=char_list; vch; vch = vch_next )
	{
		vch_next = vch->next;

  	    if ( vch->pcdata && vch->pcdata->id == id )
		  return vch;
		else
		if ( !vch->pcdata && vch->pIndexData->vnum == id )
			return vch;
	}

	return NULL;
}

/* friend stuff -- for NPC's mostly */
bool is_friend (CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (is_same_group (ch, victim))
    return TRUE;


  if (!IS_NPC (ch))
    return FALSE;

  if (!IS_NPC (victim))
  {
    if (IS_FLAG (ch->states.off, _ACT_ASSIST_PLAYERS))
      return TRUE;
    else
      return FALSE;
  }

  if (IS_AFFECTED (ch, _AFF_COMPULSION))
    return FALSE;

  if (IS_FLAG (ch->states.off, _ACT_ASSIST_ALL))
    return TRUE;

  if (ch->group && ch->group == victim->group)
    return TRUE;

  if (IS_FLAG (ch->states.off, _ACT_ASSIST_VNUM)
      && ch->pIndexData == victim->pIndexData)
    return TRUE;

  if (IS_FLAG (ch->states.off, _ACT_ASSIST_RACE) && ch->race == victim->race)
    return TRUE;

  if (IS_FLAG (ch->states.off, _ACT_ASSIST_ALIGN)
      && !IS_FLAG (ch->states.player, _ACT_NOALIGN)
      && !IS_FLAG (victim->states.player, _ACT_NOALIGN)
      && ((IS_GOOD (ch) && IS_GOOD (victim))
	  || (IS_EVIL (ch) && IS_EVIL (victim))
	  || (IS_NEUTRAL (ch) && IS_NEUTRAL (victim))))
    return TRUE;

  return FALSE;
}

bool knows_group(CHAR_DATA* ch, int group)
{
	if ( IS_NPC(ch) || group < 0 || group > maxGroup )
		return FALSE;

	return ( group <= ch->pcdata->iMemSize[MEM_MAXGROUP] && 
		ch->pcdata->group_known[group] );
}

/* returns number of people on an object */
int count_users (OBJ_DATA * obj)
{
  CHAR_DATA *fch;
  int count = 0;

  if (obj->in_room == NULL)
    return 0;

  for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
    if (fch->on == obj)
      count++;

  return count;
}


enum _WEAPON_CLASS weapon_type (const char *name)
{
  enum _WEAPON_CLASS type;

  if (!name || name[0] == '\0')
    return WEAPON_EXOTIC;

  for (type = 0; weapon_table[type].name != NULL; type++)
  {
    if (LOWER (name[0]) == LOWER (weapon_table[type].name[0])
	&& !str_prefix (name, weapon_table[type].name))
      return weapon_table[type].type;
  }

  return WEAPON_EXOTIC;
}

char *item_name (int item_type)
{
  int type;

  for (type = 0; item_table[type].name != NULL; type++)
    if (item_type == item_table[type].type)
      return item_table[type].name;

  return "none";
}

char *weapon_name (int weapon_type)
{
  int type;

  for (type = 0; weapon_table[type].name != NULL; type++)
    if (weapon_type == weapon_table[type].type)
      return weapon_table[type].name;

  return "exotic";
}


/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */

int check_immune (CHAR_DATA * ch, int dam_type)
{
  int immune, def;
  int bit;

  immune = -1;
  def = IS_NORMAL;

  if (dam_type == DAM_NONE)
    return immune;

  if (dam_type <= 3)
  {
    if (IS_FLAG (ch->states.imm, _WEAPON))
      def = IS_IMMUNE;
    else if (IS_FLAG (ch->states.res, _WEAPON))
      def = IS_RESISTANT;
    else if (IS_FLAG (ch->states.vuln, _WEAPON))
      def = IS_VULNERABLE;
  }
  else				/* magical attack */
  {
    if (IS_FLAG (ch->states.imm, _ONEPOWER))
      def = IS_IMMUNE;
    else if (IS_FLAG (ch->states.res, _ONEPOWER))
      def = IS_RESISTANT;
    else if (IS_FLAG (ch->states.vuln, _ONEPOWER))
      def = IS_VULNERABLE;
  }

  /* set bits to check -- VULN etc. must ALL be the same or this will fail */
  switch (dam_type)
  {
	  case (DAM_BASH):
		bit = _BASH;
		break;
	  case (DAM_PIERCE):
		bit = _PIERCE;
		break;
	  case (DAM_SLASH):
		bit = _SLASH;
		break;
	  case (DAM_FIRE):
		bit = _FIRE;
		break;
	  case (DAM_COLD):
		bit = _COLD;
		break;
	  case (DAM_LIGHTNING):
		bit = _LIGHTNING;
		break;
	  case (DAM_ACID):
		bit = _ACID;
		break;
	  case (DAM_POISON):
		bit = _POISON;
		break;
	  case (DAM_NEGATIVE):
		bit = _NEGATIVE;
		break;
	  case (DAM_ENERGY):
		bit = _ENERGY;
		break;
	  case (DAM_MENTAL):
		bit = _MENTAL;
		break;
	  case (DAM_DISEASE):
		bit = _DISEASE;
		break;
	  case (DAM_DROWNING):
		bit = _DROWNING;
		break;
	  case (DAM_LIGHT):
		bit = _LIGHT;
		break;
	  case (DAM_CHARM):
		bit = _COMPULSION;
		break;
	  case (DAM_HOLY):
		bit = _GOOD;
		break;
	  default:
		return def;
  }

  if (IS_FLAG (ch->states.imm, bit))
    immune = IS_IMMUNE;
  else if (IS_FLAG (ch->states.res, bit) && immune != IS_IMMUNE)
    immune = IS_RESISTANT;
  else if (IS_FLAG (ch->states.vuln, bit))
  {
    if (immune == IS_IMMUNE)
      immune = IS_RESISTANT;
    else if (immune == IS_RESISTANT)
      immune = IS_NORMAL;
    else
      immune = IS_VULNERABLE;
  }

  if (immune == -1)
    return def;
  else
    return immune;
}

/* for returning skill information */
int get_skill (CHAR_DATA * ch, int sn)
{
  int skill;

  if (!IS_VALID_SKILL(sn))			/* shorthand for level based skills */
  {
    skill = ch->level * 5 / 2;
  }

  else if (sn < -1 || sn > maxSkill)
  {
    bugf ("[handler.c::get_skill] Bad sn %d.", sn);
    skill = 0;
  }

  else if (!IS_NPC (ch))
  {
    if ((ch->level < skill_table[sn].skill_level[ch->c_class]) ||
		sn > ch->pcdata->iMemSize[MEM_MAXSKILL])
	{
      skill = 0;
	}
    else
	{
 	  skill = ch->pcdata->learned[sn];
	}
  }

  else	/* mobiles */
  {

	skill = 2 * ch->level;
  }

  if (ch->daze > 0)
  {
    if (skill_table[sn].weave_fun != NULL)
      skill *= .50;
    else
      skill *= .75;
  }

  if (!IS_NPC (ch) && ch->pcdata->condition[_DRUNK] > 10)
    skill = 9 * skill / 10;

  return URANGE (0, skill, 100);
}

/* for returning weapon information */
int get_weapon_sn (CHAR_DATA * ch, bool secondary)
{
  OBJ_DATA *wield;
  int sn;

  if (secondary)
    wield = get_eq_char (ch, WEAR_SECONDARY);
  else
    wield = get_eq_char (ch, WEAR_WIELD);

  if (wield == NULL || wield->item_type != ITEM_WEAPON)
    sn = gsn_hand_to_hand;

  else
    switch (wield->value[0])
    {
		default:				sn = -1;			break;
		case WEAPON_SWORD:		sn = gsn_sword;		break;
		case WEAPON_DAGGER:		sn = gsn_dagger;	break;
		case WEAPON_SPEAR:		sn = gsn_spear;		break;
		case WEAPON_MACE:		sn = gsn_mace;		break;
		case WEAPON_AXE:		sn = gsn_axe;		break;
		case WEAPON_FLAIL:		sn = gsn_flail;		break;
		case WEAPON_WHIP:		sn = gsn_whip;		break;
		case WEAPON_POLEARM:	sn = gsn_polearm;	break;
		case WEAPON_BOW:		sn = gsn_bow;		break;
		case WEAPON_STAFF:		sn = gsn_staff;		break;
	}

  return sn;
}

int get_weapon_skill (CHAR_DATA * ch, int sn)
{
  int skill;

  if ( !IS_VALID_SKILL(sn) )
	  skill = 3 * ch->level;

  IS_NPC(ch) ? (skill = 2 * ch->level) : (skill = ch->pcdata->learned[sn]);

  return URANGE (0, skill, 100);
}


/* used to de-screw characters */
void reset_char (CHAR_DATA * ch)
{
  int loc, mod, stat;
  OBJ_DATA *obj;
  AFFECT_DATA *af;
  int i;

  if (IS_NPC (ch))
    return;

  if (1 == TRUE
      || ch->pcdata->perm_hit == 0
      || OPR (ch) == 0
      || ch->pcdata->perm_move == 0 || ch->pcdata->last_level == 0)
  {

    /* do a FULL reset */
    for (loc = 0; loc < MAX_WEAR; loc++)
    {

      if ((obj = get_eq_char (ch, loc)) == NULL)
	continue;

      if (!obj->enchanted)
	for (af = obj->pIndexData->affected; af != NULL; af = af->next)
	{
	  mod = af->modifier;

	  switch (af->location)
	  {
	  case APPLY_SEX:
	    ch->sex -= mod;
	    if (ch->sex < 0 || ch->sex > 2)
	      ch->sex = IS_NPC (ch) ? 0 : ch->pcdata->true_sex;
	    break;

	  case APPLY_HIT:
	    ch->max_hit -= mod;
	    break;
	  case APPLY_ENDURANCE:
	    ch->max_move -= mod;
	    break;
	  default:
	    break;
	  }
	}

      for (af = obj->affected; af != NULL; af = af->next)
      {
	mod = af->modifier;
	switch (af->location)
	{
	case APPLY_SEX:
	  ch->sex -= mod;
	  break;
	case APPLY_HIT:
	  ch->max_hit -= mod;
	  break;
	case APPLY_ENDURANCE:
	  ch->max_move -= mod;
	  break;
	default:
	  break;
	}
      }
    }
    /* now reset the permanent stats */
    ch->pcdata->perm_hit = ch->max_hit;
    ch->pcdata->perm_move = ch->max_move;
    ch->pcdata->last_level = ch->played / 3600;

    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
    {
      if (ch->sex > 0 && ch->sex < 3)
	ch->pcdata->true_sex = ch->sex;
      else
	ch->pcdata->true_sex = 0;
    }
  }

  /* now restore the character to his/her true condition */
  for (stat = 0; stat < MAX_STATS; stat++)
    ch->mod_stat[stat] = 0;

  if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
    ch->pcdata->true_sex = 0;
  ch->sex = ch->pcdata->true_sex;
  ch->max_hit = ch->pcdata->perm_hit;
  ch->max_move = ch->pcdata->perm_move;

  for (i = 0; i < 4; i++)
    ch->armor[i] = 100;

  ch->hitroll = 0;
  ch->damroll = 0;
  ch->saving_throw = 0;

  /* now start adding back the effects */
  for (loc = 0; loc < MAX_WEAR; loc++)
  {
    obj = get_eq_char (ch, loc);
    if (obj == NULL)
      continue;
    for (i = 0; i < 4; i++)
      ch->armor[i] -= apply_ac (obj, loc, i);

    if (!obj->enchanted)
      for (af = obj->pIndexData->affected; af != NULL; af = af->next)
      {
	mod = af->modifier;
	switch (af->location)
	{
	case APPLY_STR:
	  ch->mod_stat[STAT_STR] += mod;
	  break;
	case APPLY_DEX:
	  ch->mod_stat[STAT_DEX] += mod;
	  break;
	case APPLY_INT:
	  ch->mod_stat[STAT_INT] += mod;
	  break;
	case APPLY_WIS:
	  ch->mod_stat[STAT_AGL] += mod;
	  break;
	case APPLY_CON:
	  ch->mod_stat[STAT_CON] += mod;
	  break;
	case APPLY_SEX:
	  ch->sex += mod;
	  break;
	case APPLY_HIT:
	  ch->max_hit += mod;
	  break;
	case APPLY_ENDURANCE:
	  ch->max_move += mod;
	  break;
	case APPLY_HITROLL:
	  ch->hitroll += mod;
	  break;
	case APPLY_DAMROLL:
	  ch->damroll += mod;
	  break;
	case APPLY_SAVES:
	  ch->saving_throw += mod;
	  break;
	case APPLY_SAVING_SPELL:
	  ch->saving_throw += mod;
	  break;
	case APPLY_AC:
	  for (i = 0; i < 4; i++)
	    ch->armor[i] += mod;
	  break;

	default:
	  break;
	}
      }

    for (af = obj->affected; af != NULL; af = af->next)
    {
      mod = af->modifier;
      switch (af->location)
      {
      case APPLY_STR:
	ch->mod_stat[STAT_STR] += mod;
	break;
      case APPLY_DEX:
	ch->mod_stat[STAT_DEX] += mod;
	break;
      case APPLY_INT:
	ch->mod_stat[STAT_INT] += mod;
	break;
      case APPLY_WIS:
	ch->mod_stat[STAT_AGL] += mod;
	break;
      case APPLY_CON:
	ch->mod_stat[STAT_CON] += mod;
	break;
      case APPLY_SEX:
	ch->sex += mod;
	break;
      case APPLY_HIT:
	ch->max_hit += mod;
	break;
      case APPLY_ENDURANCE:
	ch->max_move += mod;
	break;
      case APPLY_HITROLL:
	ch->hitroll += mod;
	break;
      case APPLY_DAMROLL:
	ch->damroll += mod;
	break;
      case APPLY_SAVES:
	ch->saving_throw += mod;
	break;
      case APPLY_SAVING_SPELL:
	ch->saving_throw += mod;
	break;
      case APPLY_AC:
	for (i = 0; i < 4; i++)
	  ch->armor[i] += mod;
	break;
      default:
	break;
      }
    }
  }

  /* now add back spell effects */
  for (af = ch->affected; af != NULL; af = af->next)
  {
    mod = af->modifier;
    switch (af->location)
    {
    case APPLY_STR:
      ch->mod_stat[STAT_STR] += mod;
      break;
    case APPLY_DEX:
      ch->mod_stat[STAT_DEX] += mod;
      break;
    case APPLY_INT:
      ch->mod_stat[STAT_INT] += mod;
      break;
    case APPLY_WIS:
      ch->mod_stat[STAT_AGL] += mod;
      break;
    case APPLY_CON:
      ch->mod_stat[STAT_CON] += mod;
      break;
    case APPLY_SEX:
      ch->sex += mod;
      break;
    case APPLY_HIT:
      ch->max_hit += mod;
      break;
    case APPLY_ENDURANCE:
      ch->max_move += mod;
      break;
    case APPLY_HITROLL:
      ch->hitroll += mod;
      break;
    case APPLY_DAMROLL:
      ch->damroll += mod;
      break;
    case APPLY_SAVES:
      ch->saving_throw += mod;
      break;
    case APPLY_SAVING_SPELL:
      ch->saving_throw += mod;
      break;
    case APPLY_AC:
      for (i = 0; i < 4; i++)
	ch->armor[i] += mod;
      break;
    default:
      break;
    }
  }

  /* make sure sex is RIGHT!!!! */
  if (ch->sex < 0 || ch->sex > 2)
    ch->sex = ch->pcdata->true_sex;
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust (CHAR_DATA * ch)
{
  if (ch == NULL)
    return 0;

  if (ch->desc != NULL && ch->desc->original != NULL)
    ch = ch->desc->original;

  if (ch->trust)
    return ch->trust;

  if (IS_NPC (ch) && ch->level >= LEVEL_HERO)
    return LEVEL_HERO - 1;
  else
    return ch->level;
}


/*
 * Retrieve a character's age.
 */
int get_age (CHAR_DATA * ch)
{

  if (ch->age == 0)		/* fix ch older than new age stat */
  {
    ch->age = 17 + (ch->played + (int) (current_time - ch->logon)) / 72000;
    ch->age = ch->age * 365;	/* convert years into days */
  }

  return (int) (ch->age / 365);
}

/* command for retrieving stats */
int get_curr_stat (CHAR_DATA * ch, int stat)
{
  int max;

  if (IS_NPC (ch) || ch->level > LEVEL_IMMORTAL)
    max = 25;

  else
  {
    max = pc_race_table[ch->race].max_stats[stat] + 4;

    if (class_table[ch->c_class].attr_prime == stat)
      max += 2;

    if (ch->race == race_lookup ("human"))
      max += 1;

    if (ch->race == race_lookup ("ogier") &&
			  stat == STAT_STR)
		{
			if (ch->c_class == WARRIOR)
				max = UMIN (max, 29);
			else
				max = UMIN (max, 26);
		}
		else
      max = UMIN (max, 25);
  }

  return URANGE (3, ch->perm_stat[stat] + ch->mod_stat[stat], max);
}

/** Function: get_max_stat
  * Descr   : Returns the maximum stat, modified by race/class.
  * Returns : See above
  * Syntax  : get_max_stat character stat_to_retreive
  * Written : v1.0 4/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int get_max_stat (CHAR_DATA * ch, int stat)
{
  int max;

  if (IS_NPC (ch) || ch->level > LEVEL_IMMORTAL)
    max = 25;

  else
  {
    max = pc_race_table[ch->race].max_stats[stat] + 4;

    if (class_table[ch->c_class].attr_prime == stat)
      max += 2;

    if (ch->race == race_lookup ("human"))
      max += 1;

    if (ch->race == race_lookup ("ogier") &&
			  stat == STAT_STR)
		{
			if (ch->c_class == WARRIOR)
				max = UMIN (max, 29);
			else
				max = UMIN (max, 26);
		}
		else
      max = UMIN (max, 25);
  }

  return max;
}


/* command for returning max training score */
int get_max_train (CHAR_DATA * ch, int stat)
{
  int max;

  if (IS_NPC (ch) || ch->level > LEVEL_IMMORTAL)
    return 25;

  max = pc_race_table[ch->race].max_stats[stat];

  if (class_table[ch->c_class].attr_prime == stat)
  {
    if (ch->race == race_lookup ("human"))
      max += 3;
    else
      max += 2;
  }
  return UMIN (max, 25);
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n (CHAR_DATA * ch)
{
  if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
    return 1000;

  if (IS_NPC (ch) && IS_FLAG (ch->states.player, _ACT_PET))
    return 0;

  return MAX_WEAR + 2 * get_curr_stat (ch, STAT_DEX) + ch->level;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w (CHAR_DATA * ch)
{
  if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
    return 10000000;

  if (IS_NPC (ch) && IS_FLAG (ch->states.player, _ACT_PET))
    return 0;

  return str_app[get_curr_stat (ch, STAT_STR)].carry * 10 + ch->level * 25;
}



/*
 * See if a string is one of the names of an object.
 */

bool is_name (char *str, char *namelist)
{
  char name[MIL], part[MIL];
  char *list, *string;

  /* fix crash on NULL namelist */
  if (namelist == NULL || str[0] == '\0')
    return FALSE;

  string = str;
  /* we need ALL parts of string to match part of namelist */
  for (;;)			/* start parsing string */
  {
    str = one_argument (str, part);

    if (part[0] == '\0')
      return TRUE;

    /* check to see if this is part of namelist */
    list = namelist;
    for (;;)			/* start parsing namelist */
    {
      list = one_argument (list, name);
      if (name[0] == '\0')	/* this name was not found */
	return FALSE;

      if (!str_prefix (string, name))
	return TRUE;		/* full pattern match */

      if (!str_prefix (part, name))
	break;
    }
  }
}

bool is_exact_name (char *str, char *namelist)
{
  char name[MIL];

  if (namelist == NULL)
    return FALSE;

  for (;;)
  {
    namelist = one_argument (namelist, name);
    if (name[0] == '\0')
      return FALSE;
    if (!str_cmp (str, name))
      return TRUE;
  }
}



/*
 * Move a char out of a room.
 */
void char_from_room (CHAR_DATA * ch)
{
  OBJ_DATA *obj;

  if (ch->in_room == NULL)
    return;

  if (!IS_NPC (ch))
    --ch->in_room->area->nplayer;

  if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL
      && obj->item_type == ITEM_LIGHT
      && obj->value[2] != 0 && ch->in_room->light > 0)
    --ch->in_room->light;

  if (ch == ch->in_room->people)
  {
    ch->in_room->people = ch->next_in_room;
  }
  else
  {
    CHAR_DATA *prev;

    for (prev = ch->in_room->people; prev; prev = prev->next_in_room)
    {
      if (prev->next_in_room == ch)
      {
	prev->next_in_room = ch->next_in_room;
	break;
      }
    }

    if (prev == NULL)
      bugf("[handler.c::char_from_room] ch not found.");
  }

  if (ch->in_room == NULL)
    ch->from_room = ROOM_VNUM_LOGIN;
  else
    ch->from_room = ch->in_room->vnum;

  ch->in_room = NULL;
  ch->next_in_room = NULL;
  ch->on = NULL;		/* sanity check! */
  return;
}



/*
 * Move a char into a room.
 */
void char_to_room (CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex)
{
  OBJ_DATA *obj;

  if (pRoomIndex == NULL)
  {
    ROOM_INDEX_DATA *room;

    bugf ("[handler.c::char_to_room] NULL pRoomIndex.");

    if ((room = get_room_index (ROOM_VNUM_LOGIN)) != NULL)
      char_to_room (ch, room);

    return;
  }

  ch->in_room = pRoomIndex;
  ch->next_in_room = pRoomIndex->people;
  pRoomIndex->people = ch;

  if (!IS_NPC (ch))
  {
    if (ch->in_room->area->empty)
    {
      ch->in_room->area->empty = FALSE;
      ch->in_room->area->age = 0;
    }
    ++ch->in_room->area->nplayer;
  }

  if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL
      && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
    ++ch->in_room->light;

  if ((IS_SET (ch->in_room->room_flags, ROOM_NOMAGIC) ||
       IS_SET (ch->in_room->room_flags, ROOM_STEDDING)))
  {
    AFFECT_DATA *pAf;
    AFFECT_DATA *pNext;
    bool pFound = FALSE;

    if (is_affected (ch, gsn_seize) || is_affected (ch, gsn_embrace))
      send_to_char ("As you enter the room, you lose your link"
		    " with the One Power!\n\r", ch);

    for (pAf = ch->affected; pAf; pAf = pNext)
    {
      pNext = pAf->next;
      if (pAf->type == skill_lookup ("sever") ||
		  pAf->bitvector == _AFF_POISON ||
		  pAf->type == gsn_treat ||
		  pAf->type == gsn_wrath ||
		  pAf->type == gsn_cloak ||
		  pAf->type == gsn_warfare ||
		  pAf->type == gsn_martyrdom ||
		  pAf->type == gsn_feral ||
		  pAf->type == skill_lookup("blood") ||
		  pAf->type == gsn_berserk ||
		  pAf->type == gsn_sneak ||
		  pAf->type == gsn_hide ||
		  pAf->type == skill_lookup("plague") ||
		  pAf->type == skill_lookup("sense evil") ||
		  pAf->type == skill_lookup("dark vision"))
		continue;
      if (skill_table[pAf->type].weave_fun ||
		  pAf->type == gsn_embrace ||
		  pAf->type == gsn_seize)
	  {
	    pFound = TRUE;
	    affect_remove (ch, pAf);	/* strip affect */
      }
    }
    if (pFound)
      send_to_char ("The weaves surrounding you dissipate.\n\r", ch);

  }


  if (IS_AFFECTED (ch, _AFF_PLAGUE))
  {
    AFFECT_DATA *af, plague;
    CHAR_DATA *vch;

    for (af = ch->affected; af != NULL; af = af->next)
    {
      if (af->type == skill_lookup("plague"))
	break;
    }

    if (af == NULL)
    {
      REMOVE_FLAG (ch->states.affected_by, _AFF_PLAGUE);
      return;
    }

    if (af->level == 1)
      return;

    plague.where = TO_AFFECTS;
    plague.type = skill_lookup("plague");
    plague.level = af->level - 1;
    plague.duration = number_range (1, 2 * plague.level);
    plague.location = APPLY_STR;
    plague.modifier = -5;
    plague.bitvector = _AFF_PLAGUE;

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {

      if (!saves_spell (plague.level - 2, vch, DAM_DISEASE)
	  && !IS_IMMORTAL (vch) &&
	  !IS_AFFECTED (vch, _AFF_PLAGUE) && number_bits (6) == 0)
      {
	send_to_char ("You feel hot and feverish.\n\r", vch);
	act ("$n shivers and looks very ill.",
	     vch, NULL, NULL, TO_ROOM, POS_RESTING);
	affect_join (vch, &plague);
      }
    }
  }


  return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch)
{
  if (obj == NULL)
  {
    bugf("[handler.c::obj_to_char] Null obj.");
    return;
  }
  if (ch == NULL)
  {
    bugf("[handler.c::obj_to_char] Null char.");
    return;
  }

  obj->next_content = ch->carrying;
  ch->carrying = obj;
  obj->carried_by = ch;
  obj->in_room = NULL;
  obj->in_obj = NULL;
  ch->carry_number += get_obj_number (obj);
  ch->carry_weight += get_obj_weight (obj);
}



/*
 * Take an obj from its character.
 */
void obj_from_char (OBJ_DATA * obj)
{
  CHAR_DATA *ch;

  if (obj == NULL)
  {
    bugf("[handler.c::obj_from_char] Null obj.");
    return;
  }
  else if ((ch = obj->carried_by) == NULL)
  {
    bugf("[handler.c::obj_from_char] null ch.");
    return;
  }

  if (obj->wear_loc != WEAR_NONE)
    unequip_char (ch, obj);

  if (ch->carrying == obj)
  {
    ch->carrying = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = ch->carrying; prev != NULL; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
	prev->next_content = obj->next_content;
	break;
      }
    }

    if (prev == NULL)
      bugf ("[handler.c::obj_from_char] obj not in list.");
  }

  obj->carried_by = NULL;
  obj->next_content = NULL;
  ch->carry_number -= get_obj_number (obj);
  ch->carry_weight -= get_obj_weight (obj);
  return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac (OBJ_DATA * obj, int iWear, int type)
{
  if (!obj || obj->item_type != ITEM_ARMOR)
    return 0;

  switch (iWear)
  {
  case WEAR_BODY:
    return 3 * obj->value[type];
  case WEAR_HEAD:
  case WEAR_LEGS:
  case WEAR_ABOUT:
  case WEAR_SHIELD:
  case WEAR_ARMS:
    return 2 * obj->value[type];
  case WEAR_SCABBARD:
  case WEAR_FACE:
  case WEAR_FEET:
  case WEAR_HANDS:
  case WEAR_NECK_1:
  case WEAR_NECK_2:
  case WEAR_WAIST:
  case WEAR_WRIST_L:
  case WEAR_WRIST_R:
  case WEAR_HOLD:
  case WEAR_SHOULDER:
  case WEAR_FINGER_L:
  case WEAR_FINGER_R:
  case WEAR_LIGHT:
  case WEAR_FLOAT:
    return obj->value[type];
  }

  return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char (CHAR_DATA * ch, int iWear)
{
  OBJ_DATA *obj;

  if (ch == NULL)
    return NULL;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == iWear)
      return obj;
  }

  return NULL;
}

/*
 * Find the angreal and sa'angreal power worn.
 */
int get_angreal_power (CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  int bonus=0, factor=0;

  if (ch == NULL)
    return -1;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc != WEAR_NONE &&
        obj->item_type == ITEM_ANGREAL &&
		obj->value[2] == ch->sex)
	{
	  if (obj->value[1] < 0) 
		return -1;
      bonus += obj->value[1];
	  if (obj->value[0] == 2) 
        factor += obj->value[1];
	}
  }

  return bonus + 100*factor;  // use math to return two values
}


/*
 * Equip a char with an obj.
 */
void equip_char (CHAR_DATA * ch, OBJ_DATA * obj, int iWear)
{
  AFFECT_DATA *paf;
  int i;

  if (get_eq_char (ch, iWear) != NULL &&
      !IS_SET (obj->wear_flags, ITEM_WEAR_LODGED))
  {
    bugf ("[handler.c::equip_char] (%d) in R#%d. Allready Equiped O#%d W[%d].",
	  IS_NPC (ch) ? ch->pIndexData->vnum : 0,
	  (ch->in_room != NULL) ? ch->in_room->vnum : 0,
	  obj->pIndexData->vnum, iWear);
    return;
  }

  /* CHANGE THIS -- We want to start a timer here, and have the object
   * slowly change the persons alignment.  (ala Matt's Dagger)
   * if ((IS_OBJ_STAT (obj, ITEM_ANTI_EVIL) && IS_EVIL (ch))
   *   || (IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_GOOD (ch))
   *   || (IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL (ch)))
   * {
   *
   * }
   */

  for (i = 0; i < 4; i++)
    ch->armor[i] -= apply_ac (obj, iWear, i);

  obj->wear_loc = iWear;

  if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      if (paf->location != APPLY_SPELL_AFFECT)
	affect_modify (ch, paf, TRUE);

  for (paf = obj->affected; paf != NULL; paf = paf->next)
    if (paf->location == APPLY_SPELL_AFFECT)
      affect_to_char (ch, paf);
    else
      affect_modify (ch, paf, TRUE);

  if (obj->item_type == ITEM_LIGHT
      && obj->value[2] != 0 && ch->in_room != NULL)
    ++ch->in_room->light;

  if (obj->item_type == ITEM_ANGREAL &&  obj->value[2] == ch->sex)
  {
	if (obj->value[0] == 1)
      send_to_char ("You feel a tickle as it touches your skin.\n\r",ch);
	else
      send_to_char ("You feel a tingle as it touches your skin.\n\r",ch);
  }
  return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char (CHAR_DATA * ch, OBJ_DATA * obj)
{
  AFFECT_DATA *paf = NULL;
  AFFECT_DATA *lpaf = NULL;
  AFFECT_DATA *lpaf_next = NULL;
  int i;

  if (obj->wear_loc == WEAR_NONE)
  {
    bugf("[handler.c::unequip_char] already unequipped.");
    return;
  }

  for (i = 0; i < 4; i++)
    ch->armor[i] += apply_ac (obj, obj->wear_loc, i);

  obj->wear_loc = -1;

  if (!obj->enchanted)
  {
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_SPELL_AFFECT)
      {
	for (lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next)
	{
	  lpaf_next = lpaf->next;
	  if ((lpaf->type == paf->type) &&
	      (lpaf->level == paf->level) &&
	      (lpaf->location == APPLY_SPELL_AFFECT))
	  {
	    affect_remove (ch, lpaf);
	    lpaf_next = NULL;
	  }
	}
      }
      else
      {
	affect_modify (ch, paf, FALSE);
	affect_check (ch, paf->where, paf->bitvector);
      }
    }
  }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
	  if (paf->location == APPLY_SPELL_AFFECT)
	  {
		  bugf ("[handler.c::unequip_char] Norm-Apply: %d");
		  for (lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next)
		  {
			  lpaf_next = lpaf->next;
			  if ((lpaf->type == paf->type) &&
				  (lpaf->level == paf->level) &&
				  (lpaf->location == APPLY_SPELL_AFFECT))
			  {
				  bugf("[handler.c::unequip_char] location = %d, type = %d", lpaf->location, lpaf->type);
				  affect_remove (ch, lpaf);
				  lpaf_next = NULL;
			  }
		  }
	  }
	  else
	  {
		  affect_modify (ch, paf, FALSE);
		  affect_check (ch, paf->where, paf->bitvector);
	  }
  }

  if (obj->item_type == ITEM_LIGHT
      && obj->value[2] != 0 && ch->in_room != NULL && ch->in_room->light > 0)
  {
    --ch->in_room->light;
  }

  return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list (OBJ_INDEX_DATA * pObjIndex, OBJ_DATA * list)
{
  OBJ_DATA *obj;
  int nMatch;

  nMatch = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (obj->pIndexData == pObjIndex)
      nMatch++;
  }

  return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room (OBJ_DATA * obj)
{
	ROOM_INDEX_DATA *in_room;
	CHAR_DATA *ch;
	
	if ((in_room = obj->in_room) == NULL)
	{
		bugf ("[handler.c::obj_from_room] NULL obj.");
		return;
	}
	
	for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
		if (ch->on == obj)
			ch->on = NULL;
		
		if (obj == in_room->contents)
		{
			in_room->contents = obj->next_content;
		}
		else
		{
			OBJ_DATA *prev;
			
			for (prev = in_room->contents; prev; prev = prev->next_content)
			{
				if (prev->next_content == obj)
				{
					prev->next_content = obj->next_content;
					break;
				}
			}
			
			if (prev == NULL)
			{
				bugf ("[handler.c::obj_from_room] obj not found.");
				return;
			}
		}
		
		obj->in_room = NULL;
		obj->next_content = NULL;
		return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room (OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex)
{
  obj->next_content = pRoomIndex->contents;
  pRoomIndex->contents = obj;
  obj->in_room = pRoomIndex;
  obj->carried_by = NULL;
  obj->in_obj = NULL;
  return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj (OBJ_DATA * obj, OBJ_DATA * obj_to)
{
  int counter = 0;

  if ( !obj || !obj_to )
	  return;

  obj->next_content = obj_to->contains;
  obj_to->contains = obj;
  obj->in_obj = obj_to;
  obj->in_room = NULL;
  obj->carried_by = NULL;

  if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT)
    obj->cost = 0;

  for (; obj_to != NULL; obj_to = obj_to->in_obj)
  {
    if (obj_to->carried_by != NULL)
    {
      obj_to->carried_by->carry_number += get_obj_number (obj);
      obj_to->carried_by->carry_weight += get_obj_weight (obj)
	* WEIGHT_MULT (obj_to) / 100;
    }

    if (++counter > 1000)
      break;
  }

  return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj (OBJ_DATA * obj)
{
  OBJ_DATA *obj_from;

  if ((obj_from = obj->in_obj) == NULL)
  {
    bugf("[handler.c::obj_from_obj] null obj_from.");
    return;
  }

  if (obj == obj_from->contains)
  {
    obj_from->contains = obj->next_content;
  }
  else
  {
    OBJ_DATA *prev;

    for (prev = obj_from->contains; prev; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
	prev->next_content = obj->next_content;
	break;
      }
    }

    if (prev == NULL)
    {
      bugf ("[handler.c::obj_from_obj] obj not found.");
      return;
    }
  }

  obj->next_content = NULL;
  obj->in_obj = NULL;

  for (; obj_from != NULL; obj_from = obj_from->in_obj)
  {
    if (obj_from->carried_by != NULL)
    {
      obj_from->carried_by->carry_number -= get_obj_number (obj);
      obj_from->carried_by->carry_weight -= get_obj_weight (obj)
	* WEIGHT_MULT (obj_from) / 100;
    }
  }

  return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj (OBJ_DATA * obj)
{
	OBJ_DATA *obj_content;
	OBJ_DATA *obj_next;
	
	if (obj->in_room != NULL)
		obj_from_room (obj);
	
	else if (obj->carried_by != NULL)
		obj_from_char (obj);
	
	else if (obj->in_obj != NULL)
		obj_from_obj (obj);
	
	for (obj_content = obj->contains; obj_content; obj_content = obj_next)
	{
		obj_next = (obj_content != NULL) ? obj_content->next_content : NULL;
		extract_obj (obj_content);
	}
	
	if (object_list == obj)
	{
		object_list = (obj != NULL) ? obj->next : NULL;
	}
	else
	{
		OBJ_DATA *prev;
		
		for (prev = object_list; prev != NULL; prev = prev->next)
		{
			if (prev->next == obj)
			{
				prev->next = obj->next;
				break;
			}
		}
		
		if (prev == NULL)
		{
			bugf("[handler.c::extract_obj] obj %d not found.", obj->pIndexData->vnum);
			return;
		}
	}
	
	--obj->pIndexData->count;
	free_obj (obj);
	return;
}

void extract_mpcode (MPROG_CODE * mpcode)
{
  if (mprog_list == mpcode)
  {
    mprog_list = mpcode->next;
  }
  else
  {
    MPROG_CODE *prev;

    for (prev = mprog_list; prev != NULL; prev = prev->next)
    {
      if (prev->next == mpcode)
      {
	prev->next = mpcode->next;
	break;
      }
    }

    if (prev == NULL)
    {
      bugf("[handler.c::extract_mpcode] mprog %d not found.", mpcode->vnum);
      return;
    }
  }

  free_mpcode (mpcode);

  return;
}

/** Function: extract_char
  * Descr   : Extracts a character object, including pets and mounts
  *         : from the world, making the obj invalid.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 5/98
  * Author  : Rom Consortium. Updated by: Gary McNickle <gary@tarmongaidon.org>
  */
void extract_char (CHAR_DATA * ch, bool fPull)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  CHAR_DATA *wch;

  nuke_pets (ch);		/* Mounts are done away with here... */

  if (fPull)
    die_follower (ch);

  stop_fighting (ch, TRUE);

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj (obj);
  }

  if (ch->in_room != NULL)
    char_from_room (ch);

  /* Death room is set in the clan table now */
  if (!fPull)
  {				/* send char to clan temple (room[2]) */
    if (is_clan (ch))
      char_to_room (ch, get_room_index (clan_table[ch->clan].room[2]));
    else
      char_to_room (ch, get_room_index (ROOM_VNUM_LOGIN));
    return;
  }

  if (IS_NPC (ch))
    --ch->pIndexData->count;

  if (ch->desc != NULL && ch->desc->original != NULL)
  {
    do_return (ch, "");
    ch->desc = NULL;
  }

  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (wch->reply == ch)
      wch->reply = NULL;
  }

  if (ch == char_list)
  {
    char_list = ch->next;
  }
  else
  {
    CHAR_DATA *prev;

    for (prev = char_list; prev != NULL; prev = prev->next)
    {
      if (prev->next == ch)
      {
	prev->next = ch->next;
	break;
      }
    }

    if (prev == NULL)
    {
      bugf("[handler.c::extract_char] char not found.");
      return;
    }
  }

  if (ch->desc != NULL)
    ch->desc->character = NULL;
  free_char (ch);
  return;
}


/** Function: get_char_area
  * Descr   : Retreives char_data in the char is found within a specific
  *         : area, or null if not.
  * Returns : CHAR_DATA or NULL
  * Syntax  : get_char_area( calling character, number of mob to locate)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
CHAR_DATA *get_char_area (CHAR_DATA * ch, char *argument)
{
  char arg[MIL];
  CHAR_DATA *ach;
  int number, count;

  if ((ach = get_char_room (ch, argument)) != NULL)
    return ach;

  number = number_argument (argument, arg);
  count = 0;
  for (ach = char_list; ach != NULL; ach = ach->next)
  {
    if (
	(ach->in_room && ch->in_room
	 && ach->in_room->area != ch->in_room->area) || !can_see (ch, ach)
	|| (!is_name (arg, ach->name) && !is_name (arg, nation_id (ach))))
      continue;
    if (++count == number)
      return ach;
  }

  return ach;
}


/** Find a char in the room.
 */
CHAR_DATA *get_char_room (CHAR_DATA * ch, char *argument)
{
  char arg[MIL];
  CHAR_DATA *rch;
  int number;
  int count;

  number = number_argument (argument, arg);
  count = 0;

  if (!str_cmp (arg, "self"))
    return ch;

  for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
  {
    if (!can_see (ch, rch) ||
	(!is_name (strip_codes (arg), strip_codes (rch->name))
	 && !is_name (strip_codes (arg), strip_codes (nation_id (rch)))))
      continue;

    if (++count == number)
      return rch;
  }

  return NULL;
}



/** Find a char in the world.
 */
CHAR_DATA *get_char_world (CHAR_DATA * ch, char *argument)
{
  char arg[MIL];
  CHAR_DATA *wch;
  int number;
  int count;

  if ((wch = get_char_room (ch, argument)) != NULL)
    return wch;

  number = number_argument (argument, arg);
  count = 0;
  for (wch = char_list; wch != NULL; wch = wch->next)
  {
    if (wch->in_room == NULL || !can_see (ch, wch)
	|| (!is_name (strip_codes (arg), strip_codes (wch->name))
	    && !is_name (strip_codes (arg), nation_id (wch))))
      continue;
    if (++count == number)
      return wch;
  }

  return NULL;
}



/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type(OBJ_DATA *list, OBJ_INDEX_DATA * pObjIndex)
{
  OBJ_DATA *obj;

  if ( !list )
	  return NULL;

  for (obj = list; obj != NULL; obj = obj->next)
  {
    if (obj->pIndexData == pObjIndex)
      return obj;
  }

  return NULL;
}


/** Find an obj in a list.
 */
OBJ_DATA *get_obj_list (CHAR_DATA * ch, char *argument, OBJ_DATA * list)
{
  char arg[MIL];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument (argument, arg);
  count = 0;

  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj (ch, obj) && is_name (arg, obj->name))
    {
      if (++count == number)
	return obj;
    }
  }

  return NULL;
}



/** Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry (CHAR_DATA * ch, char *argument, CHAR_DATA * viewer)
{
  char arg[MIL];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument (argument, arg);
  count = 0;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == WEAR_NONE
	&& can_see_obj (viewer, obj) && is_name (arg, obj->name))
    {
      if (++count == number)
	return obj;
    }
  }

  return NULL;
}



/** Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear (CHAR_DATA * ch, char *argument)
{
  char arg[MIL];
  OBJ_DATA *obj;
  int number;
  int count;

  number = number_argument (argument, arg);
  count = 0;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc != WEAR_NONE
	&& can_see_obj (ch, obj) && is_name (arg, obj->name))
    {
      if (++count == number)
	return obj;
    }
  }

  return NULL;
}


/** Function: get_obj_wear_vnum
  * Descr   : Determines if a player is wearing an object, specified by
  *         : vnum. 
  * Returns : Object pointer, or NULL if not wearing.
  * Syntax  : get_obj_wear_vnum( ch, vnum to check)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
OBJ_DATA *get_obj_wear_vnum (CHAR_DATA * ch, long vnum)
{
  OBJ_DATA *obj;

  if (vnum < 1)
    return NULL;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc != WEAR_NONE && obj->pIndexData->vnum == vnum)
      return obj;
  }

  return NULL;
}



/** Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here (CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;

  obj = get_obj_list (ch, argument, ch->in_room->contents);

  if (obj != NULL)
    return obj;

  if ((obj = get_obj_carry (ch, argument, ch)) != NULL)
    return obj;

  if ((obj = get_obj_wear (ch, argument)) != NULL)
    return obj;

  return NULL;
}



/** Find an obj in the world.
 */
OBJ_DATA *get_obj_world (CHAR_DATA * ch, char *argument)
{
  char arg[MIL];
  OBJ_DATA *obj;
  int number;
  int count;

  if ((obj = get_obj_here (ch, argument)) != NULL)
    return obj;

  number = number_argument (argument, arg);
  count = 0;
  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (can_see_obj (ch, obj) &&
	(is_name (arg, obj->name) ||
	 obj->pIndexData->vnum == atoi (argument)))
    {
      if (++count == number)
	return obj;
    }
  }

  return NULL;
}

/** Deduct cost from a character 
 */

void deduct_cost (CHAR_DATA * ch, int cost)
{
  int silver = 0, gold = 0;

  silver = UMIN (ch->silver, cost);

  if (silver < cost)
  {
    gold = ((cost - silver + 99) / 100);
    silver = cost - 100 * gold;
  }

  ch->gold -= gold;
  ch->silver -= silver;

  if (ch->gold < 0)
  {
    bugf ("[handler.c::deduct_cost] gold %d < 0", ch->gold);
    ch->gold = 0;
  }
  if (ch->silver < 0)
  {
    bugf ("[handler.c::deduct_cost] silver %d < 0", ch->silver);
    ch->silver = 0;
  }
}
/** Create a 'money' obj.
 */
OBJ_DATA *create_money (int gold, int silver)
{
  char buf[MSL];
  OBJ_DATA *obj;

  if (gold < 0 || silver < 0 || (gold == 0 && silver == 0))
  {
    bugf ("[handler.c::create_money] Bad Amount [%d].", UMIN (gold, silver));
    gold = UMAX (1, gold);
    silver = UMAX (1, silver);
  }

  if (gold == 0 && silver == 1)
    obj = create_object (get_obj_index (OBJ_VNUM_SILVER_ONE), 0);
  else
   if (gold == 1 && silver == 0)
    obj = create_object (get_obj_index (OBJ_VNUM_GOLD_ONE), 0);
  else
   if (silver == 0)
  {
    obj = create_object (get_obj_index (OBJ_VNUM_GOLD_SOME), 0);
    sprintf (buf, obj->short_descr, gold);
    free_string (obj->short_descr);
    obj->short_descr = str_dup (buf);
    obj->value[1] = gold;
    obj->cost = gold;
    obj->weight = gold / 5;
  }
  else
   if (gold == 0)
  {
    obj = create_object (get_obj_index (OBJ_VNUM_SILVER_SOME), 0);
    sprintf (buf, obj->short_descr, silver);
    free_string (obj->short_descr);
    obj->short_descr = str_dup (buf);
    obj->value[0] = silver;
    obj->cost = silver;
    obj->weight = silver / 20;
  }

  else
  {
    obj = create_object (get_obj_index (OBJ_VNUM_COINS), 0);
    sprintf (buf, obj->short_descr, silver, gold);
    free_string (obj->short_descr);
    obj->short_descr = str_dup (buf);
    obj->value[0] = silver;
    obj->value[1] = gold;
    obj->cost = 100 * gold + silver;
    obj->weight = gold / 5 + silver / 20;
  }

  return obj;
}



/** Return # of objects which an object counts as.
  * Thanks to Tony Chamberlain for the correct recursive code here.
  */
int get_obj_number (OBJ_DATA * obj)
{
  int number;

  if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY
      || IS_OBJ_STAT (obj, ITEM_NOCOUNT)
      || obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY)
    number = 0;
  else
    number = 1;

  for (obj = obj->contains; obj != NULL; obj = obj->next_content)
    number += get_obj_number (obj);

  return number;
}


/** Return weight of an object, including weight of contents.
 */
int get_obj_weight (OBJ_DATA * obj)
{
  int weight;
  OBJ_DATA *tobj;

  weight = obj->weight;
  for (tobj = obj->contains; tobj != NULL; tobj = tobj->next_content)
    weight += get_obj_weight (tobj) * WEIGHT_MULT (obj) / 100;

  return weight;
}

int get_true_weight (OBJ_DATA * obj)
{
  int weight;

  weight = obj->weight;
  for (obj = obj->contains; obj != NULL; obj = obj->next_content)
    weight += get_obj_weight (obj);

  return weight;
}

/*
 * True if room is dark.
 */
bool room_is_dark (ROOM_INDEX_DATA * pRoomIndex)
{
	if ( !pRoomIndex )
	{
		bugf("[handler.c::room_is_dark] NULL pRoomIndex passed to function.");
		return FALSE;
	}

	if (pRoomIndex->light > 0)
		return FALSE;
	
	if (IS_SET (pRoomIndex->room_flags, ROOM_DARK))
		return TRUE;
	
	if (pRoomIndex->sector_type == SECT_INSIDE
		|| pRoomIndex->sector_type == SECT_CITY)
		return FALSE;
	
	if (weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)
		return TRUE;
	
	return FALSE;
}


bool is_room_owner (CHAR_DATA * ch, ROOM_INDEX_DATA * room)
{
  if (room->owner == NULL || room->owner[0] == '\0')
    return FALSE;

  return is_name (ch->name, room->owner);
}

/** True if room is private.
 */
bool room_is_private (ROOM_INDEX_DATA * pRoomIndex)
{
  CHAR_DATA *rch;
  int count;

  if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
    return TRUE;

  count = 0;
  for (rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room)
    count++;

  if (IS_SET (pRoomIndex->room_flags, ROOM_PRIVATE) && count >= 2)
    return TRUE;

  if (IS_SET (pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1)
    return TRUE;

  if (IS_SET (pRoomIndex->room_flags, ROOM_IMP_ONLY))
    return TRUE;

  return FALSE;
}

/** Visibility on a room -- for entering and exits 
 */
bool can_see_room (CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex)
{
  if (IS_SET (pRoomIndex->room_flags, ROOM_IMP_ONLY)
      && get_trust (ch) < MAX_LEVEL)
    return FALSE;

  if (IS_SET (pRoomIndex->room_flags, ROOM_GODS_ONLY) && !IS_IMMORTAL (ch))
    return FALSE;

  if (IS_SET (pRoomIndex->room_flags, ROOM_HEROES_ONLY) && !IS_IMMORTAL (ch))
    return FALSE;

  if (IS_SET (pRoomIndex->room_flags, ROOM_NEWBIES_ONLY)
      && ch->level > 5 && !IS_IMMORTAL (ch))
    return FALSE;

  if (!(IS_IMMORTAL (ch) || IS_NPC (ch))
      && (pRoomIndex->clan > -1)
      && (ch->clan != pRoomIndex->clan
	  && !IS_FLAG (clan_table[ch->clan].flags, _THIEF_TAKER)))
  {
    return FALSE;
  }

  if (!(IS_IMMORTAL (ch) || IS_NPC (ch))
      && (pRoomIndex->c_class >= 0)
      && (ch->c_class != pRoomIndex->c_class
	  && !IS_FLAG (clan_table[ch->clan].flags, _THIEF_TAKER)))
  {
    return FALSE;
  }
  return TRUE;
}



/*
 * True if char can see victim.
 */
bool can_see (CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch == victim)
    return TRUE;

  if (victim == NULL)
    return FALSE;

  if (get_trust (ch) < victim->invis_level)
    return FALSE;

  if (get_trust (ch) < victim->incog_level && ch->in_room != victim->in_room)
    return FALSE;

  if ((!IS_NPC (ch) && IS_FLAG (ch->states.config, _HOLYLIGHT))
      || (IS_NPC (ch) && IS_IMMORTAL (ch)))
    return TRUE;

  if (IS_AFFECTED (ch, _AFF_BLIND))
    return FALSE;

  if ( IS_AFFECTED(ch, _AFF_DREAMING) && victim->vGhostRoom == GET_ID(ch) )
	  return TRUE;

  if (room_is_dark (ch->in_room) &&
      (!IS_AFFECTED (ch, _AFF_INFRARED)
       && !IS_AFFECTED (ch, _AFF_DARK_VISION)))
    return FALSE;

  if ( !in_same_world(ch, victim) )
  {
	  if ( victim->position == POS_SLEEPING 
	  && ( !IS_NPC(ch) && ch->pcdata->talents[gtn_dreaming] ) 
	  && make_skill_roll(get_skill(ch, skill_lookup("dream")), .50) )
		return TRUE;
	  else
  	    return FALSE;
  }

  /* sense evil (Gaidin/Aes Sedai skill) */
  if (IS_EVIL (victim) && IS_AFFECTED (ch, _AFF_SENSE_EVIL))
  {
    int chance;

    chance = get_skill (ch, skill_lookup("sense evil"));
    chance -= ch->level - victim->level * 3 / 2;
    if (number_percent () < chance)
      return TRUE;
  }

  /* add check to seize/embrace to reflect enhanced senses */
  if ((is_affected (ch,gsn_seize) || is_affected (ch, gsn_embrace))
      &&
      (IS_AFFECTED (victim, _AFF_INVISIBLE)
       || IS_AFFECTED (victim, _AFF_SNEAK)) && number_percent () <= ch->level)
    return TRUE;


  if (IS_AFFECTED (victim, _AFF_INVISIBLE)
      && !IS_AFFECTED (ch, _AFF_DETECT_INVIS))
    return FALSE;

  /* cloaked gaidin */
  if (IS_AFFECTED (victim, _AFF_CLOAKED))
  {
    int chance = 0;

    if (IS_NPC (ch))
    {
		chance = UMAX (5, ch->level - victim->level);
		if (number_percent () <= chance)
			return TRUE;
		else
			return FALSE;
    }

	if ( is_bonded_to(ch, victim) )
		return TRUE;

    chance =  UMAX(5, get_skill(ch, skill_lookup ("detect hidden") - get_skill (victim, gsn_cloak)));
    chance += UMAX(0, ch->level - (victim->level * .75));
    chance -= get_curr_stat (victim, STAT_DEX) * .75;
    chance += get_curr_stat (ch, STAT_INT) * 1.5;
    chance =  UMAX (5, chance);	/* min, 5% chance */

   if (number_percent () <= chance)
      return TRUE;
    else
      return FALSE;
  }

  /* sneaking */
  if (IS_AFFECTED (victim, _AFF_SNEAK)
      && !IS_AFFECTED (ch, _AFF_DETECT_HIDDEN) && victim->fighting == NULL)
  {
    int chance = 0;

    if (chance == 0 || get_skill (victim, gsn_sneak) > chance)
      chance = get_skill (victim, gsn_sneak);

    chance += get_curr_stat (victim, STAT_DEX) * 3 / 2;
    chance -= get_curr_stat (ch, STAT_INT) * 2;
    chance -= ch->level - victim->level * 3 / 2;

    if (number_percent () < chance)
      return FALSE;
  }

  if (IS_AFFECTED (victim, _AFF_HIDE)
      && !IS_AFFECTED (ch, _AFF_DETECT_HIDDEN) && victim->fighting == NULL)
    return FALSE;

  return TRUE;
}



/** True if char can see obj.
 */
bool can_see_obj (CHAR_DATA * ch, OBJ_DATA * obj)
{
  if (!IS_NPC (ch) && IS_FLAG (ch->states.config, _HOLYLIGHT))
    return TRUE;

  if (IS_SET (obj->extra_flags, ITEM_HIDDEN))
    return FALSE;

  if (IS_SET (obj->extra_flags, ITEM_VIS_DEATH))
  {
	if (obj->carried_by == ch)
          return TRUE;
	else
          return FALSE;
  }

  if (IS_AFFECTED (ch, _AFF_BLIND))
  {
    if (obj->carried_by == ch)
      return TRUE;
    else if (obj == get_eq_char (ch, obj->wear_loc))
      return TRUE;
    else
      if (obj->in_obj
	  && obj->in_obj->carried_by && obj->in_obj->carried_by == ch)
      return TRUE;
    else
      return FALSE;
  }


  if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
    return TRUE;

  if (IS_SET (obj->extra_flags, ITEM_INVIS)
      && !IS_AFFECTED (ch, _AFF_DETECT_INVIS))
    return FALSE;

  if (IS_OBJ_STAT (obj, ITEM_GLOW))
    return TRUE;

  if (room_is_dark (ch->in_room)
      && (!IS_AFFECTED (ch, _AFF_INFRARED)
	  && !IS_AFFECTED (ch, _AFF_DARK_VISION)))
    return FALSE;

  return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj (CHAR_DATA * ch, OBJ_DATA * obj)
{
  if (!IS_SET (obj->extra_flags, ITEM_NODROP))
    return TRUE;

  if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
    return TRUE;

  return FALSE;
}



/*
 * See if a string is one of the names of an object.
 */
bool is_full_name (const char *str, char *namelist)
{
  char name[MIL];

  for (;;)
  {
    namelist = one_argument (namelist, name);
    if (name[0] == '\0')
      return FALSE;
    if (!str_cmp (str, name))
      return TRUE;
  }
}


/** Function: player_silver
  * Descr   : Determines the potential amount of silver a player is
  *         : carrying. Uses each gold as 100 silver.
  * Returns : long
  * Syntax  : player_silver( whos )
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
long player_silver (CHAR_DATA * ch)
{
  return
    (ch->gold > 0) ? (ch->gold * 100) + ch->silver :
    (ch->silver > 0) ? ch->silver : 0;
}


/** Function: amt_to_player
  * Descr   : Adds an amount of silver to a players gold and silver stats
  *         : Converts amounts over 100 to gold and remaining silver
  * Returns : void
  * Syntax  : amt_to_player(to who, amount to give in silver)
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void amt_to_player (CHAR_DATA * ch, long amt)
{
  long in_gold = -1;

  if (amt >= 1000)
  {
    in_gold = amt / 100;
    amt -= (in_gold * 100);
  }

  if (in_gold > 0)
    ch->gold += in_gold;

  if (amt > 0)
    ch->silver += amt;

}


/** Function: amt_from_player
  * Descr   : Deducts an amount of monies from a players silver and gold stats.
  *         : Converts amounts over 100 to gold and remaining silver.
  * Returns : TRUE/FALSE on success
  * Syntax  : amt_from_player( from who, amount in silver to deduct )
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool amt_from_player (CHAR_DATA * ch, long amt)
{
  long in_gold = -1;
  long in_silver = -1;

  if (amt >= 1000)
  {
    in_gold = amt / 100;
    amt -= (in_gold * 100);
  }

  in_silver = amt;

  if (in_gold > 0)
  {
    if (in_gold > ch->gold)
      return FALSE;
    ch->gold -= in_gold;
  }

  if (in_silver > 0)
  {
    if (in_silver > ch->silver)
    {
      if (ch->gold < 1)
	return FALSE;

      ch->gold -= 1;
      ch->silver += 100;
    }
    ch->silver -= in_silver;
  }

  return TRUE;
}

/* convert short description to a keyword list, return it str_dup'ed */
char *short_to_name (char *short_desc)
{
  char name[MSL], tmp[MSL];
  char arg[MIL];

  name[0] = '\0';

  /* Strip color for short_desc */
  short_desc = strip_codes (short_desc);
  do
  {
    short_desc = one_argument (short_desc, arg);
    if (strlen (arg) > 2
	&& str_cmp (arg, "the")
	&& str_cmp (arg, "some") && str_cmp (arg, "with"))
    {
      sprintf (tmp, "%s%s%s", arg, name[0] ? " " : "", name);
      strcpy (name, tmp);
    }
  }
  while (arg[0]);

  if (!name[0])
    strcpy (name, "no name");

  return str_dup (name);

}
