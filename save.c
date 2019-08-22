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

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"

/* Array of containers read for proper re-nesting of objects. */
#define MAX_NEST	100
static OBJ_DATA *rgObjNest[MAX_NEST];



SOUND_DATA *load_sound (FILE * fp, SOUND_DATA * sPtr)
{
	SOUND_DATA *pSound;
	char *word;
	
	pSound = new_sound ();
	word = fread_word (fp);
	pSound->fname = str_dup (word);
	pSound->volume = fread_number (fp);
	pSound->repeats = fread_number (fp);
	pSound->priority = fread_number (fp);
	pSound->type = flag_value (sound_type_table, fread_word (fp));
	pSound->trigger = flag_value (SOUND_TABLE (pSound->type), fread_word (fp));
	pSound->music = fread_number (fp);
	pSound->targets = fread_number (fp);
	
	pSound->next = sPtr;
	return pSound;
}

void save_sounds (FILE * fp, SOUND_DATA * sPtr, char *Key)
{
	SOUND_DATA *pSound;
	const struct flag_type *Table = NULL;
	
	for (pSound = sPtr; pSound != NULL; pSound = pSound->next)
	{
		if (!pSound->fname || pSound->fname[0] == '\0')
			continue;
		
		Table = SOUND_TABLE (sPtr->type);
		
		fprintf (fp, "%s %s %d %d %d %s %s %d %d\n",
			Key,
			pSound->fname,
			pSound->volume,
			pSound->repeats,
			pSound->priority,
			sound_type_table[pSound->type].name,
			(Table == NULL) ? "none" : Table[pSound->trigger].name,
			pSound->music, pSound->targets);
	}
	
	return;
}


/**
 * Function: save_char_obj
 * Descr   : Saves a character and it's inventory to disk. 
 *         : Added functionality and checks for saving NPC's. More work
 *         : is needed before that is viable.
 * Returns : (void)
 * Syntax  : save_char_obj ( ch )
 * Written : v1.0 8/98
 * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
**/
void save_char_obj (CHAR_DATA * ch)
{
	char sSave[MIL],
		 sFile[MSL], 
		 sBackup[MSL];

	char *szTemp;
	int error = -1;

	if ( IS_NPC(ch) )
	{
		if ( !IS_FLAG(ch->states.player, _ACT_PERSISTANT) && !IS_FLAG(ch->pIndexData->states.player, _ACT_PERSISTANT) )
		{
			if (ch->desc != NULL && ch->desc->original != NULL)
				ch = ch->desc->original;
		}
	}
	
	/* create god log */
	if (!IS_NPC(ch) && (IS_IMMORTAL (ch) || ch->level >= LEVEL_IMMORTAL))
	{
		sprintf (sSave, "%s%s",
			GOD_DIR, capitalize (strip_codes (fix_name (ch->name, FALSE))));
		
		if ((fpChar = fopen (sSave, "w")) == NULL)
		{
			bugf ("[save.c::save_char_obj] Unable to open file %s for writing", sSave);
			perror (sSave);
		}
		
		fprintf (fpChar, "Lev %2d Trust %2d  %s%s\n",
					ch->level, 
					get_trust (ch), 
					ch->name, 
					ch->pcdata->title);
		fclose (fpChar);
	}
	

	szTemp = str_dup (strip_codes (ch->name));

	
	sprintf (sSave, "%s%c/%s",
		IS_NPC(ch) ? NPC_DIR : PLAYER_DIR,
		UPPER (szTemp[0]),
		capitalize (fix_name (strip_codes (ch->name), FALSE)));
	
	free_string (szTemp);
	
	sprintf (sFile, "%s%s", IS_NPC(ch) ? NPC_DIR : PLAYER_DIR, TEMP_FILE);

	if ((fpChar = fopen (sFile, "w")) == NULL)
	{
		bugf ("[save.c::save_char_obj] Unable to open file %s for writing", sSave);
		return;
	}
	else
	{
		fprintf (fpChar, "#%s\n", IS_NPC (ch) ? "MOB" : "PLAYER");
		fwrite_char (ch, fpChar);
		
		if (ch->carrying != NULL)
			fwrite_obj (ch, ch->carrying, fpChar, 0, FALSE);
		
		if (!IS_NPC(ch) && ch->pcdata->box != NULL)
			fwrite_obj (ch, ch->pcdata->box, fpChar, 0, TRUE);
		
		/* save the pets */
		if (ch->pet != NULL && ch->pet->in_room == ch->in_room &&
			!IS_FLAG (ch->pet->states.player, _ACT_MOUNT))
		{
			fprintf (fpChar, "#PET\n");
			fwrite_char (ch->pet, fpChar);
		}
		
		if (ch->mount)
		{
			fprintf (fpChar, "#MOUNT\n");
			fwrite_char (ch->mount, fpChar);
		}
		fprintf (fpChar, "#END\n");
	}
	
	fclose (fpChar);

	memset(sBackup, 0, MSL);
	sprintf(sBackup, "%s.bak", sSave);

	remove(sBackup);
	rename(sSave, sBackup);
	remove(sSave);

	error = rename (sFile, sSave);
	return;
}



/** 
 * Function: fwrite_char
 * Descr   : Writes character (PC and NPC) information to the pfile.
 *         : Has been updated to include fwrite_pet/mount.
 * Returns : void
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
 **/
void fwrite_char (CHAR_DATA * ch, FILE * fp)
{
	AFFECT_DATA *paf;
	int sn, gn, pos, i;
	
	/* Update Version */
	if (ch->version < 1)
		ch->version = VERSION;
	
	if (IS_NPC (ch))
		fprintf (fp, "Vnum %ld\n", ch->pIndexData->vnum);
	
	fprintf (fp, "Name %s~\n", ch->name);
	fprintf (fp, "Id   %ld\n", IS_NPC(ch) ? ch->pIndexData->vnum : GET_ID(ch));
	fprintf (fp, "LogO %ld\n", current_time);
	fprintf (fp, "Vers %d\n", VERSION);
	
	if (!IS_NULLSTR (ch->short_descr))
		fprintf (fp, "ShD  %s~\n", fix_string (ch->short_descr));
	
	// TaS additions
	if (!IS_NULLSTR (ch->pcdata->shipname))
		fprintf (fp, "Shipname  %s~\n", fix_string (ch->pcdata->shipname));

	fprintf (fp, "Fame  %d\n", ch->pcdata->fame);
	fprintf (fp, "Reputation  %d\n", ch->pcdata->reputation);
	fprintf (fp, "Morale  %d\n", ch->pcdata->morale);
	fprintf (fp, "Background  %d\n", ch->pcdata->background);

	// End


    if (!IS_NULLSTR (ch->pcdata->osdesc))
		fprintf (fp, "SDosc  %s~\n", fix_string (ch->pcdata->osdesc));


	if (!IS_NULLSTR (ch->long_descr))
		fprintf (fp, "LnD  %s~\n", fix_string (ch->long_descr));
	
	if (!IS_NULLSTR (ch->description))
		fprintf (fp, "Desc %s~\n", fix_string (ch->description));

	if (!IS_NPC(ch) && !IS_NULLSTR(ch->pcdata->profile))
		fprintf(fp, "Profile %s~\n", fix_string(ch->pcdata->profile));
	
	if (!IS_NULLSTR (ch->prompt))
		fprintf (fp, "Prom %s~\n", ch->prompt);
	
	//    if ( IS_FLAG(ch->states.config, _PKQUESTOR) )
	//      REMOVE_FLAG(ch->states.config, _PKQUESTOR);

	fprintf (fp, "SET:Config      %s\n", print_bits (ch->states.config));
	fprintf (fp, "SET:Comm        %s\n", print_bits (ch->states.comm));
	fprintf (fp, "SET:Penalties   %s\n", print_bits (ch->states.penalties));
	fprintf (fp, "SET:Player      %s\n", print_bits (ch->states.player));
	fprintf (fp, "SET:Wiznet      %s\n", print_bits (ch->states.wiznet));
	fprintf (fp, "SET:AffectedBy  %s\n", print_bits (ch->states.affected_by));
	fprintf (fp, "SET:Form        %s\n", print_bits (ch->states.form));
	fprintf (fp, "SET:Parts       %s\n", print_bits (ch->states.parts));
	fprintf (fp, "SET:Off         %s\n", print_bits (ch->states.off));
	fprintf (fp, "SET:Imm         %s\n", print_bits (ch->states.imm));
	fprintf (fp, "SET:Res         %s\n", print_bits (ch->states.res));
	fprintf (fp, "SET:Vuln        %s\n", print_bits (ch->states.vuln));
	
	
	if (ch->clan > -1)
		fprintf (fp, "Clan %s~\n", clan_table[ch->clan].name);
	
	if (ch->rank)
		fprintf (fp, "Rank %d\n", ch->rank);
	
	fprintf (fp, "Sex  %d\n", UMAX (0, ch->sex));
	fprintf (fp, "Size %d\n", ch->size);
	fprintf (fp, "Cla  %d\n", ch->c_class);
	fprintf (fp, "Level %d\n", ch->level);
	fprintf (fp, "Age %ld\n", ch->age);
	
	if (ch->trust != 0)
		fprintf (fp, "Tru  %d\n", ch->trust);
	
	fprintf (fp, "Scro %d\n", ch->lines);
	
	fprintf (fp, "Plyd %d\n", ch->played + (int) (current_time - ch->logon));
	
	fprintf (fp, "Room %ld\n",
		(ch->in_room == get_room_index (ROOM_VNUM_LIMBO)
		&& ch->was_in_room != NULL)
		? ch->was_in_room->vnum
		: ch->in_room == NULL ? 3001 : ch->in_room->vnum);
	
	fprintf (fp, "HV  %d %d %d %d\n",
		ch->hit, ch->max_hit, ch->move, ch->max_move);
	
	fprintf (fp, "OPR %d %d %d %d %d %d %d %d %d %d\n",
		ch->flow_str.current[AIR],		ch->flow_str.perm[AIR],
		ch->flow_str.current[EARTH],	ch->flow_str.perm[EARTH],
		ch->flow_str.current[WATER],	ch->flow_str.perm[WATER],
		ch->flow_str.current[FIRE],		ch->flow_str.perm[FIRE],
		ch->flow_str.current[SPIRIT],	ch->flow_str.perm[SPIRIT]);
	
	fprintf (fp, "Gold %ld\n", ch->gold);
	fprintf (fp, "Silv %ld\n", ch->silver);
	
	fprintf (fp, "Exp  %ld\n", ch->exp);
	
	if (ch->invis_level)
		fprintf (fp, "Invi %d\n", ch->invis_level);
	
	if (ch->incog_level)
		fprintf (fp, "Inco %d\n", ch->incog_level);
	
	fprintf (fp, "Pos  %d\n",
		ch->position == POS_FIGHTING ? POS_STANDING : ch->position == POS_STUNNED ? POS_RESTING : ch->position);
	
	if (ch->practice != 0)
		fprintf (fp, "Prac %d\n", ch->practice);

	if (ch->train != 0)
		fprintf (fp, "Trai %d\n", ch->train);
	
	if (ch->saving_throw != 0)
		fprintf (fp, "Save  %d\n", ch->saving_throw);
	
	fprintf (fp, "Alig  %d\n", ch->alignment);
	
	if (ch->hitroll != 0)
		fprintf (fp, "Hit   %d\n", ch->hitroll);
	
	if (ch->damroll != 0)
		fprintf (fp, "Dam   %d\n", ch->damroll);
	
	fprintf (fp, "ACs %d %d %d %d\n",
		ch->armor[0], ch->armor[1], ch->armor[2], ch->armor[3]);
	
	if (ch->wimpy != 0)
		fprintf (fp, "Wimp  %d\n", ch->wimpy);
	
	fprintf (fp, "Attr %d %d %d %d %d %d\n",
		ch->perm_stat[STAT_STR],
		ch->perm_stat[STAT_INT],
		ch->perm_stat[STAT_AGL],
		ch->perm_stat[STAT_DEX],
		ch->perm_stat[STAT_CON],
		ch->perm_stat[STAT_SEN]);
	
	fprintf (fp, "AMod %d %d %d %d %d %d\n",
		ch->mod_stat[STAT_STR],
		ch->mod_stat[STAT_INT],
		ch->mod_stat[STAT_AGL],
		ch->mod_stat[STAT_DEX],
		ch->mod_stat[STAT_CON],
		ch->mod_stat[STAT_SEN]);

	fprintf (fp, "Abi %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
		ch->ability[ABILITY_FASTFEET],
		ch->ability[ABILITY_STEPSOFT], 
		ch->ability[ABILITY_WORDLYWISE], 
		ch->ability[ABILITY_SHARPSENSES], 
		ch->ability[ABILITY_LEADERSHIP],
		ch->ability[ABILITY_MASTERTOUCH], 
		ch->ability[ABILITY_LONGWIND], 
		ch->ability[ABILITY_DEADEYE], 
		ch->ability[ABILITY_IGTERRAIN], 
		ch->ability[ABILITY_IGTEMP], 
		ch->ability[ABILITY_IGPAIN], 
		ch->ability[ABILITY_PREDWEATHER], 
		ch->ability[ABILITY_IGNOREWEATHER], 
		ch->ability[ABILITY_PERPITCH], 
		ch->ability[ABILITY_DRUMPLAYING], 
		ch->ability[ABILITY_FLUTEPLAYING], 
		ch->ability[ABILITY_HORNPLAYING], 
		ch->ability[ABILITY_HARPPLAYING]);

	fprintf (fp, "Imp %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
		ch->impediment[IMPEDIMENT_PRIDE],
		ch->impediment[IMPEDIMENT_GLUTTONY], 
		ch->impediment[IMPEDIMENT_SELFISHNESS], 
		ch->impediment[IMPEDIMENT_VENGEANCE], 
		ch->impediment[IMPEDIMENT_ANTISOCIAL],
		ch->impediment[IMPEDIMENT_SHOWOFF], 
		ch->impediment[IMPEDIMENT_ALCOHOLISM], 
		ch->impediment[IMPEDIMENT_PREJUDICED], 
		ch->impediment[IMPEDIMENT_CRIPPLED], 
		ch->impediment[IMPEDIMENT_LONELINESS], 
		ch->impediment[IMPEDIMENT_PARANOIA], 
		ch->impediment[IMPEDIMENT_SPEECHPROB], 
		ch->impediment[IMPEDIMENT_MUTE], 
		ch->impediment[IMPEDIMENT_DEAF], 
		ch->impediment[IMPEDIMENT_LAZINESS], 
		ch->impediment[IMPEDIMENT_CONTROLFREAK], 
		ch->impediment[IMPEDIMENT_INSOLENCE]);
		
	
	if (ch->mount != NULL && ch->vstable > 0)
		fprintf (fp, "Stable %ld\n", ch->vstable);
	
	if (!IS_NPC (ch))
	{
		if ( ch->pcdata->PracticesGiven > 0 )
			fprintf (fp, "Given %d\n", ch->pcdata->PracticesGiven);

		fprintf (fp, "Pass %s~\n", ch->pcdata->pwd);
		
		fprintf (fp, "Race %s~\n", race_table[ch->race].name);
		if (ch->pcdata->recall_to > -1)
			fprintf (fp, "Recall %ld\n", ch->pcdata->recall_to);
		fprintf (fp, "Country %s~\n", ch->pcdata->nation);
		fprintf (fp, "City %s~\n", ch->pcdata->city);
		
		fprintf (fp, "Box  %d\n", ch->pcdata->boxrented);
		fprintf (fp, "Peace %d\n", ch->pcdata->peaceful);
		fprintf (fp, "Insanity %d\n", ch->pcdata->insanity);
		fprintf (fp, "Sec  %d\n", ch->pcdata->security);
		fprintf (fp, "Bounty %ld\n", ch->pcdata->bounty);
		fprintf (fp, "Infamy %f\n", ch->pcdata->notoriety[INFAMY]);
		
		if (ch->pcdata->bamfin[0] != '\0')
			fprintf (fp, "Bin  %s~\n", ch->pcdata->bamfin);
		
		if (ch->pcdata->bamfout[0] != '\0')
			fprintf (fp, "Bout %s~\n", ch->pcdata->bamfout);
		
		fprintf (fp, "Balance %ld\n", ch->pcdata->balance);
		fprintf (fp, "Shares  %ld\n", ch->pcdata->shares);
		
		fprintf (fp, "Titl %s~\n", ch->pcdata->title);

		if ( IS_FLAG(pMud.config, _ALLOW_RDESC)
		&& ch->pcdata->r_desc[0] != '\0' )
			fprintf (fp, "RDesc %s~\n", ch->pcdata->r_desc);
		
		if (ch->pcdata->hidden_as[0] != '\0')
			fprintf (fp, "Hidden %s~\n", ch->pcdata->hidden_as);
		
		fprintf (fp, "Pnts %d\n", ch->pcdata->points);
		fprintf (fp, "TSex %d\n", ch->pcdata->true_sex);
		fprintf (fp, "LLev %d\n", ch->pcdata->last_level);
		fprintf (fp, "HVP %d %d\n", ch->pcdata->perm_hit, ch->pcdata->perm_move);
		
		fprintf (fp, "Cnd  %d %d %d %d\n",
			ch->pcdata->condition[0],
			ch->pcdata->condition[1],
			ch->pcdata->condition[2], ch->pcdata->condition[3]);
		
		if (ch->pcdata->quest_points > 0)
			fprintf (fp, "QPoints %d\n", ch->pcdata->quest_points);
		
		/* write alias */
		for (pos = 0; pos < MAX_ALIAS; pos++)
		{
			if (ch->pcdata->alias[pos] == NULL
				|| ch->pcdata->alias_sub[pos] == NULL)
				break;
			
			fprintf (fp, "Alias %s %s~\n", ch->pcdata->alias[pos],
				ch->pcdata->alias_sub[pos]);
		}
		
		// write todo
		for (pos = 0; pos < MAX_TODO; pos++)
		{
			if (ch->pcdata->todo[pos] == NULL)
				break;
			
			fprintf (fp, "Todo %s~\n", fix_string (ch->pcdata->todo[pos]));
		}
		
		/* Save note board status */
		/* Save number of boards in case that number changes */
		fprintf (fp, "Boards       %d ", MAX_BOARD);
		for (i = 0; i < MAX_BOARD; i++)
			fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
		fprintf (fp, "\n");

		if ( IS_FLAG(pMud.config, _USE_IGNORE) )
		{
			for (pos = 0; pos < MAX_IGNORE; pos++)
			{
				if (ch->pcdata->ignore[pos] == NULL)
					break;
				
				fprintf (fp, "Ignore %s\n", ch->pcdata->ignore[pos]);
				
			}
		}		
		
		/* Write out any active bonds. Reuse 'pos' variable */
		if (ch->pcdata->bonds[0] > 0)
		{
			int num = 0;
			
			/* determine correct # of bonds before storing value */
			for (pos = 1; pos < ch->pcdata->bonds[0] + 1; pos++)
				if (ch->pcdata->bonds[pos] > 0)
					num++;
				
				if (num > 0)
				{
					fprintf (fp, "Bonds %d\n", num);
					for (pos = 1; pos < ch->pcdata->bonds[0] + 1; pos++)
					{
						if (ch->pcdata->bonds[pos] > 0)
							fprintf (fp, "%d%s",
							ch->pcdata->bonds[pos],
							((pos + 1) % 4 == 0) ? "\n" : "\t");
					}
					fprintf (fp, "\n#0\n");
				}
		}
		
		for (sn = 0; sn < ch->pcdata->iMemSize[MEM_MAXSKILL]; sn++)
		{
			if (skill_table[sn].name != NULL && GET_SKILL (ch, sn) > 0)
			{
				fprintf (fp, "Sk %d %s~\n", GET_SKILL (ch, sn), skill_table[sn].name);
			}
		}
		
		for (sn = 0; sn < MAX_TALENTS; sn++)
		{
			if (talent_table[sn].name != NULL && ch->pcdata->talents[sn] > 0)
			{
				fprintf (fp, "Tn %d %s~\n", ch->pcdata->talents[sn], talent_table[sn].name);
			}
		}
		
		for (gn = 0; gn < ch->pcdata->iMemSize[MEM_MAXGROUP]; gn++)
		{
			if (knows_group(ch, gn))
			{
				fprintf (fp, "Gr %s~\n", group_table[gn].name);
			}
		}
  }				/* END OF PCDATA */
  
  for (paf = ch->affected; paf != NULL; paf = paf->next)
  {
	  if (!IS_VALID_SKILL(paf->type) || paf->owner == 0)
		  continue;
	  
	  fprintf (fp, "Affc %s~ %3d %3d %3d %3d %3d %10ld %ld\n",
		  skill_table[paf->type].name,
		  paf->where,
		  paf->level,
		  paf->duration,
		  paf->modifier, 
		  paf->location, 
		  paf->bitvector, 
		  paf->owner);
  }
  
  if (ch->sounds)
	  save_sounds (fp, ch->sounds, "Sound");
  
  // Write out 'known people' array. Reuse 'pos' variable.
  // moved from PCDATA section so that we could place the known
  // array at the bottom of the pfile for easier reading.
  if (ch->pcdata && ch->pcdata->known[0] > 0)
  {
      fprintf (fp, "Known %d\n", ch->pcdata->known[0]);
      for (pos = 1; pos < ch->pcdata->known[0] + 1; pos++)
      {
		  fprintf (fp, "%d%s",
			  ch->pcdata->known[pos], ((pos + 1) % 4 == 0) ? "\n" : "\t");
      }
      fprintf (fp, "\n#0\n");
  }
  
  fprintf (fp, "End\n\n");
  
  return;
}


/**
 * Function: fwrite_obj
 * Descr   : Writes an object, and any contents it may have, to disk. 
 *         : Updated to allow for 'box' (storage) objects.
 * Returns : void
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
**/
void fwrite_obj (CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest,
				 bool is_box)
{
	EXTRA_DESCR_DATA *ed;
	AFFECT_DATA *paf;
	

	/*
	 * Slick recursion to write lists backwards,
	 * so loading them will load in forwards order.
	 */
	if (obj->next_content != NULL)
	{
		if (is_box)
			fwrite_obj (ch, obj->next_content, fp, iNest, TRUE);
		else
			fwrite_obj (ch, obj->next_content, fp, iNest, FALSE);
	}
	
	// Dont save these types of objects...
	if (IS_OBJ_STAT(obj, ITEM_NOSAVE) || IS_OBJ_STAT(obj, ITEM_GHOST))
		return;
	
	if (is_box)
		fprintf (fp, "#B\n");
	else
		fprintf (fp, "#O\n");
	
	fprintf (fp, "Vnum %d\n", obj->pIndexData->vnum);
	
	if (obj->enchanted)
		fprintf (fp, "Enchanted\n");
	
	fprintf (fp, "Nest %d\n", iNest);
	
	/* these data are only used if they do not match the defaults */
	
	if (obj->name != obj->pIndexData->name)
		fprintf (fp, "Name %s~\n", obj->name);
	if (obj->short_descr != obj->pIndexData->short_descr)
		fprintf (fp, "ShD  %s~\n", fix_string (obj->short_descr));
	if (obj->description != obj->pIndexData->description)
		fprintf (fp, "Desc %s~\n", fix_string (obj->description));
	if (obj->extra_flags != obj->pIndexData->extra_flags)
		fprintf (fp, "ExtF %ld\n", obj->extra_flags);
	if (obj->wear_flags != obj->pIndexData->wear_flags)
		fprintf (fp, "WeaF %ld\n", obj->wear_flags);
	if (obj->item_type != obj->pIndexData->item_type)
		fprintf (fp, "Ityp %d\n", obj->item_type);
	if (obj->weight != obj->pIndexData->weight)
		fprintf (fp, "Wt   %d\n", obj->weight);
	if (obj->condition != obj->pIndexData->condition)
		fprintf (fp, "Cond %d\n", obj->condition);
	if (obj->max_condition != obj->pIndexData->max_condition)
		fprintf (fp, "MaxCond %d\n", obj->max_condition);
	
	/* variable data */
	fprintf (fp, "Wear %d\n", obj->wear_loc);
	
	if (obj->level != obj->pIndexData->level)
		fprintf (fp, "Lev  %d\n", obj->level);
	
	if (obj->timer != 0)
		fprintf (fp, "Time %d\n", obj->timer);
	
	fprintf (fp, "Cost %d\n", obj->cost);
	
	if (obj->value[0] != obj->pIndexData->value[0]
		|| obj->value[1] != obj->pIndexData->value[1]
		|| obj->value[2] != obj->pIndexData->value[2]
		|| obj->value[3] != obj->pIndexData->value[3]
		|| obj->value[4] != obj->pIndexData->value[4])
		fprintf (fp, "Val  %d %d %d %d %d\n",
		obj->value[0], obj->value[1], obj->value[2], obj->value[3],
		obj->value[4]);
	
	switch (obj->item_type)
	{
	case ITEM_HCLOAK:
		if (obj->hood != 0)
		{
			fprintf (fp, "Hood %d\n", obj->hood);
		}
		break;
	case ITEM_POTION:
	case ITEM_HERB:
		if (obj->value[1] > 0)
		{
			fprintf (fp, "Spell 1 %s~\n", skill_table[obj->value[1]].name);
		}
		
		if (obj->value[2] > 0)
		{
			fprintf (fp, "Spell 2 %s~\n", skill_table[obj->value[2]].name);
		}
		
		if (obj->value[3] > 0)
		{
			fprintf (fp, "Spell 3 %s~\n", skill_table[obj->value[3]].name);
		}
		
		break;
		
	case ITEM_STAFF:
	case ITEM_WAND:
		if (obj->value[3] > 0)
		{
			fprintf (fp, "Spell 3 %s~\n", skill_table[obj->value[3]].name);
		}
		
		break;
	default:
		break;
	}
	
	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (!IS_VALID_SKILL(paf->type)&& paf->type != -1)
			continue;
		
		if (paf->type != -1)
			fprintf( fp, "Affc %s~ %3d %3d %3d %3d %3d %10ld %ld\n",
			skill_table[paf->type].name,
			paf->where,
			paf->level,
			paf->duration,
			paf->modifier, 
			paf->location, 
			paf->bitvector, 
			paf->owner);
		else
			fprintf( fp, "Affc %s~ %3d %3d %3d %3d %3d %10ld %ld\n",
			"qgen",
			paf->where,
			paf->level,
			paf->duration,
			paf->modifier, 
			paf->location, 
			paf->bitvector, 
			paf->owner);
	}
	
	for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
	{
		fprintf (fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
	}
	
	if (obj->sounds)
		save_sounds (fp, obj->sounds, "Sound");
	
	fprintf (fp, "End\n\n");
	
	if (obj->contains != NULL)
	{
		if (is_box)
			fwrite_obj (ch, obj->contains, fp, iNest + 1, TRUE);
		else
			fwrite_obj (ch, obj->contains, fp, iNest + 1, FALSE);
	}
	
	return;
}


/**
 * Function: load_npc
 * Descr   : Loads from file, a character and his inventory into a new
 *         : char_data structure.
 * Returns : TRUE/FALSE on success
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
**/
int load_npc(CHAR_DATA *mob)
{
	char strsave[MIL];
	char buf[100];
	static FILE *fp;
	bool found;
	char *temp = str_dup (strip_codes (mob->name));
	
	/* NOTE: 'fp' was made static to avoid a compiler warning with longjmp.
	* The warning given can never happen with the way were using it here, 
	* but gcc dosent know that, and I know of no other way to avoid the 
	* warning.  I hate cheap hacks like this, but until I figure it out...
	*/
	
	found = FALSE;
	
	/* decompress if .gz file exists */
	sprintf (strsave, "%s%c/%s%s",
		NPC_DIR,
		UPPER (temp[0]),
		capitalize (fix_name (temp, FALSE)), ".gz");
	
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		fclose (fp);
		sprintf (buf, "gzip -dfq %s", strsave);
		system (buf);
	}
	
	sprintf (strsave, "%s%c/%s",
		NPC_DIR,
		UPPER (temp[0]),
		capitalize (fix_name (temp, FALSE)));
	
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		int iNest;
		
		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;
		
		found = TRUE;
		
		/* Set environment buffer for restore after a critical error while
		 * reading a pfile.  Note: if the pfile is bad, we need to set this
		 * name as banned (in the names list) with the proper reason.
		 */
		
		AttemptJump = TRUE;
		
		if (setjmp (jump_env) == 1)
		{
			char buf[MIL];
			int iLine;
			int iChar;
			
			iChar = ftell (fp);
			fseek (fp, 0, 0);
			
			for (iLine = 0; ftell (fp) < iChar; iLine++)
			{
				while (getc (fp) != '\n')
					;
			}
			fseek (fp, iChar, 0);
			
			sprintf (buf, "%s has a bad NPC file!", strip_codes (temp));
			wiznet (buf, NULL, NULL, WIZ_BUGS, 0, 0);
		
			fclose (fp);
			return READ_ERROR;
		}
		
		
		for (;;)
		{
			char letter;
			char *word;
			
			letter = fread_letter (fp);
			
			if (letter == '*')
			{
				fread_to_eol (fp);
				continue;
			}
			
			if (letter != '#')
			{
				bugf ("[save.c::load_npc] \'#\' not found.");
				break;
			}
			
			word = fread_word (fp);
			
			if (!str_cmp (word, "PLAYER"))
				fread_char (mob, fp, FALSE);
			else if (!str_cmp (word, "NPC"))
				fread_char(mob, fp, TRUE);
			else if (!str_cmp (word, "MOB"))
				fread_char(mob, fp, TRUE);
			else if (!str_cmp (word, "O"))
				fread_obj (mob, fp, FALSE);
			else if (!str_cmp (word, "B"))
				fread_obj (mob, fp, TRUE);
			else if (!str_cmp (word, "PET"))
				fread_pet (mob, fp);
			else if (!str_cmp (word, "MOUNT"))
				fread_pet (mob, fp);
			else if (!str_cmp (word, "END"))
				break;
			else
			{
				bugf ("[save.c::load_npc] bad section.");
				break;
			}
		}
		fclose (fp);
	}
	else
		return ERROR_UNDEFINED;
	
	AttemptJump = FALSE;
	
	if (found)
		return READ_OK;
	else
		return READ_POOR;
}



/**
 * Function: load_char_obj
 * Descr   : Loads from file, a character and his inventory into a new
 *         : char_data structure.
 * Returns : TRUE/FALSE on success
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
**/
int load_char_obj (DESCRIPTOR_DATA * d, char *name, bool bIsNPC)
{
	char strsave[MIL];
	char buf[100];
	CHAR_DATA *ch;
	static FILE *fp;
	bool found;
	char *temp = str_dup (strip_codes (name));
	
	/* NOTE: 'fp' was made static to avoid a compiler warning with longjmp.
	* The warning given can never happen with the way were using it here, 
	* but gcc dosent know that, and I know of no other way to avoid the 
	* warning.  I hate cheap hacks like this, but until I figure it out...
	*/
	
	ch = new_char ();

	if ( !bIsNPC )
	{
	  ch->pcdata = new_pcdata ();
	  d->character = ch;
	  ch->desc = d;
	  ch->pcdata->id = get_pc_id ();
	}

	ch->name = str_dup (name);
	
	SET_FLAG (ch->states.config, _COMBINE);
	SET_FLAG (ch->states.config, _PROMPT);
	SET_FLAG (ch->states.comm, _OOC);
	SET_FLAG (ch->states.comm, _AUCTION);
	SET_FLAG (ch->states.comm, _GOSSIP);
	
	found = FALSE;
	
	/* decompress if .gz file exists */
	sprintf (strsave, "%s%c/%s%s",
		bIsNPC ? NPC_DIR : PLAYER_DIR,
		UPPER (temp[0]),
		capitalize (fix_name (strip_codes (name), FALSE)), ".gz");
	
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		fclose (fp);
		sprintf (buf, "gzip -dfq %s", strsave);
		system (buf);
	}
	
	sprintf (strsave, "%s%c/%s",
		bIsNPC ? NPC_DIR : PLAYER_DIR,
		UPPER (temp[0]),
		capitalize (fix_name (strip_codes (name), FALSE)));
	
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		int iNest;
		
		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;
		
		found = TRUE;
		
		/* Set environment buffer for restore after a critical error while
		 * reading a pfile.  Note: if the pfile is bad, we need to set this
		 * name as banned (in the names list) with the proper reason.
		 */
		
		AttemptJump = TRUE;
		
		if (setjmp (jump_env) == 1)
		{
			char buf[MIL];
			int iLine;
			int iChar;
			
			iChar = ftell (fp);
			fseek (fp, 0, 0);
			
			for (iLine = 0; ftell (fp) < iChar; iLine++)
			{
				while (getc (fp) != '\n')
					;
			}
			fseek (fp, iChar, 0);
			
			sprintf (buf, "[*LCO*] pFile: '%s' Line %d",
				capitalize (strip_codes (name)), iLine);
			wiznet (buf, NULL, NULL, WIZ_BUGS, 0, 0);
			sprintf (buf, "%s has a bad pfile!", strip_codes (name));
			ban_name (NULL, buf);
			
			fclose (fp);
			return READ_ERROR;
		}
		
		
		for (;;)
		{
			char letter;
			char *word;
			
			letter = fread_letter (fp);
			
			if (letter == '*')
			{
				fread_to_eol (fp);
				continue;
			}
			
			if (letter != '#')
			{
				bugf ("[save.c::load_char_obj] \'#\' not found.");
				break;
			}
			
			word = fread_word (fp);
			
			if (!str_cmp (word, "PLAYER"))
				fread_char (ch, fp, FALSE);
			else if (!str_cmp (word, "NPC"))
				fread_char(ch, fp, TRUE);
			else if (!str_cmp (word, "O"))
				fread_obj (ch, fp, FALSE);
			else if (!str_cmp (word, "B"))
				fread_obj (ch, fp, TRUE);
			else if (!str_cmp (word, "PET"))
				fread_pet (ch, fp);
			else if (!str_cmp (word, "MOUNT"))
				fread_pet (ch, fp);
			else if (!str_cmp (word, "END"))
				break;
			else
			{
				bugf ("[save.c::load_char_obj] bad section.");
				break;
			}
		}
		fclose (fp);
	}
	else
		return ERROR_UNDEFINED;
	
	AttemptJump = FALSE;
	
	/* initialize race */
	if (found)
	{
		int i;
		
		if (ch->race == 0)
			ch->race = race_lookup ("human");
		
		if ( !bIsNPC )
  		  ch->size = pc_race_table[ch->race].size;
		
		ch->dam_type = 17;		/*punch */
		
		if ( !bIsNPC )
		{
			for (i = 0; i < 5; i++)
			{
				if (pc_race_table[ch->race].skills[i] == NULL)
					break;
				group_add (ch, pc_race_table[ch->race].skills[i], FALSE);
			}
		}
		add_sets (ch->states.affected_by, race_table[ch->race].aff);
		add_sets (ch->states.imm, race_table[ch->race].imm);
		add_sets (ch->states.res, race_table[ch->race].res);
		add_sets (ch->states.vuln, race_table[ch->race].vuln);
		add_sets (ch->states.form, race_table[ch->race].form);
		add_sets (ch->states.parts, race_table[ch->race].parts);
		add_sets (ch->states.off, race_table[ch->race].off);
		
	}
	
	if (found)
		return READ_OK;
	else
		return READ_POOR;
}

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )		\
				{										\
				field  = value;							\
				fMatch = TRUE;							\
				break;									\
}

#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
				if ( !str_cmp( word, literal ) )		\
				{										\
				free_string(field);						\
				field  = value;							\
				fMatch = TRUE;							\
				break;									\
}


/**
 * Function: fread_char
 * Descr   : Reads PC data from disk.
 * Returns : void
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, Gary McNickle <gary@tarmongaidon.org>
**/
void fread_char (CHAR_DATA * ch, FILE * fp, bool bIsNPC)
{
	char buf[MSL];
	char *word;
	bool fMatch;
	int count = 0, count2 = 0, pos = 0;
	int lastlogoff = current_time;
	int percent;
	
	logf("[save.c::fread_char<%s>] Loading as %s.", ch->name, bIsNPC ? "an NPC" : "a Player");

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;
		
		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;
			
		case 'A':
			KEY ("Age", ch->age, fread_number (fp));
			KEY ("Alig", ch->alignment, fread_number (fp));

			if (!str_cmp (word, "Abi"))
			{
				int ab;
				
				for (ab = 0; ab < MAX_ABILITY; ab++)
					ch->ability[ab] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Alias") && !bIsNPC) 
			{
				if (count >= MAX_ALIAS)
				{
					fread_to_eol (fp);
					fMatch = TRUE;
					break;
				}
				
				ch->pcdata->alias[count] = str_dup (fread_word (fp));
				ch->pcdata->alias_sub[count] = fread_string (fp);
				count++;
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "AC") || !str_cmp (word, "Armor"))
			{
				fread_to_eol (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "ACs"))
			{
				int i;
				
				for (i = 0; i < 4; i++)
					ch->armor[i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Affc"))
			{
				AFFECT_DATA *paf;
				int sn;
				char *temp;
				
				paf = new_affect ();
				
				temp = fread_string (fp);
				sn = skill_lookup (temp);
				
				if (!IS_VALID_SKILL(sn))
				{
					bugf ("[save.c::fread_char<%s>] unknown skill [%s].", ch->name, temp);
				}
				else
					paf->type = sn;
				
				if ( paf->type == gsn_sneak || paf->type == gsn_hide )
				{
					break;
				}

				paf->where = fread_number (fp);
				paf->level = fread_number (fp);
				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = fread_number (fp);
				paf->owner = fread_number (fp);

				if (paf->owner > 0)
				{
					/* owner 0 == system, and this affect has already
					 * been added by create_mobile 
					 */
					paf->next = ch->affected;
					ch->affected = paf;
				}
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "AttrMod") || !str_cmp (word, "AMod"))
			{
				int stat;
				for (stat = 0; stat < MAX_STATS; stat++)
					ch->mod_stat[stat] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "AttrPerm") || !str_cmp (word, "Attr"))
			{
				int stat;
				
				for (stat = 0; stat < MAX_STATS; stat++)
					ch->perm_stat[stat] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'B': // no NPC data here....
			{
				if ( bIsNPC )
					break;
				KEY ("Bamfin", ch->pcdata->bamfin, fread_string (fp));
				KEY ("Bamfout", ch->pcdata->bamfout, fread_string (fp));
				KEY ("Bin", ch->pcdata->bamfin, fread_string (fp));
				KEY ("Bout", ch->pcdata->bamfout, fread_string (fp));
				KEY ("Box", ch->pcdata->boxrented, fread_number (fp));
				KEY ("Balance", ch->pcdata->balance, fread_number (fp));
				KEY ("Bounty", ch->pcdata->bounty, fread_number (fp));
				
				/* Read in a players bonds */
				if (!str_cmp (word, "Bonds"))
				{
					int i;
					int size;
					int *memory;
					
					size = fread_number (fp);
					memory = alloc_mem (sizeof (int) * size);
					
					memory[0] = size;
					for (i = 1; i < size + 1; i++)
						memory[i] = fread_number (fp);
					
					fread_word (fp);
					free_mem (ch->pcdata->bonds, sizeof (int) * ch->pcdata->bonds[0]);
					ch->pcdata->bonds = memory;
					fMatch = TRUE;
					break;
				}
				else
					/* Read in board status */
					if (!str_cmp (word, "Boards"))
					{
						int i, num = fread_number (fp);	/* number of boards saved */
						char *boardname;
						
						for (; num; num--)	/* for each of the board saved */
						{
							boardname = fread_word (fp);
							i = board_lookup (boardname);	/* find board number */
							
							if (i == -1)		/* Does board still exist ? */
							{
								logf("[save.c::fread_char<%s>] Skipped unknown board (%s).", ch->name, boardname);
								fread_number (fp);	/* read last_note and skip info */
							}
							else			/* Save it */
								ch->pcdata->last_note[i] = fread_number (fp);
						}			/* for */
						
						fMatch = TRUE;
						break;
					}				/* Boards */
			}
			break;
				
		case 'C':
			{
				KEY ("Class", ch->c_class, fread_number (fp));
				KEY ("Cla", ch->c_class, fread_number (fp));
				KEY ("Clan", ch->clan, clan_lookup (fread_string (fp)));
				
				if ( !bIsNPC )
				{
					KEY ("Country", ch->pcdata->nation, fread_string (fp));
					KEY ("City", ch->pcdata->city, fread_string (fp));

					if (!str_cmp (word, "Condition") || !str_cmp (word, "Cond"))
					{
						ch->pcdata->condition[0] = fread_number (fp);
						ch->pcdata->condition[1] = fread_number (fp);
						ch->pcdata->condition[2] = fread_number (fp);
						fMatch = TRUE;
						break;
					}
					if (!str_cmp (word, "Cnd"))
					{
						ch->pcdata->condition[0] = fread_number (fp);
						ch->pcdata->condition[1] = fread_number (fp);
						ch->pcdata->condition[2] = fread_number (fp);
						ch->pcdata->condition[3] = fread_number (fp);
						fMatch = TRUE;
						break;
					}
				}
			}			
			break;
			
		case 'D':
			KEY ("Dam", ch->damroll, fread_number (fp));
			KEY ("Desc", ch->description, fread_string (fp));
			break;
			
		case 'E':
			if (!str_cmp (word, "End"))
			{
				/* adjust hp opr move up  -- here for speed's sake */
				percent = (current_time - lastlogoff) * 25 / 7200;
				
				percent = UMIN (percent, 100);
				
				if (percent > 0 && !IS_AFFECTED (ch, _AFF_POISON)
					&& !IS_AFFECTED (ch, _AFF_PLAGUE))
				{
					ch->hit += (ch->max_hit - ch->hit) * percent / 100;
					ch->move += (ch->max_move - ch->move) * percent / 100;
				}
				return;
			}
			KEY ("Exp", ch->exp, fread_number (fp));
			break;
			
		case 'F':
			if ( !bIsNPC )
			  KEY ("Fame", ch->pcdata->fame, fread_number(fp));

			break;
			
		case 'G':
			KEY ("Given", ch->pcdata->PracticesGiven, fread_number(fp));
			KEY ("Gold", ch->gold, fread_number (fp));
			if ((!str_cmp (word, "Group") || !str_cmp (word, "Gr")) && !bIsNPC)
			{
				int gn;
				char *temp;
				
				temp = fread_string (fp);
				gn = group_lookup (temp);
				
				if (gn < 0)
				{
					bugf ("[save.c::fread_char<%s>] unknown group [%s]. ", ch->name, temp);
				}
				else
					gn_add (ch, gn);
				
				fMatch = TRUE;
			}
			break;
			
		case 'H':
			{
				KEY ("Hitroll", ch->hitroll, fread_number (fp));
				KEY ("Hit", ch->hitroll, fread_number (fp));

				if ( !bIsNPC )
				{
					KEY ("Hidden", ch->pcdata->hidden_as, fread_string (fp));

					if (!str_cmp (word, "HVP"))
					{
						ch->pcdata->perm_hit = fread_number (fp);
						ch->pcdata->perm_move = fread_number (fp);
						fMatch = TRUE;
						break;
					}
				}
				
				if (!str_cmp (word, "HV"))
				{
					ch->hit = fread_number (fp);
					ch->max_hit = fread_number (fp);
					ch->move = fread_number (fp);
					ch->max_move = fread_number (fp);
					fMatch = TRUE;
					break;
				}
			}
			break;
			
		case 'I':
			if ( !str_cmp(word, "Id") )
			{
				if ( bIsNPC )
					ch->pIndexData->vnum = fread_number(fp);
				else
					ch->pcdata->id = fread_number(fp);

				fMatch = TRUE;
				fread_to_eol(fp);
			}
			else
			if ( !bIsNPC ) // no NPC data here...
			{
				if (!str_cmp (word, "Imp"))
			{
				int im;
				
				
				for (im = 0; im < MAX_IMPEDIMENT; im++)
					ch->impediment[im] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
				KEY ("Infamy", ch->pcdata->notoriety[INFAMY], fread_float(fp));
				KEY ("InvisLevel", ch->invis_level, fread_number (fp));
				KEY ("Inco", ch->incog_level, fread_number (fp));
				KEY ("Insanity", ch->pcdata->insanity, fread_number (fp));
				KEY ("Invi", ch->invis_level, fread_number (fp));
				
				if (!str_cmp (word, "Ignore"))
				{
					if (count2 >= MAX_IGNORE)
					{
						fread_to_eol (fp);
						fMatch = TRUE;
						break;
					}
					
					ch->pcdata->ignore[count2++] = str_dup (fread_word (fp));
					fMatch = TRUE;
					break;
				}
			}
			break;
			
		case 'K':  // No NPC data here...
			if (!str_cmp (word, "Known") && !bIsNPC)
			{
				int i;
				int size;
				int *memory;
				
				size = fread_number (fp);
				memory = alloc_mem (sizeof (int) * (size + 1));
				
				memory[0] = size;
				for (i = 1; i < size + 1; i++)
					memory[i] = fread_number (fp);
				
				fread_word (fp);	/* read final #0 */
				free_mem (ch->pcdata->known, sizeof (int) * ch->pcdata->known[0]);
				ch->pcdata->known = memory;
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'L':
			{
				if ( !bIsNPC )
				{
					KEY ("LastLevel", ch->pcdata->last_level, fread_number (fp));
					KEY ("LLev", ch->pcdata->last_level, fread_number (fp));
				}

				KEY ("Level", ch->level, fread_number (fp));
				KEY ("Lev", ch->level, fread_number (fp));
				KEY ("Levl", ch->level, fread_number (fp));
				KEY ("LogO", lastlogoff, fread_number (fp));
				KEY ("LongDescr", ch->long_descr, fread_string (fp));
				KEY ("LnD", ch->long_descr, fread_string (fp));
			}
			break;

		case 'M':
			KEYS ("Morale", ch->pcdata->morale, fread_number (fp));
			break;
			
		case 'N':
			KEYS ("Name", ch->name, fread_string (fp));
			break;
			
		case 'O':
			if (!str_cmp (word, "OPR"))
			{
				ch->flow_str.current[AIR] = fread_number (fp);
				ch->flow_str.perm[AIR] = fread_number (fp);
				ch->flow_str.current[EARTH] = fread_number (fp);
				ch->flow_str.perm[EARTH] = fread_number (fp);
				ch->flow_str.current[WATER] = fread_number (fp);
				ch->flow_str.perm[WATER] = fread_number (fp);
				ch->flow_str.current[FIRE] = fread_number (fp);
				ch->flow_str.perm[FIRE] = fread_number (fp);
				ch->flow_str.current[SPIRIT] = fread_number (fp);
				ch->flow_str.perm[SPIRIT] = fread_number (fp);
				fMatch = TRUE;
			}
			break;
			
		case 'P':
			{
				if ( !bIsNPC )
				{
					KEY ("Password", ch->pcdata->pwd, fread_string (fp));
					KEY ("Pass", ch->pcdata->pwd, fread_string (fp));
					KEY ("Peace", ch->pcdata->peaceful, fread_number (fp));
					KEY ("Points", ch->pcdata->points, fread_number (fp));
					KEY ("Pnts", ch->pcdata->points, fread_number (fp));
					KEY ("Profile", ch->pcdata->profile, fread_string (fp));
				}
				KEY ("Played", ch->played, fread_number (fp));
				KEY ("Plyd", ch->played, fread_number (fp));
				KEY ("Position", ch->position, fread_number (fp));
				KEY ("Pos", ch->position, fread_number (fp));
				KEY ("Practice", ch->practice, fread_number (fp));
				KEY ("Prac", ch->practice, fread_number (fp));
				KEY ("Prompt", ch->prompt, fread_string (fp));
				KEY ("Prom", ch->prompt, fread_string (fp));
			}
			break;
			
		case 'Q':
			{
				if ( !bIsNPC )
				  KEY ("QPoints", ch->pcdata->quest_points, fread_number (fp));
			}
			break;
			
		case 'R':
			{
				if ( !bIsNPC )
				{
					KEY ("RDesc", ch->pcdata->r_desc, fread_string (fp));
					KEY ("Recall", ch->pcdata->recall_to, fread_number (fp));
				}
				KEY ("Race", ch->race, race_lookup (fread_string (fp)));
				KEY ("Rank", ch->rank, fread_number (fp));
				// Tos
				KEY ("Reputation", ch->pcdata->reputation, fread_number (fp));
				if (!str_cmp (word, "Room"))
				{
					ch->in_room = get_room_index (fread_number (fp));
					if (ch->in_room == NULL)
						ch->in_room = get_room_index (ROOM_VNUM_LIMBO);
					fMatch = TRUE;
					break;
				}
			}			
			break;
			
		case 'S':
			{
				if ( !bIsNPC )
				{
					KEY ("SDosc", ch->pcdata->osdesc, fread_string (fp));
					KEY ("Sec", ch->pcdata->security, fread_number (fp));	/* OLC */
					KEY ("Shares", ch->pcdata->shares, fread_number (fp));
					// Tos
					KEY ("Shipname", ch->pcdata->security, fread_string (fp));
					// end
					KEY ("Size", ch->size, fread_number (fp));
					

					if ( !str_cmp (word, "Sk") )
					{
						int sn;
						int value;
						char *temp;
						
						value = fread_number (fp);
						
						temp = fread_string (fp);
						sn = skill_lookup (temp);
						
						if (sn < 0)
						{
							bugf ("[save.c::fread_char<%s>] Unknown skill [%s]. ", ch->name, temp);
						}
						
						else
							SET_SKILL (ch, sn, value);
						
			
						fMatch = TRUE;
					}
				}

				KEY ("SavingThrow", ch->saving_throw, fread_number (fp));
				KEY ("Save", ch->saving_throw, fread_number (fp));
				KEY ("Scro", ch->lines, fread_number (fp));
				KEY ("Sex", ch->sex, fread_number (fp));
				KEY ("ShortDescr", ch->short_descr, fread_string (fp));
				KEY ("ShD", ch->short_descr, fread_string (fp));
				KEY ("Silv", ch->silver, fread_number (fp));
				KEY ("Stable", ch->vstable, fread_number (fp));
				
				if (!str_cmp (word, "SET:Config"))
				{
					fMatch = TRUE;
					fread_bits (fp, ch->states.config);
					break;
				}
				else if (!str_cmp (word, "SET:Comm"))
				{
					fMatch = TRUE;
					fread_bits (fp, ch->states.comm);
					break;
				}
				else if (!str_cmp (word, "SET:Penalties"))
				{
					fMatch = TRUE;
					fread_bits (fp, ch->states.penalties);
					break;
				}
				else if (!str_cmp (word, "SET:Player"))
				{
					fMatch = TRUE;
					fread_bits (fp, ch->states.player);
					break;
				}
				else if (!str_cmp (word, "SET:Wiznet"))
				{
					fread_bits (fp, ch->states.wiznet);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:AffectedBy"))
				{
					/*
					 * fread_bits (fp, ch->states.affected_by);
					 *
					 * Only the bits for Dreaming, Warfare, & Martyrdom
					 * are set without adding a corresponding affect. We dont
					 * really want these 3 to be persistant (if we do, we should
					 * an affect for them).  So, their is NO NEED to read this value
					 * back from the pfile. 
					 */
					fread_to_eol(fp);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Form"))
				{
					fread_bits (fp, ch->states.form);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Parts"))
				{
					fread_bits (fp, ch->states.parts);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Off"))
				{
					fread_bits (fp, ch->states.off);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Imm"))
				{
					fread_bits (fp, ch->states.imm);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Res"))
				{
					fread_bits (fp, ch->states.res);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "SET:Vuln"))
				{
					fread_bits (fp, ch->states.vuln);
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "Sound"))
				{
					ch->sounds = load_sound (fp, ch->sounds);
					fMatch = TRUE;
					break;
				}
				
			}		
			break;
		
    case 'T':
		{
			if ( !bIsNPC )
			{
				KEY ("TrueSex", ch->pcdata->true_sex, fread_number (fp));
				KEY ("TSex", ch->pcdata->true_sex, fread_number (fp));

				if (!str_cmp (word, "Title") || !str_cmp (word, "Titl"))
				{
					ch->pcdata->title = fread_string (fp);
					if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ','
						&& ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
					{
						sprintf (buf, " %s", ch->pcdata->title);
						free_string (ch->pcdata->title);
						ch->pcdata->title = str_dup (buf);
					}
					fMatch = TRUE;
					break;
				}
				else if (!str_cmp (word, "Tn"))
				{
					int tl;
					int value;
					char *temp;
					
					value = fread_number (fp);
					
					temp = fread_string (fp);
					tl = talent_lookup (temp);
					
					if (tl < 0)
					{
						bugf ("[save.c::fread_char<%s>] Unknown talent [%s]", ch->name, temp);
					}
					else
						ch->pcdata->talents[tl] = value;
					fMatch = TRUE;
				}
				
				if (!str_cmp (word, "Todo"))
				{
					
					if (pos >= MAX_TODO)
					{
						fread_to_eol (fp);
						fMatch = TRUE;
						break;
					}
					
					ch->pcdata->todo[pos] = str_dup (fread_string (fp));
					pos++;
					fMatch = TRUE;
					break;
				}			
			}

			KEY ("Trai", ch->train, fread_number (fp));
			KEY ("Trust", ch->trust, fread_number (fp));
			KEY ("Tru", ch->trust, fread_number (fp));
			

		}		
		break;
		
    case 'V':
		KEY ("Version", ch->version, fread_number (fp));
		KEY ("Vers", ch->version, fread_number (fp));
		if (!str_cmp (word, "Vnum"))
		{
			ch->pIndexData = get_mob_index (fread_number (fp));
			fMatch = TRUE;
			break;
		}
		break;
		
    case 'W':
		KEY ("Wimpy", ch->wimpy, fread_number (fp));
		KEY ("Wimp", ch->wimpy, fread_number (fp));
    }
	
    if (!fMatch)
    {
		bugf ("[save.c::fread_char<%s>] No match. [%s]", ch->name, word);
		fread_to_eol (fp);
    }
  }
}


/**
 * Function: fread_pet
 * Descr   : Read pet/mount information from disk and stores it into
 *         : ch->pet, or ch->mount, respectively.
 * Returns : void
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, Updated by Gary McNickle <gary@tarmongaidon.org>
**/
void fread_pet (CHAR_DATA * ch, FILE * fp)
{
	char *word;
	CHAR_DATA *pet;
	bool fMatch;
	int lastlogoff = current_time;
	int percent;
	
	word = feof (fp) ? "END" : fread_word (fp);
	
	if (!str_cmp (word, "Vnum"))
	{
		long vnum;
		
		vnum = fread_long (fp);
		if (get_mob_index (vnum) == NULL)
		{
			bugf ("[save.c::fread_pet] bad vnum %ld.", vnum);
			pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
		}
		else
			pet = create_mobile (get_mob_index (vnum));
	}
	else
	{
		bugf ("[save.c::fread_pet] no vnum in file.", 0);
		pet = create_mobile (get_mob_index (MOB_VNUM_FIDO));
	}
	
	for (;;)
	{
		word = feof (fp) ? "END" : fread_word (fp);
		fMatch = FALSE;
		
		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;
			
		case 'A':
			KEY ("Alig", pet->alignment, fread_number (fp));
			
			if (!str_cmp (word, "ACs"))
			{
				int i;
				
				for (i = 0; i < 4; i++)
					pet->armor[i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Affc"))
			{
				AFFECT_DATA *paf;
				int sn;
				char *temp;
				
				paf = new_affect ();
				
				temp = fread_string (fp);
				sn = skill_lookup (temp);
				
				if (!IS_VALID_SKILL(sn))
					bugf ("[save.c::fread_pet] unknown skill [%s].", 0, temp);
				else
					paf->type = sn;
				
				paf->where = fread_number (fp);
				paf->level = fread_number (fp);
				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = fread_number (fp);
				paf->owner = fread_number (fp);
				
				/* Owner 0 == system, and this affect has already been
				* added by create mobile. */
				if (paf->owner > 0)
				{
					paf->next = pet->affected;
					pet->affected = paf;
				}
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "AMod"))
			{
				int stat;
				
				for (stat = 0; stat < MAX_STATS; stat++)
					pet->mod_stat[stat] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Attr"))
			{
				int stat;
				
				for (stat = 0; stat < MAX_STATS; stat++)
					pet->perm_stat[stat] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'C':
			KEY ("Clan", pet->clan, clan_lookup (fread_string (fp)));
			break;
			
		case 'D':
			KEY ("Dam", pet->damroll, fread_number (fp));
			KEY ("Desc", pet->description, fread_string (fp));
			break;
			
		case 'E':
			if (!str_cmp (word, "End"))
			{
				if (IS_FLAG (pet->states.player, _ACT_MOUNT))
					ch->mount = pet;
				else
					ch->pet = pet;
				
				pet->leader = ch;
				pet->master = ch;
				
				/* adjust hp opr move up  -- here for speed's sake */
				percent = (current_time - lastlogoff) * 25 / (2 * 60 * 60);
				
				if (percent > 0 && !IS_AFFECTED (ch, _AFF_POISON)
					&& !IS_AFFECTED (ch, _AFF_PLAGUE))
				{
					percent = UMIN (percent, 100);
					pet->hit += (pet->max_hit - pet->hit) * percent / 100;
					pet->move += (pet->max_move - pet->move) * percent / 100;
				}
				return;
			}
			KEY ("Exp", pet->exp, fread_number (fp));
			break;
			
		case 'G':
			KEY ("Gold", pet->gold, fread_number (fp));
			break;
			
		case 'H':
			KEY ("Hit", pet->hitroll, fread_number (fp));
			
			if (!str_cmp (word, "HV"))
			{
				pet->hit = fread_number (fp);
				pet->max_hit = fread_number (fp);
				pet->move = fread_number (fp);
				pet->max_move = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'L':
			KEY ("Levl", pet->level, fread_number (fp));
			KEY ("LnD", pet->long_descr, fread_string (fp));
			KEY ("LogO", lastlogoff, fread_number (fp));
			break;
			
		case 'N':
			KEY ("Name", pet->name, fread_string (fp));
			break;
			
		case 'O':
			if (!str_cmp (word, "OPR"))
			{
				pet->flow_str.current[AIR] = fread_number (fp);
				pet->flow_str.perm[AIR] = fread_number (fp);
				pet->flow_str.current[EARTH] = fread_number (fp);
				pet->flow_str.perm[EARTH] = fread_number (fp);
				pet->flow_str.current[WATER] = fread_number (fp);
				pet->flow_str.perm[WATER] = fread_number (fp);
				pet->flow_str.current[FIRE] = fread_number (fp);
				pet->flow_str.perm[FIRE] = fread_number (fp);
				pet->flow_str.current[SPIRIT] = fread_number (fp);
				pet->flow_str.perm[SPIRIT] = fread_number (fp);
			}
			break;
			
		case 'P':
			KEY ("Pos", pet->position, fread_number (fp));
			break;
			
		case 'R':
			KEY ("Race", pet->race, race_lookup (fread_string (fp)));
			break;
			
		case 'S':
			KEY ("Save", pet->saving_throw, fread_number (fp));
			KEY ("ShD", pet->short_descr, fread_string (fp));
			KEY ("Silv", pet->silver, fread_number (fp));
			
			/* I noticed the occasional mob turning up with a sex higher
			* than 3 (which will crash, in do_msat).  This is a rude
			* hack to fix that... */
			if (!str_cmp (word, "Sex"))
			{
				pet->sex = fread_number (fp);
				
				if (pet->sex > 2)
					pet->sex = number_range (1, 2);
				
				fMatch = TRUE;
				break;
			}
			break;
			
			if (!fMatch)
			{
				bugf ("[save.c::fread_pet] no match. (%s)", word);
				fread_to_eol (fp);
			}
			
    }
  }
}


/**
 * Function: fread_obj
 * Descr   : Reads object information from disk and sets ch->carrying, or
 *         : ch->pcdata->box, depending on object.
 * Returns : void
 * Syntax  : (n/a)
 * Written : v1.0 8/98
 * Author  : Rom Consortium, Gary McNickle <gary@tarmongaidon.org>
**/
void fread_obj (CHAR_DATA * ch, FILE * fp, bool is_box)
{
	OBJ_DATA *obj = NULL;
	char *word;
	char *test;
	int iNest;
	bool fMatch;
	bool fNest;
	bool fVnum = FALSE;
	bool first = FALSE;
	bool make_new = FALSE;	/* update object */
	
	if (is_box)
	{
		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;
	}
	
	word = feof (fp) ? "End" : fread_word (fp);
	
	if (!str_cmp (word, "Vnum"))
	{
		long vnum;
		first = FALSE;		/* fp will be in right place */
		
		vnum = fread_long (fp);
		if (get_obj_index (vnum) == NULL)
		{
			bugf ("[save.c::fread_obj] bad vnum %ld.", vnum);
		}
		else
		{
			obj = create_object (get_obj_index (vnum), -1);
		}
		
	}
	
	if (obj == NULL)		/* not found */
	{
		obj = new_obj ();
		obj->name = str_dup ("");
		obj->short_descr = str_dup ("");
		obj->description = str_dup ("");
	}
	
	fNest = FALSE;
	fVnum = TRUE;
	iNest = 0;
	
	for (;;)
	{
		if (first)
			first = FALSE;
		else
			word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;
		
		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;
			
		case 'A':
			if (!str_cmp (word, "Affc"))
			{
				AFFECT_DATA *paf;
				int sn;
				
				paf = new_affect ();
				test = fread_string (fp);
				sn = skill_lookup (test);
				
				if (!IS_VALID_SKILL(sn) && str_cmp(test, "qgen"))
					bugf ("[save.c::fread_obj] unknown skill.", 0);
				else
					if (str_cmp(test, "qgen"))
					  paf->type = sn;
					else
						paf->type = -1;
				
				paf->where = fread_number (fp);
				paf->level = fread_number (fp);
				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = fread_number (fp);
				paf->owner = fread_number (fp);
				paf->next = obj->affected;
				obj->affected = paf;
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'C':
			KEY ("Cond", obj->condition, fread_number (fp));
			KEY ("Cost", obj->cost, fread_number (fp));
			break;
			
		case 'D':
			KEY ("Description", obj->description, fread_string (fp));
			KEY ("Desc", obj->description, fread_string (fp));
			break;
			
		case 'E':
			
			if (!str_cmp (word, "Enchanted"))
			{
				obj->enchanted = TRUE;
				fMatch = TRUE;
				break;
			}
			
			KEY ("ExtraFlags", obj->extra_flags, fread_number (fp));
			KEY ("ExtF", obj->extra_flags, fread_number (fp));
			
			if (!str_cmp (word, "ExtraDescr") || !str_cmp (word, "ExDe"))
			{
				EXTRA_DESCR_DATA *ed;
				
				ed = new_extra_descr ();
				
				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = obj->extra_descr;
				obj->extra_descr = ed;
				fMatch = TRUE;
			}
			
			if (!str_cmp (word, "End"))
			{
				if (!fNest || (fVnum && obj->pIndexData == NULL))
				{
					bugf ("[save.c::fread_obj] incomplete object.", 0);
					if (obj)
						free_obj (obj);
					return;
				}
				else
				{
					if (!fVnum)
					{
						free_obj (obj);
						obj = create_object (get_obj_index (OBJ_VNUM_DUMMY), 0);
						if (!obj)
						{
							bugf ("[save.c::fread_obj] No object of [%ld] vnum", OBJ_VNUM_DUMMY);
							return;
						}
					}
					
					if (make_new)
					{
						int wear;
						
						wear = obj->wear_loc;
						extract_obj (obj);
						
						obj = create_object (obj->pIndexData, 0);
						obj->wear_loc = wear;
					}
					
					if (iNest == 0 || rgObjNest[iNest] == NULL)
					{
						if (is_box)
							obj_to_box (obj, ch);
						else
							obj_to_char (obj, ch);
					}
					else
						obj_to_obj (obj, rgObjNest[iNest - 1]);
					
					return;
				}
			}
			break;

		case 'H':
			KEY ("Hood", obj->hood, fread_number (fp));
			
		case 'I':
			KEY ("ItemType", obj->item_type, fread_number (fp));
			KEY ("Ityp", obj->item_type, fread_number (fp));
			break;
			
		case 'L':
			KEY ("Level", obj->level, fread_number (fp));
			KEY ("Lev", obj->level, fread_number (fp));
			break;
			
		case 'M':
			KEY ("MaxCond", obj->max_condition, fread_number (fp));
			break;
			
		case 'N':
			KEY ("Name", obj->name, fread_string (fp));
			
			if (!str_cmp (word, "Nest"))
			{
				iNest = fread_number (fp);
				if (iNest < 0 || iNest >= MAX_NEST)
				{
					bugf ("[save.c::fread_obj] bad nest %d.", iNest);
				}
				else
				{
					rgObjNest[iNest] = obj;
					fNest = TRUE;
				}
				fMatch = TRUE;
			}
			break;
			
			
		case 'S':
			KEY ("ShortDescr", obj->short_descr, fread_string (fp));
			KEY ("ShD", obj->short_descr, fread_string (fp));
			
			if (!str_cmp (word, "Sound"))
			{
				obj->sounds = load_sound (fp, obj->sounds);
				fMatch = TRUE;
				break;
			}
			else if (!str_cmp (word, "Spell"))
			{
				int iValue;
				int sn;
				
				iValue = fread_number (fp);
				
				sn = skill_lookup (fread_string (fp));
				
				if (iValue < 0 || iValue > 3)
				{
					bugf ("[save.c::fread_obj] bad iValue %d.", iValue);
				}
				else if (sn < 0)
				{
					bugf ("[save.c::fread_obj] unknown skill.", 0);
				}
				else
				{
					obj->value[iValue] = sn;
				}
				fMatch = TRUE;
				break;
			}
			
			break;
			
		case 'T':
			KEY ("Timer", obj->timer, fread_number (fp));
			KEY ("Time", obj->timer, fread_number (fp));
			break;
			
		case 'V':
			if (!str_cmp (word, "Values") || !str_cmp (word, "Vals"))
			{
				obj->value[0] = fread_number (fp);
				obj->value[1] = fread_number (fp);
				obj->value[2] = fread_number (fp);
				obj->value[3] = fread_number (fp);
				if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
					obj->value[0] = obj->pIndexData->value[0];
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Val"))
			{
				obj->value[0] = fread_number (fp);
				obj->value[1] = fread_number (fp);
				obj->value[2] = fread_number (fp);
				obj->value[3] = fread_number (fp);
				obj->value[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			if (!str_cmp (word, "Vnum"))
			{
				long vnum;
				
				vnum = fread_long (fp);
				if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
					bugf ("[save.c::fread_obj] bad vnum %ld.", vnum);
				else
					fVnum = TRUE;
				fMatch = TRUE;
				break;
			}
			break;
			
		case 'W':
			KEY ("WearFlags", obj->wear_flags, fread_number (fp));
			KEY ("WeaF", obj->wear_flags, fread_number (fp));
			KEY ("WearLoc", obj->wear_loc, fread_number (fp));
			KEY ("Wear", obj->wear_loc, fread_number (fp));
			KEY ("Weight", obj->weight, fread_number (fp));
			KEY ("Wt", obj->weight, fread_number (fp));
			break;
			
    }
	
    if (!fMatch)
    {
		bugf ("[save.c::fread_obj] no match. (%s)", word);
		fread_to_eol (fp);
    }
  }
}
