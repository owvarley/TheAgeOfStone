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
 * @@ Objective (act_move.c)
 * 
 * This file contains the commands for player movement, as well as for
 * player position states. (ie: do_sit, do_sleep, etc) Also contains the
 * code for entering portals.
 */

#if defined(WIN32)
  #include <time.h>
#else
  #include <sys/time.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"

#if defined(_WIN32)
  #pragma warning(disable:4273)   // 'symbol' : : inconsistent dll linkage.  dllexport assumed.
#endif

char *const dir_name[] = {
	"north", "east", "south", "west", "up", "down"
};

const sh_int rev_dir[] = {
	2, 3, 0, 1, 5, 4
};

const sh_int movement_loss[SECT_MAX] = {
	1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



void move_char (CHAR_DATA * ch, int door, bool follow)
{
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	OBJ_DATA *obj;
	EXIT_DATA *pexit;
	char buf[MSL];
	
	if (door < 0 || door > 5)
	{
		bugf("[act_move.c::do_move] bad door %d.", door);
		return;
	}

	in_room = ch->in_room;
	if ((pexit = in_room->exit[door]) == NULL
		|| (to_room = pexit->u1.to_room) == NULL
		|| !can_see_room (ch, pexit->u1.to_room))
	{
		send_to_char ("Alas, you cannot go that way.\n\r", ch);
		return;
	}

	// ToS
	if (to_room->sector_type == SECT_WATER)
	{
		send_to_char("Your land legs are not going to be any use on the water!\n\r", ch);
		return;
	}
	
	if (MOUNTED (ch))
	{
		if (MOUNTED (ch)->position < POS_FIGHTING)
		{
			send_to_char ("Your mount must be standing.\n\r", ch);
			return;
		}
		if (!mount_success (ch, MOUNTED (ch), FALSE))
		{
			send_to_char ("Your mount stubbornly refuses to go that way.\n\r", ch);
			return;
		}
	}
	
    /* Exit trigger, if activated, bail out. Only PCs are triggered. */
	if (!IS_NPC (ch) && mp_exit_trigger (ch, door))
		return;
	
	in_room = ch->in_room;
	if ((pexit = in_room->exit[door]) == NULL
		|| (to_room = pexit->u1.to_room) == NULL
		|| !can_see_room (ch, pexit->u1.to_room))
	{
		send_to_char ("Alas, you cannot go that way.\n\r", ch);
		return;
	}
	
	if (IS_SET (pexit->exit_info, EX_CLOSED)
		&& (!IS_AFFECTED (ch, _AFF_PASS_DOOR)
		|| IS_SET (pexit->exit_info, EX_NOPASS)) && !IS_TRUSTED (ch, ANGEL))
	{
		act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR, POS_RESTING);
		return;
	}
	
	/* 
 	 * Is character bound, or otherwise incapable of movement?
	 */
	if (!can_fight (ch, TRUE))
		return;
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION)
		&& ch->master != NULL && in_room == ch->master->in_room)
	{
		send_to_char ("What?  And leave your beloved master?\n\r", ch);
		return;
	}
	
	if (IS_SET (pexit->exit_info, EX_RANDOM))
	{
		ROOM_INDEX_DATA *rroom;
		EXIT_DATA *rexit;
		int rdoor;
		int cnt;
		
		for (cnt = 0; cnt < 5; cnt++)
		{
			rdoor = number_range (0, 5);
			if ((rexit = in_room->exit[rdoor]) != NULL
				&& (rroom = rexit->u1.to_room) != NULL
				&& (rdoor != cnt) && (rroom != in_room))
			{
				pexit = rexit;
				to_room = rroom;
				door = rdoor;
				break;
			}
		}
		
	}
	
	if (IS_SET (to_room->area->area_flags, AREA_CLOSED) && !IS_IMMORTAL (ch))
	{
		send_to_char ("That room is currently under construction.\n\r", ch);
		return;
	}
	
	if (!is_room_owner (ch, to_room) && room_is_private (to_room))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}


	
	if ((IS_NPC (ch) || ch->pcdata->rentroom != to_room->vnum) &&
		IS_SET (to_room->room_flags, ROOM_RENTED))
	{
		if (!IS_IMMORTAL (ch))
		{
			send_to_char ("That room has been rented by someone else.\n\r", ch);
			return;
		}
	}
	
	if (!IS_NPC (ch))
	{
		int move;
		
		if (!IS_IMMORTAL (ch)
			&& ((to_room->c_class > -1 && to_room->c_class != ch->c_class)
			|| (to_room->clan > -1 && to_room->clan != ch->clan)))
		{
			send_to_char ("You aren't allowed in there.\n\r", ch);
			return;
		}

//
// Removed when SECT_AIR flag removed.
//
//		if (in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR)
//		{
//			if (!IS_AFFECTED (ch, _AFF_FLYING) && !IS_IMMORTAL (ch))
//			{
//				if (MOUNTED (ch))
//				{
//					if (!IS_AFFECTED (MOUNTED (ch), _AFF_FLYING))
//					{
//						send_to_char ("Your mount can't fly.\n\r", ch);
//						return;
//					}
//				}
//				else
//				{
//					send_to_char ("You can't fly.\n\r", ch);
//					return;
//				}
//			}
//		}
		

//		if ((in_room->sector_type == SECT_WATER_NOSWIM
//			|| to_room->sector_type == SECT_WATER_NOSWIM)
//			&& (MOUNTED (ch) && IS_AFFECTED (MOUNTED (ch), _AFF_FLYING)))
//		{
//			sprintf (buf, "You can't take your mount there.\n\r");
//			send_to_char (buf, ch);
//			return;
//		}
//		
//		if ((in_room->sector_type == SECT_WATER_NOSWIM
//			|| to_room->sector_type == SECT_WATER_NOSWIM)
//			&& !IS_AFFECTED (ch, _AFF_FLYING))
//		{
//			OBJ_DATA *obj;
//			bool found;
//			
//			/*
//			 * Look for a boat.
//			 */
//			found = FALSE;
//			
//			if (IS_IMMORTAL (ch))
//				found = TRUE;
//			
//			for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
//			{
//				if (obj->item_type == ITEM_BOAT)
//				{
//					found = TRUE;
//					break;
//				}
//			}
//			if (!found)
//			{
//				send_to_char ("You need a boat to go there.\n\r", ch);
//				return;
//			}
//		}
		
		
		move = movement_loss[UMIN (SECT_MAX - 1, in_room->sector_type)]
			+ movement_loss[UMIN (SECT_MAX - 1, to_room->sector_type)];
		
		move /= 2;	 /* i.e. the average */
		
		if (!MOUNTED (ch))
		{
			/* conditional effects */
			if (IS_AFFECTED (ch, _AFF_FLYING) || IS_AFFECTED (ch, _AFF_HASTE))
				move /= 2;
			
			if (IS_AFFECTED (ch, _AFF_SLOW))
				move *= 2;
			
			if (ch->move < move)
			{
				send_to_char ("You are too exhausted.\n\r", ch);
				return;
			}
			
		}
		else
		{
			if (IS_AFFECTED (MOUNTED (ch), _AFF_FLYING)
				|| IS_AFFECTED (MOUNTED (ch), _AFF_HASTE))
				move /= 2;
			
			if (IS_AFFECTED (MOUNTED (ch), _AFF_SLOW))
				move *= 2;
			
			if (MOUNTED (ch)->move < move)
			{
				send_to_char ("Your mount is too exhausted.\n\r", ch);
				return;
			}
		}
		
		WAIT_STATE (ch, 1);
		
		if (!MOUNTED (ch))
			ch->move -= move;
		else
			MOUNTED (ch)->move -= move;
		
  }
  
  if (RIDDEN (ch))
  {
	  CHAR_DATA *rch;
	  rch = RIDDEN (ch);
	  
	  if (!mount_success (rch, ch, FALSE))
	  {
		  act ("Your mount escapes your control, and leaves $T.", rch,
			  NULL, dir_name[door], TO_CHAR, POS_RESTING);
		  if (RIDDEN (ch))
			  ch = RIDDEN (ch);
	  }
	  else
	  {
		  send_to_char ("You steady your mount.\n\r", rch);
		  return;
	  }
  }
  
  for (fch = in_room->people; fch != NULL; fch = fch_next)
  {
	  fch_next = fch->next_in_room;
	  
	  if (ch != fch && can_see (fch, ch))
	  {
		  if (MOUNTED (ch))
		  {
			  if (!IS_AFFECTED (MOUNTED (ch), _AFF_FLYING))
				  sprintf (buf, "$n leaves $t, riding on %s.",
				  MOUNTED (ch)->short_descr);
			  else
				  sprintf (buf, "$n soars $t, on %s.", MOUNTED (ch)->short_descr);
			  act (buf, ch, dir_name[door], fch, TO_VICT, POS_RESTING);
		  }
		  else
			  if (!IS_NPC (ch) && ch->pcdata->bamfout[0] != '\0')
				  act ("$t", ch, ch->pcdata->bamfout, fch, TO_VICT, POS_RESTING);
			  else
				  if (IS_IMMORTAL (ch) /* bamfout must be null here */ )
					  act ("$n leaves in a swirling mist.",
					  ch, NULL, fch, TO_VICT, POS_RESTING);
				  else
					  act ("$n leaves $t.", ch, dir_name[door], fch, TO_VICT, POS_RESTING);
				  
				  
	  }
  }
  
  snd_door_trigger (ch, door, FALSE);
  
  char_from_room (ch);
  char_to_room (ch, to_room);
  
  snd_door_trigger (ch, door, TRUE);
  
  if (MOUNTED (ch))
  {
	  char_from_room (MOUNTED (ch));
	  char_to_room (MOUNTED (ch), to_room);
  }
  
  do_look (ch, "auto");
  
  if (in_room == to_room)	/* no circular follows */
	  return;

  // Check if the room they are entering has a tornado in it
  // Added for new weather system.
  // 11/06/02
  for (obj = to_room->contents; obj != NULL; obj = obj->next_content)
  {
	  if (!str_cmp(obj->name, "tornado"));
	  {
		  aff_tornado(ch);
	  }
  }
  
  
  if (IS_SET (to_room->room_flags, ROOM_NOSNEAK)
      && (IS_AFFECTED (ch, _AFF_SNEAK) || is_affected (ch, gsn_sneak)))
  {
	  send_to_char ("You resume a normal stance as you realize "
		  "you can't hide here.\n\r", ch);
	  affect_strip (ch, gsn_sneak);
  }
  
  /* Display poofs */
  for (fch = ch->in_room->people; fch != NULL; fch = fch_next)
  {
	  fch_next = fch->next_in_room;
	  
	  if (ch != fch && can_see (fch, ch))
	  {
		  if (!IS_NPC (ch) && ch->pcdata->bamfin[0] != '\0' && !MOUNTED (ch))
			  act ("$t", ch, ch->pcdata->bamfin, fch, TO_VICT, POS_RESTING);
		  else
			  if (IS_IMMORTAL (ch) /* bamfin must be null */ )
				  act ("$n appears in a swirling mist.",
				  ch, NULL, fch, TO_VICT, POS_RESTING);
			  else
				  if (!MOUNTED (ch))
					  act ("$n has arrived.", ch, NULL, fch, TO_VICT, POS_RESTING);
				  else
				  {
					  if (!IS_AFFECTED (MOUNTED (ch), _AFF_FLYING))
					  {
						  act ("$n has arrived, riding on $t.", ch,
							  MOUNTED (ch)->short_descr, fch, TO_VICT, POS_RESTING);
					  }
					  else
						  
					  {
						  act ("$n soars in, riding on $t.",
							  ch, MOUNTED (ch)->short_descr, fch, TO_VICT, POS_RESTING);
					  }
				  }
	  }
	  
	  if (fch != ch && (IS_EVIL (ch) && IS_AFFECTED (fch, _AFF_SENSE_EVIL)))
	  {
		  send_to_char ("A chill runs down your spine, "
			  "as if a dark shadow\n\rhad just crossed over you."
			  "\n\r", fch);
	  }
  }
  
  /* Grab any followers out of the old room... */
  for (fch = in_room->people; fch != NULL; fch = fch_next)
  {
	  fch_next = fch->next_in_room;
	  
	  if (fch->master == ch)
	  {
		  if (IS_AFFECTED (fch, _AFF_COMPULSION) && fch->position < POS_STANDING)
			  do_stand (fch, "");
		  
		  if (fch->position == POS_STANDING && can_see_room (fch, to_room))
		  {
			  if (IS_SET (ch->in_room->room_flags, ROOM_LAW) &&
				  (IS_NPC (fch) && IS_FLAG (fch->states.player, _ACT_AGGRESSIVE)))
			  {
				  act ("You can't bring $N into the city.",
					  ch, NULL, fch, TO_CHAR, POS_RESTING);
				  act ("You aren't allowed in the city.",
					  fch, NULL, NULL, TO_CHAR, POS_RESTING);
				  continue;
			  }
			  
			  act ("You follow $N.", fch, NULL, ch, TO_CHAR, POS_RESTING);
			  move_char (fch, door, TRUE);
		  }
	  }
	  else

      /* Red hand skill.  Follow them if your actively fighting
	   * and you havent lost more than 1/2 of your hp already. 
	   */
      if (fch->fighting == ch
		  && !IS_NPC (fch)
		  && is_clan (fch)
		  && IS_FLAG (clan_table[fch->clan].flags, _REDHAND)
		  && can_see_room (fch, to_room)
		  && fch->hit >= fch->max_hit / 2 && fch->position >= POS_FIGHTING)
	  {
		  chprintf (fch, "You hurry to catch up to %s\n\r", PERS (ch, fch));
		  
		  move_char (fch, door, TRUE);
	  }
	  
  }
  
  /* 
   * If someone is following the char, these triggers get activated
   * for the followers before the char, but it's safer this way...
   */
  if (IS_NPC (ch) && HAS_TRIGGER (ch, TRIG_ENTRY))
	  mp_percent_trigger (ch, NULL, NULL, NULL, TRIG_ENTRY);
  
  if (!IS_NPC (ch))
	  mp_greet_trigger (ch);
  
  return;
}


/*
Push and Drag code by: Kohl Desenee
-----------------------------------
Originally coded by Ferris
Modded by Kohl Desenee
Anthony Michael Tregre.
*/
void do_push_drag (CHAR_DATA * ch, char *argument, char *verb)
{
	char arg1[MIL];
	char arg2[MIL];
	char buf[MSL];
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	CHAR_DATA *victim;
	EXIT_DATA *pexit;
	OBJ_DATA *obj;
	int door;
	int chance = 0;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	victim = get_char_room (ch, arg1);
	obj = get_obj_list (ch, arg1, ch->in_room->contents);
	
	if (victim != NULL)		/* determine chance */
	{
		chance += ch->carry_weight;
		chance -= victim->carry_weight;
		
		if (ch->size < victim->size)
			chance += (ch->size - victim->size) * 15;
		else
			chance += (ch->size - victim->size) * 10;
		
		chance += get_curr_stat (ch, STAT_STR);
		chance -= (get_curr_stat (victim, STAT_DEX) * 4) / 3;
		chance -= (GET_AC (victim, AC_BASH) / 25);
		
		if (IS_FLAG (ch->states.off, _OFF_FAST) || IS_AFFECTED (ch, _AFF_HASTE))
			chance += 10;
		if (IS_FLAG (victim->states.off, _OFF_FAST)
			|| IS_AFFECTED (victim, _AFF_HASTE))
			chance -= 30;
		
		chance += (ch->level - victim->level);

		if ( victim->position == POS_MORTAL )
			chance = 100;
	}
	
	
	if ((arg1[0] == '\0' || arg2[0] == '\0') ||
		((!victim || !can_see (ch, victim))
		&& (!obj || !can_see_obj (ch, obj))))
	{
		sprintf (buf, "%s whom or what where?\n\r", capitalize (verb));
		send_to_char (buf, ch);
		return;
	}
	
	door = -1;
	
	if (arg2 && arg2[0])
	{
		switch (LOWER (arg2[0]))
		{
			case 'n':
				door = 0;
				break;
			case 'e':
				door = 1;
				break;
			case 's':
				door = 2;
				break;
			case 'w':
				door = 3;
				break;
			case 'u':
				door = 4;
				break;
			case 'd':
				door = 5;
				break;
		}
	}
	
	if (door == -1)
	{
		chprintf (ch, "You cannot %s in that direction.\n\r", verb);
		return;
	}
	
	if (obj)
	{
		in_room = obj->in_room;
		if ((pexit = in_room->exit[door]) == NULL
			|| (to_room = pexit->u1.to_room) == NULL
			|| !can_see_room (ch, pexit->u1.to_room))
		{
			sprintf (buf, "Alas, you cannot %s in that direction.\n\r", verb);
			send_to_char (buf, ch);
			return;
		}
		
		if (IS_SET (pexit->exit_info, EX_CLOSED)
			|| IS_SET (pexit->exit_info, EX_NOPASS))
		{
			act ("You cannot $t it through the $d.",
				ch, verb, pexit->keyword, TO_CHAR, POS_RESTING);
			act ("$n decides to $t $P around!",
				ch, verb, obj, TO_ROOM, POS_RESTING);
			return;
		}
		
		if (!CAN_WEAR (obj, ITEM_TAKE))
		{
			act ("You cannot $T $p.", ch, obj, verb, TO_CHAR, POS_RESTING);
			return;
		}
		
		act ("You attempt to $T $p out of the room.",
			ch, obj, verb, TO_CHAR, POS_RESTING);
		act ("$n is attempting to $T $p out of the room.",
			ch, obj, verb, TO_ROOM, POS_RESTING);
		
		if (obj->weight > (2 * can_carry_w (ch)))
		{
			act ("$p is too heavy to $T.\n\r", ch, obj, verb, TO_CHAR, POS_RESTING);
			act ("$n attempts to $T $p, but it is too heavy.\n\r",
				ch, obj, verb, TO_ROOM, POS_RESTING);
			return;
		}
		
		if (!IS_IMMORTAL (ch) &&
			(IS_SET (ch->in_room->room_flags, ROOM_SAFE) ||
			IS_SET (ch->in_room->room_flags, ROOM_PRIVATE) ||
			IS_SET (ch->in_room->room_flags, ROOM_SOLITARY)))
		{
			send_to_char ("It won't budge.\n\r", ch);
			return;
		}
		
		if (ch->move > 10)
		{
			ch->move -= 10;
			send_to_char ("You succeed!\n\r", ch);
			act ("$n succeeds!", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			
			if (!str_cmp (verb, "drag"))
			{
				act ("$n drags $p $T!",
					ch, obj, dir_name[door], TO_ROOM, POS_RESTING);
				char_from_room (ch);
				char_to_room (ch, pexit->u1.to_room);
				do_look (ch, "auto");
				obj_from_room (obj);
				obj_to_room (obj, to_room);
				act ("$n drags $p into the room.",
					ch, obj, dir_name[door], TO_ROOM, POS_RESTING);
			}
			else if (!str_cmp (verb, "push"))
				
			{
				act ("$p flies $T!", ch, obj, dir_name[door], TO_ROOM, POS_RESTING);
				act ("$p flies $T!", ch, obj, dir_name[door], TO_CHAR, POS_RESTING);
				char_from_room (ch);
				char_to_room (ch, pexit->u1.to_room);
				act ("You notice movement from nearby to the $T.",
					ch, NULL, dir_name[rev_dir[door]], TO_ROOM, POS_RESTING);
				act ("$p flies into the room!",
					ch, obj, dir_name[door], TO_ROOM, POS_RESTING);
				char_from_room (ch);
				char_to_room (ch, in_room);
				obj_from_room (obj);
				obj_to_room (obj, to_room);
			}
		}
		else
		{
			sprintf (buf, "You are too tired to %s anything around!\n\r", verb);
			send_to_char (buf, ch);
		}
	}
	else
		
	{
		if (ch == victim)
		{
			act ("You $t yourself about the room and look very silly.",
				ch, verb, NULL, TO_CHAR, POS_RESTING);
			act ("$n decides to be silly and $t $mself about the room.",
				ch, verb, NULL, TO_ROOM, POS_RESTING);
			return;
		}
		
		in_room = victim->in_room;
		if ((pexit = in_room->exit[door]) == NULL
			|| (to_room = pexit->u1.to_room) == NULL
			|| !can_see_room (victim, pexit->u1.to_room))
		{
			sprintf (buf, "Alas, you cannot %s them that way.\n\r", verb);
			send_to_char (buf, ch);
			return;
		}
		
		if (IS_SET (pexit->exit_info, EX_CLOSED)
			&& (!IS_AFFECTED (victim, _AFF_PASS_DOOR)
			|| IS_SET (pexit->exit_info, EX_NOPASS)))
		{
			act ("You try to $t them through the $d.",
				ch, verb, pexit->keyword, TO_CHAR, POS_RESTING);
			act ("$n decides to $t you around!",
				ch, verb, victim, TO_VICT, POS_RESTING);
			act ("$n decides to $t $N around!",
				ch, verb, victim, TO_NOTVICT, POS_RESTING);
			return;
		}
		
		act ("You attempt to $t $N out of the room.",
			ch, verb, victim, TO_CHAR, POS_RESTING);
		act ("$n is attempting to $t you out of the room!",
			ch, verb, victim, TO_VICT, POS_RESTING);
		act ("$n is attempting to $t $N out of the room.",
			ch, verb, victim, TO_NOTVICT, POS_RESTING);
		
		if ((!IS_IMMORTAL (ch) &&
			(IS_NPC (victim) &&
			(IS_FLAG (victim->states.player, _ACT_TRAIN) ||
			IS_FLAG (victim->states.player, _ACT_PRACTICE) ||
			IS_FLAG (victim->states.player, _ACT_HEALER) ||
			IS_FLAG (victim->states.player, _ACT_BANKER) ||
			IS_FLAG (victim->states.imm, _PUSH) ||
			IS_SET (victim->in_room->room_flags, ROOM_SAFE) ||
			victim->pIndexData->pShop ||
			is_safe (ch, victim))))
			|| (victim->in_room == NULL ||
			(!str_cmp (verb, "push") && victim->position != POS_STANDING)))
		{
			send_to_char ("They won't budge.\n\r", ch);
			return;
		}
		
		if (IS_FLAG (victim->states.res, _PUSH))
			chance /= 2;
		else if (IS_FLAG (victim->states.vuln, _PUSH))
			chance += (int)(chance * .50);

		if ( victim->position == POS_MORTAL )
			chance = 100;

		if (number_percent () <= chance)
		{
			ch->move -= 10;
			
			if (!str_cmp (verb, "drag"))
			{
				move_char (ch, door, FALSE);
				act ("You drag $n $T.",
					ch, victim, dir_name[door], TO_CHAR, POS_RESTING);
				act ("$n is dragged $T!",
					victim, NULL, dir_name[door], TO_ROOM, POS_RESTING);
				act ("You are dragged $T!\n\r",
					victim, NULL, dir_name[door], TO_CHAR, POS_DEAD);
				
				char_from_room (victim);
				char_to_room (victim, pexit->u1.to_room);
				
				do_look (victim, "auto");
				act ("$N drags $n into the room.",
					victim, NULL, ch, TO_NOTVICT, POS_RESTING);
			}
			else
				if (!str_cmp (verb, "push"))
				{
					act ("You push $N $t.",
						ch, dir_name[door], victim, TO_CHAR, POS_RESTING);
					act ("$n flies $T!",
						victim, NULL, dir_name[door], TO_ROOM, POS_RESTING);
					act ("You fly $T!\n\r",
						victim, NULL, dir_name[door], TO_CHAR, POS_DEAD);
					
					char_from_room (victim);
					char_to_room (victim, pexit->u1.to_room);
					
					do_look (victim, "auto");
					act ("You notice movement from nearby to the $T.",
						victim, NULL, dir_name[rev_dir[door]], TO_ROOM, POS_RESTING);
					act ("$n flies into the room!",
						victim, NULL, NULL, TO_ROOM, POS_RESTING);
				}
		}
		
		else
		{
			act ("$N evades your attempts to $t $M!",
				ch, verb, victim, TO_CHAR, POS_RESTING);
			act ("$N evades $n's attempt's to $t $M!",
				ch, verb, victim, TO_NOTVICT, POS_RESTING);
			act ("You evaded $n's attempt to $t you!",
				ch, verb, victim, TO_VICT, POS_RESTING);
		}
  }
  
  return;
}

_DOFUN (do_push)
{
	do_push_drag (ch, argument, "push");
	return;
}

_DOFUN (do_drag)
{
	do_push_drag (ch, argument, "drag");
	return;
}

_DOFUN (do_north)
{
	move_char (ch, DIR_NORTH, FALSE);
	return;
}

_DOFUN (do_east)
{
	move_char (ch, DIR_EAST, FALSE);
	return;
}



_DOFUN (do_south)
{
	move_char (ch, DIR_SOUTH, FALSE);
	return;
}



_DOFUN (do_west)
{
	move_char (ch, DIR_WEST, FALSE);
	return;
}



_DOFUN (do_up)
{
	move_char (ch, DIR_UP, FALSE);
	return;
}



_DOFUN (do_down)
{
	move_char (ch, DIR_DOWN, FALSE);
	return;
}



int find_door (CHAR_DATA * ch, char *arg)
{
	EXIT_DATA *pexit;
	int door;
	
	if (!str_cmp (arg, "n") || !str_cmp (arg, "north"))
		door = 0;
	else if (!str_cmp (arg, "e") || !str_cmp (arg, "east"))
		door = 1;
	else if (!str_cmp (arg, "s") || !str_cmp (arg, "south"))
		door = 2;
	else if (!str_cmp (arg, "w") || !str_cmp (arg, "west"))
		door = 3;
	else if (!str_cmp (arg, "u") || !str_cmp (arg, "up"))
		door = 4;
	else if (!str_cmp (arg, "d") || !str_cmp (arg, "down"))
		door = 5;
	else
	{
		for (door = 0; door <= 5; door++)
		{
			if ((pexit = ch->in_room->exit[door]) != NULL
				&& IS_SET (pexit->exit_info, EX_ISDOOR)
				&& pexit->keyword != NULL && is_name (arg, pexit->keyword))
				return door;
		}
		act ("I see no $T here.", ch, NULL, arg, TO_CHAR, POS_RESTING);
		return -1;
	}
	
	if ((pexit = ch->in_room->exit[door]) == NULL)
	{
		act ("I see no door $T here.", ch, NULL, arg, TO_CHAR, POS_RESTING);
		return -1;
	}
	
	if (!IS_SET (pexit->exit_info, EX_ISDOOR))
	{
		send_to_char ("You can't do that.\n\r", ch);
		return -1;
	}
	
	return door;
}



_DOFUN (do_open)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int door;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Open what?\n\r", ch);
		return;
	}
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't reach the lock from your mount.\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* open portal */
		if (obj->item_type == ITEM_PORTAL || obj->item_type == ITEM_WAYGATE)
		{
			if (!IS_SET (obj->value[1], EX_ISDOOR))
			{
				send_to_char ("You can't seem to find a door.\n\r", ch);
				return;
			}
			
			if (!IS_SET (obj->value[1], EX_CLOSED))
			{
				send_to_char ("It's already open.\n\r", ch);
				return;
			}
			
			if (IS_SET (obj->value[1], EX_LOCKED))
			{
				send_to_char ("It's locked.\n\r", ch);
				return;
			}
			
			REMOVE_BIT (obj->value[1], EX_CLOSED);
			act ("You open $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			act ("$n opens $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			return;
		}
		
		/* 'open object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's already open.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's locked.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (obj->value[1], CONT_CLOSED);
		act ("You open $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		act ("$n opens $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'open door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;
		
		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's already open.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's locked.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (pexit->exit_info, EX_CLOSED);
		act ("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING);
		send_to_char ("Ok.\n\r", ch);
		
		/* open the other side */
		if ((to_room = pexit->u1.to_room) != NULL
			&& (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
			&& pexit_rev->u1.to_room == ch->in_room)
		{
			CHAR_DATA *rch;
			
			REMOVE_BIT (pexit_rev->exit_info, EX_CLOSED);
			for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
				act ("The $d opens.",
				rch, NULL, pexit_rev->keyword, TO_CHAR, POS_RESTING);
		}
	}
	
	return;
}



_DOFUN (do_close)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int door;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Close what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* portal stuff */
		if (obj->item_type == ITEM_PORTAL || obj->item_type == ITEM_WAYGATE)
		{
			
			if (!IS_SET (obj->value[1], EX_ISDOOR)
				|| IS_SET (obj->value[1], EX_NOCLOSE))
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			
			if (IS_SET (obj->value[1], EX_CLOSED))
			{
				send_to_char ("It's already closed.\n\r", ch);
				return;
			}
			
			/* Only portals with an 'owner' field will have been
			 * created with the 'travelling' weave.  These we can
			 * ASSume to be ok to destroy the portal (and it's mirror)
			 */
			
			if (obj->owner != NULL && !str_prefix (obj->owner, ch->name))
			{
				OBJ_DATA *mirror;
				ROOM_INDEX_DATA *dest;
				
				act ("You close $p in on itself.",
					ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n closes $p in on itself.",
					ch, obj, NULL, TO_ROOM, POS_RESTING);
				
				dest = get_room_index (obj->value[3]);
				obj_from_room (obj);
				
				if (dest)
					for (mirror = dest->contents; mirror != NULL;
					mirror = mirror->next_content)
					{
						if (mirror->item_type == ITEM_PORTAL
							&& mirror->value[3] == ch->in_room->vnum
							&& (mirror->owner && !str_prefix (mirror->owner, ch->name)))
						{
							act ("The $p closes in on itself!",
								dest->people, mirror, NULL, TO_ROOM, POS_RESTING);
							act ("The $p closes in on itself!",
								dest->people, mirror, NULL, TO_CHAR, POS_RESTING);
							obj_from_room (mirror);
						}
					}
					
					return;
			}
			
			if (obj->item_type == ITEM_PORTAL)
			{
				send_to_char ("And just how do you propose to close THAT?\n\r", ch);
				return;
			}
			
			SET_BIT (obj->value[1], EX_CLOSED);
			act ("You close $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			act ("$n closes $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			return;
		}
		
		/* 'close object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's already closed.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSEABLE))
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}
		
		SET_BIT (obj->value[1], CONT_CLOSED);
		act ("You close $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		act ("$n closes $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'close door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;
		
		pexit = ch->in_room->exit[door];
		if (IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's already closed.\n\r", ch);
			return;
		}
		
		SET_BIT (pexit->exit_info, EX_CLOSED);
		act ("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING);
		send_to_char ("Ok.\n\r", ch);
		
		/* close the other side */
		if ((to_room = pexit->u1.to_room) != NULL
			&& (pexit_rev = to_room->exit[rev_dir[door]]) != 0
			&& pexit_rev->u1.to_room == ch->in_room)
		{
			CHAR_DATA *rch;
			
			SET_BIT (pexit_rev->exit_info, EX_CLOSED);
			for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
				act ("The $d closes.",
				rch, NULL, pexit_rev->keyword, TO_CHAR, POS_RESTING);
		}
	}
	
	return;
}



bool has_key (CHAR_DATA * ch, int key)
{
	OBJ_DATA *obj;
	
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->pIndexData->vnum == key)
			return TRUE;
	}
	
	return FALSE;
}



_DOFUN (do_lock)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int door;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Lock what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* portal stuff */
		if (obj->item_type == ITEM_PORTAL || obj->item_type == ITEM_WAYGATE)
		{
			if (!IS_SET (obj->value[1], EX_ISDOOR)
				|| IS_SET (obj->value[1], EX_NOCLOSE))
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			if (!IS_SET (obj->value[1], EX_CLOSED))
			{
				send_to_char ("It's not closed.\n\r", ch);
				return;
			}
			
			if (obj->value[4] < 0 || IS_SET (obj->value[1], EX_NOLOCK))
			{
				send_to_char ("It can't be locked.\n\r", ch);
				return;
			}
			
			if (!has_key (ch, obj->value[4]))
			{
				send_to_char ("You lack the key.\n\r", ch);
				return;
			}
			
			if (IS_SET (obj->value[1], EX_LOCKED))
			{
				send_to_char ("It's already locked.\n\r", ch);
				return;
			}
			
			SET_BIT (obj->value[1], EX_LOCKED);
			act ("You lock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			act ("$n locks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			return;
		}
		
		/* 'lock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("It can't be locked.\n\r", ch);
			return;
		}
		if (!has_key (ch, obj->value[2]))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already locked.\n\r", ch);
			return;
		}
		
		SET_BIT (obj->value[1], CONT_LOCKED);
		act ("You lock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		act ("$n locks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'lock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;
		
		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("It can't be locked.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's already locked.\n\r", ch);
			return;
		}
		
		SET_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING);
		
		/* lock the other side */
		if ((to_room = pexit->u1.to_room) != NULL
			&& (pexit_rev = to_room->exit[rev_dir[door]]) != 0
			&& pexit_rev->u1.to_room == ch->in_room)
		{
			SET_BIT (pexit_rev->exit_info, EX_LOCKED);
		}
	}
	
	return;
}



_DOFUN (do_unlock)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int door;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Unlock what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* portal stuff */
		if (obj->item_type == ITEM_PORTAL || obj->item_type == ITEM_WAYGATE)
		{
			if (!IS_SET (obj->value[1], EX_ISDOOR))
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			
			if (!IS_SET (obj->value[1], EX_CLOSED))
			{
				send_to_char ("It's not closed.\n\r", ch);
				return;
			}
			
			if (obj->value[4] < 0)
			{
				send_to_char ("It can't be unlocked.\n\r", ch);
				return;
			}
			
			if (!has_key (ch, obj->value[4]))
			{
				send_to_char ("You lack the key.\n\r", ch);
				return;
			}
			
			if (!IS_SET (obj->value[1], EX_LOCKED))
			{
				send_to_char ("It's already unlocked.\n\r", ch);
				return;
			}
			
			REMOVE_BIT (obj->value[1], EX_LOCKED);
			act ("You unlock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			return;
		}
		
		/* 'unlock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("It can't be unlocked.\n\r", ch);
			return;
		}
		if (!has_key (ch, obj->value[2]))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (obj->value[1], CONT_LOCKED);
		act ("You unlock $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;
		
		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("It can't be unlocked.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (!IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n unlocks the $d.",
			ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING);
		
		/* unlock the other side */
		if ((to_room = pexit->u1.to_room) != NULL
			&& (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
			&& pexit_rev->u1.to_room == ch->in_room)
		{
			REMOVE_BIT (pexit_rev->exit_info, EX_LOCKED);
		}
	}
	
	return;
}



_DOFUN (do_pick)
{
	char arg[MIL];
	CHAR_DATA *gch;
	OBJ_DATA *obj;
	int door;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Pick what?\n\r", ch);
		return;
	}
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't reach the lock from your mount.\n\r", ch);
		return;
	}
	
	WAIT_STATE (ch, skill_table[skill_lookup("pick lock")].beats);
	
	/* look for guards */
	for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
	{
		if (IS_NPC (gch) && IS_AWAKE (gch) && ch->level + 5 < gch->level)
		{
			act ("$N is standing too close to the lock.",
				ch, NULL, gch, TO_CHAR, POS_RESTING);
			return;
		}
	}
	
	if (!IS_NPC (ch) && number_percent () > get_skill (ch, skill_lookup("pick lock")))
	{
		send_to_char ("You failed.\n\r", ch);
		check_improve (ch, skill_lookup("pick lock"), FALSE, 2);
		return;
	}
	
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* portal stuff */
		if (obj->item_type == ITEM_PORTAL || obj->item_type == ITEM_WAYGATE)
		{
			if (!IS_SET (obj->value[1], EX_ISDOOR))
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			
			if (!IS_SET (obj->value[1], EX_CLOSED))
			{
				send_to_char ("It's not closed.\n\r", ch);
				return;
			}
			
			if (obj->value[4] < 0)
			{
				send_to_char ("It can't be unlocked.\n\r", ch);
				return;
			}
			
			if (IS_SET (obj->value[1], EX_PICKPROOF))
			{
				send_to_char ("You failed.\n\r", ch);
				return;
			}
			
			REMOVE_BIT (obj->value[1], EX_LOCKED);
			act ("You pick the lock on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			act ("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			check_improve (ch, skill_lookup("pick lock"), TRUE, 2);
			return;
		}
		
		/* 'pick object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("It can't be unlocked.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_PICKPROOF))
		{
			send_to_char ("You failed.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (obj->value[1], CONT_LOCKED);
		act ("You pick the lock on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		act ("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		check_improve (ch, skill_lookup("pick lock"), TRUE, 2);
		return;
	}
	
	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'pick door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;
		
		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL (ch))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0 && !IS_IMMORTAL (ch))
		{
			send_to_char ("It can't be picked.\n\r", ch);
			return;
		}
		if (!IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL (ch))
		{
			send_to_char ("You failed.\n\r", ch);
			return;
		}
		
		REMOVE_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM, POS_RESTING);
		check_improve (ch, skill_lookup("pick lock"), TRUE, 2);
		
		/* pick the other side */
		if ((to_room = pexit->u1.to_room) != NULL
			&& (pexit_rev = to_room->exit[rev_dir[door]]) != NULL
			&& pexit_rev->u1.to_room == ch->in_room)
		{
			REMOVE_BIT (pexit_rev->exit_info, EX_LOCKED);
		}
	}
	
	return;
}


_DOFUN (do_stand)
{
	OBJ_DATA *obj = NULL;
	
	if (argument[0] != '\0')
	{
		if (ch->position == POS_FIGHTING)
		{
			send_to_char ("Maybe you should finish fighting first?\n\r", ch);
			return;
		}
		obj = get_obj_list (ch, argument, ch->in_room->contents);
		if (obj == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
		if (obj->item_type != ITEM_FURNITURE
			|| (!IS_SET (obj->value[2], STAND_AT)
			&& !IS_SET (obj->value[2], STAND_ON)
			&& !IS_SET (obj->value[2], STAND_IN)))
		{
			send_to_char ("You can't seem to find a place to stand.\n\r", ch);
			return;
		}
		if (ch->on != obj && count_users (obj) >= obj->value[0])
		{
			act ("There's no room to stand on $p.",
				ch, obj, NULL, TO_CHAR, POS_RESTING);
			return;
		}
		ch->on = obj;
	}
	
	switch (ch->position)
	{
		
			case POS_SLEEPING:
				if (IS_AFFECTED (ch, _AFF_SLEEP))
				{
					send_to_char ("You can't wake up!\n\r", ch);
					return;
				}
				
				if (obj == NULL)
				{
					send_to_char ("You wake and stand up.\n\r", ch);
					act ("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
					ch->on = NULL;
				}
				else if (IS_SET (obj->value[2], STAND_AT))
				{
					act ("You wake and stand at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], STAND_ON))
				{
					act ("You wake and stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else
				{
					act ("You wake and stand in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				ch->position = POS_STANDING;
				do_look (ch, "auto");
				break;
				
			case POS_RESTING:
			case POS_SITTING:
				if (obj == NULL)
				{
					send_to_char ("You stand up.\n\r", ch);
					act ("$n stands up.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
					ch->on = NULL;
				}
				else if (IS_SET (obj->value[2], STAND_AT))
				{
					act ("You stand at $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], STAND_ON))
				{
					act ("You stand on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else
				{
					act ("You stand in $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				ch->position = POS_STANDING;
				break;
				
			case POS_STANDING:
				if (obj == NULL)
					send_to_char ("You are already standing.\n\r", ch);
				
				else if (IS_SET (obj->value[2], STAND_AT))
				{
					act ("You stand at $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], STAND_ON))
				{
					act ("You stand on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else
				{
					act ("You stand in $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n stands on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				break;
				
			case POS_FIGHTING:
				send_to_char ("You are already fighting!\n\r", ch);
				break;
				
			default:
				break;
	}
	
	return;
}



_DOFUN (do_rest)
{
	OBJ_DATA *obj = NULL;
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't rest while mounted.\n\r", ch);
		return;
	}
	
	if (ch->position == POS_FIGHTING)
	{
		send_to_char ("You are already fighting!\n\r", ch);
		return;
	}
	
	/* okay, now that we know we can rest, find an object to rest on */
	if (argument[0] != '\0')
	{
		obj = get_obj_list (ch, argument, ch->in_room->contents);
		if (obj == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
	}
	else
		obj = ch->on;
	
	if (obj != NULL)
	{
		if (obj->item_type != ITEM_FURNITURE
			|| (!IS_SET (obj->value[2], REST_ON)
			&& !IS_SET (obj->value[2], REST_IN)
			&& !IS_SET (obj->value[2], REST_AT)))
		{
			send_to_char ("You can't rest on that.\n\r", ch);
			return;
		}
		
		if (obj != NULL && ch->on != obj && count_users (obj) >= obj->value[0])
		{
			act ("There's no more room on $p.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			return;
		}
		
		ch->on = obj;
	}
	
	switch (ch->position)
	{
		case POS_SLEEPING:
			if (IS_AFFECTED (ch, _AFF_SLEEP))
			{
				send_to_char ("You can't wake up!\n\r", ch);
				return;
			}
			
			if (obj == NULL)
			{
				send_to_char ("You wake up and start resting.\n\r", ch);
				act ("$n wakes up and starts resting.",
					ch, NULL, NULL, TO_ROOM, POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_AT))
			{
				act ("You wake up and rest at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
				act ("$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM,
					POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_ON))
			{
				act ("You wake up and rest on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
				act ("$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM,
					POS_RESTING);
			}
			else
			{
				act ("You wake up and rest in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
				act ("$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM,
					POS_RESTING);
			}
			ch->position = POS_RESTING;
			break;
			
		case POS_RESTING:
			send_to_char ("You are already resting.\n\r", ch);
			break;
			
		case POS_STANDING:
			if (obj == NULL)
			{
				send_to_char ("You rest.\n\r", ch);
				act ("$n sits down and rests.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_AT))
			{
				act ("You sit down at $p and rest.", ch, obj, NULL, TO_CHAR,
					POS_RESTING);
				act ("$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM,
					POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_ON))
			{
				act ("You sit on $p and rest.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n sits on $p and rests.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			}
			else
			{
				act ("You rest in $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n rests in $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			}
			ch->position = POS_RESTING;
			break;
			
		case POS_SITTING:
			if (obj == NULL)
			{
				send_to_char ("You rest.\n\r", ch);
				act ("$n rests.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_AT))
			{
				act ("You rest at $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n rests at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			}
			else if (IS_SET (obj->value[2], REST_ON))
			{
				act ("You rest on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n rests on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			}
			else
			{
				act ("You rest in $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				act ("$n rests in $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			}
			ch->position = POS_RESTING;
			break;
			
		default:
			break;
	}
	
	
	return;
}


_DOFUN (do_sit)
{
	OBJ_DATA *obj = NULL;
	
	if (ch->position == POS_FIGHTING)
	{
		send_to_char ("Maybe you should finish this fight first?\n\r", ch);
		return;
	}
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't sit while mounted.\n\r", ch);
		return;
	}
	
	
	/* okay, now that we know we can sit, find an object to sit on */
	if (argument[0] != '\0')
	{
		obj = get_obj_list (ch, argument, ch->in_room->contents);
		if (obj == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
	}
	else
		obj = ch->on;
	
	if (obj != NULL)
	{
		if (obj->item_type != ITEM_FURNITURE
			|| (!IS_SET (obj->value[2], SIT_ON)
			&& !IS_SET (obj->value[2], SIT_IN)
			&& !IS_SET (obj->value[2], SIT_AT)))
		{
			send_to_char ("You can't sit on that.\n\r", ch);
			return;
		}
		
		if (obj != NULL && ch->on != obj && count_users (obj) >= obj->value[0])
		{
			act ("There's no more room on $p.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			return;
		}
		
		ch->on = obj;
	}
	switch (ch->position)
	{
			case POS_SLEEPING:
				if (IS_AFFECTED (ch, _AFF_SLEEP))
				{
					send_to_char ("You can't wake up!\n\r", ch);
					return;
				}
				
				if (obj == NULL)
				{
					send_to_char ("You wake and sit up.\n\r", ch);
					act ("$n wakes and sits up.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], SIT_AT))
				{
					act ("You wake and sit at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], SIT_ON))
				{
					act ("You wake and sit on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else
				{
					act ("You wake and sit in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
					act ("$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				
				ch->position = POS_SITTING;
				break;
			case POS_RESTING:
				if (obj == NULL)
					send_to_char ("You stop resting.\n\r", ch);
				else if (IS_SET (obj->value[2], SIT_AT))
				{
					act ("You sit at $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n sits at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				
				else if (IS_SET (obj->value[2], SIT_ON))
				{
					act ("You sit on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n sits on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				ch->position = POS_SITTING;
				break;
			case POS_SITTING:
				send_to_char ("You are already sitting down.\n\r", ch);
				break;
			case POS_STANDING:
				if (obj == NULL)
				{
					send_to_char ("You sit down.\n\r", ch);
					act ("$n sits down on the ground.",
						ch, NULL, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], SIT_AT))
				{
					act ("You sit down at $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n sits down at $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else if (IS_SET (obj->value[2], SIT_ON))
				{
					act ("You sit on $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n sits on $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				else
				{
					act ("You sit down in $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
					act ("$n sits down in $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				}
				ch->position = POS_SITTING;
				break;
				
			default:
				break;
	}
	return;
}


_DOFUN (do_sleep)
{
	OBJ_DATA *obj = NULL;
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't sleep while mounted.\n\r", ch);
		return;
	}
	
	switch (ch->position)
	{
		case POS_SLEEPING:
			send_to_char ("You are already sleeping.\n\r", ch);
			break;
			
		case POS_RESTING:
		case POS_SITTING:
		case POS_STANDING:
			{
				if (argument[0] == '\0' && ch->on == NULL)
				{
					send_to_char ("You go to sleep.\n\r", ch);
					act ("$n goes to sleep.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
					ch->position = POS_SLEEPING;
				}
				else			/* find an object and sleep on it */
				{
					if (argument[0] == '\0')
						obj = ch->on;
					else
						obj = get_obj_list (ch, argument, ch->in_room->contents);
					
					if (obj == NULL)
					{
						send_to_char ("You don't see that here.\n\r", ch);
						return;
					}
					if (obj->item_type != ITEM_FURNITURE
						|| (!IS_SET (obj->value[2], SLEEP_ON)
						&& !IS_SET (obj->value[2], SLEEP_IN)
						&& !IS_SET (obj->value[2], SLEEP_AT)))
					{
						send_to_char ("You can't sleep on that!\n\r", ch);
						return;
					}
					
					if (ch->on != obj && count_users (obj) >= obj->value[0])
					{
						act ("There is no room on $p for you.",
							ch, obj, NULL, TO_CHAR, POS_RESTING);
						return;
					}
					
					ch->on = obj;
					if (IS_SET (obj->value[2], SLEEP_AT))
					{
						act ("You go to sleep at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
						act ("$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM,
							POS_RESTING);
					}
					else if (IS_SET (obj->value[2], SLEEP_ON))
					{
						act ("You go to sleep on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
						act ("$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM,
							POS_RESTING);
					}
					else
					{
						act ("You go to sleep in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
						act ("$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM,
							POS_RESTING);
					}
					ch->position = POS_SLEEPING;
				}
				
				if (!snd_trigger (ch, ch->sounds, SOUND_ROOM, SOUND_ROOM_SLEEP, TRUE))
					snd_trigger (ch, ch->sounds, SOUND_AREA, SOUND_AREA_SLEEP, TRUE);
			}
			break;
			
		case POS_FIGHTING:
			send_to_char ("You are already fighting!\n\r", ch);
			break;
			
		default:
			break;
	}
	
	return;
}



	
_DOFUN (do_wake)
{
	char arg[MIL];
	CHAR_DATA *victim;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{

		if ( IS_AFFECTED(ch, _AFF_DREAMING) )
		  do_dream(ch, "wake");
		else
		  do_stand (ch, argument);

		return;
	}
	
	if (!IS_AWAKE (ch))
	{
		send_to_char ("You are asleep yourself!\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_AWAKE (victim))
	{
		act ("$N is already awake.", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}

	if (IS_AFFECTED (victim, _AFF_SLEEP))
	{
		act ("You can't wake $M!", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}

	if ( IS_AFFECTED(victim, _AFF_DREAMING) )
	  do_dream(victim, "wake");
	
	else
	  do_stand (victim, "");

	act ("$n woke you.", ch, NULL, victim, TO_VICT, POS_DEAD);

	if (!snd_trigger
		(victim, victim->sounds, SOUND_ROOM, SOUND_ROOM_WAKE, TRUE))
		snd_trigger (victim, victim->sounds, SOUND_AREA, SOUND_AREA_WAKE, TRUE);
	return;
}



_DOFUN (do_sneak)
{
	AFFECT_DATA af;
	int iSkill = gsn_sneak;
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't sneak while riding.\n\r", ch);
		return;
	}
	
	send_to_char ("You attempt to move silently.\n\r", ch);
	affect_strip (ch, iSkill);
	
	if (IS_AFFECTED (ch, _AFF_SNEAK))
		return;
	
	if (IS_SET (ch->in_room->room_flags, ROOM_NOSNEAK))
	{
		send_to_char ("You are either stupid or blind to think you"
			" can sneak around here.\n\r", ch);
		return;
	}
	
	if (number_percent () < get_skill (ch, iSkill))
	{
		check_improve (ch, iSkill, TRUE, 3);
		af.where = TO_AFFECTS;
		af.type = gsn_sneak;
		af.level = ch->level;
		af.duration = ch->level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = _AFF_SNEAK;
		affect_to_char (ch, &af);
	}
	else
		check_improve (ch, iSkill, FALSE, 3);
	
	return;
}



_DOFUN (do_hide)
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	char arg[MIL];
	int iSkill = skill_lookup("hide");

	argument = one_argument (argument, arg);
	
	if (MOUNTED (ch))
	{
		send_to_char ("You can't hide while riding.\n\r", ch);
		return;
	}
	
	if (arg[0] != '\0')
	{
		if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
		{
			send_to_char ("You don't have that item.\n\r", ch);
			return;
		}
		obj_from_char (obj);
		SET_BIT (obj->extra_flags, ITEM_HIDDEN);
		obj_to_room (obj, ch->in_room);
		act ("$n hides $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You hide $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		return;
	}
	
	send_to_char ("You attempt to hide.\n\r", ch);
	
	if (IS_AFFECTED (ch, _AFF_HIDE))
		REMOVE_FLAG (ch->states.affected_by, _AFF_HIDE);
	
	if (number_percent () < get_skill (ch, iSkill))
	{
		check_improve (ch, iSkill, TRUE, 3);
		af.where = TO_AFFECTS;
		af.type = gsn_hide;
		af.level = ch->level;
		af.duration = ch->level;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = _AFF_HIDE;
		affect_to_char (ch, &af);
	}
	else
		check_improve (ch, iSkill, FALSE, 3);

	return;
}



/*
 * Contributed by Alander.
 */
_DOFUN (do_visible)
{
	// FIXME: nasty hack...
	if (is_affected (ch, gsn_cloak))
	{
		act ("$n tosses back his fancloth cloak, seeming to step out of the air.",
			ch, NULL, NULL, TO_ROOM, POS_RESTING);
	}
	
	affect_strip (ch, skill_lookup("invisibility"));
	affect_strip (ch, skill_lookup("mass invisibility"));
	affect_strip (ch, gsn_sneak);
	affect_strip (ch, gsn_cloak);
	affect_strip (ch, gsn_hide);
	send_to_char ("Ok.\n\r", ch);
	return;
}



_DOFUN (do_recall)
{
	char buf[MSL];
	CHAR_DATA *victim;
	ROOM_INDEX_DATA *location = NULL;
	
	if (IS_NPC (ch) && !IS_FLAG (ch->states.player, _ACT_PET) && !ch->mount)
	{
		send_to_char ("Only players can recall.\n\r", ch);
		return;
	}
	
	if (IS_FLAG (ch->states.penalties, _NORECALL))
	{
		send_to_char ("Recall? What's that?\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_BOUND_AIR))
	{
		send_to_char
			("You have been bound in Air, how do you expect to move?\n\r", ch);
		act
			("$n attempts to recall, but cannot move through the air surrounding $s",
			ch, NULL, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	
	act ("$n prays for transportation!", ch, 0, 0, TO_ROOM, POS_RESTING);
	
	if (!IS_NPC (ch) && ch->pcdata->recall_to > 0)
		location = get_room_index (ch->pcdata->recall_to);
	if (location == NULL && is_clan (ch))
		location = get_room_index (clan_table[ch->clan].room[0]);
	if (location == NULL)
		location = get_room_index (ROOM_VNUM_LOGIN);
	
	if (location == NULL)
	{
		send_to_char ("You are completely lost.\n\r", ch);
		return;
	}
	
	if (!ch->in_room || ch->in_room == location)
		return;
	
	if (IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL))
	{
		send_to_char ("The Creator has forsaken you.\n\r", ch);
		return;
	}
	
	if ((victim = ch->fighting) != NULL)
	{
		int lose, skill, iSkill = skill_lookup("recall");
		
		skill = get_skill (ch, iSkill);
		
		if (number_percent () < 80 * skill / 100)
		{
			check_improve (ch, iSkill, FALSE, 6);
			WAIT_STATE (ch, 4);
			sprintf (buf, "You failed!.\n\r");
			send_to_char (buf, ch);
			return;
		}
		
		lose = (ch->desc != NULL) ? 25 : 50;
		gain_exp (ch, 0 - lose);
		check_improve (ch, iSkill, TRUE, 4);
		sprintf (buf, "You recall from combat!  You lose %d exps.\n\r", lose);
		send_to_char (buf, ch);
		stop_fighting (ch, TRUE);
		
	}
	
	ch->move /= 2;
	act ("$n disappears.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
	char_from_room (ch);
	char_to_room (ch, location);
	
	act ("$n appears in the room.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
	do_look (ch, "auto");
	
	if (ch->pet != NULL)
	{
		char_from_room (ch->pet);
		char_to_room (ch->pet, location);
	}
	
	if (ch->mount != NULL && ch->vstable < 1)
	{
		char_from_room (ch->mount);
		char_to_room (ch->mount, location);
	}
	
	return;
}


/** Function: get_random_room
  * Descr   : Generates a room randomly from vnums 1 to top_room
  * Returns : room data
  * Syntax  : get_random_room( for who )
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
ROOM_INDEX_DATA *get_random_room (CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *room;
	
	for (;;)
	{
		room = get_room_index (number_range (1, top_room));
		if (room != NULL)
			if (can_see_room (ch, room)
				&& !room_is_private (room)
				&& !IS_SET (room->room_flags, ROOM_PRIVATE)
				&& !IS_SET (room->room_flags, ROOM_SOLITARY)
				&& !IS_SET (room->room_flags, ROOM_SAFE)
				&& (IS_NPC (ch) || IS_FLAG (ch->states.player, _ACT_AGGRESSIVE)
				|| !IS_SET (room->room_flags, ROOM_LAW)))
				break;
	}
	
	return room;
}


/** Function: do_enter
  * Descr   : Allows a player to enter a portal or secret room
  * Returns : void
  * Syntax  : enter portal_name
  * Written : v1.0 11/98
  * Author  : Rom Consortium, updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_enter)
{
	ROOM_INDEX_DATA *location;
	
	if (ch->fighting != NULL)
		return;
	
	/* nifty portal stuff */
	if (argument[0] != '\0')
	{
		ROOM_INDEX_DATA *old_room;
		OBJ_DATA *portal;
		CHAR_DATA *fch, *fch_next;
		
		old_room = ch->in_room;
		
		portal = get_obj_list (ch, argument, ch->in_room->contents);
		
		if (portal == NULL)
		{
			send_to_char ("You don't see that here.\n\r", ch);
			return;
		}
		
		
		if ( (portal->item_type != ITEM_PORTAL && portal->item_type != ITEM_WAYGATE) 
		|| (IS_SET (portal->value[1], EX_CLOSED) && !IS_TRUSTED (ch, ANGEL)))
		{
			send_to_char ("You can't seem to find a way in.\n\r", ch);
			return;
		}
		
		if (!IS_TRUSTED (ch, ANGEL) &&
			IS_SET (old_room->room_flags, ROOM_NO_RECALL))
		{
			send_to_char ("Something prevents you from leaving...\n\r", ch);
			return;
		}
		
		if (IS_SET (portal->value[2], GATE_RANDOM) || portal->value[3] == -1)
		{
			location = get_random_room (ch);
			portal->value[3] = location->vnum;	/* for record keeping :) */
		}
		
		else
		{
			if (IS_SET (portal->value[2], GATE_BUGGY) && (number_percent () < 5))
				location = get_random_room (ch);
			else
				location = get_room_index (portal->value[3]);
		}
		
		if (location == NULL
			|| location == ch->in_room
			|| !can_see_room (ch, location)
			|| (room_is_private (location) && !IS_TRUSTED (ch, IMPLEMENTOR)))
		{
			act ("$p doesn't seem to go anywhere.",
				ch, portal, NULL, TO_CHAR, POS_RESTING);
			return;
		}
		
		if (IS_NPC (ch) && IS_FLAG (ch->states.player, _ACT_AGGRESSIVE)
			&& IS_SET (location->room_flags, ROOM_LAW))
		{
			send_to_char ("Something prevents you from leaving...\n\r", ch);
			return;
		}
		
		act ("$n steps into $p.", ch, portal, NULL, TO_ROOM, POS_RESTING);
		
		if (IS_SET (portal->value[2], GATE_NORMAL_EXIT))
			act ("You enter $p.", ch, portal, NULL, TO_CHAR, POS_RESTING);
		else
			act ("You walk through $p and find yourself somewhere else...",
			ch, portal, NULL, TO_CHAR, POS_RESTING);
		
		char_from_room (ch);
		char_to_room (ch, location);
		
		if (IS_SET (portal->value[2], GATE_GOWITH))	/* take the gate along */
		{
			obj_from_room (portal);
			obj_to_room (portal, location);
		}
		
		if (IS_SET (portal->value[2], GATE_NORMAL_EXIT))
			act ("$n has arrived.", ch, portal, NULL, TO_ROOM, POS_RESTING);
		else
			act ("$n has arrived through $p.",
			ch, portal, NULL, TO_ROOM, POS_RESTING);
		
		do_look (ch, "auto");
		
		/* charges */
		if (portal->value[0] > 0)
		{
			portal->value[0]--;
			if (portal->value[0] == 0)
				portal->value[0] = -1;
		}
		
		/* protect against circular follows */
		if (old_room == location)
			return;
		
		for (fch = old_room->people; fch != NULL; fch = fch_next)
		{
			fch_next = fch->next_in_room;
			
			if (portal == NULL || portal->value[0] == -1)
				/* no following through dead portals */
				continue;
			
			if (fch->master == ch && IS_AFFECTED (fch, _AFF_COMPULSION) && fch->position < POS_STANDING)
				do_stand (fch, "");
			
			if (fch->master == ch && fch->position == POS_STANDING)
			{
				
				if (IS_SET (ch->in_room->room_flags, ROOM_LAW)
					&& (IS_NPC (fch)
					&& IS_FLAG (fch->states.player, _ACT_AGGRESSIVE)))
				{
					act ("You can't bring $N into the city.",
						ch, NULL, fch, TO_CHAR, POS_RESTING);
					
					act ("You aren't allowed in the city.",
						fch, NULL, NULL, TO_CHAR, POS_RESTING);
					
					continue;
				}
				
				act ("You follow $N.", fch, NULL, ch, TO_CHAR, POS_RESTING);
				do_enter (fch, argument);
			}
		}
		
		if (portal != NULL && portal->value[0] == -1)
		{
			act ("$p fades out of existence.",
				ch, portal, NULL, TO_CHAR, POS_RESTING);
			
			if (ch->in_room == old_room)
				act ("$p fades out of existence.",
				ch, portal, NULL, TO_ROOM, POS_RESTING);
			else
				if (old_room->people != NULL)
				{
					act ("$p fades out of existence.",
						old_room->people, portal, NULL, TO_CHAR, POS_RESTING);
					
					act ("$p fades out of existence.",
						old_room->people, portal, NULL, TO_ROOM, POS_RESTING);
				}
				
				extract_obj (portal);
		}
		
		/* 
		 * If someone is following the char, these triggers get activated
		 * for the followers before the char, but it's safer this way...
		 */
		if (IS_NPC (ch) && HAS_TRIGGER (ch, TRIG_ENTRY))
			mp_percent_trigger (ch, NULL, NULL, NULL, TRIG_ENTRY);
		
		if (!IS_NPC (ch))
			mp_greet_trigger (ch);
		
		return;
  }
  
  send_to_char ("Nope, can't do it.\n\r", ch);
  return;
}

/** Function: do_stalk
  * Descr   : Player command to follow another, or follow self
  * Returns : void
  * Syntax  : stalk ( stalker, who to stalk )
  * Written : v1.0 11/00
  * Author  : Updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_stalk)
{
	char arg[MIL];
	CHAR_DATA *victim;
	int gsn = skill_lookup("stalk");

	if ( !IS_VALID_SKILL(gsn) )
	{
		bugf("[act_move.c::do_stalk] Stalk skill not found in skill table.");
		return;
	}

	memset(arg, 0, MIL);
	one_argument (argument, arg);

	if ( !get_skill(ch, gsn) )
	{
		chprintf(ch, "Stalking? You've heard that some shady sorts are really good at following people who dont want to be followed.\n\r");
		return;
	}
	
	if (arg[0] == '\0')
	{
		chprintf(ch, "Stalk who?\n\r");
		return;
	}
	
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		chprintf(ch, "They aren't here.\n\r");
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master != NULL)
	{
		chprintf(ch, "You really would rather stay with %s.\n\r",
					 PERS(ch->master, ch));
		return;
	}
	
	if (victim == ch)
	{
		if (ch->master == NULL)
		{
			send_to_char ("You already follow yourself.\n\r", ch);
			return;
		}
		stop_follower (ch);
		return;
	}
	
	if (!IS_NPC (victim)
		&& IS_FLAG (victim->states.config, _NOFOLLOW) && !IS_IMMORTAL (ch))
	{
		bool bIsObservant = IS_AFFECTED(victim, _AFF_DETECT_HIDDEN);
		
		if ( !make_skill_roll( gsn, bIsObservant ? 2.0 : 4.0 ) )
			chprintf(victim, "%s is following closely behind you.", PERS(ch, victim));
		
		return;
	}
	
	REMOVE_FLAG (ch->states.config, _NOFOLLOW);
	
	if (ch->master != NULL)
		stop_follower (ch);
	
	add_follower (ch, victim, FALSE);
	return;
}

