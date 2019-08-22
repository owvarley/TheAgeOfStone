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
 * @@ Objective ()
 * 
 */

#include <sys/types.h>
//#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"
#include "olc.h"
#include "recycle.h"

#define HEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_HELP(ch, help)	( help = (HELP_DATA *) ch->desc->pEdit )


const struct olc_cmd_type hedit_table[] = {
/*	{	command		function	}, */

  {"keyword", hedit_keyword},
  {"text", hedit_text},
  {"new", hedit_new},
  {"level", hedit_level},
  {"commands", show_commands},
  {"delete", hedit_delete},
  {"list", hedit_list},
  {"show", hedit_show},
  {"?", show_help},

  {NULL, 0}
};

HEDIT (hedit_show)
{
  HELP_DATA *help;
  char buf[MSL * 2];

  EDIT_HELP (ch, help);

  sprintf (buf, "Keyword : [%s]\n\r"
	   "Level   : [%d]\n\r"
	   "Text    :\n\r"
	   "%s-End-\n\r", help->keyword, help->level, help->text);
  send_to_char (buf, ch);

  return FALSE;
}

HEDIT (hedit_level)
{
  HELP_DATA *help;
  int lev;

  EDIT_HELP (ch, help);

  if (IS_NULLSTR (argument) || !is_number (argument))
  {
    chprintf (ch, "Syntax: Level [-1..%d]\n\r", MAX_LEVEL);
    return FALSE;
  }

  lev = atoi (argument);

  if (lev < -1 || lev > MAX_LEVEL)
  {
    chprintf (ch, "HEdit : Level range is -1 to %d.\n\r", MAX_LEVEL);
    return FALSE;
  }

  help->level = lev;
  send_to_char ("Ok.\n\r", ch);
  return TRUE;
}

HEDIT (hedit_keyword)
{
  HELP_DATA *help;

  EDIT_HELP (ch, help);

  if (IS_NULLSTR (argument))
  {
    send_to_char ("Syntax : keyword [keywords]\n\r", ch);
    return FALSE;
  }

  free_string (help->keyword);
  help->keyword = str_dup (argument);

  send_to_char ("Ok.\n\r", ch);
  return TRUE;
}

HEDIT (hedit_new)
{
  char arg[MIL], fullarg[MIL];
  HELP_DATA *help;
  extern HELP_DATA *help_last;

  if (IS_NULLSTR (argument))
  {
    send_to_char ("Syntax : new [keyword]\n\r", ch);
    return FALSE;
  }

  strcpy (fullarg, argument);
  argument = one_argument (argument, arg);

  if (help_lookup (argument))
  {
    send_to_char ("HEdit : That help already exists.\n\r", ch);
    return FALSE;
  }

  help = new_help ();
  help->level = 0;
  help->keyword = str_dup (argument);
  help->text = str_dup ("");

  if (help_last)
    help_last->next = help;

  if (help_first == NULL)
    help_first = help;

  help_last = help;
  help->next = NULL;

  help->next_area = NULL;

  ch->desc->pEdit = (HELP_DATA *) help;
  ch->desc->editor = ED_HELP;

  send_to_char ("Ok.\n\r", ch);
  return FALSE;
}

HEDIT (hedit_text)
{
  HELP_DATA *help;

  EDIT_HELP (ch, help);

  if (!IS_NULLSTR (argument))
  {
    send_to_char ("Syntax : text\n\r", ch);
    return FALSE;
  }

  string_append (ch, &help->text);

  return TRUE;
}

void hedit (CHAR_DATA * ch, char *argument)
{
  HELP_DATA *pHelp;
  char arg[MIL];
  char command[MIL];
  int cmd;

  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  EDIT_HELP (ch, pHelp);

  if (ch->pcdata->security < 8)
  {
    send_to_char ("HEdit: Insuffecient security.\n\r", ch);
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    hedit_show (ch, argument);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  for (cmd = 0; hedit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, hedit_table[cmd].name))
    {
      if ((*hedit_table[cmd].olc_fun) (ch, argument))
      {
	HELP_DATA *pHelp;

	EDIT_HELP (ch, pHelp);
      }

      return;
    }
  }

  interpret (ch, arg);
  return;
}

_DOFUN (do_hedit)
{
  HELP_DATA *pHelp = NULL;

  if (IS_NPC (ch))
    return;

  if (argument[0] == '\0')
    pHelp = help_first;


  if (pHelp == NULL && (pHelp = help_lookup (argument)) == NULL)
  {
    send_to_char ("HEdit : That help does not exist.\n\r", ch);
    return;
  }

  ch->desc->pEdit = (void *) pHelp;
  ch->desc->editor = ED_HELP;

  return;
}

HEDIT (hedit_delete)
{
  HELP_DATA *pHelp, *temp;
  DESCRIPTOR_DATA *d;

  EDIT_HELP (ch, pHelp);

  for (d = descriptor_list; d; d = d->next)
    if (d->editor == ED_HELP && pHelp == (HELP_DATA *) d->pEdit)
      edit_done (d->character);

  if (help_first == pHelp)
    help_first = help_first->next;
  else
  {
    for (temp = help_first; temp; temp = temp->next)
      if (temp->next == pHelp)
	break;

    if (!temp)
    {
      bugf ("[olc_help.c::hedit_delete] help %s no encontrado en help_first",
	    pHelp->keyword);
      return FALSE;
    }

    temp->next = pHelp->next;
  }


  free_help (pHelp);

  send_to_char ("Ok.\n\r", ch);
  return TRUE;
}

HEDIT (hedit_list)
{
  char buf[MIL];
  int cnt = 0;
  HELP_DATA *pHelp;
  BUFFER *buffer;

  EDIT_HELP (ch, pHelp);

  buffer = new_buf ();

  for (pHelp = help_first; pHelp; pHelp = pHelp->next)
  {
    sprintf (buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
	     cnt % 4 == 3 ? "\n\r" : " ");
    add_buf (buffer, buf);
    cnt++;
  }

  if (cnt % 4)
    add_buf (buffer, "\n\r");

  page_to_char (buf_string (buffer), ch);
  free_buf (buffer);

  return FALSE;
}
