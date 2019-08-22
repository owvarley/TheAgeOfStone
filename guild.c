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
 * @@ Objective (guild.c)
 *
 *  Meant to provide a guild system configurable without recompiling. This is
 *  done via an ASCII file called "guild.dat" that you can edit or
 *  create with any text editor (vi/pico/jed/jaret/etc).  All
 *  gulid/clan related commands from ROM 2.4 have been moved to this
 *  file in an effort to keep related code located all in one spot.
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"





/** Function: is_leader
* Descr   : Validates guild leadership of (ch).
* Returns : True (yes, ch is a mortal leader) or False
* Syntax  : (n/a)
* Written : v1.1 3/98
* Author  : Gary McNickle <gary@tarmongaidon.org>
*/
bool is_leader (CHAR_DATA * ch)
{
  /* update by: Zak Johnson (jakj@usa.net) Thanks Zak! */
  return IS_FLAG (ch->states.player, _ACT_MORTAL_LEADER) ? TRUE : FALSE;
}				/* end: is_leader */



  /** Function: can_guild
  * Descr   : Validates ability of (ch) to guild other players into
  *         : a specific (or generic, if implementor) guild.
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
*/
bool can_guild (CHAR_DATA * ch)
{
  /* ok if ch is a GOD or higher */
  if (ch->level >= GOD || ch->trust >= GOD)
    return TRUE;

  /* not ok if ch is not guilded or is not a mortal leader */
  if (ch->clan < 0 || !is_leader (ch))
    return FALSE;

  return clan_table[ch->clan].ml[0];
}				/* end: can_guild */



  /** Function: can_deguild
  * Descr   : Validates ability of (ch) to de-guild players from a 
  *         : particular guild, or generic (if ch=implementor)
  * Returns : TRUE/FALSE
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
*/
bool can_deguild (CHAR_DATA * ch)
{
  /* ok if ch is a GOD or higher */
  if (ch->level >= GOD || ch->trust >= GOD)
    return TRUE;

  /* not ok if ch is not guilded or is not a mortal leader */
  if (ch->clan < 0 || !is_leader (ch))
    return FALSE;

  return clan_table[ch->clan].ml[1];
}				/* end: can_deguild */



  /** Function: can_promote
  * Descr   : Validates ability of (ch) to promote players in their guilds
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
*/
bool can_promote (CHAR_DATA * ch)
{
  /* ok if ch is a GOD or higher */
  if (ch->level >= GOD || ch->trust >= GOD)
    return TRUE;

  /* not ok if ch is not guilded or is not a mortal leader */
  if (ch->clan < 0 || !is_leader (ch))
    return FALSE;

  /* is a mortal leader, but do they have the right? */
  return clan_table[ch->clan].ml[2];
}				/* end: can_promote */



  /** Function: can_demote
  * Descr   : Validates ability of (ch) to demote a player within their guild.
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
*/
bool can_demote (CHAR_DATA * ch)
{
  /* ok if ch is a GOD or higher */
  if (ch->level >= GOD || ch->trust >= GOD)
    return TRUE;

  /* not ok if ch is not guilded or is not a mortal leader */
  if (ch->clan < 0 || !is_leader (ch))
    return FALSE;

  return clan_table[ch->clan].ml[3];
}				/* end: can_demote */




  /** Function: is_clan
  * Descr   : Returns wether or not (ch) is a member of any guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.0 ? Last Updated on 9/23/98
  * Author  : Rom Consortium. Updated by Gary McNickle
*/
bool is_clan (CHAR_DATA * ch)
{
  return ch->clan > 0 ? TRUE : FALSE;
}				/* end: is_clan */




  /** Function: is_same_clan
  * Descr   : Returns wether or not (ch) and (victim) are members of
  *         : the same guild/clan
  * Returns : True/False
  * Syntax  : (n/a)
  * Written : v1.2 1/98
  * Author  : Rom Consortium. Updated by Gary McNickle <gary@tarmongaidon.org>
*/
bool is_same_clan (CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch->clan < 0 || victim->clan < 0 ||
      (IS_FLAG (clan_table[ch->clan].flags, _INDEPENDENT)))
    return FALSE;
  else
    return (ch->clan == victim->clan);
}				/* end: is_same_clan */



/** Function: clan_lookup
  * Descr   : Returns the numeric position of a clan name within the
  *         : clan_table 
  * Returns : (int)
  * Syntax  : (n/a)
  * Written : v1.0 
  * Author  : Rom Consortium
  */
int clan_lookup (const char *name)
{
  int clan;

  for (clan = 0; clan < maxClan; clan++)
  {
    if ((clan_table[clan].name && clan_table[clan].name[0] != '\0') &&
	!str_prefix (name, clan_table[clan].name))
      return clan;
  }

  return -1;
}				/* end: clan_lookup */


/** Function: player_rank
  * Descr   : Returns the players (ch) rank as a string.
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *player_rank (CHAR_DATA * ch)
{
  if (ch->clan < 0)
    return '\0';
  return clan_table[ch->clan].rank[ch->rank].rankname;
}				/* end: player_rank */



/** Function: player_clan
  * Descr   : Returns the name of the players (ch) clan as a string
  * Returns : char*
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *player_clan (CHAR_DATA * ch)
{
  if (ch->clan < 0)
    return '\0';
  return clan_table[ch->clan].name;
}				/* end: player_clan */


/** Function: do_promote
  * Descr   : Promotes (or demotes) a character within their own guild
  * Returns : List of ranks if given no argument
  * Syntax  : promote <who> <rank #>
  * Written : v1.5 3/98 Last updated on 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_promote)
{
  char arg1[MIL];
  CHAR_DATA *victim;
  int cnt;
  int sn = 0;

  argument = one_argument (argument, arg1);

  if (!can_promote (ch))
  {
    send_to_char ("huh?\n\r", ch);
    return;
  }

  if (IS_FLAG (ch->states.penalties, _NOPROMOTE))
  {
    send_to_char ("Your authority to promote another has been revoked.\n\r",
		  ch);
    return;
  }

  if (arg1[0] == '\0' || argument[0] == '\0')
  {
    /*
       * --------------------------------------------------------------
       * Keep in mind that we are displaying the ranks as 1 - MAX_RANK,
       * so, since the structure is actually 0 - MAX_RANK-1, we need to
       * set "cnt" to cnt-1.
       * -------------------------------------------------------------- 
     */

    send_to_char ("Syntax: promote <who> <rank #>\n\r", ch);
    send_to_char ("where rank is one of the following:\n\r\n\r", ch);

    if ((victim = get_char_world (ch, arg1)) == NULL)
      victim = ch;

    if (!is_clan (victim))
    {
      send_to_char ("That person cannot be promoted.\n\r", ch);
      return;
    }

    for (cnt = 0; cnt < MAX_RANK; cnt++)
    {
      if (!IS_NULLSTR (clan_table[victim->clan].rank[cnt].rankname)
	  && str_cmp (clan_table[victim->clan].rank[cnt].rankname,
		      "Unassigned"))
	chprintf (ch, "%2d] %s\n\r", cnt + 1,
			clan_table[victim->clan].rank[cnt].rankname);
    }
    send_to_char ("\n\r", ch);
    return;
  }				/* end syntax */

  if ((victim = get_char_world (ch, arg1)) == NULL)
  {
    send_to_char ("They aren't playing.\n\r", ch);
    return;
  }

  if (!is_clan (victim))
  {
    send_to_char ("They are not a member of any guilds!\n\r", ch);
    return;
  }

  if (IS_FLAG (victim->states.penalties, _NOPROMOTE))
  {
    send_to_char ("That person may not be promoted.\n\r", ch);
    return;
  }

  if (is_clan (victim) && !is_same_clan (ch, victim)
      && ((ch->level < SUPREME) & (ch->trust < SUPREME)))
  {
    send_to_char ("They are a member of a guild different than yours!\n\r",
		  ch);
    return;
  }

  if (!str_prefix (argument, "leader")
      && ((ch != victim) | (IS_IMMORTAL (ch))))
  {
    SET_FLAG (victim->states.player, _ACT_MORTAL_LEADER);

    if (ch != victim)
      send_to_char ("They are now a mortal leader.\n\r", ch);

    send_to_char
      ("You have just been promoted to a leader of your guild!\n\r", victim);
    return;
  }

  cnt = atoi (argument) - 1;

  if (cnt < 0 || cnt > MAX_RANK - 1
      || clan_table[victim->clan].rank[cnt].rankname == NULL)
  {
    send_to_char ("That rank does not exist!", ch);
    return;
  }

  if (cnt > victim->rank && ((ch == victim) & (!IS_IMMORTAL (ch))))
  {
    send_to_char ("Heh. I dont think so...", ch);
    return;
  }

 /* bug report by: Zak Jonhnson (zakj@usa.net)
  * we were checking ch->rank here..*sigh* Thanks Zak!
  */
  if (cnt > victim->rank)
  {
    int i;

    victim->rank = cnt;
    chprintf (victim, "You have been promoted to %s!\n\r",
		    clan_table[victim->clan].rank[cnt].rankname);
    chprintf (ch, "%s has been promoted to %s!\n\r", victim->name,
		    clan_table[victim->clan].rank[cnt].rankname);
    roster_change (victim, victim->clan, TRUE);

    for (i = 0; i < cnt; i++)
    {
      if (clan_table[victim->clan].rank[i].skillname != NULL
	  && clan_table[victim->clan].rank[i].skillname[0] != '\0')
      {
	sn = skill_lookup (clan_table[victim->clan].rank[i].skillname);

	if (sn < 0)
	{
	  bugf ("[guild.c::do_promote] Add skill [%s] is not a valid skill",
		clan_table[victim->clan].rank[cnt].skillname);
	}
	else if (!victim->pcdata->learned[sn])
	{
	  chprintf (ch, "%s given to %s at %d%%\n\r",
			  skill_table[sn].name, victim->name,
			  20 + (victim->level / 4));
	  victim->pcdata->learned[sn] = 20 + (victim->level / 4);
	}
      }
    }
  }
  else if (cnt < victim->rank)
  {
    if (IS_FLAG (victim->states.player, _ACT_MORTAL_LEADER))
      REMOVE_FLAG (victim->states.player, _ACT_MORTAL_LEADER);

    victim->rank = cnt;

    chprintf (victim, "You have been demoted to %s!\n\r",
		    clan_table[victim->clan].rank[cnt].rankname);

    chprintf (ch, "%s has been demoted to %s!\n\r", victim->name,
		    clan_table[victim->clan].rank[cnt].rankname);
    roster_change (victim, victim->clan, TRUE);

    /*
       * ---------------------------------------------------------------
       * Note: I dont think it would be fair here to take away any skills
       * the victim may have earned at a higher rank. It makes no RP sense
       * to do so and only hurts the player (loss of practices etc). Imms
       * may want to keep an eye on this, as we dont want players jumping
       * guilds just to gain new skills.
       * -------------------------------------------------------------- 
     */
  }				/* else no change */

  return;
}



/** Function: do_guild
  * Descr   : Adds a character to the membership of a guild, or optionally,
  *         : removes them from a guild.
  * Returns : (n/a)
  * Syntax  : guild <who> <clan name>
  * Written : v1.3 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_guild)
{
  char arg1[MIL], arg2[MIL];
  char buf[MSL];
  CHAR_DATA *victim;
  int clan;

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if (!can_guild (ch))
  {
    send_to_char ("huh?\n\r", ch);
    return;
  }

  if (IS_FLAG (ch->states.penalties, _NOGUILD))
  {
    send_to_char ("Your ability to guild another has been revoked.\n\r", ch);
    return;
  }

  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char ("Syntax: guild <char> <cln name>\n\r", ch);
    return;
  }

  if ((victim = get_char_world (ch, arg1)) == NULL)
  {
    send_to_char ("They aren't playing.\n\r", ch);
    return;
  }

  if (IS_FLAG (victim->states.penalties, _NOGUILD))
  {
    send_to_char ("They have been banned from all guilds.\n\r", ch);
    return;
  }

  /* Dont allow imms and mortal leaders to guild themselves into
     different guilds. */
  if ((victim == ch) && ch->level < SUPREME)
  {
    send_to_char ("Guild hopping is not allowed!\n\r", ch);
    logf ("do_guild: %s attempted to guild hop.\n\r", ch->name);
    return;
  }

	  /** thanks to Zanthras for the bug fix here...*/
  if (is_clan (victim) && !is_same_clan (ch, victim) &&
      ((ch->level < SUPREME) & (ch->trust < SUPREME)))
  {
    send_to_char ("They are a member of a guild other than your own.\n\r",
		  ch);
    return;
  }

  if (!str_prefix (arg2, "none"))
  {

    send_to_char ("They are no longer a member of any guild.\n\r", ch);
    send_to_char ("You are no longer a member of any guild!\n\r", victim);
    if (!(victim->clan <= 0))
      roster_change (victim, victim->clan, FALSE);
    victim->clan = -1;
    victim->rank = 0;

    /* add by: Zak Jonhson (zakj@usa.net) */
    if (IS_FLAG (victim->states.player, _ACT_MORTAL_LEADER))
      REMOVE_FLAG (victim->states.player, _ACT_MORTAL_LEADER);

    if (victim->in_room->clan && (victim->clan != victim->in_room->clan))
      do_recall (victim, "");

    return;
  }

  if ((clan = clan_lookup (arg2)) <= 0)
  {
    send_to_char ("No such guild exists.\n\r", ch);
    return;
  }


  sprintf (buf, "They are now a %s of the %s.\n\r",
	   clan_table[clan].rank[0].rankname, clan_table[clan].name);
  send_to_char (buf, ch);

  sprintf (buf, "You are now a %s of the %s.\n\r",
	   clan_table[clan].rank[0].rankname, clan_table[clan].name);
  send_to_char (buf, victim);

  victim->clan = clan;
  victim->rank = 0;		/* lowest, default */
  roster_change (victim, victim->clan, TRUE);

  /* Guild Specific code */

  if (IS_FLAG (clan_table[victim->clan].flags, _WOLF))
  {
    AFFECT_DATA af;

    af.where = TO_AFFECTS;
    af.type = -1;
    af.level = LEVEL_HERO;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = _AFF_DARK_VISION;
    affect_to_char (victim, &af);
    send_to_char ("Your vision takes a moment to adjust to the light.\n\r",
		  ch);
  }
  else
   if (IS_FLAG (clan_table[victim->clan].flags, _HUNTER))
  {
    victim->pcdata->learned[ skill_lookup("search") ] += 20;
  }

}


/** Function: do_roster
  * Descr   : Retrieves rank/player information on players guilds
  *         : can retrieve info on current guild as well and non current guild.
  * Returns : (n/a)
  * Syntax  : roster <clan name>/ roster 
  * Written : v1.2 2/00
  * Author  : Steve Boleware <boleware@bossig.com> / Falknor <PartyG2816@aol.com>
  */
_DOFUN (do_rostor)
{
	char arg[MIL];
	char buf[MSL];
	FILE *fp;
	int clan;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		if (ch->clan == 0)
		{
			send_to_char ("Roster which clan?\n\r", ch);
			return;
		}
		else
		{
			clan = ch->clan;
		}
	}				//added to allow clan members to type roster and get their clan info as default-SB
	else
	{
		clan = clan_lookup (arg);
	}
	
	if (clan == -1)
	{
		send_to_char ("No clan by that name exists.\n\r", ch);
		return;
	}
	
	sprintf (buf, "%s%s.rst", RST_DIR, capitalize (clan_table[clan].name));
	
	if ((fp = fopen (buf, "r")) == NULL)
	{
		send_to_char ("Unable to open clan roster! Notify an imm.\n\r", ch);
		return;
	}
	
	get_roster (ch, fp, clan);
	return;
}

/** Function: roster_change
  * Descr   : Applies changes in guild ranks (guilds, promotions, and demotions)
  *         : and saves info to disk.
  * CalledBy: do_guild and do_promote
  * Returns : (n/a)
  * Syntax  : (n/a) 
  * Written : v1.2 2/00
  * Author  : Steve Boleware <boleware@bossig.com> / Falknor <falknor@tarmongaidon.org>
  */
void roster_change (CHAR_DATA * victim, int clan, bool add)
{
	char buf[MSL];
	char filebuf[MSL];
	char name[MIL];
	char letter;
	char writbuf[MSL];
	FILE *read;
	FILE *write;
	sh_int newrank = 0;
	bool done = FALSE;
	bool append = FALSE;


	return; // This is busted.... returning until fixed.
	
	if (clan < 0)
	{
		bugf ("[guild.c::roster_change] Bad value for clan");
		return;
	}
	else
		sprintf (filebuf, "%s%s.rst", RST_DIR,
		capitalize (clan_table[clan].name));
	
	read = fopen (filebuf, "r");
	
	if (!add)
		sprintf (writbuf, "#\n$~\n");
	else
		sprintf (writbuf, "#\n%s~ %d\n$\n", victim->name, victim->rank);
	
	if (read == NULL)
	{
		write = fopen (filebuf, "w");
		fprintf (write, writbuf);
		fclose (write);
		return;
	}
	else
	{
		int count = 0;
		letter = fread_letter (read);
		
		sprintf (writbuf, "#\n");
		
		if (letter != '#')
		{
			bugf ("[guild.c::roster_change] (#) file header not found .");
			fclose (read);
			return;
		}
		fread_to_eol (read);
		
		for (;;)
		{
			char *netter = fread_string (read);
			if (netter[0] != '$')
			{
				count++;
				newrank = fread_number (read);
				sprintf (name, "%s", netter);
				if (is_name (victim->name, name))
				{
					if (victim->rank == newrank)
					{
						if (!add)
						{
							append = TRUE;
							continue;
						}
						done = TRUE;
						append = FALSE;
						break;
					}
					else
					{
						append = TRUE;
						continue;
					}
				}
				sprintf (buf, "%s~ %d\n", name, newrank);
				strcat (writbuf, buf);
				append = TRUE;
			}				// end of first ifcheck-SB
			else
			{
				done = TRUE;
				break;
			}
		}				// end of for loop-SB
		
		if (count < 1)
		{
			if (add)
			{
				sprintf (buf, "%s~ %d\n", victim->name, victim->rank);
				strcat (writbuf, buf);
				append = TRUE;
			}				//end of add check-SB
		}				//end of count less than 1 check(file empty)-SB
		
		if (done)
		{
			if (append)
			{
				if (add && (count > 0))
				{
					sprintf (buf, "%s~ %d\n", victim->name, victim->rank);
					strcat (writbuf, buf);
				}			//above happens when append true and add true and count greater than 0 (file not empty)-SB
				fclose (read);
				strcat (writbuf, "$~\n");
				write = fopen (filebuf, "w");
				fprintf (write, writbuf);
				fclose (write);
				return;
			}				// end of bool check for append-SB
			else
			{
				fclose (read);
				return;
			}
		}				// end of bool check for done-SB
	}				// end of main else-SB
}




/** Function: get_roster
  * Descr   : Reads roster infomation from file on disk and displays info
  * CalledBy: do_roster
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.2 2/00
  * Author  : Steve Boleware <boleware@bossig.com> / Falknor <falknor@tarmongaidon.org>
  */
void get_roster (CHAR_DATA * ch, FILE * fp, int clan)
{
	char rank_name[MAX_RANK][MSL];
	char letter, tmpbuf[MSL], buf[MSL];
	int rank_holder[MAX_RANK][2] = { {0}, {0} };
	int rank_loop = 0;
	bool done = FALSE;
	
	for (rank_loop = 0; rank_loop < MAX_RANK; rank_loop++)
		sprintf (rank_name[rank_loop], " ");
	
	sprintf (buf, " ");
	
	letter = fread_letter (fp);
	
	if (letter != '#')
	{
		bugf ("[guild.c::get_roster] (#) file header not found .");
		fclose (fp);
		return;
	}
	
	for (;;)
	{
		char *word = fread_string (fp);
		
		if (word[0] != '$')
		{
			int rank_read = fread_number (fp);
			
			if (rank_holder[rank_read][2] > 0)
				sprintf (buf, ", %s", word);
			else
				sprintf (buf, "%s", word);
			
			strcat (rank_name[rank_read], buf);
			rank_holder[rank_read][1] = 1;
			rank_holder[rank_read][2]++;
			fread_to_eol (fp);
		}				//end of word[0] check (check if not $)-SB
		else
		{
			done = TRUE;
			fread_to_eol (fp);
			fclose (fp);
		}
		
		if (done)
		{
			break;
		}
	}				// end of for loop-SB
	
	sprintf (buf, "Clan data for %s:`w", clan_table[clan].who_name);
	
	for (rank_loop = 0; rank_loop < MAX_RANK; rank_loop++)
	{
		if (rank_holder[rank_loop][1] == 1)
		{
			sprintf (tmpbuf, "\n\r%s: `W[`C%d`W]`w\n\r",
				strcap_color (20, clan_table[clan].rank[rank_loop].rankname),
				(rank_loop + 1));
			strcat (buf, tmpbuf);
			strcat (buf, rank_name[rank_loop]);
		}
	}
	
	strcat (buf, "\n\r");
	page_to_char (buf, ch);
	
	return;
}
