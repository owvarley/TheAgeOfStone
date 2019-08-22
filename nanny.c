/** 
 * @@ Copyright (The Age of Sail Project)
 * Part of the TaoS Project.
 * The Age of Sail (TaOS): Version v0.2 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

/** 
 * @@ Copyright (The Age of Stone Project)
 *
 * The Age of Stone (TaoS): Version v0.1 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

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
 * @@ Objective (nanny.c)
 * 
 * The nanny() function is a state machine that handles the various
 * 'CON_' states a player may find themselves in.  Moved out of comm.c
 * due to it's size, we've also rewritten the function to use various
 * 'setstate' and 'getstate' functions to aid in keeping nanny() 
 * understandable and easily modified. (er, more easily anyway)
 */

#if !defined(_WIN32)
  #include <sys/time.h>
  #include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "telnet.h"
#include "merc.h"


const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };


/** Function: announce_new_player 
  * Descr   : logs the new player name, and announces it over wiznet.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
void announce_new_player (DESCRIPTOR_DATA * d, CHAR_DATA * ch)
{
  char log_buf[MSL];

  sprintf (log_buf, "%s@%s new player.", ch->name, d->host);
  log_string (log_buf);
  wiznet ("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
  wiznet (log_buf, NULL, NULL, WIZ_SITES, 0, get_trust (ch));
  pMud.newbies++;

  return;
}				/* end: announce_new_player */



/** Function: setstate_choose_color
  * Descr   : Sets the "CON_GET_COLOR" state.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 4/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void setstate_choose_color (DESCRIPTOR_DATA * d, CHAR_DATA * ch)
{
  send_to_char
    ("\n\r  This mud system utilizes ANSI color to brighten things up a bit.\n\r",
     ch);
  send_to_char
    ("ANSI can certainly help in discriminating messages from the system, but\n\r",
     ch);
  send_to_char ("on slower computers, it can also slow you down.\n\r", ch);
  send_to_char ("\n\rDo you want ANSI Color? ", ch);
  d->connected = CON_GET_COLOR;
  return;
}				/* end: setstate_choose_color */



/** Function: setstate_get_new_sex
  * Descr   : Sets the "CON_GET_NEW_SEX" state and prompts user for sex.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v0.1 04/02
  * Author  : Owen Varley <owen@interactiontraining.com>
  */
void setstate_get_new_sex (DESCRIPTOR_DATA * d)
{
  
  write_to_buffer (d, "Is your Captain Male or Female? (M/F)", 0);
  d->connected = CON_GET_NEW_SEX;

  return;
} /* end: setstate get_new_sex */



/** Function: setstate_get_new_port
  * Descr   : Allows Captain to choose the port he hails from
  *           sets the "CON_GET_NEW_PORT" state.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v0.1 04/02
  * Author  : Owen Varley <owen@interactiontraining.com>
  */
void setstate_get_new_port (DESCRIPTOR_DATA * d, CHAR_DATA * ch)
{

  send_to_char("\n\rWhat is the background of your Captain?\n\r", ch);
  send_to_char("The backgrounds available are:\n\r\n\r", ch);
  send_to_char("Merchant\n\r", ch);
  send_to_char("Naval\n\r", ch);
  send_to_char("Fishing\n\r", ch);
  send_to_char("Slaver\n\r", ch);
  send_to_char("Freelance\n\r", ch);

  send_to_char("\n\r`wWhat is your captain's background? (M, N, F, S, L)\n\r", ch);
	   


  // Sets CON state
  d->connected = CON_GET_NEW_PORT;

  return;
}				/* end: setstate_get_new_port */


  

  
/** Function: setstate_confirm_new_name
  * Descr   : Prompts user for confirmation on unknown name before setting
  *           "CON_CONFIRM_NEW_NAME" state.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  * Edited  : Owen Varley
  * Reason  : Changing Disclaimer for name.
  */

void setstate_confirm_new_name (DESCRIPTOR_DATA * d, char *argument,
				CHAR_DATA * ch)
{ 
  char buf[MSL];
  
  write_to_buffer (d,"\n\rAs the player, you are responsible for the name you choose.  All names\n\r",0);
  write_to_buffer (d, "save those that are vulgar and offensive will be permitted, however names\n\r",0);
  write_to_buffer (d, "that do not fit the theme will face a small penalty to their character's\n\r", 0);
  write_to_buffer (d, "attributes.  Exceptional names will be given bonuses.\n\r\n\r",0);

  sprintf (buf, "Are you sure this is the name you wish, Captain %s (Y/N)? ", argument);
  write_to_buffer (d, buf, 0);
  d->connected = CON_CONFIRM_NEW_NAME;

  return;
} /* end: setstate_confirm_new_name */


/*
 * NOTE: Begin connected state functions
 */


/** Function: get_name_state
  * Descr   : Receives player name, checking against various states of the
  *         : system. (such as newlock/wizlock/etc).
  * Returns : TRUE if player is a new player, and we need to get his password.
  * Syntax  : get_name_state <player name>
  * Written : v1.0 3/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
bool get_name_state (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA * ch)
{
  bool fOld;
  int result = READ_OK;

  if (argument[0] == '\0')
  {
    close_descriptor (d);
    return FALSE;
  }

  argument[0] = UPPER (argument[0]);
  if (!check_parse_name (argument))
  {
    write_to_buffer (d, "Illegal name, try another.\n\rName: ", 0);
    return FALSE;
  }

  result = load_char_obj (d, argument, FALSE);
  fOld = (result == READ_OK || result == READ_POOR);
  ch = d->character;

  if (ch->clan == 0 && !IS_IMMORTAL (ch))
    ch->clan = -1;

  switch (result)
  {
	  case READ_POOR:
		write_to_buffer (d, "There is a problem with your player file.\n\r"
				 "Please ask to speak to an implementor.\n\r", 0);
		break;

	  case READ_ERROR:
		write_to_buffer (d,
				 "A critical error has occured while reading your player"
				 "file.\n\rCouncil has been notified. Until the "
				 "file is repaired, you will not be able\n\rto log on with "
				 "this character.\n\r", 0);
		return FALSE;

	  default:
		break;
  }

  if (IS_FLAG (ch->states.penalties, _DENY))
  {
	  logf ("Denying access to %s@%s.", argument, d->host);
	  write_to_buffer (d, "You are denied access.\n\r", 0);
	  close_descriptor (d);
	  return FALSE;
  }
  
  if (check_ban (d->host, BAN_PERMIT)
      && !IS_FLAG (ch->states.penalties, _PERMIT))
  {
	  write_to_buffer (d, "Your site has been banned from this mud.\n\r", 0);
	  close_descriptor (d);
	  return FALSE;
  }
  
  if (IS_SET (pMud.flags, MUD_REDIRECT))
  {
	  char buf[MSL];
	  
	  memset(buf, 0, MSL);

	  if ( !IS_NULLSTR(pMud.RedirectMSG) )
   	    write_to_buffer (d, pMud.RedirectMSG, 0);
	  else
	    write_to_buffer (d, "The mud is closed.\n\r", 0);

	  if ( !IS_NULLSTR(pMud.RedirectURL) )
	  {
		sprintf(buf, "Please goto %s instead.\n\r", pMud.RedirectURL);
	    write_to_buffer (d, buf, 0);
	  }

	  close_descriptor (d);
	  return FALSE;
  }
  
  if (check_reconnect (d, argument, FALSE))
  {
	  fOld = TRUE;
  }
  else
  {
	  if (IS_SET (pMud.flags, MUD_CLOSED) && ch->level < ML - 2)
	  {
		  write_to_buffer (d, "\n\rThe game is currently closed for repairs.\n\r"
			  "We expect to be back up again soon.\n\r", 0);
		  close_descriptor (d);
		  return FALSE;
	  }
	  else
		  if (IS_SET (pMud.flags, MUD_IMMLOCK) && !IS_IMMORTAL (ch))
		  {
			  write_to_buffer (d, "The game is ImmLocked.\n\r", 0);
			  close_descriptor (d);
			  return FALSE;
		  }
  }
  
  if (fOld)
  {
	  /* Old player */
	  write_to_buffer (d, "Password: ", 0);
	  write_to_buffer (d, echo_off_str, 0);
	  d->connected = CON_GET_OLD_PASSWORD;
	  return FALSE;
  }
  else
  {
	  /* New player */
	  if (IS_SET (pMud.flags, MUD_NEWLOCK))
	  {
		  write_to_buffer (d, "The game is newlocked.\n\r", 0);
		  close_descriptor (d);
		  return FALSE;
	  }
	  
	  if (check_ban (d->host, BAN_NEWBIES))
	  {
		  write_to_buffer (d, "New players are not allowed from your site.\n\r",
			  0);
		  close_descriptor (d);
		  return FALSE;
	  }
	  
	  return TRUE;
  }
  
  return FALSE;
} /* end get_name_state */


/** Function: get_old_password_state
  * Descr   : checks for a valid password, if found, continues and notifies wiznet
  *           if not found, disconnects player. 
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
bool get_old_password_state (DESCRIPTOR_DATA * d, char *argument,
			     CHAR_DATA * ch)
{
  char log_buf[MSL];

  if (strcmp (crypt (argument, ch->pcdata->pwd), ch->pcdata->pwd))
  {
    write_to_buffer (d, "Wrong password.\n\r", 0);
    close_descriptor (d);
    return FALSE;
  }

  write_to_buffer (d, echo_on_str, 0);

  if (check_playing (d, ch->name))
    return TRUE;

  if (check_reconnect (d, ch->name, TRUE))
    return TRUE;

  sprintf (log_buf, "%s@%s has connected.",
	   ch->name,
	   (ch->pcdata->hidden_as && ch->pcdata->hidden_as[0] != '\0') ?
	   ch->pcdata->hidden_as : d->host);

  log_string (log_buf);
  wiznet (log_buf, NULL, NULL, WIZ_SITES, 0, get_trust (ch));
  pMud.logins++;

  snd_trigger (ch, pMud.sounds, SOUND_SYSTEM, SOUND_SYSTEM_LOGON, TRUE);


  if ( ch->hit > 1900 || ch->move > 800 )
  {
	  sprintf(log_buf, "[nanny::get_old_password_state] Warning. %s's hitpoints or endurance are too high! [%dhp|%dmv]", ch->name, ch->hit, ch->move);
	  wiznet(log_buf, NULL, NULL, WIZ_SECURE, 0, get_trust(ch));
  }

  /////////////////////////////////////////////////////////////////////////////
  /// Update the player 
  /////////////////////////////////////////////////////////////////////////////

  if ( ch->pcdata->talents[gtn_dreaming] && get_skill(ch, skill_lookup("dream")) < 1 )
  {
	  ch->pcdata->learned[skill_lookup("dream")] = UMIN(100,
		  (!str_cmp(ch->pcdata->nation,"Threefold Land")) ? 
			15 + ch->level + ch->perm_stat[STAT_AGL] : ch->level + ch->perm_stat[STAT_AGL]);

	  chprintf(ch, 
		  "\n\r\n\rThe dreaming system has finally been installed. Your talent with dreaming\n\r"
		  "has earned you a free dream skill rating of %d.  This figure is based on\n\r"
		  "your level, plus your unmodified intelligence. Aiel characters get an extra bonus...\n\r\n\r",
		  ch->pcdata->learned[skill_lookup("dream")]);
  }
  
  if (ch->version < VERSION && ch->pcdata->shares > 0)
  {

    chprintf (ch,
		    "Due to a WoT conflict, the investment system on the mud\n\r"
		    "has been disabled.  Your %ld shares have been resold to you at "
		    "a value of %ld gold.\n\r", ch->pcdata->shares,
		    ch->pcdata->shares * pMud.share_value);

    ch->pcdata->balance = UMAX (0, ch->pcdata->balance +
				(ch->pcdata->shares * pMud.share_value));
    ch->pcdata->shares = 0;
  }

  if (ch->version < VERSION)
  {
    do_save (ch, "");
    write_to_buffer (d,
		     "\n\rYour pfile is outdated. Although we have attempted to\n\r"
		     "automatically update it, if you notice anything amiss\n\r"
			 "it may be best to recreate or notify council.\n\r", 0);
	
  }
 

  if (IS_IMMORTAL (ch))
  {
    do_imotd (ch, "");
    d->connected = CON_READ_IMOTD;
  }
  else
  {
    do_motd (ch, "");
    d->connected = CON_READ_MOTD;
  }

  return TRUE;
} /* end: get_old_password_state */



/** Function: break_connect_state
  * Descr   : handles players who have lost link, but not yet been kicked off. 
  *           prompts to reconnect already connected player.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */ 
bool break_connect_state (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA * ch)
{
  DESCRIPTOR_DATA *d_next, *d_old;

  switch (*argument)
  {
  case 'y':
  case 'Y':
    for (d_old = descriptor_list; d_old != NULL; d_old = d_next)
    {
      d_next = d_old->next;
      if (d_old == d || d_old->character == NULL)
	continue;

      if (str_cmp (ch->name, d_old->original ?
		   d_old->original->name : d_old->character->name))
	continue;

      close_descriptor (d_old);
    }

    if (check_reconnect (d, ch->name, TRUE))
      return TRUE;

    write_to_buffer (d, "Reconnect attempt failed.\n\rName: ", 0);
    if (d->character != NULL)
    {
      free_char (d->character);
      d->character = NULL;
    }

    d->connected = CON_GET_NAME;
    break;

  case 'n':
  case 'N':
    write_to_buffer (d, "Name: ", 0);
    if (d->character != NULL)
    {
      free_char (d->character);
      d->character = NULL;
    }

    d->connected = CON_GET_NAME;
    break;

  default:
    write_to_buffer (d, "Please type Y or N? ", 0);
    return FALSE;

  }				/* end: switch */

  return TRUE;
}				/* end: break_connect_state */


/** Function: confirm_new_name_state
  * Descr   : When an unrecognized name is entered, this function prompts the user 
  *           to determine if name was entered correctly or not.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
bool confirm_new_name_state (DESCRIPTOR_DATA * d, char *argument,
			     CHAR_DATA * ch)
{
  char buf[MSL];

  switch (*argument)
  {
  case 'y':
  case 'Y':
    sprintf (buf, "New Captain\n\rGive me a password for %s: %s",
	     ch->name, echo_off_str);
    write_to_buffer (d, buf, 0);
    d->connected = CON_GET_NEW_PASSWORD;
    break;

  case 'n':
  case 'N':
    write_to_buffer (d, "Ok, what IS it, then? ", 0);
    free_char (d->character);
    d->character = NULL;
    d->connected = CON_GET_NAME;
    return FALSE;

  default:
    write_to_buffer (d, "Please type Yes or No? ", 0);
    return FALSE;
  }				/* end: switch */

  return TRUE;
}				/* end: confirm_new_name_state */



/** Function: get_new_password_state
  * Descr   : prompts new player for a password. 
  *         : sets "CON_CONFIRM_NEW_PASSWORD" state.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
bool get_new_password_state (DESCRIPTOR_DATA * d, char *argument,
			     CHAR_DATA * ch)
{
  char *pwdnew;
  char *p;

#if defined(unix)
  write_to_buffer (d, "\n\r", 2);
#endif

  if (strlen (argument) < 5)
  {
    write_to_buffer (d,
		     "Password must be at least five characters long.\n\rPassword: ",
		     0);
    return FALSE;
  }

  pwdnew = crypt (argument, ch->name);
  for (p = pwdnew; *p != '\0'; p++)
  {
    if (*p == '~')
    {
      write_to_buffer (d,
		       "New password not acceptable, try again.\n\rPassword: ",
		       0);
      return FALSE;
    }
  }

  free_string (ch->pcdata->pwd);
  ch->pcdata->pwd = str_dup (pwdnew);
  write_to_buffer (d, "Please retype password: ", 0);
  d->connected = CON_CONFIRM_NEW_PASSWORD;

  return TRUE;
}				/* end: get_new_password_state */



/** Function: confirm_new_password
  * Descr   : Upon entry of an unrecognized password for a character, prompts
  *         : for correct password.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
bool confirm_new_password_state (DESCRIPTOR_DATA * d, char *argument,
				 CHAR_DATA * ch)
{
	char *szPass;

#if defined(unix)
  write_to_buffer (d, "\n\r", 2);
#endif

  szPass = crypt(argument, ch->pcdata->pwd);

  if (strcmp (szPass, ch->pcdata->pwd))
  {
    write_to_buffer (d, "Passwords don't match.\n\rRetype password: ", 0);
    d->connected = CON_GET_NEW_PASSWORD;
    return FALSE;
  }

  return TRUE;
} /* end: confirm_new_password_state */


/** Function: choose_color_state
  * Descr   : receives users selection for ANSI color
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 4/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool choose_color_state (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA * ch)
{
  switch (*argument)
  {
  case 'y':
  case 'Y':
    SET_FLAG (ch->states.config, _ANSI);
    send_to_char ("\n\r`WA`BN`YS`CI`w color is active.\n\r", ch);
    break;
  default:
    send_to_char
      ("\n\rANSI not selected. Type \"color\" later to select ANSI color.\n\r",
       ch);
    break;
  }
  return TRUE;			/* regardless of input */
}				/* end: choose_color_state */


/** Function: get_new_sex_state
  * Descr   : receives players "sex" selection.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
bool get_new_sex_state (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA * ch)
{

  switch (argument[0])
  {
  case 'm':
  case 'M':
    ch->sex = SEX_MALE;
    ch->pcdata->true_sex = SEX_MALE;
    return TRUE;
    break;
  case 'f':
  case 'F':
    ch->sex = SEX_FEMALE;
    ch->pcdata->true_sex = SEX_FEMALE;
    return TRUE;
    break;
  default:
    write_to_buffer (d, "That's not a sex.\n\rWhat IS your sex? ", 0);
    return FALSE;
  }

  return TRUE;
}				/* end: get_new_sex_state */


/** Function: get_new_port_state
  * Descr   : receives players selection of their region of birth
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : 4/02
  * Author  : Owen Varley
  */
bool get_new_port_state (DESCRIPTOR_DATA * d, char *argument,
			    CHAR_DATA * ch)
{
   
   if (argument[0] == '\0')
    return FALSE;


  switch (argument[0])
  {

  case 'M':
  case 'm':
	  {
	ch->pcdata->background = 1;
	return TRUE;
	break;
	  }
  case 'F':
  case 'f':
	  {
    ch->pcdata->background = 2;
	return TRUE;
	break;
	  }
  case 'N':
  case 'n':
	  {
    ch->pcdata->background = 3;
	return TRUE;
	break;
	  }
  case 'S':
  case 's':
	  {
	ch->pcdata->background = 4;
	return TRUE;
	break;
	  }
  case 'L':
  case 'l':
	  {
	ch->pcdata->background = 5;
	return TRUE;
	break;
	  }
	  
  default:
    write_to_buffer (d, "That is not a valid Background.\n\rWhat is YOUR background?\n\r", 0);
	return FALSE;
  }

 
	return TRUE;
}				/* end: get_new_port_state */



 

 /** Function: read_motd_state
  * Descr   : Displays MOTD, Mortal News of The Day to players, also checks for
  *         : the existance of a null password string.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  */
void read_motd_state (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA * ch)
{

  if (ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
  {
    write_to_buffer (d, "Warning! Null password!\n\r", 0);
    write_to_buffer (d, "Please report old password with bug.\n\r", 0);
    write_to_buffer (d, "Type 'password null <new password>' to fix.\n\r", 0);
  }

  write_to_buffer (d, pMud.welcome_message, 0);
  ch->next = char_list;
  char_list = ch;
  d->connected = CON_PLAYING;
  reset_char (ch);

  // Initiation of a new character into the system
  // setting level, titles, exp per level, etc.

  if (ch->level < 1)
  {
    pMud.newchars++;
    ch->level = 1;
    ch->train = 3; 
    ch->practice = 5;
	
	//New player so we need to add bonuses for the abilities they selected
	//as well as add the impediments 

	// + 3 to SEN attr if they selected Sharp Senses attribute
	if (ch->ability[ABILITY_SHARPSENSES] == 1)
		ch->perm_stat[STAT_SEN] += 3;

	// - 3 to SEN attri if they selected the Deaf Impediment
	if (ch->impediment[IMPEDIMENT_DEAF] == 1)
		ch->perm_stat[STAT_SEN] -= 3;

	// Calculated plyers hit and move
	ch->max_hit  = (ch->perm_stat[STAT_CON] * 5) + 50;
	ch->max_move = (ch->perm_stat[STAT_CON] * 5) + 50;
	ch->hit = ch->max_hit;
    ch->move = ch->max_move;

    set_title (ch, "starts his life in a `Rnew`w, strange world.");
    do_outfit (ch, "");
    char_to_room (ch, get_room_index (ROOM_VNUM_SCHOOL));
    send_to_char ("\n\r", ch);

    do_help (ch, "NEWBIE INFO");
    send_to_char ("\n\r", ch);


  }

  else if (ch->in_room != NULL)
    char_to_room (ch, ch->in_room);
  else if (IS_IMMORTAL (ch))
    char_to_room (ch, get_room_index (ROOM_VNUM_CHAT));
  else
    char_to_room (ch, get_room_index (ROOM_VNUM_LOGIN));

  act ("$n has entered the game.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
  do_look (ch, "auto");

  wiznet ("$N has left real life behind.", ch, NULL,
	  WIZ_LOGINS, WIZ_SITES, get_trust (ch));

  if (ch->pet != NULL)
  {
    char_to_room (ch->pet, ch->in_room);
    act ("$n has entered the game.", ch->pet, NULL, NULL, TO_ROOM,
	 POS_RESTING);
  }

  if (ch->mount != NULL && (ch->pet != ch->mount) && ch->vstable == 0)
  {
    char_to_room (ch->mount, ch->in_room);
    act ("$n has entered the game.", ch->mount, NULL, NULL, TO_ROOM,
	 POS_RESTING);
  }

  do_board (ch, ""); /* Show board status */

  return;

}	/* end: read_motd_state */



/** Function: nanny
  * Descr   : Deals with sockets that have not yet logged in, and also parses all
  *         : connected ("CON_") states.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Rom Consortium/Gary McNickle <gary@tarmongaidon.org>
  * Edited  : Owen Varley
  * Reason  : To add new CON states for new Chargen abilities
  */
void nanny (DESCRIPTOR_DATA * d, char *argument)
{
  CHAR_DATA *ch;

  /* Delete leading spaces UNLESS character is writing a note */
  if (d->connected != CON_NOTE_TEXT)
  {
    while (isspace (*argument))
      argument++;
  }

  ch = d->character;

  switch (d->connected)
  {
  default:
    bugf ("[nanny.c::nanny] bad d->connected %d.", d->connected);
    close_descriptor (d);
    return;

  case CON_FTP_AUTH:
    handle_ftp_auth (d, argument);
    break;

  case CON_FTP_COMMAND:
    handle_ftp_command (d, argument);
    break;

  case CON_FTP_DATA:
    handle_ftp_data (d, argument);
    break;


  case CON_GET_NAME:
    if (get_name_state (d, argument, ch))
      setstate_confirm_new_name (d, argument, ch);
    break;

  case CON_GET_OLD_PASSWORD:
    get_old_password_state (d, argument, ch);
    /* sets either CON_READ_MOTD or CON_READ_IMOTD */
    break;

  case CON_BREAK_CONNECT:
    break_connect_state (d, argument, ch);
    break;

  case CON_CONFIRM_NEW_NAME:
    confirm_new_name_state (d, argument, ch);
    /* sets either state CON_GET_NEW_PASSWORD or CON_GET_NAME */
    break;

  case CON_GET_NEW_PASSWORD:
    get_new_password_state (d, argument, ch);
    /* sets state CON_CONFIRM_NEW_PASSWORD */
    break;

  case CON_CONFIRM_NEW_PASSWORD:
    if (confirm_new_password_state (d, argument, ch))
      setstate_choose_color (d, ch);
    break;

  case CON_GET_COLOR:
    choose_color_state (d, argument, ch);
    setstate_get_new_sex (d);
    break;

  case CON_GET_NEW_SEX:
    if (get_new_sex_state (d, argument, ch))
      setstate_get_new_port (d, ch);
    break;

  case CON_GET_NEW_PORT:
    if (get_new_port_state (d, argument, ch))
      read_motd_state (d, argument, ch);
    break;

   case CON_READ_IMOTD:
    write_to_buffer (d, "\n\r", 2);
    do_motd (ch, "");
    d->connected = CON_READ_MOTD;
    break;

  case CON_READ_MOTD:
    read_motd_state (d, argument, ch);
    break;

    /* states for new note system, (c)1995-96 erwin@pip.dknet.dk */
    /* ch MUST be PC here; have nwrite check for PC status! */

  case CON_NOTE_TO:
    handle_con_note_to (d, argument);
    break;

  case CON_NOTE_SUBJECT:
    handle_con_note_subject (d, argument);
    break;			/* subject */

  case CON_NOTE_EXPIRE:
    handle_con_note_expire (d, argument);
    break;

  case CON_NOTE_TEXT:
    handle_con_note_text (d, argument);
    break;

  }				/* end: switch */

  return;
}
