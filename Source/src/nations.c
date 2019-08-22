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
 * @@ Objective (country.c)
 *
 * This file contains all of the support functions for dealing with players
 * home nations and cities.  Although, the load_country and save_country
 * code resides in data_h.c
 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include "merc.h"



/** Function: nation_count
  * Descr   : Determines the number of nations in the array
  * Returns : # of nations in the array
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int nation_count (void)
{
  int x = 0;
  int count = 0;

  for (x = 0; x < MAX_NATIONS; x++)
    if (!IS_NULLSTR (nation_table[x].nation))
      count++;
  return count;
}


/** Function: city_count
  * Descr   : Determines the number of cities in the city array
  * Returns : Number of cities in the array
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int city_count (void)
{
  int x = 0;
  int count = 0;

  for (x = 0; x < MAX_NATIONS; x++)
    if (!IS_NULLSTR (nation_table[x].nation))
    {
      int y;

      for (y = 0; y < MAX_CITIES; y++)
	if (!IS_NULLSTR (nation_table[x].cities[y].city))
	  count++;
    }

  return count;
}


/** Function: city_number
  * Descr   : Determines the position in the nation_table of a players
  *         : city.
  * Syntax  : city_number( whos? )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int city_number (char *argument)
{
  int x = 0, y = 0;

  if (argument == NULL || argument[0] == '\0')
    return -1;

  /* Loop through nation table */
  for (x = 0; x < MAX_NATIONS; x++)
  {
    /* Loop through cities in this nation */
    for (y = 0; y < MAX_CITIES; y++)
    {
      if (!IS_NULLSTR (nation_table[x].cities[y].city)
	  && !str_cmp (argument, nation_table[x].cities[y].city))
	return y;
    }
  }

  return -1;
}


/** Function: national_number
  * Descr   : Determines the position in the nation_table of a players
  *         : nation.
  * Syntax  : national_number( whos? )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int national_number (char *argument)
{
  int i = -1;

  if (argument == NULL || argument[0] == '\0')
    return -1;

  for (i = 0; i < MAX_NATIONS; i++)
    if (!IS_NULLSTR (nation_table[i].nation)
	&& !str_cmp (argument, nation_table[i].nation))
      return i;

  return -1;
}

/** Function: nation_lookup
  * Descr   : Determins a nations array position given a name prefix
  * Returns : position in the array if found, or -1 if not
  * Syntax  : nation_lookup( nation_name_prefix )
  * Written : v1.0 01/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int nation_lookup (char *argument)
{
  int i = -1;

  if (argument == NULL || argument[0] == '\0')
    return -1;

  for (i = 0; i < MAX_NATIONS; i++)
    if (!IS_NULLSTR (nation_table[i].nation)
	&& !str_prefix (argument, nation_table[i].nation))
      return i;

  return -1;
}

/** Function: country_lookup
  * Descr   : Determines a nation name from a given prefix
  * Returns : The full nation name in char* format
  * Syntax  : country_lookup( nation_name )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *country_lookup (char *argument)
{
  int i = -1;

  if (argument == NULL || argument[0] == '\0')
    return NULL;

  for (i = 0; i < MAX_NATIONS; i++)
    if (!IS_NULLSTR (nation_table[i].nation)
	&& !str_prefix (argument, nation_table[i].nation))
      return nation_table[i].nation;

  return NULL;
}


/** Function: city_lookup
  * Descr   : Determines a city name given a prefix
  * Returns : The full city name, in char* format
  * Syntax  : city_lookup( city_name )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *city_lookup (char *argument)
{
  int x = 0;
  int y = 0;

  if (argument == NULL || argument[0] == '\0')
    return NULL;

  for (x = 0; x < MAX_NATIONS; x++)
    for (y = 0; y < MAX_CITIES; y++)
      if (!IS_NULLSTR (nation_table[x].cities[y].city)
	  && !str_prefix (argument, nation_table[x].cities[y].city))
	return nation_table[x].cities[y].city;

  return NULL;
}
