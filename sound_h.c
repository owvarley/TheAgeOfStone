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
 * @@ Objective (sounds.c)
 *
 * Handles all Mud Sound Protocol triggers and parsing
 */



#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"


/** Function: strsnd
  * Descr   : Parses sound data into a string
  * 	    : Also determines sounds URL by sound type.
  * Returns : Parsed String
  * Syntax  : strsnd( sound )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
char *strsnd (SOUND_DATA * sound)
{
	char buf[MSL];
	char url[MSL];
	
	if (!sound)
		return NULL;
	
	sprintf (url, "%s%s/",
		pMud.BaseSoundURL, flag_string (sound_type_table, sound->type));
	
	if (!sound->music)
		sprintf (buf, "!!SOUND(%s V=%d L=%d P=%d T=%s U=%s)",
		sound->fname, sound->volume, sound->repeats, sound->priority,
		flag_string (sound_type_table, sound->type), url);
	else
		sprintf (buf, "!!MUSIC(%s V=%d L=%d C=1 T=%s U=%s)",
		sound->fname, sound->volume, sound->repeats,
		flag_string (sound_type_table, sound->type), url);
	
	return str_dup (buf);
}



/** Function: snd_trigger
  * Descr   : Determines if a sound contains the required type/trigger.
  * Returns : TRUE or FALSE
  * Syntax  : snd_trigger ( ch, sound data, type of sound wanted, 
  *         :               sound trigger required, send to char? )
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
bool snd_trigger (CHAR_DATA * ch, SOUND_DATA * sPtr, int type,
				  int trigger, bool send)
{
	SOUND_DATA *pSound;
	
	if (!sPtr || !ch || !IS_FLAG (ch->states.config, _SOUNDON))
		return FALSE;
	
	if (type < SOUND_SYSTEM || type > SOUND_IMMORTAL)
	{
		logf ("snd_trigger: invalid type [%d]", type);
		return FALSE;
	}
	
	for (pSound = sPtr; pSound != NULL; pSound = pSound->next)
	{
		if (pSound->type != type)
			continue;
		
		/*
		 * note: although there is no type checking when assigning sounds
		 * for duplicate triggers, only the first trigger matching the requested
		 * trigger type will be used.
		 */
		if (pSound->trigger == trigger)
		{
			if (send && ch != NULL)
			{
				if (pSound->targets == TO_CHAR)
					send_to_char (strsnd (pSound), ch);
				else
				{
					CHAR_DATA *vch;
					char *sound = strsnd (pSound);
					
					for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
					{
						if (!IS_NPC (vch) && vch->desc == NULL)
							continue;
						
						send_to_char (sound, vch);
					}
				}
			}
			return TRUE;
		}
	}
	
	return FALSE;
}

/** Function: snd_door_trigger
  * Descr   : Special case handler for door triggers. Regardless of a
  *         : sounds targets, never send door sounds to the entire room!
  * Returns : void
  * Syntax  : 
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void snd_door_trigger (CHAR_DATA * ch, int door, bool enter)
{
	SOUND_DATA *pSound;
	int trigger = -1;
	
	if (door < 0 || door > 5 || !ch->in_room || !ch->in_room->sounds)
		return;
	
	switch (door)
	{
		case 0:			/* north */
			if (enter)
				trigger = SOUND_ROOM_ENTER_SOUTH;
			else
				trigger = SOUND_ROOM_EXIT_NORTH;
			break;
			
		case 1:			/* east */
			if (enter)
				trigger = SOUND_ROOM_ENTER_WEST;
			else
				trigger = SOUND_ROOM_EXIT_EAST;
			break;
			
		case 2:			/* south */
			if (enter)
				trigger = SOUND_ROOM_ENTER_NORTH;
			else
				trigger = SOUND_ROOM_EXIT_SOUTH;
			break;
			
		case 3:			/* west */
			if (enter)
				trigger = SOUND_ROOM_ENTER_EAST;
			else
				trigger = SOUND_ROOM_EXIT_WEST;
			break;
			
		case 4:			/* up */
			if (enter)
				trigger = SOUND_ROOM_ENTER_DOWN;
			else
				trigger = SOUND_ROOM_EXIT_UP;
			break;
			
		case 5:			/* down */
			if (enter)
				trigger = SOUND_ROOM_ENTER_UP;
			else
				trigger = SOUND_ROOM_EXIT_DOWN;
			break;
	}
	
	if (trigger != -1)
	{
		int any;
		ROOM_INDEX_DATA *from = get_room_index (ch->from_room);
		
		if (enter)
			any = SOUND_ROOM_ENTER_ANY;
		else
			any = SOUND_ROOM_EXIT_ANY;
		
		for (pSound = ch->in_room->sounds; pSound != NULL; pSound = pSound->next)
		{
			if (pSound->type != SOUND_ROOM)
				continue;
			
			if (pSound->trigger == trigger || pSound->trigger == any)
			{
				send_to_char (strsnd (pSound), ch);
				return;
			}
		}
		
		/* ok, no room sounds -- do we want to default and check area sounds? */
		
		if (!ch->in_room->area || !ch->in_room->area->sounds ||
			!from || from->area == ch->in_room->area)
			return;
		
		if (enter)
			trigger = SOUND_AREA_ENTER;
		else
			trigger = SOUND_AREA_LEAVE;
		
		for (pSound = ch->in_room->area->sounds; pSound != NULL;
		pSound = pSound->next)
		{
			if (pSound->type != SOUND_AREA)
				continue;
			
			if (pSound->trigger == trigger)
			{
				send_to_char (strsnd (pSound), ch);
				return;
			}
		}
	}
}
