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
 * @@ Objective (lookup.c)
 * 
 * Originally containing only 4 of the various '_lookup' functions, I've
 * since moved the majority of the rest here.  Any other _lookup functions
 * are probably taking residence in specific source files related to their
 * family of functions. (such as clan_lookup in guild.c)
 */


#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"

int form_lookup(char *form)
{
	int i=0;

	for(i=0; i < maxForm; i++)
		if ( LOWER(form[0]) == LOWER(form_table[i].form[0]) && 
			!str_cmp(form_table[i].form, form) )
			return i;

	return -1;
}

int form_lookup_sleazy(char *form)
{
	int i=0;

	for(i=0; i < maxForm; i++)
		if ( LOWER(form[0]) == LOWER(form_table[i].form[0]) && 
			!str_prefix(form, form_table[i].form) )
			return i;

	return -1;
}


int flag_lookup (const char *name, const struct flag_type *flag_table)
{
  int flag;

  if (!name || name[0] == '\0')
    return NO_FLAG;

  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if (LOWER (name[0]) == LOWER (flag_table[flag].name[0])
	&& !str_prefix (name, flag_table[flag].name))
      return flag_table[flag].bit;
  }

  return NO_FLAG;
}

int flag_lookup_exact (const char *name, const struct flag_type *flag_table)
{
  int flag;

  if (!name || name[0] == '\0')
    return NO_FLAG;

  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if (LOWER (name[0]) == LOWER (flag_table[flag].name[0])
	&& !str_cmp (name, flag_table[flag].name))
      return flag_table[flag].bit;
  }

  return NO_FLAG;
}

HELP_DATA *help_lookup (char *keyword)
{
  HELP_DATA *pHelp;
  char temp[MIL], argall[MIL];

  argall[0] = '\0';

  if (!keyword || keyword[0] == '\0')
    return NULL;

  while (keyword[0] != '\0')
  {
    keyword = one_argument (keyword, temp);

    if (argall[0] != '\0')
      strcat (argall, " ");

    strcat (argall, temp);
  }

  for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
    if (is_name (argall, pHelp->keyword))
      return pHelp;

  return NULL;
}


/* returns a group index number given the name */
int group_lookup (const char *name)
{
  int gn;

  if (!name || name[0] == '\0')
    return -1;

  for (gn = 0; gn < maxGroup; gn++)
  {
    if (group_table[gn].name == NULL)
      break;
    if (LOWER (name[0]) == LOWER (group_table[gn].name[0])
	&& !str_prefix (name, group_table[gn].name))
      return gn;
  }

  return -1;
}


int talent_lookup (const char *name)
{
  int sn = -1;

  if (!name || name[0] == '\0')
    return -1;

  for (sn = 0; sn < MAX_TALENTS; sn++)
  {
    if (talent_table[sn].name == NULL)
      break;
    if (!str_prefix (name, talent_table[sn].name))
      return sn;
  }

  return -1;
}


enum _POS_STATES position_lookup (const char *name)
{
  enum _POS_STATES pos;

  if (!name || name[0] == '\0')
    return -1;

  for (pos = POS_DEAD; position_table[pos].name != NULL; pos++)
  {
    if (LOWER (name[0]) == LOWER (position_table[pos].name[0])
	&& !str_prefix (name, position_table[pos].name))
      return pos;
  }

  return POS_INVALID;
}

enum _SEX sex_lookup (const char *name)
{
  enum _SEX sex;

  if (!name || name[0] == '\0')
    return -1;

  for (sex = SEX_NEUTRAL; sex_table[sex].name != NULL; sex++)
  {
    if (LOWER (name[0]) == LOWER (sex_table[sex].name[0])
	&& !str_prefix (name, sex_table[sex].name))
      return sex;
  }

  return SEX_INVALID;
}

int size_lookup (const char *name)
{
  enum _SIZE size;

  if (!name || name[0] == '\0')
    return -1;

  for (size = 0; size_table[size].name != NULL; size++)
  {
    if (LOWER (name[0]) == LOWER (size_table[size].name[0])
	&& !str_prefix (name, size_table[size].name))
      return size;
  }

  return -1;
}

/* returns material number */
int material_lookup (const char *name)
{
  return 0;
}


struct skill_hash_node
{
	struct skill_hash_node *next;
	const struct skill_type *skill;
	int table_pos;
};

static struct skill_hash_node *skill_hash_table[26];

void init_skill_hash_table()
{
  int c, i, pos;

  for (i = 0; i < maxSkill; i++)
  {
    struct skill_hash_node *node;

    node = alloc_perm (sizeof (struct skill_hash_node));
    node->skill = &skill_table[i];
	node->table_pos = i;

    c = LOWER (skill_table[i].name[0]);

    if (c < 'a' || c > 'z' )
    {
      bugf ("[lookup.c::init_skill_hash_table] Skill %d has illegal name: %s", i, skill_table[i].name);
      _quit (1);
    }

	pos = c - 'a';

    node->next = skill_hash_table[pos];
    skill_hash_table[pos] = node;
  }
}

/* 
 * Credit note:
 * The following race hash code and lookup provided by 
 * Erwin Andreasen
 */
struct race_hash_node
{
  struct race_hash_node *next;
  const struct race_type *race;
};

static struct race_hash_node *race_hash_table[MAX_RACE_HASH];

void init_race_hash_table ()
{
  int c, i;

  for (i = 0; i < maxRace; i++)
  {
    struct race_hash_node *node;

    node = alloc_perm (sizeof (struct race_hash_node));
    node->race = &race_table[i];

    c = LOWER (race_table[i].name[0]);

    if (c <= 0 || c >= MAX_RACE_HASH)
    {
      bugf ("[lookup.c::init_race_hash_table] Race %d has illegal name: %s", i, race_table[i].name);
      _quit(1);
    }

    node->next = race_hash_table[c];
    race_hash_table[c] = node;
  }
}

int skill_lookup (const char *skill)
{
  int c, pos, count=0;
  struct skill_hash_node *node;

  if ( IS_NULLSTR(skill) )
    return maxSkill;

  c = LOWER (skill[0]);		/* Our hash key is the first letter */

  if (c < 'a' || c > 'z')
  {
    bugf ("[lookup.c::skill_lookup] called with illegal name: %s", skill);
    return maxSkill;
  }

  pos = c - 'a';

  for (node = skill_hash_table[pos]; node; node = node->next)
  {
	++count;
	if (!str_prefix (skill, node->skill->name))
      return (node->table_pos);
  }

  return maxSkill;
}


int reference_skill(const char *skill)
{
  int c, pos, count=0;
  struct skill_hash_node *node;

  if ( IS_NULLSTR(skill) )
    return maxSkill;

  c = LOWER (skill[0]);		

  if (c < 'a' || c > 'z')
  {
    bugf ("[lookup.c::reference_skill] called with illegal name: %s", skill);
    return maxSkill;
  }

  pos = c - 'a';

  for (node = skill_hash_table[pos]; node; node = node->next)
  {
	++count;
	if (!str_cmp(skill, node->skill->name))
      return (node->table_pos);
  }

  return maxSkill;
}


/** Function: race_lookup
  * Descr   : Searches the race hash table for a particular race name
  * Returns : race position in the table, or -1 if not found
  * Syntax  : race_lookup ( race_name )
  * Written : v1.0 added 11/98
  * Author  : Erwin Andreasen
  */
int race_lookup (const char *race)
{
  int c;
  struct race_hash_node *node;

  if (!race || race[0] == '\0')
    return -1;

  c = LOWER (race[0]);		/* Our hash key is the first letter */

  if (c <= 0 || c >= MAX_RACE_HASH)
  {
    bugf ("[lookup.c::race_lookup] called with illegal name: %s", race);
    return -1;
  }

  for (node = race_hash_table[c]; node; node = node->next)
    if (!str_prefix (race, node->race->name))
      return (node->race - race_table);

  return -1;
}


int liq_lookup (const char *name)
{
  int liq;

  if (!name || name[0] == '\0')
    return -1;

  for (liq = 0; liq_table[liq].liq_name != NULL; liq++)
  {
    if (LOWER (name[0]) == LOWER (liq_table[liq].liq_name[0])
	&& !str_prefix (name, liq_table[liq].liq_name))
      return liq;
  }

  return -1;
}

int weapon_lookup (const char *name)
{
  enum _WEAPON_CLASS type;

  if (!name || name[0] == '\0')
    return -1;

  for (type = 0; weapon_table[type].name != NULL; type++)
  {
    if (LOWER (name[0]) == LOWER (weapon_table[type].name[0])
	&& !str_prefix (name, weapon_table[type].name))
      return type;
  }

  return -1;
}

int item_lookup (const char *name)
{
  enum _ITEM_TYPE type;

  if (!name || name[0] == '\0')
    return -1;

  for (type = 0; item_table[type].name != NULL; type++)
  {
    if (LOWER (name[0]) == LOWER (item_table[type].name[0])
	&& !str_prefix (name, item_table[type].name))
      return item_table[type].type;
  }

  return -1;
}

int attack_lookup (const char *name)
{
  int att;

  if (!name || name[0] == '\0')
    return 0;

  for (att = 0; attack_table[att].name != NULL; att++)
  {
    if (LOWER (name[0]) == LOWER (attack_table[att].name[0])
	&& !str_prefix (name, attack_table[att].name))
      return att;
  }

  return 0;
}

/* returns a flag for wiznet */
long wiznet_lookup (const char *name)
{
  int flag;

  if (!name || name[0] == '\0')
    return -1;

  for (flag = 0; wiznet_table[flag].name != NULL; flag++)
  {
    if (LOWER (name[0]) == LOWER (wiznet_table[flag].name[0])
	&& !str_prefix (name, wiznet_table[flag].name))
      return flag;
  }

  return -1;
}

/* returns class number */
int class_lookup (const char *name)
{
  int class;

  if (!name || name[0] == '\0')
    return -1;

  for (class = 0; class < MAX_CLASS; class++)
  {
    if (LOWER (name[0]) == LOWER (class_table[class].name[0])
	&& !str_prefix (name, class_table[class].name))
      return class;
  }

  return -1;
}
