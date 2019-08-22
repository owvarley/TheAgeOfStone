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
 * @@ Objective (interp.c)
 * 
 * This file contains primarily, the command interpreter. Although it's
 * starting to collect it's own mass of 'i dont know where else to put it'
 * functions.  do_commands should go to act_info, do_social to act_comm,
 * one_argument to string.c But if I keep moving things around as I update
 * these comments, I'll never finish...
 */


#if !defined(WIN32)
  #include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

char last_command[MIL];

/*
 * Log-all switch.
 */
bool fLogAll = FALSE;

char *parse_arguments(char *argument, char *arg_first, bool keepCaps)
{
  char cEnd = ' ';

  while (isspace (*argument))
    argument++;

  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }

    if ( !keepCaps )
      *arg_first = LOWER (*argument);
    else
      *arg_first = *argument;
      
    arg_first++;
    argument++;
  }
  *arg_first = '\0';

  while (isspace (*argument))
    argument++;

  return argument;

}

/*
 * JD - One_argument. Keeps capitals in the parsed argument
 */
char *One_argument (char *argument, char *arg_first)
{
  return parse_arguments(argument, arg_first, TRUE);
}

char *one_argument (char *argument, char *arg_first)
{
  return parse_arguments(argument, arg_first, FALSE);
}


/** The main entry point for executing commands.
 *  Can be recursively called from 'at', 'order', 'force'.
 */
void interpret (CHAR_DATA * ch, char *argument)
{
  char command[MIL];
  char logline[MIL];
  char buf[MIL];
  int cmd;
  int trust;
  bool found;

    /*
	 * Validate the argument
     */
  while (isspace (*argument))
    argument++;

  if (argument[0] == '\0')
    return;

  smash_tilde (argument);

  /*
   * No hiding.
   */
  REMOVE_FLAG (ch->states.affected_by, _AFF_HIDE);

  /*
   * Implement freeze command.
   */
  if (!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _FREEZE))
  {
    send_to_char ("You're totally frozen!\n\r", ch);
    return;
  }

  /* 
   * Grab the command word.
   * Special parsing so ' can be a command,
   * also no spaces needed after punctuation.
   */
  strcpy (logline, argument);

  /* Let's see who is doing what? -Ferric */
  strcpy (buf, argument);
  sprintf (last_command, "%s in room [%ld]: %s", ch->name,
	   ch->in_room->vnum, buf);

  if (!isalpha (argument[0]) && !isdigit (argument[0]))
  {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while (isspace (*argument))
      argument++;
  }
  else
  {
    argument = one_argument (argument, command);
  }

  /*
   * Look for command in command table.
   */
  found = FALSE;
  trust = get_trust (ch);

  for (cmd = 0; cmd != maxCommands; cmd++)
  {
    if (!IS_NULLSTR(cmd_table[cmd].name) 
	&& command[0] == cmd_table[cmd].name[0]
	&& !str_prefix (command, cmd_table[cmd].name)
	&& cmd_table[cmd].level <= trust)
    {
      found = TRUE;
      break;
    }
  }

  /*
   * Log and snoop.
   */
  if (cmd_table[cmd].log == LOG_NEVER)
    strcpy (logline, "");

  if ((!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _LOG))
      || fLogAll || cmd_table[cmd].log == LOG_ALWAYS)
  {
    char log_buf[MSL];

    sprintf (log_buf, "Log %s: %s", ch->name, logline);
    wiznet (log_buf, ch, NULL, WIZ_SECURE, 0, get_trust (ch));
    log_string (log_buf);
  }

  if (ch->desc != NULL && ch->desc->snoop_by != NULL)
  {
    write_to_buffer (ch->desc->snoop_by, "% ", 2);
    write_to_buffer (ch->desc->snoop_by, logline, 0);
    write_to_buffer (ch->desc->snoop_by, "\n\r", 2);
  }

  if (!found)
  {
	/*
	 * Look for command in socials table.
	 */
    if (!check_social (ch, command, argument))
      send_to_char ("Huh?\n\r", ch);
    return;
  }
  else /* a normal valid command.. check if it is disabled */
  if (IS_SET (cmd_table[cmd].flags, CMD_DISABLED))
  {
    send_to_char ("This command has been temporarily disabled.\n\r", ch);
    return;
  }

  /*
   * Character not in position for command?
   */
  if (ch->position < cmd_table[cmd].position)
  {
    switch (ch->position)
    {
		case POS_DEAD:
		  send_to_char ("Lie still; you are DEAD.\n\r", ch);
		  break;

		case POS_MORTAL:
		case POS_INCAP:
		  send_to_char ("You are hurt far too bad for that.\n\r", ch);
		  break;

		case POS_STUNNED:
		  send_to_char ("You are too stunned to do that.\n\r", ch);
		  break;

		case POS_SLEEPING:
		  send_to_char ("In your dreams, or what?\n\r", ch);
		  break;

		case POS_RESTING:
		  send_to_char ("Nah... You feel too relaxed...\n\r", ch);
		  break;

		case POS_SITTING:
		  send_to_char ("Better stand up first.\n\r", ch);
		  break;

		case POS_FIGHTING:
		  send_to_char ("No way!  You are still fighting!\n\r", ch);
		  break;

		default:
		  break;

    }
    return;

  }

  /*
   * Dispatch the command.
   */
  if ( cmd_table[cmd].do_fun != NULL )
    (*cmd_table[cmd].do_fun) (ch, argument);
  else
	  bugf("[interp.c::interpret] NULL function pointer for command '%s'!", 
	       cmd_table[cmd].name);

  tail_chain ();
  return;
}



bool check_social (CHAR_DATA * ch, char *command, char *argument)
{
  char arg[MIL];
  CHAR_DATA *victim;
  int cmd;
  bool found;

  found = FALSE;
  for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
  {
    if (command[0] == social_table[cmd].name[0]
	&& !str_prefix (command, social_table[cmd].name))
    {
      found = TRUE;
      break;
    }
  }

  if (!found)
    return FALSE;

  if (!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _NOEMOTE))
  {
    send_to_char ("You are anti-social!\n\r", ch);
    return TRUE;
  }

  switch (ch->position)
  {
  case POS_DEAD:
    send_to_char ("Lie still; you are DEAD.\n\r", ch);
    return TRUE;

  case POS_INCAP:
  case POS_MORTAL:
    send_to_char ("You are hurt far too bad for that.\n\r", ch);
    return TRUE;

  case POS_STUNNED:
    send_to_char ("You are too stunned to do that.\n\r", ch);
    return TRUE;

  case POS_SLEEPING:
    if (!str_cmp (social_table[cmd].name, "snore"))
      break;
    send_to_char ("In your dreams, or what?\n\r", ch);
    return TRUE;

  default:
    break;
  }

  one_argument (argument, arg);
  victim = NULL;
  if (arg[0] == '\0')
  {
    act (social_table[cmd].others_no_arg,
	 ch, NULL, victim, TO_ROOM, POS_RESTING);
    act (social_table[cmd].char_no_arg,
	 ch, NULL, victim, TO_CHAR, POS_RESTING);
  }
  else if ((victim = get_char_room (ch, arg)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
  }
  else if (victim == ch)
  {
    act (social_table[cmd].others_auto,
	 ch, NULL, victim, TO_ROOM, POS_RESTING);
    act (social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR, POS_RESTING);
  }
  else
  {
    act (social_table[cmd].others_found,
	 ch, NULL, victim, TO_NOTVICT, POS_RESTING);
    act (social_table[cmd].char_found,
	 ch, NULL, victim, TO_CHAR, POS_RESTING);
    act (social_table[cmd].vict_found,
	 ch, NULL, victim, TO_VICT, POS_RESTING);

    if (!IS_NPC (ch) && IS_NPC (victim)
	&& !IS_AFFECTED (victim, _AFF_COMPULSION)
	&& IS_AWAKE (victim) && victim->desc == NULL)
    {
      switch (number_bits (4))
      {
      case 0:

      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
	act (social_table[cmd].others_found,
	     victim, NULL, ch, TO_NOTVICT, POS_RESTING);
	act (social_table[cmd].char_found,
	     victim, NULL, ch, TO_CHAR, POS_RESTING);
	act (social_table[cmd].vict_found,
	     victim, NULL, ch, TO_VICT, POS_RESTING);
	break;

      case 9:
      case 10:
      case 11:
      case 12:
	act ("$n slaps $N.", victim, NULL, ch, TO_NOTVICT, POS_RESTING);
	act ("You slap $N.", victim, NULL, ch, TO_CHAR, POS_RESTING);
	act ("$n slaps you.", victim, NULL, ch, TO_VICT, POS_RESTING);
	break;
      }
    }
  }

  return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number (char *arg)
{

  if (*arg == '\0')
    return FALSE;

  if (*arg == '+' || *arg == '-')
    arg++;

  for (; *arg != '\0'; arg++)
  {
    if (!isdigit (*arg))
      return FALSE;
  }

  return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument (char *argument, char *arg)
{
  char *pdot;
  int number;

  for (pdot = argument; *pdot != '\0'; pdot++)
  {
    if (*pdot == '.')
    {
      *pdot = '\0';
      number = atoi (argument);
      *pdot = '.';
      strcpy (arg, pdot + 1);
      return number;
    }
  }

  strcpy (arg, argument);
  return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
 */
int mult_argument (char *argument, char *arg)
{
  char *pdot;
  int number;

  for (pdot = argument; *pdot != '\0'; pdot++)
  {
    if (*pdot == '*')
    {
      *pdot = '\0';
      number = atoi (argument);
      *pdot = '*';
      strcpy (arg, pdot + 1);
      return number;
    }
  }

  strcpy (arg, argument);
  return 1;
}



/*
 * Contributed by Alander.
 * Update by Gary McNickle
 */
_DOFUN (do_commands)
{
  BUFFER *buffer;
  char buf[MSL];
  int cmd;
  int col = 0;

  struct cmd_type *commands = NULL;

  buffer = new_buf ();

  commands = malloc ((sizeof (struct cmd_type) * maxCommands + 1));
  memset (commands, 0, sizeof (cmd_table[0]) * maxCommands + 1);

  commands =
    memcpy (commands, cmd_table, sizeof (struct cmd_type) * maxCommands + 1);

  qsort ((void *) commands, maxCommands, sizeof (cmd_table[0]), srt_cmds);

  for (cmd = 0; cmd != maxCommands; cmd++)
  {
    if (commands[cmd].level < LEVEL_HERO
	&& commands[cmd].level <= get_trust (ch))
    {
      sprintf (buf, "%-12s", commands[cmd].name);
      add_buf (buffer, buf);
      if (++col % 6 == 0)
	add_buf (buffer, "\n\r");
    }
  }

  if (col % 6 != 0)
    add_buf (buffer, "\n\r");

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);
  free (commands);

  return;
}

_DOFUN (do_wizhelp)
{
  BUFFER *buffer;
  char buf[MSL];
  int cmd;
  int col = 0;

  struct cmd_type *commands = NULL;

  buffer = new_buf ();

  commands = malloc ((sizeof (struct cmd_type) * maxCommands + 1));
  memset (commands, 0, sizeof (cmd_table[0]) * maxCommands + 1);

  commands =
    memcpy (commands, cmd_table, sizeof (struct cmd_type) * maxCommands + 1);

  qsort ((void *) commands, maxCommands, sizeof (cmd_table[0]), srt_cmds);

  for (cmd = 0; cmd != maxCommands; cmd++)
  {
    if (commands[cmd].level >= LEVEL_HERO && commands[cmd].level <= get_trust (ch))
    {
      sprintf (buf, "`w(`C%d`w)`B %-12s`w", commands[cmd].level, commands[cmd].name);
      add_buf (buffer, buf);
      if (++col % 4 == 0)
	    add_buf (buffer, "\n\r");
    }
  }

  if (col % 4 != 0)
    add_buf (buffer, "\n\r");

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);
  free (commands);
	
}


