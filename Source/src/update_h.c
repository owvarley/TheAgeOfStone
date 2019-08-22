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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "music.h"


/* used for saving */

int save_number = 0;


/*
 * Repopulate areas periodically.
 */
void area_update (bool force)
{
	char buf[MIL];
	AREA_DATA *pArea;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	static long last_update;
	start_t = time(NULL);
#endif
	
	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		
		if (++pArea->age < 3 && !force)
			continue;
		
		/*
		 * Check age and reset.
		 * Note: Mud School resets every 3 minutes (not 15).
		 */
		if (((!pArea->empty 
			&& (pArea->nplayer == 0 || pArea->age >= 15))
			|| pArea->age >= 31) 
			|| force)
		{
			ROOM_INDEX_DATA *pRoomIndex;
			
			reset_area (pArea);
			sprintf (buf, "%s has just been reset.", pArea->name);
			wiznet (buf, NULL, NULL, WIZ_RESETS, 0, 0);
			
			pArea->age = number_range (0, 3);
			pRoomIndex = get_room_index (ROOM_VNUM_SCHOOL);
			if (pRoomIndex != NULL && pArea == pRoomIndex->area)
				pArea->age = 13;
			else 
				if (pArea->nplayer == 0)
					pArea->empty = TRUE;
		}
	}

#ifdef _DEBUG
	end_t = time(NULL);

	sprintf(buf, "[update_h.c::area_update] Update took %2ld seconds. %2ld seconds since last update.", end_t - start_t, time(NULL) - last_update);
	wiznet(buf, NULL, NULL, WIZ_TRACE, 0, 0);

	last_update = time(NULL);
#endif
	
	return;
}

/*
 * OLC - Reset one room.  Called by reset_area and olc.
 */
void reset_room (ROOM_INDEX_DATA * pRoom)
{
	RESET_DATA *pReset;
	CHAR_DATA *pMob;
	CHAR_DATA *mob;
	OBJ_DATA *pObj;
	CHAR_DATA *LastMob = NULL;
	OBJ_DATA *LastObj = NULL;
	int iExit;
	int level = 0;
	bool last;
	
	if (!pRoom)
		return;
	
	pMob = NULL;
	last = FALSE;
	
	for (iExit = 0; iExit < MAX_DIR; iExit++)
	{
		EXIT_DATA *pExit;
		if ((pExit = pRoom->exit[iExit]))
		{
			pExit->exit_info = pExit->rs_flags;
			if ((pExit->u1.to_room != NULL)
				&& ((pExit = pExit->u1.to_room->exit[rev_dir[iExit]])))
			{
				/* nail the other side */
				pExit->exit_info = pExit->rs_flags;
			}
		}
	}
	
	for (pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next)
	{
		MOB_INDEX_DATA *pMobIndex;
		OBJ_INDEX_DATA *pObjIndex;
		OBJ_INDEX_DATA *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;
		int count = 0;
		int limit = 0;
		
		/*
		 * Check the chance of resetting this mob or object.
		 */
		if (pReset->command == 'M'
			|| pReset->command == 'E'
			|| pReset->command == 'G' || pReset->command == 'O')
		{
			if (pReset->chance < 100 && number_percent () > pReset->chance)
			{
				/* not this time bucko... */
				continue;
			}
		}
		
		switch (pReset->command)
		{
			default:
				bugf ("[update_h.c::reset_room] [#%ld]: bad command %c.", pRoom->vnum,
					pReset->command);
				break;
				
			case 'M':
				if (!(pMobIndex = get_mob_index (pReset->arg1)))
				{
					bugf ("[update_h.c::reset_room] [#%ld]: 'M': bad vnum %d.", pRoom->vnum,
						pReset->arg1);
					continue;
				}
				
				if ((pRoomIndex = get_room_index (pReset->arg3)) == NULL)
				{
					bugf("[update_h.c::reset_room] 'R': bad vnum %d.", pReset->arg3);
					continue;
				}
				
				if (pMobIndex->count >= pReset->arg2)
				{
					last = FALSE;
					break;
				}
				/* */
				count = 0;
				for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
				{
					if (mob->pIndexData == pMobIndex)
					{
						count++;
						
						if (count >= pReset->arg4)
						{
							last = FALSE;
							break;
						}
					}
					
					if (!IS_NPC (mob))
						snd_trigger (mob, pRoomIndex->sounds, SOUND_ROOM, SOUND_ROOM_RESET, TRUE);
				}
				
				if (count >= pReset->arg4)
					break;
				
				/* */
				
				pMob = create_mobile (pMobIndex);
				
				/*
				 * Some more hard coding.
				 */
				if (room_is_dark (pRoom))
					SET_FLAG (pMob->states.affected_by, _AFF_INFRARED);
				
				/*
				 * Pet shop mobiles get ACT_PET set.
				 */
				{
					ROOM_INDEX_DATA *pRoomIndexPrev;
					
					pRoomIndexPrev = get_room_index (pRoom->vnum - 1);
					if (pRoomIndexPrev
						&& IS_SET (pRoomIndexPrev->room_flags, ROOM_PET_SHOP))
						SET_FLAG (pMob->states.player, _ACT_PET);
				}
				
				char_to_room (pMob, pRoom);
				
				LastMob = pMob;
				level = URANGE (0, pMob->level - 2, LEVEL_HERO - 1);	/* -1 ROM */
				last = TRUE;
				break;
				
			case 'O': // Load an object into a room
				{
					int number = 1;
					
					if (!(pObjIndex = get_obj_index (pReset->arg1)))
					{
						bugf ("[update_h.c::reset_room] [#%ld] : 'O' 1 : bad vnum %d",
							pRoom->vnum, pReset->arg1);
						
						bugf ("%d %d %d %d",
							pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4);
						
						continue;
					}
					
					if (!(pRoomIndex = get_room_index (pReset->arg3)))
					{
						bugf ("[update_h.c::reset_room] [R#%ld] : 'O' 2 : bad vnum %d.", pRoom->vnum,
							pReset->arg3);
						bugf ("%d %d %d %d", pReset->arg1, pReset->arg2, pReset->arg3,
							pReset->arg4);
						continue;
					}
					
					if (pRoom->area->nplayer > 0)
					{
						last = FALSE;
						break;
					}
					
					number = count_obj_list (pObjIndex, pRoom->contents);
					
					if (number > 0 && (pReset->arg4 == 0 ||
						(pReset->arg4 != 0 && number >= pReset->arg4)))
					{
						last = FALSE;
						break;
					}
					
					pObj = create_object (pObjIndex, -1);
					if (!pObj)
					{
						bugf ("[update_h.c::reset_room] [R#%ld], Invalid object. [%d]",
							pRoom->vnum, pObjIndex);
						break;
					}
					
					pObj->cost = 0;
					obj_to_room (pObj, pRoom);
					
					if (pReset->arg4 > 1)
						for (number = 1; number < pReset->arg4; number++)
						{
							pObj = create_object (pObjIndex, -1);
							obj_to_room (pObj, pRoom);
							/* we already know pObj is good... */
						}
						last = TRUE;
						break;
				}
				
			case 'P': // Load an object into another object.

				if (!(pObjIndex = get_obj_index (pReset->arg1)))
				{
					bugf ("[update_h.c::reset_room] [R#%ld] : 'P': bad vnum %d.", pRoom->vnum,
						pReset->arg1);
					continue;
				}
				
				if (!(pObjToIndex = get_obj_index (pReset->arg3)))
				{
					bugf ("[update_h.c::reset_room] [R#%ld] : 'P': bad vnum %d.", pRoom->vnum,
						pReset->arg3);
					continue;
				}
				
				if (pReset->arg2 == -1 || pReset->arg2 > 999 )	
					limit = 999;
				else
					limit = pReset->arg2;
				
				if (pRoom->area->nplayer > 0
					|| (LastObj = get_obj_type(pRoom->contents, pObjToIndex)) == NULL
					|| (LastObj->in_room == NULL && !last)
					|| (pObjIndex->count >= limit)
					|| (count = count_obj_list (pObjIndex, LastObj->contains)) > pReset->arg4)
				{
					last = FALSE;
					break;
				}
				
				while (count < pReset->arg4)
				{
					pObj = create_object (pObjIndex, -1);
					
					if (!pObj)
					{
						bugf ("[update_h.c::reset_room] [R#%ld]. Invalid object [%d]",
							pRoom->vnum, pObjIndex);
						break;
					}
					
					obj_to_obj (pObj, LastObj);
					count++;
				}
				
				/* fix object lock state! */
				LastObj->value[1] = LastObj->pIndexData->value[1];
				last = TRUE;
				break;
				
			case 'G':
			case 'E':
				if (!(pObjIndex = get_obj_index (pReset->arg1)))
				{
					bugf ("[update_h.c::reset_room] [#%ld] : 'E' or 'G': bad vnum %d.", pRoom->vnum,
						pReset->arg1);
					continue;
				}
				
				if (!last)
					break;
				
				if (!LastMob)
				{
					bugf ("[update_h.c::reset_room] [#%ld] : 'E' or 'G': null mob for vnum %d.",
						pRoom->vnum, pReset->arg1);
					last = FALSE;
					break;
				}
				
				if (LastMob->pIndexData->pShop)	/* Shop-keeper? */
				{
					pObj = create_object (pObjIndex, -1);
					if (!pObj)
					{
						logf ("[update_h.c::reset_room] [R#%ld], invalid object [%d]",
							pRoom->vnum, pObjIndex);
						break;
					}
					SET_BIT (pObj->extra_flags, ITEM_INVENTORY);	/* ROM OLC */
					
				}
				else			/* ROM OLC else version */
				{
					int limit;
					if (pReset->arg2 > 50)	/* old format */
						limit = 6;
					else if (pReset->arg2 == -1 || pReset->arg2 == 0)	/* no limit */
						limit = 999;
					else
						limit = pReset->arg2;
					
					if (pObjIndex->count < limit || number_range (0, 4) == 0)
					{
						pObj = create_object (pObjIndex, -1);
						
						if (!pObj)
						{
							logf ("[update_h.c::reset_room] Invalid object [%d]", pObjIndex);
							break;
						}
						
					}
					else
						break;
				}
				
				obj_to_char (pObj, LastMob);
				if (pReset->command == 'E')
					equip_char (LastMob, pObj, pReset->arg3);
				last = TRUE;
				break;
				
			case 'D':
				break;
				
			case 'R':
				if (!(pRoomIndex = get_room_index (pReset->arg1)))
				{
					bugf ("[update_h.c::reset_room] [#%ld] : 'R': bad vnum %d.", pRoom->vnum,
						pReset->arg1);
					continue;
				}
				
				{
					EXIT_DATA *pExit;
					int d0;
					int d1;
					
					for (d0 = 0; d0 < pReset->arg2 - 1; d0++)
					{
						d1 = number_range (d0, pReset->arg2 - 1);
						pExit = pRoomIndex->exit[d0];
						pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
						pRoomIndex->exit[d1] = pExit;
					}
				}
				break;
		}
  }
  
  return;
}

/* 
 * OLC - Reset one area.
 */
void reset_area (AREA_DATA * pArea)
{
	ROOM_INDEX_DATA *pRoom;
	long vnum;
	
	for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
	{
		if ((pRoom = get_room_index (vnum)))
			reset_room (pRoom);
	}
	
	return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update (void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	EXIT_DATA *pexit;
	int door;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	char buf[MIL];
	static long last_update;
	start_t = time(NULL);
#endif
	
	/* Examine all mobs. */
	for (ch = char_list; ch != NULL; ch = ch_next)
	{
		ch_next = ch->next;
		
		if (!IS_NPC (ch) || ch->in_room == NULL
			|| IS_AFFECTED (ch, _AFF_COMPULSION))
			continue;
		
		if (ch->in_room->area->empty)
			continue;
		
		/* Examine call for special procedure */
		if (ch->spec_fun)
		{
			if ((*ch->spec_fun) (ch))
				continue;
		}
		else
			if (ch->pIndexData->pShop != NULL)	/* give him some gold */
				if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth)
				{
					ch->gold += ch->pIndexData->wealth * number_range (1, 20) / 5000000;
					ch->silver += ch->pIndexData->wealth * number_range (1, 20) / 50000;
				}
				
				/*
				 * Check triggers only if mobile still in default position
				 */
				if (ch->position == ch->pIndexData->default_pos)
				{
					/* Delay */
					if (HAS_TRIGGER (ch, TRIG_DELAY) && ch->mprog_delay > 0)
					{
						if (--ch->mprog_delay <= 0)
						{
							mp_percent_trigger (ch, NULL, NULL, NULL, TRIG_DELAY);
							continue;
						}
					}
					if (HAS_TRIGGER (ch, TRIG_RANDOM))
					{
						if (mp_percent_trigger (ch, NULL, NULL, NULL, TRIG_RANDOM))
							continue;
					}
				}
				
				/* That's all for sleeping / busy monster, and empty zones */
				if (ch->position != POS_STANDING)
					continue;
				
				/* Scavenge */
				if (IS_FLAG (ch->states.player, _ACT_SCAVENGER)
					&& ch->in_room->contents != NULL && number_bits (6) == 0)
				{
					OBJ_DATA *obj;
					OBJ_DATA *obj_best;
					int max;
					
					max = 1;
					obj_best = 0;
					for (obj = ch->in_room->contents; obj; obj = obj->next_content)
					{
						if (CAN_WEAR (obj, ITEM_TAKE) && can_loot (ch, obj)
							&& obj->cost > max)
							/*  && obj->cost > 0) */
						{
							obj_best = obj;
							max = obj->cost;
						}
					}
					
					if (obj_best)
					{
						obj_from_room (obj_best);
						obj_to_char (obj_best, ch);
						act ("$n gets $p.", ch, obj_best, NULL, TO_ROOM, POS_RESTING);
					}
				}
				
				/* Wander */
				if (!IS_FLAG (ch->states.player, _ACT_SENTINEL)
					&& number_bits (3) == 0
					&& (door = number_bits (5)) <= 5
					&& (pexit = ch->in_room->exit[door]) != NULL
					&& pexit->u1.to_room != NULL
					&& !IS_SET (pexit->exit_info, EX_CLOSED)
					&& !IS_SET (pexit->u1.to_room->room_flags, ROOM_NO_MOB)
					&& (!IS_FLAG (ch->states.player, _ACT_STAY_AREA)
					|| pexit->u1.to_room->area == ch->in_room->area)
					&& (!IS_FLAG (ch->states.player, _ACT_OUTDOORS)
					|| !IS_SET (pexit->u1.to_room->room_flags, ROOM_INDOORS))
					&& (!IS_FLAG (ch->states.player, _ACT_INDOORS)
					|| IS_SET (pexit->u1.to_room->room_flags, ROOM_INDOORS)))
				{
					move_char (ch, door, FALSE);
				}
	}
	
#ifdef _DEBUG
	end_t = time(NULL);

	sprintf(buf, "[update_h.c::mobile_update] Update took %ld seconds. %ld seconds since last update.",
				 end_t - start_t, time(NULL) - last_update);

	wiznet (buf, NULL, NULL, WIZ_TRACE, 0, 0);

	last_update = time(NULL);
#endif

	return;
}



/*
 * Update the weather.
 */
void weather_update (void)
{
	char buf[MSL];
	DESCRIPTOR_DATA *d;
	int diff;
	int trigger = -1;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	static long last_update;

	start_t = time(NULL);
#endif

	memset(buf, 0, MSL);

	switch (++time_info.hour)
	{
		case 5:
			trigger = SOUND_AREA_SUN_LIGHT;
			weather_info.sunlight = SUN_LIGHT;
			strcat (buf, "The day has begun.\n\r");
			break;
			
		case 6:
			trigger = SOUND_AREA_SUN_RISE;
			weather_info.sunlight = SUN_RISE;
			strcat (buf, "The sun rises in the east.\n\r");
			break;
			
		case 19:
			trigger = SOUND_AREA_SUN_SET;
			weather_info.sunlight = SUN_SET;
			strcat (buf, "The sun slowly disappears in the west.\n\r");
			break;
			
		case 20:
			trigger = SOUND_AREA_SUN_DARK;
			weather_info.sunlight = SUN_DARK;
			strcat (buf, "The night has begun.\n\r");
			break;
			
		case 24:
			time_info.hour = 0;
			time_info.day++;
			time_info.day_c++;
			break;
	}
	
	if (time_info.day >= 35)
	{
		time_info.day = 0;
		time_info.month++;
	}

	// Time to change the season
	// If mud crashes we loose time and season information
	// Need to have this saved to time.dat file at a later date.
	if (time_info.day_c >= 45)
	{
		time_info.day_c = 0;
		switch(time_info.season)
		{
		case SEASON_SUMMER: time_info.season = SEASON_AUTUMN;
							break;
		case SEASON_AUTUMN: time_info.season = SEASON_WINTER;
							break;
		case SEASON_WINTER: time_info.season = SEASON_SPRING;
							break;
		case SEASON_SPRING: time_info.season = SEASON_SUMMER;
							break;
		}		
	}
	
	if (time_info.month >= 17)
	{
		time_info.month = 0;
		time_info.year++;
	}
	
	/*
	 * Weather change.
	 */
	if (time_info.month >= 9 && time_info.month <= 16)
		diff = weather_info.mmhg > 985 ? -2 : 2;
	else
		diff = weather_info.mmhg > 1015 ? -2 : 2;
	
	weather_info.change += diff * dice (1, 4) + dice (2, 6) - dice (2, 6);
	weather_info.change = UMAX (weather_info.change, -12);
	weather_info.change = UMIN (weather_info.change, 12);
	
	weather_info.mmhg += weather_info.change;
	weather_info.mmhg = UMAX (weather_info.mmhg, 960);
	weather_info.mmhg = UMIN (weather_info.mmhg, 1040);
	
	switch (weather_info.sky)
	{
		default:
			bugf("[update_h.c::weather_update] bad sky %d.", weather_info.sky);
			weather_info.sky = SKY_CLOUDLESS;
			break;
			
		case SKY_CLOUDLESS:
			if (weather_info.mmhg < 990
				|| (weather_info.mmhg < 1010 && number_bits (2) == 0))
			{
				trigger = SOUND_AREA_CLOUDY;
				strcat (buf, "The sky is getting cloudy.\n\r");
				weather_info.sky = SKY_CLOUDY;
			}
			break;
			
		case SKY_CLOUDY:
			if (weather_info.mmhg < 970
				|| (weather_info.mmhg < 990 && number_bits (2) == 0))
			{
				trigger = SOUND_AREA_RAINING;
				strcat (buf, "It starts to rain.\n\r");
				weather_info.sky = SKY_RAINING;
			}
			
			if (weather_info.mmhg > 1030 && number_bits (2) == 0)
			{
				trigger = SOUND_AREA_CLOUDLESS;
				strcat (buf, "The clouds disappear.\n\r");
				weather_info.sky = SKY_CLOUDLESS;
			}
			break;
			
		case SKY_RAINING:
			if (weather_info.mmhg < 970 && number_bits (2) == 0)
			{
				trigger = SOUND_AREA_LIGHTNING;
				strcat (buf, "Lightning flashes in the sky.\n\r");
				weather_info.sky = SKY_LIGHTNING;
			}
			
			if (weather_info.mmhg > 1030
				|| (weather_info.mmhg > 1010 && number_bits (2) == 0))
			{
				trigger = SOUND_AREA_CLOUDY;
				strcat (buf, "The rain stopped.\n\r");
				weather_info.sky = SKY_CLOUDY;
			}
			break;
			
		case SKY_LIGHTNING:
			if (weather_info.mmhg > 1010
				|| (weather_info.mmhg > 990 && number_bits (2) == 0))
			{
				trigger = SOUND_AREA_RAINING;
				strcat (buf, "The lightning has stopped.\n\r");
				weather_info.sky = SKY_RAINING;
				break;
			}
			break;
	}
	
	if (buf[0] != '\0')
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING
				&& !IS_NPC (d->character)
				&& IS_OUTSIDE (d->character)
				&& IS_AWAKE (d->character)
				&& !IS_FLAG (d->character->states.config, _NOWEATHER))
			{
				if (trigger != -1 && d->character->zone)
					snd_trigger (d->character, d->character->zone->sounds, SOUND_AREA,trigger, TRUE);

				send_to_char (buf, d->character);
				
				if ((weather_info.sunlight == SUN_LIGHT ||
					weather_info.sunlight == SUN_DARK ||
					weather_info.sunlight == SUN_SET)
					&& IS_AFFECTED (d->character, _AFF_DARK_VISION))
				{
					send_to_char ("Your eyes take a moment to adjust to the"
						" changing light.\n\r", d->character);
				}
			}
		}
	}
	

#ifdef _DEBUG
	end_t = time(NULL);

	sprintf(buf, "[update_h.c::weather_update] Update took %ld seconds. %ld seconds since last update.",
				 end_t - start_t, time(NULL) - last_update);

	wiznet (buf, NULL, NULL, WIZ_TRACE, 0, 0);

	last_update = time(NULL);
#endif
	
	return;
}

/*
 * Update the bank system
 * (C) 1996 The Maniac from Mythran Mud
 *
 */
void bank_update (void)
{
	int value = 0;
	char file[MIL];
	FILE *fp;

	if ((time_info.hour < 9) || (time_info.hour > 17))
		return;			/* Bank is closed, no market... */

	value = number_range (0, 200);
	value -= 100;
	value /= 10;
	
	pMud.share_value = UMAX (10, pMud.share_value + value);
	
	sprintf (file, "%s%s", DATA_DIR, BANK_FILE);
	if (!(fp = fopen (file, "w")))
	{
		bugf ("[update_h.c::bank_update] fopen of %s failed", file);
		return;
	}
	fprintf (fp, "SHARE_VALUE %d\n\r", pMud.share_value);
	fclose (fp);
}


/*
 * Update all chars, including mobs.
 */
void char_update (void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *ch_quit;
	int i = 0;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	char buf[MIL];
	static long last_update;

	start_t = time(NULL);
#endif	
	
	ch_quit = NULL;
	
	/* update save counter */
	save_number++;
	
	if (save_number > 29)
		save_number = 0;
	
	for (ch = char_list; ch != NULL; ch = ch_next)
	{
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;
		
		ch_next = ch->next;
		
		if (ch->timer > 30)
			ch_quit = ch;
		
		if (ch->wtimer > 0 && ++ch->wtimer >= pMud.WaitTimer)
			return_ch_to_play (ch);
		
		if (ch->riding && ch->mount && ch->mount->in_room != ch->in_room)
			ch->riding = FALSE;
		
		// treesinging update
		if ( !IS_NPC(ch) && ch->desc && ch->desc->editor == ED_TREESING)
		{
			OBJ_DATA *pObj = (OBJ_DATA *)ch->desc->pEdit;
			
			if ( ch->position == POS_FIGHTING ||
				ch->position < POS_RESTING )
			{
				ch->desc->editor = ED_NONE;
				
				if ( pObj )
					extract_obj(pObj);
				
				ch->desc->pEdit = NULL;
				send_to_char("Your song has been interrupted.\n\r", ch);
			}
			
			if ( !pObj )
			{
				ch->desc->editor = ED_NONE;
			}
			else
			{
				if ( !IS_NULLSTR(pObj->short_descr) 
					&&   !IS_NULLSTR(pObj->description)
					&&   pObj->timer > 0)
				{
					pObj->timer--;
					send_to_char("Your treesong echoes around the room.\n\r", ch);
					act("$n's song echoes around the room.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				}
				
				if ( pObj->timer < 1 )
				{
					switch ( pObj->item_type )
					{
					case ITEM_TREASURE:
						{
							pObj->level = ch->level;
							memset(pObj->value, 0, 4);
							pObj->weight		= 100 / UMAX(2, ( (ch->level/10 + ch->level/10)-2 ));
							pObj->wear_flags	= ITEM_TAKE|ITEM_HOLD|ITEM_NO_SAC;
						}
						break;
					case ITEM_FURNITURE:
						{
							pObj->weight		= 100 / UMAX(2, ( (ch->level/10 + ch->level/10)-2 ));
							pObj->level			= ch->level;
							pObj->value[0]		= ch->level/10;
							pObj->value[1]		= (ch->level/10)*1000;
							pObj->wear_flags	= ITEM_TAKE|ITEM_HOLD|ITEM_NO_SAC;
							pObj->value[2]		= SIT_AT|SIT_ON|REST_AT|REST_ON|SLEEP_ON|SLEEP_AT;
						}
						break;
					case ITEM_WEAPON:
						{    
							int a,b;
							AFFECT_DATA af;
							int iSkill = skill_lookup("treesinging");
							
							af.type = iSkill;
							af.where = TO_OBJECT;
							af.level = pObj->level;
							af.duration = -1;
							af.location = APPLY_HITROLL;
							af.modifier = (int)(ch->level/10*(float)(get_skill(ch, iSkill)/100));
							af.bitvector = ITEM_BURN_PROOF;
							af.owner = GET_ID(ch);
							affect_to_obj(pObj, &af);
							af.location = APPLY_DAMROLL;
							affect_to_obj(pObj, &af);
							
							if ( ch->level < 24 )
								{ a = 6; b = 5; }
							else
								if ( ch->level < 45 )
									{ a = 8; b = 5; }
								else
									{ a = 10; b = 5; }
								
								pObj->weight		= 100 / ( ch->level/10 + ch->level/10 );
								pObj->level			= ch->level;
								pObj->value[0]		= WEAPON_EXOTIC;
								pObj->value[1]		= a;
								pObj->value[2]		= b;
								pObj->value[3]		= DAM_BASH;
								pObj->value[4]		= WEAPON_TWO_HANDS;
								pObj->wear_flags	= ITEM_TAKE|ITEM_HOLD|ITEM_NO_SAC|ITEM_WIELD;
								
						} 
						break;
					case ITEM_ARMOR:
						{ // 12, 16, 20 [<24, <45]
							int ac;
							
							if ( ch->level < 24)
								ac = 12;
							else
								if ( ch->level < 45)
									ac = 16;
								else 
									ac = 20;
								
								pObj->weight	= 100 / ( ch->level/10 + ch->level/10 );
								pObj->level		= ch->level;
								pObj->value[0]	= ac;
								pObj->value[1]	= ac;
								pObj->value[2]	= ac;
								pObj->value[3]	= ac;
								pObj->wear_flags = ITEM_TAKE|ITEM_HOLD|ITEM_NO_SAC|ITEM_WEAR_SHIELD;
						}
						break;
						default:
						break;
					}
					
					send_to_char("Your treesung object is finished.\n\r", ch);
					{
						OBJ_DATA *o;

						o = create_object(pObj->pIndexData, 0);
						clone_object(pObj, o);
						obj_to_char(o, ch);
						extract_obj(pObj);
					}
					ch->desc->editor = ED_NONE;
					ch->desc->pEdit = NULL;
				}
			}
		}
		
		if (ch->position >= POS_STUNNED)
		{
			/* check to see if we need to go home */
			if (IS_NPC (ch) && ch->zone != NULL && ch->zone != ch->in_room->area
				&& ch->desc == NULL && ch->fighting == NULL
				&& !IS_AFFECTED (ch, _AFF_COMPULSION) && number_percent () < 5
				&& can_fight (ch, TRUE))
			{
				act ("$n wanders on home.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				extract_char (ch, TRUE);
				continue;
			}
			
			if (ch->hit < ch->max_hit)
				ch->hit += hit_gain (ch);
			else
				ch->hit = ch->max_hit;
			
				refresh_OP (ch, FALSE);	
			
			if (ch->move < ch->max_move)
				ch->move += move_gain (ch);
			else
				ch->move = ch->max_move;
		}
		
		if (ch->position == POS_STUNNED)
			update_pos (ch);
		
		if (!IS_NPC (ch) && ch->level < LEVEL_IMMORTAL)
		{
			OBJ_DATA *obj;
			
			if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL
				&& obj->item_type == ITEM_LIGHT && obj->value[2] > 0)
			{
				if (--obj->value[2] == 0 && ch->in_room != NULL)
				{
					--ch->in_room->light;
					act ("$p goes out.", ch, obj, NULL, TO_ROOM, POS_RESTING);
					act ("$p flickers and goes out.",
						ch, obj, NULL, TO_CHAR, POS_RESTING);
					extract_obj (obj);
				}
				else if (obj->value[2] <= 5 && ch->in_room != NULL)
					act ("$p flickers.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			}
			
			if (IS_IMMORTAL (ch))
				ch->timer = 0;
			
			if (++ch->timer >= 12)
			{
				if (ch->was_in_room == NULL && ch->in_room != NULL)
				{
					ch->was_in_room = ch->in_room;
					
					if (ch->fighting != NULL)
						stop_fighting (ch, TRUE);
					
					act ("$n disappears into the void.",
						ch, NULL, NULL, TO_ROOM, POS_RESTING);
					
					send_to_char ("You disappear into the void.\n\r", ch);
					
					if (ch->level > 1)
						save_char_obj (ch);
					
					char_from_room (ch);
					char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
				}
			}
			
			gain_condition (ch, _DRUNK, -1);
			gain_condition (ch, _FULL, ch->size > SIZE_MEDIUM ? -4 : -2);
			gain_condition (ch, _THIRST, -1);
			gain_condition (ch, _HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
		}
		
		for (paf = ch->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			if (paf->duration > 0)
			{
				paf->duration--;
				if (number_range (0, 4) == 0 && paf->level > 0)
					paf->level--;		/* spell strength fades with time */
			}
			else if (paf->duration < 0)
				;
			else
			{
				if (paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0)
				{
					if (paf->type > 0 && !str_cmp(skill_table[paf->type].name, "seize") && !IS_NPC(ch) )
					{
						if (ch->pcdata->true_sex == SEX_MALE)
							chprintf(ch,"You struggle to hold on to who you are as Saidin leaves you.");
						else
							chprintf(ch,"You feel a great loss as your embrace of Saidar waxes.");
					}
					else
						if (paf->type > 0 && skill_table[paf->type].msg_off)
						{
							send_to_char (skill_table[paf->type].msg_off, ch);
							send_to_char ("\n\r", ch);
						}
				}
				
				affect_remove (ch, paf);
			}
		}
		
		/*
		 * Careful with the damages here,
		 *   MUST NOT refer to ch after damage taken,
		 *   as it may be lethal damage (on NPC).
		 */
		if ( ch != NULL && IS_AFFECTED(ch, _AFF_WOUNDED) )
		{
			int dam = 0;

			act("$n grimaces in pain as $s blood pools on the floor.",
				ch, NULL, NULL, TO_ROOM, POS_RESTING);
			chprintf(ch, "You grimace in pain from your wounds.\n\r");
			
			dam = number_range(1, UMIN(1,ch->level/5));
			ch->move = UMAX(0, ch->move - dam);

			damage(ch, ch, dam, gsn_lacerate, DAM_NONE, FALSE, FALSE);

			if ( !ch )
				continue;
		}

		if ( ch != NULL && is_affected (ch, skill_lookup("plague")))
		{
			AFFECT_DATA *af, plague;
			CHAR_DATA *vch;
			int dam;
			int iSkill = skill_lookup("plague");
			
			if (ch->in_room == NULL)
			  break;
			
			act ("$n writhes in agony as plague sores erupt from $s skin.",
				ch, NULL, NULL, TO_ROOM, POS_RESTING);
			send_to_char ("You writhe in agony from the plague.\n\r", ch);
			for (af = ch->affected; af != NULL; af = af->next)
			{
				if (af->type == iSkill)
					break;
			}
			
			if (af == NULL)
			{
				REMOVE_FLAG (ch->states.affected_by, _AFF_PLAGUE);
				break;
			}
			
			if (af->level == 1)
				break;
			
			plague.where = TO_AFFECTS;
			plague.type = iSkill;
			plague.level = af->level - 1;
			plague.duration = number_range (1, 2 * plague.level);
			plague.location = APPLY_STR;
			plague.modifier = -5;
			plague.bitvector = _AFF_PLAGUE;
			
			for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
			{
				if (!saves_spell (plague.level - 2, vch, DAM_DISEASE)
					&& !IS_IMMORTAL (vch)
					&& !IS_AFFECTED (vch, _AFF_PLAGUE) && number_bits (4) == 0)
				{
					send_to_char ("You feel hot and feverish.\n\r", vch);
					act ("$n shivers and looks very ill.",
						vch, NULL, NULL, TO_ROOM, POS_RESTING);
					affect_join (vch, &plague);
				}
			}
			
			dam = UMIN (ch->level, af->level / 5 + 1);
			
			/* deduct 10% from each flow */
			for (i = 0; i < 5; i++)
				ch->flow_str.current[i] =
				(int) (ch->flow_str.current[i] - ch->flow_str.current[i] * .10);
			
			ch->move -= dam;
			damage (ch, ch, dam, iSkill, DAM_DISEASE, FALSE, TRUE);
		}
		else if (IS_AFFECTED (ch, _AFF_POISON)
			&& ch != NULL && !IS_AFFECTED (ch, _AFF_SLOW))
			
		{
			AFFECT_DATA *poison;
			int iSkill = skill_lookup("poison");

			poison = affect_find (ch->affected, iSkill);
			
			if (poison != NULL)
			{
				act ("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				send_to_char ("You shiver and suffer.\n\r", ch);
				damage (ch, ch, poison->level / 10 + 1, iSkill,
					DAM_POISON, FALSE, TRUE);
			}
		}
		
		else if (ch->position == POS_INCAP && number_range (0, 1) == 0)
		{
			damage (ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE, TRUE);
		}
		else if (ch->position == POS_MORTAL)
		{
			damage (ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE, TRUE);
		}
  }
  
  /*
   * Autosave and autoquit.
   * Check that these chars still exist.
   */
  for (ch = char_list; ch != NULL; ch = ch_next)
  {
	  ch_next = ch->next;
	  
	  if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number)
	  {
		  log_string ("Autosaving...");
		  save_char_obj (ch);
	  }
	  
	  if (ch == ch_quit)
		  do_quit (ch, "");
  }


#ifdef _DEBUG
    end_t = time(NULL);

	sprintf(buf, "[update_h.c::char_update] Update took %ld seconds. %ld seconds since last update.",
				 end_t - start_t, time(NULL) - last_update);

	wiznet (buf, NULL, NULL, WIZ_TRACE, 0, 0); 

	last_update = time(NULL);
#endif

  return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update (void)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	AFFECT_DATA *paf, *paf_next;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	char buf[MIL];
	static long last_update;

	start_t = time(NULL);
#endif

	for (obj = object_list; obj != NULL; obj = obj_next)
	{
		CHAR_DATA *rch;
		char *message;
		
		obj_next = obj->next;
		
		/* go through affects and decrement */
		for (paf = obj->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			if (paf->duration > 0)
			{
				paf->duration--;
				if (number_range (0, 4) == 0 && paf->level > 0)
					paf->level--;		/* spell strength fades with time */
			}
			else if (paf->duration < 0)
				;
			else
			{
				if (paf_next == NULL
					|| paf_next->type != paf->type || paf_next->duration > 0)
				{
					if (paf->type > 0 && skill_table[paf->type].msg_obj)
					{
						if (obj->carried_by != NULL)
						{
							rch = obj->carried_by;
							act (skill_table[paf->type].msg_obj,
								rch, obj, NULL, TO_CHAR, POS_RESTING);
						}
						if (obj->in_room != NULL && obj->in_room->people != NULL)
						{
							rch = obj->in_room->people;
							act (skill_table[paf->type].msg_obj,
								rch, obj, NULL, TO_ALL, POS_RESTING);
						}
					}
				}
				
				affect_remove_obj (obj, paf);
			}
		}
		
		if (obj->timer > 0 && obj->timer < 4
			&& obj->pIndexData->vnum == OBJ_VNUM_DISC)
		{
			if (obj->carried_by != NULL)
				act ("$p fades in and out of view for a moment.\n\r",
				obj->carried_by, obj, NULL, TO_CHAR, POS_RESTING);
		}
		
		if (obj->timer <= 0 || --obj->timer > 0)
			continue;
		
		switch (obj->item_type)
		{
		default:
			// Message for tornado decay
			if (!str_cmp(obj->name, "a tornado"))
			{
				message = "As suddenly as it appears, $p evaporates into the atmosphere leaving a trail of destruction in its wake.";
				break;
			}
			else
			{
				message = "$p crumbles into dust.";
				break;
			}
		case ITEM_FOUNTAIN:
			message = "$p dries up.";
			break;
		case ITEM_CORPSE_NPC:
			message = "$p decays into dust.";
			break;
		case ITEM_CORPSE_PC:
			message = "$p decays into dust.";
			break;
		case ITEM_FOOD:
			message = "$p decomposes.";
			break;
		case ITEM_POTION:
			message = "$p has evaporated from disuse.";
			break;
		case ITEM_PORTAL:
			message = "$p closes in on itself.";
			break;
		case ITEM_CONTAINER:
			if (CAN_WEAR (obj, ITEM_WEAR_FLOAT))
				if (obj->contains)
					message =
					"$p flickers and vanishes, spilling its contents on the floor.";
				else
					message = "$p flickers and vanishes.";
				else
					message = "$p crumbles into dust.";
				break;
		}
		
		if (obj->carried_by != NULL)
		{
			if (IS_NPC (obj->carried_by)
				&& obj->carried_by->pIndexData->pShop != NULL)
				obj->carried_by->silver += obj->cost / 5;
			else
			{
				act (message, obj->carried_by, obj, NULL, TO_CHAR, POS_RESTING);
				if (obj->wear_loc == WEAR_FLOAT)
					act (message, obj->carried_by, obj, NULL, TO_ROOM, POS_RESTING);
			}
		}
		else if (obj->in_room != NULL && (rch = obj->in_room->people) != NULL)
		{
			if (!(obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
				&& !CAN_WEAR (obj->in_obj, ITEM_TAKE)))
			{
				act (message, rch, obj, NULL, TO_ROOM, POS_RESTING);
				act (message, rch, obj, NULL, TO_CHAR, POS_RESTING);
			}
		}
		
		if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
			&& obj->contains)
		{				/* save the contents */
			OBJ_DATA *t_obj, *next_obj;
			
			for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
			{
				next_obj = t_obj->next_content;
				obj_from_obj (t_obj);
				
				if (obj->in_obj)	/* in another object */
					obj_to_obj (t_obj, obj->in_obj);
				
				else if (obj->carried_by)	/* carried */
					if (obj->wear_loc == WEAR_FLOAT)
						if (obj->carried_by->in_room == NULL)
							extract_obj (t_obj);
						else
							obj_to_room (t_obj, obj->carried_by->in_room);
						else
							obj_to_char (t_obj, obj->carried_by);
						
						else if (obj->in_room == NULL)	/* destroy it */
							extract_obj (t_obj);
						
						else			/* to a room */
							obj_to_room (t_obj, obj->in_room);
			}
		}
		
		extract_obj (obj);
  }
  
#ifdef _DEBUG
    end_t = time(NULL);

	sprintf(buf, "[update_h.c::obj_update] Update took %ld seconds. %ld seconds since last update.",
				 end_t - start_t, time(NULL) - last_update);

	wiznet (buf, NULL, NULL, WIZ_TRACE, 0, 0);

	last_update = time(NULL);
#endif

  return;
}



/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update (void)
{
	CHAR_DATA *wch;
	CHAR_DATA *wch_next;
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *last;
	CHAR_DATA *victim;

#ifdef _DEBUG
	long start_t = 0, end_t = 0;
	char buf[MIL];
	static long last_update;

	start_t = time(NULL);
#endif

	for (wch = char_list; wch != NULL; wch = wch_next)
	{
		wch_next = wch->next;
		if (IS_NPC (wch)
			|| wch->position < POS_STANDING
			|| wch->level >= LEVEL_IMMORTAL
			|| wch->in_room == NULL || wch->in_room->area->empty)
			continue;
		
		for (ch = wch->in_room->people; ch != NULL; ch = ch_next)
		{
			int count;
			
			ch_next = ch->next_in_room;

			if (!IS_NPC (ch)
				|| !IS_FLAG (ch->states.player, _ACT_AGGRESSIVE)
				|| IS_SET (ch->in_room->room_flags, ROOM_SAFE)
				|| IS_AFFECTED (ch, _AFF_CALM)
				|| ch->fighting != NULL
				|| IS_AFFECTED (ch, _AFF_COMPULSION)
				|| !IS_AWAKE (ch)
				|| (IS_FLAG (ch->states.player, _ACT_WIMPY) && IS_AWAKE (wch))
				|| !can_see (ch, wch) || number_bits (1) == 0)
				continue;
			
			/*
			 * Ok we have a 'wch' player character and a 'ch' npc aggressor.
			 * Now make the aggressor fight a RANDOM pc victim in the room,
			 *   giving each 'vch' an equal chance of selection.
			 */
			count = 0;
			victim = NULL;
			
			for (vch = wch->in_room->people; vch != NULL; vch = vch_next)
			{
				vch_next = vch->next_in_room;
				
				if (!IS_NPC (vch)
					&& vch->level < IM
					&& ch->level >= vch->level - 5
					&& (!IS_FLAG (ch->states.player, _ACT_WIMPY) || !IS_AWAKE (vch))
					&& can_see (ch, vch))
				{
					if (number_range (0, count) == 0)
						victim = vch;
					count++;
				}
			}
			
			last = ch;

			if (victim == NULL || IS_IMMORTAL (victim))
				continue;
			
			if (RIDDEN (ch))
			{
				if (!mount_success (RIDDEN (wch), ch, FALSE))
				{
					send_to_char ("Your mount escapes your control!\n\r", RIDDEN (ch));
					multi_hit (ch, victim, TYPE_UNDEFINED);
				}
				else
				{
					send_to_char ("You manage to keep your mount under control.\n\r",
						RIDDEN (ch));
				}
			}
			else
				multi_hit (ch, victim, TYPE_UNDEFINED);
			
		}
	}

#ifdef _DEBUG
	end_t = time(NULL);

	sprintf(buf, "[update_h.c::aggr_update] Update took %2ld seconds. %ld seconds since last update.", end_t - start_t, time(NULL) - last_update);
	wiznet (buf,  NULL, NULL, WIZ_TRACE, 0, 0);

	last_update = time(NULL);	
#endif

	return;
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler (void)
{
	static int pulse_area;
	static int pulse_mobile;
	static int pulse_violence;
	static int pulse_point;
	static int pulse_music;
	static int pulse_auction;
	static int pulse_weather;
	static int pulse_weatheraff;
	static int pulse_aggress;
	
	if (--pulse_area <= 0)
	{
		pulse_area = PULSE_AREA;
		area_update (FALSE);
		bank_update ();
		
		if (!webUP)
			webUP = (init_web_server(port + 1));
	}
	
	if (--pulse_auction <= 0)
	{
		pulse_auction = PULSE_AUCTION;
		auction_update ();
	}
	
	if (--pulse_music <= 0)
	{
		pulse_music = PULSE_MUSIC;
		song_update ();
	}
	
	if (--pulse_mobile <= 0)
	{
		pulse_mobile = PULSE_MOBILE;
		mobile_update ();
	}
	
	if (--pulse_weather <= 0)
	{
		// Check added as we cannot update weather if no players are on
		pulse_weather = PULSE_WEATHER;
		weatherstate_update ();
		// See if a tornado is spawned
		tornado ();
		lightning ();
	}

	if (--pulse_weatheraff <= 0)
	{
		pulse_weatheraff = PULSE_WEATHERAFF;
		// See if a tornado is spawned
		tornado ();

		// Create lightning strikes in storms
		lightning ();
	
	}
	if (--pulse_violence <= 0)
	{
		pulse_violence = PULSE_VIOLENCE;
		violence_update ();
	}
	
	if (--pulse_point <= 0)
	{
		wiznet ("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
		pulse_point = PULSE_TICK;
		
		time_update ();
		char_update ();
		obj_update ();
	}
	
	if (pMud.timer != -1 && IS_SET (pMud.flags, MUD_COPYOVER))
	{
		if (pMud.timer-- <= 0)
			copyover_state (NULL, "");
		else
			if (pMud.timer % (PULSE_PER_SECOND * 30) == 0 && pMud.timer > 4)
			{
				char buf[MIL];
				
				sprintf (buf, "Copyover in %.2f %s.",
					(pMud.timer >=
					240) ? (float) pMud.timer / 60 /
					PULSE_PER_SECOND : (float) pMud.timer / PULSE_PER_SECOND,
				   (pMud.timer >= 240) ? "minute(s)" : "second(s)");
				
				system_msg (buf);
			}
	}

	
	if (--pulse_aggress <= 0)
	{
		pulse_aggress = PULSE_AGGRESS;
		aggr_update ();
	}

	tail_chain ();
	return;
}


