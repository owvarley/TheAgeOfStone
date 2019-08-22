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
 * @@ Objective ()
 *
 *  Functions located within this file are here only temporarily.  Much
 *  thought is being done on where functions should be located, and until
 *  the entire re-structuring is done, new functions should be placed
 *  either within this file, or one of thier own.
 */


#if !defined(WIN32)
  #include <sys/time.h>
  #include <unistd.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"

void show_obj_values2 (CHAR_DATA * ch, OBJ_DATA * obj);
bool set_obj_values2 (CHAR_DATA * ch, OBJ_DATA * pObj, int value_num, char *argument);


struct sungwood_types
{
	char *type;
	long vnum;
	sh_int min_level;
	sh_int turn_count;  // in minutes
	int endurance_cost; // per tick
};

const struct sungwood_types sungwood_table[] = 
{
	{"treasure",		0,	15,	1,	25	},	
	{"furniture",		0,	25,	5,	100	},	
	{"armor",		0,	35,	3,	75	},	
	{"weapon",		0,	35, 	3,	75	},
	{NULL,			0,	0,	0,	0	}
};


_DOFUN(do_dream)
{
	char arg[MIL];
	int skill = get_skill(ch, skill_lookup("dream"));
	float fMod = 1.0; 

	ch = CH(ch->desc);

	memset(arg, MIL, 0);
	one_argument(argument, arg);


	if ( ch->position != POS_SLEEPING && !IS_AFFECTED(ch, _AFF_DREAMING) )
	{
		chprintf(ch, "But... your wide awake!\n\r");
		return;
	}

	if ( skill < 1 )
	{
		chprintf(ch, "You don't know how to dreamwalk.\n\r");
		return;
	}

	if ( IS_AFFECTED(ch, _AFF_DREAMING) )
	{
		if ( arg[0] != '\0' && !str_prefix(arg, "wake") )
		{
			OBJ_DATA *pObj = NULL, *pMark = NULL, *pNext = NULL;
			REMOVE_FLAG(ch->states.affected_by, _AFF_DREAMING);

			// have to remove any ghost objects the character may be holding/wearing
			for ( pObj = ch->carrying; pObj != NULL; pObj = pNext )
			{
				if ( pObj->contains )
				{
					pMark = pObj->next_content;
					pNext = pObj->contains;
				}
				else
					pNext = pObj->next_content;

				if ( pNext == NULL && pMark != NULL )
				{
					pNext = pMark;
					pMark = NULL;
				}

				if ( IS_OBJ_STAT(pObj, ITEM_GHOST) )
				{
					obj_from_char(pObj);
					act ("$p dissolves into smoke.", ch, pObj, NULL, TO_ROOM, POS_RESTING);
					act ("$p dissolves into smoke.", ch, pObj, NULL, TO_CHAR, POS_RESTING);
					extract_obj (pObj);
				}
			}
			
			ch->position = POS_SLEEPING;

			// Ok, now find and remove the ghost....
			{
				CHAR_DATA *pGhost = NULL, *pNext = NULL;

				for (pGhost = char_list; pGhost; pGhost = pNext)
				{
					pNext = pGhost->next;
					if ( pGhost->vGhostRoom == GET_ID(ch) )
						break;
				} // if pGhost == NULL.... oops...

				if ( pGhost )
				{
					int i;

					ch->hit = pGhost->hit;
					ch->move = pGhost->move;
					for (i=0; i<5; i++)
						ch->flow_str.current[i] = pGhost->flow_str.current[i];

					ch->linked_to = pGhost->linked_to;
					ch->vGhostRoom = pGhost->in_room ? pGhost->in_room->vnum : ROOM_VNUM_LOGIN;
					ch->position = pGhost->position;
					ch->fighting = pGhost->fighting;
					char_from_room(pGhost);
					extract_char(pGhost, TRUE);
					
					if ( ch->fighting )
						ch->fighting->fighting = ch;

					char_from_room( ch );
					char_to_room(ch, get_room_index(ch->vGhostRoom) );
				}
				else
					bugf("[dharvest.c::do_dream] Null Ghost for %s in room %ld.", ch->name, ch->vGhostRoom);
			}


			do_wake(ch,"");
			return;
		} 
		else
		if ( arg[0] != '\0' && !str_prefix(arg, "goto") )
		{
			argument = one_argument(argument, arg); // strip 'goto'
			do_goto(ch, argument);
			return;
		}
		else
		if ( arg[0] != '\0' && !str_prefix(arg, "send") )
		{
			argument = one_argument(argument, arg); // strip 'send'
			if ( !IS_NULLSTR(argument) )
			  do_pecho(ch, argument);
			return;
		}
		else
		if ( arg[0] != '\0' && !str_prefix(arg, "find") )
		{
			char arg1[MIL];

			memset(arg1, MIL, 0);
			
			argument = one_argument(argument, arg);  // 'find' command
			argument = one_argument(argument, arg1); //  person, place or thing

			// remaining argument is search key...
			if ( arg1[0] != '\0' && !str_prefix(arg1, "person") )
				do_mwhere(ch, argument);
			else
			if ( arg1[0] != '\0' && !str_prefix(arg1, "place") )
			{
				sprintf(arg, "room name %s", argument);
				do_vfind(ch, arg);
			}
			else
			if ( arg1[0] != '\0' && !str_prefix(arg1, "thing") )
				do_owhere(ch, argument);
			else
				chprintf(ch,"Syntax: dream find <person|place|thing> keyword\n\r");

			return;
		}
		else if ( arg[0] != '\0' )
		{
			chprintf(ch,"Syntax: dream <enter>                           -- Begin dreaming.\n\r"
						"        dream wake                              -- Leave the dreamworld behind.\n\r"
				        "        dream find <person|place|thing> keyword -- Search for something.\n\r"
						"        dream send (to who) (dream text)        -- Send a dream to a sleeper.\n\r"
						"        dream goto keyword                      -- Dreamwalk to another location.\n\r");
			return;
		}


		chprintf(ch, "You are allready in Tel'Aran Rhiod!\n\r");
		return;
	}

	if ( !IS_NPC(ch) && ch->pcdata->talents[ gtn_dreaming ] ) // have they the talent?
		fMod += (rand()%2==0) ? 1.0 : 2.0;

	if ( make_skill_roll(skill, fMod) )
	{
		CHAR_DATA *pGhost = create_mobile(get_mob_index (MOB_VNUM_DISGUISE));
		ROOM_INDEX_DATA *pRoom = ch->in_room;
		OBJ_DATA *pObj = NULL, *pClone = NULL;
		enum _WEAR_LOC i;

		clone_mobile(ch, pGhost);

		if ( !pGhost )
		{
			bugf("[dharves.c::do_dreaming] NULL Ghost for %s!", ch->name);
			chprintf(ch, "System error. Unable to create ghost.\n\r");
			return;
		}

		// Casting is not explicitly required here, but to avoid confusion with other
		// wear_location defines, I've casted this both for my sake, and the compilers...
		for ( i = (enum _WEAR_LOC)WEAR_LIGHT; i < (enum _WEAR_LOC)MAX_WEAR; i++ )
		{
			pObj = get_eq_char(ch, (int)i);
			if ( pObj )
			{
				pClone = create_object(pObj->pIndexData, pObj->level);
				clone_object(pObj, pClone);
				obj_to_char(pClone, pGhost);
				equip_char(pGhost, pClone, i);
			}
		}

		ch->vGhostRoom = ch->in_room->vnum;
		pGhost->vGhostRoom = GET_ID(ch); // so we can find this ghost again...
		
		SET_FLAG(ch->states.affected_by, _AFF_DREAMING);

		pGhost->position = POS_SLEEPING;
		char_to_room(pGhost, pRoom);
		
		ch->position = POS_STANDING;
		check_improve(ch, skill_lookup("dream"), TRUE, ch->pcdata->talents[gtn_dreaming] ? 3 : 6 );
		chprintf(ch, "You find yourself in the dreamworld.\n\r");
	}
	else // failed roll
	{
		check_improve(ch, skill_lookup("dream"), FALSE, 1 );
		chprintf(ch, "You toss and turn in your sleep, dreamless.\n\r");
	}
}

_DOFUN(do_treesing)
{
	int chance = 0, i=0;
    OBJ_DATA *qObj = (OBJ_DATA *) ch->desc->pEdit;
	int iSkill = skill_lookup("treesinging");
	char cmd[MIL], arg1[MIL];

	const char *ToCharNoAffect = "The trees cannot hear your song.\n\r";
	const char *ToRoomNoAffect = "You hear a low rumble as $n raises $s voice in song.";

    argument = one_argument (argument, cmd);

	if ( IS_NPC(ch) )
	{ 
		send_to_char("Only PCs can sing to trees.\n\r", ch);
		return;
	}

	if ( ch->race != race_lookup("ogier") )
	{
		send_to_char("uh HUH, only the Ogier can sing to the trees!\n\r", ch);
		return;
	}

	if ( ch->position == POS_FIGHTING || ch->position < POS_SITTING )
	{
		send_to_char("You seem to be in the wrong state of mind for singing.\n\r", ch);
		return;
	}

	if ( ch->in_room->sector_type != SECT_JUNGLE && 
		 ch->in_room->sector_type != SECT_FOREST &&
		 ch->in_room->sector_type != SECT_MOUNTAIN &&
		 ch->in_room->sector_type != SECT_HILLS )
	{
		send_to_char("In order for one to properly sing to the trees\n\r"
					 "It is imperative that their be a tree nearby...\n\r",ch);
		return;
	}

    if (ch->desc->editor && ch->desc->editor != ED_TREESING)
	{
      send_to_char("You are currently using an OLC Editor. Please exit first.\n\r", ch);
      return;
	}
	
	if ( ch->desc->editor == ED_NONE && !qObj )
	{
		chance = (int)get_skill(ch, iSkill) * .30;

		if ( ch->pcdata->talents[gtn_tree_singing] )
			chance+=roll_dice( (int)(ch->level*.85) );
		else 
			chance+=roll_dice( (int)(ch->level*.20) );

		// put in percentile roll
		if ( number_percent() > chance )
		{
			act(ToCharNoAffect, ch, NULL, NULL, TO_CHAR, POS_RESTING);
			act(ToRoomNoAffect, ch, NULL, NULL, TO_ROOM, POS_RESTING);

			check_improve(ch, iSkill, FALSE, 1);
			return;
		}
	}

    if (str_prefix (cmd, "type")) 
	{

	  if (!qObj)
	  {
	    send_to_char("Your voice raises in song to ... ?\n\r", ch);
	    return;
	  }

	  if ( !str_prefix(cmd, "show") )
	  {
	  
		  chprintf(ch, "Your item:\n\rName %s\n\r"
					     "Short %s\n\r"
						 "Desc  %s\n\r",
						 qObj->name, qObj->short_descr, qObj->description);
		  return;
	  }

	
	  if ( !str_prefix(cmd, "form") )
	  {
		 if ( IS_NULLSTR(qObj->name) 
		 || IS_NULLSTR(qObj->short_descr) 
		 || IS_NULLSTR(qObj->description) )
		 {
			 send_to_char("You should finish your design first.\n\r", ch);
			 return;
		 }

		 send_to_char("You concentrate on your work.\n\r", ch);
		 ch->desc->editor = ED_TREESING;
		 return;
	  }


	  if ( !str_prefix(cmd, "name") )
	  {
		char sh_n[MSL];

		free_string(qObj->name);
		sprintf(sh_n, "[Treesung item by %s] %s", ch->name, argument);
		qObj->name = str_dup(sh_n);
		send_to_char("Name sung.\n\r", ch);
		return;
	  }
	  else
	  if ( !str_prefix(cmd, "short") )
	  {
		char sh_d[MSL];

		free_string(qObj->short_descr);
		sprintf(sh_d, "(`Gsungwood`w) %s", argument);
		qObj->short_descr = str_dup(sh_d);
		send_to_char("Short description sung.\n\r", ch);
		return;
	  }
	  else
	  if ( !str_prefix(cmd, "long") || !str_prefix(cmd, "description"))
	  {
		free_string(qObj->description);
		qObj->description = str_dup(argument);
		SET_BIT(qObj->extra_flags, ITEM_TREESUNG);
		send_to_char("Description set.\n\r", ch);
		return;
	  }
	  
	}
	else
	{
		OBJ_DATA *qObj = NULL;
		int timer = 0, i, value = -1;

	    argument = one_argument (argument, arg1);

		if (ch->desc->pEdit != NULL)
		{
		  send_to_char ("But you haven't finished your song!\n\r", ch);
		  return;
		}

		for (i=0; sungwood_table[i].type != NULL; i++)
		{
			if ( !str_prefix(sungwood_table[i].type, arg1) )
			{
				timer = sungwood_table[i].turn_count;
				value = item_lookup(arg1);
			}
		}

		if ( value < 0 )
		{
			send_to_char("Invalid item type., valid types are: ",ch);
			for (i=0; sungwood_table[i].type != NULL; i++)
			  chprintf(ch, "%s ", sungwood_table[i].type);

			return;
		}

		qObj = create_object (get_obj_index (OBJ_VNUM_GENERIC), 0);

		if (!qObj)
		{
		  send_to_char ("Treesinging Error: Unable to create object!\n\r", ch);
		  logf ("Treesinging Error: Unable to create object #%d", OBJ_VNUM_GENERIC);
		  return;
		}

		qObj->item_type = (enum _ITEM_TYPE)value;
		qObj->timer = timer;

		ch->desc->pEdit = (void *)qObj;	
		send_to_char ("Your treesong begins to flow into the wood.\n\r", ch);
		return;
	}

	send_to_char("syntax: treesing [type|show|name|short|long|form]\n\rValid types are:\n\r", ch);

	for (i=0; sungwood_table[i].type != NULL; i++)
		  chprintf(ch, "%s ", sungwood_table[i].type);

	return;
}


/** Function: do_qgen
  * Descr   : Quest Item Creation code.
  * Returns : Item created, once it's finished.
  * Syntax  : see below
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_qgen)
{
	char cmd[MIL], arg1[MIL];
	
	if (IS_NPC (ch))
	{
		chprintf(ch, "Not available to NPCs.\n\r");
		return;
	}
	
	if (ch->desc->editor && ch->desc->editor != ED_QGEN)
	{
		chprintf(ch, "You are currently using an OLC Editor. Please exit first.\n\r");
		return;
	}
	
	argument = one_argument (argument, cmd);
	argument = one_argument (argument, arg1);
	
	if (cmd[0] == '\0' || !str_prefix (cmd, "syntax") || !str_prefix (cmd, "help"))
	{
		chprintf(ch, "\n\rSyntax: qGen command [arguments]\n\r");
		chprintf(ch, "        qgen start\n\r");
		chprintf(ch, "        qgen finish\n\r");
		chprintf(ch, "        qgen show\n\r");
		chprintf(ch, "        qgen points\n\r");
		chprintf(ch, "        qgen delete\n\r");
		chprintf(ch, "        qgen name [string list]\n\r");
		chprintf(ch, "        qgen short [string]\n\r");
		chprintf(ch, "        qgen long [string]\n\r");
		chprintf(ch, "        qgen material [string]\n\r");
		chprintf(ch, "        qgen type [flag]\n\r");
		chprintf(ch, "        qgen wear [flag]\n\r");
		chprintf(ch, "        qgen extra [flag]\n\r");
		chprintf(ch, "        qgen vX [value]      (where X is 0-4)\n\r");
		chprintf(ch, "        qgen level [0 - %d]\n\r", ML);
		chprintf(ch, "        qgen affect [location] [delete|#xmod]\n\r");
		chprintf(ch, "        qgen cost [value]\n\r");
		chprintf(ch, "        qgen condition [value]\n\r");
		chprintf(ch, "        qgen weight [value]\n\r");
		chprintf(ch, "        qgen ? [wear|type|extra|affect]\n\r");
		return;
	}
	
	// if the command is anything but 'start', then...
	if (str_prefix (cmd, "start"))
	{
		OBJ_DATA *qObj = (OBJ_DATA *) ch->desc->pEdit;
		
		if (!qObj)
		{
			chprintf(ch, "You have to create a qGen item before you can modify one.\n\r");
			return;
		}
		
		if (!str_prefix (cmd, "finish"))
		{
			char buf[MSL];
			long eqCost = eq_cost (qObj);
			
			if (qObj->item_type < 1
			|| IS_NULLSTR(qObj->name)
			|| IS_NULLSTR(qObj->short_descr)
			|| IS_NULLSTR(qObj->description))
			{
				chprintf(ch, "Please finish your item first.\n\r");
				return;
			}
			
			if (eqCost > ch->pcdata->quest_points)
			{
				chprintf (ch,"You have %d quest points, this object\n\r"
							 "costs %d points.  You can't afford it.\n\r",
							 ch->pcdata->quest_points, eqCost);
				return;
			}
			
			if ((qObj->level < 11 && eqCost > 6)
			|| (qObj->level < 26 && eqCost > 10)
			|| (qObj->level < 41 && eqCost > 14) || eqCost > 16)
			{
				chprintf(ch,
					"\n\rYour object costs too much.  Valid costs are:\n\r\n\r"
					"Levels   1 - 10          6 Points\n\r"
					"        11 - 25         10 Points\n\r"
					"        26 - 40         14 Points\n\r"
					"        41 and up       16 Points\n\r\n\r"
					"Please adjust your item accordingly.\n\r");

				return;
			}
			
			if (!IS_SET (qObj->extra_flags, ITEM_QUEST))
				SET_BIT (qObj->extra_flags, ITEM_QUEST);
			
			ch->pcdata->quest_points -= eq_cost (qObj);
			
			sprintf(buf, "%s [%s's qGen]", qObj->name, ch->name);
			
			free_string (qObj->name);
			qObj->name = str_dup (buf);
			
			obj_to_char(qObj, ch);
			
			ch->desc->pEdit = NULL;
			ch->desc->editor = ED_NONE;
			
			
			chprintf (ch, "%s has been transferred to your inventory and\n\r"
				"your quest points have been adjusted.\n\r",
				qObj->short_descr);
			
			return;
		}
		else
		if (!str_prefix (cmd, "?"))
		{
			show_help (ch, arg1);
			return;
		}
		else
		if (!str_prefix (cmd, "delete"))
		{
			chprintf (ch, "Item deleted!\n\r");
			extract_obj(qObj);
			ch->desc->pEdit = NULL;
			ch->desc->editor = ED_NONE;
			return;
		}
		else
		if (!str_prefix (cmd, "type"))
		{
			int value = flag_value (type_flags, arg1);
			
			if (value == NO_FLAG || arg1[0] == '\0')
			{
				chprintf(ch,"Syntax: type [flag]\n\r"
					        "Type '? type' for a list of valid flags.\n\r");
				return;
			}
			memset (qObj->value, 0, 5);
			qObj->item_type = value;
			
			if (qObj->item_type == ITEM_WAYGATE)
				qObj->value[1] = EX_ISDOOR | EX_CLOSED | EX_NOLOCK;
			
			chprintf (ch, "Type set [%s].\n\r",
				flag_string (type_flags, qObj->item_type));
			return;
		}
		else
		if (!str_prefix (cmd, "level"))
		{
			int value = atoi (arg1);
			
			if (value < 0 || value > ML)
			{
				chprintf(ch, "Invalid value. Valid range is 0 - %d\n\r", ML);
				return;
			}
			
			qObj->level = value;
			chprintf (ch, "Level set to: %d.\n\r", qObj->level);
			return;
		}
		else
		if ( cmd[0] == 'v' && cmd[1] != '\0')
		{
			int which = -1;

			if (IS_NULLSTR(arg1))
			{
				chprintf(ch, "Invalid argument.\n\r");
				return;
			}

			// for some god awful reason, VC++ is choking on using atoi on cmd[1], even
			// when the watch shows it to be '1'. In fact, casting it in the watch window
			// to a (char*)cmd[1] completely trashes it... *boggle* Thank you MS...
			which = ( cmd[1] - 48 ); // char(48) = '0'
			
			if ( which < 0 || which > 4 )
			{
				chprintf(ch, "Invalid v#");
				return;
			}

			if ( set_obj_values2(ch, qObj, which, arg1))
				chprintf(ch, "Value %d set.\n\r", which);

			return;
		}
		else
		
		if (!str_prefix (cmd, "wear_flags"))
		{
			int value;
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			value = flag_value (wear_flags, arg1);
			
			if (value == NO_FLAG || arg1[0] == '\0')
			{
				chprintf (ch, "Syntax: wear [flag]\n\r"
                              "Type '? wear' for a list of valid flags.\n\r");
				return;
			}
			
			TOGGLE_BIT (qObj->wear_flags, value);
			
			chprintf (ch, "Wear Flag(s) set to: [%s].\n\r",
                          flag_string (wear_flags, qObj->wear_flags));
			return;
		}
		else
		if (!str_prefix (cmd, "extra_flags"))
		{
			int value;
			
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			value = flag_value (extra_flags, arg1);
			
			if (value == NO_FLAG || arg1[0] == '\0')
			{
				chprintf (ch, "Syntax: extra [flag]\n\r"
                          "Type '? extra-flags' for a list of valid flags.\n\r");
				return;
			}
			
			TOGGLE_BIT (qObj->extra_flags, value);
			
			chprintf (ch, "Extra Flag(s) set to: [%s].\n\r",
				          flag_string (extra_flags, qObj->extra_flags));
			return;
		}
		else
		if (!str_prefix (cmd, "points"))
		{
			long eqCost = eq_cost (qObj);

			chprintf (ch, "The current point value of this item is: %d\n\r\n\r",
				eq_cost (qObj));
			if ((qObj->level < 11 && eqCost > 6)
			|| (qObj->level < 26 && eqCost > 10)
			|| (qObj->level < 41 && eqCost > 14) || eqCost > 16)
				chprintf(ch,
					"Your object costs too much!  ");
			chprintf (ch, "Valid costs are:\n\r"
					"Levels   1 - 10     1 -  6 Points\n\r"
					"        11 - 25     1 - 10 Points\n\r"
					"        26 - 40     1 - 14 Points\n\r"
					"        41 and up   1 - 16 Points\n\r\n\r");
			return;
		}
		else if (!str_prefix (cmd, "condition"))
		{
			int value = atoi (arg1);
			
			if (value < 0)
			{
				chprintf(ch, "Invalid value.\n\r");
				return;
			}
			
			qObj->condition = value;
			chprintf (ch, "Condition set to: %d.\n\r", value);
			return;
		}
		else if (!str_prefix (cmd, "cost"))
		{
			long value = atol (arg1);
			
			if (value < 0)
			{
				chprintf(ch, "Invalid value.\n\r");
				return;
			}
			
			qObj->cost = value;
			chprintf (ch, "Cost set to: %d.\n\r", value);
			return;
		}
		else if (!str_prefix (cmd, "weight"))
		{
			int value = atoi (arg1);
			
			if (value < 0)
			{
				chprintf(ch, "Invalid value.\n\r");
				return;
			}
			
			qObj->weight = value;
			chprintf (ch, "Weight set to: %d.\n\r", value);
			return;
		}
		else if (!str_prefix (cmd, "affects"))
		{
			int value = -1;
			AFFECT_DATA *pAf;
			char mod[MSL];
			bool bDelete = FALSE;
			
			one_argument (argument, mod);
			
			if (arg1[0] == '\0' || mod[0] == '\0' )
			{
				chprintf (ch,"Syntax:  qgen affect [location] [delete|#xmod]\n\r"
                             "Valid affects are:\n\r");
				show_help (ch, "apply");
				return;
			}
			
			if ( !str_prefix(mod, "delete") )
				bDelete = TRUE;
			else
			if ((value = flag_value (apply_flags, arg1)) == NO_FLAG)
			{
				chprintf(ch, "Valid affects are:\n\r");
				show_help (ch, "apply");
				return;
			}
			
			if ( bDelete )
			{

				value = atoi(arg1);

				if (!(pAf = qObj->affected) || value == -1)
				{
				  chprintf(ch, "Non-existant affect.\n\r");
				  return;
				}

				if ( value == 0 ) 
				{
					pAf = qObj->affected;
					qObj->affected = pAf->next;
					free_affect(pAf);
				}
				else
				{
				   int cnt = 0;
				   AFFECT_DATA *pAf_next;

				   while ((pAf_next = pAf->next) && (++cnt < value))
					  pAf = pAf_next;

				   if (pAf_next) 
				   {
				      pAf->next = pAf_next->next;
					  free_affect (pAf_next);
				   }
				   else	
				   {
					  chprintf(ch, "No such affect.\n\r");
					  return;
				   }
 
				}

				chprintf(ch, "Affect deleted.\n\r");
				return;
			}

			pAf = new_affect ();
			pAf->location = value;
			pAf->modifier = atoi (mod);
			pAf->where = TO_OBJECT;
			pAf->type = -1;
			pAf->duration = -1;
			pAf->bitvector = 0;
			pAf->level = qObj->level;
			pAf->next = qObj->affected;
			qObj->affected = pAf;
			
			chprintf(ch, "Affect added.\n\r");
			return;
		}
		else
		if (!str_prefix (cmd, "name"))
		{
			char buf[MSL / 2];
			
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			if (qObj->name && qObj->name[0] != '\0')
				free_string (qObj->name);
			
			memset (buf, 0, MSL);
			strcat (buf, arg1);
			qObj->name = str_dup (buf);
			
			chprintf (ch, "Name set to: %s", qObj->name);
			
			
			return;
		}
		else
		if (!str_prefix (cmd, "short"))
		{
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			if (qObj->short_descr && qObj->short_descr[0] != '\0')
				free_string (qObj->short_descr);
			
			qObj->short_descr = str_dup (arg1);
			
			chprintf (ch, "Short Description set to: \n\r%s",
				qObj->short_descr);
			return;
		}
		else
		if (!str_prefix (cmd, "long"))
		{
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			if (qObj->description && qObj->description[0] != '\0')
				free_string (qObj->description);
			
			qObj->description = str_dup (arg1);
			
			chprintf (ch, "Long Description set to: %s", qObj->description);
			return;
		}
		else
		if (!str_prefix (cmd, "material"))
		{
			strcat (arg1, " ");
			strcat (arg1, argument);
			
			if (qObj->material && qObj->material[0] != '\0')
				free_string (qObj->material);
			
			qObj->material = str_dup (arg1);
			
			chprintf (ch, "Material set to: %s", qObj->material);
			return;
		}
		else
		if (!str_prefix (cmd, "show"))
		{
			int cnt = -1;
			AFFECT_DATA *paf;
			
			
			chprintf (ch, "\n\r\n\rLevel [%2d]  Name   : %s\n\r",
				qObj->level, qObj->name);
			
			chprintf (ch, "Short description  : %s\n\r"
				"Long description   : %s\n\r\n\r",
				qObj->short_descr, qObj->description);
			
			if (qObj->extra_descr)
			{
				EXTRA_DESCR_DATA *ed;
				
				chprintf(ch, "Ex desc kwd        : ");
				
				for (ed = qObj->extra_descr; ed; ed = ed->next)
				{
					chprintf(ch, "[");
					chprintf(ch, ed->keyword);
					chprintf(ch, "]");
				}
				
				chprintf(ch, "\n\r");
			}
			
			if (qObj->pIndexData->extra_descr)
			{
				EXTRA_DESCR_DATA *ed;
				
				chprintf(ch, "Ex desc kwd2       : ");
				
				for (ed = qObj->pIndexData->extra_descr; ed; ed = ed->next)
				{
					chprintf(ch, "[");
					chprintf(ch, ed->keyword);
					chprintf(ch, "]");
				}
				
				chprintf(ch, "\n\r");
			}
			
			chprintf (ch,
				"Vnum               : [%ld]  Type: %s  Resets: %d  Points: %d\n\r",
				qObj->pIndexData->vnum,
				item_name (qObj->item_type),
				qObj->pIndexData->reset_num, eq_cost (qObj));
			
			chprintf (ch,
				"Wear flags         : [%s]\n\rExtra flags        : [%s]\n\r",
				flag_string(wear_flags, qObj->wear_flags),
				flag_string(extra_flags, qObj->extra_flags));
			
			chprintf (ch,
				"Number             : [1/%d]  Weight: [%d/%d/%d]\n\r",
				get_obj_number (qObj),
				qObj->weight,
				get_obj_weight (qObj), get_true_weight (qObj));
			
			chprintf (ch,
				"Cost               : [%d]  Condition: [%d]  Timer: [%d]\n\r",
				qObj->cost, qObj->condition, qObj->timer);
			
			chprintf (ch,
				"In room            : [%ld]  In Object : %s  Carried by : (%s)\n\r\n\r",
				qObj->in_room == NULL ? 0 : qObj->in_room->vnum,
				qObj->in_obj ==
				NULL ? "(none)" : qObj->in_obj->short_descr,
				qObj->carried_by == NULL ? "(none)" : can_see (ch,
				qObj->
				carried_by)
				? qObj->carried_by->name : "someone");
			
			show_obj_values2 (ch, qObj);
			
			for (cnt = 0, paf = qObj->affected; paf; paf = paf->next)
			{
				if (cnt == 0)
				{
					chprintf (ch,"\n\rNumber Modifier Affects\n\r"
                                 "------ -------- -------\n\r");
				}
				chprintf (ch, "[%4d] %-8d %s\n\r",
					cnt,
					paf->modifier,
					flag_string (apply_flags, paf->location));
				
				cnt++;
			}
			
			if (!qObj->enchanted)
			{
				for (paf = qObj->pIndexData->affected; paf; paf = paf->next)
				{
					if (cnt == 0)
					{
						chprintf(ch, "\n\rNumber Modifier Affects\n\r"
                                     "------ -------- -------\n\r");
					}
					chprintf (ch, "[%4d] %-8d %s\n\r",
						cnt,
						paf->modifier,
						flag_string (apply_flags, paf->location));
					
					cnt++;
				}
			}
			return;
    }
	
  }
	// Ok, they want to start a new item...
    else
	{
	  OBJ_DATA *qObj = NULL;
	  
	  if (ch->desc->pEdit != NULL)
	  {
		  chprintf(ch, "Finish your current item first!\n\r");
		  return;
	  }
	  
	  qObj = create_object (get_obj_index (OBJ_VNUM_GENERIC), 0);
	  
	  if (!qObj)
	  {
		  chprintf(ch, "qGen Error: Unable to create object!\n\r");
		  logf ("qGen Error: Unable to create object #%d", OBJ_VNUM_GENERIC);
		  return;
	  }
	  
	  // qGen has officially begun!
	  ch->desc->editor = ED_QGEN;
	  ch->desc->pEdit = qObj;	
	  chprintf(ch, "Your qGen process has begun.\n\r");
	  return;
	}
}


_DOFUN (do_fix_weaves)
{
}


int pobj_eq_cost(OBJ_INDEX_DATA *pObjIdx)
{
  OBJ_DATA *pObj;
  int cost = -1;

  if ( !pObjIdx )
    return cost;

  pObj = create_object( pObjIdx, pObjIdx->level <= 0 ? ML/2 : pObjIdx->level );

  if ( pObj )
  {
    cost = eq_cost( pObj );    
    extract_obj( pObj );
  }

  return cost;
}

/** Function: eq_cost
 * Descr   : Determines the number of 'points' that an object is valued at
 * Returns : object points
 * Syntax  : eq_cost( object )
 * Written : v1.0 9/98
 * Author  : Gary McNickle <gary@tarmongaidon.org>
 */
int eq_cost (OBJ_DATA * pObj)
{
	AFFECT_DATA *pAf;
	int cost = 0;
	
	if (pObj == NULL)
		return 0;
	
	/* Points are irrelevant of object type 
	 * Costs: 
	 *   ITEM_LIGHT:
	 *           1ep / 200hours (max 4ep)
	 *           5ep / -1 (infinite)
	 *   ITEM_ARMOR
	 *           ep = v0+v1+v2+v3/8 + (v1*v2)
	 *   ITEM_WEAPON
	 *           ep = v1 * v2 / 2 (commented out below)
	 *           5ep / type2_flag
	 *   ITEM_ANGREAL
	 *           weak            = 4
	 *           strong          = 8
	 *           powerful        = 12
	 *           angreal         = 4
	 *           sa'angreal      = 8
	 *   flags
	 *           burnproof       = 2ep
	 *           Neverbreak      = 3ep
	 *           Visdeath/Bless  = 2ep
	 *           Glow/Hum        = 1ep   
	 *           Dark/AntiAlign  = 0ep
	 *           Evil            = -2ep
	 *   applies
	 *           str/dex/etc     = 1ep/per pt.
	 *           -2ac            = 1ep
	 *           10endurance     = 1ep
	 *           5hp             = 1ep
	 *           -1save          = 2ep
	 */
	
	if (!pObj->enchanted)
		for (pAf = pObj->pIndexData->affected; pAf; pAf = pAf->next)
		{
			switch (pAf->location)
			{
				case APPLY_LEVEL:
					cost += pAf->modifier * 5;
					break;
				case APPLY_ABILITY:
					cost += pAf->modifier * 30;
					break;
				case APPLY_HITROLL:
				case APPLY_DAMROLL:
					cost += pAf->modifier * 2;
					break;
				case APPLY_HIT:
					if (pAf->modifier != 0)
						cost += (int) pAf->modifier / 5;
					break;
				case APPLY_ENDURANCE:
					if (pAf->modifier != 0)
						cost += (int) pAf->modifier / 10;
					break;
				case APPLY_STR:
				case APPLY_DEX:
				case APPLY_WIS:
				case APPLY_CON:
				case APPLY_INT:
					(pAf->modifier > 0) ? cost += pAf->modifier : 0;
					break;
				case APPLY_AC:
					if (pAf->modifier < 0)
						cost += pAf->modifier / -2;
					else if (pAf->modifier > 0)
						cost -= pAf->modifier / 2;
					break;
					
				default:
					break;
			}
		}
		
		for (pAf = pObj->affected; pAf; pAf = pAf->next)
		{
			switch (pAf->location)
			{
				case APPLY_LEVEL:
					cost += pAf->modifier * 5;
					break;
				case APPLY_ABILITY:
					cost += pAf->modifier * 3;
					break;
				case APPLY_HITROLL:
				case APPLY_DAMROLL:
					cost += pAf->modifier * 2;
					break;
				case APPLY_HIT:
					if (pAf->modifier != 0)
						cost += (int) pAf->modifier / 5;
					break;
				case APPLY_ENDURANCE:
					if (pAf->modifier != 0)
						cost += (int) pAf->modifier / 10;
					break;
				case APPLY_STR:
				case APPLY_DEX:
				case APPLY_WIS:
				case APPLY_CON:
				case APPLY_INT:
					(pAf->modifier > 0) ? cost += pAf->modifier : 0;
					break;
				case APPLY_AC:
					if (pAf->modifier < 0)
						cost += pAf->modifier / -2;
					else if (pAf->modifier > 0)
						cost -= pAf->modifier / 2;
					break;
					
				default:
					break;
			}
		}
		
		switch (pObj->item_type)
		{
			case ITEM_POTION:
			case ITEM_HERB:
				if (pObj->value[0] > 0)
					cost += pObj->value[0] / 5;
				if (pObj->value[1] >= 0)
					cost += 3;
				if (pObj->value[2] >= 0)
					cost += 3;
				if (pObj->value[3] >= 0)
					cost += 3;
				break;
				
			case ITEM_FURNITURE:
				if (pObj->value[3] > 100)
					cost += UMAX (2, pObj->value[3] / 100);
				if (pObj->value[4] > 100)
					cost += UMAX (2, pObj->value[4] / 100);
				break;
				
			case ITEM_LIGHT:
				if (pObj->value[0] == -1 || pObj->value[0] >= 800)
					cost += 5;
				else if (pObj->value[0] > 0)
					cost += (int) pObj->value[0] / 200;
				break;
				
			case ITEM_WEAPON:
				if (pObj->value[4] != 0)
				{
					int i;
					
					for (i = 0; weapon_type2[i].name != NULL; i++)
						if (IS_SET (pObj->value[4], weapon_type2[i].bit))
							cost += 5;
				}
				
				/* adjust for average damage 
				if ( pObj->value[1] != 0 && pObj->value[2] != 0 )
				cost += pObj->value[1] * pObj->value[2] / 2;
				break;
				*/
				
			case ITEM_ARMOR:
				cost += (pObj->value[0] + pObj->value[1] +
					pObj->value[2] + pObj->value[3]) / 8;
				break;

			case ITEM_ANGREAL:
				if (pObj->value[0] < 1)
					pObj->value[0] = 1;
				if (pObj->value[0] > 2)
					pObj->value[0] = 2;
				cost += pObj->value[0] * 4;
				if (pObj->value[1] > 0)
					cost += pObj->value[1] * 4;
				break;
				
			default:
				break;
		}
		
		if (IS_SET (pObj->extra_flags, ITEM_GLOW))
			cost += 1;
		if (IS_SET (pObj->extra_flags, ITEM_INVIS))
			cost += 2;
		if (IS_SET (pObj->extra_flags, ITEM_NODROP))
			cost += 1;
		if (IS_SET (pObj->extra_flags, ITEM_BLESS))
			cost += 2;
		if (IS_SET (pObj->extra_flags, ITEM_NOREMOVE))
			cost += 1;
		if (IS_SET (pObj->extra_flags, ITEM_VIS_DEATH))
			cost += 2;
		if (IS_SET (pObj->extra_flags, ITEM_BURN_PROOF))
			cost += 2;
		if (IS_SET (pObj->extra_flags, ITEM_NOBREAK))
			cost += 3;
		
		return cost<1 ? 1 : cost;
}

/** Function: version_note
  * Descr   : Writes a note to the logfile as a reminder to the coder
  *         : to take care of something. checks version number and exits
  *         : if 'critical' is set.
  * Returns : (void)
  * Syntax  : version_note( applicable version, exit if true, note )
  * Written : v1.0 8/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void version_note (long version, bool critical, char *note)
{
  if (VERSION >= version)
  {
    logf (note);

    if (critical)
      _quit (1);
  }
}

void show_invlist_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShowNothing)
{
  char buf[MSL];
  BUFFER *output;
  char **prgpstrShow;
  int *prgnShow;
  char *pstrShow;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  OBJ_DATA *mark = NULL;
  int nShow;
  int iShow;
  int count = 0;
  bool fCombine;

  if (ch->desc == NULL)
    return;

  /*
   * Alloc space for output lines.
   */
  output = new_buf ();

  for (obj = list; obj != NULL; obj = obj->next_content)
    count++;

  prgpstrShow = alloc_mem (count * sizeof (char *));
  prgnShow = alloc_mem (count * sizeof (int));
  nShow = 0;

  /*
   * Format the list of objects.
   */
  send_to_char ("\n\r"
		"[###] [ Vnum ] [Points] Description                    Where\n\r"
		"-----------------------------------------------------------------"
		"-\n\r", ch);

  for (obj = list; obj != NULL; obj = obj_next)
  {
    if (obj->contains)
    {
      mark = obj->next_content;
      obj_next = obj->contains;
    }
    else
      obj_next = obj->next_content;

    if (obj_next == NULL && mark != NULL)
    {
      obj_next = mark;
      mark = NULL;
    }


    sprintf (buf, "%6d     %4d  %-30.30s %-25.25s",
	     obj->pIndexData->vnum,
	     eq_cost (obj),
	     strip_codes (obj->short_descr),
	     (obj->wear_loc != WEAR_NONE) ?
	     flag_string (wear_loc_strings, obj->wear_loc) :
	     (obj->in_obj != NULL) ?
	     strip_codes (obj->in_obj->short_descr) : "(inventory)");


    pstrShow = str_dup (buf);

    fCombine = FALSE;

    if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
    {
      /*
       * Look for duplicates, case sensitive.
       * Matches tend to be near end so run loop backwords.
       */
      for (iShow = nShow - 1; iShow >= 0; iShow--)
      {
	if (!strcmp (prgpstrShow[iShow], pstrShow))
	{
	  prgnShow[iShow]++;
	  fCombine = TRUE;
	  break;
	}
      }
    }

    /*
     * Couldn't combine, or didn't want to.
     */
    if (!fCombine)
    {
      prgpstrShow[nShow] = str_dup (pstrShow);
      prgnShow[nShow] = 1;
      nShow++;
    }

  }				/* end: for obj list */

  /*
   * Output the formatted list.
   */
  for (iShow = 0; iShow < nShow; iShow++)
  {
    if (prgpstrShow[iShow][0] == '\0')
    {
      free_string (prgpstrShow[iShow]);
      continue;
    }

    if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
    {
      sprintf (buf, "[%3d]  ", prgnShow[iShow]);
      add_buf (output, buf);
    }

    add_buf (output, prgpstrShow[iShow]);
    add_buf (output, "\n\r");
    free_string (prgpstrShow[iShow]);
  }

  if (fShowNothing && nShow == 0)
  {
    if (IS_NPC (ch) || IS_FLAG (ch->states.config, _COMBINE))
      send_to_char ("     ", ch);
    send_to_char ("Nothing.\n\r", ch);
  }
  page_to_char (buf_string (output), ch);

  /*
   * Clean up.
   */
  free_buf (output);
  free_mem (prgpstrShow, count * sizeof (char *));
  free_mem (prgnShow, count * sizeof (int));

  return;
}


/** Function: do_invlist
  * Descr   : Lists complete inventory of a player.
  * Returns : Listing of eq, inv and containers.
  * Syntax  : invlist ( who )
  * Written : v1.0 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_invlist)
{
  CHAR_DATA *victim;

  /* be sure to limit this to immortals via interp.c */

  if ((victim = get_char_world (ch, argument)) == NULL)
  {
    send_to_char ("They aren't playing.\n\r", ch);
    return;
  }

  show_invlist_to_char (victim->carrying, ch, FALSE);
  return;
}

/** Function: do_unlink_char
  * Descr   : Deletes a player file.
  * Returns : Result
  * Syntax  : delchar (who)
  * Written : v1.0 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_unlink_char)
{
  char strsave[MIL];
  CHAR_DATA *victim;


  if ((victim = get_char_world (ch, argument)) == NULL)
  {
    send_to_char ("They aren't playing.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL (ch))	/* use interp.c to set level to use command */
  {
    logf ("%s attempted to delete %s\n\r", ch->name, victim->name);
    send_to_char ("That was a mistake.\n\r", ch);
    return;
  }

  if (IS_NPC (ch))
  {
    send_to_char ("You can't delete NPCs!\n\r", ch);
    return;
  }

  if ((ch->level < victim->level))
  {
    send_to_char ("I think that would be a mistake.\n\r", ch);
    chprintf (victim, "%s just tried to delete you!\n\r", ch->name);
    return;
  }

  if (ch == victim)
  {
    send_to_char ("Use \"delete\" to delete yourself!\n\r", ch);
    return;
  }

  sprintf (strsave, "%s%c/%s",
	   PLAYER_DIR,
	   UPPER (victim->name[0]),
	   capitalize (fix_name (victim->name, FALSE)));

  stop_fighting (victim, TRUE);
  chprintf (ch, "You have deleted %s.\n\r", victim->name);

  act ("$n has just deleted $N!", ch, NULL, victim, TO_ROOM, POS_RESTING);
  send_to_char ("You are being deleted.\n\r", victim);

  pMud.unlinks++;
  do_quit (victim, "");
  unlink (strsave);

  return;
}


/** Function: do_guild_list
  * Descr   : Display a listing of all non-immortal flagged guilds.
  * Returns : Listing only
  * Syntax  : (N/A)
  * Written : v1.0 5/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_guild_list)
{
  int i;
  int count = 0;

  for (i = 0; i < maxClan; i++)
  {
    if (!IS_IMMORTAL (ch))
    {
      if (!IS_FLAG (clan_table[i].flags, _IMMORTAL) &&
	  clan_table[i].name != NULL && clan_table[i].name[0] != '\0')
      {
	count++;
	chprintf (ch, "[%2d] %s\n\r", count, clan_table[i].who_name);
      }
    }
    else
    {
      if (clan_table[i].name != NULL && clan_table[i].name[0] != '\0')
      {
	count++;
	chprintf (ch, "[%2d] %s\n\r", count, clan_table[i].who_name);
      }
    }
  }
  return;
}

/** Function: last_saved_on
  * Descr   : Checks the time/data a player file was last saved
  *         : Handy also for determining last logoff of a player  
  *         : Used by the "when" command.
  * Returns : Time in string format, as output only. return=void 
  * Syntax  : last_saved_on (who?)
  * Written : v1.0 4/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN(last_saved_on)
{
  FILE *fp;
  time_t date;
  char buf[MSL];

  if (*argument == '\0')
  {
    send_to_char ("You must include a player file to check.\n\r", ch);
    return;
  }

  sprintf (buf, "%s%c/%s",
	   PLAYER_DIR,
	   UPPER (argument[0]), capitalize (fix_name (argument, FALSE)));

  if ((fp = fopen (buf, "r")) != NULL)
  {
    char *string;

    for (;;)
    {
      string = fread_word (fp);
      if (!str_cmp (string, "END"))
	continue;
      else if (!str_cmp (string, "LogO"))
      {
	date = fread_number (fp);
	fclose (fp);
	chprintf (ch, "%s was last here on: %s\n\r",
			argument, (char *) ctime (&date));
	return;
      }
    }

  }
  send_to_char ("Error opening player file.\n\r", ch);
  return;
}

/** Function: do_msgs
  * Descr   : toggles certain mud messages on/off. including hunger/thirst
  *           and weather msgs.
  * Returns : status
  * Syntax  : msgs <msg>
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_msgs)
{
  char arg1[30];

  argument = one_argument (argument, arg1);

  if (!str_cmp (arg1, "hunger"))	/* toggle hunger msgs on/off */
  {
    if (IS_FLAG (ch->states.config, _NOHUNGER))
    {
      send_to_char
	("You begin listening to your body's need for sustenance.\n\r", ch);
      REMOVE_FLAG (ch->states.config, _NOHUNGER);
    }
    else
    {
      send_to_char
	("You start to ignore your body's need for sustenance.\n\r", ch);
      SET_FLAG (ch->states.config, _NOHUNGER);
    }
    return;
  }

  if (!str_cmp (arg1, "weather"))	/* toggle weather msgs on/off */
  {
    if (IS_FLAG (ch->states.config, _NOWEATHER))
    {
      send_to_char ("You begin to pay attention to the weather.\n\r", ch);
      REMOVE_FLAG (ch->states.config, _NOWEATHER);
    }
    else
    {
      send_to_char
	("You care not at all whether or not it rains or shines.\n\r", ch);
      SET_FLAG (ch->states.config, _NOWEATHER);
    }
    return;
  }

  send_to_char ("Syntax: msgs [hunger|weather]\n\r\n\r", ch);
  if (IS_FLAG (ch->states.config, _NOHUNGER))
    send_to_char
      ("You are not listening to your bodies needs for sustenance.\n\r", ch);
  else
    send_to_char
      ("You are listening to your bodies needs for sustenance.\n\r", ch);
  if (IS_FLAG (ch->states.config, _NOWEATHER))
    send_to_char ("You care not a whit weather it rains or shines.\n\r", ch);
  else
    send_to_char ("You are attuned to the weather of the region.\n\r", ch);

  return;
}

/** Function: system_msg
  * Descr   : Displays a mud wide message to players who are not marked
  *         : as QUIET and who's game state is CON_PLAYING
  * Returns : (void)
  * Syntax  : system_msg (msg)
  * Written : v1.0 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void system_msg (char *msg)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *to;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    to = d->original ? d->original : d->character;

    if (d->connected == CON_PLAYING && !IS_FLAG (to->states.config, _QUIET))
      send_to_char (msg, to);
  }

}				/* end: system_msg */


/** Function: do_checkname
  * Descr   : determines if a given name exists in the player directory
  * Returns : TRUE/FALSE
  * Syntax  : checkname (name)
  * Written : v1.0 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_checkname)
{
  FILE *fp;
  char buf[MSL];
  bool found = FALSE;

  if (argument[0] == '\0')
  {
    send_to_char ("Syntax: checkname <who>\n\r", ch);
    return;
  }

  sprintf (buf, "%s%c/%s",
	   PLAYER_DIR,
	   UPPER (argument[0]), capitalize (fix_name (argument, FALSE)));

  if ((fp = fopen (buf, "r")) != NULL)
  {
    fclose (fp);
    found = TRUE;
  }

  if (found)
    chprintf (ch, "The name %s has been taken.\n\r", argument);
  else
  {
    if (name_is_banned (argument))
      chprintf (ch, "%s has been banned.\n\r", argument);
    else
      chprintf (ch, "%s is not a name listed in the player files.\n\r",
		      argument);
  }
}



/** Function: do_petname
  * Descr   : renames pets, also changes short/long and description.
  * Returns : (void)
  * Syntax  : petname [name|short|long|desc] [argument]
  *         : note: only "desc" option does not require an argument.
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_petname)
{
  char buf[MSL];
  char command[MIL], arg2[MIL];

  /* smash all tilde's right away. */
  smash_tilde (argument);

  if (ch->pet == NULL)
  {
    send_to_char ("You dont have a pet!\n\r", ch);
    return;
  }

  if (ch->in_room != ch->pet->in_room)
  {
    send_to_char ("Kinda hard for your pet to learn his new name\n\r", ch);
    send_to_char ("if he's not even with you! *boggle*\n\r", ch);
    return;
  }

  argument = one_argument (argument, command);


  if (command[0] == '\0' || argument[0] == '\0')
  {
    send_to_char ("\n\rsyntax: petname [name|short|long|desc] <argument>\n\r",
		  ch);
    send_to_char ("\n\r  example: \"petname name fido\n\r", ch);
    send_to_char ("            \"petname short is hungry!\"\n\r", ch);
    send_to_char ("\n\rTry using color in the descriptions!\n\r\n\r", ch);
    send_to_char
      ("See \"help petname\" and \"help color\" for more information.\n\r",
       ch);
    return;
  }				/* syntax */

  if (!str_prefix (command, "name"))
  {
    FILE *fp;
    char name[MSL];

    if (argument[0] == ANSI_KEY)	/* ack! */
    {
      argument++;
      argument++;
      /* on WoT, our color codes are all one character long, so
         we only have to delete two characters here, the symbol
         that tells us "convert this to color" and the color code
         itself.  you may have to modify this to suit your mud */
    }

    argument = one_argument (argument, arg2);

    sprintf (name, "%s%c/%s",
	     PLAYER_DIR,
	     UPPER (arg2[0]), capitalize (fix_name (arg2, FALSE)));
    if ((fp = fopen (name, "r")) != NULL)
    {
      send_to_char ("That name is already taken!\n\r", ch);
      fclose (fp);
      return;
    }

    free_string (ch->pet->name);
    ch->pet->name = str_dup (capitalize (arg2));
    sprintf (buf, "Your pet has been renamed to: %s\n\r", ch->pet->name);
    send_to_char (buf, ch);

  }				/* name */
  else
   if (!str_prefix (command, "short"))
  {
    if (argument[0] == '\0')
      return;
    free_string (ch->pet->short_descr);

    /* short description should not have an "\n\r" appended to it. */
    ch->pet->short_descr = str_dup (argument);

    sprintf (buf, "%s's short description set to: \n\r%s\n\r",
	     ch->pet->name, ch->pet->short_descr);

    send_to_char (buf, ch);
  }				/* short */
  else
   if (!str_prefix (command, "long"))
  {
    if (argument[0] == '\0')
      return;
    free_string (ch->pet->long_descr);

    /* long descr needs "\n\r" appended to it. */
    sprintf (buf, "%s\n\r", argument);
    ch->pet->long_descr = str_dup (buf);

    sprintf (buf, "%s's long description set to: \n\r%s\n\r",
	     ch->pet->name, ch->pet->long_descr);
    send_to_char (buf, ch);

  }				/* long */
  else
   if (!str_prefix (command, "description"))
  {
    if (argument[0] == '\0')
    {
      string_append (ch, &ch->pet->description);
      return;
    }
  }				/* desc */
  else
    do_help (ch, "petname");

  return;

}				/* end of petname */



/** Function: do_chown
  * Descr   : allows an immortal to remove an object from a players
  *         : inventory. including objects kept in containers
  * Returns : (void)
  * Syntax  : chown (obj name) (from who)
  * Written : v1.1 2/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_chown)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char arg1[MIL];
  char arg2[MIL];

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if (arg1 == NULL || arg2 == NULL)
  {
    send_to_char ("Syntax: chown <obj> <who>\n\r", ch);
    return;
  }

  if ((victim = get_char_world (ch, arg2)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  if (((obj = get_obj_carry (victim, arg1, ch)) == NULL) &&
      ((obj = get_obj_wear (victim, arg1)) == NULL))
  {
    OBJ_DATA *list;

    for (list = victim->carrying; list != NULL; list = list->next_content)
    {
      if ((obj = get_obj_list (victim, arg1, list->contains)) != NULL)
      {
	obj_from_obj (obj);
	break;
      };
    }
  }

  if (obj == NULL)
  {
    send_to_char ("They do not posses such an item.\n\r", ch);
    return;
  }

  obj_from_char (obj);
  obj_to_char (obj, ch);
  act ("You pocket $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
  return;

}




/** Function: do_rental
  * Descr   : allows a player to rent a room or box that has been flagged 
  *         : for_rent. boxes allows storage of equipment
  * Returns : (void)
  * Syntax  : rent [room|box]
  * Written : v1.1 1/98
  * Author  : Ryouga, last updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_rental)
{
  char arg[30];

  argument = one_argument (argument, arg);

  if (!str_cmp (arg, "box"))
  {
    do_rentbox (ch, argument);
    return;
  }
  else
   if (!str_cmp (arg, "room"))
  {
    send_to_char ("Syntax: rent (room or box)\n\r", ch);
    return;
  }

  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->in_room->room_flags, ROOM_RENTAL))
  {

    if (IS_FLAG (ch->states.player, _ACT_TENNANT))
    {
      send_to_char ("You haven't checked out from last time, yet!\n\r", ch);
      return;
    }

    if (ch->gold < 30)
    {
      send_to_char
	("Hrmm... no gold eh? Maybe you should try the stables...\n\r", ch);
      return;
    }

    REMOVE_BIT (ch->in_room->room_flags, ROOM_RENTAL);
    SET_BIT (ch->in_room->room_flags, ROOM_PRIVATE);
    SET_BIT (ch->in_room->room_flags, ROOM_RENTED);
    SET_FLAG (ch->states.player, _ACT_TENNANT);

    ch->gold -= 30;
    ch->in_room->heal_rate = ch->in_room->heal_rate * 2;
    ch->in_room->opu_rate = ch->in_room->opu_rate * 2;
    send_to_char ("You rent a private room.\n\r", ch);
    ch->pcdata->rentroom = ch->in_room->vnum;
    return;
  }

  else
  {
    send_to_char ("That room has been rented.\n\r", ch);
    return;
  }
}				/* end do_rental */




/** Function: do_checkout
  * Descr   : checks a player out of a previously rented room
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.1 1/98
  * Author  : Ryouga, last updated by Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_checkout)
{

  if (IS_NPC (ch))
    return;

  if (IS_SET (ch->in_room->room_flags, ROOM_RENTED))
  {

    if (IS_FLAG (ch->states.player, _ACT_TENNANT)
	&& ch->pcdata->rentroom == ch->in_room->vnum)
    {
      send_to_char ("You check out of the room.\n\r", ch);
      SET_BIT (ch->in_room->room_flags, ROOM_RENTAL);
      REMOVE_FLAG (ch->states.player, _ACT_TENNANT);
      REMOVE_BIT (ch->in_room->room_flags, ROOM_RENTED);
      REMOVE_BIT (ch->in_room->room_flags, ROOM_PRIVATE);
      ch->in_room->heal_rate = ch->in_room->heal_rate / 2;
      ch->in_room->opu_rate = ch->in_room->opu_rate / 2;
      ch->pcdata->rentroom = 0;
    }
  }
  else
  {
    send_to_char ("This is not your room.\n\r", ch);
    return;
  }
}				/* end do_checkout */




/** Function: do_setwolf
  * Descr   : set's the flag "_ACT_WOLFBROTHER" to a players "aff" bits.
  * Returns : (void)
  * Syntax  : setwolf (who)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_setwolf)
{
  CHAR_DATA *victim;

  if (argument[0] == '\0')
  {
    send_to_char ("\n\rSyntax: setwolf <player>\n\r", ch);
    return;
  }

  if ((victim = get_char_world (ch, argument)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  if (IS_FLAG (victim->states.player, _ACT_WOLFBROTHER))
  {
    REMOVE_FLAG (victim->states.player, _ACT_WOLFBROTHER);
    send_to_char ("Your victim is no longer a wolfbrother.\n\r", ch);
    send_to_char
      ("Suddenly, the tickling in the back of your head goes away...\n\r",
       victim);
    return;
  }

  SET_FLAG (victim->states.player, _ACT_WOLFBROTHER);
  send_to_char ("They can now talk to the Wolves.\n\r", ch);
  send_to_char ("You feel a sense of wolves, off in the distance.\n\r",
		victim);
  return;
}				/* end setwolf */




/** Function: do_pflags
  * Descr   : displays the _ACT_WOLFBROTHER and Insanity status of all
  *         : logged in players.
  * Returns : (void|display only)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_pflags)
{
  DESCRIPTOR_DATA *d;
  bool Named = FALSE;

  send_to_char ("\n\r`cPlayers:           `bFlags:\n\r", ch);
  send_to_char ("`b==========================`w\n\r", ch);

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->character == NULL || !can_see (ch, d->character) || d->original != NULL)
      continue;

    Named = FALSE;

    if (d->character->pcdata->insanity)
    {
      chprintf (ch, "`c%-20.20s %02d%% Insane`w\n\r",
		      d->character->name, d->character->pcdata->insanity);
      Named = TRUE;
    }

    if (IS_FLAG (d->character->states.player, _ACT_WOLFBROTHER))
    {
      chprintf (ch, "`c%-20.20s `yWolf`w\n\r",
		      Named ? "" : d->character->name);
      Named = TRUE;
    }

    if (IS_FLAG (d->character->states.player, _ACT_BLADEMASTER))
    {
      chprintf (ch, "`c%-20.20s `BBlademaster`w\n\r",
		      Named ? "" : d->character->name);
      Named = TRUE;
    }
  }

}				/* end pflags */



/** Function: random_insanity
  * Descr   : Checks players insanity status, and randomly performs
  *         : some feat, according to how insane they are.
  * Returns : (void|action only)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void random_insanity (CHAR_DATA * ch)
{
  int number = 0;

  if (IS_NPC (ch) || ch->pcdata->insanity == 0 || IS_IMMORTAL (ch))
    return;


  if (IS_FLAG (ch->states.player, _ACT_WOLFBROTHER))
  {
    if (ch->pcdata->insanity > 95)
    {
      act ("Lost in the wolf-dream, you leave your humanity behind forever!",
	   ch, NULL, NULL, TO_CHAR, POS_DEAD);
      act("$n drops down on all four limbs, snarling as $s runs away, never to be seen again.",
 		ch, NULL, NULL, TO_NOTVICT, POS_RESTING);
      raw_kill (ch, TRUE, "Became the wolf.");
      return;
    }

    if (ch->pcdata->insanity < 25)
    {
      char toChar[MSL], toRoom[MSL];

      switch (number_range (0, 2))
      {
      case 0:
	sprintf (toChar, "%s",
		 "The taste of hot blood overcomes you for a moment,$!"
		 "the memory of an earlier age, returning.");
	sprintf (toRoom, "%s",
		 "$n's eyes flash a brilliant yellow for just an instant.");
	break;
      case 1:
	sprintf (toChar, "%s",
		 "Your senses flood with a sudden wealth of detail of your surroundings.");
	sprintf (toRoom, "%s",
		 "$n's eyes scan the room, $s nose sniffing the air.");
	break;
      case 2:
	sprintf (toChar, "%s", "A throaty growl escapes you.");
	sprintf (toRoom, "%s",
		 "A throaty growl resounds throughout the area.");
	break;
      }

      act (toChar, ch, NULL, NULL, TO_CHAR, POS_RESTING);
      act (toRoom, ch, NULL, NULL, TO_NOTVICT, POS_RESTING);
      return;
    }

    if (ch->pcdata->insanity < 50)
    {
      return;
    }

    if (ch->pcdata->insanity < 75)
    {
      return;
    }

    return;
  }

  if (ch->pcdata->insanity > 95)
  {
    send_to_char
      ("Saidin rages within you, uncontrollable. You pull more\n\r "
       "and more as you scream in defiance!\n\r", ch);
    act
      ("$n seems to fade in and out of view as his battle with the taint \n\r"
       "of Saidin finally ends.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
    raw_kill (ch, TRUE, "Went completely insane.");
  }

  number = number_percent ();

  if (number > ch->pcdata->insanity)
    return;


  if (number < 11)		/* 10% */
  {
    if (is_affected (ch, gsn_seize) && number_percent () < 30)
      affect_strip (ch, gsn_seize);
    return;
  }
  else
   if (number < 31)		/* 30% */
  {
    ROOM_INDEX_DATA *pRoomIndex;

    pRoomIndex = get_random_room (ch);

    act ("$n shakes violently, a grim look on $s face, then vanishes!",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
    char_from_room (ch);

    char_to_room (ch, pRoomIndex);
    act ("$n slowly fades into existence, looking around as if lost.",
	 ch, NULL, NULL, TO_ROOM, POS_RESTING);
    do_look (ch, "auto");

    if (is_affected (ch, gsn_seize) && number_percent () < 50)
      affect_strip (ch, gsn_seize);

    return;
  }
  else
   if (number < 61)		/* 60% */
  {
    switch (number_range (1, 6))
    {
    case 1:
    case 2:
      do_channel (ch, "rolling ring", TRUE);
      break;
    case 3:
      do_channel (ch, "mass invis", TRUE);
      break;
    case 4:
    case 5:
      do_channel (ch, "chain lightning", TRUE);
      break;
    case 6:
      do_channel (ch, "flame blast", TRUE);
      break;
    }

    if (is_affected (ch, gsn_seize) && number_percent () < 80)
      affect_strip (ch, gsn_seize);

    return;
  }

  send_to_char ("You struggle to hold on to your sanity.\n\r", ch);
  return;

}



/** Function: do_quotes
  * Descr   : Outputs a quote file to the player upon exit. Selection
  *         : is made at random from available files
  * Returns : (void|display only)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_quotes)
{
  int i;
  char buf[MSL];
  FILE *fp;

  if (num_quotes <= 0)
  {
    send_to_char ("All good things come to an end\n\r.", ch);
    return;
  }

  i = number_range (1, num_quotes);
  sprintf (buf, "%squote.%d", QUOTE_DIR, i);

  if ((fp = fopen (buf, "r")) == NULL)
  {
    log_string ("No quote files found.");
    send_to_char ("All good things must end...", ch);
    return;
  }

  send_to_char ("\n\r", ch);
  while (!feof (fp))
  {
    if (fgets (buf, MSL, fp) != NULL)
      send_to_char (buf, ch);
  }

  send_to_char ("\n\r", ch);
  fclose (fp);

}				/* end do_quotes */





/** Function: do_idle
  * Descr   : displays which characters have been sitting idle and for
  *         : how long.
  * Returns : (void|display only)
  * Syntax  : (void)
  * Written : v1.0 12/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_idle)
{
  DESCRIPTOR_DATA *d;

  send_to_char ("\n\r`cPlayers:      `bIdle Time:\n\r", ch);
  send_to_char ("`b==========================`w\n\r", ch);

  for (d = descriptor_list; d != NULL; d = d->next)
    if (d->character && can_see (ch, d->character))
      chprintf (ch, "`c%-20s %02d`w\n\r",
		      d->character->name, d->character->timer);

  return;
}				/* end do_idle */




/** Function: do_peaceful
  * Descr   : turns on (if allowed) a players (courageous) flag, allowing
  *         : them to pk, and be pk'd.
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_peaceful)
{

  if (ch->level < (pMud.level_no_peace - 5))
  {
    chprintf (ch,
		    "\n\rYou must be level %d or higher to become so aggressive!\n\r",
		    pMud.courageous);
    return;
  }
  ch->pcdata->peaceful = FALSE;
  send_to_char ("\n\rYour peaceful days are over!\n\r", ch);

}				/* end do_peaceful */



/** Function: do_pload
  * Descr   : Loads an unconnected player onto the mud, moving them to
  *         : the immortal's room so that they can be viewed/manipulated
  * Returns : (void)
  * Syntax  : pload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Update  : Characters returned to Original room by: Anthony Michael Tregre
  */
_DOFUN (do_pload)
{
  DESCRIPTOR_DATA d;
  bool isChar = FALSE;
  int result = READ_OK;

  if (argument[0] == '\0')
  {
    send_to_char ("Load who?\n\r", ch);
    return;
  }

  /* Don't want to load a second copy of a player who's already online! */
  if (get_char_world (ch, argument) != NULL)
  {
    send_to_char ("That person is already connected!\n\r", ch);
    return;
  }

  result = load_char_obj (&d, argument, FALSE);	/* char pfile exists? */

  isChar = (result == READ_OK);

  switch (result)
  {
	  case READ_POOR:
		send_to_char ("This pfile has problems. More specific information can"
			  " be found in the logs.\n\r", ch);
		break;

	  case READ_ERROR:
		send_to_char ("\n\rA critical error has occured attempting to load this"
			  " pfile.\n\rThe file has been moved to the restricted list"
			  " and cannot be loaded\n\runtil repaired.\n\r", ch);
		return;

	  case ERROR_UNDEFINED:
		send_to_char ("Load who? I can't seem to find that player file.\n\r", ch);
		return;

	  default:
		break;
  }


  if (!isChar)
    return;

  d.character->desc = NULL;
  d.character->next = char_list;
  char_list = d.character;
  d.connected = CON_PLAYING;
  reset_char (d.character);

  /* bring player to imm */
  if (d.character->in_room != NULL)
  {
    if (d.character->was_in_room == NULL)
      d.character->was_in_room = d.character->in_room;

    char_to_room (d.character, ch->in_room);
  }

  chprintf (ch, "You have pulled %s from the pattern!\n\r",
		  d.character->name);

  act ("$n has pulled $N from the pattern!",
       ch, NULL, d.character, TO_ROOM, POS_RESTING);

  if (d.character->pet != NULL)
  {
    char_to_room (d.character->pet, d.character->in_room);
    act ("$n has entered the game.", d.character->pet, NULL, NULL, TO_ROOM,
	 POS_RESTING);
  }

  if (d.character->mount != NULL)
  {
    char_to_room (d.character->mount, d.character->in_room);
    act ("$n has left reality behind once more!",
	 d.character->mount, NULL, NULL, TO_ROOM, POS_RESTING);
    add_follower (d.character->mount, d.character, TRUE);
    do_mount (d.character, d.character->mount->name);
  }

}				/* end do_pload */




/** Function: do_punload
  * Descr   : Unloads a previously "ploaded" player, returning them
  *         : back to the player directory.
  * Returns : (void)
  * Syntax  : punload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Update  : Characters returned to Original room by: Anthony Michael Tregre
  */
_DOFUN (do_punload)
{
  CHAR_DATA *victim;

  if ((victim = get_char_world (ch, argument)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  /** Person is legitimatly logged on... was not ploaded.
   */
  if (victim->desc != NULL)
  {
    send_to_char ("I don't think that would be a good idea...\n\r", ch);
    return;
  }

  if (victim->was_in_room != NULL)	/* return player and pet to orig room */
  {
    char_from_room (victim);
    char_to_room (victim, victim->was_in_room);

    if (victim->pet != NULL)
    {
      char_from_room (victim->pet);
      char_to_room (victim->pet, victim->was_in_room);
    }

    if (victim->mount != NULL)
    {
      char_from_room (victim->mount);
      char_to_room (victim->mount, victim->was_in_room);
    }
    ch->was_in_room = NULL;
  }

  do_quit (victim, "");

  act ("You have released $N back to the Pattern.",
       ch, NULL, victim, TO_CHAR, POS_DEAD);
  act ("$n has released $N back to the Pattern.",
       ch, NULL, victim, TO_ROOM, POS_RESTING);

}				/* end do_punload */



/** Function: do_release_top
  * Descr   : Removes a players "embracing the one power (AFF_EMBRACED)"
  *         : flag. Players can no longer weave.
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_release_top)
{
  int sn = -1;
  sh_int sex;
  AFFECT_DATA *paf, *paf_next;

  if (IS_NPC (ch))
    sex = ch->sex;
  else
    sex = ch->pcdata->true_sex;

  /** Note: 'release saidar' and 'release gaidin' commands share the same
    * command name, but only 'release gaidin' requires, or uses, an argument,
    * so, if argument is given, assume player meant release gaidin.
    */
  if (argument[0] != '\0')
  {
    do_unbond (ch, argument);
    return;
  }

  if (sex == SEX_MALE)
    sn = gsn_seize;
  else
    sn = gsn_embrace;

  if (!is_affected (ch, sn))
  {
    send_to_char ("But you are not touching The One Power!\n\r", ch);
    return;
  }

  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;

    if (paf->type == sn)
    {
      if (sex == SEX_MALE)
      {
	if (number_percent () < 100 - get_skill (ch, sn))
	{
	  send_to_char
	    ("Saidin fills you, the world seemingly teeming with life\n\r",
	     ch);
	  send_to_char
	    ("The will to release it just fades away in the flush of power!\n\r",
	     ch);
	  if (!IS_IMMORTAL (ch))
	    ch->pcdata->insanity++;
	  return;
	}
	send_to_char
	  ("The taint leaves a foul taste in your mouth as, regretfully, you release Saidin.\n\r",
	   ch);
      }
      else
	send_to_char
	  ("The luster of the world fades as, regretfully, you release Saidar.\n\r",
	   ch);

      affect_strip (ch, sn);
      return;
    }
  }

}				/* end of do_release_top */




/** Function: do_seize_top
  * Descr   : Set's a player's AFF_EMBRACED flag, allowing them to
  *         : weave "The One Power" for magical effects
  * Returns : (void)
  * Syntax  : (void)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_seize_top)
{
  int sn = -1;
  sh_int sex;
  AFFECT_DATA af;
  int chance;

  if (IS_NPC (ch))
    sex = ch->sex;
  else
    sex = ch->pcdata->true_sex;

  if (ch->race == race_lookup ("Ogier") && !IS_IMMORTAL (ch))
  {
    send_to_char ("You must be kidding, Ogier cant sense the One Power!\n\r",
		  ch);
    return;
  }

  if (!can_channel (ch))
  {
    send_to_char ("You cannot touch the One Power.\n\r", ch);
    return;
  }

  if (sex == SEX_MALE)
    sn = gsn_seize;
  else
    sn = gsn_embrace;

  if (IS_SET (ch->in_room->room_flags, ROOM_NOMAGIC) ||
      IS_SET (ch->in_room->room_flags, ROOM_STEDDING))
  {
    send_to_char ("You cannot sense the One Power here!\n\r", ch);
    return;
  }

  if (IS_AFFECTED (ch, _AFF_SHIELDING))
  {
    send_to_char("You can sense but not reach the One Power!\n\r", ch);
    return;
  }

  if (ch->position == POS_FIGHTING &&
      (!is_clan (ch) || !IS_FLAG (clan_table[ch->clan].flags, _ASHAMAN)))
  {
    send_to_char ("You cant concentrate enough!\n\r", ch);
    return;
  }

  WAIT_STATE (ch, skill_table[sn].beats);

  chance = number_percent ();

  if (chance > (IS_NPC (ch) ? 40 + ch->level : ch->pcdata->learned[sn]))
  {
    if (sex == SEX_MALE)
      send_to_char
	("You reach out for Saidin, but it skitters away from the void.\n\r",
	 ch);
    else
      send_to_char
	("You try to embrace Saidar, but forcing it is not the way.\n\r", ch);
    check_improve (ch, sn, FALSE, 1);
    return;
  }

  if (is_affected (ch, sn))
  {
    send_to_char
      ("The One Power courses through your body as you try to draw more than you can safely handle!\n\r",
       ch);
    if (ch->sex == SEX_MALE)
      random_insanity (ch);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = ch->level;
  af.duration = ch->level;
  af.modifier = -1;
  af.location = APPLY_SAVES;
  af.bitvector = _AFF_EMBRACED;
  af.owner = GET_ID(ch);
  af.valid = TRUE;

  affect_to_char (ch, &af);

  if (sex == SEX_MALE)
  {
    send_to_char
      ("The oily taint of Saidin rolls over you as you are filled with The One Power!\n\r",
       ch);
    random_insanity (ch);
  }
  else
    send_to_char
      ("Saidar wraps gently around you as you are filled with The One Power!\n\r",
       ch);

  return;
}				/* end do_seize_top */


/** Function: do_slay
  * Descr   : kills another player or mob, without a fight. Allows
  *         : for definable "slay message options"
  * Returns : (void)
  * Syntax  : slay (who) [option]
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_slay)
{
  CHAR_DATA *victim;
  char type[MIL];
  char who[MIL];
  char buf[MSL];
  int i = 0;
  bool found = FALSE;

  argument = one_argument (argument, who);
  argument = one_argument (argument, type);

  if (!str_prefix (who, "list") || who == NULL)
  {
    send_to_char ("\n\rSyntax: slay [who] <type>\n\r", ch);
    send_to_char ("where type is one of the following...\n\r\n\r", ch);

    for (i = 0; i < MAX_SLAY_TYPES; i++)

      if ((slay_table[i].owner == NULL) ||
	  (!str_prefix (slay_table[i].owner, ch->name) &&
	   slay_table[i].title[0] != '\0'))
      {
	sprintf (buf, "%s\n\r", slay_table[i].title);
	send_to_char (buf, ch);
      }

    send_to_char ("\n\rTyping just 'slay <player>' will work too...\n\r", ch);
    return;
  }

  if ((victim = get_char_room (ch, who)) == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }
  else
   if (ch == victim)
  {
    send_to_char
      ("The creator has fixed his cannon against self slaughter!\n\r", ch);
    return;
  }
  else
   if (!IS_NPC (victim) && get_trust (victim) >= get_trust (ch))
  {
    send_to_char ("You failed.\n\r", ch);
    return;
  }
  else
   if (type[0] == '\0')
  {
    raw_kill (victim, FALSE, "");
    return;
  }
  else

  {
    /* find the slay type */
    for (i = 0; i < MAX_SLAY_TYPES; i++)
    {
      if (!str_prefix (type, slay_table[i].title) &&
	  (slay_table[i].owner == NULL ||
	   !str_prefix (slay_table[i].owner, ch->name)))
      {
	found = TRUE;
	sprintf (buf, "%s\n\r", slay_table[i].char_msg);
	act (buf, ch, NULL, victim, TO_CHAR, POS_DEAD);
	sprintf (buf, "%s\n\r", slay_table[i].vict_msg);
	act (buf, ch, NULL, victim, TO_VICT, POS_DEAD);
	sprintf (buf, "%s\n\r", slay_table[i].room_msg);
	act (buf, ch, NULL, victim, TO_NOTVICT, POS_RESTING);
	raw_kill (victim, FALSE, "");
	return;
      }
    }
  }

  if (!found)
    send_to_char
      ("Slay type not defined. Type \"slay list\" for a complete listing of types available to you.\n\r",
       ch);

  return;
}				/* end do_slay */




/** Function: do_bonus
  * Descr   : Adds or subtracts experience points from a player, designed
  *         : to be used primarily as an RP bonus.
  * Returns : (void)
  * Syntax  : bonus (who) (amount)
  * Written : v1.0 1/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_bonus)
{
  CHAR_DATA *victim;
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  int value = 0;

  memset(arg1, 0, MIL);
  memset(arg2, 0, MIL);
  memset(arg3, 0, MIL);

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  argument = one_argument (argument, arg3);

  if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' || !is_number (arg3))
  {
    chprintf (ch, "Syntax: bonus <char> [experience|practice] <Amount>.\n\r");
    chprintf (ch, "A negative figure will deduct the amount from players,\n\r");
    chprintf (ch, "While a postive amount, will of course give them that amount.\n\r");
    return;
  }

  if ((victim = get_char_world (ch, arg1)) == NULL)
  {
    chprintf(ch, "That Player is not here.\n\r");
    return;
  }

  if (IS_NPC (victim))
  {
    chprintf(ch, "Not on NPC's.\n\r");
    return;
  }

  if (ch == victim)
  {
    chprintf(ch, "You may not give yourself a bonus.\n\r");
    return;
  }

  if (IS_IMMORTAL (victim) || victim->level >= LEVEL_IMMORTAL)
  {
    chprintf(ch, "Immortals dont need any bonuses!\n\r");
    return;
  }

  if ( arg3[0] )
    value = atoi(arg3);

  if ( !str_prefix(arg2, "practice") )
  {

	  if (value < -1 || value > 1 || value == 0)
	  {
		  chprintf(ch,"Value range is -1 to 1.\n\r");
		  return;
	  }
	  
	  victim->practice += value;
	  victim->pcdata->PracticesGiven+=value;

	  if (victim->pcdata->PracticesGiven > 10)
	  {
		  logf("%s has had more than 10 practices given to them.", victim->name);
		  chprintf(ch, "%s has had more than 10 practices given to them. This has been logged.\n\r"
			           "Please be certain that you are giving these bonuses for the right reasons.\n\r", victim->name);
	  }
	  
	  chprintf(ch, "You have given a bonus of %d practice points to %s.\n\r", value, victim->name);
	  
	  if ( value > 0 )
		  chprintf(victim, "You have been given a bonus of %d practice points!\n\r", value);
	  else
		  chprintf(victim, "You have been penalized %d practice points.\n\r", -value);
	  
	  sprintf (arg3, "$N has given %s a bonus of %d practices.", victim->name, value);
	  wiznet (arg3, ch, NULL, WIZ_SECURE, 0, get_trust(ch));
  }
  
  if ( !str_prefix(arg2, "experience") )
  {
	  if (value < -5000 || value > 5000 || value == 0)
	  {
		chprintf(ch, "Value range is -5000 to 5000.\n\r");
		return;
	  }

	  gain_exp (victim, value);

	  chprintf(ch, "You have given a bonus of %d experience points to %s.\n\r", value, victim->name);

	  if (value > 0)
		chprintf(victim, "You have been been given a bonus of %d experience points bonus.\n\r", value);
	  else
		chprintf(victim, "You have been penalized %d experience points.\n\r", -value);

 	  sprintf (arg3, "$N has given %s a bonus of %d experience points.", victim->name, value);
	  wiznet (arg3, ch, NULL, WIZ_SECURE, 0, get_trust(ch));
  
  }

  return;
}

/** Function: do_vfind
  * Descr   : Locates a virtual item, room, mob or pc, that matches requested 
  *         : data. 
  * Returns : output of item(s)
  * Syntax  : vfind ( mob | obj | room [ options -- see below ] )
  * Written : v1.0 9/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Notes   : TypeKeys defined as:
  * 		1	= vnum
  *		2	= level
  *		3	= flag
  *		4	= wearloc
  *		5	= non-weapon type
  *		6	= weapon class
  *		7	= weapon type2 (flaming/vorpal,etc)
  *		8	= affects
  *		9	= skill/weaves
  *		10	= description (long or short)
  *		11	= name
  *		12	= sex
  *		13	= class
  *		14	= race
  *		15	= nation
  *		16	= guild
  *		17 	= sector
  *		18	= heal rate
  *		19	= opr rate
  *		20	= owner
  *		21	= points (obj)
  *		22	= area (obj|mob)
  */
_DOFUN (do_vfind)
{
  char Key[MIL];
  char Type[MIL];
  int TypeKey = -1;
  bool vFound = FALSE;
  long value = -1;
  int min = -1;
  int max = -1;
  const struct flag_type *Table = NULL;
  // construct a few placeholders...
  const struct flag_type *_imm_bits = _resistance_bits;
  const struct flag_type *_res_bits = _resistance_bits;
  const struct flag_type *_vuln_bits = _resistance_bits;
  BUFFER *buf = NULL;


  /*
   * Valid values for Key: mob|obj|room
   */
  argument = one_argument (argument, Key);
  argument = one_argument (argument, Type);

  /* main command syntax */
  if (Key[0] == '\0' || Type[0] == '\0' || argument[0] == '\0')
  {
    send_to_char ("Syntax: vfind (mob|obj|room) [option (value)]\n\r", ch);
    send_to_char ("  valid options include:\n\r\n\r", ch);
    send_to_char ("    numeric: vnum, level, healrate, oprrate\n\r", ch);
    send_to_char
      ("    token  : flag, wear_loc, type, weapon class, weapon type, affects,\n\r"
       "             skill/weave, name, description, sex, class, race, nation,\n\r"
       "             guild, sector, owner, points, area\n\r\n\r", ch);

    send_to_char ("Examples: \n\r\n\r"
		  "  vfind obj flag burnproof\n\r"
		  "  vfind obj affect hp\n\r"
		  "  vfind mob level 20 40\n\r"
		  "  vfind mob race trolloc\n\r"
		  "  vfind room flag private\n\r"
		  "  vfind room guild aiel\n\r\n\r", ch);
    return;
  }

  if (!str_cmp (Type, "vnum") || !str_cmp (Type, "level"))
  {
    char buf1[MIL];
    char buf2[MIL];

    if (!str_cmp (Type, "vnum"))
      TypeKey = 1;
    else
      TypeKey = 2;

    argument = one_argument (argument, buf1);
    argument = one_argument (argument, buf2);

    if (buf1[0] == '\0' || !is_number (buf1) ||
	(buf2[0] != '\0' && !is_number (buf2)))
    {
      send_to_char ("Invalid numeric value.\n\r", ch);
      return;
    }

    min = atol (buf1);

    if (buf2[0] != '\0')
      max = atol (buf2);
  }
  else
   if (!str_cmp (Type, "flag"))
  {
    int i;

    TypeKey = 3;

    for (i = 0; flag_stat_table[i].structure != NULL; i++)
    {
      if ((value =
	   flag_lookup_exact (argument,
			      flag_stat_table[i].structure)) != NO_FLAG)
      {
	Table = flag_stat_table[i].structure;
	vFound = TRUE;
	break;
      }
    }

    if (!vFound)
    {
      send_to_char ("Unknown flag.\n\r", ch);
      return;
    }

  }
  else
   if (!str_prefix (Type, "wearloc"))
  {

    TypeKey = 4;

    if ((value = flag_lookup (argument, wear_flags)) != NO_FLAG)
    {
      vFound = TRUE;
      Table = wear_flags;
    }

    if (!vFound)
    {
      send_to_char
	("Invalid wear location. Valid wear locations are:\n\r\n\r", ch);
      show_flag_cmds (ch, wear_flags);
      return;
    }

  }
  else
   if (!str_cmp (Type, "type"))
  {

    if ((value = flag_lookup (argument, type_flags)) != NO_FLAG)
    {
      TypeKey = 5;
      vFound = TRUE;
      Table = type_flags;
    }

    if (!vFound)
      if ((value = flag_lookup (argument, weapon_class)) != NO_FLAG)
      {
	TypeKey = 6;
	vFound = TRUE;
	Table = weapon_class;
      }

    if (!vFound)
      if ((value = flag_lookup (argument, weapon_type2)) != NO_FLAG)
      {
	TypeKey = 7;
	vFound = TRUE;
	Table = weapon_type2;
      }

    if (!vFound)
    {
      send_to_char ("Invalid type. Valid types are:\n\r\n\r", ch);
      send_to_char ("-*- General Type flags -*-\n\r", ch);
      show_flag_cmds (ch, type_flags);
      send_to_char ("-*- Weapon Class flags -*-\n\r", ch);
      show_flag_cmds (ch, weapon_class);
      send_to_char ("-*- Weapon Type flags-*-\n\r", ch);
      show_flag_cmds (ch, weapon_type2);
      return;
    }

  }
  else
   if (!str_prefix (Type, "affects"))
  {
    TypeKey = 8;

    if ((value = flag_lookup (argument, _affected_by_bits)) != NO_FLAG)
    {
      vFound = TRUE;
      Table = _affected_by_bits;
    }

    if (!vFound)
      if ((value = flag_lookup (argument, apply_flags)) != NO_FLAG)
      {
	vFound = TRUE;
	Table = apply_flags;
      }


    if (!vFound)
    {
      send_to_char ("Unknown affect. Valid affects (& applies) are:\n\r\n\r",
		    ch);
      send_to_char ("-*- Affect Flags -*-\n\r", ch);
      show_flag_cmds (ch, _affected_by_bits);
      send_to_char ("-*- Apply Flags -*-\n\r", ch);
      show_flag_cmds (ch, apply_flags);
      return;
    }
  }
  else

   if (!str_prefix (Type, "skill") || !str_prefix (Type, "weave"))
  {
    TypeKey = 9;

    value = skill_lookup (argument);

    if (value == -1)
    {
      send_to_char ("Invalid skill or weave.\n\r", ch);
      return;
    }

  }
  else
   if (!str_prefix (Type, "description"))
    TypeKey = 10;
  else
   if (!str_prefix (Type, "name"))
    TypeKey = 11;

  else
   if (!str_prefix (Type, "sex"))
  {
    TypeKey = 12;

    switch (*argument)
    {
    default:
      value = SEX_NEUTRAL;
      break;
    case 'm':
    case 'M':
      value = SEX_MALE;
      break;
    case 'f':
    case 'F':
      value = SEX_FEMALE;
      break;
    }
  }
  else
   if (!str_prefix (Type, "class"))
  {
    TypeKey = 13;
    value = class_lookup (argument);

    if (value == -1)
    {
      send_to_char ("Invalid class. Valid classes are:\n\r\n\r", ch);

      for (value = 0; value < MAX_CLASS; value++)
	chprintf (ch, "%-20.20s%s",
			class_table[value].name,
			((value + 1) % 5 == 0) ? "\n\r" : (value + 1 <
							   MAX_CLASS) ? ", " :
			" ");

      return;
    }
  }
  else
   if (!str_prefix (Type, "race"))
  {
    TypeKey = 14;
    value = race_lookup (argument);

    if (value == -1)
    {
      send_to_char ("Invalid race. Valid races are:\n\r\n\r", ch);
      for (value = 0; race_table[value].name != NULL; value++)

	chprintf (ch, "%-20.20s%s",
			race_table[value].name,
			((value + 1) % 3 == 0) ? "\n\r" : "");

      return;
    }
  }
  else
   if (!str_prefix (Type, "nation"))
  {
    TypeKey = 15;
    value = national_number (argument);

    if (value == -1)
    {
      send_to_char ("Invalid nation. Valid nations are:\n\r\n\r", ch);
      for (value = 0; nation_table[value].nation[0] != '\0'; value++)

	chprintf (ch, "%-20.20s%s",
			nation_table[value].nation,
			((value + 1) % 3 == 0) ? "\n\r" : "");

      return;
    }
  }
  else
   if (!str_prefix (Type, "guild") || !str_prefix (Type, "clan"))
  {
    TypeKey = 16;
    value = clan_lookup (argument);

    if (value == -1)
    {
      send_to_char ("Invalid guild. Valid guilds are:\n\r\n\r", ch);
      for (value = 0; clan_table[value].name != NULL; value++)

	chprintf (ch, "%-20.20s%s",
			clan_table[value].name,
			((value + 1) % 3 == 0) ? "\n\r" : "");

      return;
    }
  }
  else
   if (!str_prefix (Type, "sector"))
  {
    TypeKey = 17;

    if ((value = flag_value (sector_flags, argument)) == NO_FLAG)
    {
      send_to_char ("Invalid sector  Valid sectors are:\n\r\n\r", ch);
      show_flag_cmds (ch, sector_flags);
      return;
    }


  }
  else
   if (!str_prefix (Type, "healrate"))
  {
    TypeKey = 18;

    if (!is_number (argument))
    {
      send_to_char ("Invalid heal rate.\n\r", ch);
      return;
    }

    value = atoi (argument);
  }
  else
   if (!str_prefix (Type, "oprrate"))
  {
    TypeKey = 19;

    if (!is_number (argument))
    {
      send_to_char ("Invalid opr rate.\n\r", ch);
      return;
    }

    value = atoi (argument);
  }
  else
   if (!str_prefix (Type, "owner"))
    TypeKey = 20;

  else
   if (!str_prefix (Type, "points"))
  {
    char buf1[MIL];
    char buf2[MIL];

    TypeKey = 21;

    argument = one_argument (argument, buf1);
    argument = one_argument (argument, buf2);

    if (buf1[0] == '\0' || !is_number (buf1) ||
	(buf2[0] != '\0' && !is_number (buf2)))
    {
      send_to_char ("Invalid numeric value.\n\r", ch);
      return;
    }

    min = atol (buf1);

    if (buf2[0] != '\0')
      max = atol (buf2);

  }
  else
   if (!str_prefix (Type, "area"))
    TypeKey = 22;
  else

  {
    do_help (ch, "vfind");
    return;
  }


  vFound = FALSE;
  buf = new_buf ();

  switch (*Key)
  {
  default:
    do_help (ch, "vfind");
    break;

  case 'o':
  case 'O':			/* search for object */
    {
      OBJ_DATA *obj = NULL;
      OBJ_DATA *next = NULL;

      char temp[MIL];
      int number = 0;

      for (obj = object_list; obj != NULL && number < 150; obj = next)
      {
	next = obj->next;

	vFound = FALSE;

	if (obj == NULL || !can_see_obj (ch, obj))
	  continue;

	if (number > 150)
	{
	  add_buf (buf,
		   "Maximum # of objects reached. Narrow your search.\n\r");
	  break;
	}

	switch (TypeKey)
	{
	case 1:
	  if ((max > 0
	       && (obj->pIndexData->vnum <= max &&
		   obj->pIndexData->vnum >= min))
	      || obj->pIndexData->vnum == min)
	    vFound = TRUE;

	  break;

	case 2:
	  if ((max > 0
	       && (obj->level <= max && obj->level >= min))
	      || obj->level == min)
	    vFound = TRUE;

	  break;
	case 3:
	  if ((Table == extra_flags && IS_SET (obj->extra_flags, value)) ||
	      (Table == wear_flags && IS_SET (obj->wear_flags, value)))
	    vFound = TRUE;

	  break;

	case 4:
	  if (IS_SET (obj->wear_flags, value))
	    vFound = TRUE;

	  break;

	case 5:
	  if (obj->item_type == value)
	    vFound = TRUE;

	  break;

	case 6:
	  if (obj->item_type == ITEM_WEAPON && obj->value[0] == value)
	    vFound = TRUE;

	  break;

	case 7:
	  if (obj->item_type == ITEM_WEAPON && obj->value[4] == value)
	    vFound = TRUE;

	  break;

	case 8:
	  {
	    AFFECT_DATA *pAf;

	    if (!obj->enchanted)
	    {
	      for (pAf = obj->pIndexData->affected; pAf; pAf = pAf->next)
		if (pAf->location == value || pAf->bitvector == value)
		{
		  vFound = TRUE;
		  break;
		}
	    }
	    else

	    {
	      for (pAf = obj->affected; pAf; pAf = pAf->next)
		if (pAf->location == value || pAf->bitvector == value)
		{
		  vFound = TRUE;
		  break;
		}
	    }

	  }
	  break;

	case 9:
	  switch (obj->item_type)
	  {
	  case ITEM_WAND:
	  case ITEM_STAFF:
	    if (obj->value[3] == value)
	      vFound = TRUE;
	    break;
	  case ITEM_POTION:
	  case ITEM_HERB:
	    if (obj->value[1] == value ||
		obj->value[2] == value ||
		obj->value[3] == value || obj->value[4] == value)
	      vFound = TRUE;
	    break;
	  default:
	    break;
	  }

	  break;

	case 10:
	  if (is_name (argument, obj->short_descr) ||
	      is_name (argument, obj->description))
	    vFound = TRUE;

	  break;

	case 11:
	  if (is_name (argument, obj->name))
	    vFound = TRUE;

	  break;

	case 21:		/* points */
	  {
	    int cost = eq_cost (obj);

	    if ((max > 0 && (cost <= max && cost >= min)) || cost == min)
	      vFound = TRUE;
	  }
	  break;

	case 22:		/* area */
	  if (obj->in_room &&
	      !str_prefix (obj->in_room->area->name, argument))
	    vFound = TRUE;

	  break;

	default:
	  send_to_char ("Invalid search key for objects.\n\r", ch);
	  return;


	}			/* end switch */

	if (vFound)
	{

	  if (++number > 150)
	  {
	    add_buf (buf, "Maximum # found. Narrow your search.\n\r");
	    break;
	  }
	  else

	  {
	    if (obj->carried_by != NULL
		&& can_see (ch, obj->carried_by)
		&& obj->carried_by->in_room != NULL)
	      sprintf (temp,
		       "%3d] %5d] %-29.29s`w carried by %-15.15s`w [Room %ld]\n\r",
		       number,
		       obj->pIndexData->vnum,
		       strip_codes (obj->short_descr),
		       strip_codes (PERS (obj->carried_by, ch)),
		       obj->carried_by->in_room->vnum);

	    else
	     if (obj->in_room != NULL && can_see_room (ch, obj->in_room))

	      sprintf (temp,
		       "%3d] %5d] %-29.29s`w in %-23.23s`w [Room %ld]\n\r",
		       number,
		       obj->pIndexData->vnum,
		       strip_codes (obj->short_descr),
		       strip_codes (obj->in_room->name), obj->in_room->vnum);

	    else
	      sprintf (temp, "%3d] %5d] %-29.29s`w is somewhere\n\r",
		       number,
		       obj->pIndexData->vnum, strip_codes (obj->short_descr));

	    add_buf (buf, temp);
	  }
	}
      }

      break;

    }				/* end: case 'o' */

  case 'm':
  case 'M':			/* search for mob */
    {
      CHAR_DATA *mob;
      CHAR_DATA *next;
      char temp[MIL];
      int number = 0;
      sh_int mVnum = 0;

      for (mob = char_list; mob != NULL && number < 150; mob = next)
      {
	next = mob->next;

	vFound = FALSE;

	if (number > 150)
	{
	  add_buf (buf,
		   "Maximum # of mobiles found. Narrow your search.\n\r");
	  break;
	}

	switch (TypeKey)
	{

	case 1:
	  {

	    mVnum = (IS_NPC (mob) ? mob->pIndexData->vnum : 0);

	    if ((max > 0 && (mVnum <= max && mVnum >= min)) || mVnum == min)
	      vFound = TRUE;

	    if (vFound)
	      vFound = can_see (ch, mob);

	    break;
	  }
	case 2:
	  if ((max > 0
	       && (mob->level <= max && mob->level >= min))
	      || mob->level == min)
	    vFound = TRUE;

	  break;

	case 3:
	  if (IS_NPC (mob) &&
	      ((Table == _act_bits &&
		IS_FLAG (mob->pIndexData->states.player, value))
	       || (Table == _imm_bits &&
		   IS_FLAG (mob->pIndexData->states.imm, value))
	       || (Table == _res_bits &&
		   IS_FLAG (mob->pIndexData->states.res, value))
	       || (Table == _vuln_bits &&
		   IS_FLAG (mob->pIndexData->states.vuln, value))
	       || (Table == _off_bits &&
		   IS_FLAG (mob->pIndexData->states.off, value))))
	    vFound = TRUE;

	  else
	     if ((Table == _act_bits && IS_FLAG (mob->states.player, value))
		 || (Table == _imm_bits && IS_FLAG (mob->states.imm, value))
		 || (Table == _res_bits && IS_FLAG (mob->states.res, value))
		 || (Table == _vuln_bits && IS_FLAG (mob->states.vuln, value))
		 || (Table == _off_bits && IS_FLAG (mob->states.off, value)))
	    vFound = TRUE;

	  break;

	case 8:
	  {
	    AFFECT_DATA *pAf;

	    if (mob->affected)
	    {
	      for (pAf = mob->affected; pAf; pAf = pAf->next)
		if (pAf->location == value || pAf->bitvector == value)
		{
		  vFound = TRUE;
		  break;
		}
	    }

	    if (!vFound)
	      vFound = IS_FLAG (mob->states.affected_by, value);

	    if (!vFound && IS_NPC (mob))
	      vFound = IS_FLAG (mob->pIndexData->states.affected_by, value);

	  }

	  break;

	case 10:
	  if (is_name (argument, mob->short_descr) ||
	      is_name (argument, mob->long_descr))
	    vFound = TRUE;

	  break;

	case 11:
	  if (is_name (argument, mob->name))
	    vFound = TRUE;

	  break;

	case 12:
	  if (mob->sex == value)
	    vFound = TRUE;

	  break;

	case 13:
	  if (!IS_NPC (mob) && mob->c_class == value)
	    vFound = TRUE;

	  break;

	case 14:
	  if (mob->race == value)
	    vFound = TRUE;

	  break;

	case 15:
	  if (!IS_NPC (mob) && !str_prefix (mob->pcdata->nation, argument))
	    vFound = TRUE;

	  break;

	case 16:
	  if (mob->clan == value)
	    vFound = TRUE;

	  break;

	case 22:		/* area */
	  if (mob->in_room &&
	      !str_prefix (mob->in_room->area->name, argument))
	    vFound = TRUE;

	  break;

	default:
	  send_to_char ("Invalid search key for mobs.\n\r", ch);
	  return;

	}			/* end switch */

	if (vFound)
	{
	  number++;

	  if (number > 150)
	  {
	    add_buf (buf, "Maximum # found. Narrow your search.\n\r");
	    break;
	  }
	  else
	  {
	    if (mob->in_room != NULL)
	      sprintf (temp, "%3d] %5ld] %-29.29s at %-15.15s [Room %ld]\n\r",
		       number,
		       IS_NPC (mob) ? mob->pIndexData->vnum : 0,
		       PERS (mob, ch),
		       strip_codes (mob->in_room->name), mob->in_room->vnum);
	    else

	      sprintf (temp, "%3d] %5ld] %-29.29s\n\r",
		       number,
		       IS_NPC (mob) ? mob->pIndexData->vnum : 0,
		       PERS (mob, ch));

	    add_buf (buf, temp);
	  }
	}			/* end: if vfound */

      }				/* end for */


    }
    break;

  case 'r':
  case 'R':			/* search for room */
    {
      ROOM_INDEX_DATA *room;
      long iHash = 0;
      int number = 0;
      char temp[MSL];

      for (iHash = 0; iHash < MAX_KEY_HASH && number < 150; iHash++)
      {
	for (room = room_index_hash[iHash]; room; room = room->next)
	{

	  vFound = FALSE;

	  if (number > 150)
	  {
	    add_buf (buf,
		     "Maximum # of rooms found. Narrow your search.\n\r");
	    break;
	  }

	  switch (TypeKey)
	  {
	  case 1:
	    if ((max > 0
		 && (room->vnum <= max &&
		     room->vnum >= min)) || room->vnum == min)
	      vFound = TRUE;

	    break;

	  case 3:
	    if (Table == room_flags && IS_SET (room->room_flags, value))
	      vFound = TRUE;

	    break;

	  case 10:
	    if (room->description && room->description[0] != '\0' &&
		is_name (argument, room->description))
	      vFound = TRUE;

	    break;

	  case 11:
	    if (room->name && room->name[0] != '\0' &&
		is_name (argument, room->name))
	      vFound = TRUE;

	    break;

	  case 16:
	    if (room->clan == value)
	      vFound = TRUE;

	    break;

	  case 17:
	    if (room->sector_type == value)
	      vFound = TRUE;

	    break;

	  case 18:
	    if (room->heal_rate == value)
	      vFound = TRUE;

	    break;

	  case 19:
	    if (room->opu_rate == value)
	      vFound = TRUE;

	    break;

	  case 20:
	    if (room->owner[0] != '\0' && !str_prefix (room->owner, argument))
	      vFound = TRUE;

	    break;

	  }			/* end: switch TypeKey */

	  if (vFound)
	  {
	    if (++number > 150)
	    {
	      add_buf (buf,
		       "Maximum # of rooms found. Narrow your search.\n\r");
	      break;
	    }
	    else

	    {
	      sprintf (temp, "%3d] %5ld] %-29.29s area: %-20.20s\n\r",
		       number,
		       room->vnum,
		       strip_codes (room->name),
		       strip_codes (room->area->name));

	      add_buf (buf, temp);
	    }
	  }			/* end: if vfound */
	}			/* end: for room */
      }				/* end: for iHash */
    }				/* end: case 'r' */
  }				/* end: switch key */

  if (buf->string[0] == '\0')
    send_to_char ("Unable to locate requested item(s)\n\r", ch);
  else

  {
    page_to_char (buf_string (buf), ch);
    free_buf (buf);
  }

  return;
}


/* by Zimmer */
_DOFUN (do_xlist)
{
  ROOM_INDEX_DATA *pRoom;
  RESET_DATA *pReset;
  AREA_DATA *pArea;
  char arg[MIL];
  char outbuf[MAX_STRING_LENGTH * 8];
  bool found = FALSE;
  bool fAll = FALSE;
  bool fMob = FALSE;
  bool fObj = FALSE;
  long rvnum;
  long vnum = 0;

  pArea = ch->in_room->area;

  if (argument[0] == '\0')
    fAll = TRUE;
  else
  {
    argument = one_argument (argument, arg);
    fMob = !str_cmp (arg, "mob");
    fObj = !str_cmp (arg, "obj");
    if (!fMob && !fObj)
    {
      send_to_char ("Syntax: xlist\n\r"
		    "        xlist mob <vnum>\n\r"
		    "        xlist obj <vnum>\n\r", ch);
      return;
    }
    else if (is_number (argument))
    {
      vnum = atol (argument);
      if ((vnum < pArea->min_vnum) || (vnum > pArea->max_vnum))
      {
	send_to_char ("Invalid vnum for this area!\n\r", ch);
	return;
      }
    }
  }

  strcpy (outbuf,
	  "Vnum     Description             Location Room World/Room\n\r");
  strcat (outbuf,
	  "======== ======================= ======== ==============\n\r");

  for (rvnum = pArea->min_vnum; rvnum <= pArea->max_vnum; rvnum++)
  {
    if ((pRoom = get_room_index (rvnum)) != NULL)
    {
      for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
      {
	MOB_INDEX_DATA *pMob;
	OBJ_INDEX_DATA *pObj;
	char buf[MAX_STRING_LENGTH];

	switch (pReset->command)
	{
	case 'M':
	  if (fAll || fMob)
	  {
	    if (fMob && (pReset->arg1 != vnum) && (vnum != 0))
	      break;
	    found = TRUE;
	    pMob = get_mob_index (pReset->arg1);
	    sprintf (buf, "M[%5ld] %-23.23s in room %5ld [%3ld/%3ld]\n\r",
		     pReset->arg1,
		     strip_codes (pMob->short_descr),
		     pReset->arg3, pReset->arg2, pReset->arg4);
	    strcat (outbuf, buf);
	  }
	  break;

	case 'O':
	  if (fAll || fObj)
	  {
	    if (fObj && (pReset->arg1 != vnum) && (vnum != 0))
	      break;
	    found = TRUE;
	    pObj = get_obj_index (pReset->arg1);
	    sprintf (buf, "O[%5ld] %-23.23s in room %5ld [%7ld]\n\r",
		     pReset->arg1,
		     strip_codes (pObj->short_descr),
		     pRoom->vnum, pReset->arg2);
	    strcat (outbuf, buf);
	  }
	  break;

	case 'P':
	  if (fAll || fObj)
	  {
	    if (fObj && (pReset->arg1 != vnum) && (vnum != 0))
	      break;
	    found = TRUE;
	    pObj = get_obj_index (pReset->arg1);
	    sprintf (buf, "O[%5ld] %-23.23s in obj  %5ld [%3ld/%3ld]\n\r",
		     pReset->arg1,
		     strip_codes (pObj->short_descr),
		     pRoom->vnum, pReset->arg2, pReset->arg4);
	    strcat (outbuf, buf);
	  }
	  break;

	case 'G':
	case 'E':
	  if (fAll || fObj)
	  {
	    if (fObj && (pReset->arg1 != vnum) && (vnum != 0))
	      break;
	    found = TRUE;
	    pObj = get_obj_index (pReset->arg1);
	    sprintf (buf, "O[%5ld] %-23.23s mob inv %5ld [%7ld]\n\r",
		     pReset->arg1,
		     strip_codes (pObj->short_descr),
		     pRoom->vnum, pReset->arg2);
	    strcat (outbuf, buf);
	  }
	  break;

	default:
	  break;		/* ignore all other resets ( D, R ) */
	}
      }
    }
  }

  if (!found)
  {
    send_to_char ("No reset(s) found.\n\r", ch);
    return;
  }

  page_to_char (outbuf, ch);
  return;
}

/*
 * 'show_obj_values2' and 'set_obj_values2' were obviously culled from
 * OLC. (Sorry Ivan).  These are temporary, just to get qgen up and running
 * (sigh, i know...) anyway, note to self: Rewrite these, and their OLC
 * counterparts to work together, share and share alike... 
 * --GM
 */
void show_obj_values2 (CHAR_DATA * ch, OBJ_DATA * obj)
{
  switch (obj->item_type)
  {
  default:			
    break;

  case ITEM_LIGHT:
    if (obj->value[2] == -1 || obj->value[2] == 999)	
      chprintf(ch, "[v2] Light:  Infinite[-1]\n\r");
    else
      chprintf(ch, "[v2] Light:  [%d]\n\r", obj->value[2]);
    break;


  case ITEM_PORTAL:
    chprintf(ch,
	     "[v0] Charges:        [%d]\n\r"
	     "[v1] Exit Flags:     %s\n\r"
	     "[v2] Portal Flags:   %s\n\r"
	     "[v3] Goes to (vnum): [%d]\n\r",
	     obj->value[0],
	     flag_string (exit_flags, obj->value[1]),
	     flag_string (portal_flags, obj->value[2]), obj->value[3]);
    break;

  case ITEM_WAYGATE:
    chprintf(ch,
	     "[v1] Exit Flags:     %s\n\r"
	     "[v3] Goes to (vnum): [%d]\n\r",
	     flag_string (exit_flags, obj->value[1]), obj->value[3]);
    break;

  case ITEM_FURNITURE:
    chprintf(ch,
	     "[v0] Max people      : [%d]\n\r"
	     "[v1] Max weight      : [%d]\n\r"
	     "[v2] Furniture Flags : %s\n\r"
	     "[v3] Heal bonus      : [%d]\n\r"
	     "[v4] OPU  bonus      : [%d]\n\r",
	     obj->value[0],
	     obj->value[1],
	     flag_string (furniture_flags, obj->value[2]),
	     obj->value[3], obj->value[4]);
    break;

  case ITEM_POTION:
  case ITEM_HERB:
    chprintf(ch,
	     "[v0] Level:  [%d]\n\r"
	     "[v1] Weave:  %s\n\r"
	     "[v2] Weave:  %s\n\r"
	     "[v3] Weave:  %s\n\r"
	     "[v4] Weave:  %s\n\r",
	     obj->value[0],
	     obj->value[1] != -1 ? skill_table[obj->value[1]].name
	     : "none",
	     obj->value[2] != -1 ? skill_table[obj->value[2]].name
	     : "none",
	     obj->value[3] != -1 ? skill_table[obj->value[3]].name
	     : "none",
	     obj->value[4] != -1 ? skill_table[obj->value[4]].name : "none");
    break;

  case ITEM_ARMOR:
    chprintf(ch,
	     "[v0] Ac pierce       [%d]\n\r"
	     "[v1] Ac bash         [%d]\n\r"
	     "[v2] Ac slash        [%d]\n\r"
	     "[v3] Ac exotic       [%d]\n\r",
	     obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
    break;

  case ITEM_WEAPON:
    chprintf(ch,
	     "Average Damage     : %d\n\r"
	     "[v0] Weapon class  : %s\n\r"
	     "[v1] Number of dice: [%d]\n\r"
	     "[v2] Type of dice  : [%d]\n\r"
	     "[v3] Attack Type   : %s\n\r"
	     "[v4] Special type  : %s\n\r",
	     ((1 + obj->value[2]) * obj->value[1] / 2),
	     flag_string (weapon_class, obj->value[0]),
	     obj->value[1],
	     obj->value[2],
	     attack_table[obj->value[3]].name,
	     flag_string (weapon_type2, obj->value[4]));

    break;

  case ITEM_CONTAINER:
    chprintf(ch,
	     "[v0] Weight:     [%d kg]\n\r"
	     "[v1] Flags:      [%s]\n\r"
	     "[v2] Key:     %s [%d]\n\r"
	     "[v3] Capacity    [%d]\n\r"
	     "[v4] Weight Mult [%d]\n\r",
	     obj->value[0],
	     flag_string (container_flags, obj->value[1]),
	     get_obj_index (obj->value[2])
	     ? get_obj_index (obj->value[2])->short_descr
	     : "none", obj->value[2], obj->value[3], obj->value[4]);
    break;

  case ITEM_DRINK_CON:
  case ITEM_FOUNTAIN:
    chprintf(ch,
	     "[v0] Liquid Total: [%d]\n\r"
	     "[v1] Liquid Left:  [%d]\n\r"
	     "[v2] Liquid:       %s\n\r"
	     "[v3] Poisoned:     %s\n\r",
	     obj->value[0],
	     obj->value[1],
	     liq_table[obj->value[2]].liq_name,
	     obj->value[3] != 0 ? "Yes" : "No");
    break;

  case ITEM_FOOD:
    chprintf(ch,
	     "[v0] Food hours: [%d]\n\r"
	     "[v1] Full hours: [%d]\n\r"
	     "[v3] Poisoned:   %s\n\r",
	     obj->value[0], obj->value[1], obj->value[3] != 0 ? "Yes" : "No");
    break;

  case ITEM_MONEY:
    chprintf(ch, "[v0] Gold:   [%d]\n\r", obj->value[0]);
    break;

  case ITEM_QUIVER:
    chprintf(ch, "[v0] Number of Arrows:     %d\n\r"
                 "[v1] Number of Dice:       %d\n\r"
                 "[v2] Type of Dice:         %d\n\r", 
				 obj->value[0], obj->value[1], obj->value[2]);
    break;

  case ITEM_ARROW:
    chprintf(ch, "[v1] Number of Dice:       %d\n\r"
                 "[v2] Type of Dice:         %d\n\r", obj->value[1], obj->value[2]);
    break;

  case ITEM_ANGREAL:
    chprintf(ch,
	     "[v0] Type   : %s\n\r"
	     "[v1] Power  : %s\n\r"
	     "[v2] Sex    : %s\n\r",
	     obj->value[0] == 2 ? "Sa'Angreal" : "Angreal",
		 obj->value[1] == -1 ? "Backlash" :
		 obj->value[1] == 0 ? "Drained" :
		 obj->value[1] == 1 ? "Weak" :
		 obj->value[1] == 2 ? "Strong" : "Powerful",
	     obj->value[2] == 1 ? "Male" : "Female");
    break;

  }

  return;
}



bool set_obj_values2 (CHAR_DATA * ch, OBJ_DATA * pObj, int value_num, char *argument)
{
  switch (pObj->item_type)
  {
	default:
	break;

	case ITEM_LIGHT:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_LIGHT");
		  return FALSE;
		case 2:
		  send_to_char ("HOURS OF LIGHT SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
	}
	break;

	case ITEM_POTION:
	case ITEM_HERB:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_HERB_POTION");
		  return FALSE;
		case 0:
		  send_to_char ("WEAVE LEVEL SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("WEAVE TYPE 1 SET.\n\r\n\r", ch);
		  pObj->value[1] = skill_lookup (argument);
		  break;
		case 2:
		  send_to_char ("WEAVE TYPE 2 SET.\n\r\n\r", ch);
		  pObj->value[2] = skill_lookup (argument);
		  break;
		case 3:
		  send_to_char ("WEAVE TYPE 3 SET.\n\r\n\r", ch);
		  pObj->value[3] = skill_lookup (argument);
		  break;
		case 4:
		  send_to_char ("WEAVE TYPE 4 SET.\n\r\n\r", ch);
		  pObj->value[4] = skill_lookup (argument);
		  break;
	}
	break;

	case ITEM_ARMOR:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_ARMOR");
		  return FALSE;
		case 0:
		  send_to_char ("AC PIERCE SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("AC BASH SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("AC SLASH SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
		case 3:
		  send_to_char ("AC EXOTIC SET.\n\r\n\r", ch);
		  pObj->value[3] = atoi (argument);
		  break;
	}
	break;

	case ITEM_WEAPON:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_WEAPON");
		  return FALSE;
		case 0:
		  if (!argument || argument[0] == '\0')
		  {
			show_help (ch, "wclass");
			return FALSE;
		  }
		  send_to_char ("WEAPON CLASS SET.\n\r\n\r", ch);
		  pObj->value[0] = flag_value (weapon_class, argument);
		  break;
		case 1:
		  send_to_char ("NUMBER OF DICE SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("TYPE OF DICE SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
		case 3:
		  if (!argument || argument[0] == '\0')
		  {
			show_damlist (ch);
			return FALSE;
		  }
		  send_to_char ("WEAPON TYPE SET.\n\r\n\r", ch);
		  pObj->value[3] = attack_lookup (argument);
		  break;
		case 4:
		  if (!argument || argument[0] == '\0')
		  {
			show_help (ch, "wtype");
			return FALSE;
		  }
		  send_to_char ("SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch);
		  pObj->value[4] ^= (flag_value (weapon_type2, argument) != NO_FLAG
				 ? flag_value (weapon_type2, argument) : 0);
		  break;
	}
	break;

	case ITEM_PORTAL:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_PORTAL");
		  return FALSE;

		case 0:
		  send_to_char ("CHARGES SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  {
			int value = flag_value (exit_flags, argument);

			if (value != NO_FLAG)
			{
			  send_to_char ("EXIT FLAGS SET.\n\r\n\r", ch);
			  TOGGLE_BIT (pObj->value[1], value);
			  break;
			}
			else
			  send_to_char ("Invalid exit flag(s).\n\r", ch);
		  }
		  break;
		case 2:
		  {
			int value = flag_value (portal_flags, argument);

			if (value != NO_FLAG)
			{
			  send_to_char ("PORTAL FLAGS SET.\n\r\n\r", ch);
			  TOGGLE_BIT (pObj->value[2], value);
			  break;
			}
			else
			  send_to_char ("Invalid portal flag(s).\n\r", ch);
		  }
		  break;
		case 3:
		  send_to_char ("EXIT VNUM SET.\n\r\n\r", ch);
		  pObj->value[3] = atoi (argument);
		  break;
	}
	break;

	case ITEM_WAYGATE:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_WAYGATE");
		  return FALSE;

		case 1:
		  {
			int value = flag_value (exit_flags, argument);

			if (value != NO_FLAG)
			{
			  send_to_char ("EXIT FLAGS SET.\n\r\n\r", ch);
			  TOGGLE_BIT (pObj->value[1], value);
			  break;
			}
			else
			  send_to_char ("Invalid exit flag(s).\n\r", ch);
		  }
		  break;
		case 3:
		  send_to_char ("EXIT VNUM SET.\n\r\n\r", ch);
		  pObj->value[3] = atoi (argument);
		  break;
	}
	break;

	case ITEM_FURNITURE:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_FURNITURE");
		  return FALSE;

		case 0:
		  send_to_char ("NUMBER OF PEOPLE SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("MAX WEIGHT SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("FURNITURE FLAGS TOGGLED.\n\r\n\r", ch);
		  pObj->value[2] ^= (flag_value (furniture_flags, argument) != NO_FLAG
				 ? flag_value (furniture_flags, argument) : 0);
		  break;
		case 3:
		  send_to_char ("HEAL BONUS SET.\n\r\n\r", ch);
		  pObj->value[3] = atoi (argument);
		  break;
		case 4:
		  send_to_char ("OPU BONUS SET.\n\r\n\r", ch);
		  pObj->value[4] = atoi (argument);
		  break;
	}
	break;

	case ITEM_CONTAINER:
	switch (value_num)
	{
 	    int value;

		default:
		  do_help (ch, "ITEM_CONTAINER");
		  return FALSE;
		case 0:
		  send_to_char ("WEIGHT CAPACITY SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  if ((value = flag_value (container_flags, argument)) != NO_FLAG)
		    TOGGLE_BIT (pObj->value[1], value);
		  else
		  {
			do_help (ch, "ITEM_CONTAINER");
			return FALSE;
		  }
		  send_to_char ("CONTAINER TYPE SET.\n\r\n\r", ch);
		  break;
		case 2:
		  if (atoi (argument) != 0)
		  {
			if (!get_obj_index (atoi (argument)))
			{
			  send_to_char ("THERE IS NO SUCH ITEM.\n\r\n\r", ch);
			  return FALSE;
			}

			if (get_obj_index (atoi (argument))->item_type != ITEM_KEY)
			{
			  send_to_char ("THAT ITEM IS NOT A KEY.\n\r\n\r", ch);
			  return FALSE;
			}
		  }
		  send_to_char ("CONTAINER KEY SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
		case 3:
		  send_to_char ("CONTAINER MAX WEIGHT SET.\n\r", ch);
		  pObj->value[3] = atoi (argument);
		  break;
		case 4:
		  send_to_char ("WEIGHT MULTIPLIER SET.\n\r\n\r", ch);
		  pObj->value[4] = atoi (argument);
		  break;
	}
	break;

	case ITEM_DRINK_CON:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_DRINK");
		  return FALSE;
		case 0:
		  send_to_char ("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("LIQUID TYPE SET.\n\r\n\r", ch);
		  pObj->value[2] = (liq_lookup (argument) != -1 ?
				liq_lookup (argument) : 0);
		  break;
		case 3:
		  send_to_char ("POISON VALUE TOGGLED.\n\r\n\r", ch);
		  pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
		  break;
	}
	break;

	case ITEM_FOUNTAIN:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_FOUNTAIN");
		  return FALSE;
		case 0:
		  send_to_char ("MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("LIQUID TYPE SET.\n\r\n\r", ch);
		  pObj->value[2] = (liq_lookup (argument) != -1 ?
				liq_lookup (argument) : 0);
		  break;
		case 3:
		  send_to_char ("POISON VALUE TOGGLED.\n\r\n\r", ch);
		  pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
		  break;
	}
	break;

	case ITEM_FOOD:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_FOOD");
		  return FALSE;
		case 0:
		  send_to_char ("HOURS OF FOOD SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("HOURS OF FULL SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 3:
		  send_to_char ("POISON VALUE TOGGLED.\n\r\n\r", ch);
		  pObj->value[3] = (pObj->value[3] == 0) ? 1 : 0;
		  break;
	}
	break;

	case ITEM_MONEY:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_MONEY");
		  return FALSE;
		case 0:
		  send_to_char ("GOLD AMOUNT SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("SILVER AMOUNT SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
	}
	break;

	case ITEM_QUIVER:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_QUIVER");
		  return FALSE;
		case 0:
		  send_to_char ("NUMBER OF ARROWS SET.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  send_to_char ("NUMBER OF DICE SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("TYPE OF DICE SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
	}
	break;

	case ITEM_ARROW:
	switch (value_num)
	{
		default:
		  do_help (ch, "ITEM_ARROW");
		  return FALSE;

		case 1:
		  send_to_char ("NUMBER OF DICE SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  send_to_char ("TYPE OF DICE SET.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  pObj->value[3] = DAM_PIERCE;
		  break;
	}
	break;

	case ITEM_ANGREAL:
	switch (value_num)
	{
		default:
		  return FALSE;
		case 0:
		  if (atoi (argument) != 1 && atoi (argument) != 2)
		  {
			send_to_char ("TYPE SHOULD BE 1(angreal) or 2(sa'angreal).\n\r\n\r", ch);
			return FALSE;
		  }
		  if (atoi (argument) == 1)
			send_to_char ("TYPE SET TO ANGREAL.\n\r\n\r", ch);
		  else
			send_to_char ("TYPE SET TO SA'ANGREAL.\n\r\n\r", ch);
		  pObj->value[0] = atoi (argument);
		  break;
		case 1:
		  if (atoi (argument) < -1 || atoi (argument) > 3)
		  {
			send_to_char ("POWER CAN BE -1 to 3.\n\r\n\r", ch);
			return FALSE;
		  }
		  if (atoi (argument) == -1)
			send_to_char ("POWER SET TO A BACKLASH ITEM.\n\r\n\r", ch);
		  else if (atoi (argument) == 0)
			send_to_char ("POWER SET TO A DRAINED ITEM.\n\r\n\r", ch);
		  else
			send_to_char ("POWER SET.\n\r\n\r", ch);
		  pObj->value[1] = atoi (argument);
		  break;
		case 2:
		  if (atoi (argument) != 1 && atoi (argument) != 2)
		  {
			send_to_char ("USABLE BY SHOULD BE 1(male) or 2(female).\n\r\n\r", ch);
			return FALSE;
		  }
		  if (atoi (argument) == 1)
			send_to_char ("USABLE BY MALE.\n\r\n\r", ch);
		  else
			send_to_char ("USABLE BY FEMALE.\n\r\n\r", ch);
		  pObj->value[2] = atoi (argument);
		  break;
	}
	break;
  }

  show_obj_values2 (ch, pObj);

  return TRUE;
}
