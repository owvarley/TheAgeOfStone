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
 * @@ Objective (act_comm.c)
 * 
 * This file contains high-level communication commands, including 
 * player-to-group, player-to-mud, player-to-player as well as commands to 
 * configure the various communication states for a player.
 */

#if defined(WIN32)
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"


/* At time of this writing, act_comm.c is the first file in our compile list.  
 * Move the following #pragmas to any file that you insert before act_comm
 */
#if !defined(linux) 
#if _M_IX86 == 300
#pragma message( "Compiling for i386 processors." )
#elif _M_IX86 == 400
#pragma message( "Compiling for i486 processors." )
#elif _M_IX86 == 500
#pragma message( "Compiling for Pentium processors." )
#elif _M_IX86 == 600
#pragma message( "Compiling for Pentium PRO processors." )
#elif defined(_M_ALPHA)
#pragma message( "Compiling for Alpha processors." )
#endif
#endif

//Linux dosent seem to support pragma messages.
//#if defined(linux)
//#pragma message( "Linux OS Detected. Using BSD sockets.")
//#else
#if defined(_WIN32)
#pragma message( "Win32 Detected. Using Winsock.")
//		#pragma message( "Compiling " __FILE__ "::Last modified on " __TIMESTAMP__ ) 
#endif
//#endif



/** Function: do_pkquest
  * Descr   : Toggles a players PKQUESTOR flag. While set, PK level 
  *           restrictions are ignored. Not saved to the pfile.
  * Returns : N/A
  * Syntax  : N/A
  * Written : v1.0 1/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_pkquest)
{
	if (IS_NPC (ch))
	{
		send_to_char ("Yeah yeah sure thing...\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.config, _PKQUESTOR))
	{
		send_to_char ("PK Quest flag removed. Level restrictions restored.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _PKQUESTOR);
		return;
	}
	
	SET_FLAG (ch->states.config, _PKQUESTOR);
	send_to_char ("PK Quest flag is set. Level restrictions removed!\n\r", ch);
	return;
	
}

/** Function: do_rpquest
  * Descr   : Toggles a players RPQUESTOR flag. Does nothing other than
  *           place an [RPQ] prefix to a players who listing
  * Returns : N/A
  * Syntax  : N/A
  * Written : v1.0 1/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_rpquest)
{
	if (IS_NPC (ch))
	{
		send_to_char ("Yeah yeah sure thing...\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.config, _RPQUESTOR))
	{
		send_to_char ("RP Quest flag removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _RPQUESTOR);
		return;
	}
	
	SET_FLAG (ch->states.config, _RPQUESTOR);
	send_to_char ("RP Quest flag is set.\n\r", ch);
	return;
	
}


/** Function: do_delete
  * Descr   : Deletes a player file and disconnects the player.
  *         : note: player must enter command twice.
  * Returns : void
  * Syntax  : delete [any argument will cancel]
  * Written : v1.0 11/98 
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_delete)
{
	char strsave[MIL];
	
	if (IS_NPC (ch))
		return;
	
	if (ch->pcdata->confirm_delete)
	{
		if (argument[0] != '\0')
		{
			send_to_char ("Delete status removed.\n\r", ch);
			ch->pcdata->confirm_delete = FALSE;
			return;
		}
		else
		{
			sprintf (strsave, "%s%c/%s",
				PLAYER_DIR,
				UPPER (ch->name[0]), capitalize (fix_name (ch->name, FALSE)));
			
			wiznet ("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
			stop_fighting (ch, TRUE);
			
			pMud.deletions++;
			do_quit (ch, "");
			unlink (strsave);
			
			return;
		}
	}
	
	if (argument[0] != '\0')
	{
		send_to_char ("Just type delete. No argument.\n\r", ch);
		return;
	}
	
	send_to_char ("Type delete again to confirm this command.\n\r"
		"WARNING: this command is irreversible.\n\r"
		"Typing delete with an argument will undo delete status.\n\r",
		ch);
	
	ch->pcdata->confirm_delete = TRUE;
	wiznet ("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust (ch));
}


/** Function: do_channels
  * Descr   : Displays to the player all of their channels and their
  *         : current ON or OFF states.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98 
  * Author  : Rom Consortium, updated by Bill
  *         : Updated last by Gary <gary@tarmongaidon.org>
  */
_DOFUN (do_channels)
{
	const char *C_ON = "`N(`R*`N)`w\n\r";
	const char *C_OFF = "`N( )`w\n\r";
	
	/* lists all channels and their status */
	send_to_char ("channel       status\n\r"
		"`N_`w_`W_`w_`N_`w_`W_`w_`N_`w_`W_`w_`N_`w_`W_`w_`N_`w_"
		"`W_`w_`N_`w\n\r", ch);
	
	chprintf (ch, "%sGossip            %s"
		"%sAuction           %s"
		"%sQuestion/%sAnswer   %s"
		"%sQuote             %s"
		"%sGrats             %s"
		"%sOOC               %s"
		"%sShouts            %s"
		"%sDeaf              %s"
		"%sQuiet Mode        %s",
		GOSSIPC,
		IS_FLAG (ch->states.comm, _GOSSIP) ? C_ON : C_OFF,
		AUCTIONC,
		IS_FLAG (ch->states.comm, _AUCTION) ? C_ON : C_OFF,
		QUESTIONC, ANSWERC,
		IS_FLAG (ch->states.comm, _QUESTION) ? C_ON : C_OFF,
		QUOTEC,
		IS_FLAG (ch->states.comm, _QUOTE) ? C_ON : C_OFF,
		GRATSC,
		IS_FLAG (ch->states.comm, _GRATS) ? C_ON : C_OFF,
		OOCC,
		IS_FLAG (ch->states.comm, _OOC) ? C_ON : C_OFF,
		SHOUTSC,
		IS_FLAG (ch->states.comm, _SHOUTS) ? C_ON : C_OFF,
		TELLSC,
		IS_FLAG (ch->states.config, _DEAF) ? C_ON : C_OFF,
		TELLSC, IS_FLAG (ch->states.config, _QUIET) ? C_ON : C_OFF);
	
	if (IS_IMMORTAL (ch))
	{
		chprintf (ch, "%sImmortal          %s",
			IMMORTALC,
			IS_FLAG (ch->states.comm, _WIZ) ? C_ON : C_OFF);
	}
	
	send_to_char ("`N_`w_`W_`w_`N_`w_`W_`w_`N_`w_`W_`w_`N_`w_`W_`w_`N_"
		"`w_`W_`w_`N_`w\n\r", ch);
	
	
	if (IS_FLAG (ch->states.config, _AFK))
		send_to_char ("You are AFK.\n\r", ch);
	
	if (IS_FLAG (ch->states.config, _BUILDING))
		send_to_char ("You are in building mode.\n\r", ch);
	
	if (IS_FLAG (ch->states.player, _SNOOP_PROOF))
		send_to_char ("You are immune to snooping.\n\r", ch);
	
	if (ch->lines != PAGELEN)
	{
		if (ch->lines)
		{
			chprintf (ch,
				"You display %d lines of scroll.\n\r", ch->lines + 2);
		}
		else
			send_to_char ("Scroll buffering is off.\n\r", ch);
	}
	
	if (ch->prompt != NULL)
		chprintf (ch, "Your current prompt is: %s\n\r", ch->prompt);
	if (IS_FLAG (ch->states.penalties, _NOSHOUT))
		send_to_char ("You cannot shout.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOTELL))
		send_to_char ("You cannot use tell.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOCHANNELS))
		send_to_char ("You cannot use channels.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOEMOTE))
		send_to_char ("You cannot show emotions.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOPK))
		send_to_char ("You cannot PK.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOLOOT))
		send_to_char ("You cannot loot.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOGUILD))
		send_to_char ("You cannot guild another, or be guilded.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOPROMOTE))
		send_to_char ("You cannot promote another, or be promoted.\n\r", ch);
	if (IS_FLAG (ch->states.penalties, _NOFATALS))
		send_to_char ("You cannot fataly kill someone in a PK.\n\r", ch);
}

/** Function: do_deaf
  * Descr   : Toggles the ability to receive tells.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98 
  * Author  : Rom Consortium
  */
_DOFUN (do_deaf)
{
	if (IS_FLAG (ch->states.config, _DEAF))
	{
		send_to_char ("You can now hear tells again.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _DEAF);
	}
	else
	{
		send_to_char ("From now on, you won't hear tells.\n\r", ch);
		SET_FLAG (ch->states.config, _DEAF);
	}
}

/** Function: do_quiet
  * Descr   : Toggles the ability to hear any channels or tells.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98 
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_quiet)
{
	if (IS_FLAG (ch->states.config, _QUIET))
	{
		send_to_char ("Quiet mode removed.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _QUIET);
	}
	else
	{
		send_to_char ("From now on, you will only hear says and emotes.\n\r", ch);
		SET_FLAG (ch->states.config, _QUIET);
	}
}

/** Function: do_building
  * Descr   : Toggles the ability to see global channels or tells, but
  *         : stores all tells into a buffer for later reading.
  *	    : Also makes it so no one but lvl > L3 can interact with you.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  * Note    : Dedicated to Andy. *grin* May you build in peace...
  */
_DOFUN (do_building)
{
	
	if (IS_FLAG (ch->states.config, _BUILDING))
	{
		if (IS_NPC (ch) || IS_SWITCHED (ch))
			send_to_char ("BUILD mode removed.\n\r", ch);
		else
		{
			if (buf_string (ch->pcdata->buffer)[0] == '\0')
				send_to_char ("BUILD mode removed. You have no tells to replay.\n\r",
				ch);
			else
				send_to_char ("BUILD mode removed. Type 'replay' to see tells.\n\r",
				ch);
		}
		REMOVE_FLAG (ch->states.config, _BUILDING);
	}
	else
		
	{
		send_to_char ("You are now in Building mode.\n\r", ch);
		SET_FLAG (ch->states.config, _BUILDING);
	}
}



/** Function: do_afk
  * Descr   : Toggles the ability to see global channels or tells, but
  *         : stores all tells into a buffer for later reading.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_afk)
{
	if (IS_NPC (ch))
	{
		send_to_char ("Sorry dood, but only players may go afk.\n\r", ch);
		return;
	}
	ch = CH (ch->desc);
	
	if (IS_FLAG (ch->states.config, _AFK))
	{
		if (buf_string (ch->pcdata->buffer)[0] == '\0')
			send_to_char ("AFK mode removed. You have no tells to replay.\n\r", ch);
		else
			send_to_char ("AFK mode removed. Type 'replay' to see tells.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _AFK);
	}
	else
		
	{
		send_to_char ("You are now in AFK mode.\n\r", ch);
		SET_FLAG (ch->states.config, _AFK);
	}
}


/** Function: do_replay
  * Descr   : Replays tells that have been stored into a buffer due to
  *         : AFK status.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_replay)
{
	if (IS_NPC (ch))
	{
		send_to_char ("You can't replay.\n\r", ch);
		return;
	}
	
	if (buf_string (ch->pcdata->buffer)[0] == '\0')
	{
		send_to_char ("You have no tells to replay.\n\r", ch);
		return;
	}
	
	page_to_char (buf_string (ch->pcdata->buffer), ch);
	clear_buf (ch->pcdata->buffer);
}

/** Function: do_color
  * Descr   : Toggles color codes being sent to the user
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_color)
{
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.config, _ANSI))
		{
			SET_FLAG (ch->states.config, _ANSI);
			send_to_char ("Your `Wc`Bo`Yl`Co`Rr`w vision returns.\n\r", ch);
		}
		
		else
		{
			REMOVE_FLAG (ch->states.config, _ANSI);
			send_to_char ("For some reason, your vision is all in"
				" shades of grey...\n\r", ch);
		}
	}
}


/** Function: do_clantalk
  * Descr   : If argument is given, sends argument across guild only
  *         : communication channel, else toggles status of this channel
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_clantalk)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	
	if (!is_clan (ch) || IS_FLAG (clan_table[ch->clan].flags, _INDEPENDENT))
	{
		send_to_char ("You aren't in a clan.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		
		if (!IS_FLAG (ch->states.comm, _GUILD))
		{
			send_to_char ("Clan channel is now ON\n\r", ch);
			SET_FLAG (ch->states.comm, _GUILD);
		}
		else
			
		{
			send_to_char ("Clan channel is now OFF\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _GUILD);
		}
		return;
		
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.penalties, _NOCHANNELS))
	{
		send_to_char ("The Creator has revoked your channel priviliges.\n\r", ch);
		return;
	}
	
	SET_FLAG (ch->states.comm, _GUILD);
	
	sprintf (buf, "`B%s %s`w: %s\n\r",
		(player_rank (ch) == '\0') ? "" : player_rank (ch),
		ch->name, argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& is_same_clan (ch, d->character)
			&& IS_FLAG (d->character->states.comm, _GUILD)
			&& !IS_FLAG (d->character->states.config, _QUIET))
		{
			send_to_char (buf, d->character);
		}
		else
			if (d->connected == CON_PLAYING
				&& d->character != ch
				&& IS_FLAG (d->character->states.comm, _GUILD)
				&& !IS_FLAG (d->character->states.config, _QUIET)
				&& IS_FLAG (d->character->states.player, _ACT_WOLFBROTHER)
				&& IS_FLAG (ch->states.player, _ACT_WOLFBROTHER))
			{
				send_to_char ("You hear the faint sound of wolves in the"
					" distance.\n\r", d->character);
			}
	}
	
	return;
}

/** Function: do_towertalk
  * Descr   : Hack of clantalk that sends to both gaidan and Aes Sedai /only/.
  *         : Moridin was adamant on making a full new channel. *shrugs*
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : JD Smith <falknor@tarmongaidon.org
  */
_DOFUN (do_towertalk)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	
	if (!is_clan (ch)
		|| (!IS_FLAG (clan_table[ch->clan].flags, _SEDAI)
		&& !IS_FLAG (clan_table[ch->clan].flags, _TOWERGUARD)
		&& !IS_FLAG (clan_table[ch->clan].flags, _GAIDIN)) )
	{
		send_to_char ("You aren't in a tower clan.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		
		if (!IS_FLAG (ch->states.comm, _TOWER_TALK))
		{
			send_to_char ("Tower Talk channel is now ON\n\r", ch);
			SET_FLAG (ch->states.comm, _TOWER_TALK);
		}
		else
			
		{
			send_to_char ("Tower Talk channel is now OFF\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _TOWER_TALK);
		}
		return;
		
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.penalties, _NOCHANNELS))
	{
		send_to_char ("The Creator has revoked your channel priviliges.\n\r", ch);
		return;
	}
	
	SET_FLAG (ch->states.comm, _TOWER_TALK);
	
	sprintf (buf, "`WTT] `B%s %s`w: %s\n\r",
		(player_rank (ch) == '\0') ? "" : player_rank (ch), ch->name,
		argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& (IS_FLAG (clan_table[d->character->clan].flags, _SEDAI)
			|| IS_FLAG (clan_table[d->character->clan].flags, _TOWERGUARD)
			|| IS_FLAG (clan_table[d->character->clan].flags, _GAIDIN))
			&& IS_FLAG (d->character->states.comm, _TOWER_TALK)
			&& !IS_FLAG (d->character->states.config, _QUIET))
		{
			send_to_char (buf, d->character);
		}
	}
	
	return;
}

/** Function: do_bondtalk
  * Descr   : If no argument is given, toggles the status of this channel.
  *         : Else: if the sender is the holder of the bond, then all who
  *	    : are bonded to them see argument, otherwise, only the sender
  *	    : and the bond holder see the message.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_bondtalk)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	
	if (IS_NPC (ch))
	{
		send_to_char ("NPC's dont hold bonds...\n\r", ch);
		return;
	}
	
	if (ch->pcdata->bonds[0] == 0)
	{
		send_to_char ("But your not bonded to anyone!\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.comm, _BOND))
		{
			send_to_char ("Bond channel is now ON\n\r", ch);
			SET_FLAG (ch->states.comm, _BOND);
		}
		else
		{
			send_to_char ("Bond channel is now OFF\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _BOND);
		}
		return;
	}
	
	if (IS_FLAG (ch->states.penalties, _NOCHANNELS))
	{
		send_to_char ("The Creator has revoked your channel priviliges.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}
	
	SET_FLAG (ch->states.comm, _BOND);
	
	sprintf (buf, "[%s] %s %s: %s%s`w\n\r",
		BONDH, (player_rank (ch) == '\0') ? "" : player_rank (ch),
		ch->name, BONDC, argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& (ch == d->character
			|| is_bonded_to (d->character, ch)
			|| is_bonded_to (ch, d->character))
			&& IS_FLAG (d->character->states.comm, _BOND))
		{
			send_to_char (buf, d->character);
		}
	}
	
	return;
}


/** Function: do_immtalk
  * Descr   : if no argument is given, toggles the status of this channel.
  *         : Else, sends argument across immortal only channel.
  * Returns : void
  * Syntax  : immtalk [message]
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_immtalk)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.comm, _WIZ))
		{
			send_to_char ("Immortal channel is now ON\n\r", ch);
			SET_FLAG (ch->states.comm, _WIZ);
		}
		else
		{
			send_to_char ("Immortal channel is now OFF\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _WIZ);
		}
		return;
	}
	
	SET_FLAG (ch->states.comm, _WIZ);
	
	sprintf (buf, "[%s] %s %s: %s%s`w\n\r",
		IMMORTALH, (player_rank (ch) == '\0') ? "" : player_rank (ch),
		ch->name, IMMORTALC, argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& IS_IMMORTAL (d->character)
			&& IS_FLAG (d->character->states.comm, _WIZ))
		{
			send_to_char (buf, d->character);
		}
	}
	
	return;
}

/** Function: is_ignoring
  * Descr   : Determines if ( receiver ) is ignoring ( sender ) or not.
  *         : (ie: rcvr has set ignore on all comm. from sender )
  * Returns : TRUE/FALSE
  * Syntax  : is_ignoring( sender data, receiver data )
  * Written : v1.0 11/98
  * Author  : Dominic J. Eidson <eidsod01@condor.stcloud.msus.edu>
  *         : Updated by Gary <gary@tarmongaidon.org>
  */
bool is_ignoring (CHAR_DATA * sender, CHAR_DATA * receiver)
{
	int pos;
	bool found = FALSE;
	
	if (IS_NPC (sender) || IS_NPC (receiver) || !IS_FLAG(pMud.config, _USE_IGNORE))
		return FALSE;
	
	for (pos = 0; pos < MAX_IGNORE; pos++)
	{
		if (receiver->pcdata->ignore[pos] == NULL)
			break;
		if (!str_cmp (sender->name, receiver->pcdata->ignore[pos]))
			found = TRUE;
	}
	return found;
}


/** Function: public_ch
  * Descr   : Function to replace the repetitive code from global channels
  * Returns : void
  * Syntax  : public_ch( sender, message, channel type, channel bitname )
  * Written : v1.0 11/98
  * Author  : Blade of "- E -". Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void public_ch (CHAR_DATA * ch, char *argument, char *type, const int bitname)
{
	char buf[MIL + 200];
	DESCRIPTOR_DATA *d;
	
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.comm, bitname))
		{
			sprintf (buf, "%s channel is now ON.\n\r", type);
			send_to_char (buf, ch);
			SET_FLAG (ch->states.comm, bitname);
		}
		else
		{
			sprintf (buf, "%s channel is now OFF.\n\r", type);
			send_to_char (buf, ch);
			REMOVE_FLAG (ch->states.comm, bitname);
		}
	}
	
	else				/* msg sent, turn channel on if it isn't already */
	{
		if (IS_FLAG (ch->states.config, _QUIET))
		{
			send_to_char ("You must turn off quiet mode first.\n\r", ch);
			return;
		}
		if (IS_FLAG (ch->states.penalties, _NOCHANNELS))
		{
			send_to_char ("The gods have revoked your channel priviliges.\n\r", ch);
			return;
		}
		
		if (IS_AFFECTED (ch, _AFF_GAG_AIR) && bitname != _OOC)
		{
			send_to_char ("You can't open your mouth!\n\r", ch);
			return;
		}
		
		if (IS_AFFECTED (ch, _AFF_MUTE) && bitname != _OOC)
		{
			send_to_char ("You can't speak past the wound in your mouth!\n\r", ch);
			return;
		}

		SET_FLAG (ch->states.comm, bitname);
		
		if (bitname != _OOC && bitname != _GOSSIP)
			argument = makedrunk (argument, ch);
		
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			CHAR_DATA *victim;
			victim = d->original ? d->original : d->character;
			
			if ( !victim || (!IS_IMMORTAL(ch) && IS_AFFECTED(victim, _AFF_DEAF)) )
				continue;

			if (d->connected == CON_PLAYING
				&& IS_FLAG (victim->states.comm, bitname)
				&& !IS_FLAG (victim->states.config, _QUIET))
			{
				
				switch (bitname)
				{
				case _OOC:
					sprintf (buf, "[%s] %s '%s%s`w'\n\r",
						OOCH, PERS (ch, victim), OOCC, argument);
					break;
				case _AUCTION:
					sprintf (buf, "[%s] %s '%s%s`w'\n\r",
						AUCTIONH, PERS (ch, victim), AUCTIONC, argument);
					break;
				case _GOSSIP:
					if ( IS_FLAG(pMud.config, _USE_OLD_GOSSIP) )
					{
						sprintf (buf, "[%s] %s '%s%s`w'\n\r",
							GOSSIPH, PERS (ch, victim), GOSSIPC, argument);
					}
					break;
				case _GRATS:
					sprintf (buf, "[%s] %s '%s%s`w'\n\r",
						GRATSH, PERS (ch, victim), GRATSC, argument);
					break;
				case _QUOTE:
					sprintf (buf, "[%s] %s '%s%s`w'\n\r",
						QUOTEH, PERS (ch, victim), QUOTEC, argument);
					break;
				case _QUESTION:
					if (!str_cmp(type,"Question"))
					{
						sprintf (buf, "[%s] %s '%s%s`w'\n\r",
							QUESTIONH, PERS (ch, victim), QUESTIONC, argument);
						break;
					}
					else
					{
						sprintf (buf, "[%s] %s '%s%s`w'\n\r",
							ANSWERH, PERS (ch, victim), ANSWERC, argument);
						break;
					}
					
					break;
				}			/* end of case */
				
				if (!is_ignoring (ch, victim))
					send_to_char (buf, d->character);
			}
		}
	}
}


/** Function: do_sound
  * Descr   : Toggles wether or not sounds (MSP data) will be sent
  *         : to the player.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_sound)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _SOUNDON))
	{
		send_to_char ("You will no longer hear sounds.\n\r", ch);
		REMOVE_FLAG (ch->states.config, _SOUNDON);
	}
	else
		
	{
		send_to_char ("You will now hear sounds.\n\r", ch);
		SET_FLAG (ch->states.config, _SOUNDON);
	}
	
}


/** Function: do_ooc
  * Descr   : Sends an 'out of character' message to the entire mud
  * Returns : void
  * Syntax  : ooc message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_ooc)
{
	public_ch (ch, argument, "OOC", _OOC);
}


/** Function: do_gossip
  * Descr   : Sends an (in character) gossip message to the entire mud.
  * Returns : void
  * Syntax  : gossip what?
  * Written : v1.0 12/98
  * Author  : Bill Larson
  */
_DOFUN (do_gossip)
{
	DESCRIPTOR_DATA *d;


	if ( IS_FLAG(pMud.config, _USE_OLD_GOSSIP) )
	{
		public_ch (ch, argument, "Gossip", _GOSSIP);
		return;
	}
	
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.comm, _GOSSIP))
		{
			send_to_char ("Gossip channel is now ON.\n\r", ch);
			SET_FLAG (ch->states.comm, _GOSSIP);
		}
		else
		{
			send_to_char ("Gossip channel is now OFF.\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _GOSSIP);
		}
		
		return;
	}
	
	/* gossip message sent, turn gossip on if it isn't already */
	if (IS_FLAG (ch->states.config, _QUIET))
	{
		send_to_char ("You must turn off quiet mode first.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.comm, _NOCHANNELS))
	{
		send_to_char ("The gods have revoked your channel priviliges.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't talk past the wound in your mouth!\n\r", ch);
		return;
	}

	SET_FLAG (ch->states.comm, _GOSSIP);
	
	chprintf (ch, "You gossip '%s%s`w'\n\r", GOSSIPC, argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *victim;
		
		victim = d->original ? d->original : d->character;
		
		if (d->connected == CON_PLAYING
			&& d->character != ch
			&& IS_FLAG (victim->states.comm, _GOSSIP)
			&& !IS_AFFECTED(victim, _AFF_DEAF)
			&& !IS_FLAG (victim->states.config, _QUIET))
		{
			if (!is_ignoring (ch, victim))
			{
				if (IS_IMMORTAL (d->character))
				{
					chprintf (d->character,
						"[%sGossip`w] %s '%s%s`w'\n\r",
						GOSSIPC, ch->name, GOSSIPC, argument);
				}
				
				else
				{
					chprintf (d->character,
						"%s '%s%s`w'\n\r", GOSSIPH, GOSSIPC, argument);
				}
			}
		}
	}
}


/** Function: do_grats
  * Descr   : sends a "congratz" (typically out of character) message to
  *         : everyone on the mud.
  * Returns : void
  * Syntax  : grats message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_grats)
{
	public_ch (ch, argument, "Gratz", _GRATS);
}


/** Function: do_quote
  * Descr   : Sends a quote (should be in character) to the entire mud
  * Returns : void
  * Syntax  : quote message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_quote)
{
	public_ch (ch, argument, "Quote", _QUOTE);
}


/** Function: do_question
  * Descr   : Asks a question to the entire mud (is out of character)
  * Returns : void
  * Syntax  : question message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_question)
{
	public_ch (ch, argument, "Question", _QUESTION);
}


/** Function: do_answer
  * Descr   : Ideally, answers a previous question.
  * Returns : void
  * Syntax  : answer message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_answer)
{
	public_ch (ch, argument, "Answer", _QUESTION);
}


/** Function: emphatic
  * Descr   : Returns the expression of a string. ie: excited, questioning.
  * Returns : 0 == Normal, 1 == Emphatic (!) 2 == Questioning (?)
  * Syntax  : emphatic ( string to check ) 
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
int emphatic (char *argument)
{
	if (strstr (argument, "!") != NULL)
		return 1;
	else if (strstr (argument, "?") != NULL)
		return 2;
	
	return 0;
}

/** Function: say_as_you_will
  * Descr   : A multi-channel function that converts text into an emphatic
  *         : string. (using func: emphatic) before displaying it.
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void say_as_you_will (CHAR_DATA * ch, char *argument, char *type)
{
	char arg[50];
	CHAR_DATA *victim;
	int exptype = 0;
	int position = POS_RESTING;
	
	argument = makedrunk (one_argument (argument, arg), ch);
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Say what?\n\r", ch);
		return;
	}
	
	
	if ((victim = get_char_world (ch, arg)) == NULL || IS_NPC (victim)
		|| victim->in_room != ch->in_room)
	{
		send_to_char ("They are not here.\n\r", ch);
		return;
	}
	
	if (IS_IMMORTAL (ch))
		position = POS_DEAD;
	else
	if ( IS_AFFECTED(victim, _AFF_DEAF) || IS_FLAG(victim->states.config, _DEAF) )
	{
		send_to_char("They cant hear you.\n\r", ch);
		return;
	}
	

	exptype = emphatic (argument);
	
	if (!str_cmp(type,"Whisper"))
	{
		switch (exptype)
		{
		case 0:
			act ("You whisper '$t' to $N.",
				ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n whispers '$t' to you.",
				ch, argument, victim, TO_VICT, position);
			act ("$n whispers in $N's ear.",
				ch, NULL, victim, TO_NOTVICT, POS_RESTING);
			break;
			
		case 1:
			act ("You whisper emphatically '$t' to $N.",
				ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n emphatically whispers, '$t' to you.",
				ch, argument, victim, TO_VICT, position);
			act ("$n whispers emphatically in $N's ear.",
				ch, NULL, victim, TO_NOTVICT, POS_RESTING);
			break;
			
		case 2:
			act ("You whisper the question, '$t' to $N.",
				ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n questioningly whispers, '$t' to you.",
				ch, argument, victim, TO_VICT, position);
			act ("$n whispers questioningly in $N's ear.",
				ch, argument, victim, TO_NOTVICT, POS_RESTING);
			break;
			
			break;
			
		}				/* end: switch (whisper) */
	}
	else				/* default */
	{
		switch (exptype)
		{
		case 0:
			act ("You say '$t' to $N.", ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n says to you, '$t'.",
				ch, argument, victim, TO_VICT, POS_RESTING);
			act ("$n says '$t' to $N.",
				ch, argument, victim, TO_NOTVICT, POS_RESTING);
			break;
			
		case 1:
			act ("You emphatically say, '$t' to $N.",
				ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n says emphatically, '$t' to you.",
				ch, argument, victim, TO_VICT, POS_RESTING);
			act ("$n emphatically says '$t' to $N.",
				ch, argument, victim, TO_NOTVICT, POS_RESTING);
			break;
			
		case 2:
			act ("You ask $N, '$t'", ch, argument, victim, TO_CHAR, POS_DEAD);
			act ("$n asks you, '$t'", ch, argument, victim, TO_VICT, position);
			act ("$n asks $N, '$t'", ch, argument, victim, TO_NOTVICT, POS_RESTING);
			break;
			
			break;
		}				/* end: switch (say) */
	}
}


/** Function: do_dirsay
  * Descr   : Sends an emphasised say to players.
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_dirsay)
{
	say_as_you_will (ch, argument, "Say");
	return;
}


/** Function: do_whisper
  * Descr   : Sends a "whisper" to players. (converts normal say to a whisper
  *         : like emote)
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_whisper)
{
	say_as_you_will (ch, argument, "Whisper");
	return;
}


/** Function: comm_say
  * Descr   : converted from "do_say". Now a multi-channel version that
  *         : handles normal says, as well as ooc says.
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 5/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
void comm_say (CHAR_DATA * ch, char *argument, char *type)
{
	char *copy;
	
	char buf[MSL];
	if (argument[0] == '\0')
	{
		send_to_char ("Say what?\n\r", ch);
		return;
	}
	
	copy = str_dup (argument);

	// ooc say
	if (!str_cmp(type,"ooc"))
	{
		sprintf (buf, "$n says %s '%s%s`w'", OSAYH, OSAYC, argument);
		act (buf, ch, NULL, ch, TO_ROOM, POS_RESTING);
		sprintf (buf, "You say %s '%s%s`w'", OSAYH, OSAYC, argument);
		act (buf, ch, NULL, ch, TO_CHAR, POS_RESTING);
	}
	// ic say
	else 
	{
		CHAR_DATA *pch = NULL, *pch_next = NULL;

		if (IS_AFFECTED (ch, _AFF_GAG_AIR))
		{
			send_to_char ("You can't open your mouth!\n\r", ch);
			return;
		}

		if (IS_AFFECTED (ch, _AFF_MUTE))
		{
			send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
			return;
		}
		
		argument = makedrunk (argument, ch);

		sprintf (buf, "You say '%s%s`w'", OSAYC, argument);
		act (buf, ch, NULL, ch, TO_CHAR, POS_RESTING);

		sprintf (buf, "$n says '%s%s`w'", OSAYC, argument);
		for ( pch = ch->in_room->people; pch != NULL; pch = pch_next)
		{
			pch_next = pch->next_in_room;

			if ( !IS_IMMORTAL(ch) && (IS_AFFECTED(pch, _AFF_DEAF) || IS_FLAG(pch->states.config, _DEAF)) )
				continue;

			act (buf, ch, NULL, pch, TO_VICT, POS_RESTING);

			if (!IS_NPC(ch) && IS_NPC (pch) && HAS_TRIGGER (pch, TRIG_SPEECH)
				&& pch->position == pch->pIndexData->default_pos)
				mp_act_trigger (copy, pch, ch, NULL, NULL, TRIG_SPEECH);

		}

	}
	
	return;
	
}


/** Function: do_osay
  * Descr   : Uses comm_say to convert normal say into an "ooc" say.
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_osay)
{
	comm_say (ch, argument, "ooc");
	return;
}

/** Function: do_say
  * Descr   : Converted "do_say". Sends a "say" to room.
  * Returns : void
  * Syntax  : n/a
  * Written : v1.0 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_say)
{
	comm_say (ch, argument, "say");
	return;
}


/** Function: do_shout
  * Descr   : Sends a 'shout' to everyone listening on the entire mud.
  * Returns : void
  * Syntax  : shout what?
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_shout)
{
	DESCRIPTOR_DATA *d;
	char buf[MSL];
	
	if (argument[0] == '\0')
	{
		if (!IS_FLAG (ch->states.comm, _SHOUTS))
		{
			send_to_char ("You can hear shouts again.\n\r", ch);
			SET_FLAG (ch->states.comm, _SHOUTS);
		}
		else
		{
			send_to_char ("You will no longer hear shouts.\n\r", ch);
			REMOVE_FLAG (ch->states.comm, _SHOUTS);
		}
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}

	if (IS_FLAG (ch->states.penalties, _NOSHOUT))
	{
		send_to_char ("You can't shout.\n\r", ch);
		return;
	}
	
	SET_FLAG (ch->states.comm, _SHOUTS);
	WAIT_STATE (ch, 12);
	
	argument = makedrunk (argument, ch);
	
	act ("You shout '$T'", ch, NULL, argument, TO_CHAR, POS_RESTING);
	sprintf (buf, "[%s] %s '%s%s`w'", SHOUTSH, ch->name, SHOUTSC, argument);
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *victim;
		victim = d->original ? d->original : d->character ? d->character : NULL;
	
		if ( !victim || (!IS_IMMORTAL(ch) && IS_AFFECTED(victim, _AFF_DEAF)) )
			continue;

		if (d->connected == CON_PLAYING
			&& d->character != ch
				&& IS_FLAG (victim->states.comm, _SHOUTS)
			&& !IS_FLAG (victim->states.config, _QUIET))
		{
			if (!is_ignoring (ch, victim))
				act (buf, ch, NULL, d->character, TO_VICT, POS_RESTING);
		}
	}
	
	return;
}


/** Function: do_tell
  * Descr   : Sends some (that sender can see) a tell. Set's receivers
  *         : 'reply to' data.
  * Returns : void
  * Syntax  : tell who what
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_tell)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	if (IS_FLAG (ch->states.penalties, _NOTELL)
		|| IS_FLAG (ch->states.config, _DEAF))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.config, _QUIET))
	{
		send_to_char ("You must turn off quiet mode first.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.config, _DEAF))
	{
		send_to_char ("You must turn off deaf mode first.\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Tell whom what?\n\r", ch);
		return;
	}
	
	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
 	 */
	if ((victim = get_char_world (ch, arg)) == NULL
		|| (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR) && !IS_IMMORTAL (victim))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE) && !IS_IMMORTAL (victim))
	{
		send_to_char ("The gash across your mouth makes speaking diffiuclt at best.\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (victim))
		argument = makedrunk (argument, ch);
	
	if (victim->desc == NULL && !IS_NPC (victim))
	{
		act ("$N seems to have misplaced $S link...try again later.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s tells you '%s%s`w'\n\r",
			PERS (ch, victim), TELLSC, argument);
		buf[0] = UPPER (buf[0]);
		
		if (!is_ignoring (ch, victim))
			add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	if ( ( !IS_IMMORTAL(ch) || !IS_AFFECTED(ch, _AFF_DREAMING) ) && (!IS_AWAKE(victim) || IS_AFFECTED(victim, _AFF_DEAF)) )
	{
		act ("$E can't hear you.", ch, 0, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if ((IS_FLAG (victim->states.config, _QUIET)
		|| IS_FLAG (victim->states.config, _DEAF)) && !IS_IMMORTAL (ch))
	{
		act ("$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (IS_FLAG (victim->states.config, _AFK))
	{
		if (IS_NPC (victim))
		{
			act ("$E is AFK, and not receiving tells.",
				ch, NULL, victim, TO_CHAR, POS_RESTING);
			return;
		}
		
		act ("$E is AFK, but your tell will go through when $E returns.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s tells you '%s%s`w'\n\r",
			PERS (ch, victim), TELLSC, argument);
		
		buf[0] = UPPER (buf[0]);
		
		if (!is_ignoring (ch, victim))
			add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	
	if (IS_FLAG (victim->states.config, _BUILDING))
	{
		if (IS_NPC (victim))
		{
			act ("$E is building, and not receiving tells.",
				ch, NULL, victim, TO_CHAR, POS_RESTING);
			return;
		}
		
		act ("$E is building, but your tell will go through when $E finishes.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s tells you '%s%s`w'\n\r",
			PERS (ch, victim), TELLSC, argument);
		
		buf[0] = UPPER (buf[0]);
		
		if (!is_ignoring (ch, victim))
			add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	sprintf (buf, "You tell $N '%s%s`w'", TELLSC, argument);
	act (buf, ch, argument, victim, TO_CHAR, POS_RESTING);
	
	if (!is_ignoring (ch, victim))
	{
		sprintf (buf, "$n tells you  '%s%s`w'", TELLSC, argument);
		
		if (IS_IMMORTAL (ch))
			act (buf, ch, argument, victim, TO_VICT, POS_DEAD);
		else
			act (buf, ch, argument, victim, TO_VICT, POS_RESTING);
		
		victim->reply = ch;
	}
	
	if (!IS_NPC (ch) && IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_SPEECH))
		mp_act_trigger (argument, victim, ch, NULL, NULL, TRIG_SPEECH);
	
	return;
}


/** Function: do_reply
  * Descr   : Replies to a tell that someone has previously sent you.
  *         : "Reply To" data is stored in ch->reply as the last person
  *         : to send you a tell.
  * Returns : void
  * Syntax  : reply ( message )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_reply)
{
	CHAR_DATA *victim;
	char buf[MSL];
	
	if (IS_FLAG (ch->states.penalties, _NOTELL))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	
	if ((victim = ch->reply) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR) && !IS_IMMORTAL (victim))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE) && !IS_IMMORTAL (victim))
	{
		send_to_char ("The gash across your mouth makes speaking diffiuclt at best.\n\r", ch);
		return;
	}
	
	if (!IS_IMMORTAL (victim))
		argument = makedrunk (argument, ch);
	
	if (victim->desc == NULL && !IS_NPC (victim))
	{
		act ("$N seems to have misplaced $S link...try again later.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s replies '%s%s`w'\n\r",
			PERS (ch, victim), REPLIESC, argument);
		
		buf[0] = UPPER (buf[0]);
		add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	if (!IS_IMMORTAL (ch) && (!IS_AWAKE (victim) || IS_AFFECTED(victim, _AFF_DEAF)) )
	{
		act ("$E can't hear you.", ch, 0, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if ((IS_FLAG (victim->states.config, _QUIET)
		|| IS_FLAG (victim->states.config, _DEAF))
		&& !IS_IMMORTAL (ch) && !IS_IMMORTAL (victim))
	{
		act ("$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if ( (!IS_IMMORTAL (victim) || !IS_AFFECTED(victim, _AFF_DREAMING) ) && !IS_AWAKE (ch))
	{
		send_to_char ("In your dreams, or what?\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.config, _AFK))
	{
		if (IS_NPC (victim))
		{
			act ("$E is AFK, and not receiving tells.",
				ch, NULL, victim, TO_CHAR, POS_RESTING);
			return;
		}
		
		act ("$E is AFK, but your tell will go through when $E returns.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s replies '%s%s`w'\n\r",
			PERS (ch, victim), REPLIESC, argument);
		
		buf[0] = UPPER (buf[0]);
		add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	if (IS_FLAG (victim->states.config, _BUILDING))
	{
		if (IS_NPC (victim))
		{
			act ("$E is building, and not receiving tells.",
				ch, NULL, victim, TO_CHAR, POS_RESTING);
			return;
		}
		
		act ("$E is building, but your tell will go through when $E finishes.",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "%s replies '%s%s`w'\n\r",
			PERS (ch, victim), REPLIESC, argument);
		
		buf[0] = UPPER (buf[0]);
		add_buf (victim->pcdata->buffer, buf);
		
		return;
	}
	
	sprintf (buf, "You reply to $N '%s%s`w'", REPLIESC, argument);
	
	act (buf, ch, argument, victim, TO_CHAR, POS_DEAD);
	
	sprintf (buf, "$n replies '%s%s`w'", REPLIESC, argument);
	act (buf, ch, argument, victim, TO_VICT, POS_DEAD);
	
	victim->reply = ch;
	
	return;
}



/** Function: do_yell
  * Descr   : Displays the 'yell' to everyone in the yeller's area. 
  * Returns : void
  * Syntax  : yell (what?)
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_yell)
{
	DESCRIPTOR_DATA *d;
	
	if (IS_FLAG (ch->states.penalties, _NOSHOUT))
	{
		send_to_char ("You can't yell.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth to yell!\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("Yell what?\n\r", ch);
		return;
	}
	
	argument = makedrunk (argument, ch);
	
	act ("You yell '$t'", ch, argument, NULL, TO_CHAR, POS_RESTING);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& d->character != ch
			&& d->character->in_room != NULL
			&& d->character->in_room->area == ch->in_room->area
			&& !IS_FLAG (d->character->states.config, _QUIET))
		{
			if (!is_ignoring (ch, d->character))
				act ("$n yells '$t'",
				ch, argument, d->character, TO_VICT, POS_RESTING);
		}
	}
	
	return;
}

/** Function: do_emote
  * Descr   : Displays an 'emote' to the room.
  * Returns : void
  * Syntax  : emote what?
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_emote)
{
	char DQUOTE = 34;		/* a double quote; " */
	char EMOTE_C = 'c';		/* dark cyan         */
	
	char *srcptr, *dstptr, tmp[MSL];
	int highlight_on = 0;		/* false */
	
	memset (tmp, 0, MSL);
	
	if ((!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _NOEMOTE))
		|| IS_AFFECTED (ch, _AFF_GAG_AIR) || IS_AFFECTED (ch, _AFF_BOUND_AIR))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("Emote what?\n\r", ch);
		return;
	}
	
	srcptr = argument;
	dstptr = tmp;
	
	while (*srcptr)
	{
		if (*srcptr == DQUOTE)
		{
			highlight_on = !highlight_on;
			if (highlight_on)
				*dstptr++ = DQUOTE;
			
			*dstptr++ = ANSI_KEY;
			*dstptr++ = (highlight_on) ? EMOTE_C : 'w';
			if (!highlight_on)
				*dstptr++ = DQUOTE;
		}
		else
			*dstptr++ = *srcptr;
		
		srcptr++;
	}
	
	/* just in case some dummy didn't end his quote */
	if (highlight_on)
	{
		*dstptr++ = '`';
		*dstptr++ = 'w';
		*dstptr++ = DQUOTE;
	}
	*dstptr = 0;
	
	if (tmp[0] == '\0')
		sprintf (tmp, "%s", argument);
	
	MOBtrigger = FALSE;
	act ("$n $T", ch, NULL, tmp, TO_ROOM, POS_RESTING);
	act ("$n $T", ch, NULL, tmp, TO_CHAR, POS_RESTING);
	MOBtrigger = TRUE;
	return;
}


/** Function: do_pmote
  * Descr   : Sends an emote to the room. Replaces a matched players name with 
  *			: "you/your".
  * Returns : void
  * Syntax  : pmote argument
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_pmote)
{
	CHAR_DATA *vch;
	char *letter, *name;
	char last[MIL], temp[MSL];
	int matches = 0;
	
	if ((!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _NOEMOTE)) ||
		IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("Emote what?\n\r", ch);
		return;
	}
	
	act ("$n $t `W[$n]`w", ch, argument, NULL, TO_CHAR, POS_RESTING);
	
	for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
	{
		if (vch->desc == NULL || vch == ch)
			continue;
		
		if ((letter = strstr (argument, vch->name)) == NULL)
		{
			MOBtrigger = FALSE;
			act ("$N $t `W[$n]`w", vch, argument, ch, TO_CHAR, POS_RESTING);
			MOBtrigger = TRUE;
			continue;
		}
		
		strcpy (temp, argument);
		temp[strlen (argument) - strlen (letter)] = '\0';
		last[0] = '\0';
		name = vch->name;
		
		for (; *letter != '\0'; letter++)
		{
			if (*letter == '\'' && matches == strlen (vch->name))
			{
				strcat (temp, "r");
				continue;
			}
			
			if (*letter == 's' && matches == strlen (vch->name))
			{
				matches = 0;
				continue;
			}
			
			if (matches == strlen (vch->name))
			{
				matches = 0;
			}
			
			if (*letter == *name)
			{
				matches++;
				name++;
				if (matches == strlen (vch->name))
				{
					strcat (temp, "you");
					last[0] = '\0';
					name = vch->name;
					continue;
				}
				strncat (last, letter, 1);
				continue;
			}
			
			matches = 0;
			strcat (temp, last);
			strncat (temp, letter, 1);
			last[0] = '\0';
			name = vch->name;
		}
		
		MOBtrigger = FALSE;
		act ("$N $t `W[$n]`w", vch, temp, ch, TO_CHAR, POS_RESTING);
		MOBtrigger = TRUE;
	}
	
	return;
}


/** Function: do_bug
  * Descr   : Sends a notice of a found bug to the bug log file
  * Returns : void
  * Syntax  : bug message
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_bug)
{
	char buf[MIL * 2];
	
	if (argument[0] == '\0')
	{
		send_to_char ("Perhaps you mistyped something?\n\r", ch);
		return;
	}
	
	sprintf (buf, "%s%s", DATA_DIR, BUG_FILE);
	append_file (ch, buf, argument);
	send_to_char ("Bug logged.\n\r", ch);
	
	sprintf (buf, "%s reports a bug:\n\r[%s]", ch->name, argument);
	wiznet (buf, NULL, NULL, WIZ_BUGS, 0, 0);
	
	return;
}

/** Function: do_typo
  * Descr   : Logs a notice of a found typo to the type log file
  * Returns : void
  * Syntax  : typo ( argument )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_typo)
{
	char buf[MIL * 2];
	
	if (argument[0] == '\0')
	{
		send_to_char ("Perhaps you mistyped something?\n\r", ch);
		return;
	}
	
	sprintf (buf, "%s%s", DATA_DIR, TYPO_FILE);
	append_file (ch, buf, argument);
	send_to_char ("Typo logged.\n\r", ch);
	
	sprintf (buf, "%s reports a typo:\n\r[%s]", ch->name, argument);
	wiznet (buf, NULL, NULL, WIZ_BUGS, 0, 0);
	
	return;
}


/** Function: do_quit
  * Descr   : Exits a player from the game. (Saves pfile by default)
  *         : Violence timer must not be active, nor can the player be
  *         : charmed or the controller of a circle.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  *		    : Updated by Frank Williams to support linking.
  */
_DOFUN (do_quit)
{
  DESCRIPTOR_DATA *d, *d_next;
  int id, link_count;
  CHAR_DATA *gch;
  CHAR_DATA *leader;
  
  if (IS_NPC (ch))
    return;
	
  if (ch->position == POS_FIGHTING)
  {
    send_to_char ("No way! You are still fighting!\n\r", ch);
	return;
  }
	
  if (ch->position < POS_STUNNED)
  {
    send_to_char ("You're not DEAD yet.\n\r", ch);
	return;
  }
	
  if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master != NULL)
  {
	send_to_char ("Your master wouldn't approve!\n\r", ch);
	return;
  }
	
  if (IS_AFFECTED (ch, _AFF_CAPTURED))
  {
	send_to_char ("I'm afraid escape will not be so easy!\n\r", ch);
	return;
  }
	
  if (ch->linked_to > 0)
  {
	leader = get_char_by_id(ch->linked_to);
	if (ch == leader)
	{
	  send_to_char ("Release or pass the link first!\n\r", ch);
	  return;
	}
	send_to_char ("You rip free from the circle!\n\r", ch);
	ch->move = 0;
	ch->linked_to = 0;
	for (gch = char_list, link_count=0; gch != NULL; gch = gch->next)
	{
	  if (is_linked (gch, leader) && gch != leader)
	  {
	    link_count++;
	    send_to_char ("The circle wavers and nearly folds!\n\r", gch);
		gch->move -= gch->move/2;
	  }
	}
	if (!link_count) // leader is the only one left in the circle
	{
	  send_to_char ("The circle wavers then colapses!\n\r",leader);
	  leader->linked_to = 0;
	  leader->move = 0;
      act ("$n gasps for breath.", leader, NULL, NULL, TO_ROOM, POS_RESTING);
	}
	else
	{
	  send_to_char ("The circle wavers and nearly folds!\n\r",leader);
	  leader->move -= leader->move/2;
	}
  } // end of linked check

	/* get rid of rented room info */
	if (IS_FLAG (ch->states.player, _ACT_TENNANT))
	{
		ROOM_INDEX_DATA *room;
		
		REMOVE_FLAG (ch->states.player, _ACT_TENNANT);
		
		if ((room = get_room_index (ch->pcdata->rentroom)) != NULL)
		{
			REMOVE_BIT (room->room_flags, ROOM_RENTED);
			REMOVE_BIT (room->room_flags, ROOM_PRIVATE);
			room->heal_rate = room->heal_rate / 2;
			room->opu_rate = room->opu_rate / 2;
		}
	}
	
	snd_trigger (ch, pMud.sounds, SOUND_SYSTEM, SOUND_SYSTEM_QUIT, TRUE);
	do_quotes (ch, "");
	
	act ("$n has left the game.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
	logf ("%s has quit.", ch->name);
	wiznet ("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
		get_trust (ch));
	
	save_char_obj (ch);
	
	/* Free note that might be there somehow */
	if (ch->pcdata->in_progress)
		free_note (ch->pcdata->in_progress);
	
	id = GET_ID(ch);
	d = ch->desc;
	
	/*
	 * After extract_char the ch is no longer valid!
	 */
	extract_char (ch, TRUE);
	if (d != NULL)
		close_descriptor (d);
	
	/* toast evil cheating bastards */
	for (d = descriptor_list; d != NULL; d = d_next)
	{
		CHAR_DATA *tch;
		
		d_next = d->next;
		tch = d->original ? d->original : d->character;
		if (tch && GET_ID(tch) == id)
		{
			extract_char (tch, TRUE);
			close_descriptor (d);
		}
	}
	
	return;
}


/** Function: do_save
  * Descr   : Saves (by default) a players data to their pfile, optionally
  *         : (if ch level > 57 ) saves various data files.
  * Returns : void
  * Syntax  : save [data file]
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_save)
{

	if ( IS_NPC(ch) && 
	   ( IS_FLAG(ch->pIndexData->states.player, _ACT_PERSISTANT) || IS_FLAG(ch->states.player, _ACT_PERSISTANT)) )
	{
		CHAR_DATA *vch;

		if ( ch->desc && ch->desc->character )
			vch = ch->desc->character;
		else
			vch = ch;

		chprintf(ch, "NPC saved.\n\r");
		save_char_obj(vch);
		return;
	}

	if (get_trust (ch) >= L8 && argument[0] != '\0')
	{
		
		if (!str_prefix (argument, "options") || !str_prefix(argument, "syntax") )
		{
			chprintf (ch, "Save options\n\r"
				"save <class|guild|skill|names|group"
				"|race|liquid|help|socials|nations>\n\r");
			return;
		}

		if (!str_prefix (argument, "mud") || !str_prefix(argument, "config"))
		{
			chprintf (ch, "%s\n\r",
				save_mud_data () ? "Mud config data saved." : "Error saving mud config data.");
			return;
		}
		if (!str_prefix (argument, "class"))
		{
			chprintf (ch, "%s\n\r",
				save_class_data ()? "Class data saved." : "Error saving class data.");
			return;
		}
		if (!str_prefix (argument, "commands"))
		{
			chprintf (ch, "%s\n\r",
				save_commands ()? "Command data saved." : "Error saving command data.");
			return;
		}
		if (!str_prefix (argument, "guild"))
		{
			chprintf (ch, "%s\n\r",
				save_guilds ()? "Guild data saved." : "Error saving guild data.");
			return;
		}
		if (!str_prefix (argument, "skills"))
		{
			chprintf (ch, "%s\n\r",
				save_skill_data ()? "Skill data saved." : "Error saving skill data.");
			return;
		}
		if (!str_prefix (argument, "names"))
		{
			chprintf (ch, "%s\n\r",
				save_names ()? "Name data saved." : "Error saving name data.");
			return;
		}
		if (!str_prefix (argument, "group"))
		{
			
			chprintf (ch, "%s\n\r",
				save_groups ()? "Group data saved." : "Error saving group data.");
			return;
		}
		if (!str_prefix (argument, "race"))
		{
			chprintf (ch, "%s\n\r",
				save_races ()? "Race data saved." : "Error saving Race data.");
			return;
		}
		if (!str_prefix (argument, "liquid"))
		{
			chprintf (ch, "%s\n\r",
				save_liquids ()? "Liquid data saved." : "Error saving Liquid data.");
			return;
		}
		if (!str_prefix (argument, "forms"))
		{
			chprintf (ch, "%s\n\r",
				save_forms ()? "Form data saved." : "Error saving form data.");
			return;
		}
						  
	}
	
	if (get_trust(ch) >= L4 && ch->pcdata->security >= 7 && argument[0] != '\0')
	{
		if (!str_prefix (argument, "help"))
		{
			chprintf (ch, "%s\n\r",
				save_helps (FALSE) ? "Help file saved." : "Error saving help file.");
			return;
		}
		if (!str_prefix (argument, "social"))
		{
			chprintf (ch, "%s\n\r",
				save_social_table ()? "Social file saved." : "Error saving socials.");
			return;
		}
	}
	
	if (argument[0] != '\0')
		return;
	
	save_char_obj (ch);
	send_to_char ("Saving by request...\n\r", ch);
	
	if ( IS_FLAG(pMud.config, _SAVE_DELAY) && !IS_IMMORTAL (ch) ) 
		WAIT_STATE (ch, 4 * PULSE_VIOLENCE);

	return;
}


/** Function: do_follow
  * Descr   : Player command to follow another, or follow self
  * Returns : void
  * Syntax  : follow ( follower, who to follow )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_follow)
{
	char arg[MIL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Follow whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master != NULL)
	{
		act ("But you'd rather follow $N!",
			ch, NULL, ch->master, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (victim == ch)
	{
		if (ch->master == NULL)
		{
			send_to_char ("You already follow yourself.\n\r", ch);
			return;
		}
		stop_follower (ch);
		return;
	}
	
	if (!IS_NPC (victim)
		&& IS_FLAG (victim->states.config, _NOFOLLOW) && !IS_IMMORTAL (ch))
	{
		act ("$N doesn't seem to want any followers.\n\r",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	REMOVE_FLAG (ch->states.config, _NOFOLLOW);
	
	if (ch->master != NULL)
		stop_follower (ch);
	
	add_follower (ch, victim, TRUE);
	return;
}


/** Function: add_follower
  * Descr   : Adds ch to masters follower list.
  * Returns : void
  * Syntax  : add_follower( follower, master )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
void add_follower (CHAR_DATA * ch, CHAR_DATA * master, bool bNotify)
{
	if (ch->master != NULL)
	{
		bugf("[act_comm.c::add_follower] non-null master.");
		return;
	}
	
	ch->master = master;
	ch->leader = NULL;
	
	if (can_see (master, ch) && bNotify)
		act ("$n now follows you.", ch, NULL, master, TO_VICT, POS_DEAD);

	if (!bNotify)
  	  act ("You begin stalking $N.", ch, NULL, master, TO_CHAR, POS_RESTING);
	else
	  act ("You now follow $N.", ch, NULL, master, TO_CHAR, POS_RESTING);
	
	return;
}


/** Function: stop_follower
  * Descr   : Removes a follower from master (system call)
  * Returns : void
  * Syntax  : stop_follower( follower data )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
void stop_follower (CHAR_DATA * ch)
{
	if (ch->master == NULL)
	{
		bugf("[act_comm.c::stop_follower]  null master.");
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION))
	{
		REMOVE_FLAG (ch->states.affected_by, _AFF_COMPULSION);
		affect_strip (ch, skill_lookup("compell"));
	}
	
	if (can_see (ch->master, ch) && ch->in_room != NULL)
	{
		act ("$n stops following you.", ch, NULL, ch->master, TO_VICT, POS_DEAD);
		act ("You stop following $N.", ch, NULL, ch->master, TO_CHAR, POS_DEAD);
	}
	if (ch->master->pet == ch)
		ch->master->pet = NULL;
	
	if (ch->master->mount == ch)
		ch->master->mount = NULL;
	
	ch->master = NULL;
	ch->leader = NULL;
	return;
}


/** Function: nuke_pets
  * Descr   : Removes (&extracts) all pets from ch (including mounts)
  * Returns : void
  * Syntax  : nuke_pets( from who )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
void nuke_pets (CHAR_DATA * ch)
{
	CHAR_DATA *pet = ch->pet;
	CHAR_DATA *mount = ch->mount;
	
	if (mount && pet && pet == mount)
		ch->pet = NULL;
	
	if (pet != NULL)
	{
		stop_follower (pet);
		
		if (pet->in_room != NULL)
			act ("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT, POS_RESTING);
		
		extract_char (pet, TRUE);
	}
	
	ch->pet = NULL;
	
	if (mount)
	{
		do_dismount (ch, "");
		
		if (mount->in_room != NULL)
			act ("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT, POS_RESTING);
		
		extract_char (mount, TRUE);
	}
	
	ch->mount = NULL;
	
	return;
}


/** Function: die_follower
  * Descr   : Removes all followers (pets/charmies) from ch.
  * Returns : void
  * Syntax  : die_follower( from who? )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
void die_follower (CHAR_DATA * ch)
{
	CHAR_DATA *fch;
	
	if (ch->master != NULL)
	{
		if (ch->master->pet == ch)
			ch->master->pet = NULL;
		
		stop_follower (ch);
	}
	
	ch->leader = NULL;
	
	for (fch = char_list; fch != NULL; fch = fch->next)
	{
		if (fch->master == ch)
			stop_follower (fch);
		
		if (fch->leader == ch)
			fch->leader = fch;
	}
	
	return;
}


/** Function: do_order
  * Descr   : Forces a compelled (charmed) person to do the compeller's
  *         : bidding.  Some commands (like delete) are disallowed.
  * Returns : void
  * Syntax  : order ( leader, who to do what )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_order)
{
	char buf[MSL];
	char arg[MIL], arg2[MIL];
	CHAR_DATA *victim;
	CHAR_DATA *och;
	CHAR_DATA *och_next;
	bool found;
	bool fAll;
	
	argument = one_argument (argument, arg);
	one_argument (argument, arg2);
	
	if (!str_cmp (arg2, "delete") || !str_cmp (arg2, "mob"))
	{
		send_to_char ("That will NOT be done.\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Order whom to do what?\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION))
	{
		send_to_char ("You feel like taking, not giving, orders.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "all"))
	{
		fAll = TRUE;
		victim = NULL;
	}
	else
	{
		fAll = FALSE;
		
		if (!strcmp (arg, "mount"))
		{
			if (!ch->mount)
			{
				send_to_char ("You don't have a mount.\n\r", ch);
				return;
			}
			
			if (ch->mount->in_room != ch->in_room)
			{
				send_to_char ("Your mount isn't here!\n\r", ch);
				return;
			}
			
			else
			{
				victim = ch->mount;
			}
		}
		
		else if ((victim = get_char_room (ch, arg)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}
		
		if (victim == ch)
		{
			send_to_char ("Aye aye, right away!\n\r", ch);
			return;
		}
		
		if (victim->mount == ch)
		{
			if (!mount_success (ch, victim, FALSE))
			{
				act ("$N ignores your orders.",
					ch, NULL, victim, TO_CHAR, POS_RESTING);
				return;
			}
			
			else
			{
				sprintf (buf, "%s orders you to \'%s\'.", ch->name, argument);
				send_to_char (buf, victim);
				interpret (victim, argument);
				return;
			}
		}
		
		else if (!IS_AFFECTED (victim, _AFF_COMPULSION) || victim->master != ch
			|| (IS_IMMORTAL (victim) && victim->trust >= ch->trust))
		{
			send_to_char ("Do it yourself!\n\r", ch);
			return;
		}
	}
	
	found = FALSE;
	for (och = ch->in_room->people; och != NULL; och = och_next)
	{
		och_next = och->next_in_room;
		
		if (IS_AFFECTED (och, _AFF_COMPULSION)
			&& och->master == ch && (fAll || och == victim))
		{
			found = TRUE;
			sprintf (buf, "$n orders you to '%s'.", argument);
			act (buf, ch, NULL, och, TO_VICT, POS_DEAD);
			interpret (och, argument);
		}
	}
	
	if (found)
	{
		WAIT_STATE (ch, PULSE_VIOLENCE);
		send_to_char ("Ok.\n\r", ch);
	}
	else
		send_to_char ("You have no followers here.\n\r", ch);
	
	return;
}

/** Function: do_group
  * Descr   : Allow's "ch" to enter a followed 'argument' character into
  *         : ch's group.  Ch must be the group leader.
  *         : Will also display the group members to anyone in the group.
  * Returns : void
  * Syntax  : group ( leader data, let who in argument )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_group)
{
	char buf[MSL];
	char arg[MIL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		CHAR_DATA *gch;
		CHAR_DATA *leader;
		
		leader = (ch->leader != NULL) ? ch->leader : ch;
		sprintf (buf, "%s's group:\n\r", PERS (leader, ch));
		send_to_char (buf, ch);
		
		for (gch = char_list; gch != NULL; gch = gch->next)
		{
			if (is_same_group (gch, ch))
			{
				sprintf (buf,
					"[%2d %s] %-16.16s %4d/%4d hp %4d/%4d en %5ld xp\n\r",
					gch->level,
					IS_NPC (gch) ? "Mob" : class_table[gch->c_class].who_name,
					capitalize (PERS (gch, ch)),
					gch->hit, gch->max_hit,
					gch->move, gch->max_move,
					(IS_NPC (gch) || gch->level >= LEVEL_HERO) ? 0 :
				((gch->level + 1) *
					exp_per_level (gch, gch->pcdata->points) - gch->exp));
				send_to_char (buf, ch);
			}
		}
		return;
	}
	
	/* We want to allow mortally wounded characters to check
	 * a 'gr' output, but not to join a group. So, we put this check
	 * here.
	 */
	if (ch->position <= POS_MORTAL)
	{
		send_to_char ("You are hurt far to bad for that.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch))
	{
		send_to_char ("But you are following someone else!\n\r", ch);
		return;
	}
	
	if (victim->master != ch && ch != victim)
	{
		act ("$N isn't following you.", ch, NULL, victim, TO_CHAR, POS_DEAD);
		return;
	}
	
	if (IS_AFFECTED (victim, _AFF_COMPULSION))
	{
		send_to_char ("You can't remove charmed mobs from your group.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION))
	{
		act ("You like your master too much to leave $m!",
			ch, NULL, victim, TO_VICT, POS_RESTING);
		return;
	}
	
	if (is_same_group (victim, ch) && ch != victim)
	{
		victim->leader = NULL;
		act ("$n removes $N from $s group.",
			ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		act ("$n removes you from $s group.",
			ch, NULL, victim, TO_VICT, POS_DEAD);
		act ("You remove $N from your group.",
			ch, NULL, victim, TO_CHAR, POS_DEAD);
		return;
	}
	
	victim->leader = ch;
	act ("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
	act ("You join $n's group.", ch, NULL, victim, TO_VICT, POS_RESTING);
	act ("$N joins your group.", ch, NULL, victim, TO_CHAR, POS_DEAD);
	return;
}

/** Function: do_split
  * Descr   : Splits an amount of coin to everyone in the splitters
  *         : group who's in the same room.
  * Returns : void
  * Syntax  : split ( who's splitting, amount )
  * Written : v1.0 11/98
  * Author  : Gnort, God of Chaos.
  *         : Updated by Rom Consortium & Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_split)
{
	char buf[MSL];
	char arg1[MIL], arg2[MIL];
	CHAR_DATA *gch;
	int members;
	int amount_gold = 0, amount_silver = 0;
	int share_gold, share_silver;
	int extra_gold, extra_silver;
	
	argument = one_argument (argument, arg1);
	one_argument (argument, arg2);
	
	if (arg1[0] == '\0')
	{
		send_to_char ("Split how much?\n\r", ch);
		return;
	}
	
	amount_silver = atoi (arg1);
	
	if (arg2[0] != '\0')
		amount_gold = atoi (arg2);
	
	if (amount_gold < 0 || amount_silver < 0)
	{
		send_to_char ("Your group wouldn't like that.\n\r", ch);
		return;
	}
	
	if (amount_gold == 0 && amount_silver == 0)
	{
		send_to_char ("You hand out zero coins, but no one notices.\n\r", ch);
		return;
	}
	
	if (ch->gold < amount_gold || ch->silver < amount_silver)
	{
		send_to_char ("You don't have that much to split.\n\r", ch);
		return;
	}
	
	members = 0;
	for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
	{
		if (is_same_group (gch, ch) && !IS_AFFECTED (gch, _AFF_COMPULSION))
			members++;
	}
	
	if (members < 2)
	{
		send_to_char ("Just keep it all.\n\r", ch);
		return;
	}
	
	share_silver = amount_silver / members;
	extra_silver = amount_silver % members;
	
	share_gold = amount_gold / members;
	extra_gold = amount_gold % members;
	
	if (share_gold == 0 && share_silver == 0)
	{
		send_to_char ("Don't even bother, cheapskate.\n\r", ch);
		return;
	}
	
	ch->silver -= amount_silver;
	ch->silver += share_silver + extra_silver;
	ch->gold -= amount_gold;
	ch->gold += share_gold + extra_gold;
	
	if (share_silver > 0)
	{
		sprintf (buf,
			"You split %d silver coins. Your share is %d silver.\n\r",
			amount_silver, share_silver + extra_silver);
		send_to_char (buf, ch);
	}
	
	if (share_gold > 0)
	{
		sprintf (buf,
			"You split %d gold coins. Your share is %d gold.\n\r",
			amount_gold, share_gold + extra_gold);
		send_to_char (buf, ch);
	}
	
	if (share_gold == 0)
	{
		sprintf (buf, "$n splits %d silver coins. Your share is %d silver.",
			amount_silver, share_silver);
	}
	else if (share_silver == 0)
	{
		sprintf (buf, "$n splits %d gold coins. Your share is %d gold.",
			amount_gold, share_gold);
	}
	else
	{
		sprintf (buf, "$n splits %d silver and %d gold coins, "
			"giving you %d silver and %d gold.\n\r",
			amount_silver, amount_gold, share_silver, share_gold);
	}
	
	for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
	{
		if (gch != ch && is_same_group (gch, ch)
			&& !IS_AFFECTED (gch, _AFF_COMPULSION))
		{
			act (buf, ch, NULL, gch, TO_VICT, POS_DEAD);
			gch->gold += share_gold;
			gch->silver += share_silver;
		}
	}
	
	return;
}


/** Function: do_gtell
  * Descr   : Sends a "tell" to everyone in the tellers group.
  * Returns : void
  * Syntax  : gtell ( speaker, message )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_gtell)
{
	CHAR_DATA *gch;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Tell your group what?\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.penalties, _NOTELL)
		|| IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("Your message didn't get through!\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}
	
	chprintf (ch, "You tell your group '%s%s`w'\n\r", GTALKC, argument);
	
	for (gch = char_list; gch != NULL; gch = gch->next)
	{
		if (is_same_group (gch, ch) && gch != ch)
			chprintf (gch, "%s tells the group '%s%s`w'\n\r",
			PERS (ch, gch), GTALKC, argument);
	}
	
	return;
}

/** Function: do_tsay
  * Descr   : Sends a 'say' to everyone within a certain 'place' -vs- the
  *         : entire room.
  * Returns : void
  * Syntax  : tsay ( speaker, what to say )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_tsay)
{
	
	if (!ch->on || !IS_SET (ch->on->value[2], IS_PLACE))
	{
		send_to_char ("But your not in a specific place!\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("Say what?\n\r", ch);
		return;
	}
	
	act ("$n says (here) '$T'", ch, NULL, argument, TO_PLACE, POS_RESTING);
	act ("You say (here) '$T'", ch, NULL, argument, TO_CHAR, POS_RESTING);
	
}

/** Function: do_temote
  * Descr   : Sends an emote to a 'place' -vs- the entire room.
  * Returns : void
  * Syntax  : temote ( sender, emote argument )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_temote)
{
	
	if (!ch->on || !IS_SET (ch->on->value[2], IS_PLACE))
	{
		send_to_char ("But your not in a specific place!\n\r", ch);
		return;
	}
	
	if (!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _NOEMOTE))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_GAG_AIR))
	{
		send_to_char ("You can't open your mouth!\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_MUTE))
	{
		send_to_char ("You can't speak around the gash in your mouth!\n\r", ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char ("Emote what?\n\r", ch);
		return;
	}
	
	act ("$n $T", ch, NULL, argument, TO_PLACE, POS_RESTING);
	act ("$n $T", ch, NULL, argument, TO_CHAR, POS_RESTING);
	return;
}

/** Function: is_same_group
  * Descr   : Determines if ach, and bch are in the same group or not.
  *           
  * Returns : TRUE/FALSE
  * Syntax  : is_same_group(ch1, ch2)
  * Written : 
  * Author  : Rom Consortium
  * Note	: It is very important that this be an equivalence relation:
  *			: (1) A ~ A
  *			: (2) if A ~ B then B ~ A
  *			: (3) if A ~ B  and B ~ C, then A ~ C
  */
bool is_same_group (CHAR_DATA * ach, CHAR_DATA * bch)
{
	if (ach == NULL || bch == NULL)
		return FALSE;
	
	if (ach->leader != NULL)
		ach = ach->leader;
	if (bch->leader != NULL)
		bch = bch->leader;

	if ( ach == bch || is_linked(ach, bch) )
		return TRUE;
	else
		return FALSE;
}


char *makedrunk (char *string, CHAR_DATA * ch)
{
	/* This structure defines all changes for a character */
	struct struckdrunk drunk[] = {
		{3, 10,
		{"a", "a", "a", "A", "aa", "ah", "Ah", "ao", "aw", "oa", "ahhhh"}},
		{8, 5,
		{"b", "b", "b", "B", "B", "vb"}},
		{3, 5,
		{"c", "c", "C", "cj", "sj", "zj"}},
		{5, 2,
		{"d", "d", "D"}},
		{3, 3,
		{"e", "e", "eh", "E"}},
		{4, 5,
		{"f", "f", "ff", "fff", "fFf", "F"}},
		{8, 2,
		{"g", "g", "G"}},
		{9, 6,
		{"h", "h", "hh", "hhh", "Hhh", "HhH", "H"}},
		{7, 6,
		{"i", "i", "Iii", "ii", "iI", "Ii", "I"}},
		{9, 5,
		{"j", "j", "jj", "Jj", "jJ", "J"}},
		{7, 2,
		{"k", "k", "K"}},
		{3, 2,
		{"l", "l", "L"}},
		{5, 8,
		{"m", "m", "mm", "mmm", "mmmm", "mmmmm", "MmM", "mM", "M"}},
		{6, 6,
		{"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
		{6, 6,
		{"n", "n", "nn", "Nn", "nnn", "nNn", "N"}},
		{3, 6,
		{"o", "o", "ooo", "ao", "aOoo", "Ooo", "ooOo"}},
		{3, 2,
		{"p", "p", "P"}},
		{5, 5,
		{"q", "q", "Q", "ku", "ququ", "kukeleku"}},
		{4, 2,
		{"r", "r", "R"}},
		{2, 5,
		{"s", "ss", "zzZzssZ", "ZSssS", "sSzzsss", "sSss"}},
		{5, 2,
		{"t", "t", "T"}},
		{3, 6,
		{"u", "u", "uh", "Uh", "Uhuhhuh", "uhU", "uhhu"}},
		{4, 2,
		{"v", "v", "V"}},
		{4, 2,
		{"w", "w", "W"}},
		{5, 6,
		{"x", "x", "X", "ks", "iks", "kz", "xz"}},
		{3, 2,
		{"y", "y", "Y"}},
		{2, 9,
		{"z", "z", "ZzzZz", "Zzz", "Zsszzsz", "szz", "sZZz", "ZSz", "zZ", "Z"}}
	};
	
	char buf[1024];
	char temp;
	int pos = 0;
	int drunklevel;
	int randomnum;
	
	/* Check how drunk a person is... */
	if (IS_NPC (ch))
		drunklevel = 0;
	else
		drunklevel = ch->pcdata->condition[_DRUNK];
	
	if (drunklevel > 0)
	{
		do
		{
			temp = UPPER (*string);
			if ((temp >= 'A') && (temp <= 'Z'))
			{
				if (drunklevel > drunk[temp - 'A'].min_drunk_level)
				{
					randomnum = number_range (0, drunk[temp - 'A'].number_of_rep);
					strcpy (&buf[pos], drunk[temp - 'A'].replacement[randomnum]);
					pos += strlen (drunk[temp - 'A'].replacement[randomnum]);
				}
				else
					buf[pos++] = *string;
			}
			else
			{
				if ((temp >= '0') && (temp <= '9'))
				{
					temp = '0' + number_range (0, 9);
					buf[pos++] = temp;
				}
				else
					buf[pos++] = *string;
			}
		}
		while (*string++);
		buf[pos] = '\0';		/* Mark end of the string... */
		strcpy (string, buf);
		return (string);
	}
	return (string);
}


/** Function: do_ignore
  * Descr   : Toggles ignore status for individuals on/off
  *         : 
  * Returns : void
  * Syntax  : ignore <who>
  * Written : v1.0 ?
  * Author  : Dominic J. Eidson
  *         : 
  */
_DOFUN (do_ignore)
{
	CHAR_DATA *rch;
	char arg[MIL], buf[MSL];
	DESCRIPTOR_DATA *d;
	int pos;
	bool found = FALSE;
	
	if (ch->desc == NULL)
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;
	
	if (IS_NPC (rch))
		return;
	
	smash_tilde (argument);
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		if (rch->pcdata->ignore[0] == NULL)
		{
			send_to_char ("You are not ignoring anyone.\n\r", ch);
			return;
		}
		send_to_char ("You are currently ignoring:\n\r", ch);
		
		for (pos = 0; pos < MAX_IGNORE; pos++)
		{
			if (rch->pcdata->ignore[pos] == NULL)
				break;
			
			sprintf (buf, "    %s\n\r", rch->pcdata->ignore[pos]);
			send_to_char (buf, ch);
		}
		return;
	}
	
	for (pos = 0; pos < MAX_IGNORE; pos++)
	{
		if (rch->pcdata->ignore[pos] == NULL)
			break;
		
		if (!str_cmp (arg, rch->pcdata->ignore[pos]))
		{
			send_to_char ("Ignore removed.\n\r", ch);
			free_string (rch->pcdata->ignore[pos]);
			rch->pcdata->ignore[pos] = NULL;
			found = TRUE;
			return;
		}
	}
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *wch;
		
		if (d->connected != CON_PLAYING || !can_see (ch, d->character))
			continue;
		
		wch = (d->original != NULL) ? d->original : d->character;
		
		if (!can_see (ch, wch))
			continue;
		
		if (!str_cmp (arg, wch->name))
		{
			found = TRUE;
			if (wch == ch)
			{
				send_to_char ("You ignore yourself for a moment, but it passes.\n\r",
					ch);
				return;
			}
			if (wch->level >= LEVEL_IMMORTAL)
			{
				send_to_char ("That person is very hard to ignore.\n\r", ch);
				return;
			}
		}
	}
	
	if (!found)
	{
		send_to_char ("No one by that name is playing.\n\r", ch);
		return;
	}
	
	for (pos = 0; pos < MAX_IGNORE; pos++)
	{
		if (rch->pcdata->ignore[pos] == NULL)
			break;
	}
	
	if (pos >= MAX_IGNORE)
	{
		send_to_char ("Sorry, you have reached the ignore limit.\n\r", ch);
		return;
	}
	
	/* make a new ignore */
	rch->pcdata->ignore[pos] = str_dup (arg);
	sprintf (buf, "You are now ignoring %s.\n\r", arg);
	send_to_char (buf, ch);
	return;
}
