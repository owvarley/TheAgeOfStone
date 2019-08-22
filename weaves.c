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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

_WEAVE (weave_skim)
// 13Aug00 link & skill code added
{
  CHAR_DATA *victim;
  OBJ_DATA *portal;
  char arg1[MSL];
  int calc_level;

  one_argument (arg, arg1);

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  (LPA(ch, ALL) / 10) + 3 +
	  level * GET_SKILL (ch, find_spell (ch, "skim")) / 100;
    
  if ((victim = get_char_world (ch, arg1)) == NULL
      || victim == ch
      || victim->in_room == NULL
      || !can_see_room (ch, victim->in_room)
      || IS_SET (victim->in_room->room_flags, ROOM_SAFE)
      || IS_SET (victim->in_room->room_flags, ROOM_PRIVATE)
      || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY)
      || IS_SET (victim->in_room->room_flags, ROOM_NO_RECALL)
      || IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL)
      || victim->level >= calc_level 
	  || (!IS_NPC (victim) && victim->level >= LEVEL_HERO))	/* NOT trust */
  {
    send_to_char ("You failed.\n\r", ch);
    return;
  }

  if (IS_SET (victim->in_room->area->area_flags, AREA_UNUSED)
      || IS_SET (victim->in_room->area->area_flags, AREA_CLOSED))
  {
    send_to_char ("That area is off limits right now.\n\r", ch);
    return;
  }

  portal = create_object (get_obj_index (OBJ_VNUM_PORTAL), 0);

  if (!portal)
    return;

  portal->timer = calc_level / 25;
  portal->value[3] = victim->in_room->vnum;
  portal->level = calc_level;

  obj_to_room (portal, ch->in_room);

  act ("$p rises up from the ground.", ch, portal, NULL, TO_ROOM,
       POS_RESTING);
  act ("$p rises up before you.", ch, portal, NULL, TO_CHAR, POS_RESTING);
}


_WEAVE (weave_armor)
// 9Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn))
  {
    if (victim == ch)
      send_to_char ("You are already armored.\n\r", ch);
    else
      act ("$N is already armored.", ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  if (is_obj || IS_NPC(ch))
  {
    af.duration = level / 10;
	af.modifier = -20;
  }
  else
  {
    af.duration = LPA(ch, AIR);
    af.modifier = ((GET_SKILL (ch, find_spell (ch, "armor")) / -10)
                 * LPA(ch, AIR)) - LPA(ch, AIR);
  }
  af.location = APPLY_AC;
  af.bitvector = 0;
  af.owner = GET_ID(ch);
  affect_to_char (victim, &af);
  send_to_char ("You feel someone protecting you.\n\r", victim);
  if (ch != victim)
    act ("$N is protected by your weave.", ch, NULL, victim, TO_CHAR,
	 POS_RESTING);
  return;
}


_WEAVE (weave_blindness)
// 12Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;
  int iSkill = skill_lookup("blindness");

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = (LPA(ch, AIR) / 3) + level * GET_SKILL (ch, iSkill) / 100;

  if (IS_AFFECTED (victim, _AFF_BLIND) || saves_spell (calc_level, victim, DAM_WEAVE))
    return;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.location = APPLY_HITROLL;
  af.modifier = calc_level / -10;
  af.duration = 1 + calc_level;
  af.bitvector = _AFF_BLIND;
  af.owner = GET_ID(ch);
  affect_to_char (victim, &af);
  send_to_char ("You are blinded!\n\r", victim);
  act ("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
  return;
}


_WEAVE (weave_call_lightning)
// 12Aug00 link & skill code added
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  int dam, calc_level;

  if (!IS_OUTSIDE (ch))
  {
    send_to_char ("You must be out of doors.\n\r", ch);
    return;
  }

  if (weather_info.sky < SKY_RAINING)
  {
    send_to_char ("You need bad weather.\n\r", ch);
    return;
  }

  if (is_obj || IS_NPC(ch))
  {
	calc_level = level / 2;
    dam = dice (calc_level, 8);
  }
  else
  {
    calc_level = (LPA(ch, ALL) / 10) +
	  level * GET_SKILL (ch, find_spell (ch, "call lightning")) / 100;
    dam = dice (calc_level / 2, (LPA(ch, ALL) / 2));
  }
  send_to_char ("The Creator's lightning strikes your foes!\n\r", ch);
  act ("$n calls The Creator's lightning to strike $s foes!",
       ch, NULL, NULL, TO_ROOM, POS_RESTING);

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;

    if (vch->in_room == NULL)
      continue;

    if (vch->in_room == ch->in_room && in_same_world(ch, vch))
    {
      if (vch != ch && (IS_NPC (ch) ? !IS_NPC (vch) : IS_NPC (vch)) && in_same_world(ch, vch))
      {
  	    damage (ch, vch, (saves_spell (calc_level, vch, DAM_LIGHTNING)
			  ? dam / 2 : dam), sn, DAM_LIGHTNING, TRUE, TRUE);
	    shock_effect (vch, calc_level / 2, dam, TARGET_CHAR, 0);
      }
      continue;
    }
    if (vch->in_room->area == ch->in_room->area
	&& IS_OUTSIDE (vch) && IS_AWAKE (vch))
      send_to_char ("Lightning flashes in the sky.\n\r", vch);
  }

  return;
}

/* RT calm spell stops all fighting in the room */
_WEAVE (weave_calm)
// 13Aug00 link & skill code added
{
  CHAR_DATA *vch;
  int mlevel = 0;
  int count = 0;
  int high_level = 0;
  int chance, calc_level;
  AFFECT_DATA af;


  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = (LPA(ch, ALL) / 10) +
	  level * GET_SKILL (ch, find_spell (ch, "calm")) / 100;

  /* get sum of all mobile levels in the room */
  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch->position == POS_FIGHTING)
    {
      count++;
      if (IS_NPC (vch))
	    mlevel += vch->level;
      else
	    mlevel += vch->level / 2;
      high_level = UMAX (high_level, vch->level);
    }
  }

  /* compute chance of stopping combat */
  chance = 4 * calc_level - high_level + 2 * count;

  if (IS_IMMORTAL (ch))		/* always works */
    mlevel = 0;

  if (number_range (0, chance) >= mlevel)	/* hard to stop large fights */
  {
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
      if (IS_NPC (vch) && (IS_FLAG (vch->states.imm, _ONEPOWER)))
	return;

      if (IS_AFFECTED (vch, _AFF_CALM) || IS_AFFECTED (vch, _AFF_BERSERK)
	  || is_affected (vch, gsn_frenzy))
	return;

      send_to_char ("A wave of calm passes over you.\n\r", vch);

      if (vch->fighting || vch->position == POS_FIGHTING)
	stop_fighting (vch, FALSE);


      af.where = TO_AFFECTS;
      af.type = sn;
      af.level = calc_level;
      af.duration = calc_level / 4;
      af.location = APPLY_HITROLL;
      af.modifier = calc_level / -8;
      af.bitvector = _AFF_CALM;
      af.owner = GET_ID(ch);
      affect_to_char (vch, &af);

      af.location = APPLY_DAMROLL;
      affect_to_char (vch, &af);
    }
  }
}


_WEAVE (weave_chain_lightning)
// 13Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *tmp_vict, *last_vict, *next_vict;
  bool found;
  int dam, calc_level;
  
  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = (LPA(ch, ALL) / 10) +
	  level * GET_SKILL (ch, find_spell (ch, "chain lightning")) / 100;
  
  /* first strike */

  act ("A lightning bolt leaps from $n's hand and arcs to $N.",
       ch, NULL, victim, TO_ROOM, POS_RESTING);
  act ("A lightning bolt leaps from your hand and arcs to $N.",
       ch, NULL, victim, TO_CHAR, POS_RESTING);
  act ("A lightning bolt leaps from $n's hand and hits you!",
       ch, NULL, victim, TO_VICT, POS_DEAD);

  dam = dice (calc_level, calc_level/5);
  if (saves_spell (calc_level, victim, DAM_LIGHTNING))
    dam /= 2;
  else
    shock_effect (victim, calc_level / 3, dam, TARGET_CHAR, 0);
  damage (ch, victim, dam, sn, DAM_LIGHTNING, TRUE, TRUE);
  last_vict = victim;
  calc_level -= 4;			/* decrement damage */

  /* new targets */
  while (calc_level > 0)
  {
    found = FALSE;
    for (tmp_vict = ch->in_room->people;
	 tmp_vict != NULL; tmp_vict = next_vict)
    {
      next_vict = tmp_vict->next_in_room;
     }				/* end target searching loop */

    if (!found)			/* no target found, hit the caster */
    {
      if (ch == NULL)
	    return;
      if (last_vict == ch)	/* no double hits */
	  {
  	    act ("The bolt seems to have fizzled out.", ch, 
		  NULL, NULL, TO_ROOM, POS_RESTING);
        act ("The bolt grounds out through your body.", ch, NULL, NULL,
	      TO_CHAR, POS_RESTING);
	    return;
	  }
      last_vict = ch;
      act ("The bolt arcs to $n...whoops!", ch, NULL, NULL, TO_ROOM,
        POS_RESTING);
      send_to_char ("You are struck by your own lightning!\n\r", ch);
      dam = dice (calc_level, calc_level/5);
      if (saves_spell (calc_level, ch, DAM_LIGHTNING))
	    dam /= 2;
      damage (ch, ch, dam, sn, DAM_LIGHTNING, TRUE, TRUE);
      level -= 4;		/* decrement damage */
      if (ch == NULL)
	    return;
	}
    /* now go back and find more targets */
  }
}

_WEAVE (weave_sever)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int skill;
  AFFECT_DATA *paf;
  sh_int sex;

  if (!victim)
  {
    send_to_char ("Who would you like to sever?\n\r", ch);
    return;
  }

  if (is_safe (ch, victim))
    return;

  if (victim == ch)
  {
    send_to_char ("Why would you want to sever your own link to"
		  " the One Power?\n\r", ch);
    return;
  }
// temp ban till this weave is fixed up.
  else
  {
    send_to_char ("You failed.\n\r", ch);
    return;
  }

  sex = TRUESEX (victim);

  if (sex == SEX_MALE)
    skill = skill_lookup("seize");
  else
    skill = gsn_embrace;

  for (paf = victim->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type == skill)
    {
      act ("$n has severed your link to the One Power!", ch, NULL, victim,
	   TO_VICT, POS_DEAD);
      if (ch != victim)
	act ("You have severed $N's link to the One Power!", ch, NULL, victim,
	     TO_CHAR, POS_RESTING);
      affect_remove (victim, paf);
      return;
    }
  }

  send_to_char ("You failed.\n\r", ch);
  return;
}

_WEAVE (weave_slice)
{
	CHAR_DATA *victim = NULL;
	bool fAll = FALSE;
	int skill = -1;
	char arg1[MIL];
	char arg2[MIL];
	
	arg = one_argument (arg, arg1);
	one_argument (arg, arg2);
	
	/* possible arguments:
	
	 *  (none)            victim=ch, fAll = true
	 *  all               victim=ch, fAll = true
	 *  skill             victim=ch, fAll = false
	 *  affects           victim=ch, fAll = false
	 *  player            victim=player, fAll = true
	 *  player all        victim=player, fAll = true
	 *  palyer skill      victim=player, fAll = false
	 *  affects           victim=player, fAll = false
	 */
	
	
	if (arg1[0] == '\0' || !str_cmp (arg1, "all"))
	{
		victim = ch;
		fAll = TRUE;
	}
	else
	{
		skill = skill_lookup(arg1);
		
		if ( IS_VALID_SKILL(skill) )
		{
			victim = ch;
		}
	}
	
	if (victim == NULL)
	{
		if ((victim = get_char_room (ch, arg1)) == NULL)
		{
			send_to_char ("Slice what?\n\r", ch);
			return;
		}
		
		/* from here on, we have a valid victim */
		
		if (arg2[0] == '\0' || !str_cmp (arg2, "all"))
			fAll = TRUE;
		else
		{
			skill = skill_lookup(arg2);
			
			if (!IS_VALID_SKILL(skill))
			{
				send_to_char ("Slice who's what?\n\r", ch);
				return;
			}
		}
		
	}
	
	if (skill == skill_lookup("poison") ||
		skill == skill_lookup("plague") ||
		skill == skill_lookup ("sever") ||
		skill == gsn_berserk ||
		skill == gsn_sneak ||
		skill == skill_lookup("blood") ||
		skill == gsn_cloak ||
		skill == gsn_hide ||
		skill == gsn_warfare ||
		skill == gsn_martyrdom ||
		skill == gsn_feral ||
		skill == gsn_wrath ||
		skill == gsn_treat)
	{
		send_to_char ("You can't slice through that!\n\r", ch);
		return;
	}
	
	if (!fAll && (!IS_VALID_SKILL(skill) || !is_affected (victim, skill)))
	{
		if (ch == victim)
			send_to_char ("You dont seem to be affected by any such weave.\n\r",
			ch);
		else
			chprintf (ch,
			"%s dosen't seem to be affected by any such weave.\n\r",
			PERS (victim, ch));
		return;
	}
	else
		
	{	/* See if we can get rid of the weave or not. */
		
		int chance = 0;
		
		if (ch == victim)
			chance = 100;
		else
			if (OPR (ch) > OPR (victim))
				chance = UMIN (50 + (OPR (ch) - OPR (victim)), 100);
			else
				chance = 50 - (OPR (victim) - OPR (ch));
			
			if (!fAll)
			{
				if (number_range (1, 100) <= chance)
				{
					affect_strip (victim, skill);
					send_to_char ("You slice through the weave.\n\r", ch);
					
					if (ch != victim)
						chprintf (victim, "%s slices through your %s weave!\n\r",
						IS_NPC (ch) ? ch->short_descr :
					PERS (ch, victim), skill_table[skill].name);
					return;
				}
				else
				{
					send_to_char ("You failed.\n\r", ch);
					return;
				}
			}
			else
				
			{
				AFFECT_DATA *af;
				AFFECT_DATA *af_next;
				bool success = FALSE;
				
				for (af = victim->affected; af != NULL; af = af_next)
				{
					af_next = af->next;
					
					if (af->type != gsn_seize &&
						af->type != gsn_embrace &&
						af->type != skill_lookup ("sever") &&
						af->type != skill_lookup("poison") &&
						af->type != gsn_treat &&
						af->type != gsn_wrath &&
						af->type != gsn_cloak &&
						af->type != gsn_warfare &&
						af->type != gsn_martyrdom &&
						af->type != gsn_feral &&
						af->type != skill_lookup("blood") &&
						af->type != gsn_berserk &&
						af->type != gsn_sneak &&
						af->type != gsn_hide &&
						af->type != skill_lookup("plague") &&
						number_range (1, 100) <= chance)
					{
						success = TRUE;
						
						if (af->type > 0 && skill_table[af->type].msg_off)
							chprintf (victim, "%s\n\r", skill_table[af->type].msg_off);
						else
							send_to_char ("One of your weaves has been sliced away!\n\r",
							victim);
						
						/* important: dont access af after call to affect_remove */
						affect_remove (victim, af);
					}
				}
				if (!success)
					send_to_char ("You failed.\n\r", ch);
				else
					send_to_char ("You slice cleanly through!\n\r", ch);
			}
	}
}

_WEAVE (weave_compell)
// 10Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_safe (ch, victim))
    return;

  if (victim == ch)
  {
    send_to_char ("You like yourself even better!\n\r", ch);
    return;
  }

  calc_level = level + LPA(ch,SPIRIT) * 100 / GET_SKILL(ch, skill_lookup("compell")); 

  if (IS_AFFECTED (victim, _AFF_COMPULSION)
      || IS_AFFECTED (ch, _AFF_COMPULSION)
      || LPA(ch,ALL) < LPA (victim,ALL)
      || IS_FLAG (victim->states.imm, _COMPULSION)
      || saves_spell (calc_level, victim, DAM_WEAVE))
    return;

  if (victim->master)
    stop_follower (victim);

  add_follower (victim, ch, TRUE);
  victim->leader = ch;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = number_fuzzy (calc_level / 4);
  af.location = 0;
  af.modifier = 0;
  af.bitvector = _AFF_COMPULSION;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  act ("Wouldn't you like to please $n?", ch, NULL, victim, TO_VICT,
       POS_DEAD);

  if (ch != victim)
    act ("$N looks at you obediently.", ch, NULL, victim, TO_CHAR,
	 POS_RESTING);
  return;
}


_WEAVE (weave_shielding)
// 10Sep00 new weave
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_safe (ch, victim))
    return;
  
  if (victim == ch)
  {
    send_to_char ("You feel a chil run down your spine!\n\r", ch);
    return;
  }

  if (is_affected (victim, skill_lookup("seize")) ||
	  is_affected (victim, gsn_embrace))
  {
	if (IS_NPC(victim))
      calc_level = level - 5 + 
	    LPA(ch,SPIRIT) * GET_SKILL(ch, skill_lookup("shielding")) / 250;
	else
	  calc_level = level - 
	    LPA(victim,ALL) * GET_SKILL(victim, skill_lookup("slice")) / 100 +
		LPA(ch,SPIRIT) * GET_SKILL(ch, skill_lookup("shielding")) / 250;
  }
  else
    calc_level = level + 
	  LPA(ch,SPIRIT) * GET_SKILL(ch, skill_lookup("shielding")) / 50; 

  if (can_channel (victim) && (
	   IS_FLAG (victim->states.imm, _COMPULSION) ||
	   saves_spell (calc_level, victim, DAM_WEAVE) ||
	   ch->move * LPA(ch,ALL) / ch->max_move < 3 +
	     victim->move * LPA(victim,ALL) / victim->max_move))
  {
    send_to_char ("Your weave does not slide into place.\n\r", ch);
    send_to_char ("You feel a chill run down your spine!\n\r", victim);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_SHIELDING))
  {
    send_to_char ("They are already shielded.\n\r", ch);
    return;
  }

  affect_strip (victim, gsn_embrace);
  affect_strip (victim, skill_lookup("seize"));  
  
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = number_fuzzy (calc_level / 4);
  af.location = 0;
  af.modifier = 0;
  af.bitvector = _AFF_SHIELDING;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("A shielding of `Ws`wp`Wir`Wi`wt`w slides into place!\n\r", victim);
  send_to_char ("Your shielding of `Ws`wp`Wir`wi`Wt`w slides into place!\n\r", ch);
  return;
}


_WEAVE (weave_pain)
// 13Aug00 link & skill code added
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = LPA(ch, SPIRIT) +
	  level * GET_SKILL (ch, skill_lookup("pain")) / 100;
  
  if (saves_spell (calc_level, victim, DAM_WEAVE))
  {
    send_to_char ("You feel a momentary twinge, but it passes.", victim);
    return;
  }

  if (!IS_NPC(ch) && ch->pcdata->talents[gtn_spirit])
    damage (ch, (CHAR_DATA *) vo, dice (calc_level / 8, 15) + calc_level,
	  sn, DAM_HARM, TRUE, TRUE);
  else
    damage (ch, (CHAR_DATA *) vo, dice (calc_level / 10, 10) + calc_level - 3, 
	  sn, DAM_HARM, TRUE, TRUE);

  act ("You have been harmed by $n!!", ch, NULL, victim, TO_VICT, POS_DEAD);
  act ("$N flinches in pain as he is harmed by your weave.", ch, NULL, victim,
       TO_CHAR, POS_RESTING);

  if (IS_AFFECTED (victim, _AFF_PAINED))
    return;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = number_fuzzy (calc_level / 4);
  af.location = 0;
  af.modifier = 0;
  af.bitvector = _AFF_PAINED;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  return;
}



_WEAVE (weave_continual_light)
{
  OBJ_DATA *light;
  char arg1[MSL];

  one_argument (arg, arg1);

  if (arg1[0] != '\0')		/* do a glow on some object */
  {
    light = get_obj_carry (ch, arg1, ch);

    if (light == NULL)
    {
      send_to_char ("You don't see that here.\n\r", ch);
      return;
    }

    if (IS_OBJ_STAT (light, ITEM_GLOW))
    {
      act ("$p is already glowing.", ch, light, NULL, TO_CHAR, POS_RESTING);
      return;
    }

    SET_BIT (light->extra_flags, ITEM_GLOW);
    act ("$p glows with a white light.", ch, light, NULL, TO_ALL,
	 POS_RESTING);
    return;
  }

  light = create_object (get_obj_index (OBJ_VNUM_LIGHT_BALL), 0);
  if (!light)
    return;
  obj_to_room (light, ch->in_room);
  act ("$n twiddles $s thumbs and $p appears.", ch, light, NULL, TO_ROOM,
       POS_RESTING);
  act ("You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR,
       POS_RESTING);
  return;
}


_WEAVE (weave_cloud_dancing)
// 13Aug00 link & skill code added
{
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = LPA(ch, ALL) +
	  level * GET_SKILL (ch, find_spell (ch, "cloud dancing")) / 100;

  if (!str_cmp (arg, "better"))
    weather_info.change += dice (calc_level / 3, 4);
  else if (!str_cmp (arg, "worse"))
    weather_info.change -= dice (calc_level / 3, 4);
  else
    send_to_char ("Do you want it to get better or worse?\n\r", ch);

  send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_bind)
// 13Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (victim == ch)
  {
    send_to_char ("For some reason, you dont think that's a good idea.\n\r",
		  ch);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_BOUND_AIR))
  {
    chprintf (ch, "%s is already bound!", PERS (victim, ch));
    return;
  }

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
  {
    calc_level =  LPA(ch, AIR) +
	  level * GET_SKILL (ch, find_spell (ch, "bind")) / 100;
    if (ch->pcdata->talents[gtn_air])
      level += ch->flow_str.current[AIR];
  }

  if (saves_spell (calc_level, victim, DAM_WEAVE))
  {
    if (can_channel (victim))
      chprintf (ch, "%s slices your weave in two!\n\r",
		      PERS (victim, ch));
    else
      chprintf (ch, "Your weave of air misses.\n\r");
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = calc_level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_BOUND_AIR;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  chprintf (ch, "Your weave of air surrounds %s.\n\r",
		  PERS (victim, ch));

  act ("Something wraps around your arms and legs, you can't move!",
       ch, NULL, victim, TO_VICT, POS_RESTING);
  act ("$N suddenly goes rigid, apparently unable to move.",
       ch, NULL, victim, TO_NOTVICT, POS_RESTING);

  stop_fighting (victim, TRUE);
  return;
}


_WEAVE (weave_gag)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (victim == ch)
  {
    send_to_char ("For some reason, you dont think that's a good idea.\n\r",
		  ch);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_GAG_AIR))
  {
    chprintf (ch, "%s is already gagged!", PERS (victim, ch));
    return;
  }

  if (saves_spell (level, victim, DAM_WEAVE))
  {
    if (can_channel (victim))
      chprintf (ch, "%s slices your weave in two!\n\r",
		      PERS (victim, ch));
    else
      chprintf (ch,
		      "You trip and fall, losing your concentration.\n\r");
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_GAG_AIR;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  chprintf (ch, "Your weave of air gags %s.\n\r", PERS (victim, ch));
  act ("Something slaps across your mouth, forcing it shut!",
       ch, NULL, victim, TO_VICT, POS_RESTING);
  act ("A band of ...something... wraps around $N's mouth.",
       ch, NULL, victim, TO_NOTVICT, POS_RESTING);
  return;
}

_WEAVE (weave_blind)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (victim == ch)
  {
    send_to_char ("For some reason, you dont think that's a good idea.\n\r",
		  ch);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_BLIND_AIR))
  {
    chprintf (ch, "%s is already blinded!", PERS (victim, ch));
    return;
  }

  if (saves_spell (level, victim, DAM_WEAVE))
  {
    if (can_channel (victim))
      chprintf (ch, "%s slices your weave in two!\n\r",
		      PERS (victim, ch));
    else
      chprintf (ch,
		      "You trip and fall, losing your concentration.\n\r");
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.location = APPLY_HITROLL;
  af.modifier = -4;
  af.duration = 1 + level;
  af.bitvector = _AFF_BLIND;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  chprintf (ch, "Your weave of air blinds %s.\n\r", PERS (victim, ch));
  act ("Something slaps across your eyes, and you are suddenly blind!",
       ch, NULL, victim, TO_VICT, POS_RESTING);
  act ("A band of ...something... suddenly covers $N's eyes.",
       ch, NULL, victim, TO_NOTVICT, POS_RESTING);

  stop_fighting (victim, FALSE);
  return;

}

_WEAVE (weave_create_light)
{
  OBJ_DATA *light;

  light = create_object (get_obj_index (OBJ_VNUM_LIGHT_BALL), 0);
  if (light == NULL)
  {
    bugf ("[weaves.c::create_light] NULL object. [%d]", OBJ_VNUM_LIGHT_BALL);
    send_to_char ("Bug: unable to create light.\n\r", ch);
    return;
  }

  light->timer = level;
  obj_to_char (light, ch);
  wear_obj (ch, light, TRUE);
  return;
}

_WEAVE (weave_flameblade)
{
  OBJ_DATA *sword;
  enum _WEAPON_CLASS wpn;

  if (get_skill (ch, gsn_dagger) >= get_skill (ch, gsn_sword))
  {
    sword = create_object (get_obj_index (OBJ_VNUM_DAGGER_FIRE), 0);
    wpn = WEAPON_DAGGER;
  }
  else
  {
    sword = create_object (get_obj_index (OBJ_VNUM_SWORD_FIRE), 0);
    wpn = WEAPON_SWORD;
  }

  if (sword == NULL)
  {
    send_to_char ("Bug with weave fire sword. Logged.\n\r", ch);
    bugf ("[weaves.c::weave_flame_blade] NULL object.");
    return;
  }

  sword->level = ch->level + 5;
  sword->value[0] = wpn;
  sword->value[1] = UMAX (3, ch->level / 10 + 2);
  sword->value[2] = UMAX (4, ch->level / 10 + 4);
  sword->value[3] = attack_lookup ("flame");
  sword->value[4] = (ch->level > (LEVEL_HERO / 2)) ?
    WEAPON_FLAMING | WEAPON_SHARP : WEAPON_FLAMING;
  sword->timer = ch->level * 2;

  obj_to_char (sword, ch);
  wear_obj (ch, sword, TRUE);
  return;
}

_WEAVE (weave_airblade)
{
  OBJ_DATA *sword;
  enum _WEAPON_CLASS wpn;

  if (get_skill (ch, gsn_dagger) >= get_skill (ch, gsn_sword))
  {
    sword = create_object (get_obj_index (OBJ_VNUM_DAGGER_AIR), 0);
    wpn = WEAPON_DAGGER;
  }
  else
  {
    sword = create_object (get_obj_index (OBJ_VNUM_SWORD_AIR), 0);
    wpn = WEAPON_SWORD;
  }

  if (sword == NULL)
  {
    send_to_char ("Bug with weave air sword. Logged.\n\r", ch);
    bugf ("[weaves.c::weave_airblade] NULL object.");
    return;
  }

  sword->level = ch->level + 5;
  sword->value[0] = wpn;
  sword->value[1] = UMAX (3, ch->level / 10 + 2);
  sword->value[2] = UMAX (4, ch->level / 10 + 4);
  sword->value[3] = attack_lookup ("chill");
  sword->value[4] = (ch->level > (LEVEL_HERO / 2)) ?
    WEAPON_FROST | WEAPON_SHARP : WEAPON_FROST;
  sword->timer = ch->level * 2;

  obj_to_char (sword, ch);
  wear_obj (ch, sword, TRUE);
  return;
}


_WEAVE (weave_restore_sight)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (!is_affected (victim, skill_lookup("blindness")))
  {
    if (victim == ch)
      send_to_char ("You aren't blind.\n\r", ch);
    else
      act ("$N doesn't appear to be blinded.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  if (check_dispel (level, victim, skill_lookup("blind")))
  {
    send_to_char ("Your vision returns!\n\r", victim);
    act ("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM,
	 POS_RESTING);
  }
  else
    send_to_char ("Weave failed.\n\r", ch);
}


_WEAVE (weave_cure_critical)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int heal;

  heal = dice (3, 8) + level - 6;

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  victim->hit = UMIN (victim->hit + heal, victim->max_hit);
  victim->wtimer = 0;

  update_pos (victim);

  if (is_affected (victim, gsn_treat))
    affect_strip (victim, gsn_treat);

  send_to_char ("You feel better!\n\r", victim);
  send_to_char ("Ok.\n\r", ch);

  return;
}

_WEAVE (weave_clear_disease)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (!is_affected (victim, skill_lookup("plague")))
  {
    if (victim == ch)
      send_to_char ("You aren't ill.\n\r", ch);
    else
      act ("$N doesn't appear to be diseased.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  if (check_dispel (level, victim, skill_lookup("plague")))
  {
    send_to_char ("Your sores vanish.\n\r", victim);
    act ("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM,
	 POS_RESTING);
  }
  else
    send_to_char ("Weave failed.\n\r", ch);
}


_WEAVE (weave_cure_light)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int heal;

  heal = dice (1, 8) + level / 3;

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  victim->hit = UMIN (victim->hit + heal, victim->max_hit);
  victim->wtimer = 0;

  update_pos (victim);

  if (is_affected (victim, gsn_treat))
    affect_strip (victim, gsn_treat);

  send_to_char ("You feel better!\n\r", victim);
  send_to_char ("Ok.\n\r", ch);

  return;
}


_WEAVE (weave_clear_poison)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (!is_affected (victim, skill_lookup("poison")))
  {
    if (victim == ch)
      send_to_char ("You aren't poisoned.\n\r", ch);
    else
      act ("$N doesn't appear to be poisoned.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  if (check_dispel (level, victim, skill_lookup("poison")))
  {
    send_to_char ("A warm feeling runs through your body.\n\r", victim);
    act ("$n looks much better.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
  }
  else
    send_to_char ("Weave failed.\n\r", ch);
}

_WEAVE (weave_cure_serious)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int heal;

  heal = dice (2, 8) + level / 2;

  if (victim == ch && !is_obj)
  {
    send_to_char ("You can't heal yourself!\n\r", ch);
    return;
  }

  victim->hit = UMIN (victim->hit + heal, victim->max_hit);
  victim->wtimer = 0;
  update_pos (victim);
  if (is_affected (victim, gsn_treat))
    affect_strip (victim, gsn_treat);

  send_to_char ("You feel better!\n\r", victim);
  send_to_char ("Ok.\n\r", ch);

  return;
}



_WEAVE (weave_detect_evil)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_DETECT_EVIL))
  {
    if (victim == ch)
      send_to_char ("You can already sense evil.\n\r", ch);
    else
      act ("$N can already detect evil.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_DETECT_EVIL;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your eyes tingle.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_detect_good)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_DETECT_GOOD))
  {
    if (victim == ch)
      send_to_char ("You can already sense good.\n\r", ch);
    else
      act ("$N can already detect good.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_DETECT_GOOD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your eyes tingle.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_detect_hidden)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_DETECT_HIDDEN))
  {
    if (victim == ch)
      send_to_char ("You are already as alert as you can be. \n\r", ch);
    else
      act ("$N can already sense hidden lifeforms.", ch, NULL, victim,
	   TO_CHAR, POS_RESTING);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = _AFF_DETECT_HIDDEN;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your awareness improves.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_detect_invis)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_DETECT_INVIS))
  {
    if (victim == ch)
      send_to_char ("You can already see invisible.\n\r", ch);
    else
      act ("$N can already see invisible things.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_DETECT_INVIS;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your eyes tingle.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_detect_weave)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_DETECT_WEAVE))
  {
    if (victim == ch)
      send_to_char ("You can already sense weaves.\n\r", ch);
    else
      act ("$N can already sense weaves.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.modifier = 0;
  af.location = APPLY_NONE;
  af.bitvector = _AFF_DETECT_WEAVE;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your eyes tingle.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}


_WEAVE (weave_detect_poison)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;

  if (obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD)
  {
    if (obj->value[3] != 0)
      send_to_char ("You smell poisonous fumes.\n\r", ch);
    else
      send_to_char ("It looks delicious.\n\r", ch);
  }
  else
  {
    send_to_char ("It doesn't look poisoned.\n\r", ch);
  }

  return;
}


bool leads_to (ROOM_INDEX_DATA * from, ROOM_INDEX_DATA * to)
{
  int dir = 0;

  if (from == to)
    return FALSE;

  for (dir = 0; dir < 6; dir++)
  {
    EXIT_DATA *pexit;

    pexit = from->exit[dir];

    if (pexit == NULL ||
	IS_SET (pexit->exit_info, EX_CLOSED) || (pexit->u1.to_room == NULL))
      continue;

    if (pexit->u1.to_room == to)
      return TRUE;
  }

  return FALSE;
}

_WEAVE (weave_bladestorm)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;

  send_to_char ("The air around you hardens into razor sharp strips!\n\r",
		ch);
  act ("$n hardens the air into razor sharp strips!", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;

    if (vch->in_room == NULL)
      continue;

    if ((vch->in_room == ch->in_room || leads_to (ch->in_room, vch->in_room)) && in_same_world(ch, vch))
    {
      if (vch != ch 
	  && !is_same_group (ch, vch))
      {
		damage (ch, vch, level + LPA(ch, AIR) * 4 +
		  dice (4, GET_SKILL (ch, find_spell (ch, "skim")))/5,
			  sn, DAM_WEAVE, TRUE, TRUE);
      }
      continue;
    }

  }

  return;
}


_WEAVE (weave_earthquake)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;

  send_to_char ("The earth trembles beneath your feet!\n\r", ch);
  act ("$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;

    if (vch->in_room == NULL)
      continue;

    if ((vch->in_room == ch->in_room || leads_to (ch->in_room, vch->in_room)) && in_same_world(ch, vch))
    {
      if (vch != ch
&& !is_same_group (ch, vch))
      {
	if (IS_AFFECTED (vch, _AFF_FLYING))
	{
	  damage (ch, vch, 0, sn, DAM_WEAVE, TRUE, TRUE);
	}
	else
	{
	  sh_int dam = 0;

	  if (!IS_NPC (ch) && ch->pcdata->talents[gtn_earth] > 0)
	    dam = (level + dice (4, 16) * 2) + (LPA(ch, EARTH) * 5);
	  else
	    dam = level + dice (4, 16) + LPA(ch, EARTH) * 3;
	  damage (ch, vch, dam, sn, DAM_EARTH, TRUE, TRUE);
	}
      }
      continue;
    }

    if (vch->in_room->area == ch->in_room->area)
      send_to_char ("The earth trembles and shivers.\n\r", vch);
  }

  return;
}


_WEAVE (weave_rolling_ring)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;

  send_to_char ("The earth erupts in stone and fire around you!\n\r", ch);
  act ("The earth erupts in stone and fire around $n!.", ch, NULL, NULL,
       TO_ROOM, POS_RESTING);

  for (vch = char_list; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next;

    if (vch->in_room == NULL)
      continue;

	/* We actually want this weave to affect players inside safe rooms,
     * but not let the channeler cast it from within a safe room. Also,
     * we dont want to affect shops, or trainers/healers/etc. 
     * Therefore, instead of 'is_safe', we check the following...
     */

    if (IS_SET (ch->in_room->room_flags, ROOM_SAFE) && target != TAR_INSANE)
    {
      send_to_char ("Not in this room.\n\r", ch);
      return;
    }

    /* no killing healers, trainers, etc */
    if (!in_same_world(ch, vch) 
	|| is_same_group (ch, vch) 
	|| (IS_NPC (vch) && 
	((IS_FLAG(vch->states.player, _ACT_TRAIN)
	|| IS_FLAG (vch->states.player, _ACT_PRACTICE)
	|| IS_FLAG (vch->states.player, _ACT_HEALER)
	|| IS_FLAG (vch->states.player, _ACT_BANKER)
	|| (vch->pIndexData->pShop != NULL)))))
    {
		continue;
    }

    if (vch->in_room == ch->in_room || leads_to (ch->in_room, vch->in_room))
    {
      if (vch != ch)
      {
	if (IS_AFFECTED (vch, _AFF_FLYING))
	{
	  damage (ch, vch, level / 2, sn, DAM_EARTH, TRUE, FALSE);
	  damage (ch, vch, level / 2, sn, DAM_FIRE, TRUE, FALSE);
	}
	else
	{
	  damage (ch, vch, level + dice (2, 8), sn, DAM_EARTH, TRUE, FALSE);
	  damage (ch, vch, level + dice (2, 8), sn, DAM_FIRE, TRUE, FALSE);
	}
      }
      continue;
    }

  }

  return;
}

_WEAVE (weave_align_armor)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf;
  int result, fail;
  int ac_bonus, added;
  bool ac_found = FALSE;

  if (obj->item_type != ITEM_ARMOR)
  {
    send_to_char ("That isn't an armor.\n\r", ch);
    return;
  }

  if (obj->wear_loc != -1)
  {
    send_to_char ("The item must be carried to be enchanted.\n\r", ch);
    return;
  }

  /* this means they have no bonus */
  ac_bonus = 0;
  fail = 25;			/* base 25% chance of failure */

  /* find the bonuses */

  if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_AC)
      {
	ac_bonus = paf->modifier;
	ac_found = TRUE;
	fail += 5 * (ac_bonus * ac_bonus);
      }

      else			/* things get a little harder */
	fail += 20;
    }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location == APPLY_AC)
    {
      ac_bonus = paf->modifier;
      ac_found = TRUE;
      fail += 5 * (ac_bonus * ac_bonus);
    }

    else			/* things get a little harder */
      fail += 20;
  }

  /* apply other modifiers */
  fail -= level;

  if (IS_OBJ_STAT (obj, ITEM_BLESS))
    fail -= 15;
  if (IS_OBJ_STAT (obj, ITEM_GLOW))
    fail -= 5;

  fail = URANGE (5, fail, 85);

  result = number_percent ();

  /* the moment of truth */
  if (result < (fail / 5))	/* item destroyed */
  {
    act ("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
	ch->move /= 2;
	ch->hit /= 2;
    extract_obj (obj);
    return;
  }

  if (result < (fail / 3))	/* item disenchanted */
  {
    AFFECT_DATA *paf_next;

    act ("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
    obj->enchanted = TRUE;

    /* remove all affects */
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      free_affect (paf);
    }
    obj->affected = NULL;

//    /* clear all flags */
//    obj->extra_flags = 0;
//    Phil said remove this

    return;

  }

  if (result <= fail)		/* failed, no bad result */
  {
    send_to_char ("Nothing seemed to happen.\n\r", ch);
    return;
  }

  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
    AFFECT_DATA *af_new;
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
      af_new->owner = GET_ID(ch);
    }
  }

  if (result <= (90 - level / 5))	/* success! */
  {
    act ("$p shimmers with a gold aura.", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p shimmers with a gold aura.", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    added = -1;
  }

  else				/* exceptional enchant */
  {
    act ("$p glows a brillant gold!", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$p glows a brillant gold!", ch, obj, NULL, TO_ROOM, POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    SET_BIT (obj->extra_flags, ITEM_GLOW);
    added = -2;
  }

  /* now add the enchantments */

  if (obj->level < LEVEL_HERO)
    obj->level = UMIN (LEVEL_HERO - 1, obj->level + 1);

  if (ac_found)
  {
    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_AC)
      {
	paf->type = sn;
	paf->modifier += added;
	paf->level = UMAX (paf->level, level);
      }
    }
  }
  else				/* add a new affect */
  {
    paf = new_affect ();

    paf->where = TO_OBJECT;
    paf->type = sn;
    paf->level = level;
    paf->duration = -1;
    paf->location = APPLY_AC;
    paf->modifier = added;
    paf->bitvector = 0;
    paf->next = obj->affected;
    paf->owner = GET_ID(ch);
    obj->affected = paf;
  }

}

_WEAVE (weave_steelcloth)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf;
  int result, fail;
  int ac_bonus, added;
  bool ac_found = FALSE;

  if (obj->item_type != ITEM_CLOTHING)
  {
    send_to_char ("That isn't clothing!\n\r", ch);
    return;
  }

  if (obj->wear_loc != -1)
  {
    send_to_char ("The item must be carried to be hardened.\n\r", ch);
    return;
  }

  if (!IS_OBJ_STAT (obj, ITEM_NONMETAL))
  {
    send_to_char ("This item cannot be hardened.\n\r", ch);
    return;
  }

  /* this means they have no bonus */
  ac_bonus = 0;
  fail = 25;			/* base 25% chance of failure */

  /* find the bonuses */

  if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_AC)
      {
	ac_bonus = paf->modifier;
	ac_found = TRUE;
	fail += 5 * (ac_bonus * ac_bonus);
      }

      else			/* things get a little harder */
	fail += 20;
    }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location == APPLY_AC)
    {
      ac_bonus = paf->modifier;
      ac_found = TRUE;
      fail += 5 * (ac_bonus * ac_bonus);
    }

    else			/* things get a little harder */
      fail += 20;
  }

  /* apply other modifiers */
  fail -= level;

  if (IS_OBJ_STAT (obj, ITEM_BLESS))
    fail -= 15;
  if (IS_OBJ_STAT (obj, ITEM_GLOW))
    fail -= 5;

  fail = URANGE (5, fail, 85);

  result = number_percent ();

  /* the moment of truth */
  if (result < (fail / 5))	/* item destroyed */
  {
    act ("$p becomes stiff... and cracks!", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p becomes stiff... and cracks!", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
	ch->hit = 1;			 // exploding in your face hurts
    extract_obj (obj);
    return;
  }

  if (result < (fail / 3))	/* item disenchanted */
  {
    AFFECT_DATA *paf_next;

    act ("$p stiffens momentarily, then relaxes...oops.", ch, obj, NULL,
	 TO_CHAR, POS_RESTING);
    act ("$p stiffens momentarily, then relaxes.", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
    obj->enchanted = TRUE;

    /* remove all affects */
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      free_affect (paf);
    }
    obj->affected = NULL;

//    /* clear all flags */
//    obj->extra_flags = 0;
//	  as per Phil this was removed

    return;

  }

  if (result <= fail)		/* failed, no bad result */
  {
    send_to_char ("Nothing seemed to happen.\n\r", ch);
    return;
  }

  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
    AFFECT_DATA *af_new;
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
      af_new->owner = GET_ID(ch);
    }
  }

  if (result <= (90 - level / 5))	/* success! */
  {
    act ("$p radiates a gold light.", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$p radiates a gold light.", ch, obj, NULL, TO_ROOM, POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    added = -1;
  }

  else				/* exceptional enchant */
  {
    act ("$p flares a brillant gold!", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$p flares a brillant gold!", ch, obj, NULL, TO_ROOM, POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    SET_BIT (obj->extra_flags, ITEM_GLOW);
    added = -2;
  }

  /* now add the enchantments */

  if (obj->level < LEVEL_HERO)
    obj->level = UMIN (LEVEL_HERO - 1, obj->level + 1);

  if (ac_found)
  {
    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_AC)
      {
	paf->type = sn;
	paf->modifier += added;
	paf->level = UMAX (paf->level, level);
      }
    }
  }
  else				/* add a new affect */
  {
    paf = new_affect ();

    paf->where = TO_OBJECT;
    paf->type = sn;
    paf->level = level;
    paf->duration = ch->level;
    paf->location = APPLY_AC;
    paf->modifier = added;
    paf->bitvector = 0;
    paf->next = obj->affected;
    paf->owner = GET_ID(ch);
    obj->affected = paf;
  }

}



_WEAVE (weave_align_weapon)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA *paf;
  int result, fail;
  int hit_bonus, dam_bonus, added;
  bool hit_found = FALSE, dam_found = FALSE;

  if (obj->item_type != ITEM_WEAPON)
  {
    send_to_char ("That isn't a weapon.\n\r", ch);
    return;
  }

  if (obj->wear_loc != -1)
  {
    send_to_char ("The item must be carried to be enchanted.\n\r", ch);
    return;
  }

  /* this means they have no bonus */
  hit_bonus = 0;
  dam_bonus = 0;
  fail = 25;			/* base 25% chance of failure */

  /* find the bonuses */

  if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_HITROLL)
      {
	hit_bonus = paf->modifier;
	hit_found = TRUE;
	fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL)
      {
	dam_bonus = paf->modifier;
	dam_found = TRUE;
	fail += 2 * (dam_bonus * dam_bonus);
      }

      else			/* things get a little harder */
	fail += 25;
    }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location == APPLY_HITROLL)
    {
      hit_bonus = paf->modifier;
      hit_found = TRUE;
      fail += 2 * (hit_bonus * hit_bonus);
    }

    else if (paf->location == APPLY_DAMROLL)
    {
      dam_bonus = paf->modifier;
      dam_found = TRUE;
      fail += 2 * (dam_bonus * dam_bonus);
    }

    else			/* things get a little harder */
      fail += 25;
  }

  /* apply other modifiers */
  fail -= 3 * level / 2;

  if (IS_OBJ_STAT (obj, ITEM_BLESS))
    fail -= 15;
  if (IS_OBJ_STAT (obj, ITEM_GLOW))
    fail -= 5;

  fail = URANGE (5, fail, 95);

  result = number_percent ();

  /* the moment of truth */
  if (result < (fail / 5))	/* item destroyed */
  {
    act ("$p shivers violently and explodes!", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p shivers violently and explodeds!", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
	ch->hit = 1;
	ch->move = 1;
	extract_obj (obj);
    return;
  }

  if (result < (fail / 2))	/* item disenchanted */
  {
    AFFECT_DATA *paf_next;

    act ("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    act ("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM,
	 POS_RESTING);
    obj->enchanted = TRUE;

    /* remove all affects */
    for (paf = obj->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      free_affect (paf);
    }
    obj->affected = NULL;

//    /* clear all flags */
//    obj->extra_flags = 0;
//    As per Phil, don't remove the flags.

    return;
  }

  if (result <= fail)		/* failed, no bad result */
  {
    send_to_char ("Nothing seemed to happen.\n\r", ch);
    return;
  }

  /* okay, move all the old flags into new vectors if we have to */
  if (!obj->enchanted)
  {
    AFFECT_DATA *af_new;
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

  if (result <= (100 - level / 5))	/* success! */
  {
    act ("$p glows blue.", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$p glows blue.", ch, obj, NULL, TO_ROOM, POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    added = 1;
  }

  else				/* exceptional enchant */
  {
    act ("$p glows a brillant blue!", ch, obj, NULL, TO_CHAR, POS_RESTING);
    act ("$p glows a brillant blue!", ch, obj, NULL, TO_ROOM, POS_RESTING);
    SET_BIT (obj->extra_flags, ITEM_MAGIC);
    SET_BIT (obj->extra_flags, ITEM_GLOW);
    added = 2;
  }

  /* now add the enchantments */

  if (obj->level < LEVEL_HERO - 1)
    obj->level = UMIN (LEVEL_HERO - 1, obj->level + 1);

  if (dam_found)
  {
    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_DAMROLL)
      {
	paf->type = sn;
	paf->modifier += added;
	paf->level = UMAX (paf->level, level);
	if (paf->modifier > 4)
	  SET_BIT (obj->extra_flags, ITEM_HUM);
      }
    }
  }
  else				/* add a new affect */
  {
    paf = new_affect ();

    paf->where = TO_OBJECT;
    paf->type = sn;
    paf->level = level;
    paf->duration = -1;
    paf->location = APPLY_DAMROLL;
    paf->modifier = added;
    paf->bitvector = 0;
    paf->next = obj->affected;
    paf->owner = GET_ID(ch);
    obj->affected = paf;

  }

  if (hit_found)
  {
    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_HITROLL)
      {
	paf->type = sn;
	paf->modifier += added;
	paf->level = UMAX (paf->level, level);
	if (paf->modifier > 4)
	  SET_BIT (obj->extra_flags, ITEM_HUM);
      }
    }
  }
  else				/* add a new affect */
  {
    paf = new_affect ();

    paf->type = sn;
    paf->level = level;
    paf->duration = -1;
    paf->location = APPLY_HITROLL;
    paf->modifier = added;
    paf->bitvector = 0;
    paf->next = obj->affected;
    paf->owner = GET_ID(ch);
    obj->affected = paf;
  }

}



/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
_WEAVE (weave_energy_drain)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  if (victim != ch)
    ch->alignment = UMAX (-1000, ch->alignment - 50);

  if (saves_spell (level, victim, DAM_NEGATIVE))
  {
    send_to_char ("You feel a momentary chill.\n\r", victim);
    return;
  }


  if (victim->level <= 2)
  {
    dam = ch->hit + 1;
  }
  else
  {
    gain_exp (victim, 0 - number_range (level / 2, 3 * level / 2));

    victim->move /= 2;
    dam = dice (1, level);
    ch->hit += dam;
  }

  send_to_char ("You feel your life slipping away!\n\r", victim);
  send_to_char ("Wow....what a rush!\n\r", ch);
  damage (ch, victim, dam, sn, DAM_NEGATIVE, TRUE, TRUE);

  return;
}


_WEAVE (weave_fireball)
// 21Aug00 link & skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  static const sh_int dam_each[] = {
    0,
    0, 0, 4, 6, 8, 10, 12, 14, 16, 18,
    20, 22, 24, 26, 30, 35, 40, 45, 50, 55,
    60, 65, 70, 75, 80, 82, 84, 86, 88, 90,
    92, 94, 96, 98, 100, 102, 104, 106, 108, 110,
    112, 114, 116, 118, 120, 122, 124, 126, 128, 130,
	135, 140, 145, 150, 155, 160, 165, 170, 175, 180
  };
  int dam;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level = LPA(ch, FIRE) +
	  level * GET_SKILL (ch, find_spell (ch, "fireball")) / 100;
  calc_level = UMIN (calc_level, 60);
  dam = number_range (dam_each[calc_level] / 2, dam_each[calc_level] * 2);

  if (saves_spell (calc_level, victim, DAM_FIRE))
    dam /= 2;

  if (!IS_NPC (ch) && !is_obj)
  {
	  if (ch->pcdata->talents[gtn_fire] > 0)
		  dam += LPA (ch, FIRE) * LPA (ch, FIRE);
	  else
		  dam += LPA (ch, FIRE);
  }
  damage (ch, victim, dam, sn, DAM_FIRE, TRUE, TRUE);
  return;
}

_WEAVE (weave_firebar)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  static const sh_int dam_each[] = {
    0,
    0, 0, 4, 6, 8, 10, 12, 14, 16, 18,
    20, 22, 24, 26, 30, 35, 40, 45, 50, 55,
    60, 65, 70, 75, 80, 82, 84, 86, 88, 90,
    92, 94, 96, 98, 100, 102, 104, 106, 108, 110,
    112, 114, 116, 118, 120, 122, 124, 126, 128, 130
  };
  int dam;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  (LPA(ch, ALL) / 10) +
	  level * GET_SKILL (ch, find_spell (ch, "firebar")) / 100;

  calc_level = UMIN (calc_level, sizeof (dam_each) / sizeof (dam_each[0]) - 1);
  calc_level = UMAX (0, calc_level);

  dam = number_range (dam_each[calc_level] / 2, 
	                  dam_each[calc_level] * 2) * 1.5;

  if (saves_spell (calc_level, victim, DAM_FIRE))
    dam /= 2;

  if (!IS_NPC (ch) && ch->pcdata->talents[gtn_fire] > 0)
    dam += LPA(ch, FIRE) * 8;
  else
    dam += LPA(ch, FIRE) * 4;

  damage (ch, victim, dam, sn, DAM_FIRE, TRUE, TRUE);
  return;
}

_WEAVE (weave_ward_fire)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  AFFECT_DATA af;

  if (IS_OBJ_STAT (obj, ITEM_BURN_PROOF))
  {
    act ("$p is already protected from burning.", ch, obj, NULL, TO_CHAR,
	 POS_RESTING);
    return;
  }

  af.where = TO_OBJECT;
  af.type = sn;
  af.level = level;
  af.duration = number_fuzzy (level / 4);
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = ITEM_BURN_PROOF;

  affect_to_obj (obj, &af);

  act ("You ward $p from fire.", ch, obj, NULL, TO_CHAR, POS_RESTING);
  act ("$p is surrounded by a protective aura.", ch, obj, NULL, TO_ROOM,
       POS_RESTING);
}


_WEAVE (weave_flame_blast)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	CHAR_DATA *vch, *vch_next;
	int dam, hp_dam, dice_dam;
	int hpch;
	
	act ("$n shoots forth a cone of fire.", ch, NULL, victim, TO_NOTVICT,
		POS_RESTING);
	act ("$n shoots a cone of hot fire over you!", ch, NULL, victim, TO_VICT,
		POS_DEAD);
	act ("You shoot forth a cone of fire.", ch, NULL, NULL, TO_CHAR,
		POS_RESTING);
	
	hpch = UMAX (10, ch->hit);
	hp_dam = number_range (hpch / 9 + 1, hpch / 5);
	dice_dam = dice (level/2, 20);
	
	dam = UMAX (hp_dam + dice_dam / 10, dice_dam + hp_dam / 10);
	fire_effect (victim->in_room, level, dam / 2, TARGET_ROOM, GET_ID(ch));
	
	for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next_in_room;
		
		if (vch == victim)		/* full damage */
		{
			if (saves_spell (level, vch, DAM_FIRE))
			{
				fire_effect (vch, level / 2, dam / 4, TARGET_CHAR, GET_ID(ch));
				damage (ch, vch, dam / 2, sn, DAM_FIRE, TRUE, TRUE);
			}
			else
			{
				fire_effect (vch, level, dam, TARGET_CHAR, GET_ID(ch));
				damage (ch, vch, dam, sn, DAM_FIRE, TRUE, TRUE);
			}
		}
		else			/* partial damage */
		{
			if (saves_spell (level - 2, vch, DAM_FIRE))
			{
				fire_effect (vch, level / 4, dam / 8, TARGET_CHAR, GET_ID(ch));
				damage (ch, vch, dam / 4, sn, DAM_FIRE, TRUE, TRUE);
			}
			else
			{
				fire_effect (vch, level / 2, dam / 4, TARGET_CHAR, GET_ID(ch));
				damage (ch, vch, dam / 2, sn, DAM_FIRE, TRUE, TRUE);
			}
		}
	}
}


_WEAVE (weave_floating_disc)
{
  OBJ_DATA *disc, *floating;

  floating = get_eq_char (ch, WEAR_FLOAT);
  if (floating != NULL && IS_OBJ_STAT (floating, ITEM_NOREMOVE))
  {
    act ("You can't remove $p.", ch, floating, NULL, TO_CHAR, POS_RESTING);
    return;
  }

  disc = create_object (get_obj_index (OBJ_VNUM_DISC), 0);
  if (!disc)
    return;
  disc->value[0] = ch->level * 10;	/* 10 pounds per level capacity */
  disc->value[3] = ch->level * 5;	/* 5 pounds per level max per item */
  disc->timer = ch->level * 4 - number_range (0, level / 2);

  act ("$n has created a floating black disc.", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);
  send_to_char ("You create a floating disc.\n\r", ch);
  obj_to_char (disc, ch);
  wear_obj (ch, disc, TRUE);
  return;
}

_WEAVE (weave_fly)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_FLYING))
  {
    if (victim == ch)
      send_to_char ("You are already airborne.\n\r", ch);
    else
      act ("$N doesn't need your help to fly.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + 3;
  af.location = 0;
  af.modifier = 0;
  af.bitvector = _AFF_FLYING;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your feet rise off the ground.\n\r", victim);
  act ("$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);
  return;
}


/* RT clerical berserking spell */
_WEAVE (weave_frenzy)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn) || IS_AFFECTED (victim, _AFF_BERSERK))
  {
    if (victim == ch)
      send_to_char ("You are already in a frenzy.\n\r", ch);
    else
      act ("$N is already in a frenzy.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (is_affected (victim, skill_lookup ("calm")))
  {
    if (victim == ch)
      send_to_char ("Why don't you just relax for a while?\n\r", ch);
    else
      act ("$N doesn't look like $e wants to fight anymore.",
	   ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }

  if ((IS_GOOD (ch) && !IS_GOOD (victim)) ||
      (IS_NEUTRAL (ch) && !IS_NEUTRAL (victim)) ||
      (IS_EVIL (ch) && !IS_EVIL (victim)))
  {
    act ("Your god doesn't seem to like $N", ch, NULL, victim, TO_CHAR,
	 POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level / 3;
  af.modifier = level / 6;
  af.bitvector = 0;

  af.location = APPLY_HITROLL;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  af.location = APPLY_DAMROLL;
  affect_to_char (victim, &af);

  af.modifier = 10 * (level / 12);
  af.location = APPLY_AC;
  affect_to_char (victim, &af);

  send_to_char ("You are filled with holy wrath!\n\r", victim);
  act ("$n gets a wild look in $s eyes!", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);
}

_WEAVE (weave_travel)
{
  CHAR_DATA *victim;
  bool gate_pet;
  char arg1[MSL];

  one_argument (arg, arg1);

  if (((victim = get_char_world (ch, arg1)) == NULL)
      || victim == ch
      || victim->in_room == NULL
      || !can_see_room (ch, victim->in_room)
      || IS_SET (victim->in_room->room_flags, ROOM_NO_GATE)
      || IS_SET (victim->in_room->room_flags, ROOM_SAFE)
      || IS_SET (victim->in_room->room_flags, ROOM_PRIVATE)
      || IS_SET (victim->in_room->room_flags, ROOM_SOLITARY)
      || IS_SET (victim->in_room->room_flags, ROOM_NO_RECALL)
      || IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL)
      || (!IS_NPC (victim) && victim->level >= LEVEL_HERO)	/* NOT trust */
      || (IS_NPC (victim) && saves_spell (level, victim, DAM_WEAVE)))
  {
    send_to_char ("You failed.\n\r", ch);
    return;
  }

  if (IS_SET (victim->in_room->area->area_flags, AREA_UNUSED)
      || IS_SET (victim->in_room->area->area_flags, AREA_CLOSED))
  {
    send_to_char ("That area is off limits right now.\n\r", ch);
    return;
  }

  if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
    gate_pet = TRUE;
  else
    gate_pet = FALSE;

  act ("$n steps through a gateway and vanishes.", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);
  send_to_char ("You step through a gateway and vanish.\n\r", ch);
  char_from_room (ch);
  char_to_room (ch, victim->in_room);

  act ("$n has arrived through a gateway.", ch, NULL, NULL, TO_ROOM,
       POS_RESTING);
  do_look (ch, "auto");

  if (gate_pet)
  {
    act ("$n steps through a gate and vanishes.", ch->pet, NULL, NULL,
	 TO_ROOM, POS_RESTING);
    send_to_char ("You step through a gate and vanish.\n\r", ch->pet);
    char_from_room (ch->pet);
    char_to_room (ch->pet, victim->in_room);
    act ("$n has arrived through a gate.", ch->pet, NULL, NULL, TO_ROOM,
	 POS_RESTING);
    do_look (ch->pet, "auto");
  }

  if (MOUNTED (ch))
  {
    act ("$n steps through a gate and vanishes.",
	 MOUNTED (ch), NULL, NULL, TO_ROOM, POS_RESTING);
    send_to_char ("You step through a gate and vanish.\n\r", MOUNTED (ch));
    char_from_room (MOUNTED (ch));
    char_to_room (MOUNTED (ch), victim->in_room);
    act ("$n has arrived through a gate.", MOUNTED (ch), NULL, NULL, TO_ROOM,
	 POS_RESTING);
    do_look (MOUNTED (ch), "auto");
  }
}


_WEAVE (weave_enhance_strength)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn))
  {
    if (victim == ch)
      send_to_char ("You are already as strong as you can get!\n\r", ch);
    else
      act ("$N can't get any stronger.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_STR;
  af.modifier = 1 + (level >= 18) + (level >= 25) + (level >= 32);
  af.bitvector = 0;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("Your muscles surge with heightened power!\n\r", victim);
  act ("$n's muscles surge with heightened power.", victim, NULL, NULL,
       TO_ROOM, POS_RESTING);
  return;
}


/* RT haste spell */
_WEAVE (weave_haste)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn) || IS_AFFECTED (victim, _AFF_HASTE)
      || IS_FLAG (victim->states.off, _OFF_FAST))
  {
    if (victim == ch)
      send_to_char ("You can't move any faster!\n\r", ch);
    else
      act ("$N is already moving as fast as $E can.",
	   ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_SLOW))
  {
    if (!check_dispel (level, victim, skill_lookup ("slow")))
    {
      if (victim != ch)
	send_to_char ("Weave failed.\n\r", ch);
      send_to_char ("You feel momentarily faster.\n\r", victim);
      return;
    }
    act ("$n is moving less slowly.", victim, NULL, NULL, TO_ROOM,
	 POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  if (victim == ch)
    af.duration = level / 2;
  else
    af.duration = level / 4;
  af.location = APPLY_DEX;
  af.modifier = 1 + (level >= 18) + (level >= 25) + (level >= 32);
  af.bitvector = _AFF_HASTE;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("You feel yourself moving more quickly.\n\r", victim);
  act ("$n is moving more quickly.", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}



_WEAVE (weave_heal)
{
	sh_int heal;
	
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	
	//  if it is a mob doing the healing, or herbs, just heal a little,
	//  and don't remove treat and don't drain moves
	if (is_obj)
	{
		victim->hit = UMIN(victim->max_hit, victim->hit+50+dice (1, 50));
		victim->wtimer = 0;
		return;
	}
	else if (IS_NPC (ch))
	{
		victim->hit = UMIN(victim->max_hit,
			victim->hit + level + dice (2, level));
		victim->wtimer = 0;
		return;
	} 
	
	heal = UMIN (victim->hit + GET_SKILL (ch, find_spell (ch, "heal")) + 
		LPA(ch,ALL), victim->max_hit) - victim->hit;
	
	if (victim == ch)
	{
		chprintf(ch, "You can't heal yourself!\n\r");
		return;
	}
	
	victim->move = UMAX (victim->move - heal, 0);
	
	if (ch->pcdata->talents[gtn_healing] > 0)
	{
		heal = UMIN (victim->hit + 2 * heal, victim->max_hit) - victim->hit;
		
		if (is_affected (victim, skill_lookup("poison"))
			&& check_dispel (level, victim, skill_lookup("poison")))
		{
			chprintf(victim, "You feel the poison leaving your system.\n\r");
			chprintf(ch, "You sense the poison leaving %s's system.\n\r", PERS (victim, ch));
		}
		if (is_affected (victim, skill_lookup("plague"))
			&& check_dispel (level, victim, skill_lookup("plague")))
			act ("The sores covering $N begin to fade away.", ch, NULL, victim,
			TO_ROOM, POS_RESTING);
		
		if (is_affected (victim, skill_lookup("blind"))
			&& check_dispel (level, victim, skill_lookup("blind")))
		{
			chprintf(victim, "You can see again!\n\r");
			chprintf (ch, "%s can see again.\n\r", PERS (victim, ch));
		}
	}
	
	
	// those that dont require a talent, but require the power to heal...
	if (is_affected (victim, gsn_lacerate))
	{
		chprintf(victim, "Your wounds knit together!\n\r");
		chprintf (ch, "%s's wounds knit together.\n\r", PERS (victim, ch));
		affect_strip(victim, gsn_lacerate);
	}
	
	
	heal = UMIN (victim->hit + 3 * heal, victim->max_hit) - victim->hit;
	victim->hit += heal;
	victim->wtimer = 0;
	
	update_pos (victim);
	
	if (is_affected (victim, gsn_treat))
		affect_strip (victim, gsn_treat);
	
	if ( !IS_NPC(victim) )
		victim->pcdata->condition[_HUNGER] = 0;
	
	chprintf(victim, "The healing restores your body, but taxes it.  You should probably rest and eat.\n\r");
	chprintf(ch, "Your healing has restored %s to good health.\n\r", PERS(victim, ch));
	
	return;
}


_WEAVE (weave_scald)
// 14Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  OBJ_DATA *obj_lose, *obj_next;
  int dam = 0;
  bool fail = TRUE;
  int calc_level;
  int skill_percent;

  if (is_obj || IS_NPC(ch))
  {
	calc_level = level;
    skill_percent = 50;
  }
  else
  {
	skill_percent = GET_SKILL (ch, find_spell (ch, "scald"));
	calc_level =  ((LPA(ch, EARTH) + LPA(ch, EARTH)) / 3) +
	  level * skill_percent / 100;
    if ((ch->pcdata->talents[gtn_fire] ||
	     ch->pcdata->talents[gtn_earth]))
	  calc_level += 5;
  }
  if (!saves_spell (calc_level + 2, victim, DAM_FIRE)
      && !IS_FLAG (victim->states.imm, FIRE))
  {
    for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
    {
      obj_next = obj_lose->next_content;
      if (number_range (1, 2 * calc_level) > obj_lose->level
	  && !saves_spell (calc_level, victim, DAM_FIRE)
	  && !IS_OBJ_STAT (obj_lose, ITEM_NONMETAL)
	  && !IS_OBJ_STAT (obj_lose, ITEM_BURN_PROOF))
      {
	switch (obj_lose->item_type)
	{
	case ITEM_ARMOR:
	  if (obj_lose->wear_loc != -1)	/* remove the item */
	  {
	    if (can_drop_obj (victim, obj_lose)
		&& (obj_lose->weight / 10) <
		number_range (1, 2 * get_curr_stat (victim, STAT_DEX))
		&& remove_obj (victim, obj_lose->wear_loc, TRUE))
	    {
	      act ("$n yelps and throws $p to the ground!",
		   victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	      act ("You remove and drop $p before it burns you.",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 100;
	      obj_from_char (obj_lose);
	      obj_to_room (obj_lose, victim->in_room);
	      fail = FALSE;
	    }
	    else		/* stuck on the body! ouch! */
	    {
	      act ("Your skin is seared by $p!",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 100;
	      fail = FALSE;
	    }

	  }
	  else			/* drop it if we can */
	  {
	    if (can_drop_obj (victim, obj_lose))
	    {
	      act ("$n yelps and throws $p to the ground!",
		   victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	      act ("You and drop $p before it burns you.",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 200;
	      obj_from_char (obj_lose);
	      obj_to_room (obj_lose, victim->in_room);
	      fail = FALSE;
	    }
	    else		/* cannot drop */
	    {
	      act ("Your skin is seared by $p!",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 50;
	      fail = FALSE;
	    }
	  }
	  break;
	case ITEM_WEAPON:
	  if (obj_lose->wear_loc != -1)	/* try to drop it */
	  {
	    if (IS_WEAPON_STAT (obj_lose, WEAPON_FLAMING))
	      continue;

	    if (can_drop_obj (victim, obj_lose)
		&& remove_obj (victim, obj_lose->wear_loc, TRUE))
	    {
	      act ("$n is burned by $p, and throws it to the ground.",
		   victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	      send_to_char
		("You throw your red-hot weapon to the ground!\n\r", victim);
	      dam += obj_lose->level * skill_percent / 100;
	      obj_from_char (obj_lose);
	      obj_to_room (obj_lose, victim->in_room);
	      fail = FALSE;
	    }
	    else		/* YOWCH! */
	    {
	      send_to_char ("Your weapon sears your flesh!\n\r", victim);
	      dam += obj_lose->level * skill_percent / 50;
	      fail = FALSE;
	    }
	  }
	  else			/* drop it if we can */
	  {
	    if (can_drop_obj (victim, obj_lose))
	    {
	      act ("$n throws a burning hot $p to the ground!",
		   victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	      act ("You and drop $p before it burns you.",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 200;
	      obj_from_char (obj_lose);
	      obj_to_room (obj_lose, victim->in_room);
	      fail = FALSE;
	    }
	    else		/* cannot drop */
	    {
	      act ("Your skin is seared by $p!",
		   victim, obj_lose, NULL, TO_CHAR, POS_RESTING);
	      dam += obj_lose->level * skill_percent / 50;
	      fail = FALSE;
	    }
	  }
	  break;

	default:
	  break;
	}
      }
    }
  }
  if (fail)
    dam = calc_level;
  else
	dam += calc_level + LPA(ch, EARTH) * LPA(ch, FIRE) + skill_percent;
  damage (ch, victim, dam, sn, DAM_FIRE, TRUE, TRUE);
}

_WEAVE (weave_erode)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  OBJ_DATA *obj_lose, *obj_next;
  bool fail = TRUE;

  if (!saves_spell (level + 2, victim, DAM_WEAVE)
      && !IS_FLAG (victim->states.imm, _ONEPOWER))
  {
    for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
    {
      obj_next = obj_lose->next_content;
      if (number_range (1, 2 * level) > obj_lose->level
	  && !saves_spell (level, victim, DAM_WEAVE)
	  && !IS_SET(obj_lose->extra_flags, ITEM_NOBREAK)
	  && !IS_OBJ_STAT (obj_lose, ITEM_NONMETAL))
      {
	obj_lose->condition = UMAX (0, obj_lose->condition - ch->level / 4);
	if (obj_lose->condition <= 5)
	{
	  act ("$n's $p crumbles into dust.",
	       victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	}
	else if (obj_lose->condition <= 15)
	{
	  act
	    ("A resounding CRACK echoes through the area as $n's $p breaks apart.",
	     victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
	}
	else
	  act
	    ("$n's $p turns a dark shade of red as rust encroaches upon it.",
	     victim, obj_lose, NULL, TO_ROOM, POS_RESTING);
      }
    }
  }
  if (fail)
  {
    send_to_char ("Your weave had no effect.\n\r", ch);
    send_to_char ("You feel a momentary chill.\n\r", victim);
  }

  return;
}


_WEAVE (weave_delve)
{
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  char buf[MSL];
  AFFECT_DATA *paf;

  sprintf (buf,
	   "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
	   obj->name,
	   item_name (obj->item_type),
	   flag_string(extra_flags, obj->extra_flags),
	   obj->weight / 10, obj->cost, obj->level);
  send_to_char (buf, ch);

  switch (obj->item_type)
  {
  case ITEM_POTION:
  case ITEM_HERB:
    sprintf (buf, "Level %d weaves of:", obj->value[0]);
    send_to_char (buf, ch);

    if (IS_VALID_SKILL(obj->value[1]))
    {
      send_to_char (" '", ch);
      send_to_char (skill_table[obj->value[1]].name, ch);
      send_to_char ("'", ch);
    }

    if (IS_VALID_SKILL(obj->value[2]))
    {
      send_to_char (" '", ch);
      send_to_char (skill_table[obj->value[2]].name, ch);
      send_to_char ("'", ch);
    }

    if (IS_VALID_SKILL(obj->value[3]))
    {
      send_to_char (" '", ch);
      send_to_char (skill_table[obj->value[3]].name, ch);
      send_to_char ("'", ch);
    }

    if (IS_VALID_SKILL(obj->value[4]))
    {
      send_to_char (" '", ch);
      send_to_char (skill_table[obj->value[4]].name, ch);
      send_to_char ("'", ch);
    }

    send_to_char (".\n\r", ch);
    break;

    if (IS_VALID_SKILL(obj->value[3]))
    {
      send_to_char (" '", ch);
      send_to_char (skill_table[obj->value[3]].name, ch);
      send_to_char ("'", ch);
    }

    send_to_char (".\n\r", ch);
    break;

  case ITEM_DRINK_CON:
    sprintf (buf, "It holds %s-colored %s.\n\r",
	     liq_table[obj->value[2]].liq_color,
	     liq_table[obj->value[2]].liq_name);
    send_to_char (buf, ch);
    break;

  case ITEM_CONTAINER:
    sprintf (buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
	     obj->value[0], obj->value[3], flag_string(container_flags,obj->value[1]));
    send_to_char (buf, ch);
    if (obj->value[4] != 100)
    {
      sprintf (buf, "Weight multiplier: %d%%\n\r", obj->value[4]);
      send_to_char (buf, ch);
    }
    break;

  case ITEM_WEAPON:
    send_to_char ("Weapon type is ", ch);
    switch (obj->value[0])
    {
		case (WEAPON_EXOTIC):
			send_to_char ("exotic.\n\r", ch);
			break;
		case (WEAPON_SWORD):
			send_to_char ("sword.\n\r", ch);
			break;
		case (WEAPON_DAGGER):
			send_to_char ("dagger.\n\r", ch);
			break;
		case (WEAPON_SPEAR):
			send_to_char ("spear.\n\r", ch);
			break;
		case (WEAPON_STAFF):
			send_to_char("staff.\n\r", ch);
			break;
		case (WEAPON_MACE):
			send_to_char ("mace/club.\n\r", ch);
			break;
		case (WEAPON_AXE):
			send_to_char ("axe.\n\r", ch);
			break;
		case (WEAPON_FLAIL):
			send_to_char ("flail.\n\r", ch);
			break;
		case (WEAPON_WHIP):
			send_to_char ("whip.\n\r", ch);
			break;
		case (WEAPON_POLEARM):
			send_to_char ("polearm.\n\r", ch);
			break;
		default:
			send_to_char ("unknown.\n\r", ch);
			break;
    }

    sprintf (buf, "Damage is %dd%d (average %d).\n\r",
	     obj->value[1], obj->value[2],
	     (1 + obj->value[2]) * obj->value[1] / 2);

    send_to_char (buf, ch);

    if (obj->value[4])		/* weapon flags */
    {
      sprintf (buf, "Weapons flags: %s\n\r", flag_string(weapon_type2, obj->value[4]));
      send_to_char (buf, ch);
    }
    break;

  case ITEM_ARMOR:
    sprintf (buf,
	     "Armor class is %d pierce, %d bash, %d slash, and %d vs. weaves.\n\r",
	     obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
    send_to_char (buf, ch);
    break;

  default:
    break;
  }

  if (!obj->enchanted)
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location != APPLY_NONE && paf->modifier != 0)
      {
	    sprintf (buf, "Affects %s by %d.\n\r",
		  flag_string(apply_flags, paf->location), paf->modifier);
 	    send_to_char (buf, ch);
   	    if (paf->bitvector)
		{
	      switch (paf->where)
		  {
	        case TO_AFFECTS:
	          sprintf (buf, "Adds %s affect.\n",
		        flag_string(apply_flags, paf->bitvector));
	          break;
	        case TO_OBJECT:
	          sprintf (buf, "Adds %s object flag.\n",
		        flag_string(extra_flags, paf->bitvector));
	          break;
	        case TO_IMMUNE:
	          sprintf (buf, "Adds immunity to %s.\n",
		        flag_string(_resistance_bits, paf->bitvector));
	          break;
	        case TO_RESIST:
	          sprintf (buf, "Adds resistance to %s.\n\r",
		        flag_string(_resistance_bits, paf->bitvector));
	          break;
	        case TO_VULN:
	          sprintf (buf, "Adds vulnerability to %s.\n\r",
		        flag_string(_resistance_bits, paf->bitvector));
	          break;
	        default:
	          sprintf (buf, "Unknown bit %d: %ld\n\r",
		        paf->where, paf->bitvector);
	          break;
		  }
	      send_to_char (buf, ch);
		}
      }
    }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->location != APPLY_NONE && paf->modifier != 0)
    {
		  sprintf (buf, "Affects %s by %d",
	//	       flag_string (_affected_by_bits, paf->location), paf->modifier);
			   flag_string (apply_flags, paf->location), paf->modifier);
		  send_to_char (buf, ch);
		
		  if (paf->duration > -1)
			sprintf (buf, ", %d hours.\n\r", paf->duration);
		  else
			sprintf (buf, ".\n\r");
		  send_to_char (buf, ch);
		
		  if (paf->bitvector)
		  {
				switch (paf->where)
				{
					case TO_AFFECTS:
					  sprintf (buf, "Adds %s affect.\n",
						   flag_string(_affected_by_bits, paf->bitvector));
					  break;
					case TO_OBJECT:
					  sprintf (buf, "Adds %s object flag.\n",
						   flag_string(extra_flags, paf->bitvector));
					  break;
					case TO_WEAPON:
					  sprintf (buf, "Adds %s weapon flags.\n",
						   flag_string(weapon_type2, paf->bitvector));
					  break;
					case TO_IMMUNE:
					  sprintf (buf, "Adds immunity to %s.\n",
						   flag_string(_resistance_bits, paf->bitvector));
					  break;
					case TO_RESIST:
					  sprintf (buf, "Adds resistance to %s.\n\r",
						   flag_string(_resistance_bits, paf->bitvector));
					  break;
					case TO_VULN:
					  sprintf (buf, "Adds vulnerability to %s.\n\r",
						   flag_string(_resistance_bits, paf->bitvector));
					  break;
					default:
					  sprintf (buf, "Unknown bit %d: %ld\n\r",
						   paf->where, paf->bitvector);
					  break;
				}
			send_to_char (buf, ch);
	  }
	}
  }
	return;
}

_WEAVE (weave_invisibility)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  AFFECT_DATA af;

  /* object invisibility */
  if (target == TARGET_OBJ)
  {
    obj = (OBJ_DATA *) vo;

    if (IS_OBJ_STAT (obj, ITEM_INVIS))
    {
      act ("$p is already invisible.", ch, obj, NULL, TO_CHAR, POS_RESTING);
      return;
    }

    af.where = TO_OBJECT;
    af.type = sn;
    af.level = level;
    af.duration = level + 12;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = ITEM_INVIS;
    affect_to_obj (obj, &af);

    act ("$p fades out of sight.", ch, obj, NULL, TO_ALL, POS_RESTING);
    return;
  }

  /* character invisibility */
  victim = (CHAR_DATA *) vo;

  if (IS_AFFECTED (victim, _AFF_INVISIBLE))
    return;

  act ("$n fades out of existence.", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + 12;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = _AFF_INVISIBLE;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("You fade out of existence.\n\r", victim);
  return;
}


_WEAVE (weave_lightning_bolt)
// 21Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  static const sh_int dam_each[] = {
    0,
    5, 8, 11, 13, 15, 18, 21, 23, 25, 28,
    31, 34, 37, 40, 40, 41, 42, 42, 43, 44,
    44, 45, 46, 46, 47, 48, 48, 49, 50, 50,
    92, 94, 96, 98, 100, 102, 104, 106, 108, 110,
    112, 114, 116, 118, 120, 122, 124, 126, 128, 130,
	135, 140, 145, 150, 155, 160, 165, 170, 175, 180
  };
  int dam;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
	if (ch->pcdata->talents[gtn_water])
      calc_level = (LPA(ch, WATER)) +
	  level * GET_SKILL (ch, find_spell (ch, "lightning")) / 100;
	else
      calc_level = (LPA(ch, WATER) / 2) +
	  level * GET_SKILL (ch, find_spell (ch, "lightning")) / 100;

  calc_level = UMIN (60, calc_level);
  dam = number_range (dam_each[calc_level] / 2,
	                  dam_each[calc_level] * 2);

  if (saves_spell (calc_level, victim, DAM_LIGHTNING))
    dam /= 2;

  if (!IS_NPC(ch) &&
      ch->pcdata->talents[gtn_water])
  {
    dam += LPA(ch, AIR) * LPA(ch, WATER);
    damage (ch, victim, dam, sn, DAM_LIGHTNING, TRUE, TRUE);
    shock_effect (victim, calc_level, dam, TARGET_CHAR, 0);
  }
  else
  {
    damage (ch, victim, dam, sn, DAM_LIGHTNING, TRUE, TRUE);
    shock_effect (victim, calc_level / 2, dam, TARGET_CHAR, 0);
  }
  return;
}


// 9/9/00
// Taken out of the command table (replaced with dreaming talent)
// This was kept here to possibly be used with angreal code.
// --GM
_WEAVE (weave_locate_object)
{
  char buf[MIL];
  BUFFER *buffer;
  OBJ_DATA *obj;
  OBJ_DATA *in_obj;
  bool found;
  int number = 0, max_found;
  char arg1[MIL];

  one_argument (arg, arg1);

  found = FALSE;
  number = 0;
  max_found = IS_IMMORTAL (ch) ? 200 : 2 * level;

  buffer = new_buf ();

  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (!can_see_obj (ch, obj) || !is_name (arg1, obj->name)
	|| IS_OBJ_STAT (obj, ITEM_NOLOCATE) || number_percent () > 2 * level
	|| ch->level < obj->level)
      continue;

    found = TRUE;
    number++;

    for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
      ;

    if (in_obj->carried_by != NULL && can_see (ch, in_obj->carried_by))
    {
      sprintf (buf, "one is carried by %s\n\r",
	       PERS (in_obj->carried_by, ch));
    }
    else
    {
      if (IS_IMMORTAL (ch) && in_obj->in_room != NULL)
	sprintf (buf, "one is in %s [Room %ld]\n\r",
		 in_obj->in_room->name, in_obj->in_room->vnum);
      else
	sprintf (buf, "one is in %s\n\r",
		 in_obj->in_room == NULL
		 ? "somewhere" : in_obj->in_room->name);
    }

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

  return;
}


_WEAVE (weave_mass_invisibility)
{
  AFFECT_DATA af;
  CHAR_DATA *gch;

  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (!is_same_group (gch, ch) || IS_AFFECTED (gch, _AFF_INVISIBLE))
      continue;
    act ("$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM,
	 POS_RESTING);
    send_to_char ("You slowly fade out of existence.\n\r", gch);

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level / 2;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = _AFF_INVISIBLE;
    af.owner = GET_ID(ch);

    affect_to_char (gch, &af);
  }
  send_to_char ("Ok.\n\r", ch);

  return;
}


_WEAVE (weave_align_phase)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  LPA(ch, SPIRIT) +
	  level * GET_SKILL (ch, find_spell (ch, "passdoor")) / 100;

  if ((victim != ch) && (90 > GET_SKILL (ch, find_spell (ch, "passdoor"))))
  {
    send_to_char ("You are not skilled enough to cast that on another.\n\r", ch);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_PASS_DOOR))
  {
    if (victim == ch)
      send_to_char ("You are already out of phase.\n\r", ch);
    else
      act ("$N is already shifted out of phase.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = number_fuzzy (calc_level / 4);
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = _AFF_PASS_DOOR;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  act ("$n turns translucent.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
  send_to_char ("You turn translucent.\n\r", victim);
  return;
}


/* RT plague spell, very nasty */
_WEAVE (weave_plague)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  LPA(ch, SPIRIT) +
	  level * GET_SKILL (ch, find_spell (ch, "plague")) / 100;

  if (saves_spell (calc_level, victim, DAM_DISEASE)
      || IS_AFFECTED (victim, _AFF_PLAGUE))
  {
    if (ch == victim)
      send_to_char ("You feel momentarily ill, but it passes.\n\r", ch);
    else
      act ("$N seems to be unaffected.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level * 3 / 4;
  af.duration = calc_level;
  af.location = APPLY_STR;
  if (IS_NPC(ch))
    af.modifier = -5;
  else
    af.modifier = LPA(ch, SPIRIT);
  af.bitvector = _AFF_PLAGUE;
  affect_join (victim, &af);

  send_to_char
    ("You scream in agony as plague sores erupt from your skin.\n\r", victim);
  act ("$n screams in agony as plague sores erupt from $s skin.",
       victim, NULL, NULL, TO_ROOM, POS_RESTING);
}


_WEAVE (weave_poison)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  LPA(ch, SPIRIT) +
	  level * GET_SKILL (ch, find_spell (ch, "poison")) / 100;

  if (target == TARGET_OBJ)
  {
    obj = (OBJ_DATA *) vo;

    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
      if (IS_OBJ_STAT (obj, ITEM_BLESS) || IS_OBJ_STAT (obj, ITEM_BURN_PROOF))
      {
	act ("Your weave fails to corrupt $p.", ch, obj, NULL, TO_CHAR,
	     POS_RESTING);
	return;
      }
      obj->value[3] = 1;
      act ("$p is infused with poisonous vapors.", ch, obj, NULL, TO_ALL,
	   POS_RESTING);
      return;
    }

    if (obj->item_type == ITEM_WEAPON)
    {
      if (IS_WEAPON_STAT (obj, WEAPON_FLAMING)
	  || IS_WEAPON_STAT (obj, WEAPON_FROST)
	  || IS_WEAPON_STAT (obj, WEAPON_VAMPIRIC)
	  || IS_WEAPON_STAT (obj, WEAPON_VORPAL)
	  || IS_WEAPON_STAT (obj, WEAPON_SHOCKING)
	  || IS_OBJ_STAT (obj, ITEM_BLESS)
	  || IS_OBJ_STAT (obj, ITEM_BURN_PROOF))
      {
	act ("You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR,
	     POS_RESTING);
	return;
      }

      if (IS_WEAPON_STAT (obj, WEAPON_POISON))
      {
	act ("$p is already envenomed.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	return;
      }

      af.where = TO_WEAPON;
      af.type = sn;
      af.level = calc_level / 2;
      af.duration = calc_level / 8;
      af.location = 0;
      af.modifier = 0;
      af.bitvector = WEAPON_POISON;
      affect_to_obj (obj, &af);

      act ("$p is coated with deadly venom.", ch, obj, NULL, TO_ALL,
	   POS_RESTING);
      return;
    }

    act ("You can't poison $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
    return;
  }

  victim = (CHAR_DATA *) vo;

  if (IS_AFFECTED (victim, _AFF_POISON)
      || saves_spell (calc_level, victim, DAM_POISON))
  {
    act ("$n turns slightly green, but it passes.", victim, NULL, NULL,
	 TO_ROOM, POS_RESTING);
    send_to_char ("You feel momentarily ill, but it passes.\n\r", victim);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = calc_level;
  af.location = APPLY_STR;
  if (IS_NPC(ch))
    af.modifier = -2;
  else
    af.modifier = LPA(ch, SPIRIT) / 2;
  af.bitvector = _AFF_POISON;
  affect_join (victim, &af);
  send_to_char ("You feel very sick.\n\r", victim);
  act ("$n looks very ill.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
  return;
}


_WEAVE (weave_ward_evil)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_PROTECT_EVIL)
      || IS_AFFECTED (victim, _AFF_PROTECT_GOOD))
  {
    if (victim == ch)
      send_to_char ("You are already protected.\n\r", ch);
    else
      act ("$N is already protected.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.location = APPLY_SAVING_SPELL;
  if (IS_NPC(ch))
    af.modifier = -1;
  else
    af.modifier = -1 * LPA(ch, SPIRIT);
  af.bitvector = _AFF_PROTECT_EVIL;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("You feel holy and pure.\n\r", victim);
  if (ch != victim)
    act ("$N is protected from evil.", ch, NULL, victim, TO_CHAR,
	 POS_RESTING);
  return;
}


_WEAVE (weave_ward_good)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_PROTECT_GOOD)
      || IS_AFFECTED (victim, _AFF_PROTECT_EVIL))
  {
    if (victim == ch)
      send_to_char ("You are already protected.\n\r", ch);
    else
      act ("$N is already protected.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.location = APPLY_SAVING_SPELL;
  if (IS_NPC(ch))
    af.modifier = -1;
  else
    af.modifier = -1 * LPA(ch, SPIRIT);
  af.bitvector = _AFF_PROTECT_GOOD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("You feel aligned with darkness.\n\r", victim);
  if (ch != victim)
    act ("$N is protected from good.", ch, NULL, victim, TO_CHAR,
	 POS_RESTING);
  return;
}


_WEAVE (weave_refresh)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;


  //  if it is a mob doing the healing, or herbs, just refresh a little,
  if (IS_NPC (ch) || is_obj)
  {
    victim->move = UMIN (victim->move + level, victim->max_move);
    return;
  }


  if (victim == ch)
  {
    send_to_char ("You can't refresh yourself!\n\r", ch);
    return;
  }

  victim->move = UMIN (LPA(ch, ALL) + victim->move + 
	  GET_SKILL (ch, find_spell (ch, "refresh")) /2, victim->max_move);

  if (ch->pcdata->talents[gtn_healing] > 0)
  {
    victim->move = UMIN (victim->move + level, victim->max_move);
  }

  if (victim->max_move == victim->move)
    send_to_char ("You feel fully refreshed!\n\r", victim);
  else
    send_to_char ("You feel less tired.\n\r", victim);
  if (ch != victim)
    send_to_char ("Ok.\n\r", ch);
  return;
}

_WEAVE (weave_air_shield)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED (victim, _AFF_AIR_SHIELD))
  {
    if (victim == ch)
      send_to_char ("You are already shielded in Air.\n\r", ch);
    else
      act ("$N is already shielded in Air.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_EARTH_SHIELD))
  {
    send_to_char ("Your shield of Air cancels Earth.\n\r", ch);
    affect_strip (victim, skill_lookup("earth shield"));
  }
  if (IS_AFFECTED (victim, _AFF_FIRE_SHIELD))
  {
    send_to_char ("Your shield of Air cancels Fire.\n\r", ch);
    affect_strip (victim, skill_lookup("fire shield"));
  }
  if (IS_AFFECTED (victim, _AFF_FROST_SHIELD))
  {
    send_to_char ("Your shield of Air cancels Frost.\n\r", ch);
    affect_strip (victim, skill_lookup("frost shield"));
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + LPA(ch, AIR);
  af.location = APPLY_AC;
  af.modifier = -15 * LPA(ch, AIR);
  af.bitvector = _AFF_AIR_SHIELD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  act ("$n is surrounded by a shield of air.", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);
  send_to_char ("You are surrounded by a shield of air.\n\r", victim);
  return;
}


_WEAVE (weave_earth_shield)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn))
  {
    if (victim != ch)
      act ("You remove the shield of Earth from $N.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    send_to_char ("The earth and rock encasing you falls to the ground as dirt and dust.\n\r", victim);
    act ("The earth and rock encasing $n falls to the ground as dirt and dust.",
      victim, NULL, NULL, TO_ROOM, POS_RESTING);
    affect_strip (victim, skill_lookup("earth shield"));
    return;
  }

  if (!IS_OUTSIDE (ch))
  {
    send_to_char ("There is not enough earth in here.\n\r", ch);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_FIRE_SHIELD))
  {
    send_to_char ("Your shield of Earth cancels Fire.\n\r", ch);
    affect_strip (victim, skill_lookup("fire shield"));
  }
  if (IS_AFFECTED (victim, _AFF_FROST_SHIELD))
  {
    send_to_char ("Your shield of Earth cancels Frost.\n\r", ch);
    affect_strip (victim, skill_lookup("frost shield"));
  }
  if (IS_AFFECTED (victim, _AFF_AIR_SHIELD))
  {
    send_to_char ("Your shield of Earth cancels Air.\n\r", ch);
    affect_strip (victim, skill_lookup("air shield"));
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + LPA(ch, EARTH);
  af.location = APPLY_AC;
  af.modifier = -50 * LPA(ch, EARTH);
  af.bitvector = _AFF_EARTH_SHIELD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  act
    ("The earth erupts around $n, entombing $s in a shell of hardened earth and rock.",
     victim, NULL, NULL, TO_ROOM, POS_RESTING);
  send_to_char ("You are surrounded by solid earth!\n\r", victim);
  return;
}

_WEAVE (weave_fire_shield)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn))
  {
    if (victim == ch)
      send_to_char ("You are already sheathed in Fire!\n\r", ch);
    else
      act ("$N is already protected by Fire.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_EARTH_SHIELD))
  {
    send_to_char ("Your shield of Fire cancels Earth.\n\r", ch);
    affect_strip (victim, skill_lookup("earth shield"));
  }
  if (IS_AFFECTED (victim, _AFF_FROST_SHIELD))
  {
    send_to_char ("Your shield of Fire cancels Frost.\n\r", ch);
    affect_strip (victim, skill_lookup("frost shield"));
  }
  if (IS_AFFECTED (victim, _AFF_AIR_SHIELD))
  {
    send_to_char ("Your shield of Fire cancels Air.\n\r", ch);
    affect_strip (victim, skill_lookup("air shield"));
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + LPA(ch, FIRE);
  af.location = APPLY_AC;
  af.modifier = -5 * LPA(ch, FIRE);
  af.bitvector = _AFF_FIRE_SHIELD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  act ("An aura of cold Fire explodes into being around $n.",
       victim, NULL, NULL, TO_ROOM, POS_RESTING);

  send_to_char ("You are surrounded by Fire!\n\r", victim);
  return;
}


_WEAVE (weave_frost_shield)
// 04Oct00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected (victim, sn))
  {
    if (victim == ch)
      send_to_char ("You are already shielded in frost.\n\r", ch);
    else
      act ("$N is already covered in frost.", ch, NULL, victim, TO_CHAR,
	   POS_RESTING);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_EARTH_SHIELD))
  {
    send_to_char ("Your shield of Frost cancels Earth.\n\r", ch);
    affect_strip (victim, skill_lookup("earth shield"));
  }
  if (IS_AFFECTED (victim, _AFF_FIRE_SHIELD))
  {
    send_to_char ("Your shield of Frost cancels Fire.\n\r", ch);
    affect_strip (victim, skill_lookup("fire shield"));
  }
  if (IS_AFFECTED (victim, _AFF_AIR_SHIELD))
  {
    send_to_char ("Your shield of Frost cancels Air.\n\r", ch);
    affect_strip (victim, skill_lookup("air shield"));
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level + LPA(ch, WATER);
  af.location = APPLY_AC;
  af.modifier = -10 * LPA(ch, WATER);
  af.bitvector = _AFF_FROST_SHIELD;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);

  act
    ("The moisture in the air suddenly freezes around $n, sheathing $s in frost.",
     victim, NULL, NULL, TO_ROOM, POS_RESTING);

  send_to_char ("You are covered in frost.\n\r", victim);
  return;
}


_WEAVE (weave_sleep)
// 14Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  (LPA(ch, SPIRIT) / 2) +
	  level * GET_SKILL (ch, find_spell (ch, "sleep")) / 100;

  if (IS_AFFECTED (victim, _AFF_SLEEP)
      || (calc_level) < victim->level
      || saves_spell (calc_level, victim, DAM_CHARM))
    return;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = 4 + calc_level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = _AFF_SLEEP;
  affect_join (victim, &af);

  if (IS_AWAKE (victim))
  {
    send_to_char ("You feel very sleepy ..... zzzzzz.\n\r", victim);
    act ("$n goes to sleep.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
    victim->position = POS_SLEEPING;
  }
  return;
}

_WEAVE (weave_slow)
// 12Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  (LPA(ch, ALL) / 10) + 3 +
	  level * GET_SKILL (ch, find_spell (ch, "slow")) / 100;

  if (is_affected (victim, sn) || IS_AFFECTED (victim, _AFF_SLOW))
  {
    if (victim == ch)
      send_to_char ("You can't move any slower!\n\r", ch);
    else
      act ("$N can't get any slower than that.",
	   ch, NULL, victim, TO_CHAR, POS_RESTING);
    return;
  }

  if (saves_spell (calc_level, victim, DAM_WEAVE)
      || IS_FLAG (victim->states.imm, _ONEPOWER))
  {
    if (victim != ch)
      send_to_char ("Nothing seemed to happen.\n\r", ch);
    send_to_char ("You feel momentarily lethargic.\n\r", victim);
    return;
  }

  if (IS_AFFECTED (victim, _AFF_HASTE))
  {
    if (!check_dispel (calc_level, victim, skill_lookup("haste")))
    {
      if (victim != ch)
	    send_to_char ("Weave failed.\n\r", ch);
      send_to_char ("You feel momentarily slower.\n\r", victim);
      return;
    }

    act ("$n is moving less quickly.", victim, NULL, NULL, TO_ROOM,
	 POS_RESTING);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = calc_level / 2;
  af.location = APPLY_DEX;
  af.modifier = calc_level / -8;
  af.bitvector = _AFF_SLOW;
  af.owner = GET_ID(ch);

  affect_to_char (victim, &af);
  send_to_char ("You feel yourself slowing d o w n...\n\r", victim);
  act ("$n starts to move in slow motion.", victim, NULL, NULL, TO_ROOM,
       POS_RESTING);
  return;
}


_WEAVE (weave_weaken)
// 13Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int calc_level;

  if (is_obj || IS_NPC(ch))
	calc_level = level;
  else
    calc_level =  (LPA(ch, ALL) / 10) + 3 +
	  level * GET_SKILL (ch, find_spell (ch, "weaken")) / 100;

  if (is_affected (victim, sn) ||
	  saves_spell (calc_level, victim, DAM_WEAVE))
    return;

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = calc_level;
  af.duration = calc_level / 2;
  af.location = APPLY_STR;
  af.modifier = calc_level / -5;
  af.bitvector = _AFF_WEAKEN;
  af.owner = GET_ID(ch);

  if ( !IS_NPC(victim) )
    victim->pcdata->condition[_HUNGER] = 0;

  affect_to_char (victim, &af);
  send_to_char
    ("You feel your strength slip away and the first stabs of hunger.\n\r",
     victim);
  act ("$n looks tired and weak.", victim, NULL, NULL, TO_ROOM, POS_RESTING);
  return;
}

// These two weaves need to be removed...
_WEAVE (weave_recall)
{
	do_recall(ch,"");
}

_WEAVE(weave_sanctum)
{
	do_recall(ch,"");
}

_WEAVE (weave_fire_breath)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *vch, *vch_next;
  int dam, hp_dam, dice_dam;
  int hpch;

  act ("$n breathes forth a cone of fire.", ch, NULL, victim, TO_NOTVICT,
       POS_RESTING);
  act ("$n breathes a cone of hot fire over you!", ch, NULL, victim, TO_VICT,
       POS_DEAD);
  act ("You breath forth a cone of fire.", ch, NULL, NULL, TO_CHAR,
       POS_RESTING);

  hpch = UMAX (10, ch->hit);
  hp_dam = number_range (hpch / 9 + 1, hpch / 5);
  dice_dam = dice (level, 20);

  dam = UMAX (hp_dam + dice_dam / 10, dice_dam + hp_dam / 10);
  fire_effect (victim->in_room, level, dam / 2, TARGET_ROOM, GET_ID(ch));

  for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
  {
    vch_next = vch->next_in_room;


    if (vch == victim)		/* full damage */
    {
      if (saves_spell (level, vch, DAM_FIRE))
      {
	fire_effect (vch, level / 2, dam / 4, TARGET_CHAR, GET_ID(ch));
	damage (ch, vch, dam / 2, sn, DAM_FIRE, TRUE, TRUE);
      }
      else
      {
	fire_effect (vch, level, dam, TARGET_CHAR, GET_ID(ch));
	damage (ch, vch, dam, sn, DAM_FIRE, TRUE, TRUE);
      }
    }
    else			/* partial damage */
    {
      if (saves_spell (level - 2, vch, DAM_FIRE))
      {
	fire_effect (vch, level / 4, dam / 8, TARGET_CHAR, GET_ID(ch));
	damage (ch, vch, dam / 4, sn, DAM_FIRE, TRUE, TRUE);
      }
      else
      {
	fire_effect (vch, level / 2, dam / 4, TARGET_CHAR, GET_ID(ch));
	damage (ch, vch, dam / 2, sn, DAM_FIRE, TRUE, TRUE);
      }
    }
  }
}

_WEAVE (weave_create_spring)
// 12Aug00 link/skill code added
{
  OBJ_DATA *spring;

  spring = create_object (get_obj_index (OBJ_VNUM_SPRING), 0);
  if (is_obj || IS_NPC (ch))
    spring->timer = ch->level;
  else
	spring->timer = LPA(ch, WATER) * LPA(ch, WATER) *
	  GET_SKILL (ch, find_spell (ch, "create spring")) / 100;
  obj_to_room (spring, ch->in_room);
  act ("$p flows from the ground.", ch, spring, NULL, TO_ROOM, POS_RESTING);
  act ("$p flows from the ground.", ch, spring, NULL, TO_CHAR, POS_RESTING);
  return;
}

_WEAVE (weave_pummel)
// 12Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  static const sh_int dam_each[] = {
    0,
    3, 3, 4, 4, 5, 6, 6, 6, 6, 6,
    8, 8, 8, 8, 8, 9, 9, 9, 9, 9,
    10, 10, 10, 10, 10, 11, 11, 11, 11, 11,
    14, 14, 14, 14, 14, 16, 16, 16, 16, 16,
    20, 20, 20, 20, 20, 25, 25, 25, 25, 25
  };
  int dam;

  level = UMIN (level, sizeof (dam_each) / sizeof (dam_each[0]) - 1);
  level = UMAX (0, level);
  dam = number_range (dam_each[level] / 2, dam_each[level] * 2);

  if (saves_spell (level, victim, DAM_AIR))
    dam /= 2;

  if (!IS_NPC (ch))
  {
	dam = dam * GET_SKILL (ch, find_spell (ch, "pummel")) / 100;
	if (ch->pcdata->talents[gtn_air] > 0)
      dam += LPA (ch, AIR) * 2;
    else
      dam += LPA (ch, AIR) / 2;
  }

  damage (ch, victim, dam, sn, DAM_AIR, TRUE, TRUE);
  return;
}

_WEAVE (weave_squeeze)
// 12Aug00 link/skill code added
{
	
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	static const sh_int dam_each[] = {
		0,
			5, 8, 11, 13, 15, 18, 21, 23, 25, 28,
			31, 34, 37, 40, 40, 41, 42, 42, 43, 44,
			44, 45, 46, 46, 47, 48, 48, 49, 50, 50,
			51, 52, 52, 53, 54, 54, 55, 56, 56, 57,
			58, 58, 59, 60, 60, 61, 62, 62, 63, 64
	};
	int dam;
	
	level = UMIN (level, sizeof (dam_each) / sizeof (dam_each[0]) - 1);
	level = UMAX (0, level);
	dam = number_range (dam_each[level] / 2, dam_each[level] * 2);
	
	if (!IS_NPC (ch))
	{
		if (ch->pcdata->talents[gtn_air] > 0)
			dam += LPA(ch, AIR) * LPA(ch, AIR);
		else
			dam += LPA(ch, AIR);
		dam = dam * GET_SKILL (ch, skill_lookup("squeeze")) / 100;
	}
	
	if (!is_affected (victim, sn) && !IS_AFFECTED (victim, _AFF_SLOW))
	{
		bool CanBeSlowed = FALSE;
		
		if (IS_AFFECTED (victim, _AFF_HASTE))
		{
			if (check_dispel (level, victim, skill_lookup("haste")))
				CanBeSlowed = TRUE;
		}
		else
			CanBeSlowed = TRUE;
		
		if (CanBeSlowed)
		{
			AFFECT_DATA *af = new_affect ();
			
			if (TRUESEX (ch) == TRUESEX (victim) && can_channel (victim))
				chprintf (victim,
				"Flows of air constrict painfully around your chest and legs.\n\r");
			else
				chprintf (victim,
				"You feel a sudden, painful constriction in your chest and legs.\n\r");
			
			act ("$N no longer moves so quickly.",
				ch, NULL, victim, TO_CHAR, POS_RESTING);
			
			af->where = TO_AFFECTS;
			af->type = sn;
			af->level = level;
			af->duration = level / 2;
			af->location = APPLY_DEX;
			af->modifier = -1 - (level >= 18) - (level >= 25) - (level >= 32);
			af->bitvector = _AFF_SLOW;
			af->owner = GET_ID(ch);
			
			affect_to_char (victim, af);
		}
	}
	
	if (saves_spell (level, victim, DAM_AIR))
		dam /= 2;
	
	if (is_obj || IS_NPC (ch))
		dam += LPA(ch, ALL) + level / 2;
	damage (ch, victim, dam, sn, DAM_AIR, TRUE, TRUE);
	return;
}


_WEAVE (weave_ice_shard)
// 12Aug00 link/skill code added
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  static const sh_int dam_each[] = {
    0,
    3, 3, 4, 4, 5, 6, 6, 6, 6, 6,
    8, 8, 8, 8, 8, 9, 9, 9, 9, 9,
    10, 10, 10, 10, 10, 11, 11, 11, 11, 11,
    14, 14, 14, 14, 14, 16, 16, 16, 16, 16,
    20, 20, 20, 20, 20, 25, 25, 25, 25, 25
  };
  int dam;
  

  level = UMIN (level, sizeof (dam_each) / sizeof (dam_each[0]) - 1);
  level = UMAX (0, level);
  if (is_obj || IS_NPC (ch))
    dam = number_range (dam_each[level] / 2, dam_each[level] * 2);
  else
  {
	dam = number_range (dam_each[level] / 2, dam_each[level] * 2) + 
		LPA(ch, AIR) + LPA(ch, WATER);
	dam = dam * GET_SKILL (ch, find_spell (ch, "ice shard")) / 100;
  }
  if (saves_spell (level, victim, DAM_COLD))
    dam /= 2;
  else
    if (!IS_NPC(ch) &&
  	    (ch->pcdata->talents[gtn_water] ||
	     ch->pcdata->talents[gtn_air]))
	{
	  dam += LPA(ch, AIR) * LPA(ch, WATER);
	  cold_effect (victim, level, dam, TARGET_CHAR, GET_ID(ch));
	}
  damage (ch, victim, dam, sn, DAM_COLD, TRUE, TRUE);
  return;
}


_WEAVE (weave_dehydrate)
// 04Oct00 link code fixed
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (saves_spell (level, victim, DAM_WEAVE)
      || IS_FLAG (victim->states.imm, _ONEPOWER))
  {
    victim->move -= 25;
    send_to_char ("You feel slightly dehydrated.\n\r", victim);
    send_to_char ("Your weave did not have full affect.\n\r", ch);
    return;
  }

  if ( !IS_NPC(victim) )
  {
    ch->pcdata->condition[_THIRST] -= 50;
  }
  if (!IS_NPC(ch))
  {
     victim->move = victim->move * 100 / (100 + 
        (LPA(ch, WATER) * GET_SKILL (ch, skill_lookup("dehydrate"))));
    send_to_char ("You suddenly feel rather dehydrated.\n\r", victim);
    send_to_char ("Your opponent seems to shrivel like a prune.\n\r", ch);
  }
  return;
}

