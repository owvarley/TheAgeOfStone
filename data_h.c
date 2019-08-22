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
 * @@ Objective (data_h.c)
 *
 * This file contains the bulk of the /data/.dat file load/save routines.
 *
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "merc.h"

#if !defined(_WIN32)
#include <dlfcn.h>
#else
#include <windows.h>
#endif


#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( string, literal ) )	\
				{					\
				field  = value;			\
				fMatch = TRUE;			\
				break;				\
				}


char *errmsg (int err)
{
	static char buf[13];
	
	memset (buf, 0, 13);
	
	sprintf (buf, "%s",
#if !defined(_WIN32)
		err == ETXTBSY ? "ETXTBSY" :
	err == ELOOP ? "ELOOP" : 
#endif
	err == EEXIST ? "EEXIST" :
	err == EISDIR ? "EISDIR" :
	err == EFAULT ? "EFAULT" :
	err == EACCES ? "EACCES" :
	err == ENAMETOOLONG ? "ENAMETOOLONG" :
	err == ENOENT ? "ENOENT" :
	err == ENOTDIR ? "ENOTDIR" :
	err == EMFILE ? "EMFILE" :
	err == ENFILE ? "ENFILE" :
	err == ENOMEM ? "ENOMEM" :
	err == EROFS ? "EROFS" :
	err == ENOSPC ? "ENOSPC" : "(unknown)");
	return buf;
}


/** Function: append_tilde
  * Descr   : Used to append a tilde to the end of a string, for printing
  *         : padded strings to ascii files.
  * Returns : modified string
  * Syntax  : append_tilde (string to append)
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *append_tilde (char *str)
{
	char temp[MSL];
	
	memset (temp, 0, MSL);
	sprintf (temp, "%s~", str);
	
	return str_dup (temp);
}

char *fix_skill_name (char *name)
{
	static char buf[MIL];
	int i = 0;
	
	memset (buf, 0, MIL);
	memcpy (buf, name, strlen (name));
	
	for (i = 0; buf[i] != '\0'; i++)
		switch (buf[i])
		{
		  case ' ':
			  buf[i] = '_';
			  break;
		  default:
			  break;
		}
	
	return buf;
}


bool load_commands (void)
{
	int cmd = 0;
	char buf[MIL];
	FILE *fp;
	char *errStr;
	char *string;
	
#if defined(_WIN32)
	HMODULE handle;
	FARPROC fPtr;
#else
	void *handle;
	int (*fPtr) (int);
#endif
	
	
	//  void *ptr = NULL;
	
	sprintf (buf, "%s%s", DATA_DIR, COMMAND_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		logf ("[%s] Could not open file %s in order to read command data.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	maxCommands = -1;
	
	fscanf (fp, "#%d\n", &maxCommands);
	
	cmd_table = malloc (sizeof (struct cmd_type) * maxCommands + 1);
	memset (cmd_table, 0, sizeof (cmd_table[0]) * maxCommands + 1);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
			break;
		
		if (string[0] == '#')
		{
			fread_to_eol (fp);
			continue;
		}
		
		/* cheap hack I know... */
		if (!IS_NULLSTR (string) && string[0] == '+')
			cmd_table[cmd].name = str_dup ("'");
		else
			cmd_table[cmd].name = str_dup (string);
		
		cmd_table[cmd].position = fread_number (fp);
		cmd_table[cmd].level = fread_number (fp);
		cmd_table[cmd].log = fread_number (fp);
		cmd_table[cmd].flags = fread_flag (fp);
		cmd_table[cmd].lookup_name = fread_string (fp);
		cmd++;
	}
	
	fclose (fp);
	
#if !defined(_WIN32)
	/* load the symbol table from ./wot */
	handle = dlopen (NULL, RTLD_LAZY);
	
	if (!handle)
	{	
		perror (dlerror ());
		_quit (errno);
	}
	
	
	
#else
	handle = GetModuleHandle(APPNAME);
	
	if (!handle)
	{
		perror ("Unable to load symbol table!");
		_quit (errno);
	}
#endif
	
	
	/* attempt to find all command functions within the executable
	 * setting the do_fun pointer in the command table as we go.
	 */
	
	for (cmd = 0; cmd != maxCommands; cmd++)
	{
		memset (buf, 0, MIL);
		
		if (!IS_NULLSTR (cmd_table[cmd].name))
		{
			if (IS_NULLSTR (cmd_table[cmd].lookup_name))
				sprintf (buf, "do_%s", cmd_table[cmd].name);
			else
				sprintf (buf, "%s", fix_skill_name(cmd_table[cmd].lookup_name));
		}
		else
			break;
		
#if !defined(_WIN32)
		fPtr = (int (*)(int)) dlsym (handle, buf);
		errStr = dlerror ();
#else
		fPtr = GetProcAddress(handle, buf);
		errStr = LastErrorStr();
#endif
		
		if (!fPtr)
		{
			bugf ("[data_h.c::load_commands] Unable to locate command function for '%s'", buf);
			// _quit(1);
		}
		else
			cmd_table[cmd].do_fun = (DO_FUN *) fPtr;
	}
	
#if !defined(_WIN32)
	dlclose (handle);
	//	#else
	//	  FreeLibrary(handle);
#endif
	
	logf (NORMAL "Loaded %d commands." NO_COLOR, CYAN, cmd);
	
	/* Since we exit on all critical errors, return TRUE by default */
	return TRUE;
	
}

/** Function: save_commands
  * Descr   : Saves the command table to file for later reloading.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_commands (void)
{
	FILE *fp;
	char buf[MIL];
	int cmd;
	
	sprintf (buf, "%s%s", DATA_DIR, COMMAND_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf ("[data_h.c::save_commands] [%s] Could not open file %s in order to save command data.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	for (cmd = 1; cmd_table[cmd].name != NULL; ++cmd)
		;				/* simple count */
	
	fprintf (fp, "#%d\n"
		"## Command Table Data File\n"
		"## Written %s"
		"## Contains %d commands\n"
		"## File Format as follows.\n"
		"# Command name\n"
		"# Minimum Position required to execute\n"
		"# Minimum Level required to execute\n"
		"# When to log this command\n"
		"# Who to display this command to\n"
		"# The name of the actual _CMD() in the code. This can be used as an\n"
		"# alternate name. If the command 'do_'name'' is not found in the symbol\n"
		"# table, then this command_name is looked for instead. One of the two must\n"
		"# exist.\n#\n"
		"#\n"
		"# Position States are:\n"
		"# POS_DEAD      0\n"
		"# POS_MORTAL    1\n"
		"# POS_INCAP     2\n"
		"# POS_STUNNED   3\n"
		"# POS_SLEEPING  4\n"
		"# POS_RESTING   5\n"
		"# POS_SITTING   6\n"
		"# POS_FIGHTING  7\n"
		"# POS_STANDING  8\n"
		"#\n"
		"# Log Levels are:\n"
		"# LOG_NORMAL    0\n"
		"# LOG_ALWAYS    1\n"
		"# LOG_NEVER     2\n"
		"#\n"
		"#Command        Position        Level   Log     Flags   CMD\n"
		"#---------------------------------------------------------------------\n",
		cmd, ctime (&current_time), cmd);
	
	
	for (cmd = 0; cmd_table[cmd].name != NULL; cmd++)
	{
		
		if (IS_SET (cmd_table[cmd].flags, CMD_CHANGED))
			REMOVE_BIT (cmd_table[cmd].flags, CMD_CHANGED);
		
		if (!IS_NULLSTR(cmd_table[cmd].name) && cmd_table[cmd].name[0] == '\'')
		{
			free_string (cmd_table[cmd].name);
			cmd_table[cmd].name = str_dup ("+");
		}
		
		fprintf (fp, "%s%s\t%d\t\t%d\t%d\t%s\t%s~\n",
			cmd_table[cmd].name,
			strlen (cmd_table[cmd].name) > 7 ? "" : "\t",
			cmd_table[cmd].position,
			cmd_table[cmd].level,
			cmd_table[cmd].log,
			print_flags (cmd_table[cmd].flags),
			IS_NULLSTR (cmd_table[cmd].lookup_name) ? "" : cmd_table[cmd].
			lookup_name);
	}
	
	fprintf (fp, "\nEnd\n");
	fclose (fp);
	return TRUE;
}


/** Function: save_mud_data
  * Descr   : Saves the current state of the mud (wizlock,etc) as well as
  *         : some other useful(?) information.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_mud_data (void)
{
	FILE *fp;
	char buf[MIL];
	
	sprintf (buf, "%s%s", DATA_DIR, MUD_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf ("[data_h.c::save_mud_data] [%s] Could not open file %s in order to save mud data.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "#MUDDATA\n");
	fprintf (fp, "# This 'muddata' file is used to keep track of various\n"
		         "# 'states' and settings of the mud.  This is where the\n"
		         "# commands 'mudstate' & 'mudcfg' get their information.\n");

	fprintf (fp, "Version              %d\n",  pMud.version);
	fprintf (fp, "Logins               %ld\n", pMud.logins);
	fprintf (fp, "NewChars             %ld\n", pMud.newchars);
	fprintf (fp, "Newbies              %ld\n", pMud.newbies);
	fprintf (fp, "Deletions            %d\n",  pMud.deletions);
	fprintf (fp, "Unlinks              %d\n",  pMud.unlinks);
	fprintf (fp, "CopyoverLast         %ld\n", pMud.last_cover);
	fprintf (fp, "Flags                %s\n",  print_flags (pMud.flags));
	fprintf (fp, "#--Configuration Settings\n");
	fprintf (fp, "MudName              %s~\n", pMud.mud_name);
	fprintf (fp, "Exp_Mod              %f\n",  pMud.exp_mod);
	fprintf (fp, "Courageous           %d\n",  pMud.courageous);
	fprintf (fp, "LevelNoPeace         %d\n",  pMud.level_no_peace);
	fprintf (fp, "PkLimit              %d\n",  pMud.pk_limit);
	fprintf (fp, "BoxCost              %d\n",  pMud.box_cost);
	fprintf (fp, "ShareValue           %d\n",  pMud.share_value);
	fprintf (fp, "MaxPCTalents         %d\n",  pMud.MaxPCTalents);
	fprintf (fp, "MaxPCSpecialties     %d\n",  pMud.MaxPCSpecialties);
    fprintf (fp, "WaitTimer            %d\n",  pMud.WaitTimer);
	fprintf (fp, "ViolenceTimer        %d\n",  pMud.ViolenceTimer);
	fprintf (fp, "MaxCreationPoints    %d\n",  pMud.MaxCreationPoints);
	fprintf (fp, "BaseSoundURL         %s~\n", pMud.BaseSoundURL);
    fprintf (fp, "RedirectMSG          %s~\n", pMud.RedirectMSG);
	fprintf (fp, "RedirectURL          %s~\n", pMud.RedirectURL);
	fprintf (fp, "WelcomeMSG           %s~\n", pMud.welcome_message);
    fprintf (fp, "ConfigFlags          %s\n",  print_bits(pMud.config));

	if (pMud.sounds)
		save_sounds (fp, pMud.sounds, "Sound");
	
	fprintf (fp, "#0\n");
	fprintf (fp, "\nEnd\n");
	fclose (fp);
	
	logf ("Mud data file saved as: %s", buf);
	
	return TRUE;
}


/** Function: load_mud_data
  * Descr   : Loads last saved mud state (if any) from datafile.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_mud_data (void)
{
	FILE *fp;
	char buf[MIL];
	char *string;
	bool fMatch = FALSE;
	
	sprintf (buf, "%s%s", DATA_DIR, MUD_FILE);
	
	pMud.version = VERSION;
	pMud.logins = 0;
	pMud.newchars = 0;
	pMud.newbies = 0;
	pMud.deletions = 0;
	pMud.unlinks = 0;
	pMud.timer = -1;
	pMud.flags = 0;
	pMud.last_cover = 0;
	pMud.exp_mod = 1.0f;
	pMud.sounds = NULL;
	pMud.mud_name = &str_empty[0];
	pMud.welcome_message = &str_empty[0];
	pMud.courageous = 10;
	pMud.level_no_peace = 15;
	pMud.pk_limit = 5;
	pMud.box_cost = 3000;
	pMud.share_value = 100;
	pMud.MaxPCTalents = 1;
	pMud.MaxPCSpecialties = 1;
	pMud.WaitTimer = 2;
	pMud.ViolenceTimer = 3;
	pMud.BaseSoundURL = &str_empty[0];
	pMud.MaxCreationPoints = 200;
	pMud.RedirectMSG = &str_empty[0];
	pMud.RedirectURL = &str_empty[0];
	set_init(pMud.config);

	SET_FLAG(pMud.config, _ENFORCE_EMBRACE);
	SET_FLAG(pMud.config, _CHECK_BURN);
	SET_FLAG(pMud.config, _ALLOW_KILL_STEALING);
	SET_FLAG(pMud.config, _ALLOW_WHO_CLASS);
	SET_FLAG(pMud.config, _ALLOW_WHO_GUILD);
	SET_FLAG(pMud.config, _ALLOW_WHO_RACE);
	SET_FLAG(pMud.config, _ALLOW_WHO_NATION);
	SET_FLAG(pMud.config, _ALLOW_WHO_LEVEL);
	SET_FLAG(pMud.config, _GUILD_ONLY_IN_WHO);
	SET_FLAG(pMud.config, _USE_DRUNK);
	SET_FLAG(pMud.config, _USE_IGNORE);
	SET_FLAG(pMud.config, _ALLOW_BANK_TRANSFERS);
	SET_FLAG(pMud.config, _COUNT_LOOT);
	SET_FLAG(pMud.config, _DESTROY_LOSER);
	SET_FLAG(pMud.config, _BAN_LOSER_NAME);
	SET_FLAG(pMud.config, _ALLOW_RDESC);
	SET_FLAG(pMud.config, _FORCE_CUSTOMIZE);
	SET_FLAG(pMud.config, _DETERMINE_OPR_BY_SEX);

	if (!(fp = fopen (buf, "r")))
	{
		bugf ("[data_h.c::load_mud_data] [%s] Could not open file %s in order to read mud data. Creating.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	if (str_cmp (fread_word (fp), "#MUDDATA"))
	{
		bugf("[data_h.c::load_mud_data] Invalid mud data file (%s).\n\r", buf);
		return FALSE;
	}
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
		{
			fMatch = TRUE;
			break;
		}
		
		fMatch = FALSE;
		
		switch (UPPER (string[0]))
		{
			case '#':	// comment identifier 
				fMatch = TRUE;
				fread_to_eol (fp);
				break;			
			case 'B':
				KEY("BoxCost", pMud.box_cost, fread_number(fp));
				KEY("BaseSoundURL", pMud.BaseSoundURL, fread_string(fp));
				break;
			case 'C':
				KEY("CopyoverLast", pMud.last_cover, fread_long(fp));
				KEY("Courageous", pMud.courageous, fread_number(fp));
				if ( !str_cmp("ConfigFlags", string) )
				{
					fMatch = TRUE;
					fread_bits( fp, pMud.config );
				}
				break;
			case 'D':
				KEY("Deletions", pMud.deletions, fread_long(fp));
				break;
			case 'E':
				KEY("Exp_Mod", pMud.exp_mod, fread_float(fp));
				break;
			case 'F':
				KEY("Flags", pMud.flags, fread_flag(fp));
				break;
			case 'L':
				KEY("Logins", pMud.logins, fread_long(fp));
				KEY("LevelNoPeace", pMud.level_no_peace, fread_number(fp));
				break;
			case 'M':
				KEY("MudName", pMud.mud_name, fread_string(fp));
				KEY("MaxCreationPoints", pMud.MaxCreationPoints, fread_number(fp));
				KEY("MaxPCTalents", pMud.MaxPCTalents, fread_number(fp));
				KEY("MaxPCSpecialties", pMud.MaxPCSpecialties, fread_number(fp));
				break;
			case 'N':
				KEY("Newbies", pMud.newbies, fread_long(fp));
				KEY("NewChars", pMud.newchars, fread_long(fp));
				break;
			case 'P':
				KEY("PkLimit", pMud.pk_limit, fread_number(fp));
				break;
			case 'R':
				KEY("RedirectMSG", pMud.RedirectMSG, fread_string(fp));
				KEY("RedirectURL", pMud.RedirectURL, fread_string(fp));
				break;
			case 'S':
				KEY("ShareValue", pMud.share_value, fread_number(fp));
				if ( !str_cmp("Sound", string) )
				{
					fMatch = TRUE;
					pMud.sounds = load_sound (fp, pMud.sounds);
				}
				break;
			case 'U':
				KEY("Unlinks", pMud.unlinks, fread_number(fp));
				break;
			case 'V':
				KEY("Version", pMud.version, fread_number(fp));
				KEY("ViolenceTimer", pMud.ViolenceTimer, fread_number(fp));
				break;
			case 'W':
				KEY("WaitTimer", pMud.WaitTimer, fread_number(fp));
				KEY("WelcomeMSG", pMud.welcome_message, fread_string(fp));
				break;
		}
		
		if (!fMatch)
		{
			bugf ("[data_h.c::load_mud_data] Invalid Key: %s", string);
			fread_to_eol(fp);
		}
	}
	
	fclose (fp);
	logf (NORMAL "Loaded Mud Data File." NO_COLOR, CYAN);
	
	if (IS_SET (pMud.flags, MUD_COPYOVER))
	{
		REMOVE_BIT (pMud.flags, MUD_COPYOVER);
	}
	
	if ( pMud.version == 0 )
		pMud.version = VERSION;

	return TRUE;
}


/** Function: save_names
  * Descr   : Saves the banned names linked list to file for later reloading.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_names (void)
{
	FILE *fp;
	char buf[MIL];
	NAME_DATA *Name;
	
	sprintf (buf, "%s%s", DATA_DIR, NAMES_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_names] [%s] Could not open file %s in order to save names data.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "#NAMES\n\n");
	fprintf (fp, "Name         By           Lvl Date      Reason\n"
		"-------------------------------------------------------------------\n");
	
	for (Name = name_first; Name; Name = Name->next)
	{
		fprintf (fp, "%-12s %-12s %3d %ld %s~\n",
			append_tilde (Name->name),
			append_tilde (Name->by),
			Name->by_lvl, Name->date_stamp, Name->reason);
	}
	
	fprintf (fp, "\nEnd~\n");
	fclose (fp);
	return TRUE;
}


/** Function: load_names
  * Descr   : Loads the banned names list into a linked list.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_names (void)
{
	FILE *fp;
	NAME_DATA *Name;
	char buf[MSL];
	char *string = NULL;
	
	sprintf (buf, "%s%s", DATA_DIR, NAMES_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_names] [%s] Could not open file %s in order to load data.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	if (str_cmp (fread_word (fp), "#NAMES"))
	{
		bugf("[data_h.c::load_names] Invalid data file (%s).\n\r", buf);
		return FALSE;
	}
	
	while (str_prefix ("---", fread_word (fp)))
		;
	
	for (;;)
	{
		
		string = fread_string (fp);
		
		if (feof (fp) || !str_cmp (string, "End"))
			break;
		
		if (*string != '\0')
		{
			Name = new_name ();
			
			if (!name_first)
				name_first = Name;
			if (name_last)
				name_last->next = Name;
			
			name_last = Name;
			
			Name->name = str_dup (string);
			Name->by = fread_string (fp);
			Name->by_lvl = fread_number (fp);
			Name->date_stamp = atoi (fread_word (fp));
			Name->reason = fread_string (fp);
		}
		
	}
	
	logf (NORMAL "Loaded %d Names." NO_COLOR, CYAN, top_name);
	fclose (fp);
	return TRUE;
}



/** Function: load_liquids
  * Descr   : Loads the liquid database into a structure array
  *	    : Also clears (nulls) the structure beforehand.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_liquids (void)
{
	FILE *fp;
	char buf[MIL];
	
	int i = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, LIQUID_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_liquids] [%s] Could not open data file %s.", errmsg (errno), buf);
		return FALSE;
	}
	
	if (str_cmp (fread_word (fp), "Liquid"))
	{
		bugf("[data_h.c::load_liquids] Invalid data file.");
		_quit (1);
	}
	
	/* read past liquid header and seperator line */
	fread_to_eol (fp);
	fread_to_eol (fp);
	
	
	memset (liq_table, 0, sizeof (liq_table));
	i = 0;
	
	for (;;)
	{
		
		if (i >= MAX_LIQUIDS)
		{
			bugf("[data_h.c::load_liquids] Max Liquid count reached. No more entries will be read.");
			break;
		}
		
		liq_table[i].liq_name = fread_string (fp);
		
		if (!str_cmp (liq_table[i].liq_name, "End"))
		{
			free_string (liq_table[i].liq_name);
			liq_table[i].liq_name = str_dup ("");
			i--;
			break;
		}
		
		liq_table[i].liq_color = fread_string (fp);
		liq_table[i].liq_affect[0] = fread_number (fp);
		liq_table[i].liq_affect[1] = fread_number (fp);
		liq_table[i].liq_affect[2] = fread_number (fp);
		liq_table[i].liq_affect[3] = fread_number (fp);
		liq_table[i].liq_affect[4] = fread_number (fp);
		
		i++;
	}				/* end: for() */
	
	liq_table[MAX_LIQUIDS].liq_name = str_dup ("");
	logf (NORMAL "Loaded %d Liquids." NO_COLOR, CYAN, i);
	
	fclose (fp);
	return TRUE;
}



/** Function: save_liquids
  * Descr   : Saves the liquid database to file for later reloading.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_liquids (void)
{
	FILE *fp;
	char buf[MIL];
	int i = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, LIQUID_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_liquids] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "Liquid		     Color	    proof      full"
		"   thirst   food     size\n"
		"---------------------------------------------------"
		"-------------------------\n");
	
	for (;;)
	{
		if (!liq_table[i].liq_name)
		{
			logf ("Saved %d liquids.", i);
			break;
		}
		
		fprintf (fp, "%-18s   %-15s %03d\t%03d\t%03d\t%03d\t%03d\n",
			append_tilde (liq_table[i].liq_name),
			append_tilde (liq_table[i].liq_color),
			liq_table[i].liq_affect[0],
			liq_table[i].liq_affect[1],
			liq_table[i].liq_affect[2],
			liq_table[i].liq_affect[3], liq_table[i].liq_affect[4]);
		
		i++;
	}
	
	fprintf (fp, "End~\n");
	fclose (fp);
	return TRUE;
}


/** Function: load_pc_races
  * Descr   : Loads the PC section of the race database into memory.
  * Returns : TRUE on success
  * Syntax  : (internal, is passed file pointer by load_races)
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_pc_races (FILE * fp)
{
	char *string;
	int i = 0;
	int skill = 0;
	bool fMatch;
	
	fread_to_eol(fp);
	fscanf (fp, "#%d", &maxPCRace);
	pc_race_table = malloc (sizeof (struct pc_race_type) * maxPCRace);
	memset (pc_race_table, 0, sizeof (struct pc_race_type) * maxPCRace);
	
	for (;;)
	{
		
		string = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;
		
		if (!str_cmp (string, "#0") || !str_cmp (string, "End"))
		{
			logf (NORMAL "Loaded %d PC Races." NO_COLOR, CYAN, i);
			return FALSE;
		}
		
		switch (UPPER (string[0]))
		{
				
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;
			case 'A':
				KEY ("Align", pc_race_table[i].defAlign, fread_number (fp));
				break;
			case 'C':
				if (!str_cmp (string, "ClassX"))
				{
					int x = 0;
					
					while (x < MAX_CLASS)
					{
						pc_race_table[i].class_mult[x] = fread_number (fp);
						x++;
					}
					
					fMatch = TRUE;
				}
				break;
				
			case 'M':
				if (!str_cmp (string, "MStat"))
				{
					int x = 0;
					
					while (x < MAX_STATS)
					{
						pc_race_table[i].max_stats[x] = fread_number (fp);
						x++;
					}
					
					fMatch = TRUE;
				}
				break;
				
			case 'P':
				KEY ("Points", pc_race_table[i].points, fread_number (fp));
				break;
				
			case 'R':
				KEY ("Race", pc_race_table[i].name, fread_string (fp));
				break;
				
			case 'S':
				if (!str_cmp (string, "Skill"))
				{
					pc_race_table[i].skills[skill] = fread_string (fp);
					skill++;
					fMatch = TRUE;
				}
				else
					if (!str_cmp (string, "Stats"))
					{
						int x = 0;
						
						while (x < MAX_STATS)
						{
							pc_race_table[i].stats[x] = fread_number (fp);
							x++;
						}
						fMatch = TRUE;
					}
					else
						if (!str_cmp (string, "Size"))
						{
							pc_race_table[i].size = fread_number (fp);
							fMatch = TRUE;
							i++;
							skill = 0;
						}
						break;
						
			case 'W':
				if (!str_cmp (string, "Who"))
				{
					char *temp;
					
					temp = fread_string (fp);
					sprintf (pc_race_table[i].who_name, "%s", temp);
					fMatch = TRUE;
				}
				break;
		}
	}
	return TRUE;
}



/** Function: load_races
  * Descr   : Loads the race database from file. Also calls load_pc_races
  *         : for the PC specific portion.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_races (void)
{
	FILE *fp;
	char buf[MIL];
	char *string;
	bool fMatch = FALSE;
	
	int i = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, RACE_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_races] [%s] Could not open data file %s.", errmsg (errno), buf);
		return FALSE;
	}
	
	i = 0;
	fscanf (fp, "#%d\n", &maxRace);
	race_table = malloc (sizeof (struct race_type) * (maxRace));
	memset (race_table, 0, sizeof (race_table[0]) * maxRace);
	
	for (;;)
	{
		
		string = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;
		
		if (!str_cmp (string, "End"))
			break;
		
		switch (UPPER (string[0]))
		{
			case '#':
				if (!str_cmp (string, "#PC"))
				{
					logf (NORMAL "Loaded %d Races." NO_COLOR, CYAN, i);
					if (!load_pc_races (fp))
					{
						fclose(fp);
						return FALSE;
					}
					fclose (fp);
					return TRUE;
				}
				else
					
				{
					fMatch = TRUE;
					fread_to_eol (fp);
				}
				break;
				
			case 'A':
				if (!str_cmp (string, "Act"))
				{
					fread_bits(fp, race_table[i].act);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (string, "Aff"))
				{
					fread_bits(fp, race_table[i].aff);
					fMatch = TRUE;
					break;
				}
				break;
			case 'O':
				if (!str_cmp (string, "Off"))
				{
					fread_bits(fp, race_table[i].off);
					fMatch = TRUE;
					break;
				}
				break;
				
			case 'F':
				if (!str_cmp (string, "Form"))
				{
					fread_bits(fp, race_table[i].form);
					fMatch = TRUE;
					break;
				}
				break;
				
			case 'I':
				if (!str_cmp (string, "Imm"))
				{
					fread_bits(fp, race_table[i].imm);
					fMatch = TRUE;
					break;
				}
				break;
				
			case 'P':
				if (!str_cmp (string, "Parts"))
				{
					fread_bits(fp, race_table[i++].parts);
					fMatch = TRUE;
					break;
				}
				else
					if (!str_cmp (string, "PC"))
					{
						char *word;
						
						word = fread_word (fp);
						race_table[i].pc_race = !str_cmp (word, "TRUE");
						fMatch = TRUE;
						break;
					}
					break;
					
			case 'R':
				KEY ("Race", race_table[i].name, fread_string (fp));
				if (!str_cmp (string, "Res"))
				{
					fread_bits(fp, race_table[i].res);
					fMatch = TRUE;
					break;
				}
				break;
				
			case 'V':
				if (!str_cmp (string, "Vuln"))
				{
					fread_bits(fp, race_table[i].vuln);
					fMatch = TRUE;
					break;
				}
				break;
		}
		
		if (!fMatch)
		{
			bugf("[data_h.c::load_races] Invalid Key: %s", string);
			fread_to_eol (fp);
		}
  }
  
  return TRUE;
}


/** Function: save_races
  * Descr   : Saves the race database to a text file
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_races (void)
{
	FILE *fp;
	char buf[MIL];
	int i = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, RACE_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_races] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "#%d\n", maxRace);
	fprintf (fp, "## Race Table Data file\n## Written %s",
		ctime (&current_time));
	fprintf (fp, "## Contains %d races, Including %d PC races\n",
		maxRace, 
		maxPCRace);
	fprintf (fp, "## File Format (Data specific to all races)\n");
	fprintf (fp, "## See the PC specific section for more details.\n#\n");
	fprintf (fp, "#\n");
	fprintf (fp, "# Race	(string)        // The name of the race\n");
	fprintf (fp, "# PC    (word)          // TRUE or FALSE if it's a PC race\n");
	fprintf (fp, "# Act   (bitfields)     // Action flags\n");
	fprintf (fp, "# Aff   (bitfields)     // Affected by flags\n");
	fprintf (fp, "# Off   (bitfields)     // Offensive flags\n");
	fprintf (fp, "# Imm   (bitfields)     // Immunity flags\n");
	fprintf (fp, "# Res   (bitfields)     // Resistance flags\n");
	fprintf (fp, "# Vuln  (bitfields)     // Vulnerability flags\n");
	fprintf (fp, "# Form  (bitfields)     // Form flags\n");
	fprintf (fp, "# Parts (bitfields)     // Body part flags\n");
	fprintf (fp, "#\n");
	
	
	/* save NPC races first. */
	for (;;)
	{
		if (!race_table[i].name || i >= maxRace)
		{
			logf ("Saved %d NPC races.", i);
			break;
		}
		
		fprintf (fp, "Race\t %s~\n", race_table[i].name);
		fprintf (fp, "PC %s\n", race_table[i].pc_race ? "TRUE" : "FALSE");
		fprintf (fp, "Act %s\n", print_bits (race_table[i].act));
		fprintf (fp, "Aff %s\n", print_bits (race_table[i].aff));
		fprintf (fp, "Off %s\n", print_bits (race_table[i].off));
		fprintf (fp, "Imm %s\n", print_bits (race_table[i].imm));
		fprintf (fp, "Res %s\n", print_bits (race_table[i].res));
		fprintf (fp, "Vuln %s\n", print_bits (race_table[i].vuln));
		fprintf (fp, "Form %s\n", print_bits (race_table[i].form));
		fprintf (fp, "Parts %s\n\n", print_bits (race_table[i].parts));
		i++;
	}
	
	fprintf (fp, "#PC\n#%d\n", maxPCRace);
	fprintf (fp, "## PC Race Table Data \n");
	fprintf (fp, "## Contains %d PC races.\n", maxPCRace);
	fprintf (fp, "## Data Format\n");
	fprintf (fp, "#\n");
	fprintf (fp, "# Race    (string)      // The name of the race\n");
	fprintf (fp, "# Who     (string)      // The name that shows in 'who'\n");
	fprintf (fp, "# Points  (short int)   // Creation point cost\n");
	fprintf (fp, "# ClassX  (int array)   // Experience Point multiplier for each class.\n");
	fprintf (fp, "# Skill   (string)      // A racial skill\n");
	fprintf (fp, "# Stats   (int array)   // Starting stats (minimums)\n");
	fprintf (fp, "# MStat   (int array)   // Maximum racial (unmodified) stats.\n");
	fprintf (fp, "# Align   (integer)     // Starting alignment\n");
	fprintf (fp, "# Size    (enum)        // TINY/SMALL/MEDIUM/LARGE/HUGE/GIANT\n");
	fprintf (fp, "#\n");
	
	i = 0;
	
	for (;;)
	{
		int x = 0;
		
		if (i >= maxPCRace
			|| pc_race_table[i].name == NULL || pc_race_table[i].name[0] == '\0')
		{
			logf ("Saved %d PC Races.", i);
			break;
		}
		
		fprintf (fp, "Race\t %s~\n", pc_race_table[i].name);
		fprintf (fp, "Who \t %s~\n", pc_race_table[i].who_name);
		fprintf (fp, "Points %d\n", pc_race_table[i].points);
		fprintf (fp, "ClassX");
		for (x = 0; x < MAX_CLASS; x++)
			fprintf (fp, " %d", pc_race_table[i].class_mult[x]);
		fprintf (fp, "\n");
		
		for (x = 0; x < 5; x++)
			if (pc_race_table[i].skills[x] &&
				skill_lookup (pc_race_table[i].skills[x]) >= 0)
				fprintf (fp, "Skill %s~\n", pc_race_table[i].skills[x]);
			
			fprintf (fp, "Stats");
			
			for (x = 0; x < MAX_STATS; x++)
				fprintf (fp, " %d", pc_race_table[i].stats[x]);
			
			fprintf (fp, "\nMStat");
			
			for (x = 0; x < MAX_STATS; x++)
				fprintf (fp, " %d", pc_race_table[i].max_stats[x]);
			
			fprintf (fp, "\nAlign %d\n", pc_race_table[i].defAlign);
			fprintf (fp, "Size  %d\n\n", pc_race_table[i].size);
			
			i++;
	}
	
	fprintf (fp, "#0\n\nEnd\n");
	fclose (fp);
	return TRUE;
}

/** Function: load_groups
  * Descr   : Reads in data/GROUP_FILE and sets the group_table accordingly
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_groups (void)
{
	FILE *fp;
	char buf[MSL];
	char *string;
	int group = -1;
	int skill = 0;
	int i = 0;
	bool fMatch = FALSE;
	
	sprintf (buf, "%s%s", DATA_DIR, GROUP_FILE);
	
	if ((fp = fopen (buf, "r")) == NULL)
	{
		bugf("[data_h.c::load_groups] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return save_groups ();
	}
	
	maxGroup = -1;
	fscanf (fp, "#%d\n", &maxGroup);
	group_table = malloc (sizeof (struct group_type) * (maxGroup));
	memset (group_table, 0, sizeof (group_table[0]) * maxGroup);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
			break;
		
		switch (UPPER (string[0]))
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;
				
			case 'G':
				group++;
				skill = 0;
				group_table[group].name = fread_string (fp);
				fMatch = TRUE;
				break;
				
			case 'R':
				for (i = 0; i < MAX_CLASS; i++)
					group_table[group].rating[i] = fread_number (fp);
				fMatch = TRUE;
				break;
				
			case 'S':
				group_table[group].spells[skill] = fread_string (fp);
				skill++;
				fMatch = TRUE;
				break;
				
			case 'F':
				if (!str_cmp (string, "FLAGS"))
				{
					fread_bits (fp, group_table[group].flags);
					fMatch = TRUE;
				}
				break;
		}
		
		if (!fMatch)
		{
			bugf("[data_h.c::load_groups] Invalid Key %s.", string);
			fread_to_eol(fp);
		}

	} /* end: for(;;) */
	
	fclose (fp);
	logf (NORMAL "Loaded %d Groups." NO_COLOR, CYAN, group);
	return TRUE;
	
} /* end: load_groups */



/** Function: save_groups
  * Descr   : Saves the group data to a text file in ../data dir
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */  
bool save_groups (void)
{
	FILE *fp;
	char buf[MIL];
	int i = 0;
	int count = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, GROUP_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_groups] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	/* Count valid groups */
	for (i = 0; i < maxGroup; i++)
		if (group_table[i].name
			&& group_table[i].name[0] != '\0'
			&& !IS_FLAG (group_table[i].flags, _DELETED))
			count++;
		
		fprintf (fp, "#%d\n", count);
		fprintf (fp, "## Group Table Data file\n## Written %s",
			ctime (&current_time));
		fprintf (fp, "## Contains %d groups.\n", count);
		fprintf (fp, "## File Format\n#\n");
		fprintf (fp, "# GROUP          // The Name of this group.\n");
		fprintf (fp, "# RATINGS        // The class ratings for this group. \n");
		fprintf (fp, "# SKILL          // One of the skills contained within the group\n");
		fprintf (fp, "# FLAGS          // The flags for this group.\n");
		fprintf (fp, "#\n");
		
		for (i = 0; i < maxGroup; i++)
		{
			int x = 0;
			
			if (group_table[i].name == NULL)
				break;
			
			if (IS_FLAG (group_table[i].flags, _DELETED))
				continue;
			
			if (IS_FLAG (group_table[i].flags, _CHANGED))
				REMOVE_FLAG (group_table[i].flags, _CHANGED);
			
			fprintf (fp, "GROUP\t%s~\n", group_table[i].name);
			
			fprintf (fp, "RATINGS\t");
			
			for (x = 0; x < MAX_CLASS; x++)
				fprintf (fp, "%d ", group_table[i].rating[x]);
			
			fprintf (fp, "\n");
			
			x = 0;
			while (group_table[i].spells[x] != NULL && x < MAX_IN_GROUP)
			{
				fprintf (fp, "SKILL\t%s~\n", group_table[i].spells[x]);
				x++;
			}
			
			fprintf (fp, "FLAGS\t%s\n", print_bits (group_table[i].flags));
			
			/* indicate end of group, next should follow another group */
			fprintf (fp, "#0\n\n");
		}
		
		fprintf (fp, "\nEnd\n");
		fclose (fp);
		return TRUE;
		
}

/** Function: load_quotes
  * Descr   : Determines the number, and existance of quote files in QUOTE_DIR
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_quotes (void)
{
	int n = 0;
	FILE *fp;
	char buf[100];
	
	sprintf (buf, "%squote.1", QUOTE_DIR);
	
	while ((fp = fopen (buf, "r")) != NULL)
	{
		fclose (fp);
		n++;
		sprintf (buf, "%squote.%d", QUOTE_DIR, n);
	}
	n--;
	
	num_quotes = n;
	logf (NORMAL "Loaded %d Quote files." NO_COLOR, CYAN, num_quotes);
	return TRUE;
}



/** Function: save_skill_data
  * Descr   : Saves the skill_table to /data/skills.dat for later
  *         : editing of appropriate fields (ie: not weave_fun or gsn)
  * Returns : TRUE/FALSE on success
  * Syntax  : void
  * Written : v1.0 01/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_skill_data (void)
{
	FILE *fp;
	char buf[MIL];
	sh_int count = 0;
	sh_int i;
	

	for ( i = 0; i < maxSkill; i++ )
		if ( !IS_NULLSTR(skill_table[i].name) )
			count++;

	sprintf (buf, "%s%s", DATA_DIR, SKILL_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_skill_data] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "#%d\n", count);
	fprintf (fp, "## Skill Table Data file\n## Written %s", ctime (&current_time));
	fprintf (fp, "## Contains %d skills.\n", count);
	fprintf (fp, "## File Format\n#\n");
	fprintf (fp, "# Skill          // The Name of this skill.\n");
	fprintf (fp, "# sLevels        // The level each class may use this skill at.\n");
	fprintf (fp, "# sRates         // The rate at wich a class improves in this skill\n");
	fprintf (fp, "#                // and also the creation cost per class.\n");
	fprintf (fp, "# Targets        // Determines what/who this skill may be used on\n");
	fprintf (fp, "# MinPos         // The minimum position the user must be in to use this skill.\n");
	fprintf (fp, "# gFlags         // Which Guilds (if limited by guild) may use this skill.\n");
	fprintf (fp, "# FlowStr        // The recommended strenth, per flow, to channel this weave.\n");
	fprintf (fp, "#                // The order is: Air/Earth/Water/Fire/Spirit\n");
	fprintf (fp, "# Beats          // The # of pulses to pause the user after using this skill.\n");
	fprintf (fp, "# DamMsg         // The damage message used when successful.\n");
	fprintf (fp, "# MsgOff         // The message sent to the user when the skill wears off.\n");
	fprintf (fp, "# MsgObj         // The message seen when the skill wears off of an object.\n");
	fprintf (fp, "# cPoints		   // The class points for the skill.\n");
	fprintf (fp, "# sClass		   // The class of the skill class being: COMBAT, CRAFT, MUSIC, KNOWLEDGE,\n");
	fprintf (fp, "#				   // SURVIVAL, STEALTH.\n");
	fprintf (fp, "# Prereq		   // The sn of any prerequired skills.\n");
	fprintf (fp, "#\n");
	
	for (count = 0; count < maxSkill; count++)
	{
		if (IS_FLAG (skill_table[count].flags, _CHANGED))
			REMOVE_FLAG (skill_table[count].flags, _CHANGED);

		if ( IS_NULLSTR(skill_table[count].name) )
			continue; // skip this empty skill
		
		fprintf (fp, "Skill   %s~\n", skill_table[count].name);
		fprintf (fp, "sLevels ");
		for (i = 0; i < MAX_CLASS; i++)
			fprintf (fp, "%d ", skill_table[count].skill_level[i]);
		fprintf (fp, "\nsRates  ");
		for (i = 0; i < MAX_CLASS; i++)
			fprintf (fp, "%d ", skill_table[count].rating);
		fprintf (fp, "\nTargets %d\n", skill_table[count].target);
		fprintf (fp, "MinPos  %d\n", skill_table[count].minimum_position);
		fprintf (fp, "\nBeats   %d\n", skill_table[count].beats);
		fprintf (fp, "DamMsg  %s~\n", skill_table[count].noun_damage);
		fprintf (fp, "MsgOff  %s~\n", skill_table[count].msg_off);
		fprintf (fp, "MsgObj  %s~\n", skill_table[count].msg_obj);
		fprintf (fp, "sFlags  %s\n\n", print_bits (skill_table[count].flags));
	}
	fprintf (fp, "\nEnd\n");
	fclose (fp);
	return TRUE;
}

bool load_skill_data (void)
{
	FILE *fp;
	char buf[MIL];
	char *string;
	char *errStr;
	bool fMatch = FALSE;
	bool fFound = TRUE;
	int count = -1;
	long lSize = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, "skills.dat");
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_skill_data] [%s] Could not open data file %s, creating.",
			errmsg (errno), buf);
		return save_skill_data ();
	}
	

	maxSkill = -1;
	
	fscanf (fp, "#%d\n", &maxSkill);
	lSize = (sizeof( skill_table[0] )*maxSkill) + 1;

	skill_table = malloc (lSize);
	memset (skill_table, 0, lSize);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
		{
			fMatch = TRUE;
			break;
		}
		
		fMatch = FALSE;
		
		if (!fFound && str_cmp ("Skill", string))
			continue;
		
		switch (string[0])
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;
				
			case 'B':
				KEY ("Beats", skill_table[count].beats, fread_number (fp));
				break;

			case 'c':
				if (!str_cmp (string, "cPoints"))
				{
					sh_int i;

					for (i = 0; i < MAX_CLASS; i++)
					skill_table[count].class_points[i] = fread_number (fp);


					fMatch = TRUE;
					break;
				}
				break;
				
			case 'D':
				KEY ("DamMsg", skill_table[count].noun_damage, fread_string (fp));
				break;
				
			case 'M':
				KEY ("MinPos", skill_table[count].minimum_position, fread_number (fp));
				KEY ("MsgObj", skill_table[count].msg_obj, fread_string (fp));
				KEY ("MsgOff", skill_table[count].msg_off, fread_string (fp));
				break;

			case 'P':
				if (!str_cmp (string, "Prereq"))
				{
					sh_int i;
					for (i = 0; i <= 10; i++)
						skill_table[count].prereq[i] = fread_number (fp);

					fMatch = TRUE;
					break;
				}
				break;

			case 's':
				if (!str_cmp (string, "sFlags"))
				{
					fread_bits (fp, skill_table[count].flags);
					fMatch = TRUE;
					if (IS_FLAG (skill_table[count].flags, _DELETED))
						REMOVE_FLAG (skill_table[count].flags, _DELETED);
					break;
				}
				else
				if (!str_cmp (string, "sLevels"))
				{
					sh_int i;
					
					for (i = 0; i < MAX_CLASS; i++)
						skill_table[count].skill_level[i] = fread_number (fp);
					
					fMatch = TRUE;
					break;
				}
				else
				if (!str_cmp (string, "sRates"))
				{
			
					skill_table[count].rating = fread_number (fp);
					fMatch = TRUE;
					break;
				}
				else
				if (!str_cmp (string, "sClass"))
				{
					sh_int i;
					for (i = 0; i < MAX_CLASS; i++)
					skill_table[count].skill_class[i] = fread_number (fp);

				    fMatch = TRUE;
				break;
				}
				break;
						
			case 'S':
				if (!str_cmp (string, "Skill"))
				{
					skill_table[++count].name = fread_string (fp);
					
					if (count < 0)
					{
						fFound = FALSE;
						continue;
					}
					else
						fFound = TRUE;
					
					fMatch = TRUE;
					break;
				}
				break;
				
			case 'T':
				KEY ("Targets", skill_table[count].target, fread_number (fp));
				break;
				
		}				/* end switch */
		
		if (!fMatch)
		{
			bugf ("[data_h.c::load_skills] Invalid Key: %s", string);
			fread_to_eol(fp);
		}
  }
  
  fclose (fp);
  logf (NORMAL "Loaded %d Skills." NO_COLOR, CYAN,
	  sizeof (skill_table));

  //
  // Ok, now for the hard part... Setting up all the weave pointers.
  // All valid skills should have a counterpart in the command table, thus having
  // a valid skill function.
  //
  {
#if defined(_WIN32)
	HMODULE handle;
	FARPROC fPtr;
#else
	void *handle;
	int (*fPtr) (int);
#endif

#if !defined(_WIN32)
	/* load the symbol table from ./wot */
	handle = dlopen (NULL, RTLD_LAZY);
	
	if (!handle)
	{	
		perror (dlerror ());
		_quit (errno);
	}
#else
	handle = GetModuleHandle(APPNAME);
	
	if (!handle)
	{
		bugf("[data_h.c::load_skill_data] Unable to load symbol table!");
		_quit (errno);
	}
#endif

	for (count = 0; count != maxSkill; count++)
	{
		memset (buf, 0, MIL);
		
		if (!IS_NULLSTR (skill_table[count].name))
		{
			sprintf (buf, "weave_%s", fix_skill_name(skill_table[count].name));
		}
		else
			break;
		
#if !defined(_WIN32)
		fPtr = (int (*)(int)) dlsym (handle, buf);
		errStr = dlerror ();
#else
		fPtr = GetProcAddress(handle, buf);
		errStr = LastErrorStr();
#endif

		// only skills that have a weave_(skillname) function counterpart will be found,
		// so we can assume that it is a weave, and not just a skill. (skills would have
		// do_(skillname) functions, not weave_(skillname).
		if (fPtr)
			skill_table[count].weave_fun = (WEAVE_FUN *) fPtr;
	}
	
#if !defined(_WIN32)
	dlclose (handle);
	//	#else
	//	  FreeLibrary(handle);
#endif

    
  }

  return TRUE;
  
}

/** Function: save_class_data
  * Descr   : Saves the current class data for each class
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 3/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_class_data (void)
{
	FILE *fp;
	char buf[MIL];
	sh_int i;
	
	sprintf (buf, "%s%s", DATA_DIR, CLASS_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_class_data] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	fprintf (fp, "#\n");
	fprintf (fp, "## Class Table Data file\n");
	fprintf (fp, "## Written %s", ctime (&current_time));
	fprintf (fp, "## Contains %d classes.\n", MAX_CLASS);
	fprintf (fp, "## File Format\n");
	fprintf (fp, "#\n");
	fprintf (fp, "# Name               // The full name of the class\n");
	fprintf (fp, "# WhoName            // Three-letter name for 'who'\n");
	fprintf (fp, "# Prime              // Prime attribute\n");
	fprintf (fp, "# Weapon             // First weapon (vnum)\n");
	fprintf (fp, "# Adept              // Maximum skill level learned via 'practice'\n");
	fprintf (fp, "# Thac0              // Thac0 for level 0\n");
	fprintf (fp, "# Thac32             // Thac0 for level 32\n");
	fprintf (fp, "# MinHp              // Min hp gained on leveling\n");
	fprintf (fp, "# MaxHp              // Max hp gained on leveling\n");
	fprintf (fp, "# BaseGrp            // Base skill group\n");
	fprintf (fp, "# DefaultGrp         // Default skill group\n");
	fprintf (fp, "#\n# attr_prime values are given in enums.h as \'_STAT\'\n#\n");
	
	for (i = 0; i < MAX_CLASS; i++)
	{
		fprintf (fp, "Name       %s~\n", class_table[i].name);
		fprintf (fp, "WhoName    %s~\n", class_table[i].who_name);
		fprintf (fp, "Prime      %d\n", class_table[i].attr_prime);
		fprintf (fp, "Weapon     %ld\n", class_table[i].weapon);
		fprintf (fp, "Adept      %d\n", class_table[i].skill_adept);
		fprintf (fp, "Thac0      %d\n", class_table[i].thac0_00);
		fprintf (fp, "Thac32     %d\n", class_table[i].thac0_32);
		fprintf (fp, "MinHp      %d\n", class_table[i].hp_min);
		fprintf (fp, "MaxHp      %d\n", class_table[i].hp_max);
		fprintf (fp, "BaseGrp    %s~\n", class_table[i].base_group);
		fprintf (fp, "DefaultGrp %s~\n\n", class_table[i].default_group);
	}
	
	fprintf (fp, "#0\nEnd\n");
	fclose (fp);
	
	logf ("Class data file saved as: %s", buf);
	
	return TRUE;
}


/** Function: load_class_data
  * Descr   : Loads class specific data from datafile.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 3/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_class_data (void)
{
	FILE *fp;
	char buf[MIL];
	char *string;
	bool fMatch = FALSE;
	sh_int class = -1;
	
	sprintf (buf, "%s%s", DATA_DIR, CLASS_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_class_data] [%s] Could not open data file %s, Creating.",
			errmsg (errno), buf);
		return save_class_data ();
	}
	
	memset (class_table, 0, sizeof (class_table[0]) * MAX_CLASS);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
		{
			fMatch = TRUE;
			break;
		}
		
		fMatch = FALSE;
		
		switch (UPPER (string[0]))
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;			/* comment identifier */
			case 'A':
				KEY ("Adept", class_table[class].skill_adept, fread_number (fp));
				break;
			case 'B':
				KEY ("BaseGrp", class_table[class].base_group, fread_string (fp));
				break;
			case 'D':
				KEY ("DefaultGrp", class_table[class].default_group, fread_string (fp));
				break;
			case 'M':
				KEY ("MaxHp", class_table[class].hp_max, fread_number (fp));
				KEY ("MinHp", class_table[class].hp_min, fread_number (fp));
				break;
			case 'N':
				if (!str_cmp (string, "Name"))
				{
					fMatch = TRUE;
					class++;
					class_table[class].name = fread_string (fp);
				}
				break;
			case 'P':
				KEY ("Prime", class_table[class].attr_prime, fread_number (fp));
				break;
			case 'T':
				KEY ("Thac0", class_table[class].thac0_00, fread_number (fp));
				KEY ("Thac32", class_table[class].thac0_32, fread_number (fp));
				break;
			case 'W':
				KEY ("Weapon", class_table[class].weapon, fread_number (fp));
				KEY ("WhoName", class_table[class].who_name, fread_string (fp));
				break;
			default:
				break;
		}
		
		if (!fMatch)
		{
			bugf ("[data_h.c::load_class_data] Invalid Key: %s", string);
			fread_to_eol(fp);
		}
	}
	
	fclose (fp);
	logf (NORMAL "Loaded %d Classes." NO_COLOR, CYAN, class + 1);
	
	return TRUE;
}

/** Function: save_slay_data
  * Descr   : Saves the current slay_table data
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 3/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_slay_data (void)
{
	FILE *fp;
	char buf[MIL];
	sh_int i;
	sh_int count = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, SLAY_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		logf ("[data_h.c::save_slay_data] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	/* count the active slay types, if any */
	for (i = 0; i < MAX_SLAY_TYPES; i++)
		if (slay_table[i].title && slay_table[i].title[0] != '\0')
			count++;
		
		fprintf (fp, "#\n");
		fprintf (fp, "## Slay Table Data file\n");
		fprintf (fp, "## Written %s", ctime (&current_time));
		fprintf (fp, "## Contains %d slay types.\n", count);
		fprintf (fp, "## File Format\n");
		fprintf (fp, "#\n");
		fprintf (fp, "# Owner      // The name of the Owner of this slay type\n");
		fprintf (fp,
			"#            // If 'Owner' is defined, only that person will be\n");
		fprintf (fp, "#            // able to see or use this type.\n");
		fprintf (fp, "# Title      // The title of the slay type.\n");
		fprintf (fp, "# CharMsg    // The message sent to the slayer.\n");
		fprintf (fp, "# VictMsg    // The message sent to the victim.\n");
		fprintf (fp, "# RoomMsg    // The message sent to everyone else.\n");
		fprintf (fp, "#\n");
		
		for (i = 0; i < MAX_SLAY_TYPES; i++)
		{
			if (slay_table[i].title && slay_table[i].title[0] != '\0')
			{
				fprintf (fp, "\nOwner   %s~\n", slay_table[i].owner);
				fprintf (fp, "Title   %s~\n", slay_table[i].title);
				fprintf (fp, "CharMsg %s~\n", slay_table[i].char_msg);
				fprintf (fp, "VictMsg %s~\n", slay_table[i].vict_msg);
				fprintf (fp, "RoomMsg %s~\n", slay_table[i].room_msg);
			}
		}
		
		fprintf (fp, "#0\nEnd\n");
		fclose (fp);
		
		logf ("slay_table data file saved as: %s", buf);
		
		return TRUE;
}


/** Function: load_slay_data
  * Descr   : Loads slay table data from datafile.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 3/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_slay_data (void)
{
	FILE *fp;
	char buf[MIL];
	char *string;
	bool fMatch = FALSE;
	sh_int slay = -1;
	
	sprintf (buf, "%s%s", DATA_DIR, SLAY_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_slay_data] [%s] Could not open data file %s. Creating.",
			errmsg (errno), buf);
		return save_slay_data ();
	}
	
	
	memset (slay_table, 0, sizeof (slay_table[0]) * MAX_SLAY_TYPES);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
		{
			fMatch = TRUE;
			break;
		}
		
		fMatch = FALSE;
		
		switch (UPPER (string[0]))
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;			/* comment identifier */
			case 'C':
				KEY ("CharMsg", slay_table[slay].char_msg, fread_string (fp));
				break;
			case 'O':
				if (!str_cmp (string, "Owner"))
				{
					slay++;
					slay_table[slay].owner = fread_string (fp);
					fMatch = TRUE;
				}
				break;
			case 'R':
				KEY ("RoomMsg", slay_table[slay].room_msg, fread_string (fp));
				break;
			case 'T':
				KEY ("Title", slay_table[slay].title, fread_string (fp));
				break;
			case 'V':
				KEY ("VictMsg", slay_table[slay].vict_msg, fread_string (fp));
				break;
			default:
				break;
		}
		
		if (!fMatch)
		{
			bugf ("[data_h.c::load_slay_data] Invalid Key: %s", string);
			fread_to_eol(fp);
		}
	}
	
	fclose (fp);
	logf (NORMAL "Loaded %d Slay types." NO_COLOR, CYAN, slay + 1);
	
	return TRUE;
}

/** Function: save_guilds
  * Descr   : Writes the clan_table to /data/guild.dat file.
  * Returns : (N/A)
  * Syntax  : (N/A)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_guilds ()
{
	FILE *fp;
	int i;
	int value;
	char buf[MSL];
	
	sprintf (buf, "%sguild.dat", DATA_DIR);
	
	if (!(fp = fopen (buf, "w+")))
	{
		bugf ("[data_h.c::save_guilds] Error opening data file %s.", buf);
		return FALSE;
	}
	
	/* Determine # of guilds to save */
	value = 0;
	for (i = 0; i < maxClan; i++)
		if (clan_table[i].name
			&& clan_table[i].name[0] != '\0'
			&& !IS_FLAG (clan_table[i].flags, _DELETED))
			value++;
		
		fprintf (fp, "#%d\n", value);
		fprintf (fp, "## Guild Table Data file\n## Written %s",
			ctime (&current_time));
		fprintf (fp, "## Contains %d guilds.\n", value);
		fprintf (fp, "## File Format\n#\n");
		fprintf (fp, "# Guild  (string)           // The Name of this guild.\n");
		fprintf (fp,
			"# Who    (string)           // The name shown in 'who' listing.\n");
		fprintf (fp,
			"# Rooms # # # (numeric)     // The hall|morgue&temple room vnums.\n");
		fprintf (fp,
			"# Rank  # (number) (string) // The rank name and number of this rank.\n");
		fprintf (fp,
			"# Skill # (number) (string) // The skill associated with rank (number)\n");
		fprintf (fp,
			"# ML # # # # (all numeric)  // The Mortal leader bits for this guild.\n");
		fprintf (fp, "# Flags x (bitarray)        // This guilds flags.\n");
		fprintf (fp, "#\n");
		
		for (i = 0; i < maxClan; i++)
		{
			if (clan_table[i].name
				&& clan_table[i].name[0] != '\0'
				&& !IS_FLAG (clan_table[i].flags, _DELETED))
			{
				fprintf (fp, "\nGuild\t%s~\n", clan_table[i].name);
				fprintf (fp, "Who\t%s~\n", clan_table[i].who_name);
				fprintf (fp, "Rooms\t%d\t%d\t%d\n",
					clan_table[i].room[0],
					clan_table[i].room[1], clan_table[i].room[2]);
				
				for (value = 0; value < MAX_RANK; value++)
				{
					if (clan_table[i].rank[value].rankname == NULL ||
						clan_table[i].rank[value].rankname[0] == '\0')
						clan_table[i].rank[value].rankname = str_dup ("Unassigned");
					
					fprintf (fp, "Rank\t%d\t%s~\n",
						value + 1, clan_table[i].rank[value].rankname);
				}
				
				for (value = 0; value < MAX_RANK; value++)
					if (clan_table[i].rank[value].skillname != NULL &&
						clan_table[i].rank[value].skillname[0] != '\0')
						fprintf (fp, "Skill\t%d\t%s~\n",
						value + 1, clan_table[i].rank[value].skillname);
					
					fprintf (fp, "ML\t%d  %d  %d  %d\n",
						clan_table[i].ml[0],
						clan_table[i].ml[1], clan_table[i].ml[2], clan_table[i].ml[3]);
					
					/* Remove "changed" bit before writing flags to file */
					if (IS_FLAG (clan_table[i].flags, _CHANGED))
						REMOVE_FLAG (clan_table[i].flags, _CHANGED);
					
					fprintf (fp, "Flags %s \n", print_bits (clan_table[i].flags));
					
					fprintf (fp, "\n");
			}
		}
		
		fprintf (fp, "\nEnd\n");
		fclose (fp);
		logf ("Guild data file saved.");
		return TRUE;
}


/** Function: load_guilds
  * Descr   : Loads the information specific to each guild/clan from
  *           the ../data/guild.dat file.
  * Returns : (n/a)
  * Syntax  : (n/a)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_guilds (void)
{
	FILE *fp;
	char buf[MSL];
	char *string;
	int count = 0;
	int i;
	bool fMatch = FALSE;
	
	sprintf (buf, "%sguild.dat", DATA_DIR);
	
	if ((fp = fopen (buf, "r")) == NULL)
	{
		bugf("[data_h.c::load_guilds] Could not open data file %s.", buf);
		_quit (1);
	}
	
	fscanf (fp, "#%d\n", &maxClan);
	clan_table = malloc (sizeof (struct clan_type) * (maxClan));
	memset (clan_table, 0, sizeof (clan_table[0]) * maxClan);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End") || count > maxClan)
			break;
		
		switch (UPPER (string[0]))
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;
				
			case 'F':
				fread_bits (fp, clan_table[count].flags);
				count++;
				fMatch = TRUE;
				break;
				
			case 'G':
				clan_table[count].name = fread_string (fp);
				fMatch = TRUE;
				break;
				
			case 'R':
				if (!str_cmp (string, "Rooms"))
				{
					clan_table[count].room[0] = fread_number (fp);	/* hall   */
					clan_table[count].room[1] = fread_number (fp);	/* morgue */
					clan_table[count].room[2] = fread_number (fp);	/* temple */
					fMatch = TRUE;
				}
				else if (!str_cmp (string, "Rank"))
				{
					i = fread_number (fp);
					clan_table[count].rank[i - 1].rankname = fread_string (fp);
					fMatch = TRUE;
				}
				break;
				
			case 'S':
				i = fread_number (fp);
				clan_table[count].rank[i - 1].skillname = fread_string (fp);
				fMatch = TRUE;
				break;
				
			case 'M':
				clan_table[count].ml[0] = fread_number (fp);
				clan_table[count].ml[1] = fread_number (fp);
				clan_table[count].ml[2] = fread_number (fp);
				clan_table[count].ml[3] = fread_number (fp);
				fMatch = TRUE;
				break;
				
			case 'W':
				clan_table[count].who_name = fread_string (fp);
				fMatch = TRUE;
				break;
				
		}				/* end of switch */
		
		if (!fMatch)
		{
			bugf ("[data_h.c::fread_guilds] Invalid Key: %s", string);
			fread_to_eol (fp);
		}

	}				/* end of while (!feof) */
	
	fclose (fp);
	
	if (count > 0)
		logf (NORMAL "Loaded %d Guilds." NO_COLOR, CYAN, count);
	
	return TRUE;
}				/* end: load_guilds */



bool save_countries(void)
{
	FILE *fp;
	char buf[MIL];
	int i,z, value = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, NATION_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_countries] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	for (i = 0; nation_table[i].nation; i++)
		if (!IS_NULLSTR(nation_table[i].nation))
			value++;
		
		fprintf (fp, "#%d\n", value);
		fprintf (fp, "## Nation Data file\n## Written %s",
			ctime (&current_time));
		fprintf (fp, "## Contains %d nations.\n", value);
		fprintf (fp, "## File Format\n#\n");
		fprintf (fp, "#\n");
		
		
		for (i = 0; nation_table[i].nation; i++)
		{
			fprintf(fp, "NATION\t%s~  %s~\t%s\n",
				nation_table[i].nation,
				nation_table[i].nationid,
				(nation_table[i].selectable) ? "TRUE" : "FALSE");
			
			for (z=0; nation_table[i].cities[z].city; z++)
			{
				if ( !IS_NULLSTR(nation_table[i].cities[z].city) )
				{
					fprintf(fp, "\tCITY\t%s~\n", nation_table[i].cities[z].city);
					
					if (!IS_NULLSTR(nation_table[i].cities[z].skill))
					{
						int x;
						
						fprintf(fp, "\tSKILL\t%s~\t%d %s\t",
							nation_table[i].cities[z].skill, 
							nation_table[i].cities[z].bonus,
							(nation_table[i].cities[z].autoskill) ? "TRUE" : "FALSE");
						
						for (x=0; x < MAX_STATS; x++)
							fprintf(fp, "%d ", nation_table[i].cities[z].attr_bonus[x]);
						
						fprintf(fp, "\n");
					}
				}
			}
		}
		fprintf(fp, "\nEnd\n");
		fclose(fp);
		
		return TRUE;
		
}


/** Function: load_countries
  * Descr   : Loads the nations into the nation table from the datafile
  * Returns : void
  * Syntax  : void
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_countries (void)
{
	FILE *fp;
	int nations = -1;
	int cstates = -1;
	char *string;
	char buf[MSL];
	
	memset (nation_table, 0, sizeof (nation_table[0]) * MAX_NATIONS);
	sprintf (buf, "%s%s", DATA_DIR, NATION_FILE);
	
	if (!(fp = fopen (buf, "r")))
	{
		bugf("[data_h.c::load_countries] Could not open data file %s.", buf);
		return FALSE;
	}
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "#") || !str_cmp (string, ""))
			continue;
		if (!str_cmp (string, "End"))
			break;
		
		if (!str_cmp (string, "NATION"))
		{
			nations++;
			nation_table[nations].nation = fread_string (fp);
			
			nation_table[nations].nationid = fread_string (fp);
			if (nation_table[nations].nationid[0] == '\0')
				nation_table[nations].nationid =
				str_dup (nation_table[nations].nation);
			
			nation_table[nations].selectable = (!str_cmp (fread_word (fp), "TRUE"));
			cstates = -1;		/* reset city counter */
		}				/* end: if string = NATION */
		
		else
			if (!str_cmp (string, "CITY"))
			{
				
				cstates++;		/* set to -1 to by NATION, reset to 0 here. */
				
				nation_table[nations].cities[cstates].city = fread_string (fp);
				
			}				/* end: if string = CITY */
			else
				if (!str_cmp (string, "SKILL"))
				{
					int x;
					
					nation_table[nations].cities[cstates].skill = fread_string (fp);
					
					nation_table[nations].cities[cstates].bonus = fread_number (fp);
					nation_table[nations].cities[cstates].autoskill =
						(!str_cmp (fread_word (fp), "TRUE"));
					
					for (x = 0; x < MAX_STATS; x++)
						nation_table[nations].cities[cstates].attr_bonus[x] =
						fread_number (fp);
				}
				
	}				/* end: for() */
	
	logf (NORMAL "Loaded %d Nations." NO_COLOR, CYAN, nations+1);
	fclose (fp);
	return TRUE;
}

/** Function: save_forms
  * Descr   : Saves the forms linked list to file for later reloading.
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 6/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool save_forms (void)
{
	FILE *fp;
	char buf[MIL];
	int i, value = 0;
	
	sprintf (buf, "%s%s", DATA_DIR, FORM_FILE);
	
	if (!(fp = fopen (buf, "w")))
	{
		bugf("[data_h.c::save_forms] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return FALSE;
	}
	
	for (i = 0; i < maxForm; i++)
		if (form_table[i].form && form_table[i].form[0] != '\0')
			value++;
		
		fprintf (fp, "#%d\n", value);
		fprintf (fp, "## Blademaster Forms Data file\n## Written %s", ctime (&current_time));
		fprintf (fp, "## Contains %d forms.\n", value);
		fprintf (fp, "## File Format\n#\n");
		fprintf (fp, "# Form              // The Name of this form.\n");
		fprintf (fp, "# aType             // The Attack/Defense Type.\n");
		fprintf (fp, "#                   // These are:\n"
					 "#                   // 0 = ATTACK    - This is an offensive form.\n"
					 "#                   // 1 = DEFEND    - This is a defensive form.\n"
					 "#                   // 2 = EITHER    - This form works as both.\n");
		fprintf (fp, "# fType             // The affect type.\n");
		fprintf (fp, "#                   // These are:\n"
			         "#                   // 0 = NONE      - Form does nothing constructive.\n"
					 "#                   // 1 = AVOID     - Form attempts to avoid hits.\n"
					 "#                   // 2 = RIPOSTE   - Form ripostes attacks.\n"
					 "#                   // 3 = DAMAGE    - Form does normal attack damage.\n"
					 "#                   // 4 = CRITICAL  - Form does critical damage.\n"
					 "#                   // 5 = SIMUL     - Form accepts hits, and hits back.\n"
					 "#                   // 6 = EXTRA     - Form does extra damage.\n"
					 "#                   // 7 = DISARM    - Form disarms opponent.\n"
					 "#                   // 8 = BREAK     - Form breaks opponents weapon.\n"
					 "#                   // 9 = KNOCKDOWN - Form knocks opponent down.\n"
					 "#                   // 10= BLIND     - Form blinds opponent temporarily.\n"
					 "#                   // 11= MULTI     - Form does multiple attacks.\n");
		fprintf (fp, "# difficulty        // The difficulty level.\n");
		fprintf (fp, "# Nation            // Specific nation?\n");
		fprintf (fp, "# toChar            // Message blademaster sees.\n");
		fprintf (fp, "# toVict            // Message victim sees.\n");
		fprintf (fp, "# toRoom            // Message the room sees.\n");
		fprintf (fp, "#\n");
		

		for (i = 0; i < maxForm; i++)
		{
			fprintf (fp, "Form     %s~\n", form_table[i].form);
			fprintf (fp, "aType    %d\n", form_table[i].aType);
			fprintf (fp, "fType    %d\n", form_table[i].fType);
			fprintf (fp, "Diff     %d\n", form_table[i].difficulty);
			fprintf (fp, "Nation   %s~\n", form_table[i].nation);
			fprintf (fp, "CharMsg  %s~\n", form_table[i].toChar);
			fprintf (fp, "VictMsg  %s~\n", form_table[i].toVict);
			fprintf (fp, "RoomMsg  %s~\n\n", form_table[i].toRoom);
		}
		
		fprintf (fp, "End\n");
		fclose (fp);
		return TRUE;
}

/** Function: load_forms
  * Descr   : Reads in data/forms.dat and sets the forms_table accordingly
  * Returns : TRUE on success
  * Syntax  : void
  * Written : v1.0 6/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool load_forms (void)
{
	FILE *fp;
	char buf[MSL];
	char *string;
	bool fMatch = FALSE;
	int Forms = -1;
	
	sprintf (buf, "%s%s", DATA_DIR, FORM_FILE);
	
	if ((fp = fopen (buf, "r")) == NULL)
	{
		bugf("[data_h.c::load_forms] [%s] Could not open data file %s.",
			errmsg (errno), buf);
		return errno;
	}
	
	maxForm = -1;
	fscanf (fp, "#%d\n", &maxForm);
	form_table = malloc (sizeof (struct form_type) * (maxForm));
	memset (form_table, 0, sizeof (form_table[0]) * maxForm);
	
	for (;;)
	{
		string = feof (fp) ? "End" : fread_word (fp);
		
		if (!str_cmp (string, "End"))
			break;
		
		switch (string[0])
		{
			case '#':
				fMatch = TRUE;
				fread_to_eol (fp);
				break;
			case 'F':
				fMatch = TRUE;
				Forms++;
				form_table[Forms].form = fread_string (fp);
				break;
			case 'a':
				{
					fMatch = TRUE;
					form_table[Forms].aType = fread_number (fp);
					break;
				}
			case 'f':
				fMatch = TRUE;
				form_table[Forms].fType = fread_number (fp);
				break;
			case 'D':
				fMatch = TRUE;
				form_table[Forms].difficulty = fread_number (fp);
				break;
			case 'N':
				fMatch = TRUE;
				form_table[Forms].nation = fread_string (fp);
				break;
			case 'C':
				fMatch = TRUE;
				form_table[Forms].toChar = fread_string (fp);
				break;
			case 'V':
				fMatch = TRUE;
				form_table[Forms].toVict = fread_string (fp);
				break;
			case 'R':
				fMatch = TRUE;
				form_table[Forms].toRoom = fread_string (fp);
				break;
		}
		
		if (!fMatch)
		{
			bugf("[data_h.c::fread_forms] Invalid Key: %s", string);
			fread_to_eol (fp);
		}
	
	}				/* end: for(;;) */
	
	fclose (fp);
	logf (NORMAL "Loaded %d Forms." NO_COLOR, CYAN, Forms + 1);
	return TRUE;
	
}				/* end: load_forms */
