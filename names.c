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
 * @@ Objective (names.c)
 * 
 * WoT uses a linked list to store names that have been flagged as illegal
 * names for one reason or another. The code to support this is kept here.
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/** Function: name_is_banned
  * Descr   : Determines if the passed name is included in the banned list
  * Returns : TRUE if found
  * Syntax  : name_is_banned( name in question )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool name_is_banned (char *name)
{
  NAME_DATA *Nm;

  for (Nm = name_first; Nm != NULL; Nm = Nm->next)
    if (!str_cmp (name, Nm->name))
      return TRUE;

  return FALSE;
}


/** Function: show_banned_list
  * Descr   : Displays the banned names list to the player. Will display
  *         : in detail, or compressed format
  * Returns : output only
  * Syntax  : show_banned_list ( to who, [detail] )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void show_banned_list (CHAR_DATA * ch, char *argument)
{
  NAME_DATA *Nm;
  char arg[MIL];
  bool detail = FALSE;
  int cnt = 0;
  BUFFER *buf;
  char temp[MSL];


  argument = one_argument (argument, arg);

  detail = (arg[0] != '\0' && !str_prefix (arg, "detailed"));

  buf = new_buf ();

  if (ch->level < L6)
  {
    send_to_char ("You dont have access to this command.\n\r", ch);
    return;
  }

  if (detail)
    send_to_char
      ("[##] Name         Banned by    Ban Lvl   BannedOn  Reason\n\r"
       "---------------------------------------------------------"
       "--------------------\n\r", ch);

  for (Nm = name_first; Nm; Nm = Nm->next)
  {
    if (Nm->name)
    {
      if (detail)
      {
	struct tm *date;
	char dtstr[11];

	date = localtime (&Nm->date_stamp);
	sprintf (dtstr, "%02d/%02d/%02d",
		 date->tm_mon, date->tm_mday, date->tm_year);

	sprintf (temp,
		 "%3d  %-12.12s %-12.12s    %2d     %s  %-25.25s\n\r",
		 ++cnt, Nm->name, Nm->by, Nm->by_lvl, dtstr, Nm->reason);

	add_buf (buf, temp);

      }
      else
      {
	sprintf (temp, "%-12.12s %s",
		 Nm->name, (++cnt % 4 == 0) ? "\n\r" : "  ");
	add_buf (buf, temp);
      }
    }
  }

  page_to_char (buf_string (buf), ch);
  free_buf (buf);
  return;

}


/** Function: ban_name
  * Descr   : Adds a name and reason for ban to banned names list.
  * Returns : void
  * Syntax  : ban_name (caller, who to ban, why? )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Note    : Multiple word names, or names containing an ' character,
  *         : need to be surrounded in double quotes for this to work
  *         : properly
  */
void ban_name (CHAR_DATA * ch, char *argument)
{
  char *undo = "undo /undo remove /remove delete /delete";
  NAME_DATA *Nm;
  char name[MIL];

  /* ch may at times, equal null, if system calls this function
     instead of a player, so be sure to account for that. */

  argument = one_argument (argument, name);

  if (name[0] == '\0' || !str_prefix (name, "/show"))
  {
    if (ch != NULL)
      show_banned_list (ch, "detailed");
    return;
  }

  if (is_name (name, undo))
  {
    argument = one_argument (argument, name);
    unban_name (ch, name);
    return;
  }

  if (!str_prefix (name, "/save"))
  {
    save_names ();
    send_to_char ("Banned names database saved.\n\r", ch);
    return;
  }

  if (name_is_banned (name))
  {
    if (ch != NULL)
      send_to_char ("That name is already banned.\n\r", ch);
    return;
  }

  Nm = new_name ();

  if (!name_first)
    name_first = Nm;

  if (name_last)
    name_last->next = Nm;

  name_last = Nm;
  Nm->next = NULL;

  Nm->name = str_dup (name);

  if (ch == NULL)
  {
    Nm->by = str_dup ("System");
    Nm->by_lvl = ML - 2;
  }
  else
  {
    Nm->by = str_dup (ch->name);
    Nm->by_lvl = ch->level;
  }

  if (argument[0] != '\0')
    Nm->reason = str_dup (argument);

  Nm->date_stamp = current_time;

  if (ch != NULL)
    chprintf (ch, "%s added to banned name list.\n\r", Nm->name);

  save_names ();

  return;
}


/** Function: unban_name
  * Descr   : Removes a banned name from the ban listing.
  * Returns : void
  * Syntax  : unban_name ( name_to_remove )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void unban_name (CHAR_DATA * ch, char *argument)
{
  NAME_DATA *Nm;
  NAME_DATA *previous;
  char name[MIL];

  previous = NULL;

  argument = one_argument (argument, name);

  if (ch != NULL && ch->level < ML - 2)
  {
    send_to_char ("You don't have the authority.\n\r", ch);
    return;
  }

  if (name[0] == '\0')
  {
    if (ch != NULL)
      send_to_char ("Remove who from the list?.\n\r", ch);
    return;
  }

  if (!name_is_banned (name))
  {
    if (ch != NULL)
      send_to_char ("That name is not on the list.\n\r", ch);
    return;
  }

  for (Nm = name_first; Nm != NULL; Nm = Nm->next)
  {
    if (!str_cmp (name, Nm->name))
    {
      if (Nm == name_first)
      {
	previous = Nm->next;
	name_first = previous;
	free_name (Nm);
	send_to_char ("Name removed from list.\n\r", ch);
	save_names ();
	return;
      }
      else
       if (Nm == name_last)
      {
	previous->next = NULL;
	name_last = previous;
	free_name (Nm);
	send_to_char ("Name removed from list.\n\r", ch);
	save_names ();
	return;
      }
      else

      {
	previous->next = Nm->next;
	free_name (Nm);
	send_to_char ("Name removed from list.\n\r", ch);
	save_names ();
	return;
      }
    }

    previous = Nm;
  }

}
