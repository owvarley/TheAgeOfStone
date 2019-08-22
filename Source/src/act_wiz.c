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
 * @@ Objective (act_wiz.c)
 *
 * This file contains non-building specific immortal commands.
 */


#if !defined(WIN32)
  #include <sys/time.h>
  #include <unistd.h>

   
#else
  #include <winsock.h>
  #include <process.h>
  #include <io.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "merc.h"
#include "recycle.h"



_DOFUN (do_mudcfg)
{
	char arg1[MIL], arg2[MIL], arg3[MIL];

	memset(arg1,0,MIL); memset(arg2, 0, MIL); memset(arg3, 0, MIL); 

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if ( IS_NULLSTR(arg1) || !str_prefix(arg1, "show") || get_trust(ch) < ML-2 )
	{
		int i;

		send_to_char("Current Configuration---\n\r\n\r", ch);

		chprintf(ch, "Mud Name                   %s\n\r", pMud.mud_name);
		chprintf(ch, "Version                    %s\n\r", versionToString(pMud.version));
		chprintf(ch, "Welcome Message            %s\n\r", pMud.welcome_message);
		chprintf(ch, "Base Sound URL             %s\n\r", pMud.BaseSoundURL);
		chprintf(ch, "RedirectURl                %s\n\r", pMud.RedirectURL);
		chprintf(ch, "Redirect Message           %s\n\r", pMud.RedirectMSG);
		chprintf(ch, "\n\r--In Game Settings\n\r\n\r");
		chprintf(ch, "Courageous Level           %d\n\r", pMud.courageous);
		chprintf(ch, "Forced no Peace level      %d\n\r", pMud.level_no_peace);
		chprintf(ch, "PK Range Limit             %d\n\r", pMud.pk_limit);
		chprintf(ch, "Box Rental cost            %d\n\r", pMud.box_cost);
		chprintf(ch, "Bank share value           %d\n\r", pMud.share_value);
		chprintf(ch, "Wait Timer                 %d\n\r", pMud.WaitTimer);
		chprintf(ch, "Violence Timer             %d\n\r", pMud.ViolenceTimer);
		chprintf(ch, "Experience Modifier        %1.2f\n\r", pMud.exp_mod);
		chprintf(ch, "\n\r--Creation Specific Settings\n\r\n\r");
		chprintf(ch, "Max Creation Points        %d\n\r", pMud.MaxCreationPoints);
		chprintf(ch, "Max Talents                %d\n\r", pMud.MaxPCTalents);
		chprintf(ch, "Max Specialties            %d\n\r", pMud.MaxPCSpecialties);
		chprintf(ch, "\n\r--Flag Values\n\r\n\r");

		for (i=0; _system_bits[i].name != NULL; i++)
		{
			chprintf(ch, "%-25.25s [%s]\n\r",
					_system_bits[i].name, 
					IS_FLAG(pMud.config, _system_bits[i].bit) ? "On " : "Off");
		}
		return;
	}

	if ( !str_prefix(arg1, "mudname") )
	{
		if ( IS_NULLSTR(arg2) )
		{
			chprintf(ch, "Invalid argument.\n\r");
			return;
		}
		if ( !str_prefix(arg2, "delete") )
		{
			free_string(pMud.mud_name);
			pMud.mud_name = str_dup("");
			chprintf(ch, "Mud name deleted.\n\r");
			return;
		}
		
		free_string(pMud.mud_name);
		pMud.mud_name = str_dup(arg2);
		chprintf(ch, "Mud name changed to: %s\n\r", pMud.mud_name);
		return;
	}
	else
	if ( !str_prefix(arg1, "welcomemsg") )
	{
		if ( IS_NULLSTR(arg2) )
		{
			chprintf(ch, "Invalid argument.\n\r");
			return;
		}
		if ( !str_prefix(arg2, "delete") )
		{
			free_string(pMud.welcome_message);
			pMud.welcome_message = str_dup("");
			chprintf(ch, "Welcome message deleted.\n\r");
			return;
		}
		
		free_string(pMud.welcome_message);
		pMud.welcome_message = str_dup(arg2);
		chprintf(ch, "Welcome message changed to: %s\n\r", pMud.welcome_message);
		return;
	}
	else
	if ( !str_prefix(arg1, "basesoundurl") )
	{
		if ( IS_NULLSTR(arg2) )
		{
			chprintf(ch, "Invalid argument.\n\r");
			return;
		}
		if ( !str_prefix(arg2, "delete") )
		{
			free_string(pMud.BaseSoundURL);
			pMud.BaseSoundURL = str_dup("");
			chprintf(ch, "Base Sound URL deleted.\n\r");
			return;
		}
		
		free_string(pMud.BaseSoundURL);
		pMud.BaseSoundURL = str_dup(arg2);
		chprintf(ch, "Base Sound URL changed to: %s\n\r", pMud.BaseSoundURL);
		return;
	}
	else
	if ( !str_prefix(arg1, "redirecturl") )
	{
		if ( IS_NULLSTR(arg2) )
		{
			chprintf(ch, "Invalid argument.\n\r");
			return;
		}
		if ( !str_prefix(arg2, "delete") )
		{
			free_string(pMud.RedirectURL);
			pMud.RedirectURL = str_dup("");
			chprintf(ch, "Redirect URL deleted.\n\r");
			return;
		}
		
		free_string(pMud.RedirectURL);
		pMud.RedirectURL = str_dup(arg2);
		chprintf(ch, "Redirect URL changed to: %s\n\r", pMud.RedirectURL);
		return;
	}
	else
	if ( !str_prefix(arg1, "redirectmsg") )
	{
		if ( IS_NULLSTR(arg2) )
		{
			chprintf(ch, "Invalid argument.\n\r");
			return;
		}
		if ( !str_prefix(arg2, "delete") )
		{
			free_string(pMud.RedirectMSG);
			pMud.RedirectMSG = str_dup("");
			chprintf(ch, "Redirect Message deleted.\n\r");
			return;
		}
		
		free_string(pMud.RedirectMSG);
		pMud.RedirectMSG = str_dup(arg2);
		chprintf(ch, "Redirect message changed to: %s\n\r", pMud.RedirectMSG);
		return;
	}
	else
	if ( !str_prefix(arg1, "courageous") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > LEVEL_HERO-1)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-%d\n\r", LEVEL_HERO-1);
			return;
		}

		pMud.courageous = iValue;
		chprintf(ch, "Courageous level set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "nopeace") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > LEVEL_HERO-1)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-%d\n\r", LEVEL_HERO-1);
			return;
		}

		pMud.level_no_peace = iValue;
		chprintf(ch, "Level peace is forced off set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "pklimit") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > LEVEL_HERO-1)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-%d\n\r", LEVEL_HERO-1);
			return;
		}

		pMud.pk_limit = iValue;
		chprintf(ch, "PK limit level range set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "boxcost") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 5000)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-5000\n\r");
			return;
		}

		pMud.box_cost = iValue;
		chprintf(ch, "Box rental cost set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "sharevalue") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 5000)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-5000\n\r");
			return;
		}

		pMud.share_value = iValue;
		chprintf(ch, "Share value set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "waittimer") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 50)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-50\n\r");
			return;
		}

		pMud.WaitTimer = iValue;
		chprintf(ch, "Wait timer set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "violencetimer") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 50)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-50\n\r");
			return;
		}

		pMud.ViolenceTimer = iValue;
		chprintf(ch, "Violence timer set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "maxcreationpoints") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 500)))
		{
			chprintf(ch, "Invalid argument. Valid range is 0-500\n\r");
			return;
		}

		pMud.MaxCreationPoints = iValue;
		chprintf(ch, "Max Creation Points set to %d\n\r", iValue);

		return;
	}
	if ( !str_prefix(arg1, "maxtalents") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 50)))
		{
			chprintf(ch, "Invalid argument. Valid Range is 0-50\n\r");
			return;
		}

		pMud.MaxPCTalents = iValue;
		chprintf(ch, "Max Talents set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "maxspecialties") )
	{
		int iValue = -1;

		if ( IS_NULLSTR(arg2) || ((( iValue = atoi(arg2) ) < 0) || (iValue > 50)))
		{
			chprintf(ch, "Invalid argument. Valid Range is 0-50\n\r");
			return;
		}

		pMud.MaxPCSpecialties = iValue;
		chprintf(ch, "Max Specialties set to %d\n\r", iValue);

		return;
	}
	else
	if ( !str_prefix(arg1, "expmod") )
	{
		float fValue = (float)atof(arg2);

		if ( fValue <= 0.50 )
		{
			chprintf(ch, "Invalid value. Range is 0.5 to 3.0\n\r");
			return;
		}

		pMud.exp_mod = fValue;
		chprintf(ch, "Experience Modifier set to %2.2f", pMud.exp_mod);
		return;
	}

	else
	if ( !str_prefix(arg1, "flag") )
	{
		int iValue = -1;
		
		if ( !IS_NULLSTR(arg2) )
			iValue = flag_lookup(arg2, _system_bits);

		if ( iValue < 1 )
		{
			chprintf(ch, "Invalid flag.  Valid choices are:\n\r\n\r");
			show_flag_cmds(ch, _system_bits);
			return;
		}

		if ( IS_FLAG(pMud.config, iValue) )
			REMOVE_FLAG(pMud.config, iValue);
	    else
			SET_FLAG(pMud.config, iValue);

		chprintf(ch, "Flag set.\n\r");
		return;
	}

	chprintf(ch, "Syntax: mudcfg <option> <value>.  Valid options are;\n\r\n\r"
		         "mudname           -- Set the name of the mud.\n\r"
				 "welcomemessage    -- Set the welcome message seen when loggin in.\n\r"
				 "basesoundurl      -- Set the Base URL where your downloadable sound files are located.\n\r"
				 "redirecturl       -- Set the URL to redirect players to when the mud is moved.\n\r"
				 "redirectmsg       -- Set the message to show to players when the mud has been moved.\n\r"
				 "courageous        -- Set the minimum level a player needs to be to turn off their 'peaceful' flag.\n\r"
				 "nopeace           -- Set the level when a players 'peaceful' flag is forced off.\n\r"
				 "pklimit           -- This sets the range of levels that 2 players must be within to pk.\n\r"
				 "boxcost           -- Set the cost to rent a storage box.\n\r"
				 "sharevalue        -- Set the current share value of the investment system.\n\r"
				 "waittimer         -- Set the amount of time (in ticks) mortally wounded players will wait before moved to the healers.\n\r"
				 "violencetimer     -- Currently unused.\n\r"
				 "maxcreationpoints -- Set a cap on the maximum number of creation points that may be taken during creation.\n\r"
				 "maxtalents        -- Set a cap on the maximum number of talents a player may choose during creation.\n\r"
				 "maxspecialties    -- Set a cap on the maximum number of specialties a player may choose during creation.\n\r"
				 "flag <option>     -- Toggles a named flags status. Pass the name of the flag as the 'option' to toggle.\n\r\n\r");

}


_DOFUN (do_flag)
{
	const struct flag_type *table = NULL;
	char what[MIL];
	char which[MIL];
	char flag[MIL];
	
	memset (what, 0, MIL);
	memset (which, 0, MIL);
	memset (flag, 0, MIL);
	
	argument = one_argument (argument, what);
	argument = one_argument (argument, which);
	argument = one_argument (argument, flag);
	
	
	if (!str_prefix (what, "mobile") || !str_prefix (what, "char"))
	{
		CHAR_DATA *Who = NULL;
		int iset = 0, value = 0;
		char bit[MIL];
		
		memset (bit, 0, MIL);
		
		if ((Who = get_char_world (ch, which)) == NULL)
		{
			send_to_char ("Noone like that found in heaven or earth.\n\r", ch);
			return;
		}
		
		if (!str_prefix (flag, "communications"))
		{
			iset = 1;
			table = _comm_bits;
		}
		else if (!str_prefix (flag, "actions"))
		{
			iset = 2;
			table = _act_bits;
		}
		else if (!str_prefix (flag, "penalties"))
		{
			iset = 3;
			table = _penalty_bits;
		}
		else if (!str_prefix (flag, "configuration"))
		{
			iset = 4;
			table = _config_bits;
		}
		else if (!str_prefix (flag, "wiznet"))
		{
			iset = 5;
			table = _wiznet_bits;
		}
		else if (!str_prefix (flag, "form"))
		{
			iset = 6;
			table = _form_bits;
		}
		else if (!str_prefix (flag, "parts"))
		{
			iset = 7;
			table = _part_bits;
		}
		else if (!str_prefix (flag, "off"))
		{
			iset = 8;
			table = _off_bits;
		}
		else if (!str_prefix (flag, "resistances"))
		{
			iset = 9;
			table = _resistance_bits;
		}
		else if (!str_prefix (flag, "immunities"))
		{
			iset = 10;
			table = _resistance_bits;
		}
		else if (!str_prefix (flag, "vulnerabilities"))
		{
			iset = 11;
			table = _resistance_bits;
		}
		else if (!str_prefix (flag, "affected"))
		{
			iset = 12;
			table = _affected_by_bits;
		}
		else
		{
			send_to_char
				("Syntax: flag <obj|mob|room> <flagtype> <which flag(s)>\n\r", ch);
			send_to_char ("Valid flagtypes are:\n\r"
				"object flag types  : extra, wear\n\r\n\r"
				"room flag types    : room\n\r\n\r"
				"mob/char flag types: act config comm penalties wiznet aff\n\r"
				"                     form parts off imm res vuln\n\r\n\r", ch);
			return;
		}
		
		/* Ok, We have a valid table, but no bit(s) to set... 
		do they need a list of bits? */
		if (IS_NULLSTR (argument))
		{
			show_flag_cmds (ch, table);
			return;
		}
		
		while (!IS_NULLSTR (argument))
		{
			argument = one_argument (argument, bit);
			
			if ((value = flag_lookup (bit, table)) < 0)
				chprintf (ch, "Invalid bit. [%s]\n\r", bit);
			else
			{
				switch (iset)
				{
					case 1:
						if (!IS_FLAG (Who->states.comm, value))
							SET_FLAG (Who->states.comm, value);
						else
							REMOVE_FLAG (Who->states.comm, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 2:
						if (!IS_FLAG (Who->states.player, value))
							SET_FLAG (Who->states.player, value);
						else
							REMOVE_FLAG (Who->states.player, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 3:
						if (!IS_FLAG (Who->states.penalties, value))
							SET_FLAG (Who->states.comm, value);
						else
							REMOVE_FLAG (Who->states.penalties, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 4:
						if (!IS_FLAG (Who->states.config, value))
							SET_FLAG (Who->states.config, value);
						else
							REMOVE_FLAG (Who->states.config, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 5:
						if (!IS_FLAG (Who->states.wiznet, value))
							SET_FLAG (Who->states.wiznet, value);
						else
							REMOVE_FLAG (Who->states.wiznet, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 6:
						if (!IS_FLAG (Who->states.form, value))
							SET_FLAG (Who->states.form, value);
						else
							REMOVE_FLAG (Who->states.form, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 7:
						if (!IS_FLAG (Who->states.parts, value))
							SET_FLAG (Who->states.parts, value);
						else
							REMOVE_FLAG (Who->states.parts, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 8:
						if (!IS_FLAG (Who->states.off, value))
							SET_FLAG (Who->states.off, value);
						else
							REMOVE_FLAG (Who->states.off, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 9:
						if (!IS_FLAG (Who->states.res, value))
							SET_FLAG (Who->states.res, value);
						else
							REMOVE_FLAG (Who->states.res, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 10:
						if (!IS_FLAG (Who->states.imm, value))
							SET_FLAG (Who->states.imm, value);
						else
							REMOVE_FLAG (Who->states.imm, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 11:
						if (!IS_FLAG (Who->states.vuln, value))
							SET_FLAG (Who->states.vuln, value);
						else
							REMOVE_FLAG (Who->states.vuln, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
					case 12:
						if (!IS_FLAG (Who->states.affected_by, value))
							SET_FLAG (Who->states.affected_by, value);
						else
							REMOVE_FLAG (Who->states.affected_by, value);
						
						chprintf (ch, "[%s] flag set.\n\r", bit);
						break;
				}
			}
    }
	
    return;
  }
  else if (!str_prefix (what, "object"))
  {
	  OBJ_DATA *Obj = NULL;
	  
	  if ((Obj = get_obj_world (ch, which)) == NULL)
	  {
		  send_to_char ("Nothing like that found in heaven or earth.\n\r", ch);
		  return;
	  }
	  
	  if (!str_prefix (flag, "extra"))
		  table = extra_flags;
	  else if (!str_prefix (flag, "wear"))
		  table = wear_flags;
	  else
	  {
		  send_to_char ("Invalid flag type.\n\r", ch);
		  return;
	  }
	  
	  /* Ok, We have a valid table, but no bit(s) to set... 
	  do they need a list of bits? */
	  if (IS_NULLSTR (argument) && table)
	  {
		  show_flag_cmds (ch, table);
		  return;
	  }
	  
	  if (table != NULL && !IS_NULLSTR (argument))
	  {
		  long value;
		  
		  if ((value = flag_value (table, argument)) != NO_FLAG)
		  {
			  if (table == wear_flags)
				  TOGGLE_BIT (Obj->wear_flags, value);
			  else
				  TOGGLE_BIT (Obj->extra_flags, value);
			  
			  chprintf (ch, "%s flags set.\n\r",
				  (table == wear_flags) ? "Wear" : "Extra");
			  
			  return;
		  }
	  }
  }
  else if (!str_prefix (what, "room"))
  {
	  ROOM_INDEX_DATA *Room = NULL;
	  
	  if ((Room = find_location (ch, which)) == NULL)
	  {
		  send_to_char ("Nowhere like that found in heaven or earth.\n\r", ch);
		  return;
	  }
	  
	  if (!str_prefix (flag, "room"))
		  table = room_flags;
	  
	  
	  /* Ok, We have a valid table, but no bit(s) to set... 
	  do they need a list of bits? */
	  if (IS_NULLSTR (argument) && table)
	  {
		  show_flag_cmds (ch, table);
		  return;
	  }
	  
	  if (table != NULL && !IS_NULLSTR (argument))
	  {
		  long value;
		  
		  if ((value = flag_value (table, argument)) != NO_FLAG)
		  {
			  TOGGLE_BIT (Room->room_flags, value);
			  
			  chprintf (ch, "Room flags set.\n\r");
			  return;
		  }
	  }
  }
  
  send_to_char ("Syntax: flag <obj|mob|room> <flagtype> <which flag(s)>\n\r", ch);
  send_to_char ("Valid flagtypes are:\n\r"
	  "object flag types  : extra, wear\n\r\n\r"
	  "room flag types    : room\n\r\n\r"
	  "mob/char flag types: act config comm penalties wiznet aff\n\r"
	  "                     form parts off imm res vuln\n\r\n\r", ch);
  return;
}

_DOFUN (do_disable)
{
	int cmd = 0;
	bool found = FALSE;
	
	if (IS_NULLSTR (argument))
	{
		int count = 0;
		
		for (cmd = 0; !IS_NULLSTR (cmd_table[cmd].name); cmd++)
			if (IS_SET (cmd_table[cmd].flags, CMD_DISABLED))
			{
				if (count++ == 0)
					send_to_char ("The following commands are disabled...\n\r\n\r", ch);
				
				chprintf (ch, "%s\n\r", cmd_table[cmd].name);
			}
			
			chprintf (ch, "\n\r%d commands are disabled.\n\r", count);
			return;
	}
	
	cmd = 0;
	while (cmd_table[cmd].name && cmd_table[cmd].name[0] != '\0')
	{
		if (!str_cmp (argument, cmd_table[cmd].name))
		{
			found = TRUE;
			break;
		}
		
		cmd++;
	}
	
	if (found)
	{
		if (!IS_SET (cmd_table[cmd].flags, CMD_DISABLED))
		{
			SET_BIT (cmd_table[cmd].flags, CMD_DISABLED);
			chprintf (ch, "[%s] command disabled.\n\r", cmd_table[cmd].name);
			return;
		}
		else
		{
			REMOVE_BIT (cmd_table[cmd].flags, CMD_DISABLED);
			chprintf (ch, "[%s] command enabled.\n\r", cmd_table[cmd].name);
			return;
		}
	}
	
	chprintf (ch, "[%s] command not found.\n\r", argument);
}

/** Function: do_odata
  * Descr   : Output a complete listing of all objects reset onto the mud
  *         : to a data file suitable for importing to a spreadsheet.
  * Returns : void
  * Syntax  : do_odata ()
  * Written : v1.0 05/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_odata)
{
	const char delim = '|';
	FILE *fp;
	char buff[MSL];
	OBJ_INDEX_DATA *pObj;
	long vnum; 
	int nMatch;
	
	sprintf (buff, "%s%s", DATA_DIR, "objects.out");
	/* Open the data file for appending */
	if (!(fp = fopen (buff, "w+")))
	{
		logf ("[%s] Could not open file %s in order to save object data.",
			errmsg (errno), buff);
		send_to_char ("Unable to open output file. Aborting.\n\r", ch);
		return;
	}
	
	nMatch = 0;
	
	/* Loop through all objects, reset or not */
	for (vnum = 0; nMatch < top_obj_index; vnum++)
	{
		if ((pObj = get_obj_index (vnum)) != NULL)
		{
			
			nMatch++;
			
			fprintf (fp, "%d%c%ld%c%d%c%d%c",
				pObj->vnum, delim,
				pObj->area->vnum, delim, 
				pObj->level, delim,
				pobj_eq_cost(pObj), delim);

			
			fprintf (fp, "%s%c%s%c%s%c",
				strip_codes (pObj->short_descr), delim,
				flag_string (extra_flags, pObj->extra_flags), delim,
				flag_string (wear_flags, pObj->wear_flags), delim);
			
			fprintf (fp, "%d%c%d%c%d%c",
				pObj->condition, delim,
				pObj->weight, delim, pObj->cost, delim);
			
			fprintf (fp, "%s%c", flag_string (type_flags, pObj->item_type), delim);
			
			fprintf (fp, "%d%c%d%c%d%c%d%c%d\n",
				pObj->value[0], delim,
				pObj->value[1], delim,
				pObj->value[2], delim, pObj->value[3], delim, pObj->value[4]);
			
		}
	}
	
	fclose (fp);
	send_to_char ("Compiling complete.\n\r", ch);
	return;
}


/** Function: do_state
  * Descr   : Displays current mud state/stats
  * Returns : output only
  * Syntax  : (n/a)
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_state)
{
	char *time_str = ctime (&pMud.last_cover);
	
	/* ctime appends a \n to the end of the string.. delete that */
	time_str[strlen (time_str) - 1] = '\0';
	
	chprintf (ch,
		"Mud data file version              : %d\n\r"
		"Total # of logins                  : %ld\n\r"
		"Total # of new characters          : %ld\n\r"
		"Total # of new players             : %ld\n\r"
		"Total # of deletions by players    : %d\n\r"
		"Total # of deletions by immortal   : %d\n\r"
		"Total # of banned names            : %d\n\r"
		"Time/date of last copyover         : %s\n\r"
		"Active states of Mud               : %s\n\r",
		pMud.version,
		pMud.logins,
		pMud.newchars,
		pMud.newbies,
		pMud.deletions, pMud.unlinks, top_name, time_str,
		/* (char *)ctime( &pMud.last_cover ), */
		flag_string (mud_flags, pMud.flags));
	
}

/** Function: display_poof
  * Descr   : Display's poofin/poofout information to the room, dependent
  *         : on argument "arg" (in/out)
  * Returns : (void)
  * Syntax  : display_poof( who, [in|out] )
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void display_poof (CHAR_DATA * ch, char *arg)
{
	CHAR_DATA *vch, *next_vch;
	
	if (!IS_IMMORTAL (ch) || IS_NPC (ch) || IS_SWITCHED (ch)
		|| ch->in_room == NULL)
		return;
	
	for (vch = ch->in_room->people; vch != NULL; vch = next_vch)
	{
		next_vch = vch->next_in_room;
		
		if (vch == ch || !can_see (vch, ch))
			continue;
		
		if (!str_cmp (arg, "in"))
		{
			if (ch->pcdata->bamfin[0] != '\0')
				act ("$t", ch, ch->pcdata->bamfin, vch, TO_VICT, POS_RESTING);
			else
				act ("$n steps out of a hole in the Pattern.",
				ch, NULL, vch, TO_VICT, POS_RESTING);
			
		}
		else
		{
			if (ch->pcdata->bamfout[0] != '\0')
				act ("$t", ch, ch->pcdata->bamfout, vch, TO_VICT, POS_RESTING);
			else
				act ("$n rips a hole in the Pattern and steps through"
				", closing it behind them.",
				ch, NULL, vch, TO_VICT, POS_RESTING);
			
		}
		
	}
	
	return;
}


_DOFUN (do_wiznet)
{
	int flag;
	char buf[MSL];
	
	if (argument[0] == '\0')
		/* Show wiznet options - just like channel command */
	{
		send_to_char ("WELCOME TO WIZNET!!!\n\r", ch);
		send_to_char ("   Option      Status\n\r", ch);
		send_to_char ("---------------------\n\r", ch);
		/* list of all wiznet options */
		buf[0] = '\0';
		
		for (flag = 0; wiznet_table[flag].name != NULL; flag++)
		{
			if (wiznet_table[flag].level <= get_trust (ch))
			{
				chprintf (ch, "%-9s %s%s", wiznet_table[flag].name,
					IS_FLAG (ch->states.wiznet,
					wiznet_table[flag].flag) ? "ON " : "OFF",
					((flag + 1) % 4 == 0) ? "\n\r" : "\t");
			}
		}
		
		return;
	}
	
	if (!str_prefix (argument, "on"))
	{
		send_to_char ("Welcome to Wiznet!\n\r", ch);
		SET_FLAG (ch->states.wiznet, WIZ_ON);
		return;
	}
	
	if (!str_prefix (argument, "off"))
	{
		send_to_char ("Signing off of Wiznet.\n\r", ch);
		REMOVE_FLAG (ch->states.wiznet, WIZ_ON);
		return;
	}
	
	flag = wiznet_lookup (argument);
	
	if (flag == -1 || get_trust (ch) < wiznet_table[flag].level)
	{
		send_to_char ("No such option.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.wiznet, wiznet_table[flag].flag))
	{
		sprintf (buf, "You will no longer see %s on wiznet.\n\r",
			wiznet_table[flag].name);
		send_to_char (buf, ch);
		REMOVE_FLAG (ch->states.wiznet, wiznet_table[flag].flag);
		return;
	}
	else
	{
		sprintf (buf, "You will now see %s on wiznet.\n\r",
			wiznet_table[flag].name);
		send_to_char (buf, ch);
		SET_FLAG (ch->states.wiznet, wiznet_table[flag].flag);
		return;
	}
}



void wiznet (char *string, CHAR_DATA * ch, OBJ_DATA * obj,
			 long flag, long flag_skip, int min_level)
{
	DESCRIPTOR_DATA *d;
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& IS_IMMORTAL (d->character)
			&& IS_FLAG (d->character->states.wiznet, WIZ_ON)
			&& (!flag || IS_FLAG (d->character->states.wiznet, flag))
			&& (!flag_skip || !IS_FLAG (d->character->states.wiznet, flag_skip))
			&& get_trust (d->character) >= min_level && d->character != ch)
		{
			if (IS_FLAG (d->character->states.wiznet, WIZ_PREFIX))
				send_to_char ("--> ", d->character);
			act (string, d->character, obj, ch, TO_CHAR, POS_DEAD);
		}
	}
	
	return;
}


/* equips a character */
_DOFUN (do_outfit)
{
	OBJ_DATA *obj;
	int i, sn;
	long vnum;
	
	if (ch->level > 5 || IS_NPC (ch))
	{
		send_to_char ("Find it yourself!\n\r", ch);
		return;
	}

	if ( (obj = create_object( get_obj_index(OBJ_VNUM_MAP), 0)) != NULL )
      obj_to_char (obj, ch);

	if ((obj = get_eq_char (ch, WEAR_LIGHT)) == NULL)
	{
		obj = create_object (get_obj_index (OBJ_VNUM_SCHOOL_BANNER), 0);
		if (obj)
		{
			obj->cost = 0;
			obj_to_char (obj, ch);
			equip_char (ch, obj, WEAR_LIGHT);
		}
	}
	
	if ((obj = get_eq_char (ch, WEAR_BODY)) == NULL)
	{
		obj = create_object (get_obj_index (OBJ_VNUM_SCHOOL_VEST), 0);
		if (obj)
		{
			obj->cost = 0;
			obj_to_char (obj, ch);
			equip_char (ch, obj, WEAR_BODY);
		}
	}
	
	/* do the weapon thing */
	if ((obj = get_eq_char (ch, WEAR_WIELD)) == NULL)
	{
		vnum = OBJ_VNUM_SCHOOL_SWORD;	/* just in case! */
		
		for (i = 0; weapon_table[i].name != NULL; i++)
		{
			sn = skill_lookup(weapon_table[i].name);

			if (IS_VALID_SKILL(sn) && ch->pcdata->learned[sn] > 1)
			{
				vnum = weapon_table[i].vnum;
				break;
			}
		}
		
		obj = create_object (get_obj_index (vnum), 0);
		if (obj)
		{
			obj_to_char (obj, ch);
			equip_char (ch, obj, WEAR_WIELD);
		}
	}
	
	if (((obj = get_eq_char (ch, WEAR_WIELD)) == NULL
		|| !IS_WEAPON_STAT (obj, WEAPON_TWO_HANDS))
		&& (obj = get_eq_char (ch, WEAR_SHIELD)) == NULL)
	{
		obj = create_object (get_obj_index (OBJ_VNUM_SCHOOL_SHIELD), 0);
		if (obj)
		{
			obj->cost = 0;
			obj_to_char (obj, ch);
			equip_char (ch, obj, WEAR_SHIELD);
		}
	}
	
	send_to_char ("You have been equipped by the Creator.\n\r", ch);
}


/* RT nochannels command, for those spammers */
_DOFUN (do_nochannels)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Nochannel whom?", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.penalties, _NOCHANNELS))
	{
		REMOVE_FLAG (victim->states.penalties, _NOCHANNELS);
		send_to_char ("The gods have restored your channel priviliges.\n\r",
			victim);
		send_to_char ("NOCHANNELS removed.\n\r", ch);
		sprintf (buf, "$N restores channels to %s", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _NOCHANNELS);
		send_to_char ("The gods have revoked your channel priviliges.\n\r",
			victim);
		send_to_char ("NOCHANNELS set.\n\r", ch);
		sprintf (buf, "$N revokes %s's channels.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	
	return;
}


_DOFUN (do_smote)
{
	CHAR_DATA *vch;
	char *letter, *name;
	char last[MIL], temp[MSL];
	int matches = 0;
	
	if (!IS_NPC (ch) && IS_FLAG (ch->states.penalties, _NOEMOTE))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("Emote what?\n\r", ch);
		return;
	}
	
	if (strstr (argument, ch->name) == NULL)
	{
		send_to_char ("You must include your name in an smote.\n\r", ch);
		return;
	}
	
	send_to_char (argument, ch);
	send_to_char ("\n\r", ch);
	
	for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
	{
		if (vch->desc == NULL || vch == ch)
			continue;
		
		if ((letter = strstr (argument, vch->name)) == NULL)
		{
			send_to_char (argument, vch);
			send_to_char ("\n\r", vch);
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
		
		send_to_char (temp, vch);
		send_to_char ("\n\r", vch);
	}
	
	return;
}

_DOFUN (do_bamfin)
{
	char buf[MSL];
	
	if (!IS_NPC (ch))
	{
		smash_tilde (argument);
		
		if (argument[0] == '\0')
		{
			sprintf (buf, "Your poofin is %s\n\r", ch->pcdata->bamfin);
			send_to_char (buf, ch);
			return;
		}
		
		if (strstr (strip_codes (argument), ch->name) == NULL)
		{
			send_to_char ("You must include your name.\n\r", ch);
			return;
		}
		
		free_string (ch->pcdata->bamfin);
		ch->pcdata->bamfin = str_dup (argument);
		
		sprintf (buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin);
		send_to_char (buf, ch);
	}
	return;
}



_DOFUN (do_bamfout)
{
	char buf[MSL];
	
	if (!IS_NPC (ch))
	{
		smash_tilde (argument);
		
		if (argument[0] == '\0')
		{
			sprintf (buf, "Your poofout is %s\n\r", ch->pcdata->bamfout);
			send_to_char (buf, ch);
			return;
		}
		
		if (strstr (strip_codes (argument), ch->name) == NULL)
		{
			send_to_char ("You must include your name.\n\r", ch);
			return;
		}
		
		free_string (ch->pcdata->bamfout);
		ch->pcdata->bamfout = str_dup (argument);
		
		sprintf (buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout);
		send_to_char (buf, ch);
	}
	return;
}



_DOFUN (do_deny)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Deny whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	SET_FLAG (victim->states.penalties, _DENY);
	send_to_char ("You are denied access!\n\r", victim);
	sprintf (buf, "$N denies access to %s", victim->name);
	wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	send_to_char ("OK.\n\r", ch);
	save_char_obj (victim);
	stop_fighting (victim, TRUE);
	do_quit (victim, "");
	
	return;
}



_DOFUN (do_disconnect)
{
	char arg[MIL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Disconnect whom?\n\r", ch);
		return;
	}
	
	if (is_number (arg))
	{
		int desc;
		
		desc = atoi (arg);
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->descriptor == desc)
			{
				close_descriptor (d);
				send_to_char ("Ok.\n\r", ch);
				return;
			}
		}
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (victim->desc == NULL)
	{
		act ("$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR,
			POS_DEAD);
		return;
	}
	
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d == victim->desc)
		{
			close_descriptor (d);
			send_to_char ("Ok.\n\r", ch);
			return;
		}
	}
	
	bugf ("[act_wiz.c::do_disconnect] desc not found.");
	send_to_char ("Descriptor not found!\n\r", ch);
	return;
}

_DOFUN (do_echo)
{
	DESCRIPTOR_DATA *d;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Global echo what?\n\r", ch);
		return;
	}
	
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING)
		{
			if (get_trust (d->character) >= get_trust (ch))
				send_to_char ("global> ", d->character);
			send_to_char (argument, d->character);
			send_to_char ("\n\r", d->character);
		}
	}
	
	return;
}



_DOFUN (do_recho)
{
	DESCRIPTOR_DATA *d;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Local echo what?\n\r", ch);
		
		return;
	}
	
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING && d->character->in_room == ch->in_room)
		{
			if (get_trust (d->character) >= get_trust (ch))
				send_to_char ("local> ", d->character);
			send_to_char (argument, d->character);
			send_to_char ("\n\r", d->character);
		}
	}
	
	return;
}

_DOFUN (do_zecho)
{
	DESCRIPTOR_DATA *d;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Zone echo what?\n\r", ch);
		return;
	}
	
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->connected == CON_PLAYING
			&& d->character->in_room != NULL && ch->in_room != NULL
			&& d->character->in_room->area == ch->in_room->area)
		{
			if (get_trust (d->character) >= get_trust (ch))
				send_to_char ("zone> ", d->character);
			send_to_char (argument, d->character);
			send_to_char ("\n\r", d->character);
		}
	}
}

_DOFUN (do_pecho)
{
	char arg[MIL];
	CHAR_DATA *victim;
	
	argument = one_argument (argument, arg);
	
	if (argument[0] == '\0' || arg[0] == '\0')
	{
		send_to_char ("Personal echo what?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("Target not found.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (victim)
		&& (IS_FLAG (victim->states.config, _BUILDING)
		&& get_trust (ch) < get_trust (victim)))
	{
		send_to_char ("You really shouldn't interrupt a builder at work.\n\r",
			ch);
		return;
	}

	if ( (IS_AFFECTED(ch, _AFF_DREAMING) && !IS_IMMORTAL(ch)) && victim->position > POS_SLEEPING )
	{
		chprintf(ch, "You cant send dreams to those not dreaming.\n\r");
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch) && get_trust (ch) != MAX_LEVEL)
		send_to_char ("personal> ", victim);
	
	send_to_char (argument, victim);
	send_to_char ("\n\r", victim);
	send_to_char ("personal> ", ch);
	send_to_char (argument, ch);
	send_to_char ("\n\r", ch);
}


ROOM_INDEX_DATA *find_location (CHAR_DATA * ch, char *arg)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	
	if (is_number (arg))
		return get_room_index (atol (arg));
	
	if ((victim = get_char_world (ch, arg)) != NULL)
		return victim->in_room;
	
	if ((obj = get_obj_world (ch, arg)) != NULL)
		return obj->in_room;
	
	return NULL;
}



_DOFUN (do_transfer)
{
	char arg1[MIL];
	char arg2[MIL];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0')
	{
		send_to_char ("Transfer whom (and where)?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg1, "all"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING
				&& d->character != ch
				&& d->character->in_room != NULL && can_see (ch, d->character))
			{
				char buf[MSL];
				sprintf (buf, "%s %s", d->character->name, arg2);
				do_transfer (ch, buf);
			}
		}
		return;
	}
	
	/*
	* Thanks to Grodyn for the optional location parameter.
	*/
	if (arg2[0] == '\0')
	{
		location = ch->in_room;
	}
	else
	{
		if ((location = find_location (ch, arg2)) == NULL)
		{
			send_to_char ("No such location.\n\r", ch);
			return;
		}
		
		if (!is_room_owner (ch, location) && room_is_private (location)
			&& get_trust (ch) < MAX_LEVEL)
		{
			send_to_char ("That room is private right now.\n\r", ch);
			return;
		}
	}
	
	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (victim)
		&& (IS_FLAG (victim->states.config, _BUILDING)
		&& get_trust (ch) < get_trust (victim)))
	{
		send_to_char ("You really shouldn't interrupt a builder at work.\n\r",
			ch);
		return;
	}
	
	if (victim->in_room == NULL)
	{
		send_to_char ("They are in limbo.\n\r", ch);
		return;
	}
	
	if ((victim->level > ch->level) && (!IS_NPC (victim)))
	{
		send_to_char ("I dont think they would appreciate that.\n\r", ch);
		return;
	}
	
	if (victim->fighting != NULL)
		stop_fighting (victim, TRUE);
	act ("$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM,
		POS_RESTING);
	char_from_room (victim);
	char_to_room (victim, location);
	act ("$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM,
		POS_RESTING);
	
	if (MOUNTED (victim))
	{
		char_from_room (MOUNTED (victim));
		char_to_room (MOUNTED (victim), location);
		send_to_char ("Your rider is being transferred, and so are you.\n\r",
			MOUNTED (victim));
	}
	
	if (ch->pet != NULL)
	{
		char_from_room (ch->pet);
		char_to_room (ch->pet, location);
	}
	
	
	if (ch != victim)
		act ("$n has transferred you.", ch, NULL, victim, TO_VICT, POS_DEAD);
	do_look (victim, "auto");
	send_to_char ("Ok.\n\r", ch);
}



_DOFUN (do_at)
{
	char arg[MIL];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	OBJ_DATA *on;
	CHAR_DATA *wch;
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("At where what?\n\r", ch);
		return;
	}
	
	if ((location = find_location (ch, arg)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}
	
	for (wch = location->people; wch != NULL; wch = wch->next_in_room)
	{
		if (!IS_NPC (wch)
			&& (IS_FLAG (wch->states.config, _BUILDING)
			&& get_trust (ch) < get_trust (wch)))
		{
			send_to_char ("You really shouldn't interrupt a builder at work.\n\r",
				ch);
			return;
		}
	}
	
	if (!is_room_owner (ch, location) && room_is_private (location)
		&& get_trust (ch) < MAX_LEVEL)
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}
	
	original = ch->in_room;
	on = ch->on;
	char_from_room (ch);
	char_to_room (ch, location);
	interpret (ch, argument);
	
	/*
	* See if 'ch' still exists before continuing!
	* Handles 'at XXXX quit' case.
	*/
	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (wch == ch)
		{
			char_from_room (ch);
			char_to_room (ch, original);
			ch->on = on;
			break;
		}
	}
	
	return;
}



_DOFUN (do_goto)
{
	ROOM_INDEX_DATA *location;
	CHAR_DATA *rch;
	int count = 0;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Goto where?\n\r", ch);
		return;
	}
	
	if ((location = find_location (ch, argument)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}
	
	count = 0;
	for (rch = location->people; rch != NULL; rch = rch->next_in_room)
	{
		count++;
		if (!IS_NPC (rch)
			&& (IS_FLAG (rch->states.config, _BUILDING)
			&& get_trust (ch) < get_trust (rch)))
		{
			send_to_char ("You really shouldn't interrupt a builder at work.\n\r",
				ch);
			return;
		}
	}
	
	if (!is_room_owner (ch, location) && room_is_private (location)
		&& (count > 1 || get_trust (ch) < MAX_LEVEL))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}
	
	if (ch->fighting != NULL)
		stop_fighting (ch, TRUE);
	
	display_poof (ch, "out");
	
	char_from_room (ch);
	char_to_room (ch, location);
	
	display_poof (ch, "in");
	
	if (ch->pet != NULL)
	{
		char_from_room (ch->pet);
		char_to_room (ch->pet, location);
	}
	
	if (MOUNTED (ch))
	{
		char_from_room (MOUNTED (ch));
		char_to_room (MOUNTED (ch), location);
		send_to_char
			("Your rider is a god, and did a goto. You went along for the ride.\n\r",
			ch);
	}
	
	do_look (ch, "auto");
	return;
}

_DOFUN (do_violate)
{
	ROOM_INDEX_DATA *location;
	CHAR_DATA *rch;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Goto where?\n\r", ch);
		return;
	}
	
	if ((location = find_location (ch, argument)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}
	
	if (!room_is_private (location))
	{
		send_to_char ("That room isn't private, use goto.\n\r", ch);
		return;
	}
	
	for (rch = location->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!IS_NPC (rch)
			&& (IS_FLAG (rch->states.config, _BUILDING)
			&& get_trust (ch) < get_trust (rch)))
		{
			send_to_char ("You really shouldn't interrupt a builder at work.\n\r",
				ch);
			return;
		}
	}
	
	
	if (ch->fighting != NULL)
		stop_fighting (ch, TRUE);
	
	display_poof (ch, "out");
	
	char_from_room (ch);
	char_to_room (ch, location);
	
	display_poof (ch, "in");
	
	do_look (ch, "auto");
	return;
}

/* RT to replace the 3 stat commands */

_DOFUN (do_stat)
{
	char arg[MIL];
	char arg2[MIL];
	char argbuf[MIL];
	
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);
	sprintf (argbuf, "%s %s %s", arg, arg2, argument);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r"
			"  stat <name>\n\r"
			"  stat obj <name>\n\r"
			"  stat mob <name>\n\r" "  stat room <number>\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "room"))
	{
		do_rstat (ch, arg2);
		return;
	}
	
	if (!str_cmp (arg, "obj"))
	{
		do_ostat (ch, arg2);
		return;
	}
	
	if (!str_cmp (arg, "char") || !str_cmp (arg, "mob"))
	{
		do_mstat (ch, arg2);
		return;
	}
	
	/* do it the old way */
	
	if (get_obj_world (ch, arg) != NULL)
	{
		do_ostat (ch, argbuf);
		return;
	}
	
	if (get_char_world (ch, arg) != NULL)
	{
		do_mstat (ch, argbuf);
		return;
	}
	
	if (find_location (ch, arg) != NULL)
	{
		do_rstat (ch, argbuf);
		return;
	}
	
	send_to_char ("Nothing by that name found anywhere.\n\r", ch);
}


_DOFUN (do_rstat)
{
	char buf[MSL];
	char arg[MIL];
	ROOM_INDEX_DATA *location;
	OBJ_DATA *obj;
	CHAR_DATA *rch;
	int door;
	
	one_argument (argument, arg);
	location = (arg[0] == '\0') ? ch->in_room : find_location (ch, arg);
	if (location == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}
	
	if (!is_room_owner (ch, location) && ch->in_room != location
		&& room_is_private (location) && !IS_TRUSTED (ch, IMPLEMENTOR))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}
	
	sprintf (buf, "Name: '%s'\n\rArea: '%s'\n\r",
		location->name, location->area->name);
	send_to_char (buf, ch);
	
	sprintf (buf,
		"Vnum: %ld  Sector: %d  Light: %d  Healing: %d  Ability: %d\n\r",
		location->vnum,
		location->sector_type,
		location->light, location->heal_rate, location->opu_rate);
	send_to_char (buf, ch);
	
	sprintf (buf,
		"Room flags: %d.\n\rDescription:\n\r%s",
		location->room_flags, location->description);
	send_to_char (buf, ch);
	
	if (location->extra_descr != NULL)
	{
		EXTRA_DESCR_DATA *ed;
		
		send_to_char ("Extra description keywords: '", ch);
		for (ed = location->extra_descr; ed; ed = ed->next)
		{
			send_to_char (ed->keyword, ch);
			if (ed->next != NULL)
				send_to_char (" ", ch);
		}
		send_to_char ("'.\n\r", ch);
	}
	
	send_to_char ("Characters:", ch);
	for (rch = location->people; rch; rch = rch->next_in_room)
	{
		if (can_see (ch, rch))
		{
			send_to_char (" ", ch);
			one_argument (rch->name, buf);
			send_to_char (buf, ch);
		}
	}
	
	send_to_char (".\n\rObjects:   ", ch);
	for (obj = location->contents; obj; obj = obj->next_content)
	{
		send_to_char (" ", ch);
		one_argument (obj->name, buf);
		send_to_char (buf, ch);
	}
	send_to_char (".\n\r", ch);
	
	for (door = 0; door <= 5; door++)
	{
		EXIT_DATA *pexit;
		
		if ((pexit = location->exit[door]) != NULL)
		{
			sprintf (buf,
				"Door: %d.  To: %ld.  Key: %ld.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
				door,
				(pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum),
				pexit->key,
				pexit->exit_info,
				pexit->keyword,
				pexit->description[0] != '\0'
				? pexit->description : "(none).\n\r");
			send_to_char (buf, ch);
		}
	}
	
	return;
}



_DOFUN (do_ostat)
{
	char buf[MSL];
	char arg[MIL];
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	int cnt = 0;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Stat what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_world (ch, argument)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}
	
	sprintf (buf, "Level [%2d]  Name   : %s\n\r", obj->level, obj->name);
	send_to_char (buf, ch);
	
	
	sprintf (buf, "Short description  : %s\n\r"
		"Long description   : %s\n\r\n\r",
		obj->short_descr, obj->description);
	
	send_to_char (buf, ch);
	
	if (obj->extra_descr)
	{
		EXTRA_DESCR_DATA *ed;
		
		send_to_char ("Ex desc kwd        : ", ch);
		
		for (ed = obj->extra_descr; ed; ed = ed->next)
		{
			send_to_char ("[", ch);
			send_to_char (ed->keyword, ch);
			send_to_char ("]", ch);
		}
		
		send_to_char ("\n\r", ch);
	}
	
	if (obj->pIndexData->extra_descr)
	{
		EXTRA_DESCR_DATA *ed;
		
		send_to_char ("Ex desc kwd2       : ", ch);
		
		for (ed = obj->pIndexData->extra_descr; ed; ed = ed->next)
		{
			send_to_char ("[", ch);
			send_to_char (ed->keyword, ch);
			send_to_char ("]", ch);
		}
		
		send_to_char ("\n\r", ch);
	}
	
	sprintf (buf,
		"Vnum               : [%d]  Type: %s  Resets: %d  Points: %d\n\r",
		obj->pIndexData->vnum, item_name (obj->item_type),
		obj->pIndexData->reset_num, eq_cost (obj));
	
	send_to_char (buf, ch);
	
	sprintf (buf, "Wear flags         : [%s]\n\r"
		"Extra flags        : [%s]\n\r",
		flag_string (wear_flags, obj->wear_flags),
		flag_string (extra_flags, obj->extra_flags));
	
	send_to_char (buf, ch);
	
	sprintf (buf, "Number             : [1/%d]  Weight: [%d/%d/%d]\n\r",
		get_obj_number (obj),
		obj->weight, get_obj_weight (obj), get_true_weight (obj));
	
	send_to_char (buf, ch);
	
	sprintf (buf, "Cost               : [%d]  Condition: [%d]  Timer: [%d]\n\r",
		obj->cost, obj->condition, obj->timer);
	
	send_to_char (buf, ch);
	
	sprintf (buf,
		"In room            : [%ld]  In object : %s  Carried by : (%s)\n\r\n\r",
		obj->in_room == NULL ? 0 : obj->in_room->vnum,
		obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr,
		obj->carried_by == NULL ? "(none)" : can_see (ch,
							 obj->
							 carried_by) ? obj->
							 carried_by->name : "someone");
	
	send_to_char (buf, ch);
	
	show_obj_values (ch, obj->pIndexData);
	
	for (cnt = 0, paf = obj->affected; paf; paf = paf->next)
	{
		if (cnt == 0)
		{
			send_to_char ("\n\rNumber Modifier Affects\n\r", ch);
			send_to_char ("------ -------- -------\n\r", ch);
		}
		sprintf (buf, "[%4d] %-8d %s\n\r",
			cnt, paf->modifier, flag_string (apply_flags, paf->location));
		
		send_to_char (buf, ch);
		cnt++;
	}
	
	if (!obj->enchanted)
		for (paf = obj->pIndexData->affected; paf; paf = paf->next)
		{
			if (cnt == 0)
			{
				send_to_char ("\n\rNumber Modifier Affects\n\r", ch);
				send_to_char ("------ -------- -------\n\r", ch);
			}
			sprintf (buf, "[%4d] %-8d %s\n\r",
				cnt, paf->modifier, flag_string (apply_flags, paf->location));
			
			send_to_char (buf, ch);
			cnt++;
		}
		
		return;
}


_DOFUN (do_mstat)
{
	char buf[MSL];
	char arg[MIL];
	char arg2[MIL];
	AFFECT_DATA *paf, *paf_last = NULL;
	CHAR_DATA *victim;

	
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Stat whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (arg2[0] != '\0' && !str_prefix (arg2, "skills") && !IS_NPC (victim))
	{
		sh_int sn;
		sh_int count = 0;
		char buf[MIL];
		BUFFER *buffer;
		
		buffer = new_buf ();
		sprintf (buf, "Skill listing for %s.\n\r", PERS (victim, ch));
		add_buf (buffer, buf);
		
		for (sn = 0; sn < victim->pcdata->iMemSize[MEM_MAXSKILL]; sn++)
		{
			if (victim->pcdata->learned[sn] > 0 &&
				skill_table[sn].weave_fun == NULL)
			{
				sprintf (buf, "%3d] %-12.12s (%3d%%) %s",
					++count,
					skill_table[sn].name,
					victim->pcdata->learned[sn],
					((1 + count) % 3 == 0) ? "\n\r" : "  ");
				add_buf (buffer, buf);
			}
		}
		page_to_char (buf_string (buffer), ch);
		free_buf (buffer);
		return;
	}
	else
	if (arg2[0] != '\0' && !str_prefix (arg2, "weaves") && !IS_NPC (victim))
	{
		sh_int sn;
		sh_int count = 0;
		char buf[MIL];
		BUFFER *buffer;
		
		buffer = new_buf ();
		sprintf (buf, "Weave listing for %s.\n\r", PERS (victim, ch));
		add_buf (buffer, buf);
		
		for (sn = 0; sn < victim->pcdata->iMemSize[MEM_MAXSKILL]; sn++)
		{
			if (victim->pcdata->learned[sn] > 0 &&
				skill_table[sn].weave_fun != NULL)
			{
				sprintf (buf, "%3d] %-12.12s (%3d%%) %s",
					++count,
					skill_table[sn].name,
					victim->pcdata->learned[sn],
					((1 + count) % 3 == 0) ? "\n\r" : "  ");
				add_buf (buffer, buf);
			}
		}
		sprintf (buf, "\n\r\tWARNING: `RDO NOT`w reveal this information to anyone.\n\r");
		add_buf (buffer, buf);
		page_to_char (buf_string (buffer), ch);
		free_buf (buffer);
		return;
	}
	else

	if (arg2[0] != '\0' && !str_prefix (arg2, "abilities") && !IS_NPC (victim))
	{		
		int ab;
		char buf[MIL];
		ab = 0;
		
		// Need all the if statemetns as the name of the Abilities is not stored
		sprintf (buf, "Listing abilities/impediments for %s.\n\r\n\r", PERS (victim, ch));
		send_to_char(buf, ch);
		
		send_to_char("`W--`w--`N--`w--`W--`N--`w--`W--`N--`w--`CA`cbilitie`Cs`W--`w--`N--`w--`W--`N--`w--`W--`N--`w--\n\r", ch);
		if (victim->ability[ABILITY_FASTFEET] | 1)
		{
			send_to_char("`CFast Feet    `W|`c +3 to all Initiative rolls.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_STEPSOFT] | 1) 
		{
			send_to_char("`CStep Soft    `W|`c +3 on sneak rolls.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_WORDLYWISE] | 1) 
		{
			send_to_char("`CWorldly Wise `W|`c Extra info on look command.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_SHARPSENSES] | 1) 
		{
			send_to_char("`CSharp Senses `W|`c +3 on sense rolls.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_LEADERSHIP] | 1)
		{
			send_to_char("`CLeadership   `W|`c Grouping penalties removed.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_MASTERTOUCH] | 1) 
		{
			send_to_char("`CMastertouch  `W|`c +3 on all craft rolls`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_LONGWIND] | 1) 
		{
			send_to_char("`CLongwind     `W|`c Run/shout penalty reduces to *2`w`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_DEADEYE] | 1) 
		{
			send_to_char("`CDead Eye     `W|`c +5 on ranged weapons.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_IGTERRAIN] | 1) 
		{
			send_to_char("`CIgn Terain   `W|`c Negates all terrain stam penalities`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_IGTEMP] | 1) 
		{
			send_to_char("`CIgn Temp     `W|`c Ignores temperature effects.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_IGPAIN] | 1) 
		{
			send_to_char("`CIgn Pain     `W|`c No negative effects due to low hp`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_PREDWEATHER] | 1) 
		{
			send_to_char("`CPred Weather `W|`c Can predict Weather`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_IGNOREWEATHER] | 1) 
		{
			send_to_char("`CIgn weather  `W|`c No negative effects from weather.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_PERPITCH] | 1) 
		{
			send_to_char("`CPerfectPitch `W|`c +3 on instrument rolls.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_DRUMPLAYING] | 1) 
		{
			send_to_char("`CDrumplaying  `W|`c Can play drums.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_FLUTEPLAYING] | 1) 
		{
			send_to_char("`CFlute playing`W|`c Can play the flute.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_HORNPLAYING] | 1) 
		{
			send_to_char("`CHorn playing `W|`c Can play the horn.`w\n\r", ch);
			ab++;
		}
		if (victim->ability[ABILITY_HARPPLAYING] | 1)
		{
			send_to_char("`CHarp playing `W|`c Can play the harp.`w\n\r", ch);
			ab++;
		}

		// They have no abilities
		if (ab == 0)
			send_to_char("No `Cabilities`w selected", ch);
		
		// Resetting counter for impediments
		ab =0;

		send_to_char("`W--`w--`N--`w--`W--`N--`w--`W--`N--`w--`CIm`pedimen`Cts`W--`w--`N--`w--`W--`N--`w--`W--`N--`w--\n\r", ch);
		if (victim->impediment[IMPEDIMENT_PRIDE] | 1)
		{
			send_to_char("`CPride        `W|`c Cannot flee if other PC are in the room.`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_GLUTTONY] | 1) 
		{
			send_to_char("`CGluttony     `W|`c All food values eaten are halved.`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_SELFISHNESS] | 1) 
		{
			send_to_char("`CSelfishness  `W|`c Cannot give items or drop items with PC present`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_VENGEANCE] | 1) 
		{
			send_to_char("`CVengeance    `W|`c If attacked cannot attack anyone but assailant for 10 minutes`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_ANTISOCIAL] | 1)
		{
			send_to_char("`CAntisocial   `W|`c Unable to group`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_SHOWOFF] | 1) 
		{
			send_to_char("`CShowoff      `W|`c if PC are present, you intentionally fail 25% of Init Rolls`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_ALCOHOLISM] | 1) 
		{
			send_to_char("`CAlcoholism   `W|`c must drink alchohol every 2 rl hours or -2 to all ATTR until`w`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_PREJUDICED] | 1) 
		{
			send_to_char("`CPrejudiced   `W|`c cannot learn dialects, cannot group with other nationalities.`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_CRIPPLED] | 1) 
		{
			send_to_char("`CCrippled     `W|`c longer walk delay, cannot run/sneak/trackless`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_LONELINESS] | 1) 
		{
			send_to_char("`CLoneliness   `W|`c -1 to ATTR if no other PC are in room`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_PARANOIA] | 1) 
		{
			send_to_char("`CParanoia     `W|`c -1 to ATTR if PC are in room, -2 to ATTR when grouped`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_SPEECHPROB] | 1) 
		{
			send_to_char("`CSpeech prob  `W|`c cannot learn dialects, stam req for say, 25% of words mangled`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_MUTE] | 1) 
		{
			send_to_char("`CMute         `W|`c Say and shout disabled`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_DEAF] | 1) 
		{
			send_to_char("`CDeaf         `W|`c Can read 75% of all words in all dialects, -3 SENSE`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_LAZINESS] | 1) 
		{
			send_to_char("`CLaziness     `W|`c -1 to ATTR when grouped, 10% autorest if not hungryw\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_CONTROLFREAK] | 1) 
		{
			send_to_char("`CControl freak`W|`c You cannot be in a group you do not lead,-1 to ATTR when grouped`w\n\r", ch);
			ab++;
		}
		if (victim->impediment[IMPEDIMENT_INSOLENCE] | 1) 
		{
			send_to_char("`CInsolence    `W|`c 50% chance of failling learning roll`w\n\r", ch);
			ab++;
		}
		
		return;
	}
	
	
	if ( !IS_NPC(victim) )
	{
		sprintf( buf, "Notoriety: [Fame: %3.3f] [Infamy: %3.3f]\n\r",
			victim->pcdata->notoriety[FAME],
			victim->pcdata->notoriety[INFAMY]);
		send_to_char(buf,ch);
	}
	
	sprintf (buf, "Short description  : %s\n\rLong  description  : %s\n\r",
		victim->short_descr,
		(victim->long_descr[0] != '\0') ?
		victim->long_descr : "(none)\n\r");
	
	send_to_char (buf, ch);
	
	sprintf (buf,
		"Vnum [%ld]\tType    [%s]\tRace  [%s]\tGroup [%d]\tSex [%s]\n\r",
		IS_NPC (victim) ? victim->pIndexData->vnum : 0,
		IS_NPC (victim) ? "npc" : "pc", race_table[victim->race].name,
		IS_NPC (victim) ? victim->group : 0, sex_table[victim->sex].name);
	send_to_char (buf, ch);
	
	sprintf (buf, "Str  [%d/%d]\tInt     [%d/%d]\tWis   [%d/%d]\tDex   "
		"[%d/%d]\tCon [%d/%d]\n\r",
		victim->perm_stat[STAT_STR], get_curr_stat (victim, STAT_STR),
		victim->perm_stat[STAT_INT], get_curr_stat (victim, STAT_INT),
		victim->perm_stat[STAT_AGL], get_curr_stat (victim, STAT_AGL),
		victim->perm_stat[STAT_DEX], get_curr_stat (victim, STAT_DEX),
		victim->perm_stat[STAT_CON], get_curr_stat (victim, STAT_CON),
		victim->perm_stat[STAT_SEN], get_curr_stat (victim, STAT_SEN));
	send_to_char (buf, ch);
	
	sprintf (buf,
		"Hitpoints [%d/%d]\n\rAbility   [%d/%d]\n\rEndurance [%d/%d]\n\r",
		victim->hit, victim->max_hit, TAU (victim), OPR (victim),
		victim->move, victim->max_move);
	send_to_char (buf, ch);
	
	if (!IS_NPC (victim))
	{
		sprintf (buf, "Practices [%d]\t\t\tTrains [%d]\t\tPoints [%d]\n\r",
			victim->practice, victim->train, victim->pcdata->points);
		send_to_char (buf, ch);
		
		if (is_clan (victim))
		{
			sprintf (buf, "Clan:     %s\t\tRank (%d):  %s\t\n\r\n\r",
				player_clan (victim), victim->rank + 1, player_rank (victim));
			send_to_char (buf, ch);
		}
		
		chprintf (ch, "Quest Points: %d\n\r", victim->pcdata->quest_points);
	}
	else
	{
		sprintf (buf, "\n\rCount: %d  Killed: %d\n\r",
			victim->pIndexData->count, victim->pIndexData->killed);
		send_to_char (buf, ch);
	}
	
	sprintf (buf,
		"Class: %s\tAlign: %d\tGold: %ld\tSilver: %ld\tExp: %ld\n\r",
		IS_NPC (victim) ? "mobile" : class_table[victim->c_class].name,
		victim->alignment, victim->gold, victim->silver, victim->exp);
	send_to_char (buf, ch);
	
	if (!IS_NPC (ch) && ch->pcdata->insanity > 0)
	{
		sprintf (buf, "Insanity Rating: %02d%%\n\r", ch->pcdata->insanity);
		send_to_char (buf, ch);
	}
	
	sprintf (buf, "Armor: pierce: %d  bash: %d  slash: %d  weave: %d\n\r",
		GET_AC (victim, AC_PIERCE), GET_AC (victim, AC_BASH),
		GET_AC (victim, AC_SLASH), GET_AC (victim, AC_EXOTIC));
	send_to_char (buf, ch);
	
	sprintf (buf,
		"Hit: %d  Dam: %d  Saves: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
		GET_HITROLL (victim), GET_DAMROLL (victim),
		victim->saving_throw, size_table[victim->size].name,
		position_table[victim->position].name, victim->wimpy);
	send_to_char (buf, ch);
	
	if (IS_NPC (victim))
	{
		sprintf (buf, "Damage: %dd%d  Message:  %s\n\r",
			victim->damage[DICE_NUMBER],
			victim->damage[DICE_TYPE], attack_table[victim->dam_type].noun);
		send_to_char (buf, ch);
	}
	sprintf (buf, "Fighting: %s\n\r",
		victim->fighting ? victim->fighting->name : "(none)");
	send_to_char (buf, ch);
	
	chprintf (ch, "Air %2d|%2d Earth %2d|%2d Water %2d|%2d "
		"Fire %2d|%2d Spirit %2d|%2d OPR: %2d TAU: %2d\n\r",
		victim->flow_str.current[AIR],
		victim->flow_str.perm[AIR],
		victim->flow_str.current[EARTH],
		victim->flow_str.perm[EARTH],
		victim->flow_str.current[WATER],
		victim->flow_str.perm[WATER],
		victim->flow_str.current[FIRE],
		victim->flow_str.perm[FIRE],
		victim->flow_str.current[SPIRIT],
		victim->flow_str.perm[SPIRIT], OPR (victim), TAU (victim));
	
	if (!IS_NPC (victim))
	{
		sprintf (buf,
			"Thirst: %d  Hunger: %d  Full: %d  Drunk: %d\n\r",
			victim->pcdata->condition[_THIRST],
			victim->pcdata->condition[_HUNGER],
			victim->pcdata->condition[_FULL],
			victim->pcdata->condition[_DRUNK]);
		send_to_char (buf, ch);
	}
	
	sprintf (buf, "Carry number: %d  Carry weight: %ld\n\r",
		victim->carry_number, get_carry_weight (victim) / 10);
	send_to_char (buf, ch);
	
	
	if (!IS_NPC (victim))
	{
		sprintf (buf,
			"Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
			get_age (victim),
			(int) (victim->played + current_time - victim->logon) / 3600,
			victim->pcdata->last_level, victim->timer);
		send_to_char (buf, ch);
	}
	
	
	
	sprintf (buf, "Master: %s  Leader: %s  Pet: %s  Horse: %s\n\r",
		victim->master ? victim->master->name : "(none)",
		victim->leader ? victim->leader->name : "(none)",
		victim->pet ? victim->pet->name : "(none)",
		victim->mount ? victim->mount->name : "(none)");
	send_to_char (buf, ch);
	
	if (!IS_NPC (victim))
	{
		sprintf (buf, "Security: %d.\n\r", victim->pcdata->security);
		send_to_char (buf, ch);
	}
	
	if (IS_NPC (victim) && victim->spec_fun != 0)
	{
		sprintf (buf, "Mobile has special procedure %s.\n\r",
			spec_name (victim->spec_fun));
		send_to_char (buf, ch);
	}
	
	chprintf (ch, "Act : %s\n\r",
		bit_string (_act_bits, victim->states.player));
	chprintf (ch, "Com : %s\n\r",
		bit_string (_comm_bits, victim->states.comm));
	chprintf (ch, "Off : %s\n\r",
		bit_string (_off_bits, victim->states.off));
	chprintf (ch, "Imm : %s\n\r",
		bit_string (_resistance_bits, victim->states.imm));
	chprintf (ch, "Res : %s\n\r",
		bit_string (_resistance_bits, victim->states.res));
	chprintf (ch, "Vul : %s\n\r",
		bit_string (_resistance_bits, victim->states.vuln));
	chprintf (ch, "Pen : %s\n\r",
		bit_string (_penalty_bits, victim->states.penalties));
	
	if (victim->states.affected_by[0] > 0 || victim->states.affected_by[1] > 0)
	  chprintf (ch, "Aff: %s\n\r", 
	    bit_string (_affected_by_bits, 
		victim->states.affected_by));
	
	if (IS_NPC (ch))
	  chprintf (ch, "Form: %s\n\rPart: %s\n\r", 
		bit_string (_form_bits, victim->states.form),
		bit_string (_part_bits, victim->states.parts));
		
	for (paf = victim->affected; paf != NULL; paf = paf->next)
	{
		if (paf_last != NULL && paf->type == paf_last->type)
			sprintf (buf, "                    ");
		else
			sprintf (buf, "%-20.20s", skill_table[paf->type].name);

		send_to_char (buf, ch);

		chprintf (ch, ": modifies %s by %d ", 
			flag_string(apply_flags, paf->location), paf->modifier);

		if (paf->duration == -1)
			sprintf (buf, "permanently");
		else
			sprintf (buf, "for %d hours", paf->duration);
		
		send_to_char (buf, ch);
		send_to_char ("\n\r", ch);
		paf_last = paf;
	}
	return;
}


/* ofind and mfind replaced with vnum, vnum skill also added */

_DOFUN (do_vnum)
{
	char arg[MIL];
	char *string;
	
	string = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  vnum obj <name>\n\r", ch);
		send_to_char ("  vnum mob <name>\n\r", ch);
		send_to_char ("  vnum skill <skill or weave>\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "obj"))
	{
		do_ofind (ch, string);
		return;
	}
	
	if (!str_cmp (arg, "mob") || !str_cmp (arg, "char"))
	{
		do_mfind (ch, string);
		return;
	}
	
	if (!str_cmp (arg, "skill") || !str_cmp (arg, "weave"))
	{
		do_slookup (ch, string);
		return;
	}
	/* do both */
	do_mfind (ch, argument);
	do_ofind (ch, argument);
}


_DOFUN (do_mfind)
{
	extern int top_mob_index;
	char buf[MSL];
	char arg[MIL];
	MOB_INDEX_DATA *pMobIndex;
	long vnum;
	int nMatch;
	bool fAll;
	bool found;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Find whom?\n\r", ch);
		return;
	}
	
	fAll = FALSE;			/* !str_cmp( arg, "all" ); */
	found = FALSE;
	nMatch = 0;
	
	/*
	* Yeah, so iterating over all vnum's takes 10,000 loops.
	* Get_mob_index is fast, and I don't feel like threading another link.
	* Do you?
	* -- Furey
	*/
	for (vnum = 0; nMatch < top_mob_index; vnum++)
	{
		if ((pMobIndex = get_mob_index (vnum)) != NULL)
		{
			nMatch++;
			if (fAll || is_name (argument, pMobIndex->player_name))
			{
				found = TRUE;
				sprintf (buf, "[%5ld] %s\n\r",
					pMobIndex->vnum, pMobIndex->short_descr);
				send_to_char (buf, ch);
			}
		}
	}
	
	if (!found)
		send_to_char ("No mobiles by that name.\n\r", ch);
	
	return;
}



_DOFUN (do_ofind)
{
	extern int top_obj_index;
	char buf[MSL];
	char arg[MIL];
	OBJ_INDEX_DATA *pObjIndex;
	long vnum;
	int nMatch;
	bool fAll;
	bool found;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Find what?\n\r", ch);
		return;
	}
	
	fAll = FALSE;			/* !str_cmp( arg, "all" ); */
	found = FALSE;
	nMatch = 0;
	
	/*
	* Yeah, so iterating over all vnum's takes 10,000 loops.
	* Get_obj_index is fast, and I don't feel like threading another link.
	* Do you?
	* -- Furey
	*/
	for (vnum = 0; nMatch < top_obj_index; vnum++)
	{
		if ((pObjIndex = get_obj_index (vnum)) != NULL)
		{
			nMatch++;
			if (fAll || is_name (argument, pObjIndex->name))
			{
				found = TRUE;
				sprintf (buf, "[%5d] [%2d] %s\n\r",
					pObjIndex->vnum, 
					pobj_eq_cost(pObjIndex),
					pObjIndex->short_descr);
				send_to_char (buf, ch);
			}
		}
	}
	
	if (!found)
		send_to_char ("No objects by that name.\n\r", ch);
	
	return;
}


_DOFUN (do_owhere)
{
	char buf[MIL];
	BUFFER *buffer;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	int number = 0, max_found;
	
	found = FALSE;
	number = 0;
	max_found = 200;
	
	buffer = new_buf ();
	
	if (argument[0] == '\0')
	{
		send_to_char ("Find what?\n\r", ch);
		return;
	}
	
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (!can_see_obj (ch, obj) || !is_name (argument, obj->name)
			|| ch->level < obj->level)
			continue;
		
		found = TRUE;
		number++;
		
		
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
			;
		
		if (in_obj->carried_by != NULL
			&& can_see (ch, in_obj->carried_by)
			&& in_obj->carried_by->in_room != NULL)
			
			sprintf (buf, "%3d) %s is carried by %s [Room %ld]\n\r",
			number,
			obj->short_descr,
			PERS (in_obj->carried_by, ch),
			in_obj->carried_by->in_room->vnum);
		
		else
			if (in_obj->in_room != NULL && can_see_room (ch, in_obj->in_room))
				
				sprintf (buf, "%3d) %s is in %s [Room %ld]\n\r",
				number,
				obj->short_descr,
				in_obj->in_room->name, in_obj->in_room->vnum);
			else
				sprintf (buf, "%3d) %s is somewhere\n\r", number, obj->short_descr);
			
			buf[0] = UPPER (buf[0]);
			add_buf (buffer, buf);
			
			if (number >= max_found)
				break;
	}
	
	if (!found)
		send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
	else
		page_to_char (buf_string (buffer), ch);
	
	free_buf (buffer);
}


_DOFUN (do_mwhere)
{
	char buf[MSL];
	BUFFER *buffer;
	CHAR_DATA *victim;
	bool found;
	int count = 0;
	
	if (argument[0] == '\0')
	{
		DESCRIPTOR_DATA *d;
		
		/* show characters logged */
		
		buffer = new_buf ();
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->character != NULL
				&& d->connected >= CON_PLAYING
				&& d->character->in_room != NULL
				&& can_see (ch, d->character)
				&& can_see_room (ch, d->character->in_room))
			{
				victim = d->character;
				count++;
				if (d->original != NULL)
					sprintf (buf, "%3d) %s (in the body of %s) is in %s [%ld]\n\r",
					count, d->original->name, victim->short_descr,
					victim->in_room->name, victim->in_room->vnum);
				else
					sprintf (buf, "%3d) %s is in %s [%ld]\n\r",
					count, victim->name, victim->in_room->name,
					victim->in_room->vnum);
				add_buf (buffer, buf);
			}
		}
		
		page_to_char (buf_string (buffer), ch);
		free_buf (buffer);
		return;
	}
	
	found = FALSE;
	buffer = new_buf ();
	for (victim = char_list; victim != NULL; victim = victim->next)
	{
		if (victim->in_room != NULL
			&& is_name (argument, victim->name)
			&& can_see (ch, victim) && can_see_room (ch, victim->in_room))
		{
			found = TRUE;
			count++;
			sprintf (buf, "%3d) [%5ld] %-28s [%5ld] %s\n\r", 
				count,
				IS_NPC (victim) ? victim->pIndexData->vnum : 0,
				IS_NPC (victim) ? victim->short_descr : victim->name,
				victim->in_room->vnum, victim->in_room->name);
			add_buf (buffer, buf);
		}
	}
	
	if (!found)
		act ("You didn't find any $T.", ch, NULL, argument, TO_CHAR, POS_DEAD);
	else
		page_to_char (buf_string (buffer), ch);
	
	free_buf (buffer);
	
	return;
}


/** Function: do_copyover
  * Descr   : Initialize a copyover state.
  * Returns : (void)
  * Syntax  : do_copyover (timer value|any other argument halts copyover)
  * Written : v1.0 12/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void do_copyover (CHAR_DATA * ch, char *argument)
{
	int value = -1;
	char arg1[MIL];
	bool copyAreas = FALSE;
	
	if (argument[0] != '\0')
	{
		argument = one_argument (argument, arg1);
		
		if (is_number (arg1))
			value = atoi (arg1);
		else if (!str_cmp (arg1, "now"))
			value = 1;
	}
	else
		value = 60;
	
	if (!IS_SET (pMud.flags, MUD_COPYOVER))
	{
		char buf[MIL];
		char arg[MIL];
		
		/* 
		* Assumes a proper syntax of: copyover ### /area <reason>
		*/
		if (strstr (argument, "/area"))
		{
			copyAreas = TRUE;
			argument = one_argument (argument, arg);
		}
		
		value = value * PULSE_PER_SECOND;
		sprintf (buf, "Copyover %sby %s in %.2f %s.%s%s\n\r",
			copyAreas ? "[w/Areas] " : "",
			(ch) ? ch->name : "System",
			value >= 240 ? (float) value / 60 / PULSE_PER_SECOND :
		(float) value / PULSE_PER_SECOND,
			value >= 240 ? "minute(s)" : "second(s)",
			!IS_NULLSTR (argument) ? "\n\rReason: " : "",
			!IS_NULLSTR (argument) ? argument : "");
		
		system_msg (buf);
		
		SET_BIT (pMud.flags, MUD_COPYOVER);
		pMud.timer = value;
		
		if (copyAreas)
			do_asave (NULL, "world backup");
		
		return;
	}
	
	if (value == -1)
	{
		if (ch)
			wiznet ("$N has canceled the copyover.", ch, NULL, 0, 0, 0);
		else
			wiznet ("Someone at CONSOLE has canceled the copyover.", NULL, NULL, 0,
			0, 0);
		
		if (ch)
			send_to_char ("Copyover Canceled.\n\r", ch);
		
		REMOVE_BIT (pMud.flags, MUD_COPYOVER);
		pMud.timer = -1;
	}
	
}

/*  Copyover - Original idea: Fusion of MUD++
*  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
*  http://pip.dknet.dk/~pip1773
*  Changed into a ROM patch after seeing the 100th request for it :)
*/
_DOFUN (copyover_state)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf[100], buf2[100], buf3[100];
	extern int control;		/* main.c */
	extern int ftp_control;
	char file[MIL];
	struct timeval time_now;
	
	sprintf (file, "%s%s", DATA_DIR, COPYOVER_FILE);
	fp = fopen (file, "w");
	
	
	if (!fp)
	{
		if (ch)
			send_to_char ("Copyover file not writeable, aborted.\n\r", ch);
		bugf ("[act_wiz.c::copyover_state] Could not write to copyover file: %s", file);
		return;
	}
	
	gettimeofday (&time_now, NULL);
	pMud.timer = -1;
	pMud.last_cover = (time_t) time_now.tv_sec;
	
    /** 
	  * Consider changing all saved areas here.
	  * Note: WoT uses a temp buffer to write to when writing area
	  * files, if successful, it renames this to the area file name. So
	  * asaving here should be fairly safe.
	  */
    do_asave (NULL, "");
	  
	/* For each playing descriptor, save its state */
	for (d = descriptor_list; d; d = d_next)
	{
		CHAR_DATA *och = CH (d);
		d_next = d->next;
		
		if (!d->character || d->connected < CON_PLAYING)
		{
			write_to_descriptor (d,
				"\n\rSorry, we are rebooting. Come back in a few minutes.\n\r",
				0);
			
			close_descriptor (d);		/* throw'em out */
		}
		
		else
		{
			sprintf (buf, "\n\r *** COPYOVER by %s - please remain seated!\n\r",
				(ch == NULL) ? "System" : PERS (ch, och));
			
			fprintf (fp, "%d %s %s %d\n", d->descriptor,
				fix_name (och->name, FALSE),
				d->host, (d->out_compress != NULL));
			
			write_to_descriptor (d, buf, 0);
			
		  /*
           * Turn off Mud Compression 
           */
		  compressEnd (d);
			  
		  save_char_obj (och);
		}  
	}  
	
	fprintf (fp, "-1\n");
	fclose (fp);
	
	/* Close reserve and other always-open files and 
	release other resources */
	
	shutdown_web_server();
	save_mud_data ();
	
	/* exec - descriptors are inherited */
	
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	sprintf (buf3, "%d", ftp_control);
#if defined(WIN32)
	_execl (EXE_FILE, "WoT", buf, "copyover", buf2, buf3, (char *) NULL);
#else
	execl (EXE_FILE, "WoT", buf, "copyover", buf2, buf3, (char *) NULL);
#endif
	/* Failed - sucessful exec will not return */
	bugf ("[act_wiz.c::copyover_state] Failed! execl");
	
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name[100];
	char host[MSL];
	char file[MIL];
	int desc;
	int compress = 0;
	bool fOld;
	int result = READ_OK;
	
	logf ("Copyover recovery initiated");
	
	sprintf (file, "%s%s", DATA_DIR, COPYOVER_FILE);
	fp = fopen (file, "r");
	
	if (!fp)
		/* there are some descriptors open which will hang forever then ? */
	{
		perror ("copyover_recover:fopen");
		logf ("Copyover file not found. Exitting.\n\r");
		_quit (1);
	}
	
	unlink (file);
	/* In case something crashes - doesn't prevent reading  */
	
	for (;;)
	{
		fscanf (fp, "%d %s %s %d\n", &desc, name, host, &compress);
		if (desc == -1)
			break;
		
		d = new_descriptor ();
		d->descriptor = desc;
		
		/* Write something, and check if it goes error-free */
		if (!write_to_descriptor (d, "\n\rRestoring from copyover...\n\r", 0))
		{
#if defined(WIN32)
			_close(desc);
#else
			close (desc);		/* nope */
#endif
			continue;
		}
		
		/* fix the damn name */
		sprintf (name, "%s", fix_name (name, TRUE));
		
		d->host = str_dup (host);
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER;
		/* -15, so close_descriptor frees the char */
		
		/* Now, find the pfile */
		result = load_char_obj (d, name, FALSE);
		fOld = (result == READ_OK || result == READ_POOR);
		
		if (!fOld)			/* Player file not found?! */
		{
			write_to_descriptor (d, "\n\rSomehow, your character was lost in the"
				" copyover. Sorry.\n\r", 0);
			close_descriptor (d);
		}
		
		else			/* ok! */
		{
			write_to_descriptor (d, "\n\rCopyover recovery complete.\n\r", 0);
			
			/* Just In Case */
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_LOGIN);
			
			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;
			
			char_to_room (d->character, d->character->in_room);
			do_look (d->character, "auto");
			act ("$n materializes!", d->character, NULL, NULL, TO_ROOM,
				POS_RESTING);
			d->connected = CON_PLAYING;
			
			if (d->character->pet != NULL)
			{
				char_to_room (d->character->pet, d->character->in_room);
				act ("$n materializes!.", d->character->pet, NULL, NULL, TO_ROOM,
					POS_RESTING);
			}
			
			if (d->character->mount != NULL)
			{
				char_to_room (d->character->mount, d->character->in_room);
				act ("$n materializes.", d->character->mount, NULL, NULL, TO_ROOM,
					POS_RESTING);
				add_follower (d->character->mount, d->character, TRUE);
				do_mount (d->character, d->character->mount->name);
			}
			
			/* Restore Mud Compression Protocol... 
			if ( compress > 0 )
			{
			send_to_char("Restoring Mud Compression State...", d->character);
			if (!compressStart(d));
			send_to_char("FAILED!\n\r", d->character);
		} */
			
		}
		
	}
	
	fclose (fp);
	
}



_DOFUN (do_reboo)
{
	send_to_char ("If you want to REBOOT, spell it out.\n\r", ch);
	return;
}

_DOFUN (do_reboot)
{
	char buf[MSL];
	extern bool merc_down;
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA *vch;
	
	if (ch->invis_level < LEVEL_HERO)
	{
		sprintf (buf, "Reboot by %s.", ch->name);
		do_echo (ch, buf);
	}
	
	save_mud_data ();
	
	merc_down = TRUE;
	for (d = descriptor_list; d != NULL; d = d_next)
	{
		d_next = d->next;
		vch = d->original ? d->original : d->character;
		if (vch != NULL)
			save_char_obj (vch);
		close_descriptor (d);
	}
	
	return;
}

_DOFUN (do_shutdown)
{
	char buf[MSL];
	char file[MIL];
	extern bool merc_down;
	CHAR_DATA *vch,*vch_next;
	
	if (ch->invis_level < LEVEL_HERO)
		sprintf (buf, "Shutdown by %s.", ch->name);
	
	sprintf (file, "%s%s", DATA_DIR, SHUTDOWN_FILE);
	append_file (ch, file, buf);
	
	merc_down = TRUE;
	save_mud_data ();

	for (vch = char_list; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next;
		if ( !IS_NPC(vch) || IS_FLAG(vch->states.player, _ACT_PERSISTANT) )
			save_char_obj(vch);

		if ( vch->desc )
		  close_descriptor(vch->desc);
	}
	return;
}

_DOFUN (do_protect)
{
	CHAR_DATA *victim;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Protect whom from snooping?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, argument)) == NULL)
	{
		send_to_char ("You can't find them.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.player, _SNOOP_PROOF))
	{
		act ("$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);
		send_to_char ("Your snoop-proofing was just removed.\n\r", victim);
		REMOVE_FLAG (victim->states.player, _SNOOP_PROOF);
	}
	else
	{
		act ("$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);
		send_to_char ("You are now immune to snooping.\n\r", victim);
		SET_FLAG (victim->states.player, _SNOOP_PROOF);
	}
}



_DOFUN (do_snoop)
{
	char arg[MIL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char buf[MSL];
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Snoop whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (victim->desc == NULL)
	{
		send_to_char ("No descriptor to snoop.\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("Cancelling all snoops.\n\r", ch);
		wiznet ("$N stops being such a snoop.",
			ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust (ch));
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->snoop_by == ch->desc)
				d->snoop_by = NULL;
		}
		return;
	}
	
	if (victim->desc->snoop_by != NULL)
	{
		send_to_char ("Busy already.\n\r", ch);
		return;
	}
	
	if (!is_room_owner (ch, victim->in_room) && ch->in_room != victim->in_room
		&& room_is_private (victim->in_room) && !IS_TRUSTED (ch, IMPLEMENTOR))
	{
		send_to_char ("That character is in a private room.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch)
		|| IS_FLAG (victim->states.player, _SNOOP_PROOF))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (ch->desc != NULL)
	{
		for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by)
		{
			if (d->character == victim || d->original == victim)
			{
				send_to_char ("No snoop loops.\n\r", ch);
				return;
			}
		}
	}
	
	victim->desc->snoop_by = ch->desc;
	sprintf (buf, "$N starts snooping on %s",
		(IS_NPC (ch) ? victim->short_descr : victim->name));
	wiznet (buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust (ch));
	send_to_char ("Ok.\n\r", ch);
	return;
}



_DOFUN (do_switch)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Switch into whom?\n\r", ch);
		return;
	}
	
	if (ch->desc == NULL)
		return;
	
	if (ch->desc->original != NULL)
	{
		send_to_char ("You are already switched.\n\r", ch);
		return;
	}
	
	if (ch->desc->editor != ED_NONE)
	{
		chprintf(ch, "Finish what ever it is your editing first.\n\r");
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (victim))
	{
		send_to_char ("You can only switch into mobiles.\n\r", ch);
		return;
	}
	
	if (!is_room_owner (ch, victim->in_room) && ch->in_room != victim->in_room
		&& room_is_private (victim->in_room) && !IS_TRUSTED (ch, IMPLEMENTOR))
	{
		send_to_char ("That character is in a private room.\n\r", ch);
		return;
	}
	
	if (victim->desc != NULL)
	{
		send_to_char ("Character in use.\n\r", ch);
		return;
	}
	
	sprintf (buf, "$N switches into %s", victim->short_descr);
	wiznet (buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust (ch));
	
	ch->desc->character = victim;
	ch->desc->original = ch;
	victim->desc = ch->desc;
	ch->desc = NULL;
	
	if (ch->prompt != NULL)
		victim->prompt = str_dup (ch->prompt);
	
	add_sets (victim->states.comm, ch->states.comm);
	victim->lines = ch->lines;
	send_to_char ("Ok.\n\r", victim);
	return;
}



_DOFUN (do_return)
{
	char buf[MSL];
	
	if (ch->desc == NULL)
		return;
	
	if (ch->desc->original == NULL)
	{
		send_to_char ("You aren't switched.\n\r", ch);
		return;
	}
	
	send_to_char
		("You return to your original body. Type replay to see any missed tells.\n\r",
		ch);
	
	if (ch->prompt != NULL)
	{
		free_string (ch->prompt);
		ch->prompt = NULL;
	}
	
	sprintf (buf, "$N returns from %s.", ch->short_descr);
	wiznet (buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE,
		get_trust (ch));
	
	ch->desc->character = ch->desc->original;
	ch->desc->original = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc = NULL;
	
	return;
}

/* trust levels for load and clone */
bool obj_check (CHAR_DATA * ch, OBJ_DATA * obj)
{
	if (IS_TRUSTED (ch, DEMI))
		return TRUE;
	else
		return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone (CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * clone)
{
	OBJ_DATA *c_obj, *t_obj;
	
	
	for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
	{
		if (obj_check (ch, c_obj))
		{
			t_obj = create_object (c_obj->pIndexData, 0);
			if (!t_obj)
			{
				send_to_char ("BUG: Non existant object. Logged.\n\r", ch);
				return;
			}
			clone_object (c_obj, t_obj);
			obj_to_obj (t_obj, clone);
			recursive_clone (ch, c_obj, t_obj);
		}
	}
}

/* [GM] a disguise for imms who want to play... 
 * syntax: disguise [create|short|long|name] <description>
 * clones a copy of the imm, and set's descriptions to command line
 * arguments
 */
_DOFUN (do_disguise)
{
	CHAR_DATA *clone;
	MOB_INDEX_DATA *pMobIndex;
	char buf[MSL];
	char command[20];
	
	if (!IS_IMMORTAL (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, command);
	
	/* syntax check 
     * I know this seems cludgy, instead of just letting it fall through
     * the loop, but this way I dont have to check for switched status
     * three times.
     */
	if ((command[0] == '\0' 
	|| !str_prefix (command, "help")) 
	|| (str_prefix (command, "create") 
	 && str_prefix (command, "short") 
     && str_prefix (command, "name") 
	 && str_prefix (command, "long")))
	{
		send_to_char
			("\n\rSyntax: disguise [create|short|long|name] <description>\n\r\n\r", ch);
		send_to_char ("disguise create [optional name]\n\r\n\r", ch);
		send_to_char ("Once created, and you have \"switched\" to the new disguise,\n\r", ch);
		send_to_char ("you have the following options: \n\r\n\r", ch);
		send_to_char ("disguise short <short description>\n\r", ch);
		send_to_char ("disguise long <long description>\n\r", ch);
		send_to_char ("disguise name <name for new disguise>\n\r\n\r", ch);
		send_to_char ("\"desc\" will place you in the description editor.\n\r", ch);
		return;
	}
	
	if (!str_prefix (command, "create"))	/* create the fool thing */
	{
		if ((pMobIndex = get_mob_index (MOB_VNUM_DISGUISE)) == NULL)
		{
			send_to_char ("Error: Clone Mob not created.\n\r", ch);
			logf ("Error: Clone Mob %d not created!", MOB_VNUM_DISGUISE);
			return;
		}
		
		/* let 'em name it on the command line, but not their own name! */
		if (argument[0] != '\0' && str_prefix (ch->name, argument))
			sprintf (buf, "%s", argument);
		else
			sprintf (buf, "%s's alter-ego", ch->name);
		
		clone = create_mobile (pMobIndex);
		char_to_room (clone, ch->in_room);
		clone_mobile (ch, clone);
		
		if (clone->name[0] != '\0')
			free_string (clone->name);
		clone->name = str_dup (buf);
		
		if (clone->short_descr[0] != '\0');
		free_string (clone->short_descr);
		clone->short_descr = str_dup ("");
		
		/* reformat for long description */
		sprintf (buf, "%s is here.\n\r", clone->name);
		if (clone->long_descr[0] != '\0');
		free_string (clone->long_descr);
		clone->long_descr = str_dup (buf);
		
		/* again reformat, back to name */
		sprintf (buf, "%s", clone->name);
		
		if (clone->description[0] != '\0');
		free_string (clone->description);
		clone->description = str_dup ("");
		
		act ("$n has pulled $s alter-ego from the Pattern!",
			ch, NULL, clone, TO_ROOM, POS_RESTING);
		
		wiznet ("$N loads a disguise.",
			ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
		
		send_to_char ("Your diguise has been created.\n\r", ch);
		chprintf (ch, "Now type \"switch %s\" and edit\n\r", clone->name);
		send_to_char ("your disguised descriptions.\n\r", ch);
		send_to_char ("Dont forget to equip it!\n\r", ch);
		return;
	}				/* end: create */
	else
		/* ok, made it this far, are they switched into the disguise yet? */
		if (ch->desc->original == NULL)
		{
			send_to_char ("You have to switch into your double, first!\n\r", ch);
			return;
		}
		
		/* since they are switched, we access the mobile by "ch" */
		if (!str_prefix (command, "name"))	/* set short, if switched */
		{
			if (argument[0] == '\0')
			{
				send_to_char ("You must supply a name!\n\r", ch);
				return;
			}
			
			/* make sure this player dosent exist already! */
			if ((clone = get_char_world (ch, argument)) != NULL)
			{
				send_to_char ("Illegal name, try another.\n\r", ch);
				return;
			}
			
			if (ch->name[0] != '\0')
				free_string (ch->name);
			ch->name = str_dup (argument);
			return;
		}				/* end: name */
		
		
		if (!str_prefix (command, "short"))	/* set short, if switched */
		{
			if (argument[0] == '\0')
			{
				send_to_char ("You must supply a short description!\n\r", ch);
				return;
			}
			
			if (ch->short_descr[0] != '\0')
				free_string (ch->short_descr);
			ch->short_descr = str_dup (argument);
			return;
		}				/* end: short */
		else
			if (!str_prefix (command, "long"))	/* set long, if switched */
			{
				if (argument[0] == '\0')
				{
					send_to_char ("You must supply a long description!\n\r", ch);
					return;
				}
				
				if (ch->long_descr[0] != '\0')
					free_string (ch->long_descr);
				ch->long_descr = str_dup (argument);
				return;
			}				/* end: long */
			
			return;
}

/* command that is similar to load */
_DOFUN (do_clone)
{
	char arg[MIL];
	char *rest;
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	
	rest = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Clone what?\n\r", ch);
		return;
	}
	
	if (!str_prefix (arg, "object"))
	{
		mob = NULL;
		obj = get_obj_here (ch, rest);
		if (obj == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
	}
	else if (!str_prefix (arg, "mobile") || !str_prefix (arg, "character"))
	{
		obj = NULL;
		mob = get_char_room (ch, rest);
		if (mob == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
	}
	else				/* find both */
	{
		mob = get_char_room (ch, argument);
		obj = get_obj_here (ch, argument);
		if (mob == NULL && obj == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
	}
	
	/* clone an object */
	if (obj != NULL)
	{
		OBJ_DATA *clone;
		
		if (!obj_check (ch, obj))
		{
			send_to_char ("Your powers are not great enough for such a task.\n\r",
				ch);
			return;
		}
		
		clone = create_object (obj->pIndexData, 0);
		if (!clone)
		{
			send_to_char ("BUG: do_clone: Invalid 'clone' object. Logged.\n\r", ch);
			return;
		}
		clone_object (obj, clone);
		if (obj->carried_by != NULL)
			obj_to_char (clone, ch);
		else
			obj_to_room (clone, ch->in_room);
		recursive_clone (ch, obj, clone);
		
		act ("$n has created $p.", ch, clone, NULL, TO_ROOM, POS_RESTING);
		act ("You clone $p.", ch, clone, NULL, TO_CHAR, POS_DEAD);
		wiznet ("$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
		return;
	}
	else if (mob != NULL)
	{
		CHAR_DATA *clone;
		OBJ_DATA *new_obj;
		char buf[MSL];
		
		if (!IS_NPC (mob))
		{
			send_to_char ("You can only clone mobiles.\n\r", ch);
			return;
		}
		
		if ((mob->level > 20 && !IS_TRUSTED (ch, GOD))
			|| (mob->level > 10 && !IS_TRUSTED (ch, IMMORTAL))
			|| (mob->level > 5 && !IS_TRUSTED (ch, DEMI))
			|| (mob->level > 0 && !IS_TRUSTED (ch, ANGEL))
			|| !IS_TRUSTED (ch, AVATAR))
		{
			send_to_char ("Your powers are not great enough for such a task.\n\r",
				ch);
			return;
		}
		
		clone = create_mobile (mob->pIndexData);
		clone_mobile (mob, clone);
		
		for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
		{
			if (obj_check (ch, obj))
			{
				new_obj = create_object (obj->pIndexData, 0);
				if (!new_obj)
				{
					send_to_char ("BUG: do_clone:Invalid new_obj. Logged.\n\r", ch);
					return;
				}
				clone_object (obj, new_obj);
				recursive_clone (ch, obj, new_obj);
				obj_to_char (new_obj, clone);
				new_obj->wear_loc = obj->wear_loc;
			}
		}
		char_to_room (clone, ch->in_room);
		act ("$n has created $N.", ch, NULL, clone, TO_ROOM, POS_RESTING);
		act ("You clone $N.", ch, NULL, clone, TO_CHAR, POS_DEAD);
		sprintf (buf, "$N clones %s.", clone->short_descr);
		wiznet (buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
		return;
	}
}

/* RT to replace the two load commands */

_DOFUN (do_load)
{
	char arg[MIL];
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  load mob <vnum>\n\r", ch);
		send_to_char ("  load obj <vnum> <level>\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "mob") || !str_cmp (arg, "char"))
	{
		do_mload (ch, argument);
		return;
	}
	
	if (!str_cmp (arg, "obj"))
	{
		do_oload (ch, argument);
		return;
	}
	/* echo syntax */
	do_load (ch, "");
}


_DOFUN (do_mload)
{
	char arg[MIL];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;
	char buf[MSL];
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0' || !is_number (arg))
	{
		send_to_char ("Syntax: load mob <vnum>.\n\r", ch);
		return;
	}
	
	if ((pMobIndex = get_mob_index (atol (arg))) == NULL)
	{
		send_to_char ("No mob has that vnum.\n\r", ch);
		return;
	}
	
	victim = create_mobile (pMobIndex);
	char_to_room (victim, ch->in_room);
	act ("$n has created $N!", ch, NULL, victim, TO_ROOM, POS_RESTING);
	sprintf (buf, "$N loads %s.", victim->short_descr);
	wiznet (buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
	send_to_char ("Ok.\n\r", ch);
	return;
}



_DOFUN (do_oload)
{
	char arg1[MIL], arg2[MIL];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	int level;
	
	argument = one_argument (argument, arg1);
	one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || !is_number (arg1))
	{
		send_to_char ("Syntax: load obj <vnum> <level>.\n\r", ch);
		return;
	}
	
	level = get_trust (ch);	/* default */
	
	if (arg2[0] != '\0')		/* load with a level */
	{
		if (!is_number (arg2))
		{
			send_to_char ("Syntax: oload <vnum> <level>.\n\r", ch);
			return;
		}
		level = atoi (arg2);
		if (level < 0 || level > get_trust (ch))
		{
			send_to_char ("Level must be be between 0 and your level.\n\r", ch);
			return;
		}
	}
	
	if ((pObjIndex = get_obj_index (atol (arg1))) == NULL)
	{
		send_to_char ("No object has that vnum.\n\r", ch);
		return;
	}
	
	obj = create_object (pObjIndex, level);
	if (!obj)
	{
		send_to_char ("That object does not exist.\n\r", ch);
		return;
	}
	if (CAN_WEAR (obj, ITEM_TAKE))
		obj_to_char (obj, ch);
	else
		obj_to_room (obj, ch->in_room);
	act ("$n has created $p!", ch, obj, NULL, TO_ROOM, POS_RESTING);
	wiznet ("$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust (ch));
	send_to_char ("Ok.\n\r", ch);
	return;
}



_DOFUN (do_purge)
{
	char arg[MIL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	DESCRIPTOR_DATA *d;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		CHAR_DATA *vnext;
		OBJ_DATA *obj_next;
		
		for (victim = ch->in_room->people; victim != NULL; victim = vnext)
		{
			vnext = victim->next_in_room;
			if (IS_NPC (victim) &&
				!IS_FLAG (victim->states.player, _ACT_NOPURGE) &&
				!IS_FLAG (victim->states.player, _ACT_MOUNT) &&
				victim != ch /* safety precaution */ )
				extract_char (victim, TRUE);
		}
		
		for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (!IS_OBJ_STAT (obj, ITEM_NOPURGE))
				extract_obj (obj);
		}
		
		act ("$n balefires the room!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		act ("You unleash balefire in the room!", ch, NULL, NULL, TO_CHAR,
			POS_DEAD);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (!IS_NPC (victim))
	{
		
		if (ch == victim)
		{
			send_to_char ("Only Lews was ever dumb enough to balefire himself!\n\r",
				ch);
			return;
		}
		
		if (get_trust (ch) <= get_trust (victim))
		{
			send_to_char ("Maybe that wasn't a good idea...\n\r", ch);
			chprintf (victim, "%s tried to balefire you!\n\r", ch->name);
			return;
		}
		
		act ("$n balefires $N.", ch, 0, victim, TO_NOTVICT, POS_RESTING);
		act ("You balefire $N.", ch, 0, victim, TO_CHAR, POS_DEAD);
		
		if (victim->level > 1)
			save_char_obj (victim);
		
		d = victim->desc;
		extract_char (victim, TRUE);
		
		if (d != NULL)
			close_descriptor (d);
		
		return;
	}
	
	act ("$n balefires $N.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
	act ("You balefire $N.", ch, NULL, victim, TO_CHAR, POS_DEAD);
	extract_char (victim, TRUE);
	return;
}



_DOFUN (do_advance)
{
	char buf[MSL];
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;
	int level;
	int iLevel;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
	{
		send_to_char ("Syntax: advance <char> <level>.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	if ((level = atoi (arg2)) < 1 || level > 60)
	{
		send_to_char ("Level must be 1 to 60.\n\r", ch);
		return;
	}
	
	if (level > get_trust (ch))
	{
		send_to_char ("Limited to your trust level.\n\r", ch);
		return;
	}
	
	/*
	* Lower level:
	*   Reset to level 1.
	*   Then raise again.
	*   Currently, an imp can lower another imp.
	*   -- Swiftest
	*/
	if (level <= victim->level)
	{
		int temp_prac;
		
		send_to_char ("Lowering a player's level!\n\r", ch);
		send_to_char ("**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim);
		temp_prac = victim->practice;
		victim->level = 1;
		victim->exp = exp_per_level (victim, victim->pcdata->points);
		victim->max_hit = 10;
		determine_OP (victim, 100);
		victim->max_move = 100;
		victim->practice = 0;
		victim->hit = victim->max_hit;
		victim->move = victim->max_move;
		advance_level (victim, TRUE, TRUE);
		victim->practice = temp_prac;
		if (level < pMud.level_no_peace)
			victim->pcdata->peaceful = TRUE;
	}
	else
	{
		send_to_char ("Raising a player's level!\n\r", ch);
		send_to_char ("**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim);
	}
	
	for (iLevel = victim->level; iLevel < level; iLevel++)
	{
		victim->level += 1;
		advance_level (victim, TRUE, TRUE);
	}
	sprintf (buf, "You are now level %d.\n\r", victim->level);
	send_to_char (buf, victim);
	victim->exp = exp_per_level (victim, victim->pcdata->points)
		* UMAX (1, victim->level);
	victim->trust = 0;
	if (level > pMud.level_no_peace)
	{
		victim->pcdata->peaceful = FALSE;
		send_to_char ("You have lost your peaceful state of mind.\n\r", victim);
	}
	save_char_obj (victim);
	return;
}



_DOFUN (do_trust)
{
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;
	int level;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number (arg2))
	{
		send_to_char ("Syntax: trust <char> <level>.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}
	
	if ((level = atoi (arg2)) < 0 || level > 60)
	{
		send_to_char ("Level must be 0 (reset) or 1 to 60.\n\r", ch);
		return;
	}
	
	if (level > get_trust (ch))
	{
		send_to_char ("Limited to your trust.\n\r", ch);
		return;
	}
	
	victim->trust = level;
	return;
}



_DOFUN (do_restore)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	
	one_argument (argument, arg);
	if (arg[0] == '\0' || !str_cmp (arg, "room"))
	{
		/* cure room */
		
		for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
		{
			
			affect_strip (vch, skill_lookup("plague"));
			affect_strip (vch, skill_lookup("poison"));
			affect_strip (vch, skill_lookup("blind"));
			affect_strip (vch, skill_lookup("sleep"));
			
			vch->hit = vch->max_hit;
			vch->wtimer = 0;
			refresh_OP (vch, TRUE);
			vch->move = vch->max_move;
			update_pos (vch);
			act ("$n has restored you.", ch, NULL, vch, TO_VICT, POS_DEAD);
		}
		
		sprintf (buf, "$N restored room %ld.", ch->in_room->vnum);
		wiznet (buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust (ch));
		
		send_to_char ("Room restored.\n\r", ch);
		return;
		
	}
	
	if (get_trust (ch) > L5 && !str_cmp (arg, "all"))
	{
		/* cure all */
		
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			victim = d->character;
			
			if (victim == NULL || IS_NPC (victim))
				continue;
			
			affect_strip (victim, skill_lookup("plague"));
			affect_strip (victim, skill_lookup("poison"));
			affect_strip (victim, skill_lookup("blind"));
			affect_strip (victim, skill_lookup("sleep"));
			
			victim->hit = victim->max_hit;
			refresh_OP (victim, TRUE);
			victim->move = victim->max_move;
			victim->wtimer = 0;
			update_pos (victim);
			if (victim->in_room != NULL)
				act ("$n has restored you.", ch, NULL, victim, TO_VICT, POS_DEAD);
		}
		send_to_char ("All active players restored.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	affect_strip (victim, skill_lookup("plague"));
	affect_strip (victim, skill_lookup("poison"));
	affect_strip (victim, skill_lookup("blind"));
	affect_strip (victim, skill_lookup("sleep"));
	victim->hit = victim->max_hit;
	victim->move = victim->max_move;
	victim->wtimer = 0;
	refresh_OP (victim, TRUE);
	
	update_pos (victim);
	act ("$n has restored you.", ch, NULL, victim, TO_VICT, POS_DEAD);
	sprintf (buf, "$N restored %s",
		IS_NPC (victim) ? victim->short_descr : victim->name);
	wiznet (buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust (ch));
	send_to_char ("Ok.\n\r", ch);
	return;
}


_DOFUN (do_freeze)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Freeze whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.penalties, _FREEZE))
	{
		REMOVE_FLAG (victim->states.penalties, _FREEZE);
		send_to_char ("You can play again.\n\r", victim);
		send_to_char ("FREEZE removed.\n\r", ch);
		sprintf (buf, "$N thaws %s.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _FREEZE);
		send_to_char ("You can't do ANYthing!\n\r", victim);
		send_to_char ("FREEZE set.\n\r", ch);
		sprintf (buf, "$N puts %s in the deep freeze.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	
	save_char_obj (victim);
	
	return;
}



_DOFUN (do_log)
{
	char arg[MIL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Log whom?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "all"))
	{
		if (fLogAll)
		{
			fLogAll = FALSE;
			send_to_char ("Log ALL off.\n\r", ch);
		}
		else
		{
			fLogAll = TRUE;
			send_to_char ("Log ALL on.\n\r", ch);
		}
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	/*
	* No level check, gods can log anyone.
	*/
	if (IS_FLAG (victim->states.penalties, _LOG))
	{
		REMOVE_FLAG (victim->states.penalties, _LOG);
		send_to_char ("LOG removed.\n\r", ch);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _LOG);
		send_to_char ("LOG set.\n\r", ch);
	}
	
	return;
}



_DOFUN (do_noemote)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Noemote whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		chprintf (victim, "%s just tried to noemote you!\n\r", ch->name);
		return;
	}
	
	if (IS_FLAG (victim->states.penalties, _NOEMOTE))
	{
		REMOVE_FLAG (victim->states.penalties, _NOEMOTE);
		send_to_char ("You can emote again.\n\r", victim);
		send_to_char ("NOEMOTE removed.\n\r", ch);
		sprintf (buf, "$N restores emotes to %s.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _NOEMOTE);
		send_to_char ("You can't emote!\n\r", victim);
		send_to_char ("NOEMOTE set.\n\r", ch);
		sprintf (buf, "$N revokes %s's emotes.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	
	return;
}



_DOFUN (do_noshout)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Noshout whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.penalties, _NOSHOUT))
	{
		REMOVE_FLAG (victim->states.penalties, _NOSHOUT);
		send_to_char ("You can shout again.\n\r", victim);
		send_to_char ("NOSHOUT removed.\n\r", ch);
		sprintf (buf, "$N restores shouts to %s.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _NOSHOUT);
		send_to_char ("You can't shout!\n\r", victim);
		send_to_char ("NOSHOUT set.\n\r", ch);
		sprintf (buf, "$N revokes %s's shouts.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	
	return;
}



_DOFUN (do_notell)
{
	char arg[MIL], buf[MSL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Notell whom?", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (victim->states.penalties, _NOTELL))
	{
		REMOVE_FLAG (victim->states.penalties, _NOTELL);
		send_to_char ("You can tell again.\n\r", victim);
		send_to_char ("NOTELL removed.\n\r", ch);
		sprintf (buf, "$N restores tells to %s.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	else
	{
		SET_FLAG (victim->states.penalties, _NOTELL);
		send_to_char ("You can't tell!\n\r", victim);
		send_to_char ("NOTELL set.\n\r", ch);
		sprintf (buf, "$N revokes %s's tells.", victim->name);
		wiznet (buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0);
	}
	
	return;
}



_DOFUN (do_peace)
{
	CHAR_DATA *rch;
	
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		if (rch->fighting != NULL)
			stop_fighting (rch, TRUE);
		
		if (IS_NPC (rch) && IS_FLAG (rch->states.player, _ACT_AGGRESSIVE))
			REMOVE_FLAG (rch->states.player, _ACT_AGGRESSIVE);
	}
	
	send_to_char ("Ok.\n\r", ch);
	return;
}

_DOFUN (do_wizlock)
{
	if (!IS_SET (pMud.flags, MUD_IMMLOCK))
	{
		wiznet ("$N has wizlocked the game.", ch, NULL, 0, 0, 0);
		send_to_char ("Game wizlocked.\n\r", ch);
		SET_BIT (pMud.flags, MUD_IMMLOCK);
	}
	else
	{
		wiznet ("$N removes wizlock.", ch, NULL, 0, 0, 0);
		send_to_char ("Game un-wizlocked.\n\r", ch);
		REMOVE_BIT (pMud.flags, MUD_IMMLOCK);
	}
	
	return;
}


_DOFUN (do_closemud)
{
	if (!IS_SET (pMud.flags, MUD_CLOSED))
	{
		wiznet ("$N has closed the game.", ch, NULL, 0, 0, 0);
		send_to_char ("Game closed.\n\r", ch);
		SET_BIT (pMud.flags, MUD_CLOSED);
	}
	else
	{
		wiznet ("$N reopens the mud.", ch, NULL, 0, 0, 0);
		send_to_char ("Game reopened.\n\r", ch);
		REMOVE_BIT (pMud.flags, MUD_CLOSED);
	}
	
	return;
}

_DOFUN (do_newlock)
{
	if (!IS_SET (pMud.flags, MUD_NEWLOCK))
	{
		wiznet ("$N locks out new characters.", ch, NULL, 0, 0, 0);
		send_to_char ("New characters have been locked out.\n\r", ch);
		SET_BIT (pMud.flags, MUD_NEWLOCK);
	}
	else
	{
		wiznet ("$N allows new characters back in.", ch, NULL, 0, 0, 0);
		send_to_char ("Newlock removed.\n\r", ch);
		REMOVE_BIT (pMud.flags, MUD_NEWLOCK);
	}
	
	return;
}

_DOFUN (do_redirect)
{
	if (!IS_SET (pMud.flags, MUD_REDIRECT))
	{
		wiznet ("$N sets the mud to redirect.", ch, NULL, 0, 0, 0);
		send_to_char ("The mud will now redirect callers.\n\r", ch);
		SET_BIT (pMud.flags, MUD_REDIRECT);
	}
	else
	{
		wiznet ("$N cancels redirection.", ch, NULL, 0, 0, 0);
		send_to_char ("The mud no longer redirects callers.\n\r", ch);
		REMOVE_BIT (pMud.flags, MUD_REDIRECT);
	}
	
	return;
}


_DOFUN (do_slookup)
{
	char arg[MIL];
	int sn;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		chprintf(ch, "Lookup which skill or weave?\n\r");
		return;
	}
	
	if (!str_cmp (arg, "all"))
	{
		for (sn = 0; sn < maxSkill; sn++)
		{
			if (skill_table[sn].name == NULL)
				break;
			 chprintf(ch, "Sn: %3d  Skill/weave: '%s'\n\r", sn, skill_table[sn].name);
		}
	}
	else
	{
		if ((sn = skill_lookup (arg)) < 0)
		{
			chprintf(ch, "No such skill or weave.\n\r");
			return;
		}
		
		chprintf(ch, "Sn: %3d  Skill/weave: '%s'\n\r", sn, skill_table[sn].name);
	}
	
	return;
}

/* RT set replaces sset, mset, oset, and rset */

_DOFUN (do_set)
{
	char arg[MIL];
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  set mob   <name> <field> <value>\n\r", ch);
		send_to_char ("  set obj   <name> <field> <value>\n\r", ch);
		send_to_char ("  set room  <room> <field> <value>\n\r", ch);
		send_to_char ("  set skill <name> <weave or skill> <value>\n\r", ch);
		return;
	}
	
	if (!str_prefix (arg, "mobile") || !str_prefix (arg, "character"))
	{
		do_mset (ch, argument);
		return;
	}
	
	if (!str_prefix (arg, "skill") || !str_prefix (arg, "weave"))
	{
		do_sset (ch, argument);
		return;
	}
	
	if (!str_prefix (arg, "object"))
	{
		do_oset (ch, argument);
		return;
	}
	
	if (!str_prefix (arg, "room"))
	{
		do_rset (ch, argument);
		return;
	}
	/* echo syntax */
	do_set (ch, "");
}


_DOFUN (do_sset)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char wizbuf[MSL];
	CHAR_DATA *victim;
	int value;
	int sn;
	bool fAll;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  set skill <name> <weave or skill> <value>\n\r", ch);
		send_to_char ("  set skill <name> all <value>\n\r", ch);
		send_to_char ("   (use the name of the skill, not the number)\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	
	fAll = !str_cmp (arg2, "all");
	sn = 0;
	if (!fAll && (sn = skill_lookup (arg2)) < 0)
	{
		send_to_char ("No such skill or weave.\n\r", ch);
		return;
	}
	
	/*
	* Snarf the value.
	*/
	if (!is_number (arg3))
	{
		send_to_char ("Value must be numeric.\n\r", ch);
		return;
	}
	
	value = atoi (arg3);
	if (value < 0 || value > 100)
	{
		send_to_char ("Value range is 0 to 100.\n\r", ch);
		return;
	}
	
	if (fAll)
	{
		sprintf (wizbuf, "$N has 'set' all of %s's skills to %d.",
			victim->name, value);
		
		for (sn = 0; sn < victim->pcdata->iMemSize[MEM_MAXSKILL]; sn++)
		{
			
			/* Handle special circumstances for chanelling */
			if (sn == gsn_embrace)
			{
				if (victim->pcdata->true_sex == SEX_MALE)
					continue;
				
				if (!IS_FLAG (victim->states.player, _ACT_CHANNELER))
					SET_FLAG (victim->states.player, _ACT_CHANNELER);
				
				send_to_char ("You can now embrace the One Power!\n\r", victim);
				victim->pcdata->learned[sn] = value;
				determine_OP (victim, 100);
			}
			else
				if (sn == gsn_seize)
				{
					if (victim->pcdata->true_sex == SEX_FEMALE)
						continue;
					
					if (!IS_FLAG (victim->states.player, _ACT_CHANNELER))
						SET_FLAG (victim->states.player, _ACT_CHANNELER);
					
					send_to_char ("You can now seize the One Power!\n\r", victim);
					victim->pcdata->learned[sn] = value;
					determine_OP (victim, 100);
				}
				else
					if (skill_table[sn].name != NULL)
						victim->pcdata->learned[sn] = value;
		}
	}
	
	else
	{
		/* Handle special circumstances for chanelling */
		if (sn == gsn_embrace)
		{
			if (victim->pcdata->true_sex == SEX_MALE)
			{
				send_to_char ("Only women can embrace the One Power.\n\r", ch);
				return;
			}
			
			if (!IS_FLAG (victim->states.player, _ACT_CHANNELER))
				SET_FLAG (victim->states.player, _ACT_CHANNELER);
			
			send_to_char ("You can now embrace the One Power!\n\r", victim);
			victim->pcdata->learned[sn] = value;
			determine_OP (victim, 100);
		}
		else
			if (sn == gsn_seize)
			{
				if (victim->pcdata->true_sex == SEX_FEMALE)
				{
					send_to_char ("Only men can seize the One Power.\n\r", ch);
					return;
				}
				
				if (!IS_FLAG (victim->states.player, _ACT_CHANNELER))
					SET_FLAG (victim->states.player, _ACT_CHANNELER);
				
				send_to_char ("You can now seize the One Power!\n\r", victim);
				victim->pcdata->learned[sn] = value;
				determine_OP (victim, 100);
				
			}
			else
				
				victim->pcdata->learned[sn] = value;
			sprintf (wizbuf, "$N has 'set' %s's [%s] skill to %d.",
				victim->name, skill_table[sn].name, value);
	}
	
	if (wizbuf[0] != '\0')
		wiznet (wizbuf, ch, NULL, WIZ_SECURE, 0, get_trust (ch));
	return;
}

_DOFUN (do_mset)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char buf[100];
	char wizbuf[MSL];
	
	CHAR_DATA *victim;
	int value = 0;
	
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);
	
	
	if (arg1[0] == '\0' || arg2[0] == '\0' ||
		(arg3[0] == '\0' && str_prefix (arg2, "ability")))
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  set char <name> <field> <value>\n\r", ch);
		send_to_char ("  Field being one of:\n\r", ch);
		send_to_char ("    str int wis dex con sex class level\n\r", ch);
		send_to_char ("    race group gold silver hp ability endurance\n\r", ch);
		send_to_char ("    align train thirst hunger drunk full\n\r", ch);
		send_to_char ("    practice security nation age ability\n\r", ch);
		send_to_char ("    air earth water fire spirit\n\r", ch);
		if (ch->level >= L2)
			send_to_char ("    recall qpoints.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	/* clear areas for mobs */
	victim->zone = NULL;
	
	/*
	* Snarf the value (which need not be numeric).
	*/
	value = is_number (arg3) ? atoi (arg3) : -1;
	
	/*
	* Set something.
	*/
	if (!str_cmp (arg2, "ability"))
	{
		send_to_char ("Ability Rating Set.\n\r", ch);
		determine_OP (victim, 100);
	}
	else
	if (!str_cmp (arg2, "qpoints"))
	{
		if (ch->level < L2)
		{
			send_to_char ("Invalid security.\n\r", ch);
			return;
		}
		
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs!\n\r", ch);
			return;
		}
		
		if (value > 1000)
		{
			send_to_char ("Max value is 1000, adjusting.\n\r", ch);
			value = 1000;
		}
		
		if (value > -1)
		{
			chprintf (ch, "%s's quest points are now %d.\n\r",
				victim->name, value);
			victim->pcdata->quest_points = value;
			chprintf (victim, "You now have %d quest points.\n\r",
				victim->pcdata->quest_points);
			return;
		}
	}
	
	if (!str_cmp (arg2, "recall"))
	{
		
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's!\n\r", ch);
			return;
		}
		
		if (get_trust (ch) >= L2)
		{
			ROOM_INDEX_DATA *pRoom;
			
			if ((pRoom = get_room_index (value)) == NULL)
			{
				send_to_char ("That room dosen't exist.\n\r", ch);
				return;
			}
			
			if ((pRoom->clan > -1) && pRoom->clan != victim->clan)
			{
				chprintf (ch, "That room is reserved for members of the\n\r"
					"%s guild only.\n\r", clan_table[pRoom->clan].name);
				return;
			}
			send_to_char ("Recall set.\n\r", ch);
			victim->pcdata->recall_to = value;
		}
		else
		{
			send_to_char ("Invalid security.\n\r", ch);
		}
	}
	else
	if (!str_cmp (arg2, "bounty"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's!\n\r", ch);
			return;
		}
		
		if (value < 1)
		{
			send_to_char ("Value must be a positive number.\n\r", ch);
			return;
		}
		
		victim->pcdata->bounty = value;
	}
	else
	if (!str_cmp (arg2, "age"))
	{
		if (value < 1)
		{
			send_to_char ("Age must be greater than 0.\n\r", ch);
			return;
		}
		victim->age = value * 365;
	}
	else
	
	if (!str_cmp (arg2, "str"))
	{
		
		if (value < 3 || value > get_max_train (victim, STAT_STR))
		{
			sprintf (buf,
				"Strength range is 3 to %d\n\r.",
				get_max_train (victim, STAT_STR));
			send_to_char (buf, ch);
			return;
		}
		
		victim->perm_stat[STAT_STR] = value;
	}
	else
	if (!str_cmp (arg2, "nation"))
	{
		int i, y;
		
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if ((city_lookup (argument) == NULL))
		{
			send_to_char ("Valid Nations:\n\r", ch);
			for (i = 0; i < MAX_NATIONS; i++)
			{
				if (!IS_NULLSTR(nation_table[i].nation))
				{
					chprintf (ch, "%s\n\r", nation_table[i].nation);
					for (y = 0; y < MAX_CITIES; y++)
						if (nation_table[i].cities[y].city != NULL
							&& nation_table[i].cities[y].city[0] != '\0')
						{
							chprintf (ch, "  City: %s\n\r",
								nation_table[i].cities[y].city);
						}
				}
			}
			send_to_char ("Nation is set automatically by setting the city.\n\r",
				ch);
			return;
		}
		
		/* find the country, by finding the city. set country to i, city to y */
		for (i = 0; i < MAX_NATIONS; i++)
		{
			for (y = 0; y < MAX_CITIES; y++)
				if (nation_table[i].cities[y].city
					&& nation_table[i].cities[y].city[0] != '\0'
					&& !str_cmp (argument, nation_table[i].cities[y].city))
				{
					free_string (victim->pcdata->nation);
					free_string (victim->pcdata->city);
					victim->pcdata->nation = str_dup (nation_table[i].nation);
					victim->pcdata->city = str_dup (nation_table[i].cities[y].city);
				}
		}
		
		chprintf (ch, "%s is now from the city of %s in %s.\n\r",
			victim->name, victim->pcdata->city,
			victim->pcdata->nation);
		
		chprintf (victim, "You are now from the city of %s in %s.\n\r",
			victim->pcdata->city, victim->pcdata->nation);
		
	}
	else
	if (!str_cmp (arg2, "security"))	/* OLC */
	{
		if (IS_NPC (ch))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (value < 0)
		{
			if (ch->pcdata->security != 0 || value > get_trust (ch))
			{
				sprintf (buf, "Valid security is 0-%d.\n\r", get_trust (ch));
				send_to_char (buf, ch);
			}
			else
			{
				send_to_char ("Valid security is 0 only.\n\r", ch);
			}
			return;
		}
		victim->pcdata->security = value;
	}
	else
	if (!str_cmp (arg2, "int"))
	{
		if (value < 3 || value > get_max_train (victim, STAT_INT))
		{
			sprintf (buf,
				"Intelligence range is 3 to %d.\n\r",
				get_max_train (victim, STAT_INT));
			send_to_char (buf, ch);
			return;
		}
		
		victim->perm_stat[STAT_INT] = value;
	}
	else
	if (!str_cmp (arg2, "agl"))
	{
		if (value < 3 || value > get_max_train (victim, STAT_AGL))
		{
			sprintf (buf,
				"Agility range is 3 to %d.\n\r", get_max_train (victim,
				STAT_AGL));
			send_to_char (buf, ch);
			return;
		}
		
		victim->perm_stat[STAT_AGL] = value;
	}
	else
	if (!str_cmp (arg2, "dex"))
	{
		if (value < 3 || value > get_max_train (victim, STAT_DEX))
		{
			sprintf (buf,
				"Dexterity ranges is 3 to %d.\n\r",
				get_max_train (victim, STAT_DEX));
			send_to_char (buf, ch);
			return;
		}
		
		victim->perm_stat[STAT_DEX] = value;
	}
	else
	if (!str_cmp (arg2, "con"))
	{
		if (value < 3 || value > get_max_train (victim, STAT_CON))
		{
			sprintf (buf,
				"Constitution range is 3 to %d.\n\r",
				get_max_train (victim, STAT_CON));
			send_to_char (buf, ch);
			return;
		}
		
		victim->perm_stat[STAT_CON] = value;
	}
	else
	if (!str_prefix (arg2, "sex"))
	{
		if (value < 0 || value > 2)
		{
			send_to_char ("Sex range is 0 to 2.\n\r", ch);
			return;
		}
		victim->sex = value;
		if (!IS_NPC (victim))
		{
			int gsn = gsn_seize;
			int gsn2 = gsn_embrace;
			if (get_skill (victim, gsn) > 0 && value == SEX_FEMALE)
			{
				victim->pcdata->learned[gsn2] =
					victim->pcdata->learned[gsn];
				victim->pcdata->learned[gsn] = 0;
			}
			else if (get_skill (victim, gsn2) > 0 && value != SEX_FEMALE)
			{
				victim->pcdata->learned[gsn] =
					victim->pcdata->learned[gsn2];
				victim->pcdata->learned[gsn] = 0;
			}
			
			victim->pcdata->true_sex = value;
		}
	}
	else
	if (!str_prefix (arg2, "class"))
	{
		int c_class;
		
		if (IS_NPC (victim))
		{
			send_to_char ("Mobiles have no class.\n\r", ch);
			return;
		}
		
		c_class = class_lookup (arg3);
		if (c_class == -1)
		{
			char buf[MSL];
			
			strcpy (buf, "Possible classes are: ");
			for (c_class = 0; c_class < MAX_CLASS; c_class++)
			{
				if (c_class > 0)
					strcat (buf, " ");
				strcat (buf, class_table[c_class].name);
			}
			strcat (buf, ".\n\r");
			
			send_to_char (buf, ch);
			return;
		}
		
		victim->c_class = c_class;
		
		if (c_class == CHANNELER)
		{
			if (!IS_FLAG (ch->states.player, _ACT_CHANNELER))
				SET_FLAG (ch->states.player, _ACT_CHANNELER);
		}
		
	}
	else
	if (!str_prefix (arg2, "level"))
	{
		if (!IS_NPC (victim))
		{
			send_to_char ("Not on PC's.\n\r", ch);
			return;
		}
		
		if (value < 0 || value > 60)
		{
			send_to_char ("Level range is 0 to 60.\n\r", ch);
			return;
		}
		victim->level = value;
	}
	else
	if (!str_prefix (arg2, "gold"))
	{
		victim->gold = value;
	}
	else
	if (!str_prefix (arg2, "silver"))
	{
		victim->silver = value;
	}
	else
	if (!str_prefix (arg2, "hp"))
	{
		if (value < -10 || value > 30000)
		{
			send_to_char ("Hp range is -10 to 30,000 hit points.\n\r", ch);
			return;
		}
		victim->max_hit = value;
		if (!IS_NPC (victim))
			victim->pcdata->perm_hit = value;
		
	}
	else
	if (!str_prefix (arg2, "air") || !str_prefix (arg2, "earth") ||
		!str_prefix (arg2, "water") || !str_prefix (arg2, "fire") ||
		!str_prefix (arg2, "spirit"))
	{
		
		if (value < 0 || value > 8)
		{
			send_to_char ("Ability range is 0 to 8 Ability points.\n\r", ch);
			return;
		}
		
		switch (*arg2)
		{
			
			case 'A':
			case 'a':
				victim->flow_str.current[AIR] = value;
				victim->flow_str.perm[AIR] = value;
				send_to_char ("Air Set.\n\r", ch);
				break;
				
			case 'E':
			case 'e':
				victim->flow_str.current[EARTH] = value;
				victim->flow_str.perm[EARTH] = value;
				send_to_char ("Earth Set.\n\r", ch);
				break;
				
			case 'W':
			case 'w':
				victim->flow_str.current[WATER] = value;
				victim->flow_str.perm[WATER] = value;
				send_to_char ("Water Set.\n\r", ch);
				break;
				
			case 'F':
			case 'f':
				victim->flow_str.current[FIRE] = value;
				victim->flow_str.perm[FIRE] = value;
				send_to_char ("Fire Set.\n\r", ch);
				break;
				
			case 'S':
			case 's':
				victim->flow_str.current[SPIRIT] = value;
				victim->flow_str.perm[SPIRIT] = value;
				send_to_char ("Spirit Set.\n\r", ch);
				break;
			default:
				send_to_char ("That flow does not exist.\n\r", ch);
		}
		
	}
	else
	if (!str_prefix (arg2, "endurance"))
	{
		if (value < 0 || value > 30000)
		{
			send_to_char ("Endurance range is 0 to 30,000 move points.\n\r", ch);
			return;
		}
		victim->max_move = value;
		if (!IS_NPC (victim))
			victim->pcdata->perm_move = value;
	}
	else
	if (!str_prefix (arg2, "practice"))
	{
		if (value < 0 || value > 250)
		{
			send_to_char ("Practice range is 0 to 250 sessions.\n\r", ch);
			return;
		}
		victim->practice = value;
	}
	else
	if (!str_prefix (arg2, "train"))
	{
		if (value < 0 || value > 50)
		{
			send_to_char ("Training session range is 0 to 50 sessions.\n\r", ch);
			return;
		}
		victim->train = value;
	}
	else
	if (!str_prefix (arg2, "align"))
	{
		if (value < -1000 || value > 1000)
		{
			send_to_char ("Alignment range is -1000 to 1000.\n\r", ch);
			return;
		}
		victim->alignment = value;
	}
	else
	if (!str_prefix (arg2, "thirst"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (value < -1 || value > 100)
		{
			send_to_char ("Thirst range is -1 to 100.\n\r", ch);
			return;
		}
		
		victim->pcdata->condition[_THIRST] = value;
	}
	else
	if (!str_prefix (arg2, "drunk"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (value < -1 || value > 100)
		{
			send_to_char ("Drunk range is -1 to 100.\n\r", ch);
			return;
		}
		
		victim->pcdata->condition[_DRUNK] = value;
	}
	else
	if (!str_prefix (arg2, "full"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (value < -1 || value > 100)
		{
			send_to_char ("Full range is -1 to 100.\n\r", ch);
			return;
		}
		
		victim->pcdata->condition[_FULL] = value;
	}
	else
	if (!str_prefix (arg2, "hunger"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		
		if (value < -1 || value > 100)
		{
			send_to_char ("Full range is -1 to 100.\n\r", ch);
			return;
		}
		
		victim->pcdata->condition[_HUNGER] = value;
	}
	else
	if (!str_prefix (arg2, "race"))
	{
		int race;
		
		race = race_lookup (arg3);
		
		if (race == -1)
		{
			send_to_char ("That is not a valid race.\n\r", ch);
			return;
		}
		
		victim->race = race;
	}
	else
	if (!str_prefix (arg2, "group"))
	{
		if (!IS_NPC (victim))
		{
			send_to_char ("Only on NPCs.\n\r", ch);
			return;
		}
		victim->group = value;
	}
	else
	/* 
	 * Generate usage message.
	 */
	do_mset (ch, "");
	
	/*
	 * Report the command use
	 */
	sprintf (wizbuf, "$N mset: %s %s %s", victim->name, arg2, arg3);
	wiznet (wizbuf, ch, NULL, WIZ_SECURE, 0, get_trust (ch));
	
	return;
}


_DOFUN (do_string)
{
	char type[MIL];
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	
	argument = one_argument (argument, type);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);
	
	if (type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0'
		|| arg3[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  string char <name> <field> <string>\n\r", ch);
		send_to_char ("    fields: name short long desc title spec", ch);
		if (get_trust (ch) > L3)
			send_to_char (" hide\n\r", ch);
		else
			send_to_char ("\n\r", ch);
		send_to_char ("  string obj  <name> <field> <string>\n\r", ch);
		send_to_char ("    fields: name short long extended\n\r", ch);
		return;
	}
	
	if (!str_prefix (type, "character") || !str_prefix (type, "mobile"))
	{
		if ((victim = get_char_world (ch, arg1)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}
		
		/* clear area for mobs */
		victim->zone = NULL;
		
		/* string something */
		
		if (!str_prefix (arg2, "name"))
		{
			if (!IS_NPC (victim))
			{
				send_to_char ("Not on PC's.\n\r", ch);
				return;
			}
			free_string (victim->name);
			victim->name = str_dup (arg3);
			return;
		}
		
		if (get_trust (ch) > L2 && !str_prefix (arg2, "hide")
			&& !IS_NPC (victim) && arg3[0] != '\0')
		{
			free_string (victim->pcdata->hidden_as);
			victim->pcdata->hidden_as = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "description"))
		{
			free_string (victim->description);
			victim->description = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "short"))
		{
			free_string (victim->short_descr);
			victim->short_descr = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "long"))
		{
			free_string (victim->long_descr);
			strcat (arg3, "\n\r");
			victim->long_descr = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "title"))
		{
			if (IS_NPC (victim))
			{
				send_to_char ("Not on NPC's.\n\r", ch);
				return;
			}
			
			set_title (victim, arg3);
			return;
		}
		
		if (!str_prefix (arg2, "spec"))
		{
			if (!IS_NPC (victim))
			{
				send_to_char ("Not on PC's.\n\r", ch);
				return;
			}
			
			if ((victim->spec_fun = spec_lookup (arg3)) == 0)
			{
				send_to_char ("No such spec fun.\n\r", ch);
				return;
			}
			
			return;
		}
	}
	
	if (!str_prefix (type, "object"))
	{
		/* string an obj */
		
		if ((obj = get_obj_world (ch, arg1)) == NULL)
		{
			send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
			return;
		}
		
		/* cant string "quest" items */
		if (IS_SET (obj->extra_flags, ITEM_QUEST))
		{
			send_to_char ("Quest prizes may not be restrung.\n\r", ch);
			return;
		}
		
		if (IS_SET (obj->extra_flags, ITEM_TREESUNG))
		{
			send_to_char ("Treesung items may not be restrung.\n\r", ch);
			return;
		}
		
		if (IS_SET (obj->extra_flags, ITEM_GUILD))
		{
			send_to_char ("Guild items may not be restrung.\n\r", ch);
			return;
		}
		
		if (!str_prefix (arg2, "name"))
		{
			free_string (obj->name);
			obj->name = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "short"))
		{
			free_string (obj->short_descr);
			obj->short_descr = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "long"))
		{
			free_string (obj->description);
			obj->description = str_dup (arg3);
			return;
		}
		
		if (!str_prefix (arg2, "ed") || !str_prefix (arg2, "extended"))
		{
			EXTRA_DESCR_DATA *ed;
			
			argument = one_argument (argument, arg3);
			if (argument == NULL)
			{
				send_to_char ("Syntax: oset <object> ed <keyword> <string>\n\r", ch);
				return;
			}
			
			strcat (argument, "\n\r");
			
			ed = new_extra_descr ();
			
			ed->keyword = str_dup (arg3);
			ed->description = str_dup (argument);
			ed->next = obj->extra_descr;
			obj->extra_descr = ed;
			return;
		}
	}
	
	
	/* echo bad use message */
	do_string (ch, "");
}



_DOFUN (do_oset)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char wizbuf[MSL];
	OBJ_DATA *obj;
	int value;
	
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  set obj <object> <field> <value>\n\r", ch);
		send_to_char ("  Field being one of:\n\r", ch);
		send_to_char ("    value0 value1 value2 value3 value4 (v1-v4)\n\r", ch);
		send_to_char ("    extra wear level weight cost timer\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_world (ch, arg1)) == NULL)
	{
		send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
		return;
	}
	
	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = atoi (arg3);
	
    /*
	 * Report Command usage
	 */
	sprintf (wizbuf, "$N oset: %s '%s' value to: %s",
		obj->short_descr, arg2, arg3);
	wiznet (wizbuf, ch, NULL, WIZ_SECURE, 0, get_trust (ch));
	
	/*
	 * Set something.
	 */
	if (!str_cmp (arg2, "value0") || !str_cmp (arg2, "v0"))
	{
		obj->value[0] = UMIN (50, value);
		return;
	}
	
	if (!str_cmp (arg2, "value1") || !str_cmp (arg2, "v1"))
	{
		obj->value[1] = value;
		return;
	}
	
	if (!str_cmp (arg2, "value2") || !str_cmp (arg2, "v2"))
	{
		obj->value[2] = value;
		return;
	}
	
	if (!str_cmp (arg2, "value3") || !str_cmp (arg2, "v3"))
	{
		obj->value[3] = value;
		return;
	}
	
	if (!str_cmp (arg2, "value4") || !str_cmp (arg2, "v4"))
	{
		obj->value[4] = value;
		return;
	}
	
	if (!str_prefix (arg2, "extra"))
	{
		obj->extra_flags = value;
		return;
	}
	
	if (!str_prefix (arg2, "wear"))
	{
		obj->wear_flags = value;
		return;
	}
	
	if (!str_prefix (arg2, "level"))
	{
		obj->level = value;
		return;
	}
	
	if (!str_prefix (arg2, "weight"))
	{
		obj->weight = value;
		return;
	}
	
	if (!str_prefix (arg2, "cost"))
	{
		obj->cost = value;
		return;
	}
	
	if (!str_prefix (arg2, "timer"))
	{
		obj->timer = value;
		return;
	}
	
	/*
	 * Generate usage message.
	 */
	do_oset (ch, "");
	return;
}



_DOFUN (do_rset)
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	ROOM_INDEX_DATA *location;
	int value;
	
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  set room <location> <field> <value>\n\r", ch);
		send_to_char ("  Field being one of:\n\r", ch);
		send_to_char ("    flags sector\n\r", ch);
		return;
	}
	
	if ((location = find_location (ch, arg1)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}
	
	if (!is_room_owner (ch, location) && ch->in_room != location
		&& room_is_private (location) && !IS_TRUSTED (ch, IMPLEMENTOR))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}
	
	/*
	 * Snarf the value.
	 */
	if (!is_number (arg3))
	{
		send_to_char ("Value must be numeric.\n\r", ch);
		return;
	}
	value = atoi (arg3);
	
	/*
	 * Set something.
	 */
	if (!str_prefix (arg2, "flags"))
	{
		location->room_flags = value;
		return;
	}
	
	if (!str_prefix (arg2, "sector"))
	{
		location->sector_type = value;
		return;
	}
	
	/*
	 * Generate usage message.
	 */
	do_rset (ch, "");
	return;
}

/** Function: do_sockets
  * Descr   : Display connected state of various ports along with their
  *         : hostname if applicable.
  * Returns : output only
  * Syntax  : (n/a)
  * Written : v1.0 8/98
  * Author  : Silverhand. Updated by Gary McNickle (gary@tarmongaidon.org)
  */
_DOFUN (do_sockets)
{
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	char buf[MSL];
	char buf2[MSL * 2];
	char host[MIL];
	int count;
	char *st;
	char s[100];
	char idle[10];
	
	
	count = 0;
	buf[0] = '\0';
	buf2[0] = '\0';
	
	/* Added a nicer look here, just like the look and feel much better
	-Silverhand */
	strcat (buf2, "\n\r`c:`b"
		"==========================================================================="
		"`c:`w\n\r"
		"`b|<><><><><><><><><><><><><><>     `y Sockets `b     <><><><><><><><><><><><><><>|"
		"`w\n\r`c:`b"
		"==========================================================================="
		"`c:`w\n\r`b"
		"|  [`cNum State    Login`b@`c  Idl`b] [ `cPlayer Name`b ] [  `cHosts`b  ]          "
		"         |`w\n\r`c:`b"
		"===========================================================================`c:`w\n\r");
	
	for (d = descriptor_list; d; d = d->next)
	{
		if (d->character && can_see (ch, d->character))
		{
			switch (d->connected)
			{
			case CON_PLAYING:
				st = "PLAYING ";
				break;
			case CON_GET_NAME:
				st = "Get Name";
				break;
			case CON_GET_OLD_PASSWORD:
				st = "Passwd  ";
				break;
			case CON_CONFIRM_NEW_NAME:
				st = "New Nam ";
				break;
			case CON_GET_NEW_PASSWORD:
				st = "New Pwd ";
				break;
			case CON_CONFIRM_NEW_PASSWORD:
				st = "Con Pwd ";
				break;
			case CON_GET_NEW_RACE:
				st = "New Rac ";
				break;
			case CON_GET_NEW_SEX:
				st = "New Sex ";
				break;
			case CON_ROLL_SSTATS:
				st = "New Stat";
				break;
			case CON_GET_ALIGNMENT:
				st = "New Aln ";
				break;
			case CON_GEN_GROUPS:
				st = " Custom ";
				break;
			case CON_PICK_WEAPON:
				st = " Weapon ";
				break;
			case CON_READ_IMOTD:
				st = " IMOTD  ";
				break;
			case CON_BREAK_CONNECT:
				st = "LINKDEAD";
				break;
			case CON_READ_MOTD:
				st = "  MOTD  ";
				break;
			case CON_ROLL_STATS:
				st = " Reroll ";
				break;
			case CON_GET_NEW_PORT:
				st = "New Natn";
				break;
			case CON_CHOOSE_CHANNEL:
				st = "Choose C";
				break;
			case CON_NOTE_TO:
			case CON_NOTE_SUBJECT:
			case CON_NOTE_EXPIRE:
			case CON_NOTE_TEXT:
			case CON_NOTE_FINISH:
				st = "  Note  ";
				break;
			case CON_GET_COLOR:
				st = "Get Clr ";
				break;
			case CON_GET_NEW_CITY:
				st = "Get City";
				break;
			default:
				st = "UNKNOWN!";
				break;
			}
			count++;
			
			/* Format "login" value... */
			vch = d->original ? d->original : d->character;
			
			strftime (s, 100, "%I:%M%p", localtime (&vch->logon));
			
			if (vch->timer > 0)
				sprintf (idle, "%-2d", vch->timer);
			else
				sprintf (idle, "  ");
			
			/* 
			 * The 'hidden_as' field is used for implementors to allow them
			 * to hide their alts from other imms. The field can only be
			 * edited by editing the pfile, or with the string command by
			 * imps level L1 or greater.
             */
			if (vch->pcdata->hidden_as
				&& vch->pcdata->hidden_as[0] != '\0' && get_trust (ch) < L1)
				sprintf (host, "%-28.28s", vch->pcdata->hidden_as);
			else
				sprintf (host, "%-28.28s", d->host);
			
			sprintf (buf,
				"`b|  [`c%-3d %s %s  %s`b]  `c%s   `c %s `b|`N\n\r",
				d->descriptor,
				strcap_color (8, st),
				strcap_color (7, s),
				strcap_color (2, idle),
				(d->original) ? strcap_color (12, d->original->name)
				: (d->character) ? strcap_color (12, d->character->name)
				: "(None!)", strcap_color (28, host));
			
			strcat (buf2, buf);
			
    }
  }
  
  strcat (buf2,
	  "`b|								            |`N");
  sprintf (buf,
	  "\n\r`b|  Users:`c%-2d `b                                                                |`N\n\r",
	  count);
  strcat (buf2, buf);
  strcat (buf2, "`c:`b================================================="
	  "==========================`c:`w\n\r");
  send_to_char (buf2, ch);
  return;
}




/* Thanks to Grodyn for pointing out bugs in this function.  */
_DOFUN (do_force)
{
	char buf[MSL];
	char arg[MIL];
	char arg2[MIL];
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Force whom to do what?\n\r", ch);
		return;
	}
	
	one_argument (argument, arg2);
	
	if (!str_cmp (arg2, "delete") || !str_prefix (arg2, "mob"))
	{
		send_to_char ("That will NOT be done.\n\r", ch);
		return;
	}
	
	sprintf (buf, "$n forces you to '%s'.", argument);
	
	if (!str_cmp (arg, "all"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
		
		if (get_trust (ch) < MAX_LEVEL - 3)
		{
			send_to_char ("Not at your level!\n\r", ch);
			return;
		}
		
		for (vch = char_list; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next;
			
			if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch))
			{
				act (buf, ch, NULL, vch, TO_VICT, POS_DEAD);
				interpret (vch, argument);
			}
		}
	}
	else if (!str_cmp (arg, "players"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
		
		if (get_trust (ch) < MAX_LEVEL - 2)
		{
			send_to_char ("Not at your level!\n\r", ch);
			return;
		}
		
		for (vch = char_list; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next;
			
			if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch)
				&& vch->level < LEVEL_HERO)
			{
				act (buf, ch, NULL, vch, TO_VICT, POS_DEAD);
				interpret (vch, argument);
			}
		}
	}
	else if (!str_cmp (arg, "gods"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
		
		if (get_trust (ch) < MAX_LEVEL - 2)
		{
			send_to_char ("Not at your level!\n\r", ch);
			return;
		}
		
		for (vch = char_list; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next;
			
			if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch)
				&& vch->level >= LEVEL_HERO)
			{
				act (buf, ch, NULL, vch, TO_VICT, POS_DEAD);
				interpret (vch, argument);
			}
		}
	}
	else
	{
		CHAR_DATA *victim;
		
		if ((victim = get_char_world (ch, arg)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}
		
		if (victim == ch)
		{
			send_to_char ("Aye aye, right away!\n\r", ch);
			return;
		}
		
		if (!is_room_owner (ch, victim->in_room)
			&& ch->in_room != victim->in_room
			&& room_is_private (victim->in_room) && !IS_TRUSTED (ch, IMPLEMENTOR))
		{
			send_to_char ("That character is in a private room.\n\r", ch);
			return;
		}
		
		if (get_trust (victim) >= get_trust (ch))
		{
			send_to_char ("Do it yourself!\n\r", ch);
			return;
		}
		
		if (!IS_NPC (victim) && get_trust (ch) < MAX_LEVEL - 3)
		{
			send_to_char ("Not at your level!\n\r", ch);
			return;
		}
		
		act (buf, ch, NULL, victim, TO_VICT, POS_DEAD);
		interpret (victim, argument);
	}
	
	send_to_char ("Ok.\n\r", ch);
	return;
}



/*
 * New routines by Dionysos.
 */
_DOFUN (do_invis)
{
	int level;
	char arg[MSL];
	
	/* RT code for taking a level argument */
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
		/* take the default path */
		
		if (ch->invis_level)
		{
			ch->invis_level = 0;
			act ("$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM,
				POS_RESTING);
			send_to_char ("You slowly fade back into existence.\n\r", ch);
		}
		else
		{
			ch->invis_level = get_trust (ch);
			act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM,
				POS_RESTING);
			send_to_char ("You slowly vanish into thin air.\n\r", ch);
		}
		else
		/* do the level thing */
		{
			level = atoi (arg);
			if (level < 2 || level > get_trust (ch))
			{
				send_to_char ("Invis level must be between 2 and your level.\n\r", ch);
				return;
			}
			else
			{
				ch->reply = NULL;
				ch->invis_level = level;
				act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM,
					POS_RESTING);
				send_to_char ("You slowly vanish into thin air.\n\r", ch);
			}
		}
		
		return;
}


_DOFUN (do_incognito)
{
	int level;
	char arg[MSL];
	
	/* RT code for taking a level argument */
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
		/* take the default path */
		
		if (ch->incog_level)
		{
			ch->incog_level = 0;
			act ("$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			send_to_char ("You are no longer cloaked.\n\r", ch);
		}
		else
		{
			ch->incog_level = get_trust (ch);
			act ("$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			send_to_char ("You cloak your presence.\n\r", ch);
		}
		else
		/* do the level thing */
		{
			level = atoi (arg);
			if (level < 2 || level > get_trust (ch))
			{
				send_to_char ("Incog level must be between 2 and your level.\n\r", ch);
				return;
			}
			else
			{
				ch->reply = NULL;
				ch->incog_level = level;
				act ("$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				send_to_char ("You cloak your presence.\n\r", ch);
			}
		}
		
		return;
}



_DOFUN (do_holylight)
{
	if (IS_NPC (ch))
		return;
	
	if (IS_FLAG (ch->states.config, _HOLYLIGHT))
	{
		REMOVE_FLAG (ch->states.config, _HOLYLIGHT);
		send_to_char ("Holy light mode off.\n\r", ch);
	}
	else
	{
		SET_FLAG (ch->states.config, _HOLYLIGHT);
		send_to_char ("Holy light mode on.\n\r", ch);
	}
	
	return;
}

/* prefix command: it will put the string typed on each line typed */
_DOFUN (do_prefix)
{
	char buf[MIL];
	
	if (argument[0] == '\0')
	{
		if (ch->prefix[0] == '\0')
		{
			send_to_char ("You have no prefix to clear.\r\n", ch);
			return;
		}
		
		send_to_char ("Prefix removed.\r\n", ch);
		free_string (ch->prefix);
		ch->prefix = str_dup ("");
		return;
	}
	
	if (ch->prefix[0] != '\0')
	{
		sprintf (buf, "Prefix changed to %s.\r\n", argument);
		free_string (ch->prefix);
	}
	else
	{
		sprintf (buf, "Prefix set to %s.\r\n", argument);
	}
	
	ch->prefix = str_dup (argument);
}


_DOFUN (do_rename)
{
	char old_name[MIL], new_name[MIL], strsave[MIL];
	
	CHAR_DATA *victim;
	FILE *file;
	
	argument = one_argument (argument, old_name);	/* find new/old name */
	one_argument (argument, new_name);
	
	/* Trivial checks */
	if (!old_name[0])
	{
		send_to_char ("Rename who?\n\r", ch);
		return;
	}
	
	victim = get_char_world (ch, old_name);
	
	if (!victim)
	{
		send_to_char ("There is no such person online.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim))
	{
		send_to_char ("You cannot use Rename on NPCs.\n\r", ch);
		return;
	}
	
	/* allow rename self new_name,but otherwise only lower level */
	if ((victim != ch) && (get_trust (victim) >= get_trust (ch)))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if ((victim == ch) && (get_trust (ch) < L2))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}
	
	if (!victim->desc || (victim->desc->connected != CON_PLAYING))
	{
		send_to_char ("This player has lost his link or is"
			" inside a pager or the like.\n\r", ch);
		return;
	}
	
	if (!new_name[0])
	{
		send_to_char ("Rename to what new name?\n\r", ch);
		return;
	}
	
	if (!check_parse_name (new_name))
	{
		send_to_char ("The new name is illegal.\n\r", ch);
		return;
	}
	
	sprintf (strsave, "%s%c/%s",
		PLAYER_DIR,
		UPPER (new_name[0]), capitalize (fix_name (new_name, FALSE)));
	
	file = fopen (strsave, "r");	/* attempt to to open pfile */
	if (file)
	{
		send_to_char ("A player with that name already exists!\n\r", ch);
		fclose (file);
		return;
	}
	
	/* Check .gz file ! */
	sprintf (strsave, "%s%c/%s.gz",
		PLAYER_DIR,
		UPPER (new_name[0]), capitalize (fix_name (new_name, FALSE)));
	
	file = fopen (strsave, "r");	/* attempt to open pfile */
	if (file)
	{
		send_to_char ("A player with that name already exists in"
			" a compressed file!\n\r", ch);
		fclose (file);
		return;
	}
	
	if (get_char_world (ch, new_name))
	/* check for playing level-1 non-saved */
	{
		send_to_char ("A player with the name you specified already"
			" exists!\n\r", ch);
		return;
	}
	
	/* Save the filename of the old name */
	sprintf (strsave, "%s%c/%s",
		PLAYER_DIR,
		UPPER (victim->name[0]),
		capitalize (fix_name (victim->name, FALSE)));
	
	/* Rename the character and save him to a new file */
	/* NOTE: Players who are level 1 do NOT get saved under a new name */
	
	free_string (victim->name);
	victim->name = str_dup (capitalize (new_name));
	
	save_char_obj (victim);
	
	/* unlink the old file */
	unlink (strsave);		/* unlink does return a value.. but we do not care */
	
	/* That's it! */
	send_to_char ("Character renamed.\n\r", ch);
	
	victim->position = POS_STANDING;	
	act ("$n has renamed you to $N!", ch, NULL, victim, TO_VICT, POS_DEAD);
	
} /* do_rename */


/* olevel and mlevel
 * Originally written by LordRom (omar@physio.mcgill.ca)
 * Revised by  Kyoung (uyunk00@mcl.ucsb.edu)
 */
_DOFUN (do_olevel)
{
	char buf[MIL];
	char level[MIL];
	char name[MIL];
	BUFFER *buffer;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	int number = 0, max_found;
	
	found = FALSE;
	number = 0;
	max_found = 200;
	
	buffer = new_buf ();
	
	argument = one_argument (argument, level);
	if (level[0] == '\0')
	{
		send_to_char ("Syntax: olevel <level>\n\r", ch);
		send_to_char ("        olevel <level> <name>\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, name);
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (obj->level != atoi (level))
			continue;
		
		if (name[0] != '\0' && !is_name (name, obj->name))
			continue;
		
		found = TRUE;
		number++;
		
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);
		
		if (in_obj->carried_by != NULL
			&& can_see (ch, in_obj->carried_by)
			&& in_obj->carried_by->in_room != NULL)
		{
			sprintf (buf, "%3d) %s is carried by %s [Room %ld]\n\r",
				number, obj->short_descr, PERS (in_obj->carried_by, ch),
				in_obj->carried_by->in_room->vnum);
		}
		else
			if (in_obj->in_room != NULL && can_see_room (ch, in_obj->in_room))
			{
				sprintf (buf, "%3d) %s is in %s [Room %ld]\n\r",
					number, obj->short_descr, in_obj->in_room->name,
					in_obj->in_room->vnum);
			}
			else
				sprintf (buf, "%3d) %s is somewhere\n\r", number, obj->short_descr);
			
			buf[0] = UPPER (buf[0]);
			add_buf (buffer, buf);
			
			if (number >= max_found)
				break;
	}
	
	if (!found)
		send_to_char ("Nothing like that in heaven or earth.\n\r", ch);
	else
		page_to_char (buf_string (buffer), ch);
	
	free_buf (buffer);
}

_DOFUN (do_mlevel)
{
	char buf[MIL];
	BUFFER *buffer;
	CHAR_DATA *victim;
	bool found;
	int count = 0;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Syntax: mlevel <level>\n\r", ch);
		return;
	}
	
	found = FALSE;
	buffer = new_buf ();
	
	for (victim = char_list; victim != NULL; victim = victim->next)
	{
		if (victim->in_room != NULL
			&& can_see (ch, victim) && atoi (argument) == victim->level)
		{
			found = TRUE;
			count++;
			sprintf (buf, "%3d) [%5ld] %-28s [%5ld] %s\n\r", count,
				IS_NPC (victim) ? victim->pIndexData->vnum : 0,
				IS_NPC (victim) ? victim->short_descr : victim->name,
				victim->in_room->vnum, victim->in_room->name);
			add_buf (buffer, buf);
		}
	}
	
	if (!found)
		act ("You didn't find any mob of level $T.", ch, NULL, argument, TO_CHAR,
		POS_DEAD);
	else
		page_to_char (buf_string (buffer), ch);
	
	free_buf (buffer);
	
	return;
}


/* otype
 * This code was written by Kadian of Distant Thunder if you have any
 * problems with this code feel free to e-mail at kadian@nurf.com ...no 
 * need to give any credit...it's a tiny snippet :)
 */
_DOFUN (do_otype)
{
	int type;
	int type2;
	long vnum = 1;
	char buf[MSL];
	char buffer[12 * MSL];
	char arg1[MIL];
	char arg2[MIL];
	char *item;
	OBJ_INDEX_DATA *obj;
	bool found;
	
	
	item = one_argument (argument, arg1);
	
	one_argument (item, arg2);
	
	found = FALSE;
	buffer[0] = '\0';
	
	if (arg1[0] == '\0')
	{
		send_to_char ("Type 'Help Otype' for usage\n\r", ch);
		return;
	}
	
	type2 = 0;
	
	if ((!str_cmp (arg1, "armor") || !str_cmp (arg1, "weapon"))
		&& arg2[0] == '\0')
	{
		send_to_char ("Type 'Help Otype' for proper usage.\n\r", ch);
		return;
	}
	
	else
		if (!str_cmp (arg1, "armor"))
		{
			type = flag_value (type_flags, arg1);
			
			if ((type2 = flag_value (wear_flags, arg2)) == NO_FLAG)
			{
				send_to_char ("No such armor type.\n\r", ch);
				return;
			}
		}
		else
			if (!str_cmp (arg1, "weapon"))
			{
				type = flag_value (type_flags, arg1);
				
				if ((type2 = flag_value (weapon_class, arg2)) == NO_FLAG)
				{
					send_to_char ("No such weapon type.\n\r", ch);
					return;
				}
			}
			else
				
			{
				if ((type = flag_value (type_flags, arg1)) == NO_FLAG)
				{
					send_to_char ("Unknown Type.\n\r", ch);
					return;
				}
			}
			
			for (; vnum <= top_vnum_obj; vnum++)
			{
				if ((obj = get_obj_index (vnum)) != NULL)
				{
					if ((obj->item_type == type
						&& type2 == 0
						&& str_cmp (arg1, "weapon")
						&& str_cmp (arg1, "armor"))
						|| (obj->item_type == type
						&& obj->value[0] == type2
						&& str_cmp (arg1, "armor"))
						|| (obj->item_type == type
						&& IS_SET (obj->wear_flags, type2) && str_cmp (arg1, "weapon")))
						
					{
						sprintf (buf, "%35s - %5ld - Area [%ld]\n\r",
							obj->short_descr, vnum, obj->area->vnum);
						
						found = TRUE;
						strcat (buffer, buf);
					}
				}
			}
			
			if (!found)
				send_to_char ("No objects of that type exist\n\r", ch);
			else
				if (ch->lines)
					page_to_char (buffer, ch);
				else
					send_to_char (buffer, ch);
				
}

_DOFUN (do_repop)
{
	char buf[MSL];
	AREA_DATA *pArea;
	long vnum;
	bool found_area = FALSE;
	
	if (argument[0] == 0)
	{
		reset_area (ch->in_room->area);
		send_to_char ("Area repop!\n\r", ch);
		return;
	}
	
	if (is_number (argument))
	{
		vnum = atol (argument);
		for (pArea = area_first; pArea; pArea = pArea->next)
		{
			if (pArea->vnum == vnum)
			{
				sprintf (buf, "%s has been repoped!\n\r", &pArea->name[8]);
				send_to_char (buf, ch);
				found_area = TRUE;
				reset_area (pArea);
			}
		}
		if (!found_area)
			send_to_char ("No such area!\n\r", ch);
		return;
	}
	
	if (!strcmp (argument, "all"))
	{
		for (pArea = area_first; pArea; pArea = pArea->next)
		{
			reset_area (pArea);
		}
		send_to_char ("World repop!\n\r", ch);
	}
	return;
}

_DOFUN (do_todo)
{
	CHAR_DATA *rch;
	char arg[MIL], buf[MSL];
	int pos;
	bool found = FALSE;
	
	smash_tilde (argument);
	
	if (ch->desc == NULL)
		rch = ch;
	else
		rch = ch->desc->original ? ch->desc->original : ch;
	
	if (IS_NPC (rch))
		return;
	
	argument = one_argument (argument, arg);
	
	
	if (arg[0] == '\0' || (!str_cmp (arg, "list")))
	{
		if (rch->pcdata->todo[0] == NULL)
		{
			send_to_char ("There is nothing on your todo list.\n\r", ch);
			return;
		}
		
		send_to_char ("Your list of things to do:\n\r", ch);
		
		for (pos = 0; pos < MAX_TODO; pos++)
		{
			if (rch->pcdata->todo[pos] == NULL)
				break;
			chprintf (ch, "    %d:  %s\n\r", pos + 1, rch->pcdata->todo[pos]);
		}
		return;
	}
	
	if (!str_prefix (arg, "remove"))
	{
		if (argument[0] == '\0')
		{
			send_to_char ("Remove which number from todo list?\n\r", ch);
			return;
		}
		
		for (pos = 0; pos < MAX_TODO; pos++)
		{
			if (rch->pcdata->todo[pos] == NULL)
				break;
			
			if (found)
			{
				rch->pcdata->todo[pos - 1] = rch->pcdata->todo[pos];
				rch->pcdata->alias[pos] = NULL;
				continue;
			}
			
			if (is_number (argument))
			{
				int freq;
				freq = atoi (argument) - 1;
				
				if (rch->pcdata->todo[freq] != NULL)
				{
					send_to_char ("Reminder removed.\n\r", ch);
					free_string (rch->pcdata->todo[freq]);
					rch->pcdata->todo[freq] = NULL;
					found = TRUE;
				}	// checks if valid number-SB
			}		//checks if argument is number-SB
			else
			{
				send_to_char ("Syntax: todo remove <#>.\n\r", ch);
				return;
			}
		}				//end of for loop-SB
		if (!found)
			send_to_char ("No reminder of that number to remove.\n\r", ch);
		return;
	}				//end of remove part-SB
	
	if (!str_prefix (arg, "change"))
	{
		char arg1[MIL];
		argument = one_argument (argument, arg1);
		
		if (is_number (arg1))
		{
			int freq;
			freq = atoi (arg1) - 1;
			
			if (rch->pcdata->todo[freq] != NULL)
			{
				free_string (rch->pcdata->todo[freq]);
				rch->pcdata->todo[freq] = str_dup (argument);
				sprintf (buf, "Reminder #%d is now changed to '%s'.\n\r", freq + 1,
					argument);
				send_to_char (buf, ch);
				return;
			}
			else
			{
				send_to_char ("No reminder of that number to change.\n\r", ch);
				return;
			}
		}				//end of is number check-SB
		else
		{
			send_to_char ("Syntax: todo  change # <string>.\n\r", ch);
			return;
		}
	}				//end of change part-SB
	
	if (!str_prefix (arg, "add"))
	{
		for (pos = 0; pos < MAX_TODO; pos++)
		{
			if (rch->pcdata->todo[pos] == NULL)
				break;
		}
		if (pos >= MAX_TODO)
		{
			send_to_char ("Sorry, you have reached the ToDo list limit.\n\r", ch);
			return;
		}
		// make a new todo
		if (!IS_NULLSTR (rch->pcdata->todo[pos]))
			free_string (rch->pcdata->todo[pos]);

		rch->pcdata->todo[pos] = str_dup (argument);
		sprintf (buf, "Reminder #%d is now set to '%s'.\n\r", pos + 1, argument);
		send_to_char (buf, ch);
		return;
	}
}

_DOFUN (do_update)
{
	char arg1[MIL], arg2[MIL], arg3[MIL];
	char temp[MSL], temp2[MSL], temparg[MSL];
	bool found = FALSE;
	UPDATE_DATA *curr;
	
	argument = one_argument (argument, arg1);
	
	if (IS_NPC (ch))
	{
		chprintf (ch, "Nuh uh, Players only command.\n\r");
		return;
	}
	
	if (arg1[0] == '\0')
	{
		chprintf (ch, "Syntax:\n\r");
		chprintf (ch, "\tupdate motd <add/delete> <keyword> [text]\n\r");
		chprintf (ch, "\tupdate imotd <add/delete> <keyword> [text]\n\r");
		chprintf (ch, "\tupdate jobs <add/delete> <player> [job list]\n\r");
		chprintf (ch,
			"\tupdate wizlist add <level> <player> <job> <email>\n\r");
		chprintf (ch, "\tupdate wizlist delete <player>\n\r");
		chprintf (ch,
			"\tupdate wizlist modify <name> <field> <new value>\n\r");
		return;
	}
	
	if (!str_prefix (arg1, "wizlist"))
	{
		do_wizlist (ch, argument);
		return;
	}
	
	/*
	 * Welp, it's not a simple call to update anything that's already written.
	 * Now we need to further parse argument and start doing the more
	 * complicated code.
	 */
	
	argument = one_argument (argument, arg2);
	argument = One_argument (argument, arg3);
	
	if ((arg2[0] == '\0') || (arg3[0] == '\0'))
	{
		do_update (ch, "");		/* Print that syntax message */
		return;
	}
	
	if (!str_prefix (arg1, "motd") || !str_prefix (arg1, "imotd")
		|| !str_prefix (arg1, "jobs"))
	{
		if (!str_prefix (arg2, "add"))
		{
			if ((arg3[0] == '\0') || (argument[0] == '\0'))
			{
				do_update (ch, "");	/* Syntax again */
				return;
			}
			
			if (strlen_color (arg3) > 20)
			{
				chprintf (ch, "Keyword %s too long.\n\r", arg2);
				return;
			}
			
			if (strlen_color (argument) > 50)
			{
				int i, j = 0, LastSpace;
				temparg[0] = '\0';
				temp[0] = '\0';
				for (i = 0; argument[i] != '\0'; i++)
				{
					int l;
					
					if (argument[i] == ANSI_KEY )	/* Color codes, don't add to line wrap counter */
					{
						sprintf (temp2, "%s%c%c", temp, argument[i], argument[i + 1]);
						sprintf (temp, "%s", temp2);
						sprintf (temparg, "%s", temp);
						i++;
						continue;
					}
					
					if (j > 50)
					{
						temp[0] = '\0';
						l = 0;
						
						for (LastSpace = strlen (temparg) - 1; LastSpace > 0; LastSpace--)
						{
							l++;
							
							if ((l > 20) || (temparg[LastSpace] == ' '))
								break;
						}
						
						for (l = 0; temparg[l] != '\0'; l++)
						{
							if (argument[l] == '`')
							{
								sprintf (temp2, "%s%c%c", temp, temparg[l], temparg[l + 1]);
								sprintf (temp, "%s", temp2);
								l++;
								continue;
							}
							
							if (l == LastSpace)
							{
								sprintf (temp2, "%s\n\r                     `B|`w ", temp);
								sprintf (temp, "%s", temp2);
							}
							else
							{
								sprintf (temp2, "%s%c", temp, temparg[l]);
								sprintf (temp, "%s", temp2);
							}
						}
						j = 0;
					}
					
					sprintf (temp2, "%s%c", temp, argument[i]);
					sprintf (temp, "%s", temp2);
					sprintf (temparg, "%s", temp);
					j++;
				}
				sprintf (argument, "%s", temparg);
			}
			
			
			while (strlen_color (arg3) < 20)
			{
				sprintf (temp, "%s ", arg3);
				sprintf (arg3, "%s", temp);
			}
			
			curr = new_udata ();
			curr->keyword = str_dup (arg3);
			curr->information = str_dup (argument);
			if (!str_prefix (arg1, "motd"))
			{
				curr->next = motd_list;
				motd_list = curr;
			}
			if (!str_prefix (arg1, "imotd"))
			{
				curr->next = imotd_list;
				imotd_list = curr;
			}
			if (!str_prefix (arg1, "jobs"))
			{
				curr->next = jobs_list;
				jobs_list = curr;
			}
			
			save_updates ();
			return;
    }
	
    if (!str_prefix (arg2, "delete"))
    {
		UPDATE_DATA *prev;
		prev = NULL;
		
		if (!str_prefix (arg1, "motd"))
		{
			for (curr = motd_list; curr; prev = curr, curr = curr->next)
			{
				if (str_prefix (arg3, strip_codes (curr->keyword)))
					continue;
				
				found = TRUE;
				
				if (prev == NULL)
					motd_list = motd_list->next;
				else
					prev->next = curr->next;
				
				free_udata (curr);
				save_updates ();
			}
			if (found)
				chprintf (ch, "MOTD Keyword removed.\n\r");
			else
				chprintf (ch, "Keyword %s not found.\n\r", arg3);
			return;
		}
		
		if (!str_prefix (arg1, "imotd"))
		{
			for (curr = imotd_list; curr; prev = curr, curr = curr->next)
			{
				if (str_prefix (arg3, strip_codes (curr->keyword)))
					continue;
				
				found = TRUE;
				
				if (prev == NULL)
					imotd_list = imotd_list->next;
				else
					prev->next = curr->next;
				
				free_udata (curr);
				save_updates ();
			}
			if (found)
				chprintf (ch, "IMOTD Keyword removed.\n\r");
			else
				chprintf (ch, "Keyword %s not found.\n\r", arg3);
			return;
		}
		
		if (!str_prefix (arg1, "jobs"))
		{
			for (curr = jobs_list; curr; prev = curr, curr = curr->next)
			{
				if (str_prefix (arg3, strip_codes (curr->keyword)))
					continue;
				
				found = TRUE;
				
				if (prev == NULL)
					jobs_list = jobs_list->next;
				else
					prev->next = curr->next;
				
				free_udata (curr);
				save_updates ();
			}
			if (found)
				chprintf (ch, "Player removed from jobs list.\n\r");
			else
				chprintf (ch, "Player not found in jobs list.\n\r");
			return;
		}
    }
  }
  
  do_update (ch, "");		/* Be safe and show syntax if they didn't fit any of the if statements. */
}

void load_updates (void)
{
	FILE *fp;
	UPDATE_DATA *motd_last, *imotd_last, *jobs_last;
	
	if ((fp = fopen (UPDATE_FILE, "r")) == NULL)
		return;
	
	motd_last = NULL;
	imotd_last = NULL;
	jobs_last = NULL;
	for (;;)
	{
		UPDATE_DATA *curr;
		char *word;
		if (feof (fp))
		{
			fclose (fp);
			return;
		}
		
		curr = new_udata ();
		word = fread_word (fp);
		
		curr->keyword = fread_string (fp);
		curr->information = fread_string (fp);
		fread_to_eol (fp);
		
		if (!str_cmp (word, "motd"))
		{
			if (motd_list == NULL)
				motd_list = curr;
			else
				motd_last->next = curr;
			motd_last = curr;
		}
		if (!str_cmp (word, "imotd"))
		{
			if (imotd_list == NULL)
				imotd_list = curr;
			else
				imotd_last->next = curr;
			imotd_last = curr;
		}
		if (!str_cmp (word, "jobs"))
		{
			if (jobs_list == NULL)
				jobs_list = curr;
			else
				jobs_last->next = curr;
			jobs_last = curr;
		}
	}
}

void save_updates (void)
{
	UPDATE_DATA *curr;
	FILE *fp;
	
	if ((motd_list == NULL) && (imotd_list == NULL) && (jobs_list == NULL))
	{
		unlink (UPDATE_FILE);
		return;
	}
	
	
	if ((fp = fopen (UPDATE_FILE, "w")) == NULL)
		perror (UPDATE_FILE);
	
	if (motd_list)
	{
		for (curr = motd_list; curr; curr = curr->next)
			fprintf (fp, "motd %s~ %s~\n", curr->keyword, curr->information);
	}
	
	if (imotd_list)
	{
		for (curr = imotd_list; curr; curr = curr->next)
			fprintf (fp, "imotd %s~ %s~\n", curr->keyword, curr->information);
	}
	
	if (jobs_list)
	{
		for (curr = jobs_list; curr; curr = curr->next)
			fprintf (fp, "jobs %s~ %s~\n", curr->keyword, curr->information);
	}
	
	fclose (fp);
	
	return;
}
