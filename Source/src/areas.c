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
 * @@ Objective (areas.c)
 *
 * This file contains the functions that read the area files and parse
 * all areas/mobs/rooms/objects/resets/sounds/shops from each areafile.
 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include "merc.h"





/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
                if ( !str_cmp( word, literal ) )    \
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
                }

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
                    free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
                }




/* Load the bank information (economy info)
 * By Maniac from Mythran Mud
 */
void load_bank (void)
{
	FILE *fp;
	int number = 0;
	char file[MIL];
	
	sprintf (file, "%s%s", DATA_DIR, BANK_FILE);
	
	if (!(fp = fopen (file, "r")))
		return;
	
	for (;;)
	{
		char *word;
		char letter;
		
		do
		{
			letter = getc (fp);
			if (feof (fp))
			{
				fclose (fp);
				return;
			}
		}
		while (isspace (letter));
		ungetc (letter, fp);
		
		word = fread_word (fp);
		
		if (!str_cmp (word, "SHARE_VALUE"))
		{
			number = fread_number (fp);
			if (number > 0)
				pMud.share_value = number;
		}
	}
} /* end of load_bank() */


/* Snarf a mob section.  new style */
void load_mobiles (FILE * fp)
{
	MOB_INDEX_DATA *pMobIndex;
	//  long flag;

	if (!area_last)		
	{
		bugf ("[areas.c::load_mobiles] no #AREA seen yet.");
		_quit (1);
	}
	
	for (;;)
	{
		long vnum;
		char letter;
		int iHash;
		
		letter = fread_letter (fp);
		if (letter != '#')
		{
			bugf("[areas.c::load_mobiles] # not found.");
			_quit (1);
		}
		
		vnum = fread_long (fp);
		
		if (vnum == 0)
			break;
		
		fBootDb = FALSE;
		
		if (get_mob_index (vnum) != NULL)
		{
			bugf("[areas.c::load_mobiles] vnum %ld duplicated.", vnum);
			_quit (1);
		}
		fBootDb = TRUE;
		
		pMobIndex = new_mob_index ();
		pMobIndex->vnum = vnum;
		pMobIndex->area = area_last;	
		pMobIndex->player_name = fread_string (fp);
		pMobIndex->short_descr = fread_string (fp);
		pMobIndex->long_descr = fread_string (fp);
		pMobIndex->description = fread_string (fp);
		pMobIndex->race = race_lookup (fread_string (fp));
		
		pMobIndex->long_descr[0] = UPPER (pMobIndex->long_descr[0]);
		pMobIndex->description[0] = UPPER (pMobIndex->description[0]);
		
		fread_bits (fp, pMobIndex->states.player);
		fread_bits (fp, pMobIndex->states.affected_by);
		
		if (!IS_FLAG (pMobIndex->states.player, _ACT_NPC))
			SET_FLAG (pMobIndex->states.player, _ACT_NPC);
		
		pMobIndex->pShop = NULL;
		
		pMobIndex->alignment = fread_number (fp);
		pMobIndex->group = fread_number (fp);
		
		pMobIndex->level = fread_number (fp);
		pMobIndex->hitroll = fread_number (fp);
		
		/* read hit dice */
		pMobIndex->hit[DICE_NUMBER] = fread_number (fp);
		/* 'd'          */ fread_letter (fp);
		pMobIndex->hit[DICE_TYPE] = fread_number (fp);
		/* '+'          */ fread_letter (fp);
		pMobIndex->hit[DICE_BONUS] = fread_number (fp);
		
		/* read OPR_dice dice */
		pMobIndex->OPR_dice[DICE_NUMBER] = fread_number (fp);
		fread_letter (fp);
		pMobIndex->OPR_dice[DICE_TYPE] = fread_number (fp);
		fread_letter (fp);
		pMobIndex->OPR_dice[DICE_BONUS] = fread_number (fp);
		
		/* read damage dice */
		pMobIndex->damage[DICE_NUMBER] = fread_number (fp);
		fread_letter (fp);
		pMobIndex->damage[DICE_TYPE] = fread_number (fp);
		fread_letter (fp);
		pMobIndex->damage[DICE_BONUS] = fread_number (fp);
		
		pMobIndex->dam_type = attack_lookup (fread_word (fp));
		
		/* read armor class */
		pMobIndex->ac[AC_PIERCE] = fread_number (fp) * 10;
		pMobIndex->ac[AC_BASH] = fread_number (fp) * 10;
		pMobIndex->ac[AC_SLASH] = fread_number (fp) * 10;
		pMobIndex->ac[AC_EXOTIC] = fread_number (fp) * 10;
		
		/* read flags and add in data from the race table */
		fread_bits (fp, pMobIndex->states.off);
		fread_bits (fp, pMobIndex->states.imm);
		fread_bits (fp, pMobIndex->states.res);
		fread_bits (fp, pMobIndex->states.vuln);
		
		/* vital statistics */
		pMobIndex->start_pos = position_lookup (fread_word (fp));
		pMobIndex->default_pos = position_lookup (fread_word (fp));
		pMobIndex->sex = sex_lookup (fread_word (fp));
		pMobIndex->wealth = fread_number (fp);
		
		fread_bits (fp, pMobIndex->states.form);
		fread_bits (fp, pMobIndex->states.parts);
		
		/* size */
		pMobIndex->size = size_lookup (fread_word (fp));
		pMobIndex->material = str_dup (fread_word (fp));
		
		for (;;)
		{
			letter = fread_letter (fp);
			
			if (letter == 'F')
				fread_to_eol (fp);
			else if (letter == 'M')
			{
				MPROG_LIST *pMprog;
				char *word;
				int trigger = 0;
				
				pMprog = new_mprog ();
				word = fread_word (fp);
				
				if (!(trigger = flag_lookup (word, mprog_flags)))
				{
					bugf("[areas.c::load_mobiles<mobprogs>] invalid trigger.", 0);
					_quit (1);
				}
				
				SET_BIT (pMobIndex->mprog_flags, trigger);
				pMprog->trig_type = trigger;
				pMprog->vnum = fread_long (fp);
				pMprog->trig_phrase = fread_string (fp);
				pMprog->next = pMobIndex->mprogs;
				pMobIndex->mprogs = pMprog;
			}
			else
			{
				ungetc (letter, fp);
				break;
			}
		}
		
		iHash = vnum % MAX_KEY_HASH;
		pMobIndex->next = mob_index_hash[iHash];
		mob_index_hash[iHash] = pMobIndex;
		top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;	
		assign_area_vnum (vnum);	
		kill_table[URANGE (0, pMobIndex->level, MAX_LEVEL - 1)].number++;
  }
  
  return;
}

/* Snarf an obj section. new style */
void load_objects (FILE * fp)
{
	OBJ_INDEX_DATA *pObjIndex;
	
	if (!area_last)		
	{
		bugf ("[areas.c::load_objects] no #AREA seen yet.");
		_quit (1);
	}
	
	for (;;)
	{
		long vnum;
		char letter;
		int iHash;
		
		letter = fread_letter (fp);
		if (letter != '#')
		{
			bugf("[areas.c::load_objects] # not found.");
			_quit (1);
		}
		
		vnum = fread_long (fp);
		if (vnum == 0)
			break;
		
		fBootDb = FALSE;
		if (get_obj_index (vnum) != NULL)
		{
			bugf("[areas.c::load_objects] vnum %ld duplicated.", vnum);
			_quit (1);
		}
		fBootDb = TRUE;
		
		pObjIndex = new_obj_index ();
		pObjIndex->vnum = vnum;
		pObjIndex->area = area_last;	
		pObjIndex->reset_num = 0;
		pObjIndex->name = fread_string (fp);
		pObjIndex->short_descr = fread_string (fp);
		pObjIndex->description = fread_string (fp);
		pObjIndex->material = fread_string (fp);
		
		pObjIndex->item_type = item_lookup (fread_word (fp));
		pObjIndex->extra_flags = fread_flag (fp);
		pObjIndex->wear_flags = fread_flag (fp);
		switch (pObjIndex->item_type)
		{
		case ITEM_WEAPON:
			pObjIndex->value[0] = weapon_type (fread_word (fp));
			pObjIndex->value[1] = fread_number (fp);
			pObjIndex->value[2] = fread_number (fp);
			pObjIndex->value[3] = attack_lookup (fread_word (fp));
			pObjIndex->value[4] = fread_flag (fp);
			break;
		case ITEM_QUIVER:
		case ITEM_ARROW:
		case ITEM_ANGREAL:
			pObjIndex->value[0] = fread_number (fp);
			pObjIndex->value[1] = fread_number (fp);
			pObjIndex->value[2] = fread_number (fp);
			pObjIndex->value[3] = fread_number (fp);
			pObjIndex->value[4] = fread_number (fp);
			break;
		case ITEM_CONTAINER:
			pObjIndex->value[0] = fread_number (fp);
			pObjIndex->value[1] = fread_flag (fp);
			pObjIndex->value[2] = fread_number (fp);
			pObjIndex->value[3] = fread_number (fp);
			pObjIndex->value[4] = fread_number (fp);
			break;
		case ITEM_DRINK_CON:
		case ITEM_FOUNTAIN:
			pObjIndex->value[0] = fread_number (fp);
			pObjIndex->value[1] = fread_number (fp);
			pObjIndex->value[2] = liq_lookup (fread_word (fp));
			pObjIndex->value[3] = fread_number (fp);
			pObjIndex->value[4] = fread_number (fp);
			break;
		case ITEM_WAND:
		case ITEM_STAFF:
			pObjIndex->value[0] = fread_number (fp);
			pObjIndex->value[1] = fread_number (fp);
			pObjIndex->value[2] = fread_number (fp);
			pObjIndex->value[3] = skill_lookup (fread_word (fp));
			pObjIndex->value[4] = fread_number (fp);
			break;
		case ITEM_POTION:
		case ITEM_HERB:
			pObjIndex->value[0] = fread_number (fp);
			pObjIndex->value[1] = skill_lookup (fread_word (fp));
			pObjIndex->value[2] = skill_lookup (fread_word (fp));
			pObjIndex->value[3] = skill_lookup (fread_word (fp));
			pObjIndex->value[4] = skill_lookup (fread_word (fp));
			break;
		default:
			pObjIndex->value[0] = fread_flag (fp);
			pObjIndex->value[1] = fread_flag (fp);
			pObjIndex->value[2] = fread_flag (fp);
			pObjIndex->value[3] = fread_flag (fp);
			pObjIndex->value[4] = fread_flag (fp);
			break;
		}
		pObjIndex->level = fread_number (fp);
		pObjIndex->weight = fread_number (fp);
		pObjIndex->cost = fread_number (fp);
		
		/* condition */
		letter = fread_letter (fp);
		
		switch (letter)
		{
			case ('P'):
				pObjIndex->condition = 100;
				break;
			case ('G'):
				pObjIndex->condition = 90;
				break;
			case ('A'):
				pObjIndex->condition = 75;
				break;
			case ('W'):
				pObjIndex->condition = 50;
				break;
			case ('D'):
				pObjIndex->condition = 25;
				break;
			case ('B'):
				pObjIndex->condition = 10;
				break;
			case ('R'):
				pObjIndex->condition = 0;
				break;
			default:
				pObjIndex->condition = 100;
				break;
		}
		
		for (;;)
		{
			char letter;
			
			letter = fread_letter (fp);
			
			if (letter == 'A')
			{
				AFFECT_DATA *paf;
				
				paf = new_affect ();
				paf->where = TO_OBJECT;
				paf->type = -1;
				paf->level = pObjIndex->level;
				paf->location = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->duration = fread_number (fp);
				paf->bitvector = 0;
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
			}
			
			else if (letter == 'F')
			{
				AFFECT_DATA *paf;
				
				paf = new_affect ();
				letter = fread_letter (fp);
				switch (letter)
				{
					case 'A':
					case 'P':
						paf->where = TO_AFFECTS;
						break;
					case 'I':
						paf->where = TO_IMMUNE;
						break;
					case 'R':
						paf->where = TO_RESIST;
						break;
					case 'V':
						paf->where = TO_VULN;
						break;
					case 'W':
						paf->where = TO_WEAPON;
						break;
					case 'M':
						paf->where = TO_ROOM_AFF;
						break;
					default:
						bugf("[areas.c::load_objects] Bad where on flag set.");
						_quit (1);
				}
				
				paf->type = -1;
				paf->level = pObjIndex->level;
				paf->location = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->duration = fread_number (fp);
				paf->bitvector = fread_flag (fp);
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
			}
			
			else if (letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;
				
				ed = new_extra_descr ();
				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = pObjIndex->extra_descr;
				pObjIndex->extra_descr = ed;
			}
			else
				if (letter == 'S')
				{
					fread_word (fp);
					pObjIndex->sounds = load_sound (fp, pObjIndex->sounds);
				}
				
				else
				{
					ungetc (letter, fp);
					break;
				}
		}
		
		iHash = vnum % MAX_KEY_HASH;
		pObjIndex->next = obj_index_hash[iHash];
		obj_index_hash[iHash] = pObjIndex;
		top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;	
		assign_area_vnum (vnum);	
  }
  
  return;
}


/* OLC
 * Snarf an 'area' header line.   
 */
void load_area (FILE * fp)
{
	AREA_DATA *pArea;
	char *word;
	bool fMatch;

	pArea = new_area ();
	pArea->age = 15;
	pArea->file_name = str_dup (strArea);
	pArea->vnum = top_area;
	pArea->name = str_dup ("New Area");
	pArea->security = 9;		/* 9 -- Hugin */
	pArea->area_flags = AREA_LOADING;
	pArea->last_mod = str_dup ("");
	pArea->version = 0;
	
	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;
		
		switch (UPPER (word[0]))
		{
			case 'F': 
				KEY("Flags", pArea->area_flags, fread_flag(fp));
				break;
			case 'L':
				SKEY ("LastSaved", pArea->last_mod);
				break;
			case 'N':
				SKEY ("Name", pArea->name);
				break;
			case 'S':
				KEY ("Security", pArea->security, fread_number (fp));
				
				if (!str_cmp (word, "Sound"))
					pArea->sounds = load_sound (fp, pArea->sounds);
				else
					if (!str_cmp (word, "Sector"))
						pArea->sector_type = fread_number (fp);
					
					break;
					
			case 'V':
				
				if (!str_cmp (word, "VNUMs"))
				{
					pArea->min_vnum = fread_long (fp);
					pArea->max_vnum = fread_long (fp);
				}
				else
					if (!str_cmp (word, "Version"))
					{
						pArea->version = fread_number (fp);
					}
					break;
					
			case 'E':
				if (!str_cmp (word, "End"))
				{
					fMatch = TRUE;
					
					if (area_first == NULL)
						area_first = pArea;
					
					if (area_last != NULL)
						area_last->next = pArea;
					
					area_last = pArea;
					pArea->next = NULL;
					
					current_area = pArea;
					
					REMOVE_BIT (area_last->area_flags, AREA_LOADING);
					return;
				}
				break;
			case 'B':
				SKEY ("Builders", pArea->builders);
				break;
			case 'C':
				SKEY ("Credits", pArea->credits);
				break;
		}
	}
}

/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum (long vnum)
{
	if (area_last->min_vnum == 0 || area_last->max_vnum == 0)
	{
		area_last->min_vnum = area_last->max_vnum = vnum;
		if (vnum != URANGE (area_last->min_vnum, vnum, area_last->max_vnum))
		{
			if (vnum < area_last->min_vnum)
				area_last->min_vnum = vnum;
			else
				area_last->max_vnum = vnum;
		}
	}
	return;
}


/* Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset (ROOM_INDEX_DATA * pR, RESET_DATA * pReset)
{
	RESET_DATA *pr;
	
	if (!pR)
		return;
	
	pr = pR->reset_last;
	
	if (!pr)
	{
		pR->reset_first = pReset;
		pR->reset_last = pReset;
	}
	else
	{
		pR->reset_last->next = pReset;
		pR->reset_last = pReset;
		pR->reset_last->next = NULL;
	}
	
	top_reset++;
	return;
}



/* Snarf a reset section. */
void load_resets (FILE * fp)
{
	RESET_DATA *pReset;
	long iLastRoom = 0;
	long iLastObj = 0;

	if (!area_last)
	{
		bugf ("[areas.c::load_resets] no #AREA seen yet.");
		_quit (1);
	}
	
	for (;;)
	{
		ROOM_INDEX_DATA *pRoomIndex;
		EXIT_DATA *pexit = NULL;
		char letter;
		OBJ_INDEX_DATA *temp_index;
		
		if ((letter = fread_letter (fp)) == 'S')
			break;
		
		if (letter == '*')
		{
			fread_to_eol (fp);
			continue;
		}
		
		pReset = new_reset_data ();
		pReset->command = letter;
		
		pReset->arg1 = fread_number (fp);
		pReset->arg2 = fread_number (fp);
		pReset->arg3 = fread_number (fp);
		pReset->arg4 = fread_number (fp);
		pReset->chance = fread_number (fp);
		
		fread_to_eol (fp);
		
		/*
 	 	 * Validate parameters.
		 * We're calling the index functions for the side effect.
		 */
		switch (letter)
		{
			default:
				bugf ("[areas.c::load_resets] bad command '%c'.", letter);
				break;
				
			case 'M':
				get_mob_index (pReset->arg1);
				if ((pRoomIndex = get_room_index (pReset->arg3)))
				{
					new_reset (pRoomIndex, pReset);
					iLastRoom = pReset->arg3;
				}
				break;
				
			case 'O':
				temp_index = get_obj_index (pReset->arg1);
				temp_index->reset_num++;
				if ((pRoomIndex = get_room_index (pReset->arg3)))
				{
					new_reset (pRoomIndex, pReset);
					iLastObj = pReset->arg3;
				}
				break;
				
			case 'P':
				temp_index = get_obj_index (pReset->arg1);
				temp_index->reset_num++;
				if ((pRoomIndex = get_room_index (iLastObj)))
				{
					new_reset (pRoomIndex, pReset);
				}
				break;
				
			case 'G':
			case 'E':
				temp_index = get_obj_index (pReset->arg1);
				temp_index->reset_num++;
				if ((pRoomIndex = get_room_index (iLastRoom)))
				{
					new_reset (pRoomIndex, pReset);
					iLastObj = iLastRoom;
				}
				break;
				
			case 'D':
				pRoomIndex = get_room_index (pReset->arg1);
				
				if (pReset->arg2 < 0
					|| pReset->arg2 > (MAX_DIR - 1)
					|| !pRoomIndex || !(pexit = pRoomIndex->exit[pReset->arg2]))
				{
					bugf("[areas.c::load_resets] 'D': exit %d not door.", pReset->arg2);
					_quit (1);
				}
				
				pexit->rs_flags = pReset->arg3;
				pexit->exit_info = pReset->arg3;
				
				if (pReset->arg3 == 2)
				{
					SET_BIT (pexit->rs_flags, EX_ISDOOR | EX_CLOSED | EX_LOCKED);
					SET_BIT (pexit->exit_info, EX_ISDOOR | EX_CLOSED | EX_LOCKED);
				}
				
				if (!IS_SET (pexit->rs_flags, EX_ISDOOR))
				{
					bugf("[areas.c::load_resets] 'D': exit %d not a door. (II)", pReset->arg2);
					_quit (1);
				}
				
				break;
				
			case 'R':
				pRoomIndex = get_room_index (pReset->arg1);
				
				if (pReset->arg2 < 0 || pReset->arg2 > MAX_DIR)
				{
					bugf("[areas.c::load_resets] 'R': bad exit %d.", pReset->arg2);
					_quit (1);
				}
				
				if (pRoomIndex)
					new_reset (pRoomIndex, pReset);
				
				break;
		}
		
  }
  
  return;
}

/*  Snarf a room section. */
void load_rooms (FILE * fp)
{
	ROOM_INDEX_DATA *pRoomIndex;

	if (area_last == NULL)
	{
		bugf("[areas.c::load_rooms] no #AREA seen yet.");
		_quit (1);
	}
	
	for (;;)
	{
		long vnum;
		char letter;
		int door;
		int iHash;
		
		letter = fread_letter (fp);
		if (letter != '#')
		{
			bugf("[areas.c::load_rooms] # not found.");
			_quit (1);
		}
		
		vnum = fread_long (fp);
		if (vnum == 0)
			break;
		
		fBootDb = FALSE;
		if (get_room_index (vnum) != NULL)
		{
			bugf("[areas.c::load_rooms] vnum %ld duplicated.", vnum);
			_quit (1);
		}
		fBootDb = TRUE;
		
		pRoomIndex = new_room_index ();
		pRoomIndex->owner = str_dup ("");
		pRoomIndex->people = NULL;
		pRoomIndex->contents = NULL;
		pRoomIndex->extra_descr = NULL;
		pRoomIndex->area = area_last;
		pRoomIndex->vnum = vnum;
		pRoomIndex->name = fread_string (fp);
		pRoomIndex->description = fread_string (fp);
		/* Area number */ fread_number (fp);
		pRoomIndex->room_flags = fread_flag (fp);
		pRoomIndex->sector_type = fread_number (fp);
		pRoomIndex->light = 0;

		for (door = 0; door <= 5; door++)
			pRoomIndex->exit[door] = NULL;
		
		/* defaults */
		pRoomIndex->heal_rate = 100;
		pRoomIndex->opu_rate = 100;
		
		for (;;)
		{
			letter = fread_letter (fp);
			
			if (letter == 'S')	/* End of Room */
				break;
			
			if (letter == 'Z')
			{
				fread_word (fp);
				pRoomIndex->sounds = load_sound (fp, pRoomIndex->sounds);
				break;
			}
			else
				if (letter == 'H')	/* healing room */
					pRoomIndex->heal_rate = fread_number (fp);
				
				else if (letter == 'M')	/* OPU room */
					pRoomIndex->opu_rate = fread_number (fp);
				
				else if (letter == 'C')	/* clan */
					pRoomIndex->clan = clan_lookup (fread_string (fp));
				
				else if (letter == 'L')	/* class */
				{
					pRoomIndex->c_class = class_lookup (fread_string (fp));
				}
				
				
				else if (letter == 'D')
				{
					EXIT_DATA *pexit;
					/* int locks; */
					
					door = fread_number (fp);
					if (door < 0 || door > 5)
					{
						bugf("[areas.c::load_rooms] vnum %ld has bad door number.", vnum);
						_quit (1);
					}
					
					pexit = new_exit ();
					pexit->description = fread_string (fp);
					pexit->keyword = fread_string (fp);
					pexit->exit_info = 0;
					pexit->rs_flags = fread_number (fp);	
					pexit->key = fread_number (fp);
					pexit->u1.vnum = fread_long (fp);
					pexit->orig_door = door;	
					pRoomIndex->exit[door] = pexit;
				}
				else
					if (letter == 'E')
					{
						EXTRA_DESCR_DATA *ed;
						
						ed = new_extra_descr ();
						ed->keyword = fread_string (fp);
						ed->description = fread_string (fp);
						ed->next = pRoomIndex->extra_descr;
						pRoomIndex->extra_descr = ed;
					}
					
					else if (letter == 'O')
					{
						if (pRoomIndex->owner[0] != '\0')
						{
							bugf("[areas.c::load_rooms] duplicate owner.", 0);
							return;		/* exit(1); */
						}
						
						pRoomIndex->owner = fread_string (fp);
					}
					
					else
					{
						bugf("[areas.c::load_rooms] vnum %ld has flag not 'DES'.", vnum);
						_quit (1);
					}
		}
		
		iHash = vnum % MAX_KEY_HASH;
		pRoomIndex->next = room_index_hash[iHash];
		room_index_hash[iHash] = pRoomIndex;
		top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;	
		assign_area_vnum (vnum);	
  }
  
  return;
}



/* Snarf a shop section. */
void load_shops (FILE * fp)
{
	SHOP_DATA *pShop;

	for (;;)
	{
		MOB_INDEX_DATA *pMobIndex;
		int iTrade;
		
		pShop = new_shop ();
		pShop->keeper = fread_number (fp);
		
		if (pShop->keeper == 0)
			break;
		
		for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
			pShop->buy_type[iTrade] = fread_number (fp);
		
		pShop->profit_buy = fread_number (fp);
		pShop->profit_sell = fread_number (fp);
		pShop->open_hour = fread_number (fp);
		pShop->close_hour = fread_number (fp);
		fread_to_eol (fp);
		pMobIndex = get_mob_index (pShop->keeper);
		pMobIndex->pShop = pShop;
		
		if (shop_first == NULL)
			shop_first = pShop;
		if (shop_last != NULL)
			shop_last->next = pShop;
		
		shop_last = pShop;
		pShop->next = NULL;
	}
	
	return;
}


/* Snarf spec proc declarations. */
void load_specials (FILE * fp)
{
	for (;;)
	{
		MOB_INDEX_DATA *pMobIndex;
		char letter;
		
		switch (letter = fread_letter (fp))
		{
		default:
			bugf("[areas.c::load_specials] letter '%c' not *MS.", letter);
			_quit (1);
			
		case 'S':
			return;
			
		case '*':
			break;
			
		case 'M':
			pMobIndex = get_mob_index (fread_number (fp));
			pMobIndex->spec_fun = spec_lookup (fread_word (fp));
			if (pMobIndex->spec_fun == 0)
			{
				bugf("[areas.c::load_specials] 'M': vnum %ld.", pMobIndex->vnum);
				_quit (1);
			}
			break;
		}
		
		fread_to_eol (fp);
	}

}


/* Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits (void)
{
	ROOM_INDEX_DATA *pRoomIndex;
	EXIT_DATA *pexit;
	int iHash;
	int door;
	
	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pRoomIndex = room_index_hash[iHash];
		pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
		{
			bool fexit;
			
			fexit = FALSE;
			for (door = 0; door <= 5; door++)
			{
				if ((pexit = pRoomIndex->exit[door]) != NULL)
				{
					if (pexit->u1.vnum <= 0 || get_room_index (pexit->u1.vnum) == NULL)
						pexit->u1.to_room = NULL;
					else
					{
						fexit = TRUE;
						pexit->u1.to_room = get_room_index (pexit->u1.vnum);
					}
				}
			}
		}
	}
	
	return;
}

/*
 * Load mobprogs section
 */
void load_mobprogs (FILE * fp)
{
	MPROG_CODE *pMprog;

	if (area_last == NULL)
	{
		bugf("[areas.c::load_mobprogs] no #AREA seen yet.");
		_quit (1);
	}
	
	for (;;)
	{
		long vnum;
		char letter;
		
		letter = fread_letter (fp);
		if (letter != '#')
		{
			bugf("[areas.c::load_mobprogs] vnum not found.");
			_quit (1);
		}
		
		vnum = fread_long (fp);
		if (vnum == 0)
			break;
		
		fBootDb = FALSE;
		if (get_mprog_index (vnum) != NULL)
		{
			bugf ("[areas.c::load_mobprogs] vnum %ld duplicated.", vnum);
			_quit (1);
		}
		fBootDb = TRUE;
		
		pMprog = new_mpcode ();
		pMprog->vnum = vnum;
		pMprog->code = fread_string (fp);
		pMprog->author  = fread_string( fp );
		pMprog->title   = fread_string( fp );
		
		if (mprog_list == NULL)
			mprog_list = pMprog;
		else
		{
			pMprog->next = mprog_list;
			mprog_list = pMprog;
		}
		
	}
	return;
}

/*
 *  Translate mobprog vnums pointers to real code
 */
void fix_mobprogs (void)
{
	MOB_INDEX_DATA *pMobIndex;
	MPROG_LIST *list;
	MPROG_CODE *prog;
	int iHash;
	
	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pMobIndex = mob_index_hash[iHash];
		pMobIndex != NULL; pMobIndex = pMobIndex->next)
		{
			for (list = pMobIndex->mprogs; list != NULL; list = list->next)
			{
				if ((prog = get_mprog_index (list->vnum)) != NULL)
					list->code = prog->code;
				else
				{
					bugf("[areas.c::fix_mobprogs] code vnum %ld not found.", list->vnum);
					continue;
				}
			}
		}
	}
}
