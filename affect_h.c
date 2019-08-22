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
 * @@ Objective (affect_h.c)
 *
 * This file contains the support code for applying, modifying and 
 * deleting affects on players, mobs and objects.
 */


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"


/* enchanted stuff for eq */
void affect_enchant (OBJ_DATA * obj)
{
	/* okay, move all the old flags into new vectors if we have to */
	if (!obj->enchanted)
	{
		AFFECT_DATA *paf, *af_new;
		obj->enchanted = TRUE;
		
		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			af_new = new_affect ();
			
			af_new->next = obj->affected;
			obj->affected = af_new;
			
			af_new->where = paf->where;
			af_new->type = UMAX (0, paf->type);
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
			af_new->owner = paf->owner;
		}
	}
}


/*
 * Apply or remove an affect to a character.
 */
void affect_modify (CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd)
{
	OBJ_DATA *wield;
	int mod, i;
	
	mod = paf->modifier;
	
	if (fAdd)
	{
		if (!IS_FLAG (ch->states.affected_by, paf->bitvector))
			switch (paf->where)
		{
		  case TO_AFFECTS:
			  SET_FLAG (ch->states.affected_by, paf->bitvector);
			  break;
		  case TO_IMMUNE:
			  SET_FLAG (ch->states.imm, paf->bitvector);
			  break;
		  case TO_RESIST:
			  SET_FLAG (ch->states.res, paf->bitvector);
			  break;
		  case TO_VULN:
			  SET_FLAG (ch->states.vuln, paf->bitvector);
			  break;
		  default:
			  break;
		}
	}
	else
	{
		if (IS_FLAG (ch->states.affected_by, paf->bitvector))
			switch (paf->where)
		{
		  case TO_AFFECTS:
			  REMOVE_FLAG (ch->states.affected_by, paf->bitvector);
			  break;
		  case TO_IMMUNE:
			  REMOVE_FLAG (ch->states.imm, paf->bitvector);
			  break;
		  case TO_RESIST:
			  REMOVE_FLAG (ch->states.res, paf->bitvector);
			  break;
		  case TO_VULN:
			  REMOVE_FLAG (ch->states.vuln, paf->bitvector);
			  break;
		  default:
			  break;
		}
		mod = 0 - mod;
	}
	
	switch (paf->location)
	{
		default:
			bugf ("[affect_h.c::affect_modify] bad loc: %d on ch: %s in R#%d",
				paf->location, (ch != NULL) ? (IS_NPC (ch)) ?
				ch->short_descr : ch->name : "(none)",
				(ch->in_room != NULL) ? ch->in_room->vnum : 0);
			return;
			
		case APPLY_NONE:
			break;
		case APPLY_STR: 
			ch->mod_stat[STAT_STR] += mod;
			break;
		case APPLY_DEX:
			ch->mod_stat[STAT_DEX] += mod;
			break;
		case APPLY_INT:
			ch->mod_stat[STAT_INT] += mod;
			break;
		case APPLY_WIS:
			ch->mod_stat[STAT_AGL] += mod;
			break;
		case APPLY_CON:
			ch->mod_stat[STAT_CON] += mod;
			break;
		case APPLY_SEX:
			ch->sex += mod;
			break;
		case APPLY_CLASS:
			break;
		case APPLY_LEVEL:
			break;
		case APPLY_AGE:
			break;
		case APPLY_HEIGHT:
			break;
		case APPLY_WEIGHT:
			break;
		case APPLY_HIT:
			ch->max_hit += mod;
			break;
		case APPLY_ENDURANCE:
			ch->max_move += mod;
			break;
		case APPLY_GOLD:
			break;
		case APPLY_EXP:
			break;
		case APPLY_AC:
			for (i = 0; i < 4; i++)
				ch->armor[i] += mod;
			break;
		case APPLY_HITROLL:
			ch->hitroll += mod;
			break;
		case APPLY_DAMROLL:
			ch->damroll += mod;
			break;
		case APPLY_SAVES:
			ch->saving_throw += mod;
			break;
	#if defined(AllowSaveTypes)
		case APPLY_SAVING_ROD:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_PETRI:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_BREATH:
			ch->saving_throw += mod;
			break;
	#endif
		case APPLY_SAVING_SPELL:
			ch->saving_throw += mod;
			break;
		case APPLY_SPELL_AFFECT:
			break;
	}
	
	/*
	 * Check for weapon wielding.
	 * Guard against recursion (for weapons with affects).
	 */
	if (!IS_NPC (ch) 
		&& (wield = get_eq_char (ch, WEAR_WIELD)) != NULL
		&& get_obj_weight (wield) > (str_app[get_curr_stat (ch, STAT_STR)].wield * 10))
	{
		static int depth;
		
		if (depth == 0)
		{
			depth++;
			act ("You drop $p.", ch, wield, NULL, TO_CHAR, POS_DEAD);
			act ("$n drops $p.", ch, wield, NULL, TO_ROOM, POS_RESTING);
			obj_from_char (wield);
			obj_to_room (wield, ch->in_room);
			depth--;
		}
	}
	
	return;
}


/* find an effect in an affect list */
AFFECT_DATA *affect_find (AFFECT_DATA * paf, int sn)
{
	AFFECT_DATA *paf_find;
	
	for (paf_find = paf; paf_find != NULL; paf_find = paf_find->next)
	{
		if (paf_find->type == sn)
			return paf_find;
	}
	
	return NULL;
}

/* fix object affects when removing one */
void affect_check (CHAR_DATA * ch, int where, int vector)
{
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	
	if (where == TO_OBJECT || where == TO_WEAPON || vector == 0)
		return;
	
	for (paf = ch->affected; paf != NULL; paf = paf->next)
		if (paf->where == where && paf->bitvector == vector)
		{
			switch (where)
			{
				case TO_AFFECTS:
					SET_FLAG (ch->states.affected_by, vector);
					break;
				case TO_IMMUNE:
					SET_FLAG (ch->states.imm, vector);
					break;
				case TO_RESIST:
					SET_FLAG (ch->states.res, vector);
					break;
				case TO_VULN:
					SET_FLAG (ch->states.vuln, vector);
					break;
				}
				return;
		}
		
		for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
		{
			if (obj->wear_loc == -1)
				continue;
			
			for (paf = obj->affected; paf != NULL; paf = paf->next)
				if (paf->where == where && paf->bitvector == vector)
				{
					switch (where)
					{
						case TO_AFFECTS:
							SET_FLAG (ch->states.affected_by, vector);
							break;
						case TO_IMMUNE:
							SET_FLAG (ch->states.imm, vector);
							break;
						case TO_RESIST:
							SET_FLAG (ch->states.res, vector);
							break;
						case TO_VULN:
							SET_FLAG (ch->states.vuln, vector);
						
					}
					return;
				}
				
				if (obj->enchanted)
					continue;
				
				for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
					if (paf->where == where && paf->bitvector == vector)
					{
						switch (where)
						{
							case TO_AFFECTS:
								SET_FLAG (ch->states.affected_by, vector);
								break;
							case TO_IMMUNE:
								SET_FLAG (ch->states.imm, vector);
								break;
							case TO_RESIST:
								SET_FLAG (ch->states.res, vector);
								break;
							case TO_VULN:
								SET_FLAG (ch->states.vuln, vector);
								break;
						}
						return;
					}
		}
}

/*
 * Give an affect to a char.
 */
void affect_to_char (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	AFFECT_DATA *paf_new;
	
	paf_new = new_affect ();
	
	*paf_new = *paf;
	
	paf_new->next = ch->affected;
	ch->affected = paf_new;
	
	affect_modify (ch, paf_new, TRUE);
	return;
}

/* give an affect to an object */
void affect_to_obj (OBJ_DATA * obj, AFFECT_DATA * paf)
{
	AFFECT_DATA *paf_new;
	
	paf_new = new_affect ();
	
	*paf_new = *paf;
	paf_new->next = obj->affected;
	obj->affected = paf_new;
	
	/* apply any affect vectors to the object's extra_flags */
	if (paf->bitvector)
		switch (paf->where)
    {
		case TO_OBJECT:
			SET_BIT (obj->extra_flags, paf->bitvector);
			break;
		case TO_WEAPON:
			if (obj->item_type == ITEM_WEAPON)
				SET_BIT (obj->value[4], paf->bitvector);
			break;
		default:
			break;
    }
	
	
	return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	int where;
	int vector;
	
	if (ch->affected == NULL)
	{
		bugf("[affect_h.c::affect_remove] no affect.");
		return;
	}
	
	affect_modify (ch, paf, FALSE);
	where = paf->where;
	vector = paf->bitvector;
	
	if (paf == ch->affected)
	{
		ch->affected = paf->next;
	}
	else
	{
		AFFECT_DATA *prev;
		
		for (prev = ch->affected; prev != NULL; prev = prev->next)
		{
			if (prev->next == paf)
			{
				prev->next = paf->next;
				break;
			}
		}
		
		if (prev == NULL)
		{
			bugf("[affect_h.c::affect_remove] cannot find paf.");
			return;
		}
	}
	
	free_affect (paf);
	
	affect_check (ch, where, vector);
	return;
}

void affect_remove_obj (OBJ_DATA * obj, AFFECT_DATA * paf)
{
	int where, vector;
	if (obj->affected == NULL)
	{
		bugf("[affect_h.c::affect_remove_obj] no affect.");
		return;
	}
	
	if (obj->carried_by != NULL && obj->wear_loc != -1)
		affect_modify (obj->carried_by, paf, FALSE);
	
	where = paf->where;
	vector = paf->bitvector;
	
	/* remove flags from the object if needed */
	if (paf->bitvector)
		switch (paf->where)
    {
		case TO_OBJECT:
			REMOVE_BIT (obj->extra_flags, paf->bitvector);
			break;
		case TO_WEAPON:
			if (obj->item_type == ITEM_WEAPON)
				REMOVE_BIT (obj->value[4], paf->bitvector);
			break;
		default:
			break;
    }
	
	if (paf == obj->affected)
	{
		obj->affected = paf->next;
	}
	else
	{
		AFFECT_DATA *prev;
		
		for (prev = obj->affected; prev != NULL; prev = prev->next)
		{
			if (prev->next == paf)
			{
				prev->next = paf->next;
				break;
			}
		}
		
		if (prev == NULL)
		{
			bugf("[affect_h.c::affect_remove_obj] cannot find paf.");
			return;
		}
	}
	
	free_affect (paf);
	
	if (obj->carried_by != NULL && obj->wear_loc != -1)
		affect_check (obj->carried_by, where, vector);
	return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip (CHAR_DATA * ch, int sn)
{
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;
	
	for (paf = ch->affected; paf != NULL; paf = paf_next)
	{
		paf_next = paf->next;
		if (paf->type == sn)
			affect_remove (ch, paf);
	}
	
	return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected (CHAR_DATA * ch, int sn)
{
	AFFECT_DATA *paf;
	
	for (paf = ch->affected; paf != NULL; paf = paf->next)
	{
		if (paf->type == sn)
			return TRUE;
	}
	
	return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	AFFECT_DATA *paf_old;
	bool found;
	
	found = FALSE;
	for (paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next)
	{
		if (paf_old->type == paf->type)
		{
			paf->level = (paf->level += paf_old->level) / 2;
			paf->duration += paf_old->duration;
			paf->modifier += paf_old->modifier;
			affect_remove (ch, paf_old);
			break;
		}
	}
	
	affect_to_char (ch, paf);
	return;
}
