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


/***************************************************************************
 *  File: string.c                                                         *
 *              							   *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"


/** Function: versionToString
  * Descr   : Converts a version number, 
  *         : into primary.release.secondary.build version string
  * Returns : char *
  * Syntax  : versionToString( version # )
  * Written : v1.0 10/98
  * Author  : Erwin Andreasen (erwin@pip.dknet.dk)
  */
const char *versionToString (int version)
{
  static char buf[64];
  sprintf (buf, "%d.%02d.%02d",
	   version / 10000,
	   (version - ((10000 * (version / 10000)))) / 100, version % 100);

  return buf;
}


bool is_allowed (int c)
{
  if (c == '\'' || c == ' ' || isalpha (c))
    return TRUE;
  else
    return FALSE;
}

char *fix_name (char *name, bool restore)
{
	char buf[MIL];
	int i = 0;
	
	memset (buf, 0, MIL);
	memcpy (buf, name, strlen (name));
	
	if (restore)
	{
		for (i = 0; buf[i] != '\0'; i++)
			switch (buf[i])
		{
		  case '_':
			  buf[i] = '\'';
			  break;
		  case '-':
			  buf[i] = ' ';
			  break;
		  default:
			  break;
		}
	}
	else
		
	{
		for (i = 0; buf[i] != '\0'; i++)
			switch (buf[i])
		{
		  case '\'':
			  buf[i] = '_';
			  break;
		  case ' ':
			  buf[i] = '-';
			  break;
		  default:
			  break;
		}
	}
	
	return str_dup (buf);
	
}


char *pad (char *str, int count, char dir)
{
  char buf[MSL];
  char buf2[MSL];

  int i = 0;
  int size = 0;

  buf[0] = '\0';
  memset (buf2, 0, MSL);

  if (count <= 0 || count > MSL - 1)
    return "";

  if (str == NULL)
  {
    memset (buf2, ' ', count);
    return str_dup (buf2);
  }
  else
  {
    if (strlen (str) - (count_codes (str) * 2) > count)
      size = 0;
    else
      size = count - (strlen (str) - (count_codes (str) * 2));
  }

  switch (UPPER (dir))
  {
  case 'L':
  case 'B':			/* left/beginning */
    memset (buf2, ' ', size);
    strncat (buf2, str, count - size + (count_codes (str) * 2));
    break;

  case 'R':
  case 'E':			/* right/ending */
    {
      memcpy (buf2, str, strlen (str));
      if (strlen (str) - (count_codes (str) * 2) > count)
	buf2[count + 1] = '\0';
      else
	for (i = strlen (str) - (count_codes (str) * 2) + 1; i < count; i++)
	  buf2[i] = ' ';
      break;
    }
  }

  return str_dup (buf2);
}

/** Function: strip_codes
  * Descr   : Strip's the mud ANSI codes from a string (ie: `w) 
  * Returns : parsed string
  * Syntax  : strip_codes ( string to parse )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *strip_codes (char *str)
{
  int i = 0;
  char buf[MSL];

  memset (buf, 0, MSL);

  if (!str || *str == '\0')
    return str;

  while (*str != '\0')
  {
    if (*str == ANSI_KEY)
    {
      str++;
      str++;
    }
    else
    {
      buf[i] = *str;
      i++;
      str++;
    }
  }

  return str_dup (buf);
}


/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit (CHAR_DATA * ch, char **pString)
{
  send_to_char ("-========- Entering EDIT Mode -=========-\n\r", ch);
  send_to_char ("    Type .h on a new line for help\n\r", ch);
  send_to_char (" Terminate with a ~ or @ on a blank line.\n\r", ch);
  send_to_char ("-=======================================-\n\r", ch);

  if (*pString == NULL)
  {
    *pString = str_dup ("");
  }
  else
  {
    **pString = '\0';
  }

  ch->desc->pString = pString;

  return;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append (CHAR_DATA * ch, char **pString)
{
  send_to_char ("-=======- Entering APPEND Mode -========-\n\r", ch);
  send_to_char ("    Type .h on a new line for help\n\r", ch);
  send_to_char (" Terminate with a ~ or @ on a blank line.\n\r", ch);
  send_to_char ("-=======================================-\n\r", ch);

  if (*pString == NULL)
  {
    *pString = str_dup ("");
  }
  send_to_char (nolineas (*pString), ch);

  if (IS_FLAG (ch->states.config, _MUDFTP))
  {
    ch->desc->pString = pString;
    if (ftp_push (ch->desc))	/* ftp: PUSH mode */
      send_to_char
	("Editing string via mudFTP push connection.  Use ~ or @ to abort.\n",
	 ch);
    else			/* PULL mode */
    {
      char buf[MSL];
      sprintf (buf,
	       "Sending mudFTP request. If your client does not support mudFTP, abort this\n"
	       "edit (type ~ or @ on a blank line), toggle mudftp off, and try again.\n"
	       "\ntmp/%lu%c\n", (unsigned long) pString, 230);
      send_to_char (buf, ch);
    }
  }
  else
  {
    send_to_char (numlineas (*pString), ch);
    if (*(*pString + strlen (*pString) - 1) != '\r')
      send_to_char ("\n\r", ch);

    ch->desc->pString = pString;
  }

  return;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char *string_replace (char *orig, char *old, char *s_new)
{
  char xbuf[MSL];
  int i;

  xbuf[0] = '\0';
  strcpy (xbuf, orig);
  if (strstr (orig, old) != NULL)
  {
    i = strlen (orig) - strlen (strstr (orig, old));
    xbuf[i] = '\0';
    strcat (xbuf, s_new);
    strcat (xbuf, &orig[i + strlen (old)]);
    free_string (orig);
  }

  return str_dup (xbuf);
}



/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add (CHAR_DATA * ch, char *argument)
{
  char buf[MSL];

  /*
   * Thanks to James Seng
   */
  smash_tilde (argument);


  if (IS_FLAG (ch->states.config, _MUDFTP) && str_cmp (argument, "@"))
  {
    send_to_char ("Type @  to manually abort FTP mode.\n\r"
		  "If mudFTP is not supported by your client, abort this"
		  " edit and toggle mudftp off.\n\r", ch);
    return;
  }

  if (*argument == '.')
  {
    char arg1[MIL];
    char arg2[MIL];
    char arg3[MIL];
    char tmparg3[MIL];

    argument = one_argument (argument, arg1);

    if (!str_cmp (arg1, "./") || !str_cmp (arg1, "//"))
    {
      interpret (ch, argument);
      send_to_char ("Command performed.\n\r", ch);
      return;
    }

    argument = first_arg (argument, arg2, FALSE);
    strcpy (tmparg3, argument);
    argument = first_arg (argument, arg3, FALSE);

    if (!str_cmp (arg1, ".c") || !str_cmp (arg1, "/c"))
    {
      send_to_char ("String cleared.\n\r", ch);
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup ("");
      return;
    }

    if (!str_cmp (arg1, ".s"))
    {
      send_to_char ("String so far:\n\r", ch);
      send_to_char (nolineas (*ch->desc->pString), ch);
      return;
    }

    if (!str_cmp (arg1, ".sl") || !str_cmp (arg1, "/s"))
    {
      send_to_char ("String so far:\n\r", ch);
      send_to_char (numlineas (*ch->desc->pString), ch);
      return;
    }

    if (!str_cmp (arg1, ".r") || !str_cmp (arg1, "/r"))
    {
      if (arg2[0] == '\0')
      {
	send_to_char ("usage:  .r \"old string\" \"new string\"\n\r", ch);
	return;
      }

      *ch->desc->pString = string_replace (*ch->desc->pString, arg2, arg3);
      sprintf (buf, "'%s' replaced with '%s'.\n\r", arg2, arg3);
      send_to_char (buf, ch);
      return;
    }
    if (!str_cmp (arg1, ".-"))
    {
      *ch->desc->pString = del_last_line (*ch->desc->pString);
      send_to_char ("Line removed.\n\r", ch);
      return;
    }

    if (!str_cmp (arg1, ".f") || !str_cmp (arg1, "/f"))
    {
      *ch->desc->pString = format_string (*ch->desc->pString);
      send_to_char ("String formatted.\n\r", ch);
      return;
    }

    if (!str_cmp (arg1, ".ld") || !str_cmp (arg1, "/ld"))
    {
      *ch->desc->pString = string_linedel (*ch->desc->pString, atoi (arg2));
      send_to_char ("Linea borrada.\n\r", ch);
      return;
    }

    if (!str_cmp (arg1, ".li") || !str_cmp (arg1, "/li"))
    {
      *ch->desc->pString =
	string_lineadd (*ch->desc->pString, tmparg3, atoi (arg2));
      send_to_char ("Line inserted.\n\r", ch);
      return;
    }

    if (!str_cmp (arg1, ".lr") || !str_cmp (arg1, "/lr"))
    {
      *ch->desc->pString = string_linedel (*ch->desc->pString, atoi (arg2));
      *ch->desc->pString =
	string_lineadd (*ch->desc->pString, tmparg3, atoi (arg2));
      send_to_char ("Line replaced.\n\r", ch);
      return;
    }

    if (!str_cmp (arg1, ".h") || !str_cmp (arg1, "/h"))
    {
      send_to_char ("Sedit help (commands on blank line):   \n\r", ch);
      send_to_char (".-               	      - delete last line \n\r", ch);
      send_to_char (".r or /r  'old' 'new'   - replace a substring \n\r", ch);
      send_to_char ("                          (requires '', \"\") \n\r", ch);
      send_to_char (".h or /h                - get help (this info)\n\r", ch);
      send_to_char (".s or /s                - show string so far  \n\r", ch);
      send_to_char (".f or /f                - (word wrap) string  \n\r", ch);
      send_to_char (".c or /c                - clear string so far \n\r", ch);
      send_to_char (".ld or /ld <num>        - delete line number <num>\n\r",
		    ch);
      send_to_char
	(".li or /li <num> <str>  - insert <str> at line <num>\n\r", ch);
      send_to_char
	(".lr or /lr <num> <str>  - replace line <num> with <str>\n\r", ch);
      send_to_char ("./ or //   <command>    - execute command\n\r", ch);
      send_to_char ("@                       - end string\n\r", ch);
      return;
    }

    send_to_char ("SEdit:  Invalid dot command.\n\r", ch);
    return;
  }

  if (*argument == '~' || *argument == '@')
  {
    if (ch->desc->editor == ED_MPCODE)	/* para los mobprogs */
    {
      MOB_INDEX_DATA *mob;
      int hash;
      MPROG_LIST *mpl;
      MPROG_CODE *mpc;

      EDIT_MPCODE (ch, mpc);

      if (mpc != NULL)
	for (hash = 0; hash < MAX_KEY_HASH; hash++)
	  for (mob = mob_index_hash[hash]; mob; mob = mob->next)
	    for (mpl = mob->mprogs; mpl; mpl = mpl->next)
	      if (mpl->vnum == mpc->vnum)
	      {
		sprintf (buf, "mob vnum %ld.\n\r", mob->vnum);
		send_to_char (buf, ch);
		mpl->code = mpc->code;
	      }
    }

    ch->desc->pString = NULL;

    if (ch->desc->connected == CON_NOTE_TEXT)
    {
      handle_con_note_text (ch->desc, "");
    }

    return;
  }

  strcpy (buf, *ch->desc->pString);

  /*
   * Truncate strings to MSL.
   * --------------------------------------
   */
  if (strlen (buf) + strlen (argument) >= (MSL - 4))
  {
    send_to_char ("String too long, last line skipped.\n\r", ch);

    /* Force character out of editing mode. */
    ch->desc->pString = NULL;
    return;
  }

  /*
   * Ensure no tilde's inside string.
   * --------------------------------
   */
  smash_tilde (argument);

  strcat (buf, argument);
  strcat (buf, "\n\r");
  free_string (*ch->desc->pString);
  *ch->desc->pString = str_dup (buf);

  if (IS_FLAG (ch->states.config, _ECHO))
    write_to_descriptor (ch->desc, buf, strlen (buf));

  return;
}




/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 * Modifications for color codes and blank lines by Geoff.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string (char *oldstring /*, bool fSpace */ )
{
  char xbuf[MSL];
  char xbuf2[MSL];
  char *rdesc;
  int i = 0;
  int end_of_line;
  bool cap = TRUE;
  bool bFormat = TRUE;

  xbuf[0] = xbuf2[0] = 0;

  for (rdesc = oldstring; *rdesc; rdesc++)
  {

    if (*rdesc != '`')
    {
      if (bFormat)
      {
	if (*rdesc == '\n')
	{
	  if (*(rdesc + 1) == '\r'
	      && *(rdesc + 2) == ' '
	      && *(rdesc + 3) == '\n' && xbuf[i - 1] != '\r')
	  {
	    xbuf[i] = '\n';
	    xbuf[i + 1] = '\r';
	    xbuf[i + 2] = '\n';
	    xbuf[i + 3] = '\r';
	    i += 4;
	    rdesc += 2;
	  }
	  else
	    if (*(rdesc + 1) == '\r'
		&& *(rdesc + 2) == ' '
		&& *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r')
	  {
	    xbuf[i] = '\n';
	    xbuf[i + 1] = '\r';
	    i += 2;
	  }
	  else
	    if (*(rdesc + 1) == '\r'
		&& *(rdesc + 2) == '\n' && xbuf[i - 1] != '\r')
	  {
	    xbuf[i] = '\n';
	    xbuf[i + 1] = '\r';
	    xbuf[i + 2] = '\n';
	    xbuf[i + 3] = '\r';
	    i += 4;
	    rdesc += 1;
	  }
	  else
	    if (*(rdesc + 1) == '\r'
		&& *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r')
	  {
	    xbuf[i] = '\n';
	    xbuf[i + 1] = '\r';
	    i += 2;
	  }
	  else if (xbuf[i - 1] != ' ' && xbuf[i - 1] != '\r')
	  {
	    xbuf[i] = ' ';
	    i++;
	  }
	}
	else if (*rdesc == '\r');
	else
	  if (*rdesc == 'i'
	      && *(rdesc + 1) == '.'
	      && *(rdesc + 2) == 'e' && *(rdesc + 3) == '.')
	{
	  xbuf[i] = 'i';
	  xbuf[i + 1] = '.';
	  xbuf[i + 2] = 'e';
	  xbuf[i + 3] = '.';
	  i += 4;
	  rdesc += 3;
	}
	else if (*rdesc == ' ')
	{
	  if (xbuf[i - 1] != ' ')
	  {
	    xbuf[i] = ' ';
	    i++;
	  }
	}
	else if (*rdesc == ')')
	{
	  if (xbuf[i - 1] == ' '
	      && xbuf[i - 2] == ' '
	      && (xbuf[i - 3] == '.'
		  || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
	  {
	    xbuf[i - 2] = *rdesc;
	    xbuf[i - 1] = ' ';
	    xbuf[i] = ' ';
	    i++;
	  }
	  else
	    if (xbuf[i - 1] == ' '
		&& (xbuf[i - 2] == ',' || xbuf[i - 2] == ';'))
	  {
	    xbuf[i - 1] = *rdesc;
	    xbuf[i] = ' ';
	    i++;
	  }
	  else
	  {
	    xbuf[i] = *rdesc;
	    i++;
	  }
	}
	else if (*rdesc == ',' || *rdesc == ';')
	{
	  if (xbuf[i - 1] == ' ')
	  {
	    xbuf[i - 1] = *rdesc;
	    xbuf[i] = ' ';
	    i++;
	  }
	  else
	  {
	    xbuf[i] = *rdesc;
	    if (*(rdesc + 1) != '\"')
	    {
	      xbuf[i + 1] = ' ';
	      i += 2;
	    }
	    else
	    {
	      xbuf[i + 1] = '\"';
	      xbuf[i + 2] = ' ';
	      i += 3;
	      rdesc++;
	    }
	  }

	}
	else if (*rdesc == '.' || *rdesc == '?' || *rdesc == '!')
	{
	  if (xbuf[i - 1] == ' '
	      && xbuf[i - 2] == ' '
	      && (xbuf[i - 3] == '.'
		  || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
	  {
	    xbuf[i - 2] = *rdesc;
	    if (*(rdesc + 1) != '\"')
	    {
	      xbuf[i - 1] = ' ';
	      xbuf[i] = ' ';
	      i++;
	    }
	    else
	    {
	      xbuf[i - 1] = '\"';
	      xbuf[i] = ' ';
	      xbuf[i + 1] = ' ';
	      i += 2;
	      rdesc++;
	    }
	  }
	  else
	  {
	    xbuf[i] = *rdesc;
	    if (*(rdesc + 1) != '\"')
	    {
	      xbuf[i + 1] = ' ';
	      xbuf[i + 2] = ' ';
	      i += 3;
	    }
	    else
	    {
	      xbuf[i + 1] = '\"';
	      xbuf[i + 2] = ' ';
	      xbuf[i + 3] = ' ';
	      i += 4;
	      rdesc++;
	    }
	  }
	  cap = TRUE;
	}
	else
	{
	  xbuf[i] = *rdesc;
	  if (cap)
	  {
	    cap = FALSE;
	    xbuf[i] = UPPER (xbuf[i]);
	  }
	  i++;
	}
      }
      else
      {
	xbuf[i] = *rdesc;
	i++;
      }
    }
    else
    {
      if (*(rdesc + 1) == 'Z')
	bFormat = !bFormat;
      xbuf[i] = *rdesc;
      i++;
      rdesc++;
      xbuf[i] = *rdesc;
      i++;
    }
  }
  xbuf[i] = 0;
  strcpy (xbuf2, xbuf);

  rdesc = xbuf2;

  xbuf[0] = 0;

  for (;;)
  {
    end_of_line = 77;
    for (i = 0; i < end_of_line; i++)
    {
      if (*(rdesc + i) == '`')
      {
	end_of_line += 2;
	i++;
      }

      if (!*(rdesc + i))
	break;

      if (*(rdesc + i) == '\r')
	end_of_line = i;
    }
    if (i < end_of_line)
    {
      break;
    }
    if (*(rdesc + i - 1) != '\r')
    {
      for (i = (xbuf[0] ? (end_of_line - 1) : (end_of_line - 4)); i; i--)
      {
	if (*(rdesc + i) == ' ')
	  break;
      }
      if (i)
      {
	*(rdesc + i) = 0;
	strcat (xbuf, rdesc);
	strcat (xbuf, "\n\r");
	rdesc += i + 1;
	while (*rdesc == ' ')
	  rdesc++;
      }
      else
      {
	bugf ("[string.c::format_string] No spaces");
	*(rdesc + (end_of_line - 2)) = 0;
	strcat (xbuf, rdesc);
	strcat (xbuf, "-\n\r");
	rdesc += end_of_line - 1;
      }
    }
    else
    {
      *(rdesc + i - 1) = 0;
      strcat (xbuf, rdesc);
      strcat (xbuf, "\r");
      rdesc += i;
      while (*rdesc == ' ')
	rdesc++;
    }
  }
  while (*(rdesc + i)
	 && (*(rdesc + i) == ' '
	     || *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
    i--;

  *(rdesc + i + 1) = 0;
  strcat (xbuf, rdesc);

  if (xbuf[strlen (xbuf) - 2] != '\n')
    strcat (xbuf, "\n\r");

  free_string (oldstring);
  return (str_dup (xbuf));
}




/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg (char *argument, char *arg_first, bool fCase)
{
  char cEnd;

  while (*argument == ' ')
    argument++;

  cEnd = ' ';

  if (*argument == '\''
      || *argument == '"' || *argument == '%' || *argument == '(')
  {
    if (*argument == '(')
    {
      cEnd = ')';
      argument++;
    }
    else
      cEnd = *argument++;
  }

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }

    if (fCase)
      *arg_first = LOWER (*argument);
    else
      *arg_first = *argument;

    arg_first++;
    argument++;
  }

  *arg_first = '\0';

  while (*argument == ' ')
    argument++;

  return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char *string_unpad (char *argument)
{
  char buf[MSL];
  char *s;

  s = argument;

  while (*s == ' ')
    s++;

  strcpy (buf, s);
  s = buf;

  if (*s != '\0')
  {
    while (*s != '\0')
      s++;
    s--;

    while (*s == ' ')
      s--;
    s++;
    *s = '\0';
  }

  free_string (argument);
  return str_dup (buf);
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char *string_proper (char *argument)
{
  char *s;

  s = argument;

  while (*s != '\0')
  {
    if (*s != ' ')
    {
      *s = UPPER (*s);
      while (*s != ' ' && *s != '\0')
	s++;
    }
    else
    {
      s++;
    }
  }

  return argument;
}

char *string_linedel (char *string, int line)
{
  char *strtmp = string;
  char buf[MSL];
  int cnt = 1, tmp = 0;

  buf[0] = '\0';

  for (; *strtmp != '\0'; strtmp++)
  {
    if (cnt != line)
      buf[tmp++] = *strtmp;

    if (*strtmp == '\n')
    {
      if (*(strtmp + 1) == '\r')
      {
	if (cnt != line)
	  buf[tmp++] = *(++strtmp);
	else
	  ++strtmp;
      }

      cnt++;
    }
  }

  buf[tmp] = '\0';

  free_string (string);
  return str_dup (buf);
}

char *string_lineadd (char *string, char *newstr, int line)
{
  char *strtmp = string;
  int cnt = 1, tmp = 0;
  bool done = FALSE;
  char buf[MSL];

  buf[0] = '\0';

  for (; *strtmp != '\0' || (!done && cnt == line); strtmp++)
  {
    if (cnt == line && !done)
    {
      strcat (buf, newstr);
      strcat (buf, "\n\r");
      tmp += strlen (newstr) + 2;
      cnt++;
      done = TRUE;
    }

    buf[tmp++] = *strtmp;

    if (done && *strtmp == '\0')
      break;

    if (*strtmp == '\n')
    {
      if (*(strtmp + 1) == '\r')
	buf[tmp++] = *(++strtmp);

      cnt++;
    }

    buf[tmp] = '\0';
  }

  free_string (string);
  return str_dup (buf);
}

/* buf queda con la linea sin \n\r */
char *getline (char *str, char *buf)
{
  int tmp = 0;
  bool found = FALSE;

  while (*str)
  {
    if (*str == '\n')
    {
      found = TRUE;
      break;
    }

    buf[tmp++] = *(str++);
  }

  if (found)
  {
    if (*(str + 1) == '\r')
      str += 2;
    else
      str += 1;
  }				/* para que quedemos en el inicio de la prox linea */

  buf[tmp] = '\0';

  return str;
}

char *numlineas (char *string)
{
  int cnt = 1;
  static char buf[MSL * 2];
  char buf2[MSL], tmpb[MSL];

  buf[0] = '\0';

  while (*string)
  {
    string = getline (string, tmpb);
    sprintf (buf2, "%2d. %s\n\r", cnt++, tmpb);
    strcat (buf, buf2);
  }

  return buf;
}

/*****************************
 Name:      del_last_line
 Purpose:   Removes last line from string
 Called by: many.
 ******************/
char *del_last_line (char *string)
{
  int len;
  bool found = FALSE;

  char xbuf[MSL];

  xbuf[0] = '\0';
  if (string == NULL || string[0] == '\0')
    return (str_dup (xbuf));

  strcpy (xbuf, string);

  for (len = strlen (xbuf); len > 0; len--)
  {
    if (xbuf[len] == '\r')
    {
      if (!found)		/* back it up */
      {
	if (len > 0)
	  len--;
	found = TRUE;
      }
      else			/* found second one */
      {
	xbuf[len + 1] = '\0';
	free_string (string);
	return (str_dup (xbuf));
      }
    }
  }
  xbuf[0] = '\0';
  free_string (string);
  return (str_dup (xbuf));
}

char *nolineas (char *string)
{
  static char buf[MSL * 2];
  char buf2[MSL], tmpb[MSL];

  buf[0] = '\0';

  while (*string)
  {
    string = getline (string, tmpb);
    sprintf (buf2, "%s\n\r", tmpb);
    strcat (buf, buf2);
  }

  return buf;
}

char *strcap_color (int length, char *string, ...)
{

  static char buf[10][2 * MSL];
  char buf2[2 * MSL];
  static int spread;
  int place = 0;
  va_list args;
  spread = (++spread) % 10;
  buf[spread][0] = '\0';
  va_start (args, string);
  vsprintf (buf[spread], string, args);
  va_end (args);


  if (length < 0)
    return buf[spread];

  while (strlen_color (buf[spread]) < length)	// Needs trailing spaces
  {
    sprintf (buf2, "%s ", buf[spread]);
    sprintf (buf[spread], "%s", buf2);
  }

  if (strlen_color (buf[spread]) > length)	// Needs capped at length
  {
    int count = 0;
    buf[spread][0] = '\0';
    while (count < length)
    {
      if (string[place] == '`' && string[place + 1] != '`')
      {
	sprintf (buf2, "%s%c%c", buf[spread], string[place],
		 string[place + 1]);
	sprintf (buf[spread], "%s", buf2);
	place++;
	place++;
	continue;
      }
      if (string[place] == '`' && (string[place + 1] == '`'))
      {
	sprintf (buf2, "%s%c%c", buf[spread], string[place],
		 string[place + 1]);
	sprintf (buf[spread], "%s", buf2);
	place++;
	place++;
	count++;
	continue;
      }
      sprintf (buf2, "%s%c", buf[spread], string[place]);
      sprintf (buf[spread], "%s", buf2);
      count++;
      place++;
    }
  }
  sprintf (buf2, "%s", buf[spread]);
  sprintf (buf[spread], "%s", buf2);
  return buf[spread];
}


/*
 * This is a modified version of a function written by Wreck.
 * Original at http://dark.nrg.dtu.dk/~wreck/mud/code/color.txt
 */
int strlen_color (char *argument)
{
  char *str;
  int length = 0;

  if (IS_NULLSTR (argument))
    return 0;

  str = argument;
  while (*str != '\0')
  {
    if (*str != '`')
    {
      str++;
      length++;
      continue;
    }
    if (*(++str) == '`')
      length++;
    str++;
  }
  return length;
}
