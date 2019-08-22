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

#if !defined(_WIN32)
  #include <dlfcn.h>
#else
  #include <windows.h>
#endif

#include "merc.h"
#include "olc.h"

#define CEDIT( fun )            bool fun( CHAR_DATA *ch, char *argument )


char *cmd_flag_str (long flags)
{
  static char buf[MIL];

  memset (buf, 0, MIL);

  sprintf (buf, "%s%s%s%s%s",
	   (flags & CMD_CHANGED) ? "`CC`w" : "-",
	   (flags & CMD_DELETED) ? "`RD`w" : "-",
	   (flags & CMD_DISABLED) ? "`ND`w" : "-",
	   (flags & CMD_GUILD_ONLY) ? "`GG`w" : "-",
	   (flags & CMD_REMOVED) ? "`bR`w" : "-");

  return buf;
}

CEDIT (cedit_list)
{
  int cmd = 0, cnt = 0, len = 0;
  int min = -1, max = -1;
  char arg1[MIL], arg2[MIL], buf[MSL];
  BUFFER *buffer;
  struct cmd_type *commands = NULL;

  memset (arg1, 0, MIL);
  memset (arg2, 0, MIL);
  memset (buf, 0, MSL);

  commands = malloc ((sizeof (struct cmd_type) * maxCommands + 1));
  memset (commands, 0, sizeof (cmd_table[0]) * maxCommands + 1);

  commands =
    memcpy (commands, cmd_table, sizeof (struct cmd_type) * maxCommands + 1);

  qsort ((void *) commands, maxCommands, sizeof (cmd_table[0]), srt_cmds);

  buffer = new_buf ();

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if (arg1[0] != '\0')
    min = atoi (arg1);

  if (arg2[0] != '\0')
    max = atoi (arg2);

  for (cmd = 0; cmd != maxCommands; cmd++)
  {
    len = strlen (commands[cmd].name);

    if (min >= 0 && commands[cmd].level < min)
      continue;
    if (max >= 0 && commands[cmd].level > max)
      continue;
    if (min >= 0 && max < 0 && commands[cmd].level != min)
      continue;

    sprintf (buf, "[%s%2d`w] %s %s",
	     commands[cmd].level < LEVEL_HERO ? "`G" : "`B",
	     commands[cmd].level,
	     commands[cmd].name,
	     ((cnt - 1) % 3 == 1) ? "\n\r" : (len == 1) ? "\t\t\t" : (len <
								      10) ?
	     "\t\t" : "\t");
    ++cnt;
    add_buf (buffer, buf);
  }

  free (commands);

  sprintf (buf, "\n\rFound %d of %d commands.\n\r", cnt, maxCommands);

  add_buf (buffer, buf);
  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);

  return FALSE;
}

CEDIT (cedit_level)
{
  CMD_DATA *pCmd = NULL;
  int level = -1;

  EDIT_CMD (ch, pCmd);

  if (is_number (argument))
    level = atoi (argument);

  if (level < 0 || level > ML)
  {
    chprintf (ch,
		    "Syntax: level # Where '#' is a number between 0 and %d\n\r",
		    ML);
    return FALSE;
  }

  pCmd->level = level;
  return TRUE;
}


CEDIT (cedit_show)
{
  CMD_DATA *pCmd = NULL;

  EDIT_CMD (ch, pCmd);

  chprintf (ch, "\n\r[%s] Command : '%s'\n\r"
		  "Minimum Position: %s\n\r"
		  "Minimum Level   : %d\n\r\n\r"
		  "This command is %s logged.\n\r"
		  "Alternate Command name: %s\n\r\n\r",
		  cmd_flag_str (pCmd->flags), pCmd->name,
		  flag_string (position_flags, pCmd->position),
		  pCmd->level,
		  pCmd->log == LOG_NORMAL ? "not usually" :
		  pCmd->log == LOG_ALWAYS ? "always" :
		  pCmd->log == LOG_NEVER ? "never" : "(incorrect setting)",
		  (IS_NULLSTR (pCmd->lookup_name)) ? "(none)" : pCmd->
		  lookup_name);


  return FALSE;
}


CEDIT (cedit_position)
{
  CMD_DATA *pCmd = NULL;
  int i = 0;

  EDIT_CMD (ch, pCmd);

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
    pCmd->position = position_flags[i].bit;
    return TRUE;
  }

  return FALSE;
}


CEDIT (cedit_log)
{
  CMD_DATA *pCmd = NULL;
  char arg[MIL];
  char *log_types[] = { "always ", "never ", "normal", NULL };
  int i = 0, n = -1;


  EDIT_CMD (ch, pCmd);

  argument = one_argument (argument, arg);

  for (i = 0; log_types[i] != NULL; i++)
    if (!str_prefix (arg, log_types[i]))
      n = i;

  if (n < 0)
  {
    send_to_char ("Invalid log type. Valid types are: ", ch);
    for (i = 0; log_types[i] != NULL; i++)
      send_to_char (log_types[i], ch);
    send_to_char ("\n\r", ch);

    return FALSE;
  }

  switch (n)
  {
  case 0:
    pCmd->log = LOG_NORMAL;
    break;
  case 1:
    pCmd->log = LOG_ALWAYS;
    break;
  case 2:
    pCmd->log = LOG_NEVER;
    break;
  default:
    pCmd->log = LOG_NORMAL;
    break;
  }

  chprintf (ch, "Log type %d[%s] set.\n\r", n, log_types[n]);

  return TRUE;
}


CEDIT (cedit_disable)
{
  CMD_DATA *pCmd = NULL;

  EDIT_CMD (ch, pCmd);

  if (!IS_SET (pCmd->flags, CMD_DISABLED))
  {
    SET_BIT (pCmd->flags, CMD_DISABLED);
    send_to_char ("Command disabled.\n\r", ch);
    return TRUE;
  }
  else
  {
    send_to_char ("This command has allready been disabled.\n\r", ch);
    return FALSE;
  }

  return FALSE;
}


CEDIT (cedit_enable)
{
  CMD_DATA *pCmd = NULL;

  EDIT_CMD (ch, pCmd);

  if (IS_SET (pCmd->flags, CMD_DISABLED))
  {
    REMOVE_BIT (pCmd->flags, CMD_DISABLED);
    send_to_char ("Command enabled.\n\r", ch);
    return TRUE;
  }
  else
  {
    send_to_char ("This command has not been disabled.\n\r", ch);
    return FALSE;
  }

  return FALSE;
}


CEDIT (cedit_name)
{
  char arg[MIL];
  CMD_DATA *pCmd = NULL;

  EDIT_CMD (ch, pCmd);

  memset (arg, MIL, 0);
  argument = one_argument (argument, arg);

  if (!IS_NULLSTR (arg))
  {
    char buf[MIL];
    int (*fPtr) (int);

	#if !defined(WIN32)
      void *handle;

	  handle = dlopen (NULL, RTLD_LAZY);
	#else
	  HMODULE handle;

	  handle = LoadLibrary(APPNAME);
	#endif

    if (!handle)
    {
      send_to_char ("Error accessing symbol table. Aborting.\n\r", ch);
      return FALSE;
    }

    memset (buf, 0, MIL);

    sprintf (buf, "do_%s", arg);

	#if !defined(WIN32)
	  fPtr = (int (*)(int)) dlsym (handle, buf);
	#else
	  fPtr = (int (*)(int)) GetProcAddress(handle, buf);
	#endif

	#if !defined(WIN32)
	  dlclose (handle);
	#else
	  FreeLibrary(handle);
	#endif

    if (!fPtr)
    {
      send_to_char ("That command does not exist in the symbol table.\n\r"
		    "Use alternate instead.\n\r", ch);
      return FALSE;
    }

    if (!IS_NULLSTR (pCmd->name))
      free_string (pCmd->name);

    pCmd->name = str_dup (arg);
    chprintf (ch, "Name set to [%s]\n\r", pCmd->name);
    return TRUE;
  }

  return FALSE;
}


CEDIT (cedit_lookup)
{
  char arg[MIL];
  CMD_DATA *pCmd = NULL;

  EDIT_CMD (ch, pCmd);

  memset (arg, MIL, 0);
  argument = one_argument (argument, arg);

  if (!IS_NULLSTR (arg))
  {
	#if defined(_WIN32)
  	  HMODULE handle;
	#else
	  void *handle;
	#endif
	  int (*fPtr) (int);

    if (!str_cmp (arg, "delete"))
    {
      if (pCmd->lookup_name)
		free_string (pCmd->lookup_name);

      pCmd->lookup_name = str_dup ("");
      send_to_char ("Alternate cleared.\n\r", ch);

      return TRUE;
    }

	#if !defined(WIN32)
	  handle = dlopen (NULL, RTLD_LAZY);
	#else
	  handle = LoadLibrary(APPNAME);
	#endif

    if (!handle)
    {
      send_to_char ("Error accessing symbol table. Aborting.\n\r", ch);
      return FALSE;
    }

	#if !defined(WIN32)
	  fPtr = (int (*)(int)) dlsym (handle, arg);
	#else
	  fPtr = (int (*)(int)) GetProcAddress(handle, arg);
	#endif

    if (!fPtr)
    {
      send_to_char ("That command does not exist in the symbol table.\n\r",
		    ch);
      return FALSE;
    }

    if (!IS_NULLSTR (pCmd->lookup_name))
      free_string (pCmd->lookup_name);

    pCmd->lookup_name = str_dup (arg);
    chprintf (ch, "Alternate set to [%s]\n\r", pCmd->lookup_name);
    return TRUE;
  }

  return FALSE;
}


CEDIT (cedit_save)
{
  bool saved = save_commands ();

  if (saved)
    send_to_char ("Command table saved.\n\r", ch);
  else
    send_to_char ("Error. Unable to save the command table.\n\r", ch);

  return FALSE;
}

CEDIT (cedit_reload)
{
  bool success = load_commands ();

  if (!success)
    send_to_char ("Unable to load command table.\n\r", ch);
  else
    send_to_char ("Command table reloaded and reset.\n\r", ch);

  return FALSE;
}
