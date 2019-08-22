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

#define SKEDIT( fun )            bool fun( CHAR_DATA *ch, char *argument )


/** Function: skedit_flowstr
  * Descr   : Sets the flow strength values for skill/weave
  * Returns : TRUE/FALSE if changed.
  * Syntax  : flowstr flow value
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_flowstr)
{
  char *flows = "air earth water fire spirit";
  SKILL_DATA *pSkill;
  char arg1[MIL];
  int value = 0;

  EDIT_SKILL (ch, pSkill);

  argument = one_argument (argument, arg1);

  if (!pSkill->weave_fun)
  {
    send_to_char ("This is a skill, not a weave. It has no OPR value.\n\r",
		  ch);
    return FALSE;
  }

  if (arg1[0] != '\0')
  {

    if (!is_name (arg1, flows))
    {
      send_to_char
	("Invalid flow. Valid flows are: air/earth/water/fire/spirit\n\r",
	 ch);
      return FALSE;
    }

    if (argument[0] == '\0' || !is_number (argument))
    {
      send_to_char ("Invalid flow strength value.\n\r", ch);
      return FALSE;
    }

    value = atoi (argument);

    if (value < 0)
    {
      send_to_char ("Negative values are not acceptable.\n\r", ch);
      return FALSE;
    }

//    switch (UPPER (arg1[0]))
//   {
//    case 'A':
//      pSkill->flow_str[0] = value;
//     break;
//    case 'E':
//      pSkill->flow_str[1] = value;
//      break;
//    case 'W':
//      pSkill->flow_str[2] = value;
//     break;
//    case 'F':
//     pSkill->flow_str[3] = value;
//      break;
//    case 'S':
//     pSkill->flow_str[4] = value;
//     break;
//  }

    send_to_char ("Flow strength value set.\n\r", ch);
    return TRUE;
  }

  send_to_char
    ("Syntax: flowstr flow (air/earth/water/fire/spirit) value\n\r", ch);
  return FALSE;
}


/** Function: skedit_learn
  * Descr   : Sets the class ratings for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : learn class_name value
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_learn)
{
  char class[MSL];
  char value[MSL];
  SKILL_DATA *pSkill;

  EDIT_SKILL (ch, pSkill);

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

//    pSkill->rating[class_lookup (class)] = atoi (value);
    chprintf (ch, "New class rating is: %d", atoi (value));

    return TRUE;
  }

  send_to_char ("Syntax: learn class rating\n\r", ch);
  return FALSE;
}


/** Function: skedit_levels
  * Descr   : Sets the class levels for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : level class_name value
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_levels)
{
  char class[MSL];
  char value[MSL];
  SKILL_DATA *pSkill;

  EDIT_SKILL (ch, pSkill);

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

    pSkill->skill_level[class_lookup (class)] = atoi (value);
    chprintf (ch, "New class level is: %d", atoi (value));

    return TRUE;
  }

  send_to_char ("Syntax: level class level\n\r", ch);
  return FALSE;
}



/** Function: skedit_target
  * Descr   : Sets the valid targets for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : target target_type
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_target)
{
  SKILL_DATA *pSkill;
  int i = 0;

  EDIT_SKILL (ch, pSkill);

  i = flag_pos (target_flags, argument);

  if (i == -1)
  {
    send_to_char ("Invalid Target.  Valid targets are:\n\r", ch);

    for (i = 0; target_flags[i].name != NULL; i++)
      chprintf (ch, "%s\n\r", target_flags[i].name);

    return FALSE;
  }
  else
  {
    pSkill->target = target_flags[i].bit;
    return TRUE;
  }

  send_to_char ("Syntax: target target_type.  Valid targets are:\n\r", ch);

  for (i = 0; target_flags[i].name != NULL; i++)
    chprintf (ch, "%s\n\r", target_flags[i].name);

  return FALSE;

}



/** Function: skedit_position
  * Descr   : Sets the minimum position for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : position position_type
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_position)
{
  SKILL_DATA *pSkill;
  int i = 0;

  EDIT_SKILL (ch, pSkill);

  i = flag_pos (position_flags, argument);

  if (i == -1)
  {
    send_to_char ("Invalid position.  Valid positions are:\n\r", ch);

    for (i = 0; position_flags[i].name != NULL; i++)
      chprintf (ch, "%s\n\r", position_flags[i].name);

    return FALSE;
  }
  else
  {
    send_to_char ("Position Set.\n\r", ch);
    pSkill->minimum_position = position_flags[i].bit;
    return TRUE;
  }

  send_to_char ("Syntax: flag flag_name.  Valid flags are:\n\r", ch);

  for (i = 0; position_flags[i].name != NULL; i++)
    chprintf (ch, "%s\n\r", position_flags[i].name);

  return FALSE;
}


/** Function: skedit_wait
  * Descr   : Sets the wait state value for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : wait value (in pulses)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_wait)
{
  int value = 0;
  SKILL_DATA *pSkill;

  EDIT_SKILL (ch, pSkill);

  if (argument[0] != '\0' && is_number (argument))
  {

    value = atoi (argument);

    if (value < 0)
    {
      send_to_char ("Negative values are not valid.\n\r", ch);
      return FALSE;
    }

    if (value >= 24)
      chprintf (ch, "Warning: high value (%d) set.\n\r", value);
    else
      chprintf (ch, "Value of %d set.\n\r", value);

    pSkill->beats = value;
    send_to_char ("Value set.\n\r", ch);
    return TRUE;

  }

  send_to_char ("Syntax: wait wait_time\n\r", ch);

  return FALSE;
}



/** Function: skedit_msgs
  * Descr   : Edits the various messages for a skill/weave
  * Returns : TRUE/FALSE if successful
  * Syntax  : msgs msgs_slot new_msg
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_msgs)
{
  char *messages = "damage wearoff wearobj";
  SKILL_DATA *pSkill;
  char arg1[MIL];

  EDIT_SKILL (ch, pSkill);

  argument = one_argument (argument, arg1);

  if (arg1[0] != '\0' && argument[0] != '\0' && is_name (arg1, messages))
  {
    if (!str_prefix (arg1, "damage"))
    {
      if (pSkill->noun_damage)
	free_string (pSkill->noun_damage);
      pSkill->noun_damage = str_dup (argument);

      chprintf (ch, "New (damage) message of:\n\r %s set.\n\r",
		      pSkill->noun_damage);

      return TRUE;
    }

    if (!str_cmp (arg1, "wearoff"))
    {
      if (pSkill->msg_off)
	free_string (pSkill->msg_off);
      pSkill->msg_off = str_dup (argument);

      chprintf (ch, "New (wearoff) message of:\n\r %s set.\n\r",
		      pSkill->msg_off);

      return TRUE;
    }

    if (!str_cmp (arg1, "wearobj"))
    {
      if (pSkill->msg_obj)
	free_string (pSkill->msg_obj);
      pSkill->msg_obj = str_dup (argument);

      chprintf (ch, "New (wearobj) message of:\n\r %s set.\n\r",
		      pSkill->msg_obj);

      return TRUE;
    }

  }

  send_to_char ("Invalid syntax.\n\r", ch);
  send_to_char ("There are 3 messages that can be changed. These are:\n\r"
		"(damage)   On Success, to the user.\n\r"
		"(wearoff)  When the affect wears off, shown to the user.\n\r"
		"(wearobj)  When the affect wears off of an object, shown to room.\n\r",
		ch);

  return FALSE;
}



/** Function: skedit_show
  * Descr   : Displays weave information to screen.
  * Returns : TRUE/FALSE if valid weave
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_show)
{
  SKILL_DATA *pSkill;
  int i;

  EDIT_SKILL (ch, pSkill);

  chprintf (ch, "%s: %s\n\r",
		  pSkill->weave_fun ? "Weave" : "Skill", pSkill->name);

  send_to_char ("Class Levels    : ", ch);

  for (i = 0; i < MAX_CLASS; i++)
    chprintf (ch, "[%s: %2d] ",
		    class_table[i].name, pSkill->skill_level[i]);

  send_to_char ("\n\rClass Ratings   : ", ch);

  for (i = 0; i < MAX_CLASS; i++)
    chprintf (ch, "[%s: %2d] ", class_table[i].name, pSkill->rating);

  send_to_char ("\n\rTarget          : ", ch);
  switch (pSkill->target)
  {
  case TAR_INSANE:
    break;
  case TAR_IGNORE:
    send_to_char ("Ignored. (Skill Chooses)\n\r", ch);
    break;
  case TAR_CHAR_OFFENSIVE:
    send_to_char ("Offensive. (Anybody|Aggressive)\n\r", ch);
    break;
  case TAR_CHAR_DEFENSIVE:
    send_to_char ("Defensive. (Anybody|Non Aggressive)\n\r", ch);
    break;
  case TAR_CHAR_SELF:
    send_to_char ("User Only.\n\r", ch);
    break;
  case TAR_OBJ_INV:
    send_to_char ("Object in user's Inventory Only.\n\r", ch);
    break;
  case TAR_OBJ_CHAR_DEF:
    send_to_char ("Any Object (Non Aggressive).\n\r", ch);
    break;
  case TAR_OBJ_CHAR_OFF:
    send_to_char ("Any Object (Aggressive).\n\r", ch);
    break;
  }

  send_to_char ("Position        : ", ch);

  switch (pSkill->minimum_position)
  {
  case POS_DEAD:
    send_to_char ("Any.\n\r", ch);
    break;
  case POS_MORTAL:
    send_to_char ("While mortally wounded or better.\n\r", ch);
    break;
  case POS_INCAP:
    send_to_char ("While incappacitated or better.\n\r", ch);
    break;
  case POS_STUNNED:
    send_to_char ("While stunned or better.\n\r", ch);
    break;
  case POS_SLEEPING:
    send_to_char ("While Sleeping or better.\n\r", ch);
    break;
  case POS_RESTING:
    send_to_char ("While Resting or better.\n\r", ch);
    break;
  case POS_SITTING:
    send_to_char ("While Sitting or better.\n\r", ch);
    break;
  case POS_FIGHTING:
    send_to_char ("While Fighting or better.\n\r", ch);
    break;
  case POS_STANDING:
    send_to_char ("While Standing.\n\r", ch);
    break;
  default:
    break;
  }

  if (pSkill->weave_fun)
  chprintf (ch,
		  "\n\rMessages:\n\r"
		  "Damage          : %s\n\r"
		  "Wear Off        : %s\n\r"
		  "Wear Off (obj)  : %s\n\r",
		  pSkill->noun_damage, pSkill->msg_off, pSkill->msg_obj);

  return FALSE;
}




/** Function: skedit_save
  * Descr   : saves skill list to datafile (/data/skills.dat)
  * Returns : TRUE/FALSE if successful
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_save)
{
  if (save_skill_data ())
    send_to_char ("Skill database saved.\n\r", ch);
  else
    send_to_char ("Error saving skill database.\n\r", ch);

  return FALSE;
}



/** Function: skedit_flags
  * Descr   : Sets the various flags for a skill.
  * Returns : TRUE/FALSE if successful
  * Syntax  : flag flag_name
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_flags)
{
  SKILL_DATA *pSkill;
  long value = 0;
  int col = 0;

  if (argument[0] != '\0')
  {
    EDIT_SKILL (ch, pSkill);

    if ((value = flag_lookup (argument, skill_flags)) != NO_FLAG)
    {
      if (IS_FLAG (pSkill->flags, value))
	REMOVE_FLAG (pSkill->flags, value);
      else
	SET_FLAG (pSkill->flags, value);

      send_to_char ("Skill flag(s) toggled.\n\r", ch);
      return TRUE;
    }
  }

  send_to_char ("Syntax: flag skill_flag(s)\n\r Valid flags are:\n\r", ch);

  for (value = 0; skill_flags[value].name != NULL; value++)
    if (skill_flags[value].settable)
    {
      chprintf (ch, "%-20s%s",
		      skill_flags[value].name,
		      (++col % 3 == 0) ? "\n\r" : "");
    }

  send_to_char ("\n\r", ch);

  return FALSE;

}

/** Function: skedit_guild
  * Descr   : Sets the various guild flags for a skill.
  * Returns : TRUE/FALSE if successful
  * Syntax  : guild flag_name
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_guild)
{

  return FALSE;

}


/** Function: skedit_list
  * Descr   : displays the entire skill list to the screen
  * Returns : output
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKEDIT (skedit_list)
{
  BUFFER *buffer;
  char buf[MSL];
  int count = 0;

  buffer = new_buf ();

  for (count = 0; count < maxSkill; count++)
  {
    if (skill_table[count].name && skill_table[count].name[0] != '\0')
    {
      sprintf (buf, "[%-3d] %-15.15s %s",
	       (count + 1),
	       skill_table[count].name,
	       ((1 + count) % 3) == 0 ? "\n\r" : "\t");

      add_buf (buffer, buf);
    }
  }

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);

  return FALSE;
}
