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
 * @@ Objective (act_obj.c)
 *
 * This file contains the player commands for object (item) manipulation.
 * Such as: do_get, do_drop, etc.  Lower-level object commands such as
 * obj_from_char can be found in handler.c
 */


#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"



/*
 * Bowfire code -- used to draw an arrow from a quiver  
 * Bowfire Code v1.0 (c)1997-99 Feudal Realms 
 */
_DOFUN (do_draw)
{
	OBJ_DATA *quiver;
	OBJ_DATA *arrow;
	
	if ((quiver = get_eq_char (ch, WEAR_SHOULDER)) == NULL)
	{
		send_to_char ("You aren't wearing a quiver where you can get to it.\n\r",
			ch);
		return;
	}
	
	if (quiver->item_type != ITEM_QUIVER)
	{
		send_to_char ("You can only draw arrows from a quiver.\n\r", ch);
		return;
	}
	
	if (get_eq_char (ch, WEAR_HOLD) != NULL)
	{
		send_to_char ("You need a free hand to draw an arrow.\n\r", ch);
		return;
	}
	
	if (quiver->value[0] > 0)
	{
		WAIT_STATE (ch, PULSE_VIOLENCE);
		act ("$n draws an arrow from $p.",
			ch, quiver, NULL, TO_ROOM, POS_RESTING);
		act ("You draw an arrow from $p.",
			ch, quiver, NULL, TO_CHAR, POS_RESTING);
		
		arrow = create_object (get_obj_index (OBJ_VNUM_ARROW), 0);
		arrow->value[1] = quiver->value[1];
		arrow->value[2] = quiver->value[2];
		arrow->level = quiver->level;
		obj_to_char (arrow, ch);
		equip_char (ch, arrow, WEAR_HOLD);
		quiver->value[0]--;
		
		if (quiver->value[0] < 1)
		{
			act ("Your $p is now out of arrows, you need to find another one.",
				ch, quiver, NULL, TO_CHAR, POS_RESTING);
			extract_obj (quiver);
		}
		
		return;
	}
}


/* Bowfire code -- Used to dislodge an arrow already lodged */
_DOFUN (do_dislodge)
{
	OBJ_DATA *arrow = NULL;
	int dam = 0;
	int iSkill = skill_lookup("bow");
	
	if (argument[0] == '\0')	/* empty */
	{
		send_to_char ("Dislodge what?\n\r", ch);
		return;
	}
	
	if (get_eq_char (ch, WEAR_LODGED_R) != NULL)
	{
		arrow = get_eq_char (ch, WEAR_LODGED_R);
		act ("With a wrenching pull, you dislodge $p from your chest.",
			ch, arrow, NULL, TO_CHAR, POS_RESTING);
		
		unequip_char (ch, arrow);
		REMOVE_BIT (arrow->extra_flags, ITEM_LODGED);
		dam = dice ((3 * arrow->value[1]), (3 * arrow->value[2]));
		damage (ch, ch, dam, iSkill, DAM_SLASH, TRUE, TRUE);
		return;
	}
	else
		if (get_eq_char (ch, WEAR_LODGED_A) != NULL)
		{
			arrow = get_eq_char (ch, WEAR_LODGED_A);
			act ("With a tug you dislodge $p from your arm.",
				ch, arrow, NULL, TO_CHAR, POS_RESTING);
			unequip_char (ch, arrow);
			REMOVE_BIT (arrow->extra_flags, ITEM_LODGED);
			
			dam = dice ((3 * arrow->value[1]), (2 * arrow->value[2]));
			damage (ch, ch, dam, iSkill, DAM_SLASH, TRUE, TRUE);
			return;
		}
		else
			if (get_eq_char (ch, WEAR_LODGED_L) != NULL)
			{
				arrow = get_eq_char (ch, WEAR_LODGED_L);
				act ("With a tug you dislodge $p from your leg.",
					ch, arrow, NULL, TO_CHAR, POS_RESTING);
				unequip_char (ch, arrow);
				REMOVE_BIT (arrow->extra_flags, ITEM_LODGED);
				
				dam = dice ((2 * arrow->value[1]), (2 * arrow->value[2]));
				damage (ch, ch, dam, iSkill, DAM_SLASH, TRUE, TRUE);
				return;
			}
			else
				
				send_to_char ("You have nothing lodged in your body.\n\r", ch);
}



/* RT part of the corpse looting code */
bool can_loot (CHAR_DATA * ch, OBJ_DATA * obj)
{
	CHAR_DATA *owner, *wch;
	
	if (IS_IMMORTAL (ch))
		return TRUE;
	
	owner = NULL;
	for (wch = char_list; wch != NULL; wch = wch->next)
	{
    /* fix bug that allowed scavengers to take objects that someone
		* was currently sitting/standing/sleeping on. -GM*/
		if (wch->on == obj)
			return FALSE;
		
		if (obj->owner && !str_cmp (wch->name, obj->owner))
			owner = wch;
	}
	
	if (owner == NULL)
		return TRUE;
	
	if (!obj->owner || obj->owner == NULL)
		return TRUE;
	
	if (!str_cmp (ch->name, owner->name))
		return TRUE;
	
	if (!IS_NPC (owner) && IS_FLAG (owner->states.player, _CANLOOT))
		return TRUE;
	
	if (is_same_group (ch, owner))
		return TRUE;
	
	return FALSE;
}


/** Function: do_loot
  * Descr   : Command to let one player loot another while the victim is
  *         : still alive. (mortall wounded). Ensures that the victim may
  *         : not then be killed after being looted.
  * Returns : void
  * Syntax  : do_loot(looter, who [what])
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_loot)
{
	CHAR_DATA *vch;
	char arg[MIL];
	char who[MIL];
	bool found = FALSE;
	
	if (argument[0] == '\0')
	{
		send_to_char ("Loot what from who?\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, arg);
	argument = one_argument (argument, who);
	
	if (IS_FLAG (ch->states.penalties, _NOLOOT))
	{
		send_to_char ("Your looting priveledges have been revoked.\n\r", ch);
		return;
	}
	
	if (who[0] == '\0')
	{
		memcpy (who, arg, MIL);
		arg[0] = '\0';
	}
	
	if (arg[0] == '\0')
		strcpy (arg, "all");
	
	if ((vch = get_char_room (ch, who)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	/* cant use can_loot */
	
	if (IS_NPC (vch))
	{
		send_to_char ("You can't loot this one.\n\r", ch);
		return;
	}
	
	if (vch->wtimer == 0)
	{
		send_to_char ("I think they might object to that!\n\r", ch);
		return;
	}
	
	else
	{
		OBJ_DATA *obj;
		OBJ_DATA *obj_next;
		sh_int counter = 0;
		
		if (arg[0] == '\0' || !str_cmp (arg, "all") || !str_prefix (arg, "all."))
		{				/* get all, or get all.item(s) */
			
			for (obj = vch->carrying; obj; obj = obj_next)
			{
				
				obj_next = obj->next_content;
				
				if ((arg[3] == '\0' || is_name (&arg[4], obj->name))
					&& can_see_obj (ch, obj))
				{
					if (get_carry_weight (ch) + get_obj_weight (obj) <=
						can_carry_w (ch))
					{
						if ( IS_FLAG(pMud.config, _COUNT_LOOT) )
						{
							if (++counter > vch->level % 10)
							{
								chprintf (ch, "You've looted all you can from "
									"%s's corpse.", PERS (vch, ch));
								break;
							}
						}						
						obj_from_char (obj);
						obj_to_char (obj, ch);
						act ("$n loots $n from $N", ch, obj, vch, TO_NOTVICT,
							POS_RESTING);
						found = TRUE;
						chprintf (ch, "You pocket %s.\n\r", obj->name);
					}
				}
			}
			
			if (!found)
				send_to_char ("You cant find anything to loot.", ch);
		}
		else
			/* get a single object */
		{
			if (((obj = get_obj_wear (vch, arg)) != NULL ||
				(obj = get_obj_carry (vch, arg, ch)) != NULL) &&
				can_see_obj (ch, obj))
			{
				obj_from_char (obj);
				obj_to_char (obj, ch);
				chprintf (ch, "You pocket %s.\n\r", obj->name);
				act ("$n loots $n from $N", ch, obj, vch, TO_NOTVICT, POS_RESTING);
			}
			else
				
				send_to_char ("You see no such thing.\n\r", ch);
		}
		
	} /* end of get obj code */
	
	if (found)	/* looted: dont let them now kill the player */
		return_ch_to_play (vch);
	
	return;
}

void get_obj (CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container)
{
	CHAR_DATA *gch;
	int members;
	char buffer[100];
	bool bGhost = FALSE;
	
	if (!CAN_WEAR (obj, ITEM_TAKE))
	{
		send_to_char ("You can't take that.\n\r", ch);
		return;
	}
	
	if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
	{
		act ("$d: you can't carry that many items.",
			ch, NULL, obj->name, TO_CHAR, POS_RESTING);
		return;
	}

	if ( IS_AFFECTED(ch, _AFF_DREAMING) &&( obj->in_room || obj->in_obj ) )
		bGhost = TRUE;
	
	if ((!obj->in_obj || obj->in_obj->carried_by != ch)
		&& (get_carry_weight (ch) + get_obj_weight (obj) > can_carry_w (ch)))
	{
		act ("$d: you can't carry that much weight.",
			ch, NULL, obj->name, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (!can_loot (ch, obj))
	{
		act ("Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (obj->in_room != NULL)
	{
		for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
			if (gch->on == obj)
			{
				act ("$N appears to be using $p.",
					ch, obj, gch, TO_CHAR, POS_RESTING);
				return;
			}
	}
	
	
	if (container != NULL)
	{
		if (container->pIndexData->vnum == OBJ_VNUM_PIT
			&& get_trust (ch) < obj->level)
		{
			send_to_char ("You are not powerful enough to use it.\n\r", ch);
			return;
		}
		
		if (container->pIndexData->vnum == OBJ_VNUM_PIT
			&& !CAN_WEAR (container, ITEM_TAKE)
			&& !IS_OBJ_STAT (obj, ITEM_HAD_TIMER))
			obj->timer = 0;

		act ("You get $p from $P.", ch, obj, container, TO_CHAR, POS_RESTING);
		act ("$n gets $p from $P.", ch, obj, container, TO_ROOM, POS_RESTING);
		REMOVE_BIT (obj->extra_flags, ITEM_HAD_TIMER);
		obj_from_obj (obj);
	}
	else
	{
		act ("You get $p.", ch, obj, container, TO_CHAR, POS_RESTING);
		act ("$n gets $p.", ch, obj, container, TO_ROOM, POS_RESTING);
		obj_from_room (obj);
	}
	
	if (obj->item_type == ITEM_MONEY)
	{
		ch->silver += obj->value[0];
		ch->gold += obj->value[1];
		if (IS_FLAG (ch->states.config, _AUTOSPLIT))
		{
			members = 0;
			for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
			{
				if (!IS_AFFECTED (gch, _AFF_COMPULSION) && is_same_group (gch, ch))
					members++;
			}
			
			if (members > 1 && (obj->value[0] > 1 || obj->value[1]))
			{
				sprintf (buffer, "%d %d", obj->value[0], obj->value[1]);
				do_split (ch, buffer);
			}
		}
		
		extract_obj (obj);
	}
	else
	{
		if ( bGhost )
			SET_BIT(obj->extra_flags, ITEM_GHOST);

		obj_to_char (obj, ch);
		snd_trigger (ch, obj->sounds ? obj->sounds : obj->pIndexData->sounds,
			SOUND_OBJECT, SOUND_OBJ_GET, TRUE);
	}
	
	return;
}



_DOFUN (do_get)
{
	char arg1[MIL];
	char arg2[MIL];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	OBJ_DATA *container;
	bool found;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (!str_cmp (arg2, "from"))
		argument = one_argument (argument, arg2);
	
	/* Get type. */
	if (arg1[0] == '\0')
	{
		send_to_char ("Get what?\n\r", ch);
		return;
	}
	
	if (arg2[0] == '\0')
	{
		if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
		{
			/* 'get obj' */
			obj = get_obj_list (ch, arg1, ch->in_room->contents);
			if (obj == NULL)
			{
				act ("I see no $T here.", ch, NULL, arg1, TO_CHAR, POS_RESTING);
				return;
			}
			
			get_obj (ch, obj, NULL);
		}
		else
		{
			/* 'get all' or 'get all.obj' */
			found = FALSE;
			for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
			{
				obj_next = obj->next_content;
				if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
					&& can_see_obj (ch, obj))
				{
					found = TRUE;
					get_obj (ch, obj, NULL);
				}
			}
			
			if (!found)
			{
				if (arg1[3] == '\0')
					send_to_char ("I see nothing here.\n\r", ch);
				else
					act ("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR, POS_RESTING);
			}
		}
	}
	else
	{
		/* 'get ... container' */
		if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}
		
		if ((container = get_obj_here (ch, arg2)) == NULL)
		{
			act ("I see no $T here.", ch, NULL, arg2, TO_CHAR, POS_RESTING);
			return;
		}
		
		switch (container->item_type)
		{
			default:
				send_to_char ("That's not a container.\n\r", ch);
				return;
				
			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
				break;
				
			case ITEM_CORPSE_PC:
				{
					
					if (!can_loot (ch, container))
					{
						send_to_char ("You can't do that.\n\r", ch);
						return;
					}
				}
		}
		
		if (IS_SET (container->value[1], CONT_CLOSED))
		{
			act ("The $d is closed.", ch, NULL, container->name, TO_CHAR,
				POS_RESTING);
			return;
		}
		
		if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
		{
			/* 'get obj container' */
			obj = get_obj_list (ch, arg1, container->contains);
			if (obj == NULL)
			{
				act ("I see nothing like that in the $T.",
					ch, NULL, arg2, TO_CHAR, POS_RESTING);
				return;
			}
			get_obj (ch, obj, container);
		}
		else
		{
			/* 'get all container' or 'get all.obj container' */
			found = FALSE;
			for (obj = container->contains; obj != NULL; obj = obj_next)
			{
				obj_next = obj->next_content;
				if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name)))
				{
					found = TRUE;
					if (container->pIndexData->vnum == OBJ_VNUM_PIT
						&& !IS_IMMORTAL (ch))
					{
						send_to_char ("Don't be so greedy!\n\r", ch);
						return;
					}
					get_obj (ch, obj, container);
				}
			}
			
			if (!found)
			{
				if (arg1[3] == '\0')
					act ("I see nothing in the $T.",
					ch, NULL, arg2, TO_CHAR, POS_RESTING);
				else
					act ("I see nothing like that in the $T.",
					ch, NULL, arg2, TO_CHAR, POS_RESTING);
			}
		}
	}
	
	return;
}



_DOFUN (do_put)
{
	char arg1[MIL];
	char arg2[MIL];
	OBJ_DATA *container;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (!str_cmp (arg2, "in") || !str_cmp (arg2, "on"))
		argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Put what in what?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
	{
		send_to_char ("You can't do that.\n\r", ch);
		return;
	}
	
	if ((container = get_obj_here (ch, arg2)) == NULL)
	{
		act ("I see no $T here.", ch, NULL, arg2, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (container->item_type != ITEM_CONTAINER)
	{
		send_to_char ("That's not a container.\n\r", ch);
		return;
	}
	
	if (IS_SET (container->value[1], CONT_CLOSED))
	{
		act ("The $d is closed.", ch, NULL, container->name, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
	{
		/* 'put obj container' */
		if ((obj = get_obj_carry (ch, arg1, ch)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}
		
		if (obj == container)
		{
			send_to_char ("You can't fold it into itself.\n\r", ch);
			return;
		}
		
		if (!can_drop_obj (ch, obj))
		{
			send_to_char ("You can't let go of it.\n\r", ch);
			return;
		}
		
		if (WEIGHT_MULT (obj) != 100)
		{
			send_to_char ("You have a feeling that would be a bad idea.\n\r", ch);
			return;
		}
		
		if (get_obj_weight (obj) + get_true_weight (container)
			> (container->value[0] * 10)
			|| get_obj_weight (obj) > (container->value[3] * 10))
		{
			send_to_char ("It won't fit.\n\r", ch);
			return;
		}
		
		if (container->pIndexData->vnum == OBJ_VNUM_PIT
			&& !CAN_WEAR (container, ITEM_TAKE))
		{
			if (obj->timer)
				SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
			else
				obj->timer = number_range (100, 200);
		}
		obj_from_char (obj);
		obj_to_obj (obj, container);
		
		if (IS_SET (container->value[1], CONT_PUT_ON))
		{
			act ("$n puts $p on $P.", ch, obj, container, TO_ROOM, POS_RESTING);
			act ("You put $p on $P.", ch, obj, container, TO_CHAR, POS_RESTING);
		}
		else
		{
			act ("$n puts $p in $P.", ch, obj, container, TO_ROOM, POS_RESTING);
			act ("You put $p in $P.", ch, obj, container, TO_CHAR, POS_RESTING);
		}
	}
	else
	{
		/* 'put all container' or 'put all.obj container' */
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			
			if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name))
				&& can_see_obj (ch, obj)
				&& WEIGHT_MULT (obj) == 100
				&& obj->wear_loc == WEAR_NONE
				&& obj != container
				&& can_drop_obj (ch, obj)
				&& get_obj_weight (obj) + get_true_weight (container)
				<= (container->value[0] * 10)
				&& get_obj_weight (obj) < (container->value[3] * 10))
			{
				if (container->pIndexData->vnum == OBJ_VNUM_PIT
					&& !CAN_WEAR (obj, ITEM_TAKE))
				{
					if (obj->timer)
						SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
					else
						obj->timer = number_range (100, 200);
				}
				obj_from_char (obj);
				obj_to_obj (obj, container);
				
				if (IS_SET (container->value[1], CONT_PUT_ON))
				{
					act ("$n puts $p on $P.", ch, obj, container, TO_ROOM, POS_RESTING);
					act ("You put $p on $P.", ch, obj, container, TO_CHAR, POS_RESTING);
				}
				else
				{
					act ("$n puts $p in $P.", ch, obj, container, TO_ROOM, POS_RESTING);
					act ("You put $p in $P.", ch, obj, container, TO_CHAR, POS_RESTING);
				}
			}
		}
	}
	
	return;
}



_DOFUN (do_drop)
{
	char arg[MIL];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	bool found;
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Drop what?\n\r", ch);
		return;
	}
	
	if (is_number (arg))
	{
		/* 'drop NNNN coins' */
		int amount, gold = 0, silver = 0;
		
		amount = atoi (arg);
		argument = one_argument (argument, arg);
		if (amount <= 0
			|| (str_cmp (arg, "coins") && str_cmp (arg, "coin") &&
			str_cmp (arg, "gold") && str_cmp (arg, "silver")))
		{
			send_to_char ("Sorry, you can't do that.\n\r", ch);
			return;
		}
		
		if (!str_cmp (arg, "coins") || !str_cmp (arg, "coin")
			|| !str_cmp (arg, "silver"))
		{
			if (ch->silver < amount)
			{
				send_to_char ("You don't have that much silver.\n\r", ch);
				return;
			}
			
			ch->silver -= amount;
			silver = amount;
		}
		
		else
		{
			if (ch->gold < amount)
			{
				send_to_char ("You don't have that much gold.\n\r", ch);
				return;
			}
			
			ch->gold -= amount;
			gold = amount;
		}
		
		for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			
			switch (obj->pIndexData->vnum)
			{
				case OBJ_VNUM_SILVER_ONE:
					silver += 1;
					extract_obj (obj);
					break;
					
				case OBJ_VNUM_GOLD_ONE:
					gold += 1;
					extract_obj (obj);
					break;
					
				case OBJ_VNUM_SILVER_SOME:
					silver += obj->value[0];
					extract_obj (obj);
					break;
					
				case OBJ_VNUM_GOLD_SOME:
					gold += obj->value[1];
					extract_obj (obj);
					break;
					
				case OBJ_VNUM_COINS:
					silver += obj->value[0];
					gold += obj->value[1];
					extract_obj (obj);
					break;
			}
		}
		
		obj_to_room (create_money (gold, silver), ch->in_room);
		act ("$n drops some coins.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		send_to_char ("OK.\n\r", ch);
		return;
	}
	
	if (str_cmp (arg, "all") && str_prefix ("all.", arg))
	{
		/* 'drop obj' */
		if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}
		
		if (!can_drop_obj (ch, obj))
		{
			send_to_char ("You can't let go of it.\n\r", ch);
			return;
		}
		
		obj_from_char (obj);
		obj_to_room (obj, ch->in_room);
		act ("$n drops $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You drop $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		if (IS_OBJ_STAT (obj, ITEM_MELT_DROP) || IS_OBJ_STAT(obj, ITEM_GHOST) )
		{
			act ("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			act ("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			extract_obj (obj);
		}
		snd_trigger (ch, obj->sounds ? obj->sounds : obj->pIndexData->sounds,
			SOUND_OBJECT, SOUND_OBJ_DROP, TRUE);
		
	}
	else
	{
		/* 'drop all' or 'drop all.obj' */
		found = FALSE;
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			
			if ((arg[3] == '\0' || is_name (&arg[4], obj->name))
				&& can_see_obj (ch, obj)
				&& obj->wear_loc == WEAR_NONE && can_drop_obj (ch, obj))
			{
				found = TRUE;
				obj_from_char (obj);
				obj_to_room (obj, ch->in_room);
				act ("$n drops $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
				act ("You drop $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
				if (IS_OBJ_STAT (obj, ITEM_MELT_DROP) || IS_OBJ_STAT(obj, ITEM_GHOST) )
				{
					act ("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM,
						POS_RESTING);
					act ("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR,
						POS_RESTING);
					extract_obj (obj);
				}
				snd_trigger (ch,
					obj->sounds ? obj->sounds : obj->pIndexData->sounds,
					SOUND_OBJECT, SOUND_OBJ_DROP, TRUE);
			}
		}
		
		if (!found)
		{
			if (arg[3] == '\0')
				act ("You are not carrying anything.",
				ch, NULL, arg, TO_CHAR, POS_RESTING);
			else
				act ("You are not carrying any $T.",
				ch, NULL, &arg[4], TO_CHAR, POS_RESTING);
		}
	}
	
	return;
}



_DOFUN (do_give)
{
	char arg1[MIL];
	char arg2[MIL];
	char buf[MSL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Give what to whom?\n\r", ch);
		return;
	}
	
	if (is_number (arg1))
	{
		/* 'give NNNN coins victim' */
		int amount;
		bool silver;
		
		amount = atoi (arg1);
		if (amount <= 0
			|| (str_cmp (arg2, "coins") && str_cmp (arg2, "coin") &&
			str_cmp (arg2, "gold") && str_cmp (arg2, "silver")))
		{
			send_to_char ("Sorry, you can't do that.\n\r", ch);
			return;
		}
		
		silver = str_cmp (arg2, "gold");
		
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char ("Give what to whom?\n\r", ch);
			return;
		}
		
		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}
		
		if ((!silver && ch->gold < amount) || (silver && ch->silver < amount))
		{
			send_to_char ("You haven't got that much.\n\r", ch);
			return;
		}
		
		if (silver)
		{
			ch->silver -= amount;
			victim->silver += amount;
		}
		else
		{
			ch->gold -= amount;
			victim->gold += amount;
		}
		
		sprintf (buf, "$n gives you %d %s.", amount, silver ? "silver" : "gold");
		act (buf, ch, NULL, victim, TO_VICT, POS_RESTING);
		act ("$n gives $N some coins.", ch, NULL, victim, TO_NOTVICT,
			POS_RESTING);
		sprintf (buf, "You give $N %d %s.", amount, silver ? "silver" : "gold");
		act (buf, ch, NULL, victim, TO_CHAR, POS_RESTING);
		
		/*
 	 	 * Bribe trigger
		 */
		if (IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_BRIBE))
			mp_bribe_trigger (victim, ch, silver ? amount : amount * 100);
		
		if (IS_NPC (victim) && IS_FLAG (victim->states.player, _ACT_BANKER))
		{
			int change;
			
			change = (silver ? 95 * amount / 100 / 100 : 95 * amount);
			
			
			if (!silver && change > victim->silver)
				victim->silver += change;
			
			if (silver && change > victim->gold)
				victim->gold += change;
			
			if (change < 1 && can_see (victim, ch))
			{
				act
					("$n tells you 'I'm sorry, you did not give me enough to change.'",
					victim, NULL, ch, TO_VICT, POS_RESTING);
				ch->reply = victim;
				sprintf (buf, "%d %s %s",
					amount, silver ? "silver" : "gold", ch->name);
				do_give (victim, buf);
			}
			else if (can_see (victim, ch))
			{
				sprintf (buf, "%d %s %s",
					change, silver ? "gold" : "silver", ch->name);
				do_give (victim, buf);
				if (silver)
				{
					sprintf (buf, "%d silver %s",
						(95 * amount / 100 - change * 100), ch->name);
					do_give (victim, buf);
				}
				act ("$n tells you 'Thank you, come again.'",
					victim, NULL, ch, TO_VICT, POS_RESTING);
				ch->reply = victim;
			}
		}
		return;
	}
	
	if ((obj = get_obj_carry (ch, arg1, ch)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	if (obj->wear_loc != WEAR_NONE)
	{
		send_to_char ("You must remove it first.\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg2)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (IS_NPC (victim) && victim->pIndexData->pShop != NULL)
	{
		act ("$N tells you 'Sorry, you'll have to sell that.'",
			ch, NULL, victim, TO_CHAR, POS_RESTING);
		ch->reply = victim;
		return;
	}
	
	if (!can_drop_obj (ch, obj))
	{
		send_to_char ("You can't let go of it.\n\r", ch);
		return;
	}
	
	if (victim->carry_number + get_obj_number (obj) > can_carry_n (victim))
	{
		act ("$N has $S hands full.", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (get_carry_weight (victim) + get_obj_weight (obj) > can_carry_w (victim))
	{
		act ("$N can't carry that much weight.", ch, NULL, victim, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (!can_see_obj (victim, obj))
	{
		act ("$N can't see it.", ch, NULL, victim, TO_CHAR, POS_RESTING);
		return;
	}
	
	obj_from_char (obj);
	obj_to_char (obj, victim);
	MOBtrigger = FALSE;
	act ("$n gives $p to $N.", ch, obj, victim, TO_NOTVICT, POS_RESTING);
	act ("$n gives you $p.", ch, obj, victim, TO_VICT, POS_RESTING);
	act ("You give $p to $N.", ch, obj, victim, TO_CHAR, POS_RESTING);
	MOBtrigger = TRUE;
	
	/*
	 * Give trigger
	 */
	if (IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_GIVE))
		mp_give_trigger (victim, ch, obj);
	
	snd_trigger (ch, obj->sounds ? obj->sounds : obj->pIndexData->sounds,
	       SOUND_OBJECT, SOUND_OBJ_GIVE, TRUE);
	
	return;
}

_DOFUN (do_butcher)
{
	char buf[MSL];
	char arg[MSL];
	int numst = 0;
	int skill;
	OBJ_DATA *steak;
	OBJ_DATA *obj;
	int iSkill = skill_lookup("butcher");
	
	one_argument (argument, arg);
	
	if ((skill = get_skill (ch, iSkill)) < 1)
	{
		send_to_char ("Butchering is beyond your skills.\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		send_to_char ("Butcher what?\n\r", ch);
		return;
	}
	
	obj = get_obj_list (ch, arg, ch->in_room->contents);
	
	if (obj == NULL)
	{
		send_to_char ("It's not here.\n\r", ch);
		return;
	}
	
	if ((obj->item_type != ITEM_CORPSE_NPC)
		&& (obj->item_type != ITEM_CORPSE_PC))
	{
		send_to_char ("You can only butcher corpses.\n\r", ch);
		return;
	}
	
	/*
	 * create and rename the steak 
	 */
	buf[0] = '\0';
	
	strcat (buf, "A steak of ");
	strcat (buf, str_dup (obj->short_descr));
	strcat (buf, " is here.");
	
	if ((steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0)) == NULL)
	{
		send_to_char ("BUG: 'steak' object not found. Logged.\n\r", ch);
		return;
	}
	steak->description = str_dup (buf);
	steak->value[0] = ch->level / 2;
	steak->value[1] = ch->level;
	
	buf[0] = '\0';
	strcat (buf, "A steak of ");
	strcat (buf, str_dup (obj->short_descr));
	steak->short_descr = str_dup (buf);
	
	/*
 	 * Check the skill roll, and put a random ammount of steaks here. 
	 */
	if (number_percent () < skill)	/* success! */
	{
		numst = dice (1, 4);
		switch (numst)
		{
			case 1:
				
				if ((steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0)) == NULL)
				{
					send_to_char ("BUG: 'steak' object not found. Logged.\n\r", ch);
					return;
				}
				obj_to_room (steak, ch->in_room);
				act ("$n butchers a corpse and creates a steak.\n\r",
					ch, steak, NULL, TO_ROOM, POS_RESTING);
				act ("You butcher a corpse and create a steak.\n\r",
					ch, steak, NULL, TO_CHAR, POS_RESTING);
				break;
				
			case 2:
				
				if ((steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0)) == NULL)
				{
					send_to_char ("BUG: 'steak' object not found. Logged.\n\r", ch);
					return;
				}
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				act ("$n butchers a corpse and creates two steaks.\n\r",
					ch, steak, NULL, TO_ROOM, POS_RESTING);
				act ("You butcher a corpse and create two steaks.\n\r",
					ch, steak, NULL, TO_CHAR, POS_RESTING);
				break;
				
			case 3:
				
				if ((steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0)) == NULL)
				{
					send_to_char ("BUG: 'steak' object not found. Logged.\n\r", ch);
					return;
				}
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				act ("$n butchers a corpse and creates three steaks.\n\r",
					ch, steak, NULL, TO_ROOM, POS_RESTING);
				act ("You butcher a corpse and create three steaks.\n\r",
					ch, steak, NULL, TO_CHAR, POS_RESTING);
				break;
				
			case 4:
				
				if ((steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0)) == NULL)
				{
					send_to_char ("BUG: 'steak' object not found. Logged.\n\r", ch);
					return;
				}
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				steak = create_object (get_obj_index (OBJ_VNUM_STEAK), 0);
				obj_to_room (steak, ch->in_room);
				act ("$n butchers a corpse and creates four steaks.\n\r",
					ch, steak, NULL, TO_ROOM, POS_RESTING);
				act ("You butcher a corpse and create four steaks.\n\r",
					ch, steak, NULL, TO_CHAR, POS_RESTING);
				break;
			
		}
		
		check_improve (ch, iSkill, TRUE, 1);
		
	}
	else
	{
		act ("$n fails to butcher a corpse, and destroys it.\n\r",
			ch, steak, NULL, TO_ROOM, POS_RESTING);
		act ("You fail to butcher a corpse, and destroy it.\n\r",
			ch, steak, NULL, TO_CHAR, POS_RESTING);
		check_improve (ch, iSkill, FALSE, 1);
	}
	
	/*
 	 * dump items caried 
	 * Taken from the original ROM code and added into here. 
	 */
	if (obj->item_type == ITEM_CORPSE_PC)
	{ /* save the contents */
		{
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
	}
	
	if (obj->item_type == ITEM_CORPSE_NPC)
	{
		{
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
						else if (obj->in_room == NULL)
							/* destroy it */
							extract_obj (t_obj);
						else			/* to a room */
							obj_to_room (t_obj, obj->in_room);
			}
		}
	}
	
	/* 
	 * Now remove the corpse 
	 */
	extract_obj (obj);
	return;
}


/* for poisoning weapons and food/drink */
_DOFUN (do_envenom)
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int iSkill = skill_lookup("envenom");
	int percent, skill;
	
	/* find out what */
	if (argument == '\0')
	{
		send_to_char ("Envenom what item?\n\r", ch);
		return;
	}
	
	obj = get_obj_list (ch, argument, ch->carrying);
	
	if (obj == NULL)
	{
		send_to_char ("You don't have that item.\n\r", ch);
		return;
	}
	
	if ((skill = get_skill (ch, iSkill)) < 1)
	{
		send_to_char ("Are you crazy? You'd poison yourself!\n\r", ch);
		return;
	}
	
	if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
	{
		if (IS_OBJ_STAT (obj, ITEM_BLESS))
		{
			act ("You fail to poison $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			return;
		}
		
		if (number_percent () < skill)	/* success! */
		{
			act ("$n treats $p with deadly poison.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You treat $p with deadly poison.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			
			if (!obj->value[3])
			{
				obj->value[3] = 1;
				check_improve (ch, iSkill, TRUE, 4);
			}
			
			WAIT_STATE (ch, skill_table[iSkill].beats);
			return;
		}
		
		act ("You fail to poison $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		
		if (!obj->value[3])
			check_improve (ch, iSkill, FALSE, 4);
		
		WAIT_STATE (ch, skill_table[iSkill].beats);
		return;
	}
	
	if (obj->item_type == ITEM_WEAPON)
	{
		if (IS_OBJ_STAT (obj, ITEM_BLESS))
		{
			act ("You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			return;
		}
		
		if (obj->value[3] < 0 || attack_table[obj->value[3]].damage == DAM_BASH)
		{
			send_to_char ("You can only envenom edged weapons.\n\r", ch);
			return;
		}
		
		if (IS_WEAPON_STAT (obj, WEAPON_POISON))
		{
			act ("$p is already envenomed.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			return;
		}
		
		percent = number_percent ();
		
		if (percent < skill)
		{
			
			af.where = TO_WEAPON;
			af.type = iSkill;
			af.level = ch->level * percent / 100;
			af.duration = ch->level / 2 * percent / 100;
			af.location = 0;
			af.modifier = 0;
			af.bitvector = WEAPON_POISON;
			af.owner = GET_ID(ch);
			
			affect_to_obj (obj, &af);
			
			act ("$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You coat $p with venom.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			
			check_improve (ch, iSkill, TRUE, 3);
			WAIT_STATE (ch, skill_table[iSkill].beats);
			
			return;
		}
		else
		{
			act ("You fail to envenom $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
			check_improve (ch, iSkill, FALSE, 3);
			
			WAIT_STATE (ch, skill_table[iSkill].beats);
			return;
		}
	}
	
	act ("You can't poison $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	return;
}

_DOFUN (do_fill)
{
	char arg[MIL];
	char buf[MSL];
	OBJ_DATA *obj;
	OBJ_DATA *fountain;
	bool found;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Fill what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	found = FALSE;
	for (fountain = ch->in_room->contents; fountain != NULL;
	fountain = fountain->next_content)
	{
		if (fountain->item_type == ITEM_FOUNTAIN)
		{
			found = TRUE;
			break;
		}
	}
	
	if (!found)
	{
		send_to_char ("There is no fountain here!\n\r", ch);
		return;
	}
	
	if (obj->item_type != ITEM_DRINK_CON)
	{
		send_to_char ("You can't fill that.\n\r", ch);
		return;
	}
	
	if (obj->value[1] != 0 && obj->value[2] != fountain->value[2])
	{
		send_to_char ("There is already another liquid in it.\n\r", ch);
		return;
	}
	
	if (obj->value[1] >= obj->value[0])
	{
		send_to_char ("Your container is full.\n\r", ch);
		return;
	}
	
	sprintf (buf, "You fill $p with %s from $P.",
		liq_table[fountain->value[2]].liq_name);
	act (buf, ch, obj, fountain, TO_CHAR, POS_RESTING);
	sprintf (buf, "$n fills $p with %s from $P.",
		liq_table[fountain->value[2]].liq_name);
	act (buf, ch, obj, fountain, TO_ROOM, POS_RESTING);
	obj->value[2] = fountain->value[2];
	obj->value[1] = obj->value[0];
	return;
}

_DOFUN (do_pour)
{
	char arg[MSL], buf[MSL];
	OBJ_DATA *out, *in;
	CHAR_DATA *vch = NULL;
	int amount;
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Pour what into what?\n\r", ch);
		return;
	}
	
	
	if ((out = get_obj_carry (ch, arg, ch)) == NULL)
	{
		send_to_char ("You don't have that item.\n\r", ch);
		return;
	}
	
	if (out->item_type != ITEM_DRINK_CON)
	{
		send_to_char ("That's not a drink container.\n\r", ch);
		return;
	}
	
	if (!str_cmp (argument, "out"))
	{
		if (out->value[1] == 0)
		{
			send_to_char ("It's already empty.\n\r", ch);
			return;
		}
		
		out->value[1] = 0;
		out->value[3] = 0;
		sprintf (buf, "You invert $p, spilling %s all over the ground.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, out, NULL, TO_CHAR, POS_RESTING);
		
		sprintf (buf, "$n inverts $p, spilling %s all over the ground.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, out, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if ((in = get_obj_here (ch, argument)) == NULL)
	{
		vch = get_char_room (ch, argument);
		
		if (vch == NULL)
		{
			send_to_char ("Pour into what?\n\r", ch);
			return;
		}
		
		in = get_eq_char (vch, WEAR_HOLD);
		
		if (in == NULL)
		{
			send_to_char ("They aren't holding anything.", ch);
			return;
		}
	}
	
	if (in->item_type != ITEM_DRINK_CON)
	{
		send_to_char ("You can only pour into other drink containers.\n\r", ch);
		return;
	}
	
	if (in == out)
	{
		send_to_char ("You cannot change the laws of physics!\n\r", ch);
		return;
	}
	
	if (in->value[1] != 0 && in->value[2] != out->value[2])
	{
		send_to_char ("They don't hold the same liquid.\n\r", ch);
		return;
	}
	
	if (out->value[1] == 0)
	{
		act ("There's nothing in $p to pour.", ch, out, NULL, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (in->value[1] >= in->value[0])
	{
		act ("$p is already filled to the top.", ch, in, NULL, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	amount = UMIN (out->value[1], in->value[0] - in->value[1]);
	
	in->value[1] += amount;
	out->value[1] -= amount;
	in->value[2] = out->value[2];
	
	if (vch == NULL)
	{
		sprintf (buf, "You pour %s from $p into $P.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, out, in, TO_CHAR, POS_RESTING);
		sprintf (buf, "$n pours %s from $p into $P.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, out, in, TO_ROOM, POS_RESTING);
	}
	else
	{
		sprintf (buf, "You pour some %s for $N.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, NULL, vch, TO_CHAR, POS_RESTING);
		sprintf (buf, "$n pours you some %s.", liq_table[out->value[2]].liq_name);
		act (buf, ch, NULL, vch, TO_VICT, POS_RESTING);
		sprintf (buf, "$n pours some %s for $N.",
			liq_table[out->value[2]].liq_name);
		act (buf, ch, NULL, vch, TO_NOTVICT, POS_RESTING);
		
	}
}

_DOFUN (do_drink)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int amount;
	int liquid;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		for (obj = ch->in_room->contents; obj; obj = obj->next_content)
		{
			if (obj->item_type == ITEM_FOUNTAIN)
				break;
		}
		
		if (obj == NULL)
		{
			send_to_char ("Drink what?\n\r", ch);
			return;
		}
	}
	else
	{
		if ((obj = get_obj_here (ch, arg)) == NULL)
		{
			send_to_char ("You can't find it.\n\r", ch);
			return;
		}
	}
	
	if (!IS_NPC (ch) && ch->pcdata->condition[_DRUNK] > 10)
	{
		send_to_char ("You fail to reach your mouth.  *Hic*\n\r", ch);
		return;
	}
	
	switch (obj->item_type)
	{
		default:
			send_to_char ("You can't drink from that.\n\r", ch);
			return;
			
		case ITEM_FOUNTAIN:
			if ((liquid = obj->value[2]) < 0)
			{
				bugf("act_obj.c::do_drink] bad liquid number %d.", liquid);
				liquid = obj->value[2] = 0;
			}
			amount = liq_table[liquid].liq_affect[4] * 3;
			break;
			
		case ITEM_DRINK_CON:
			if (obj->value[1] <= 0)
			{
				send_to_char ("It is already empty.\n\r", ch);
				return;
			}
			
			if ((liquid = obj->value[2]) < 0)
			{
				bugf("[act_obj.c::do_drink]  bad liquid number %d.", liquid);
				liquid = obj->value[2] = 0;
			}
			
			amount = liq_table[liquid].liq_affect[4];
			amount = UMIN (amount, obj->value[1]);
			break;
	}
	if (!IS_NPC (ch) && !IS_IMMORTAL (ch)
		&& ch->pcdata->condition[_FULL] > 45)
	{
		send_to_char ("You're too full to drink more.\n\r", ch);
		return;
	}
	
	act ("$n drinks $T from $p.",
		ch, obj, liq_table[liquid].liq_name, TO_ROOM, POS_RESTING);
	act ("You drink $T from $p.",
		ch, obj, liq_table[liquid].liq_name, TO_CHAR, POS_RESTING);
	
	gain_condition (ch, _DRUNK,
		amount * liq_table[liquid].liq_affect[_DRUNK] / 36);
	gain_condition (ch, _FULL,
		amount * liq_table[liquid].liq_affect[_FULL] / 4);
	gain_condition (ch, _THIRST,
		amount * liq_table[liquid].liq_affect[_THIRST] / 10);
	gain_condition (ch, _HUNGER,
		amount * liq_table[liquid].liq_affect[_HUNGER] / 2);
	
	if (!IS_NPC (ch) && ch->pcdata->condition[_DRUNK] > 10)
		send_to_char ("You feel drunk.\n\r", ch);
	if (!IS_NPC (ch) && ch->pcdata->condition[_FULL] > 40)
		send_to_char ("You are full.\n\r", ch);
	if (!IS_NPC (ch) && ch->pcdata->condition[_THIRST] > 40)
		send_to_char ("Your thirst is quenched.\n\r", ch);
	
	if (obj->value[3] != 0)
	{
		/* The drink was poisoned ! */
		AFFECT_DATA af;
		
		act ("$n chokes and gags.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
		send_to_char ("You choke and gag.\n\r", ch);
		
		af.where = TO_AFFECTS;
		af.type = skill_lookup("poison");
		af.level = number_fuzzy (amount);
		af.duration = 3 * amount;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = _AFF_POISON;
		af.owner = -1;
		
		affect_join (ch, &af);
	}
	
	if (obj->value[0] > 0)
		obj->value[1] -= amount;
	
	return;
}



_DOFUN (do_eat)
{
	char arg[MIL];
	OBJ_DATA *obj;
	
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Eat what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	if (!IS_IMMORTAL (ch))
	{
		if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_HERB)
		{
			send_to_char ("That's not edible.\n\r", ch);
			return;
		}
		
		if (!IS_NPC (ch) && ch->pcdata->condition[_FULL] > 40)
		{
			send_to_char ("You are too full to eat more.\n\r", ch);
			return;
		}
	}
	
	act ("$n eats $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
	act ("You eat $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	
	switch (obj->item_type)
	{
		case ITEM_FOOD:
			if (!IS_NPC (ch))
			{
				int condition;
				
				condition = ch->pcdata->condition[_HUNGER];
				gain_condition (ch, _FULL, obj->value[0]);
				gain_condition (ch, _HUNGER, obj->value[1]);
				if (condition == 0 && ch->pcdata->condition[_HUNGER] > 0)
					send_to_char ("You are no longer hungry.\n\r", ch);
				else if (ch->pcdata->condition[_FULL] > 40)
					send_to_char ("You are full.\n\r", ch);
			}
			
			if (obj->value[3] != 0)
			{
				/* The food was poisoned! */
				AFFECT_DATA af;
				
				act ("$n chokes and gags.", ch, 0, 0, TO_ROOM, POS_RESTING);
				send_to_char ("You choke and gag.\n\r", ch);
				
				af.where = TO_AFFECTS;
				af.type = skill_lookup("poison");
				af.level = number_fuzzy (obj->value[0]);
				af.duration = 2 * obj->value[0];
				af.location = APPLY_NONE;
				af.modifier = 0;
				af.bitvector = _AFF_POISON;
				af.owner = -1;
				affect_join (ch, &af);
			}
			break;
			
		case ITEM_HERB:
			obj_cast_spell (obj->value[1], obj->value[0], ch, ch, NULL);
			obj_cast_spell (obj->value[2], obj->value[0], ch, ch, NULL);
			obj_cast_spell (obj->value[3], obj->value[0], ch, ch, NULL);
			break;
			
		default:
			break;
			
	}
	
	extract_obj (obj);
	return;
}



/*
 * Remove an object.
 */
bool remove_obj (CHAR_DATA * ch, int iWear, bool fReplace)
{
	OBJ_DATA *obj;
	
	if ((obj = get_eq_char (ch, iWear)) == NULL)
		return TRUE;
	
	if (!fReplace)
		return FALSE;
	
	if (IS_SET (obj->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL (ch))
	{
		act ("You can't remove $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		return FALSE;
	}
	
	unequip_char (ch, obj);
	
	if (IS_AFFECTED (ch, _AFF_CLOAKED) &&
		obj->pIndexData->vnum == OBJ_VNUM_GAIDIN_CLOAK)
	{
		affect_strip (ch, skill_lookup("cloak"));
		act ("$n seems to materialize out of a multicolored ripple in the air.",
			ch, obj, NULL, TO_ROOM, POS_RESTING);
	}
	else
		act ("$n stops using $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
	
	act ("You stop using $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj (CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace)
{
	char buf[MSL];
	
	if (ch->level < obj->level - 5)
	{
		sprintf (buf, "You must be level %d to use this object.\n\r", obj->level);
		send_to_char (buf, ch);
		act ("$n tries to use $p, but is too inexperienced.",
			ch, obj, NULL, TO_ROOM, POS_RESTING);
		return;
	}
	
	if (obj->item_type == ITEM_LIGHT)
	{
		if (!remove_obj (ch, WEAR_LIGHT, fReplace))
			return;
		act ("$n lights $p and holds it.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You light $p and hold it.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_LIGHT);
		return;
	}
	
	
	if (CAN_WEAR (obj, ITEM_WEAR_FINGER))
	{
		if (get_eq_char (ch, WEAR_FINGER_L) != NULL
			&& get_eq_char (ch, WEAR_FINGER_R) != NULL
			&& !remove_obj (ch, WEAR_FINGER_L, fReplace)
			&& !remove_obj (ch, WEAR_FINGER_R, fReplace))
			return;
		
		if (get_eq_char (ch, WEAR_FINGER_L) == NULL)
		{
			act ("$n wears $p on $s left finger.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You wear $p on your left finger.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_FINGER_L);
			return;
		}
		
		
		if (get_eq_char (ch, WEAR_FINGER_R) == NULL)
		{
			act ("$n wears $p on $s right finger.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You wear $p on your right finger.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_FINGER_R);
			return;
		}
		
		bugf("[act_obj.c::wear_obj] no free finger.");
		send_to_char ("You already wear two rings.\n\r", ch);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_NECK))
	{
		if (get_eq_char (ch, WEAR_NECK_1) != NULL
			&& get_eq_char (ch, WEAR_NECK_2) != NULL
			&& !remove_obj (ch, WEAR_NECK_1, fReplace)
			&& !remove_obj (ch, WEAR_NECK_2, fReplace))
			return;
		
		if (get_eq_char (ch, WEAR_NECK_1) == NULL)
		{
			act ("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You wear $p around your neck.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_NECK_1);
			return;
		}
		
		if (get_eq_char (ch, WEAR_NECK_2) == NULL)
		{
			act ("$n wears $p around $s neck.", ch, obj, NULL, TO_ROOM,
				POS_RESTING);
			act ("You wear $p around your neck.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_NECK_2);
			return;
		}
		
		bugf("[act_obj.c::wear_obj] no free neck.");
		send_to_char ("You already wear two neck items.\n\r", ch);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_EARRING))
	{
		if (get_eq_char (ch, WEAR_EARRING_L) != NULL
			&& get_eq_char (ch, WEAR_EARRING_R) != NULL
			&& !remove_obj (ch, WEAR_EARRING_L, fReplace)
			&& !remove_obj (ch, WEAR_EARRING_R, fReplace))
			return;
		
		if (get_eq_char (ch, WEAR_EARRING_L) == NULL)
		{
			act ("$n pins $p through $s ear.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			act ("You pin $p through your ear.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_EARRING_L);
			return;
		}
		
		if (get_eq_char (ch, WEAR_EARRING_R) == NULL)
		{
			act ("$n pins $p through $s ear.", ch, obj, NULL, TO_ROOM, POS_RESTING);
			act ("You pin $p through your ear.", ch, obj, NULL, TO_CHAR,
				POS_RESTING);
			equip_char (ch, obj, WEAR_EARRING_R);
			return;
		}
		
		bugf("[act_obj.c::wear_obj] no free ear.");
		send_to_char ("You already wear two earrings.\n\r", ch);
		return;
	}
	
	
	if (CAN_WEAR (obj, ITEM_WEAR_BODY))
	{
		if (!remove_obj (ch, WEAR_BODY, fReplace))
			return;
		act ("$n wears $p on $s torso.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your torso.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_BODY);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_SHOULDER))
	{
		if (!remove_obj (ch, WEAR_SHOULDER, fReplace))
			return;
		act ("$n slings $p over $s shoulder.", ch, obj, NULL, TO_ROOM,
			POS_RESTING);
		act ("You sling $p over your shoulder.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_SHOULDER);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_FACE))
	{
		if (!remove_obj (ch, WEAR_FACE, fReplace))
			return;
		act ("$n wears $p around $s face.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p around your face.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_FACE);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_TATTOO))
	{
		if (!remove_obj (ch, WEAR_TATTOO, fReplace))
			return;
		act ("$n scratches $p on $s arm.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You scratch $p on your arm.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_TATTOO);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_SCABBARD))
	{
		if (!remove_obj (ch, WEAR_SCABBARD, fReplace))
			return;
		act ("$n straps $p around $s waist.", ch, obj, NULL, TO_ROOM,
			POS_RESTING);
		act ("You strap $p around your waist.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_SCABBARD);
		return;
	}
	
	
	if (CAN_WEAR (obj, ITEM_WEAR_HEAD))
	{
		if (!remove_obj (ch, WEAR_HEAD, fReplace))
			return;
		act ("$n wears $p on $s head.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your head.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_HEAD);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_LEGS))
	{
		if (!remove_obj (ch, WEAR_LEGS, fReplace))
			return;
		act ("$n wears $p on $s legs.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your legs.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_LEGS);
		return;
	}
	
	
	if (CAN_WEAR (obj, ITEM_WEAR_FEET))
	{
		if (!remove_obj (ch, WEAR_FEET, fReplace))
			return;
		act ("$n wears $p on $s feet.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your feet.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_FEET);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_HANDS))
	{
		if (!remove_obj (ch, WEAR_HANDS, fReplace))
			return;
		act ("$n wears $p on $s hands.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your hands.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_HANDS);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_ARMS))
	{
		if (!remove_obj (ch, WEAR_ARMS, fReplace))
			return;
		act ("$n wears $p on $s arms.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p on your arms.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_ARMS);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_ABOUT))
	{
		if (!remove_obj (ch, WEAR_ABOUT, fReplace))
			return;
		act ("$n wears $p about $s torso.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p about your torso.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_ABOUT);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_WAIST))
	{
		if (!remove_obj (ch, WEAR_WAIST, fReplace))
			return;
		act ("$n wears $p about $s waist.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p about your waist.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_WAIST);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_WRIST))
	{
		if (get_eq_char (ch, WEAR_WRIST_L) != NULL
			&& get_eq_char (ch, WEAR_WRIST_R) != NULL
			&& !remove_obj (ch, WEAR_WRIST_L, fReplace)
			&& !remove_obj (ch, WEAR_WRIST_R, fReplace))
			return;
		
		if (get_eq_char (ch, WEAR_WRIST_L) == NULL)
		{
			act ("$n wears $p around $s left wrist.",
				ch, obj, NULL, TO_ROOM, POS_RESTING);
			act ("You wear $p around your left wrist.",
				ch, obj, NULL, TO_CHAR, POS_RESTING);
			equip_char (ch, obj, WEAR_WRIST_L);
			return;
		}
		
		if (get_eq_char (ch, WEAR_WRIST_R) == NULL)
		{
			act ("$n wears $p around $s right wrist.",
				ch, obj, NULL, TO_ROOM, POS_RESTING);
			act ("You wear $p around your right wrist.",
				ch, obj, NULL, TO_CHAR, POS_RESTING);
			equip_char (ch, obj, WEAR_WRIST_R);
			return;
		}
		
		bugf("[act_obj.c::wear_obj] no free wrist.");
		send_to_char ("You already wear two wrist items.\n\r", ch);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_SHIELD))
	{
		OBJ_DATA *weapon;
		
		/*
 		 * Should not be able to wear a shield if you are using a 
		 * two-handed weapon. Fix added by [GM, 3/98]
		 */
		weapon = get_eq_char (ch, WEAR_WIELD);
		
		if (weapon != NULL &&
			(IS_WEAPON_STAT (weapon, WEAPON_TWO_HANDS) && ch->size < SIZE_LARGE))
		{
			send_to_char
				("You cannot wear a shield while using a 2 handed weapon!\n\r", ch);
			return;
		}
		
		/* Cant wear a shield if using 2 weapons, unless the weight of the
		 * sheild is >= 5 and you are in the Aiel guild. 
		 * (ie: aiel can dual 2 spears and wear a sm. buckler strapped to arm)
		 */
		if (get_eq_char (ch, WEAR_SECONDARY) != NULL
			&& ((!is_clan (ch)
			|| !IS_FLAG (clan_table[ch->clan].flags, _AIEL)
			|| obj->weight >= 5)))
		{
			send_to_char ("You cannot use a shield while using 2 weapons.\n\r", ch);
			return;
		}
		
		if (!remove_obj (ch, WEAR_SHIELD, fReplace))
			return;
		act ("$n wears $p as a shield.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wear $p as a shield.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_SHIELD);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WIELD))
	{
		int sn, skill;
		
		if (!remove_obj (ch, WEAR_WIELD, fReplace))
			return;
		
		if (!IS_NPC (ch)
			&& get_obj_weight (obj) > (str_app[get_curr_stat (ch, STAT_STR)].wield * 10))
		{
			send_to_char ("It is too heavy for you to wield.\n\r", ch);
			return;
		}
		
		if (!IS_NPC (ch) && ch->size < SIZE_LARGE
			&& IS_WEAPON_STAT (obj, WEAPON_TWO_HANDS)
			&& (get_eq_char (ch, WEAR_SHIELD) != NULL
			|| get_eq_char (ch, WEAR_WIELD) != NULL
			|| get_eq_char (ch, WEAR_SECONDARY) != NULL))
		{
			send_to_char ("You need two hands free for that weapon.\n\r", ch);
			return;
		}
		
		act ("$n wields $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You wield $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_WIELD);
		
		sn = get_weapon_sn (ch, FALSE);
		
		if (sn == skill_lookup("hand to hand"))
			return;
		
		skill = get_weapon_skill (ch, sn);
		
		if (skill >= 100)
			act ("$p feels like a part of you!", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		else if (skill > 85)
			act ("You feel quite confident with $p.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		else if (skill > 70)
			act ("You are skilled with $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		else if (skill > 50)
			act ("Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		else if (skill > 25)
			act ("$p feels a little clumsy in your hands.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		else if (skill > 1)
			act ("You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		else
			act ("You don't even know which end is up on $p.", ch, obj, NULL,
			TO_CHAR, POS_RESTING);
		
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_HOLD))
	{
		if (get_eq_char (ch, WEAR_SECONDARY) != NULL)
		{
			send_to_char ("You cannot hold an item while using 2 weapons.\n\r", ch);
			return;
		}
		
		if (!remove_obj (ch, WEAR_HOLD, fReplace))
			return;
		act ("$n holds $p in $s hands.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		act ("You hold $p in your hands.", ch, obj, NULL, TO_CHAR, POS_RESTING);
		equip_char (ch, obj, WEAR_HOLD);
		return;
	}
	
	if (CAN_WEAR (obj, ITEM_WEAR_FLOAT))
	{
		if (!remove_obj (ch, WEAR_FLOAT, fReplace))
			return;
		act ("$n releases $p to float next to $m.", ch, obj, NULL, TO_ROOM,
			POS_RESTING);
		act ("You release $p and it floats next to you.", ch, obj, NULL, TO_CHAR,
			POS_RESTING);
		equip_char (ch, obj, WEAR_FLOAT);
		return;
	}
	
	if (fReplace)
		send_to_char ("You can't wear, wield, or hold that.\n\r", ch);
	
	return;
}



_DOFUN (do_wear)
{
	char arg[MIL];
	OBJ_DATA *obj;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Wear, wield, or hold what?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "all"))
	{
		OBJ_DATA *obj_next;
		
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
			{
				if (obj->condition < 21)
				{
					chprintf (ch, "Your %s is in to bad of shape to use.\n\r",
						obj->short_descr);
				}
				else
					wear_obj (ch, obj, TRUE);
			}
		}
		return;
	}
	else
	{
		if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}
		
		if (obj->condition < 21)
		{
			chprintf (ch, "Your %s is in to bad of shape to use.\n\r",
				obj->short_descr);
		}
		else
			wear_obj (ch, obj, TRUE);
	}
	
	return;
}



_DOFUN (do_remove)
{
	char arg[MIL];
	OBJ_DATA *obj;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Remove what?\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg, "all"))
	{
		bool found = FALSE;
		for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
		{
			if (obj->wear_loc != WEAR_NONE && can_see_obj (ch, obj))
			{
				if (!found)
					found = TRUE;
				remove_obj (ch, obj->wear_loc, TRUE);
			}
		}
		
		if (!found)
			send_to_char ("You are not wearing anything.\n\r", ch);
		
		return;
	}
	
	if ((obj = get_obj_wear (ch, arg)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	
	remove_obj (ch, obj->wear_loc, TRUE);
	return;
}



_DOFUN (do_sacrifice)
{
	char arg[MIL];
	char buf[MSL];
	OBJ_DATA *obj;
	int silver;
	
	/* variables for AUTOSPLIT */
	CHAR_DATA *gch;
	int members;
	char buffer[100];
	
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0' || !str_cmp (arg, ch->name))
	{
		act ("$n offers $mself to the Creator, who graciously declines.",
			ch, NULL, NULL, TO_ROOM, POS_RESTING);
		send_to_char
			("The Creator appreciates your offer and may accept it later.\n\r", ch);
		return;
	}
	
	obj = get_obj_list (ch, arg, ch->in_room->contents);
	if (obj == NULL)
	{
		send_to_char ("You can't find it.\n\r", ch);
		return;
	}
	
	if (obj->item_type == ITEM_CORPSE_PC)
	{
		if (obj->contains)
		{
			send_to_char ("The Creator wouldn't like that.\n\r", ch);
			return;
		}
	}
	
	
	if (!CAN_WEAR (obj, ITEM_TAKE) || CAN_WEAR (obj, ITEM_NO_SAC))
	{
		act ("$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR,
			POS_RESTING);
		return;
	}
	
	if (obj->in_room != NULL)
	{
		for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
			if (gch->on == obj)
			{
				act ("$N appears to be using $p.",
					ch, obj, gch, TO_CHAR, POS_RESTING);
				return;
			}
	}
	
	silver = UMAX (1, obj->level * 3);
	
	if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
		silver = UMIN (silver, obj->cost);
	
	if (silver == 1)
		send_to_char
		("The Creator gives you one silver coin for your sacrifice.\n\r", ch);
	else
	{
		sprintf (buf,
			"The Creator gives you %d silver coins for your sacrifice.\n\r",
			silver);
		send_to_char (buf, ch);
	}
	
	ch->silver += silver;
	
	if (IS_FLAG (ch->states.config, _AUTOSPLIT))
	{
		members = 0;
		for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
		{
			if (is_same_group (gch, ch))
				members++;
		}
		
		if (members > 1 && silver > 1)
		{
			sprintf (buffer, "%d", silver);
			do_split (ch, buffer);
		}
	}
	
	act ("$n sacrifices $p to the Creator.", ch, obj, NULL, TO_ROOM,
		POS_RESTING);
	wiznet ("$N sends up $p as a burnt offering.", ch, obj, WIZ_SACCING, 0, 0);
	extract_obj (obj);
	return;
}



_DOFUN (do_quaff)
{
	char arg[MIL];
	OBJ_DATA *obj;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Quaff what?\n\r", ch);
		return;
	}
	
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		send_to_char ("You do not have that potion.\n\r", ch);
		return;
	}
	
	if (obj->item_type != ITEM_POTION)
	{
		send_to_char ("You can quaff only potions.\n\r", ch);
		return;
	}
	
	if (ch->level < obj->level)
	{
		send_to_char ("This liquid is too powerful for you to drink.\n\r", ch);
		return;
	}
	
	act ("$n quaffs $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
	act ("You quaff $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	
	obj_cast_spell (obj->value[1], obj->value[0], ch, ch, NULL);
	obj_cast_spell (obj->value[2], obj->value[0], ch, ch, NULL);
	obj_cast_spell (obj->value[3], obj->value[0], ch, ch, NULL);
	
	extract_obj (obj);
	return;
}



_DOFUN (do_steal)
{
	char buf[MSL];
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int percent;
	int iSkill = skill_lookup("steal");

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Steal what from whom?\n\r", ch);
		return;
	}
	
	if ((victim = get_char_room (ch, arg2)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("That's pointless.\n\r", ch);
		return;
	}
	
	if (is_safe (ch, victim))
		return;
	
	if (!IS_FLAG(pMud.config, _ALLOW_KILL_STEALING) && IS_NPC (victim) && victim->position == POS_FIGHTING && victim->fighting != ch)
	{
		send_to_char ("Kill stealing is not permitted.\n\r"
			"You'd better not -- you might get hit.\n\r", ch);
		return;
	}
	
	WAIT_STATE (ch, skill_table[iSkill].beats);
	percent = number_percent ();
	
	if (!IS_AWAKE (victim))
		percent -= 10;
	else if (!can_see (victim, ch))
		percent += 25;
	else
		percent += 50;
	
	if (((ch->level + 7 < victim->level || ch->level - 7 > victim->level)
		&& !IS_NPC (victim) && !IS_NPC (ch))
		|| (!IS_NPC (ch) && percent > get_skill (ch, iSkill))
		|| (!IS_NPC (ch) && !is_clan (ch)))
	{
		/*
		 * Failure.
		 */
		send_to_char ("Oops.\n\r", ch);
		affect_strip (ch, gsn_sneak);
		
		act ("$n tried to steal from you.\n\r",
			ch, NULL, victim, TO_VICT, POS_RESTING);
		act ("$n tried to steal from $N.\n\r",
			ch, NULL, victim, TO_NOTVICT, POS_RESTING);
		
		if (!IS_NPC(ch))
			ch->pcdata->notoriety[INFAMY]+=5;
		
		switch (number_range (0, 3))
		{
			case 0:
				sprintf (buf, "%s is a lousy thief!", ch->name);
				break;
			case 1:
				sprintf (buf, "%s couldn't rob %s way out of a paper bag!",
					ch->name, (ch->sex == 2) ? "her" : "his");
				break;
			case 2:
				sprintf (buf, "%s tried to rob me!", ch->name);
				break;
			case 3:
				sprintf (buf, "Keep your hands out of there, %s!", ch->name);
				break;
		}
		if (!IS_AWAKE (victim))
			do_wake (victim, "");
		if (IS_AWAKE (victim))
			do_yell (victim, buf);
		if (!IS_NPC (ch))
		{
			if (IS_NPC (victim))
			{
				check_improve (ch, iSkill, FALSE, 2);
				multi_hit (victim, ch, TYPE_UNDEFINED);
			}
			else
			{
				sprintf (buf, "$N tried to steal from %s.", victim->name);
				wiznet (buf, ch, NULL, WIZ_FLAGS, 0, 0);
			}
		}
		
		return;
	}
	
	if (!str_cmp (arg1, "coin")
		|| !str_cmp (arg1, "coins")
		|| !str_cmp (arg1, "gold") || !str_cmp (arg1, "silver"))
	{
		int gold, silver;
		
		gold = victim->gold * number_range (1, ch->level) / 60;
		silver = victim->silver * number_range (1, ch->level) / 60;
		if (gold <= 0 && silver <= 0)
		{
			send_to_char ("You couldn't get any coins.\n\r", ch);
			return;
		}
		
		ch->gold += gold;
		ch->silver += silver;
		victim->silver -= silver;
		victim->gold -= gold;
		if (silver <= 0)
			sprintf (buf, "Bingo!  You got %d gold coins.\n\r", gold);
		else if (gold <= 0)
			sprintf (buf, "Bingo!  You got %d silver coins.\n\r", silver);
		else
			sprintf (buf, "Bingo!  You got %d silver and %d gold coins.\n\r",
			silver, gold);
		
		send_to_char (buf, ch);
		check_improve (ch, iSkill, TRUE, 2);
		return;
	}
	
	if ((obj = get_obj_carry (victim, arg1, ch)) == NULL)
	{
		send_to_char ("You can't find it.\n\r", ch);
		return;
	}
	
	if (!can_drop_obj (ch, obj)
		|| IS_SET (obj->extra_flags, ITEM_INVENTORY) || obj->level > ch->level)
	{
		send_to_char ("You can't pry it away.\n\r", ch);
		return;
	}
	
	if (ch->carry_number + get_obj_number (obj) > can_carry_n (ch))
	{
		send_to_char ("You have your hands full.\n\r", ch);
		return;
	}
	
	if (ch->carry_weight + get_obj_weight (obj) > can_carry_w (ch))
	{
		send_to_char ("You can't carry that much weight.\n\r", ch);
		return;
	}
	
	obj_from_char (obj);
	obj_to_char (obj, ch);
	act ("You pocket $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	check_improve (ch, iSkill, TRUE, 2);
	send_to_char ("Got it!\n\r", ch);
	
	if ( (!IS_NPC(ch) && ch->c_class == ROGUE) || IS_FLAG(ch->states.player, _ACT_ROGUE) )
		ch->pcdata->notoriety[FAME]+=5;
	
	return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper (CHAR_DATA * ch)
{
	CHAR_DATA *keeper;
	SHOP_DATA *pShop;
	
	pShop = NULL;
	for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
	{
		if (IS_NPC (keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
			break;
	}
	
	if (pShop == NULL)
	{
		send_to_char ("You can't do that here.\n\r", ch);
		return NULL;
	}
	
	/*
	 * Shop hours.
	 */
	if (time_info.hour < pShop->open_hour)
	{
		do_say (keeper, "Sorry, I am closed. Come back later.");
		return NULL;
	}
	
	if (time_info.hour > pShop->close_hour)
	{
		do_say (keeper, "Sorry, I am closed. Come back tomorrow.");
		return NULL;
	}
	
	/*
	 * Invisible or hidden people.
	 */
	if (!can_see (keeper, ch))
	{
		do_say (keeper, "I don't trade with folks I can't see.");
		return NULL;
	}
	
	return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper (OBJ_DATA * obj, CHAR_DATA * ch)
{
	OBJ_DATA *t_obj, *t_obj_next;
	
	/* see if any duplicates are found */
	for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
	{
		t_obj_next = t_obj->next_content;
		
		if (obj->pIndexData == t_obj->pIndexData
			&& !str_cmp (obj->short_descr, t_obj->short_descr))
		{
			/* if this is an unlimited item, destroy the new one */
			if (IS_OBJ_STAT (t_obj, ITEM_INVENTORY))
			{
				extract_obj (obj);
				return;
			}
			obj->cost = t_obj->cost;	/* keep it standard */
			break;
		}
	}
	
	if (t_obj == NULL)
	{
		obj->next_content = ch->carrying;
		ch->carrying = obj;
	}
	else
	{
		obj->next_content = t_obj->next_content;
		t_obj->next_content = obj;
	}
	
	obj->carried_by = ch;
	obj->in_room = NULL;
	obj->in_obj = NULL;
	ch->carry_number += get_obj_number (obj);
	ch->carry_weight += get_obj_weight (obj);
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper (CHAR_DATA * ch, CHAR_DATA * keeper, char *argument)
{
	char arg[MIL];
	OBJ_DATA *obj;
	int number;
	int count;
	
	number = number_argument (argument, arg);
	count = 0;
	for (obj = keeper->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc == WEAR_NONE
			&& can_see_obj (keeper, obj)
			&& can_see_obj (ch, obj) && is_name (arg, obj->name))
		{
			if (++count == number)
				return obj;
			
			/* skip other objects of the same name */
			while (obj->next_content != NULL
				&& obj->pIndexData == obj->next_content->pIndexData
				&& !str_cmp (obj->short_descr, obj->next_content->short_descr))
				obj = obj->next_content;
		}
	}
	
	return NULL;
}

int get_cost (CHAR_DATA * keeper, OBJ_DATA * obj, bool fBuy)
{
	SHOP_DATA *pShop;
	int cost;
	
	if (obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL)
		return 0;
	
	if (fBuy)
	{
		cost = (int)(obj->cost * ( (float)pShop->profit_buy / 100));
	}
	else
	{
		OBJ_DATA *obj2;
		int itype;
		
		cost = 0;
		for (itype = 0; itype < MAX_TRADE; itype++)
		{
			if (obj->item_type == pShop->buy_type[itype])
			{
				cost = (int)(obj->cost * ( (float)pShop->profit_buy / 100));
				break;
			}
		}
		
		for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content)
		{
			if (obj->pIndexData == obj2->pIndexData
				&& !str_cmp (obj->short_descr, obj2->short_descr))
			{
				if (IS_OBJ_STAT (obj2, ITEM_INVENTORY))
					cost /= 2;
				else
					cost = cost * 3 / 4;
			}
		}
	}
	
	return cost;
}

/*
 * Find a stablehand
 */
CHAR_DATA *find_stableboy (CHAR_DATA * ch)
{
	CHAR_DATA *keeper;
	bool vFound = FALSE;
	
	for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
	{
		if (IS_NPC (keeper) && IS_FLAG (keeper->states.player, _ACT_STABLEHAND))
		{
			vFound = TRUE;
			break;
		}
	}
	
	if (!vFound)
	{
		send_to_char ("You can't do that here.\n\r", ch);
		return NULL;
	}
	
	/*
	 * Invisible or hidden people.
	 */
	if (!can_see (keeper, ch) && !IS_IMMORTAL (ch))
	{
		do_say (keeper, "I don't trade with folks I can't see.");
		return NULL;
	}
	
	return keeper;
}


/** Function: do_stable
  * Descr   : Allows a character to stable their mount in a stable, and retreive it later.
  * Returns : n/a
  * Syntax  : stable
  * Written : 11/1/99
  * Author  : Gary McNickle
  */
_DOFUN (do_stable)
{
	CHAR_DATA *boy = NULL;
	
	if (IS_NPC (ch))
	{
		send_to_char ("Only for PC's.\n\r", ch);
		return;
	}
	
	if (!IS_SET (ch->in_room->room_flags, ROOM_STABLE))
	{
		send_to_char ("You cant do that here.\n\r", ch);
		return;
	}
	
	
	if ((boy = find_stableboy (ch)) == NULL)
	{
		send_to_char ("You cant seem to find the stablehand.\n\r", ch);
		return;
	}
	
	/* 
	 * Leave the mount, if any, here.
	 */
	if (ch->vstable != boy->in_room->vnum)
	{
		if (ch->mount == NULL)
		{
			if (ch->sex == SEX_FEMALE)
				do_say (boy, "But m'lady, your horse, he is no stabled 'ere.");
			else
				do_say (boy, "But m'lord, your horse, he is no stabled 'ere.");
			return;
		}
		
		if (ch->mount->in_room != ch->in_room)
		{
			if (boy->sex == SEX_MALE)
				do_emote (boy, "scratches his head in confusion.");
			else
				do_emote (boy, "scratches her head in confusion.");
			
			do_say (boy, "But, I do no see any horse...");
			return;
		}
		
		if (MOUNTED (ch))
			do_dismount (ch, "");
		
		ch->vstable = boy->in_room->vnum;
		SET_FLAG (ch->mount->states.player, _ACT_SENTINEL);
		char_from_room (ch->mount);
		
		if (ch->sex == SEX_MALE)
			do_say (boy,
			"No you worry m'lord, I will take good care 'o the animal.");
		else
			do_say (boy,
			"No you worry m'lady, I will take good care 'o the animal.");
		return;
	}
	
	/*
     * Else, mount was already dropped off here. Pick it up.
     */
	ch->vstable = 0;
	char_to_room (ch->mount, ch->in_room);
	REMOVE_FLAG (ch->mount->states.player, _ACT_SENTINEL);
	
	act ("$n leads $N out of the stable.",
		boy, NULL, ch->mount, TO_ROOM, POS_RESTING);
	
	if (ch->sex == SEX_FEMALE)
		send_to_char ("'ere you are m'lady. I've takin good care 'o 'em.\n\r",ch);
	else
		send_to_char ("'ere you are m'lord. I've takin good care 'o 'em.\n\r",ch);
	
}


_DOFUN (do_buy)
{
	char buf[MSL];
	int cost, roll;
	int iHaggle = skill_lookup("haggle");
	
	if (argument[0] == '\0')
	{
		send_to_char ("Buy what?\n\r", ch);
		return;
	}
	
	if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP)
		|| IS_SET (ch->in_room->room_flags, ROOM_MOUNT_SHOP))
	{
		char arg[MIL];
		char buf[MSL];
		CHAR_DATA *pet;
		ROOM_INDEX_DATA *pRoomIndexNext;
		ROOM_INDEX_DATA *in_room;
		
		smash_tilde (argument);
		
		if (IS_NPC (ch))
			return;
		
		argument = one_argument (argument, arg);
		
		pRoomIndexNext = get_room_index (ch->in_room->vnum + 1);
		if (pRoomIndexNext == NULL)
		{
			bugf("[act_obj.c::do_buy] bad pet shop at vnum %d.", ch->in_room->vnum);
			send_to_char ("Sorry, you can't buy that here.\n\r", ch);
			return;
		}
		
		in_room = ch->in_room;
		ch->in_room = pRoomIndexNext;
		pet = get_char_room (ch, arg);
		ch->in_room = in_room;
		
		if (pet == NULL || !IS_FLAG (pet->states.player, _ACT_PET))
		{
			send_to_char ("Sorry, you can't buy that here.\n\r", ch);
			return;
		}
		
		if (IS_FLAG (pet->states.player, _ACT_MOUNT) && ch->mount)
		{
			send_to_char ("You already own a mount.\n\r", ch);
			return;
		}
		
		if (ch->pet != NULL)
		{
			send_to_char ("You already own a pet.\n\r", ch);
			return;
		}
		
		cost = 10 * pet->level * pet->level;
		
		if ((ch->silver + 100 * ch->gold) < cost)
		{
			send_to_char ("You can't afford it.\n\r", ch);
			return;
		}
		
		if (ch->level < pet->level)
		{
			send_to_char ("You're not powerful enough to master this beast.\n\r",
				ch);
			return;
		}
		
		/* haggle */
		roll = number_percent ();
		if (roll < get_skill (ch, iHaggle))
		{
			cost -= cost / 2 * roll / 100;
			sprintf (buf, "You haggle the price down to %d coins.\n\r", cost);
			send_to_char (buf, ch);
			check_improve (ch, iHaggle, TRUE, 4);
			
		}
		
		deduct_cost (ch, cost);
		pet = create_mobile (pet->pIndexData);
		SET_FLAG (pet->states.player, _ACT_PET);
		SET_FLAG (pet->states.affected_by, _AFF_COMPULSION);
		
		SET_FLAG (pet->states.penalties, _NOTELL);
		SET_FLAG (pet->states.penalties, _NOSHOUT);
		SET_FLAG (pet->states.penalties, _NOCHANNELS);
		
		argument = one_argument (argument, arg);
		if (arg[0] != '\0')
		{
			sprintf (buf, "%s %s", pet->name, arg);
			free_string (pet->name);
			pet->name = str_dup (buf);
		}
		
		sprintf (buf, "%sA neck tag says 'I belong to %s'.\n\r",
			pet->description, ch->name);
		free_string (pet->description);
		pet->description = str_dup (buf);
		
		char_to_room (pet, ch->in_room);
		add_follower (pet, ch, TRUE);
		pet->leader = ch;
		if (IS_FLAG (pet->states.player, _ACT_MOUNT))
		{
			send_to_char ("Enjoy your mount.\n\r", ch);
			act ("$n bought $N as a mount.", ch, NULL, pet, TO_ROOM, POS_RESTING);
			ch->mount = pet;
			ch->riding = FALSE;
			return;
		}
		else
			ch->pet = pet;
		send_to_char ("Enjoy your pet.\n\r", ch);
		act ("$n bought $N as a pet.", ch, NULL, pet, TO_ROOM, POS_RESTING);
		return;
  }
  else
  {
	  CHAR_DATA *keeper;
	  OBJ_DATA *obj, *t_obj;
	  char arg[MIL];
	  int number, count = 1;
	  
	  if ((keeper = find_keeper (ch)) == NULL)
		  return;
	  
	  number = mult_argument (argument, arg);
	  obj = get_obj_keeper (ch, keeper, arg);
	  cost = get_cost (keeper, obj, TRUE);
	  
	  if (number < 1)
	  {
		  act ("$n tells you 'Get real!", keeper, NULL, ch, TO_VICT, POS_RESTING);
		  return;
	  }
	  
	  if (cost <= 0 || !can_see_obj (ch, obj))
	  {
		  act ("$n tells you 'I don't sell that -- try 'list''.",
			  keeper, NULL, ch, TO_VICT, POS_RESTING);
		  ch->reply = keeper;
		  return;
	  }
	  
	  if (!IS_OBJ_STAT (obj, ITEM_INVENTORY))
	  {
		  for (t_obj = obj->next_content;
		  count < number && t_obj != NULL; t_obj = t_obj->next_content)
		  {
			  if (t_obj->pIndexData == obj->pIndexData
				  && !str_cmp (t_obj->short_descr, obj->short_descr))
				  count++;
			  else
				  break;
		  }
		  
		  if (count < number)
		  {
			  act ("$n tells you 'I don't have that many in stock.",
				  keeper, NULL, ch, TO_VICT, POS_RESTING);
			  ch->reply = keeper;
			  return;
		  }
	  }
	  
	  if ((ch->silver + ch->gold * 100) < cost * number)
	  {
		  if (number > 1)
			  act ("$n tells you 'You can't afford to buy that many.",
			  keeper, obj, ch, TO_VICT, POS_RESTING);
		  else
			  act ("$n tells you 'You can't afford to buy $p'.",
			  keeper, obj, ch, TO_VICT, POS_RESTING);
		  ch->reply = keeper;
		  return;
	  }
	  
	  if (obj->level - 5 > ch->level)
	  {
		  act ("$n tells you 'You can't use $p yet'.",
			  keeper, obj, ch, TO_VICT, POS_RESTING);
		  ch->reply = keeper;
		  return;
	  }
	  
	  if (ch->carry_number + number * get_obj_number (obj) > can_carry_n (ch))
	  {
		  send_to_char ("You can't carry that many items.\n\r", ch);
		  return;
	  }
	  
	  if (ch->carry_weight + number * get_obj_weight (obj) > can_carry_w (ch))
	  {
		  send_to_char ("You can't carry that much weight.\n\r", ch);
		  return;
	  }
	  
	  /* haggle */
	  roll = number_percent ();
	  if (roll < get_skill (ch, iHaggle))
	  {
		  cost -= obj->cost / 2 * roll / 100;
		  act ("You haggle with $N.", ch, NULL, keeper, TO_CHAR, POS_RESTING);
		  check_improve (ch, iHaggle, TRUE, 4);
	  }
	  
	  if (number > 1)
	  {
		  sprintf (buf, "$n buys $p[%d].", number);
		  act (buf, ch, obj, NULL, TO_ROOM, POS_RESTING);
		  sprintf (buf, "You buy $p[%d] for %d silver.", number, cost * number);
		  act (buf, ch, obj, NULL, TO_CHAR, POS_RESTING);
	  }
	  else
	  {
		  act ("$n buys $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
		  sprintf (buf, "You buy $p for %d silver.", cost);
		  act (buf, ch, obj, NULL, TO_CHAR, POS_RESTING);
	  }
	  deduct_cost (ch, cost * number);
	  keeper->gold += cost * number / 100;
	  keeper->silver += cost * number - (cost * number / 100) * 100;
	  
	  for (count = 0; count < number; count++)
	  {
		  if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
			  t_obj = create_object (obj->pIndexData, obj->level);
		  else
		  {
			  t_obj = obj;
			  obj = obj->next_content;
			  obj_from_char (t_obj);
		  }
		  if (t_obj == NULL)
		  {
			  send_to_char ("BUG: do_buy:Invalid t_obj. Logged.\n\r", ch);
			  bugf ("do_buy:invalid t_obj.");
			  return;
		  }
		  
		  if (t_obj->timer > 0 && !IS_OBJ_STAT (t_obj, ITEM_HAD_TIMER))
			  t_obj->timer = 0;
		  REMOVE_BIT (t_obj->extra_flags, ITEM_HAD_TIMER);
		  obj_to_char (t_obj, ch);
		  if (cost < t_obj->cost)
			  t_obj->cost = cost;
	  }
  }
}



_DOFUN (do_list)
{
	char buf[MSL];
	
	if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
	{
		ROOM_INDEX_DATA *pRoomIndexNext;
		CHAR_DATA *pet;
		bool found;
		
		pRoomIndexNext = get_room_index (ch->in_room->vnum + 1);
		
		if (pRoomIndexNext == NULL)
		{
			bugf("[act_obj.c::do_list] bad pet shop at vnum %d.", ch->in_room->vnum);
			send_to_char ("You can't do that here.\n\r", ch);
			return;
		}
		
		found = FALSE;
		for (pet = pRoomIndexNext->people; pet; pet = pet->next_in_room)
		{
			
			if (IS_FLAG (pet->states.player, _ACT_PET)
				|| IS_FLAG (pet->states.player, _ACT_MOUNT))
			{
				if (!found)
				{
					found = TRUE;
					if (IS_FLAG (pet->states.player, _ACT_PET))
						send_to_char ("Pets for sale:\n\r", ch);
					else if (IS_FLAG (pet->states.player, _ACT_MOUNT))
						send_to_char ("Mounts for sale:\n\r", ch);
				}
				sprintf (buf, "[%2d] %8d - %s\n\r",
					pet->level, 10 * pet->level * pet->level, pet->short_descr);
				send_to_char (buf, ch);
			}
		}
		if (!found)
		{
			if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
				send_to_char ("Sorry, we're out of pets right now.\n\r", ch);
			else
				send_to_char ("Sorry, we're out of mounts right now.\n\r", ch);
		}
		return;
	}
	else
	{
		CHAR_DATA *keeper;
		OBJ_DATA *obj;
		int cost, count;
		bool found;
		char arg[MIL];
		
		if ((keeper = find_keeper (ch)) == NULL)
			return;
		
		one_argument (argument, arg);
		
		found = FALSE;
		for (obj = keeper->carrying; obj; obj = obj->next_content)
		{
			if (obj->wear_loc == WEAR_NONE
				&& can_see_obj (ch, obj)
				&& (cost = get_cost (keeper, obj, TRUE)) > 0
				&& (arg[0] == '\0' || is_name (arg, obj->name)))
			{
				if (!found)
				{
					found = TRUE;
					send_to_char ("[Lv Price Qty] Item\n\r", ch);
				}
				
				if (IS_OBJ_STAT (obj, ITEM_INVENTORY))
					sprintf (buf, "[%2d %5d -- ] %s\n\r",
					obj->level, cost, obj->short_descr);
				else
				{
					count = 1;
					
					while (obj->next_content != NULL
						&& obj->pIndexData == obj->next_content->pIndexData
						&& !str_cmp (obj->short_descr,
						obj->next_content->short_descr))
					{
						obj = obj->next_content;
						count++;
					}
					sprintf (buf, "[%2d %5d %2d ] %s\n\r",
						obj->level, cost, count, obj->short_descr);
				}
				send_to_char (buf, ch);
			}
		}
		
		if (!found)
			send_to_char ("You can't buy anything here.\n\r", ch);
		return;
	}
}



_DOFUN (do_sell)
{
	char buf[MSL];
	char arg[MIL];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost, roll;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Sell what?\n\r", ch);
		return;
	}
	
	if ((keeper = find_keeper (ch)) == NULL)
		return;
	
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		act ("$n tells you 'You don't have that item'.",
			keeper, NULL, ch, TO_VICT, POS_RESTING);
		ch->reply = keeper;
		return;
	}
	
	if (!can_drop_obj (ch, obj))
	{
		send_to_char ("You can't let go of it.\n\r", ch);
		return;
	}
	
	if (!can_see_obj (keeper, obj))
	{
		act ("$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT,
			POS_RESTING);
		return;
	}
	
	if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
	{
		act ("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT,
			POS_RESTING);
		return;
	}
	if (cost > (keeper->silver + 100 * keeper->gold))
	{
		act ("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
			keeper, obj, ch, TO_VICT, POS_RESTING);
		return;
	}
	
	act ("$n sells $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
	/* haggle */
	roll = number_percent ();
	if (roll < get_skill (ch, skill_lookup("haggle")))
	{
		send_to_char ("You haggle with the shopkeeper.\n\r", ch);
		cost += obj->cost / 2 * roll / 100;
		cost = UMIN (cost, 95 * get_cost (keeper, obj, TRUE) / 100);
		cost = UMIN (cost, (keeper->silver + 100 * keeper->gold));
		check_improve (ch, skill_lookup("haggle"), TRUE, 4);
	}
	sprintf (buf, "You sell $p for %d silver and %d gold piece%s.",
		cost - (cost / 100) * 100, cost / 100, cost == 1 ? "" : "s");

	act (buf, ch, obj, NULL, TO_CHAR, POS_RESTING);
	ch->gold += cost / 100;
	ch->silver += cost - (cost / 100) * 100;

	deduct_cost (keeper, cost);
	
	if (keeper->gold < 0)
		keeper->gold = 0;
	if (keeper->silver < 0)
		keeper->silver = 0;
	
	if (obj->item_type == ITEM_TRASH )
	{
		extract_obj (obj);
	}
	else
	{
		obj_from_char (obj);
		if (obj->timer)
			SET_BIT (obj->extra_flags, ITEM_HAD_TIMER);
		else
			obj->timer = number_range (50, 100);
		obj_to_keeper (obj, keeper);
	}
	
	return;
}



_DOFUN (do_value)
{
	char buf[MSL];
	char arg[MIL];
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost;
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Value what?\n\r", ch);
		return;
	}
	
	if ((keeper = find_keeper (ch)) == NULL)
		return;
	
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		act ("$n tells you 'You don't have that item'.",
			keeper, NULL, ch, TO_VICT, POS_RESTING);
		ch->reply = keeper;
		return;
	}
	
	if (!can_see_obj (keeper, obj))
	{
		act ("$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT,
			POS_RESTING);
		return;
	}
	
	if (!can_drop_obj (ch, obj))
	{
		send_to_char ("You can't let go of it.\n\r", ch);
		return;
	}
	
	if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
	{
		act ("$n looks uninterested in $p.", keeper, obj, ch, TO_VICT,
			POS_RESTING);
		return;
	}
	
	sprintf (buf,
		"$n tells you 'I'll give you %d silver and %d gold coins for $p'.",
		cost - (cost / 100) * 100, cost / 100);
	act (buf, keeper, obj, ch, TO_VICT, POS_RESTING);
	ch->reply = keeper;
	
	return;
}


_DOFUN (do_restring)
{
	char arg[MIL];
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	int cost = 0;
	bool verifyName = FALSE;
	
	for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
	{
		if (IS_NPC (mob) && IS_FLAG (mob->states.player, _ACT_RESTRINGER))
			break;
	}
	
	if (mob == NULL)
	{
		send_to_char ("You can't do that here.\n\r", ch);
		return;
	}
	
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg1);
	strcpy (arg2, argument);
	
	if (arg[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  restring <obj> <field> <string>\n\r", ch);
		send_to_char ("    fields: name short long\n\r", ch);
		return;
	}
	
	if ((obj = (get_obj_carry (ch, arg, mob))) == NULL)
	{
		send_to_char ("The Stringer says 'You don't have that item'.\n\r", ch);
		return;
	}
	
	if (IS_SET (obj->extra_flags, ITEM_QUEST)
		|| IS_SET(obj->extra_flags, ITEM_TREESUNG)
		|| IS_SET (obj->extra_flags, ITEM_GUILD))
	{
		send_to_char ("Ack! You can't restring quest, treesung or guild items!\n\r", ch);
		return;
	}
	
	cost += ( UMAX(1, obj->level) * 10);
	
	if (player_silver (ch) < cost * 100)
	{
		act ("$N says 'You do not have enough gold for my services.'",
			ch, NULL, mob, TO_CHAR, POS_RESTING);
		return;
	}
	
	if (!str_prefix (arg1, "name"))
	{
		verifyName = TRUE;
		free_string (obj->name);
		obj->name = strip_codes (arg2);
	}
	else if (!str_prefix (arg1, "short"))
	{
		verifyName = TRUE;
		free_string (obj->short_descr);
		obj->short_descr = strip_codes (arg2);
	}
	else if (!str_prefix (arg1, "long"))
	{
		free_string (obj->description);
		obj->description = strip_codes (arg2);
	}
	else
	{
		send_to_char ("That's not a valid field.\n\r", ch);
		return;
	}
	
	WAIT_STATE (ch, PULSE_VIOLENCE);
	
	if (verifyName)
	{
		char *string = NULL;
		char *name = NULL;
		char buffer[MSL];
		
		memset (buffer, 0, MSL);
		
		if (!IS_NULLSTR (obj->short_descr))
			string = short_to_name (obj->short_descr);
		
		if (!IS_NULLSTR (obj->name))
		{
			name = str_dup (obj->name);
			free_string (obj->name);
		}
		
		sprintf (buffer, "%s %s", string, name);
		obj->name = str_dup (buffer);
		
		if (string)
			free_string (string);
		
		if (name)
			free_string (name);
	}
	
	amt_from_player (ch, 100 * cost);
	amt_to_player (mob, 100 * cost);
	
	act ("$N takes $n's item, tinkers with it, and returns it to $n.",
		ch, NULL, mob, TO_ROOM, POS_RESTING);
	
	act ("$N takes your item, tinkers with it, and returns $p to you.\n\r",
		ch, obj, mob, TO_CHAR, POS_RESTING);
	send_to_char
		("If we find your new string offensive (or OOC), we will not be happy.\n\r"
		"This is your ONE AND ONLY Warning.\n\r", ch);

	// BigBrother
	sprintf(arg, "$N has had obj#%d '%s' field restrung to '%s' for a cost of %d gold.",
				obj->pIndexData->vnum, arg1, arg2, 100*cost);
	wiznet(arg, ch, NULL, WIZ_RESTRING, WIZ_SECURE, get_trust(ch));
		

}

/** Function: do_donate
  * Descr   : Transfers an object from the players inventory or the room
  *         : to the donation pit.
  * Returns : n/a
  * Syntax  : donate ( what )
  * Written : ?
  * Author  : chancedj@peaknet.net
  */
_DOFUN (do_donate)
{
	OBJ_DATA *pit;
	OBJ_DATA *obj;
	ROOM_INDEX_DATA *original, *pitroom;
	char arg[MIL];
	int amount;
	
	argument = one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("Donate what?\n\r", ch);
		return;
	}
	
	if (ch->position == POS_FIGHTING)
	{
		send_to_char (" You're fighting!\n\r", ch);
		return;
	}
	
	/* not in your inventory? */
	if ((obj = get_obj_carry (ch, arg, ch)) == NULL)
	{
		send_to_char ("You do not have that!\n\r", ch);
		return;
	}
	
	if (!can_drop_obj (ch, obj) && ch->level < LEVEL_IMMORTAL)
	{
		send_to_char ("Its stuck to you.\n\r", ch);
		return;
	}
	
	/* no donating corpses */
	if ((obj->item_type == ITEM_CORPSE_NPC) ||
		(obj->item_type == ITEM_CORPSE_PC))
	{
		send_to_char ("You cannot donate that!\n\r", ch);
		return;
	}
	
	/* anything with ROT_DEATH is not worth the effort */
	if (obj->timer > 0)
	{
		send_to_char ("You cannot donate that.\n\r", ch);
		return;
	}
	
	/* make sure the room is there */
	if ((pitroom = get_room_index (ROOM_VNUM_DONATION)) == NULL)
	{
		send_to_char ("No Donation Room, please inform the staff.\n\r", ch);
		return;
	}
	
	original = ch->in_room;
	
	if (ch->in_room != pitroom)
	{
		char_from_room (ch);
		char_to_room (ch, pitroom);
	}
	if ((pit = get_obj_list (ch, "basket", ch->in_room->contents)) == NULL)
	{
		send_to_char
			("Sorry, the donation basket isnt available, inform an imm immediately.\n\r", ch);
		if (original != ch->in_room)
		{
			char_from_room (ch);
			char_to_room (ch, original);
		}
		return;
	}
	
	obj_from_char (obj);
	obj_to_obj (obj, pit);
	if (original != ch->in_room)
	{
		char_from_room (ch);
		char_to_room (ch, original);
	}
	act ("$n donates $p.", ch, obj, NULL, TO_ROOM, POS_RESTING);
	act ("You donate $p.", ch, obj, NULL, TO_CHAR, POS_RESTING);
	if (obj->cost > 0 && obj->level > 0)
	{
		amount = UMAX (1, obj->cost / 2);
		chprintf (ch, "You receive %d silver for your donation.\n\r",
			amount);
		ch->silver += amount;
	}
	save_char_obj (ch);
	return;
}
