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

   /* The following code is based on ILAB OLC by Jason Dinkel & Ivan */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "recycle.h"

#define GEDIT( fun )            bool fun( CHAR_DATA *ch, char *argument )


/** Function: gedit_flags
  * Descr   : Sets the various flags associated with guilds.
  * Returns : True/False if changed.
  * Syntax  : flags flag_id...
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_flags)
{
  CLAN_DATA *pClan;
  long value = 0;

  if (argument[0] != '\0')
  {
    EDIT_GUILD (ch, pClan);

    if ((value = flag_lookup (argument, guild_flags)) != NO_FLAG)
    {
      if (IS_FLAG (pClan->flags, value))
	REMOVE_FLAG (pClan->flags, value);
      else
	SET_FLAG (pClan->flags, value);
      send_to_char ("Guild flag(s) toggled.\n\r", ch);
      return TRUE;
    }
  }

  send_to_char ("Syntax: flag [flag ID]\n\r"
		"Type '? guild' for a list of valid flags.\n\r", ch);
  return FALSE;
}

/** Function: gedit_rank
  * Descr   : Sets the guild rank name
  * Returns : True/False if changed.
  * Syntax  : rank rank_id rank_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_rank)
{
  CLAN_DATA *pClan;
  char arg1[4];


  EDIT_GUILD (ch, pClan);

  argument = one_argument (argument, arg1);

  if (is_number (arg1) && atoi (arg1) <= MAX_RANK)
  {
    int value;

    value = atoi (arg1) - 1;

    if (argument[0] != '\0')
    {
      free_string (pClan->rank[value].rankname);
      pClan->rank[value].rankname = str_dup (argument);
      send_to_char ("Rank name changed.\n\r", ch);
      return TRUE;
    }

  }

  send_to_char ("Syntax: rank rank# newname\n\r", ch);
  return FALSE;
}

/** Function: gedit_skill
  * Descr   : Sets the skills associated with the various ranks.
  * Returns : True/False if changed & notice if skill dosent exist.
  * Syntax  : skill rank_# skill_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_skill)
{
  CLAN_DATA *pClan;
  char arg1[4];


  EDIT_GUILD (ch, pClan);

  argument = one_argument (argument, arg1);

  if (is_number (arg1) && atoi (arg1) <= MAX_RANK)
  {
    int value;

    value = atoi (arg1) - 1;

    if (argument[0] != '\0')
    {
      free_string (pClan->rank[value].skillname);
      pClan->rank[value].skillname = str_dup (argument);
      if (skill_lookup (argument) == -1)
	send_to_char ("Notice: That skill does not exist.\n\r", ch);
      send_to_char ("Skill changed.\n\r", ch);
      return TRUE;
    }

  }

  send_to_char ("Syntax: skill rank# newskill\n\r", ch);
  return FALSE;
}


/** Function: gedit_create
  * Descr   : Creates a new, empty guild if room is available in the array.
  * Returns : True/False if created
  * Syntax  : (n/a)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_create)
{
  int i = maxClan + 1;
  int x = 0;
  CLAN_DATA *pClan;
  struct clan_type *new_table;

  maxClan++;
  new_table = realloc (clan_table, sizeof (struct clan_type) * maxClan);

  if (!new_table)
  {
    send_to_char ("Memory Allocation Failed!!! Unable to create guild.\n\r",
		  ch);
    return FALSE;
  }

  clan_table = new_table;

  clan_table[i].name = str_dup ("New Guild");
  clan_table[i].who_name = str_dup ("New Guild");

  for (x = 0; x < MAX_RANK; x++)
  {
    clan_table[i].rank[x].rankname = str_dup ("Empty");
    clan_table[i].rank[x].skillname = str_dup ("");
  }

  pClan = &clan_table[maxClan];
  ch->desc->pEdit = (void *) pClan;
  send_to_char ("Guild created.\n\r", ch);
  return TRUE;
}


/** Function: gedit_list
  * Descr   : List's all of the current guilds, including those not yet
  *         : saved to the data file, and those marked for deletion.
  * Returns : list of guilds.
  * Syntax  : (N/A)
  * Written : v1.0 3/98 Updated on 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_list)
{
  char buf[MIL];
  BUFFER *buffer;
  int i;
  int count = 0;

  buffer = new_buf ();

  sprintf (buf, "Num  Guild Name                      Flags\n\r"
	   "-----------------------------------------------\n\r");
  add_buf (buffer, buf);

  for (i = 0; i < maxClan; i++)
  {
    if (clan_table[i].name != NULL && clan_table[i].name[0] != '\0')
    {
      count++;
      sprintf (buf, "[%2d]  %-30s [%s]\n\r",
	       i + 1,
	       clan_table[i].name,
	       bit_string (guild_flags, clan_table[i].flags));
      add_buf (buffer, buf);
    }
  }

  sprintf (buf, "\n\r-----------------------------------------------\n\r");
  add_buf (buffer, buf);

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);
  return FALSE;
}


/** Function: gedit_show
  * Descr   : Displays currently selected guild data to the screen.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_show)
{
  CLAN_DATA *pClan;

  char buf[MIL];
  BUFFER *buffer;
  int i;

  EDIT_GUILD (ch, pClan);

  buffer = new_buf ();

  sprintf (buf, "Name     : %s %s %s\n\rWho Name : %-10s\n\r",
	   pClan->name,
	   IS_FLAG (pClan->flags, _CHANGED) ? "`c[`B*`c]`w" : "",
	   IS_FLAG (pClan->flags, _DELETED) ? "`RMarked for Deletion!`w" : "",
	   pClan->who_name);
  add_buf (buffer, buf);

  sprintf (buf,
	   "Recall Room [%-6d]  Morgue Room [%-6d]  Clan Temple [%-6d]\n\r\n\r",
	   pClan->room[0], pClan->room[1], pClan->room[2]);
  add_buf (buffer, buf);

  sprintf (buf, "Flags:      [%s]\n\r\n\r",
	   bit_string (guild_flags, pClan->flags));
  add_buf (buffer, buf);

  sprintf (buf,
	   "#    Rank                      Skill Associated with this Rank\n\r--------------------------------------------------------------\n\r");
  add_buf (buffer, buf);

  for (i = 0; i < MAX_RANK; i++)
  {
    if (!IS_NULLSTR (pClan->rank[i].rankname)
	&& str_cmp (pClan->rank[i].rankname, "Unassigned"))
    {
      sprintf (buf, "[%2d] %-25s %s\n\r",
	       i + 1,
	       (pClan->rank[i].rankname !=
		'\0') ? pClan->rank[i].rankname : "None",
	       (pClan->rank[i].skillname !=
		'\0') ? pClan->rank[i].skillname : "");
      add_buf (buffer, buf);
    }
  }

  add_buf (buffer, "\n\r");
  sprintf (buf, "Mortal Leader Rights\n\r--------------------\n\r");
  add_buf (buffer, buf);

  sprintf (buf,
	   "Can Guild   : %s\n\rCan Deguild : %s\n\rCan Promote : %s\n\rCan Demote  : %s\n\r",
	   (pClan->ml[0] == 1) ? "True" : "False",
	   (pClan->ml[1] == 1) ? "True" : "False",
	   (pClan->ml[2] == 1) ? "True" : "False",
	   (pClan->ml[3] == 1) ? "True" : "False");
  add_buf (buffer, buf);

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);
  return FALSE;
}


/** Function: gedit_name
  * Descr   : Changes the name of the currently selected guild.
  * Returns : True/False if modified
  * Syntax  : name new_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_name)
{
  CLAN_DATA *pClan;

  EDIT_GUILD (ch, pClan);

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax:  name [name]\n\r", ch);
    return FALSE;
  }

  if (clan_lookup (argument) > 0)
  {
    send_to_char ("That guild already exists.\n\r", ch);
    return FALSE;
  }

  if (pClan->name[0] != '\0')
    free_string (pClan->name);

  pClan->name = str_dup (argument);

  send_to_char ("Name set.\n\r", ch);
  return TRUE;
}


/** Function: gedit_whoname
  * Descr   : Changes the who_name of the currently selected guild.
  * Returns : True/False if modified.
  * Syntax  : whoname new_who_name
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_whoname)
{
  CLAN_DATA *pClan;

  EDIT_GUILD (ch, pClan);

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax:  whoname [name]\n\r", ch);
    return FALSE;
  }

  if (pClan->who_name[0] != '\0')
    free_string (pClan->who_name);

  pClan->who_name = str_dup (argument);

  send_to_char ("Who Name set.\n\r", ch);
  return TRUE;
}


/** Function: gedit_rooms
  * Descr   : Changes the vnum of the room(s) selected.
  * Returns : True/False if modified.
  * Syntax  : rooms [hall new#|morgue new#|temple new#]
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_rooms)
{
  CLAN_DATA *pClan;
  char arg1[10], arg2[10], arg3[10], arg4[10], arg5[10], arg6[10];
  bool set = FALSE;

  EDIT_GUILD (ch, pClan);

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax:  rooms [hall #|morgue #|temple #]\n\r", ch);
    return FALSE;
  }

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  argument = one_argument (argument, arg3);
  argument = one_argument (argument, arg4);
  argument = one_argument (argument, arg5);
  argument = one_argument (argument, arg6);

  /* kinda of convoluted, I know, but this seemed the easiest way
     to allow for any combination of commands. ie:
     hall 1 morgue 2 temple 3 
     morgue 2 hall 3 temple 1
     or even
     hall 1
     morgue 2 hall 1
   */
  if (!str_cmp (arg1, "hall") && is_number (arg2))
  {
    pClan->room[0] = atoi (arg2);
    set = TRUE;
  }
  else if (!str_cmp (arg1, "morgue") && is_number (arg2))
  {
    pClan->room[1] = atoi (arg2);
    set = TRUE;
  }
  else if (!str_cmp (arg1, "temple") && is_number (arg2))
  {
    pClan->room[2] = atoi (arg2);
    set = TRUE;
  }

  if (!str_cmp (arg3, "hall") && is_number (arg4))
  {
    pClan->room[0] = atoi (arg4);
    set = TRUE;
  }
  else if (!str_cmp (arg3, "morgue") && is_number (arg4))
  {
    pClan->room[1] = atoi (arg4);
    set = TRUE;
  }
  else if (!str_cmp (arg3, "temple") && is_number (arg4))
  {
    pClan->room[2] = atoi (arg4);
    set = TRUE;
  }

  if (!str_cmp (arg5, "hall") && is_number (arg6))
  {
    pClan->room[0] = atoi (arg6);
    set = TRUE;
  }
  else if (!str_cmp (arg5, "morgue") && is_number (arg6))
  {
    pClan->room[1] = atoi (arg6);
    set = TRUE;
  }
  else if (!str_cmp (arg5, "temple") && is_number (arg6))
  {
    pClan->room[2] = atoi (arg6);
    set = TRUE;
  }

  if (set)
  {
    send_to_char ("Room(s) set.\n\r", ch);
    return TRUE;
  }

  return FALSE;
}


/** Function: gedit_ml
  * Descr   : Changes the mortal leader settings.
  * Returns : True/False if modified
  * Syntax  : ml [true/false|true/false|true/false|true/false]
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_ml)
{
  CLAN_DATA *pClan;
  char arg1[6], arg2[6], arg3[6], arg4[6];
  bool set = FALSE;

  EDIT_GUILD (ch, pClan);

  if (argument[0] == '\0')
  {
    send_to_char
      ("Syntax:  ml TRUE|FALSE TRUE|FALSE TRUE|FALSE TRUE|FALSE\n\r", ch);
    return FALSE;
  }

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  argument = one_argument (argument, arg3);
  argument = one_argument (argument, arg4);

  if (!str_prefix (arg1, "true"))
  {
    pClan->ml[0] = TRUE;
    set = TRUE;
  }
  else if (!str_prefix (arg1, "false"))
  {
    pClan->ml[0] = FALSE;
    set = TRUE;
  }

  if (!str_prefix (arg2, "true"))
  {
    pClan->ml[1] = TRUE;
    set = TRUE;
  }
  else if (!str_prefix (arg2, "false"))
  {
    pClan->ml[1] = FALSE;
    set = TRUE;
  }

  if (!str_prefix (arg3, "true"))
  {
    pClan->ml[2] = TRUE;
    set = TRUE;
  }
  else if (!str_prefix (arg3, "false"))
  {
    pClan->ml[2] = FALSE;
    set = TRUE;
  }

  if (!str_prefix (arg4, "true"))
  {
    pClan->ml[3] = TRUE;
    set = TRUE;
  }
  else if (!str_prefix (arg4, "false"))
  {
    pClan->ml[3] = FALSE;
    set = TRUE;
  }

  if (set)
  {
    send_to_char ("Mortal Leader traits set.\n\r", ch);
    return TRUE;
  }

  return FALSE;
}

/** Function: gedit_delete
  * Descr   : Delete a guild from the clan_table.
  * Returns : True/False if modified
  * Syntax  : delete (2x)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GEDIT (gedit_delete)
{
  CLAN_DATA *pClan;

  EDIT_GUILD (ch, pClan);

  if (argument[0] != '\0')
  {
    if (IS_FLAG (pClan->flags, _DELETED))
    {
      send_to_char ("Delete cancelled.\n\r", ch);
      return TRUE;
    }
    else
      send_to_char ("No deletion was in progress.\n\r", ch);

    return FALSE;
  }

  if (!IS_FLAG (pClan->flags, _DELETED))
  {
    send_to_char
      ("Typing 'delete' again will permanetely remove this guild!\n\r"
       "-or- 'delete' with any argument will cancel the deletion.\n\r", ch);
    SET_FLAG (pClan->flags, _DELETED);
    SET_FLAG (pClan->flags, _CHANGED);
    return FALSE;
  }
  else
    /* Delete it now */
  {
    int i, j = 0, c;
    struct clan_type *new_table =
      malloc (sizeof (struct clan_type) * maxClan);

    if (!new_table)
    {
      send_to_char ("Memory Allocation error!!! Unable to delete guild.", ch);
      return FALSE;
    }

    c = clan_lookup (pClan->name);

    for (i = 0; i < maxClan; i++)
      if (i != c)
	new_table[j++] = clan_table[i];

    free (clan_table);
    clan_table = new_table;
    maxClan--;

    pClan = &clan_table[0];
    ch->desc->pEdit = (void *) pClan;

    send_to_char ("Guild deleted.\n\r", ch);
  }

  return TRUE;
}
