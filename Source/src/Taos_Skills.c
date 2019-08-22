/** 
 * @@ Copyright (The Age of Stone Project)
 *
 * The Age of Stone (TaoS): Version v0.1 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

//
// File: Taos_skill.c
// Purpose: File holds a listing of all the code for all the different skills used in Taos
// skills are broken up into different sections to make reading of the code easier. These 
// sections are:
// 1) Combat
// 2) Craft
// 3) Stealth
// 4) Survival
// 5) Music
// 6) Knowledge
// Further thoughts are spliting these different skills into separate files should they take
// up to much space in the single file.
//

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////COMBAT///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////CRAFT////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////







///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////STEALTH///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////







///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////SURVIVAL//////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

_DOFUN(do_forage)
{
//Enter variables
OBJ_DATA *food;

int luckroll, stat, skillroll, value, effectroll;




//Then check to see if player knows skill...
	if (ch->pcdata->learned[gsn_forage] <= 0)
	{
		send_to_char("You don't know the first thing about foraging for food.\n\r", ch);
		return;
	}

//Then check for required stamina...
	if (ch->move < 3)
	{
		send_to_char("You don't have enough energy to forage.\n\r", ch);
		return;
	}

//Check to see if it's a MOB...
	if ( IS_NPC(ch) )
	{ 
		send_to_char("MOBs do not eat, and therefor do not forage.\n\r", ch);
		return;
	}

//Then check to see if player is occupied...
	if ( ch->position == POS_FIGHTING || ch->position == POS_WORKING || ch->position == POS_PLAYING)
	{
		send_to_char("You're a little too busy to forage at the moment...\n\r", ch);
		return;
	}

	if ( ch->position == POS_RESTING || ch->position == POS_SITTING )
	{
		send_to_char("Get off your seat if you want to eat!.\n\r", ch);
		return;
	}

	if ( ch->position == POS_SLEEPING )
	{
		send_to_char("In your dreams, you forage food, but in the waking world you still have an empty stomach.\n\r", ch);
		return;
	}

	if ( ch->position < POS_SLEEPING )
	{
		send_to_char("You're not in any condition to forage for food!\n\r", ch);
		return;
	}

//Then check to see if player is in the great outdoors
	if ( ch->in_room->sector_type == SECT_INSIDE )
	{
		send_to_char("You momentarily consider foraging indoors, then disregard the thought as pure foolishness.\n\r",ch);
		return;
	}
	if ( ch->in_room->sector_type == SECT_CITY )
	{
		send_to_char("Although you COULD find some scraps of old meat or pieces of spoiled fruit in the city, you decide against foraging after all.\n\r",ch);
		return;
	}
	if ( ch->in_room->sector_type == SECT_ROAD )
	{
		send_to_char("You look about, but all you see is hard, packed dirt.\n\r",ch);
		return;
	}

//Now perform Skill check
luckroll = random_dice(1, 10, 0);
stat = ch->perm_stat[STAT_INT];
skillroll = skill_roll(ch, gsn_forage);
value = luckroll + stat + skillroll;

//Now check the value for success
	if (value < 17 )
	{
		send_to_char("You look about, foraging for food, yet come up empty handed.\n\r",ch);
		ch->move -= 3;
		return;
	}		
	if (value >= 17)
	{
		food = create_object(get_obj_index(OBJ_VNUM_FOOD), 0);
			if ( ch->in_room->sector_type == SECT_FOREST )
			{
				food->name = "berries forest food" ;
				food->short_descr  = "a handfull of berries";
				food->description = "A handfull of berries picked from the woods lays here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find some berries.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_JUNGLE )
			{
				food->name = "fruit jungle food" ;
				food->short_descr = "a large piece of fruit";
				food->description = "A large piece of jungle fruit lies here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find a piece of fruit.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_MOUNTAIN )
			{
				food->name = "roots mountain food" ;
				food->short_descr  = "a vegetable root";
				food->description = "An edible root lies here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find an edible root.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_SWAMP )
			{
				food->name = "flower swamp food" ;
				food->short_descr  = "a swamp flower";
				food->description = "A pretty swamp flower lies here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find an edible flower.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_DESERT )
			{
				food->name = "beetles desert food" ;
				food->short_descr  = "a few beetles";
				food->description = "A couple of dead beetles lie here on their backs.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find a couple of beatles in the sand.  Hakuna mattata!\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_BEACH )
			{
				food->name = "coconut beach food" ;
				food->short_descr  = "a coconut";
				food->description = "A fallen coconut lies here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find a coconut.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}
			if ( ch->in_room->sector_type == SECT_RIVER )
			{
				food->name = "crawfish food" ;
				food->short_descr  = "a crawfish";
				food->description = "A dead crawfish lies here.";
				food->value[1] = 10;
				send_to_char("After a bit of foraging, you manage to find a crawfish in the water.\n\r",ch);
				obj_to_char (food, ch);
				return;
			}

		}
		ch->move -= 3;

		return;
}			





///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////MUSIC////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////








///////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////KNOWLEDGE/////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

