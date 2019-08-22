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

/*
 * @@ Objective ()
 * 
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "recycle.h"

#define MPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)


const struct olc_cmd_type mpedit_table[] = {
	/*	{	command		function	}, */
	
	{"commands", show_commands},
	{"create", mpedit_create},
	{"code", mpedit_code},
	{"show", mpedit_show},
	{"author", mpedit_author},
	{"title", mpedit_title},
	{"delete", mpedit_delete},
	{"copy", mpedit_copy},
	{"?", show_help},
	
	{NULL, 0}
};

void mpedit (CHAR_DATA * ch, char *argument)
{
	MPROG_CODE *pMcode;
	char arg[MIL];
	char command[MIL];
	int cmd;
	AREA_DATA *ad;
	
	smash_tilde (argument);
	strcpy (arg, argument);
	argument = one_argument (argument, command);
	
	EDIT_MPCODE (ch, pMcode);
	
	if (pMcode)
	{
		ad = get_vnum_area (pMcode->vnum);
		
		if (ad == NULL)		/* ??? */
		{
			edit_done (ch);
			return;
		}
		
		if (!IS_BUILDER (ch, ad))
		{
			send_to_char ("MPEdit: Insufficient security to modify code.\n\r", ch);
			edit_done (ch);
			return;
		}
	}
	
	if (command[0] == '\0')
	{
		mpedit_show (ch, argument);
		return;
	}
	
	if (!str_cmp (command, "done"))
	{
		edit_done (ch);
		return;
	}
	
	for (cmd = 0; mpedit_table[cmd].name != NULL; cmd++)
	{
		if (!str_prefix (command, mpedit_table[cmd].name))
		{
			if ((*mpedit_table[cmd].olc_fun) (ch, argument) && pMcode)
				if ((ad = get_vnum_area (pMcode->vnum)) != NULL)
					SET_BIT (ad->area_flags, AREA_CHANGED);
				return;
		}
	}
	
	interpret (ch, arg);
	
	return;
}

_DOFUN (do_mpedit)
{
	MPROG_CODE *pMcode;
	char command[MIL];
	
	argument = one_argument (argument, command);
	
	if (is_number (command))
	{
		long vnum = atol (command);
		AREA_DATA *ad;
		
		if ((pMcode = get_mprog_index (vnum)) == NULL)
		{
			send_to_char ("MPEdit : That vnum does not exist.\n\r", ch);
			return;
		}
		
		ad = get_vnum_area (vnum);
		
		if (ad == NULL)
		{
			send_to_char ("MPEdit : VNUM no asignado a ningun area.\n\r", ch);
			return;
		}
		
		if (!IS_BUILDER (ch, ad))
		{
			send_to_char ("MPEdit : Insuficiente seguridad para editar area.\n\r",
				ch);
			return;
		}
		
		ch->desc->pEdit = (void *) pMcode;
		ch->desc->editor = ED_MPCODE;
		
		return;
	}
	
	if (!str_cmp (command, "delete"))
	{
		if (!IS_NULLSTR (argument))
		{
			mpedit_delete (ch, argument);
			return;
		}
		else
		{
			send_to_char ("Syntax: mpedit delete [vnum]\n\r", ch);
			return;
		}
	}
	if (!str_cmp (command, "create"))
	{
		if (argument[0] == '\0')
		{
			send_to_char ("Syntax : mpedit create [vnum]\n\r", ch);
			return;
		}
		
		mpedit_create (ch, argument);
		return;
	}
	
	send_to_char ("Syntax : mpedit [vnum]\n\r", ch);
	send_to_char ("         mpedit create [vnum]\n\r", ch);
	send_to_char ("			mpedit delete [vnum]\n\r", ch);
	
	return;
}

MPEDIT (mpedit_create)
{
	MPROG_CODE *pMcode;
	int value = atoi (argument);
	AREA_DATA *ad;
	
	if (IS_NULLSTR (argument) || value < 1)
	{
		send_to_char ("Syntax : mpedit create [vnum]\n\r", ch);
		return FALSE;
	}
	
	ad = get_vnum_area (value);
	
	if (ad == NULL)
	{
		send_to_char ("MPEdit : VNUM no asignado a ningun area.\n\r", ch);
		return FALSE;
	}
	
	if (!IS_BUILDER (ch, ad))
	{
		send_to_char ("MPEdit : Insuficiente seguridad para crear MobProgs.\n\r",
			ch);
		return FALSE;
	}
	
	if (get_mprog_index (value))
	{
		send_to_char ("MPEdit: Code vnum already exists.\n\r", ch);
		return FALSE;
	}
	
	pMcode = new_mpcode ();
	pMcode->vnum = value;
	pMcode->next = mprog_list;
	mprog_list = pMcode;
	ch->desc->pEdit = (void *) pMcode;
	ch->desc->editor = ED_MPCODE;
	pMcode->author = ch->name;
	pMcode->title = "`NUnknown`w";
	
	send_to_char ("MobProgram Code Created.\n\r", ch);
	
	return TRUE;
}

MPEDIT (mpedit_show)
{
	MPROG_CODE *pMcode;
	MOB_INDEX_DATA *pMob;
	char buf[MSL];
	long vnum;
	
	EDIT_MPCODE (ch, pMcode);
	
	buf[0] = '\0';
	
	send_to_char
		("`N=============================================================================`w\n\r",
		ch);
	chprintf (ch,
		"`yVnum: `w%5ld     `yTitle: `r%s\n\r" "`yAuthor(s): `r%s\n\r"
		"`yCode:\n\r" "`c%s`w\n\r", pMcode->vnum, pMcode->title,
		pMcode->author, pMcode->code);
	send_to_char
		("`N=============================================================================`w\n\r",
		ch);
	
	for (vnum = 0; vnum <= top_vnum_mob; vnum++)
	{
		if ((pMob = get_mob_index (vnum)) == NULL)
			continue;
		if (pMob->mprogs == NULL)
			continue;
		if (pMob->mprogs->vnum == pMcode->vnum)
		{
			sprintf (buf, "`w%5ld %s (%s) uses prog %5ld.`w\n\r",
				pMob->vnum, pMob->short_descr,
				pMob->player_name, pMob->mprogs->vnum);
			send_to_char (buf, ch);
		}
	}
	
	if (IS_NULLSTR (buf))
		chprintf (ch, "`wCurrently no mobiles use this program.`w\n\r");
	send_to_char
		("`N=============================================================================`w\n\r",
		ch);
	
	return FALSE;
}
MPEDIT (mpedit_code)
{
	MPROG_CODE *pMcode;
	EDIT_MPCODE (ch, pMcode);
	
	if (argument[0] == '\0')
	{
		string_append (ch, &pMcode->code);
		return TRUE;
	}
	
	send_to_char ("Syntax: code\n\r", ch);
	return FALSE;
}

MPEDIT (mpedit_author)
{
	MPROG_CODE *pMcode;
	
	EDIT_MPCODE (ch, pMcode);
	
	free_string (pMcode->author);
	pMcode->author = str_dup (argument);
	
	return FALSE;
}

MPEDIT (mpedit_title)
{
	MPROG_CODE *pMcode;
	
	EDIT_MPCODE (ch, pMcode);
	
	free_string (pMcode->title);
	pMcode->title = str_dup (argument);
	
	return FALSE;
}

MPEDIT (mpedit_copy)
{
	MPROG_CODE *pMcode;
	MPROG_CODE *pMcode2;		/* The mob to copy */
	long vnum;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Syntax: copy <vnum> \n\r", ch);
		return FALSE;
	}
	
	if (!is_number (argument))
	{
		send_to_char ("MPEdit: You must enter a number (vnum).\n\r", ch);
		return FALSE;
	}
	else				/* argument is a number */
	{
		vnum = atoi (argument);
		if (!(pMcode2 = get_mprog_index (vnum)))
		{
			send_to_char ("MPedit: That mprog does not exist.\n\r", ch);
			return FALSE;
		}
	}
	
	EDIT_MPCODE (ch, pMcode);
	
	free_string (pMcode->code);
	free_string (pMcode->author);
	free_string (pMcode->title);
	
	pMcode->code = str_dup (pMcode2->code);
	pMcode->author = str_dup (pMcode2->author);
	pMcode->title = str_dup (pMcode2->title);
	
	send_to_char ("Mob prog code copied.\n\r", ch);
	
	return TRUE;
}

MPEDIT (mpedit_delete)
{
	MPROG_CODE *mpcode, *mpcode2, *mpcode_next;
	char arg[MIL];
	char buf[MSL];
	int index, mpcount;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Syntax:  mpedit delete [vnum]\n\r", ch);
		return FALSE;
	}
	
	one_argument (argument, arg);
	
	if (is_number (arg))
	{
		index = atoi (arg);
		mpcode = get_mprog_index (index);
	}
	else
	{
		send_to_char ("That is not a number.\n\r", ch);
		return FALSE;
	}
	
	if (!mpcode)
	{
		send_to_char ("No such mprog.\n\r", ch);
		return FALSE;
	}
	
	/* is the person editing this object at the moment? */
	{
		if (ch->desc->pEdit
			&& ((MPROG_CODE *) ch->desc->pEdit)->vnum == mpcode->vnum)
		{
			/* yep, they sure are... */
			send_to_char ("Dropping you out of MPEdit.\n\r", ch);
			edit_done (ch);
		}
	}
	
	/* remove objects */
	mpcount = 0;
	for (mpcode2 = mprog_list; mpcode2; mpcode2 = mpcode_next)
	{
		mpcode_next = mpcode2->next;
		
		if (mpcode2->vnum == mpcode->vnum)
		{
			extract_mpcode (mpcode2);
			mpcount++;
		}
	}
	
	free_mpcode (mpcode);
	
	sprintf (buf,
		"Removed mprog vnum `C%d`w and `C%d`w occurences of the object were extracted from the mud.\n\r",
		index, mpcount);
	
	return TRUE;
}
