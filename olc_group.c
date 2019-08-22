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

#define GREDIT( fun )            bool fun( CHAR_DATA *ch, char *argument )


/** Function: gredit_save
  * Descr   : Saves the group.dat file in the data directory
  * Returns : FALSE (to show unchanged status)
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_save)
{
  int i;

  for (i = 0; i < maxGroup; i++)
    if (IS_FLAG (group_table[i].flags, _CHANGED))
      REMOVE_FLAG (group_table[i].flags, _CHANGED);

  save_groups ();
  send_to_char ("Group data file saved.\n\r", ch);
  return FALSE;
}

/** Function: gredit_list
  * Descr   : Displays a listing of all groups to the user
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_list)
{
  int i;
  BUFFER *buf;
  char temp[MSL];

  buf = new_buf ();

  add_buf (buf, " ##   Group Name              # of Skills  ");

  for (i = 0; i < MAX_CLASS; i++)
    if (class_table[i].name)
    {
      sprintf (temp, "[%s] ", capitalize (class_table[i].who_name));
      add_buf (buf, temp);
    }

  add_buf (buf, "  Flags\n\r");

  for (i = 0; i < maxGroup; i++)
  {
    if (group_table[i].name && !IS_FLAG (group_table[i].flags, _DELETED))
    {
      int x = 0;
      int skills = 0;

      for (x = 0; x < MAX_IN_GROUP; x++)
	if ((group_table[i].spells[x]) &&
	    skill_lookup (group_table[i].spells[x]) > -1)
	  skills++;

      sprintf (temp, "[%2d]  %-25s    %2d      ",
	       i, group_table[i].name, skills);

      add_buf (buf, temp);

      for (x = 0; x < MAX_CLASS; x++)
      {
	sprintf (temp, "[%3d] ", group_table[i].rating[x]);
	add_buf (buf, temp);
      }
      sprintf (temp, "[%s]\n\r",
	       bit_string (group_flags, group_table[i].flags));
      add_buf (buf, temp);

    }
  }

  page_to_char (buf_string (buf), ch);
  free_buf (buf);
  return FALSE;
}


/** Function: gredit_delete
  * Descr   : Deletes a group from the array and consolidates array when
  *         : finished.
  * Returns : (n/a)
  * Syntax  : gredit delete ( # of group to delete )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_delete)
{
  GROUP_DATA *pGroup;
  int place = -1;

  EDIT_GROUP (ch, pGroup);

  if (argument[0] != '\0' && is_number (argument))
  {
    place = atoi (argument);

    if (place >= maxGroup || place < 0 ||
	!group_table[place].name || group_table[place].name[0] == '\0')
    {
      send_to_char ("Invalid group #.\n\r", ch);
      return FALSE;
    }

    pGroup = &group_table[place];
    ch->desc->pEdit = (void *) pGroup;
  }				/* end: if argument[0] != null */

  SET_FLAG (pGroup->flags, _DELETED);
  chprintf (ch, "[%s] is marked for deletion.", pGroup->name);
  return TRUE;
}


/** Function: gredit_show
  * Descr   : Displays the currently edited group to the editor.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_show)
{
  GROUP_DATA *pGroup;

  char buf[MIL];
  BUFFER *buffer;
  int i;

  EDIT_GROUP (ch, pGroup);

  buffer = new_buf ();

  sprintf (buf, "Group	: %-20s %s %s\n\r", pGroup->name,
	   IS_FLAG (pGroup->flags, _CHANGED) ? "`c[`B*`c]`w" : "",
	   IS_FLAG (pGroup->flags,
		    _DELETED) ? "`RMarked for Deletion!`w" : "");

  add_buf (buffer, buf);

  sprintf (buf, "Ratings : Channeler [%d], Rogue [%d] Warrior [%d]\n\r\n\r",
	   pGroup->rating[0], pGroup->rating[1], pGroup->rating[2]);
  add_buf (buffer, buf);

  add_buf (buffer, "Skills  :\n\r");
  for (i = 0; i < MAX_IN_GROUP; i++)
    if (pGroup->spells[i] != NULL && skill_lookup (pGroup->spells[i]) > -1)
    {
      sprintf (buf, "[%-2d] %s\n\r", i, pGroup->spells[i]);
      add_buf (buffer, buf);
    }

  sprintf (buf, "\n\rFlags: [%s]\n\r\n\r",
	   bit_string (group_flags, pGroup->flags));
  add_buf (buffer, buf);

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);

  return TRUE;
}


/** Function: gredit_create
  * Descr   : Creates a new group in the array.
  * Returns : TRUE/FALSE if successful
  * Syntax  : create new_group_name
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_create)
{
  GROUP_DATA *pGroup;
  int i;
  int place = -1;

  for (i = 0; i < maxGroup - 1; i++)
  {
    if (!group_table[i].name || IS_FLAG (group_table[i].flags, _DELETED))
    {
      place = i;
      break;
    }
  }

  if (place == -1)
  {
    send_to_char ("There's no empty slots to create a new group.\n\r", ch);
    return FALSE;
  }

  pGroup = get_group_data (place);

  free_string (pGroup->name);

  if (argument[0] != '\0')
    pGroup->name = str_dup (argument);
  else
    pGroup->name = str_dup ("new group");

  for (i = 0; i < MAX_CLASS; i++)
    pGroup->rating[i] = IM;	/* level IMMORTAL */

  for (i = 0; i < MAX_IN_GROUP; i++)
  {
    if (pGroup->spells[i])
    {
      free_string (pGroup->spells[i]);
      pGroup->spells[i] = str_dup ("");
    }
  }

  if (place > -1)
  {
    chprintf (ch, "New group setup in table position %d\n\r", place);
    pGroup = &group_table[place];
    ch->desc->pEdit = (void *) pGroup;
    return FALSE;
  }


  send_to_char ("Syntax: create [name]\n\r", ch);
  return FALSE;
}


/** Function: gredit_flags
  * Descr   : Toggles the group flags
  * Returns : TRUE/FALSE if changed
  * Syntax  : flags flag_to_toggle
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_flags)
{
  GROUP_DATA *pGroup;
  long value = 0;

  if (argument[0] != '\0')
  {
    EDIT_GROUP (ch, pGroup);

    if ((value = flag_value (group_flags, argument)) != NO_FLAG)
    {
      if (IS_FLAG (pGroup->flags, value))
	REMOVE_FLAG (pGroup->flags, value);
      else
	SET_FLAG (pGroup->flags, value);
      send_to_char ("Group flag(s) toggled.\n\r", ch);
      return TRUE;
    }
  }

  send_to_char ("Syntax: flag [flag ID]\n\r"
		"Type '? group' for a list of valid flags.\n\r", ch);
  return FALSE;

}


/** Function: gredit_name
  * Descr   : Sets the group name
  * Returns : TRUE/FALSE if successful
  * Syntax  : name new_name
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_name)
{
  GROUP_DATA *pGroup;

  if (argument[0] != '\0')
  {
    EDIT_GROUP (ch, pGroup);

    if (pGroup->name)
      free_string (pGroup->name);

    pGroup->name = str_dup (argument);
    chprintf (ch, "Group name changed to: %s\n\r", argument);
    return TRUE;
  }

  send_to_char ("Syntax: name (new name)\n\r", ch);
  return FALSE;
}


/** Function: gredit_ratings
  * Descr   : Edits a class rating within the group
  * Returns : TRUE/FALSE if changed
  * Syntax  : rating class_name new_value
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_ratings)
{
  char class[MSL];
  char value[MSL];

  GROUP_DATA *pGroup;

  EDIT_GROUP (ch, pGroup);

  if (argument[0] != '\0')
  {
    argument = one_argument (argument, class);
    argument = one_argument (argument, value);

    if (class_lookup (class) < 0)
    {
      send_to_char ("That class does not exist.\n\r", ch);
      return FALSE;
    }

    if (!is_number (value))
    {
      send_to_char ("Second argument must be numeric.\n\r", ch);
      return FALSE;
    }

    pGroup->rating[class_lookup (class)] = atoi (value);
    chprintf (ch, "New class rating is: %d", atoi (value));

    return TRUE;
  }

  send_to_char ("Syntax: rating class level\n\r", ch);
  return FALSE;
}


/** Function: gredit_skill
  * Descr   : Edits a skill within the group
  * Returns : TRUE/FALSE if successful
  * Syntax  : skill #of_skill_to_change new_skill|delete
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GREDIT (gredit_skill)
{
  GROUP_DATA *pGroup;
  char arg1[MSL], arg2[MSL];
  int skill = -1;
  int cur_max = -1;
  int i;

  EDIT_GROUP (ch, pGroup);

  if (argument[0] != '\0')
  {
    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);

    if (!is_number (arg1))
    {
      send_to_char ("You must provide the number of the skill to change.\n\r",
		    ch);
      return FALSE;
    }

    if (arg2[0] != '\0' &&
	str_cmp (arg2, "delete") && skill_lookup (arg2) < 0)
    {
      send_to_char ("That skill does not exist.\n\r", ch);
      return FALSE;
    }

    skill = atoi (arg1);

    for (i = 0; i < MAX_IN_GROUP; i++)
      if (pGroup->spells[i])
	cur_max++;

    if (skill > MAX_IN_GROUP)
    {
      send_to_char ("This group has already reached the maximum # of"
		    " skills allowed.\n\r", ch);
      return FALSE;
    }

    if (skill > cur_max + 1)
    {
      chprintf (ch, "The last set skill is #%d, resetting your choice"
		      " to %d\n\r", cur_max, cur_max + 1);
      skill = cur_max + 1;
    }

    if (pGroup->spells[skill])
      free_string (pGroup->spells[skill]);

    if (skill_lookup (arg2) > -1)
      pGroup->spells[skill] = str_dup (arg2);
    else
      /* reorganize array if needed */
    if (arg2[0] == '\0' || !str_cmp (arg2, "delete"))
    {
      if (skill < cur_max)
      {
	pGroup->spells[skill] = str_dup (pGroup->spells[cur_max]);
	free_string (pGroup->spells[cur_max]);
	pGroup->spells[cur_max] = str_dup ("");
      }
      else
	/* be sure to change if skill == cur_max too! */
      if (skill == cur_max)
      {
	free_string (pGroup->spells[skill]);
	pGroup->spells[skill] = str_dup ("");
      }

      send_to_char ("Skill deleted.\n\r", ch);
    }

    return TRUE;

  }

  send_to_char ("Syntax: skill skill# skill_name.\n\r", ch);
  return FALSE;
}
