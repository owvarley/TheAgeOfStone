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
 * @@ Objective (maneuvers.c)
 * 
 * This file contains the code necessary to perform the military maneuvers skill.
 *
 * At the time of this release, this code is unfinished. Sorry.
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"



struct maneuver_type maneuver_table[MAX_MANEUVERS] = 
{
	{
			"High Shield Wall",
			"Arrows rain upon $N's party and clash harmlessly off their shield wall.",
			"$N calls for a high shield wall and your arrow clashes harmlessly off their defense.",
			"You call for a high shield wall as you notice a hail of arrows fall upon the group.",
			DEFENSIVE_ONLY|PROJECTILES_ONLY|REQUIRES_STR_18,	
			REDUCE_DAMAGE_BY_ALL,
			1.000f
	},

	{
			"Low Shield Wall",
			"$N's group falls into a low shield wall, protecting them from harm.",
			"$n calls $S group into a low shield wall, an impenetrable defense!",
			"You call for a low shield wall, creating an impenetrable defense!",
			DEFENSIVE_ONLY|REQUIRES_FOUR_OR_MORE,
			REDUCE_DAMAGE_BY_ALL|COMBAT_HALT,
			1.000f
	},
	
	{		
			"Pike Rally",
			"$N calls for a setting of the pikes and readies to receive $n's charge.",
			"$N's group readies for your charge, setting the pikes and impaling your steed!",
			"You call for a readying of the pikes to receive $n's charge, impaling $s steed!",
			DEFENSIVE_ONLY|REQUIRES_SKILLED_HELP,
			REDUCE_DAMAGE_BY_ALL|DAMAGE_MOUNTS,
			1.000f
	},

	{
			"Shield strike",
			"$N twists with $n's attack, a wound opened but smashing the weapon with $S shield.",
			"You hit $N, but $E rolls with the attack and smashes your weapon with $S shield.",
			"$n's attack catches you, but you crash your shield into $s weapon- damaging it.",
			DEFENSIVE_ONLY|REQUIRES_STR_18,
			REDUCE_DAMAGE_BY_THIRD|DAMAGE_OPPONENTS_WEAPON,
			1.250f
	},
	
	{
			"Shield rush",
			"$N charges quickly from the shield wall and rams $n shield-first!",
			"$N charges from the shield wall and rams you shield-first!",
			"You charge from the shield wall shield-first and ram $n!",
			OFFENSIVE_ONLY|REQUIRES_FOUR_OR_MORE|REQUIRES_MOVEMENTS|REQUIRES_STR_18,
			DAMAGE_OPPONENTS,
			2.000f
	},

	{		
			"Flanking", 
			"$N flanks behind $n while $e is distracted and attacks!",
			"$N flanks you and attacks from behind!", 
			"You flank $n, leaving a wide attack open to $s back!", 
			OFFENSIVE_ONLY|REQUIRES_FOUR_OR_MORE|REQUIRES_MOVEMENTS,
			DAMAGE_EXTRA,
			2.000f
	},

	{		
			"Hedgehog", 
			"$N's group falls back into a hedgehog, swords and pikes impaling $n.",
			"$N calls for a hedgehog- swords and pikes extended, impaling you during your charge!",
			"You call and your group falls into a hedgehog, swords and pikes rallied against $n!", 
			DEFENSIVE_ONLY|REQUIRES_SKILLED_HELP|REQUIRES_FOUR_OR_MORE,
			REDUCE_DAMAGE_BY_ALL|REVERSE_DAMAGE,
			1.250f
	},

	{		
			"Spearhead", 
			"$N spearheads $Mself between $n's rescue attempt!",
			"$N spearheads past your rescue attempt, attacking your ward!", 
			"You lunge between $n and $n's ward, spearheading $s rescue!", 
			OFFENSIVE_ONLY,
			DAMAGE_OPPONENTS|CANCEL_RESCUE,
			2.000f
	},

	{		
			"Feint", 
			"$N feigns an attack, as $R hacks into $n's $B!",
			"$N's attack misses you, but it was just a feint as $R hacks into your $B!",
			"You feint and $R hacks into a distracted $n's $B!", 
			OFFENSIVE_ONLY|REQUIRES_SKILLED_HELP,
			GROUPED_DMG|MISSED_TRIP,
			2.000f
	},

	{		
			"Cover", 
			"$N takes a blow aimed for $R and rolls with the attack.",
			"$N intercepts your attack upon $R and rolls with the blow.",
			"You intercept $n's attack upon $R and roll with the blow.", 
			DEFENSIVE_ONLY,
			COVER_PARTY|REDUCE_DAMAGE_BY_THIRD,
			2.000f
	},

	{		"Pincer", 
			"$N and $R dodge behind a distracted $n and hack into $m!", 
			"Distracted by combat, you miss $N and $R catching you in a pincer!", 
			"You and $R catch a distracted $n in a pincer formation!", 
			OFFENSIVE_ONLY|REQUIRES_SKILLED_HELP|REQUIRES_FOUR_OR_MORE,
			DAMAGE_EXTRA|GROUPED_DMG,
			2.000f
	},

	{		"Garen's Wall",
			"$N ducks behind $n and prevents $m from fleeing!", 
			"$N darts behind you in Garen's Wall and prevents you from fleeing!", 
			"Garen's Wall, You dart behind $n and prevent $m from fleeing!", 
			DEFENSIVE_ONLY|REQUIRES_FOUR_OR_MORE,
			CANCEL_FLEE,
			1.250f
			
	},

	{		NULL, NULL, NULL, NULL, 0, 0, 0.000f }

};


int check_maneuver(CHAR_DATA *ch, long maneuver)
{
	int i = 0; 
	CHAR_DATA *vch = NULL, *vch_next = NULL;
	bool bFound = FALSE;

	if ( !ch || IS_NPC(ch) || (maneuver == 0 && ch->position > POS_SITTING))
		return FALSE; 

	for (vch = char_list; vch; vch = vch_next )
	{
		vch_next = vch->next;

		if ( is_same_group(vch, ch) )
			bFound = TRUE;
	}


	if ( !bFound )
		return FALSE;

	for ( i = 0; maneuver_table[i].name != NULL; i++ )
	{
		if ( IS_SET(maneuver_table[i].flags,  maneuver) )
			return i;
	}

	return -1;

}


bool validate_requirements(CHAR_DATA *ch, CHAR_DATA *vch, int maneuver, bool report)
{
	int iGroup = 1;
	int iSkilled = 0;
	CHAR_DATA *v = NULL, *v_next = NULL;

	if ( maneuver < 0 )
		return FALSE;

	for (v=char_list; v != NULL; v=v_next)
	{
		v_next = v->next;

		if ( !is_same_group(ch, v) )
			continue;

		++iGroup;

		if ( get_skill(v, gsn_maneuvers ) > 0 )
			iSkilled++;

		if ( IS_SET(maneuver_table[maneuver].flags, REQUIRES_STR_18) && get_curr_stat (v, STAT_STR) < 18 )
		{
			if ( report )
			{
				send_to_char("Your a mite to week for that!\n\r", v);
				act("$n is a little too weak for that feat!", ch, NULL, v, TO_CHAR, POS_RESTING);
			}
			return FALSE;
		}

	}

	if ( iGroup < 2 )
	{
		if ( report )
			send_to_char("You'll need a few more hands for that.\n\r", ch);
		return FALSE;
	}

	if ( IS_SET(maneuver_table[maneuver].flags, REQUIRES_SKILLED_HELP) && iSkilled < 2 )
	{
		if ( report )
			send_to_char("You'll need skilled help for that.\n\r", ch);

		return FALSE;
	}

	if ( IS_SET(maneuver_table[maneuver].flags, REQUIRES_FOUR_OR_MORE) && iGroup < 4 )
	{
		if ( report )
			send_to_char("You'll need a few more hands for that.\n\r", ch);
		return FALSE;
	}

	if ( IS_SET(maneuver_table[maneuver].flags, REQUIRES_MOVEMENTS) && ch->move < 4 )
	{
		if ( report )
			send_to_char("You'll need to rest a bit first.\n\r", ch);

		return FALSE;
	}

	if ( IS_SET(maneuver_table[maneuver].type, PROJECTILES_ONLY) )
	{
		OBJ_DATA *pObj;

		if ( !vch )
			return FALSE;

		pObj = get_eq_char(vch, WEAR_WIELD);

		if ( pObj == NULL || pObj->value[0] != WEAPON_BOW )
			return FALSE;
	}
	
	return TRUE;
}

int lookup_maneuver(char *maneuver)
{
	int i;

	for (i=0; !IS_NULLSTR(maneuver_table[i].name); i++)
		if ( !str_prefix(maneuver_table[i].name, maneuver) )
			return i;

	return -1;

}


/* Function: execute_maneuver
 * Purpose : Contains the code for executing each of the various types of maneuvers
 * Note    : Does no error checking, or validating of maneuvers, victims, etc. This
 *         : must all be done elsewhere.
 */
bool execute_maneuver(CHAR_DATA *ch, CHAR_DATA *vch, int maneuver, char *argument)
{
	

	if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_MOUNTS) && !MOUNTED(vch) )
	{
		act("You look around for $N's mount, but cant seem to find it!", 
			ch, NULL, vch, POS_RESTING, TO_CHAR);

		return FALSE;
	}

	if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_OPPONENTS_WEAPON) &&
		get_eq_char(vch, WEAR_WIELD) == NULL )	
	{
		act("You look around for $N's weapon, but cant seem to find it!",
			ch, NULL, vch, POS_RESTING, TO_CHAR);
		return FALSE;
	}


	// roll the dice...
	if ( !make_skill_roll(get_skill(ch, gsn_maneuvers), maneuver_table[maneuver].difficulty) )
	{
		act("You fumble the maneuver.", 
			ch, NULL, NULL, POS_RESTING, TO_CHAR);
		act("$n attempts a complicated maneuver, and fails.",  
			ch, NULL, NULL, POS_RESTING, TO_ROOM);
		return FALSE;
	}


	act(maneuver_table[maneuver].toChar, ch, NULL, vch, POS_RESTING, TO_CHAR);
	act(maneuver_table[maneuver].toVict, ch, NULL, vch, POS_RESTING, TO_VICT);
	act(maneuver_table[maneuver].toRoom, ch, NULL, vch, POS_RESTING, TO_ROOM);

	// note; We are responsible for showing the damage_message, and
	// affecting the proper targets hitpoints/condition. damage() takes
	// care of everything else for us.
	if ( IS_SET(maneuver_table[maneuver].flags, OFFENSIVE_ONLY) )
	{
		int dam = 0;

		if ( vch->mount && IS_SET(maneuver_table[maneuver].type, DAMAGE_MOUNTS) )
		{
			vch = vch->mount;
		}

		dam = ch->level + get_curr_stat(ch, STAT_STR);
		dam += GET_DAMROLL (ch) * UMIN (100, get_skill(ch, gsn_maneuvers)) / 100;

		if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_OPPONENTS_WEAPON) )
		{
			OBJ_DATA *wield = get_eq_char(vch, WEAR_WIELD);

			// bloody Visual C.... is forcing me to cast damage here to a sh_int.
			if ( wield != NULL )
				wield->condition = UMIN(0, wield->condition - (sh_int)damage);
		}
		else
			vch->hit = UMIN(0, vch->hit - (sh_int)damage);

		damage(ch, vch, dam, gsn_maneuvers, TYPE_UNDEFINED, TRUE, TRUE);

	}
	
	if ( IS_SET(maneuver_table[maneuver].type, COMBAT_HALT) )
		stop_fighting(ch, TRUE);

	// maneuver successful. ;)

	/*
	if ( IS_SET(maneuver_table[maneuver].type, REDUCE_DAMAGE_BY_HALF) )
	if ( IS_SET(maneuver_table[maneuver].type, REDUCE_DAMAGE_BY_THIRD) )
	if ( IS_SET(maneuver_table[maneuver].type, REDUCE_DAMAGE_BY_TENTH) )
	if ( IS_SET(maneuver_table[maneuver].type, REDUCE_DAMAGE_BY_ALL) )
	if ( IS_SET(maneuver_table[maneuver].type, REVERSE_DAMAGE) )
	if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_OPPONENTS_WEAPON) )
	if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_OPPONENTS) ) // amt of damage, to opponent
	if ( IS_SET(maneuver_table[maneuver].type, DAMAGE_MOUNTS) )
	if ( IS_SET(maneuver_table[maneuver].type, CANCEL_RESCUE) )
	if ( IS_SET(maneuver_table[maneuver].type, COVER_PARTY) )
	if ( IS_SET(maneuver_table[maneuver].type, GROUPED_DMG) )
	if ( IS_SET(maneuver_table[maneuver].type, CANCEL_FLEE) )
	if ( IS_SET(maneuver_table[maneuver].type, MISSED_TRIP) )
	*/

	return TRUE;
}
