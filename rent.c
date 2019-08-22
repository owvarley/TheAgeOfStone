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
 * @@ Copyright(Rent box / Locker code by Lobbin <rnen@algonet.se>)
 *									   
 * @@CrNote (WoT Project Credit Note)
 *   While the author of this code was kind enough to allow us to use it
 * in our distribution, we dont feel it's fair to bombard them with questions
 * concerning our use of their code since this snippet was written for a mud
 * base other than WoT.  Therefore, please direct all questions or bug reports
 * concerning this snippet to wot@tarmongaidon.org
 */

/**
 * @@ Objective (rent.c)
 * 
 * This file contains code for allowing players to deposit items into a 
 * 'safety deposit box', and also to withdraw such items later.
 */


#include <sys/types.h>
//#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "merc.h"


_DOFUN (do_rentbox)
{
  CHAR_DATA *renter = NULL;

  for (renter = ch->next_in_room; renter; renter = renter->next_in_room)
  {
    if (IS_NPC (renter) && IS_FLAG (renter->states.player, _ACT_BANKER))
      break;
  }

  if (!renter)
  {
    send_to_char ("The Banker is currently not available.\n\r", ch);
    return;
  }

  if (ch->pcdata->boxrented == 0)
  {
    if (player_silver (ch) < pMud.box_cost)
    {
      send_to_char ("You can't afford to rent a box.\n\r", ch);
      return;
    }
    else
    {
      amt_from_player (ch, pMud.box_cost);
      send_to_char ("You have rented you very own box.\n\r", ch);
      ch->pcdata->boxrented = 1;
      return;
    }
  }
  else
    send_to_char ("You allready have a box rented!\n\r", ch);

}


_DOFUN (do_deposit)
{
  char buf[MSL];
  OBJ_DATA *obj;

  if (argument[0] == '\0')
  {
    send_to_char ("Deposit what into the box?\n\r", ch);
    return;
  }

  if (ch->pcdata->item_box >= MAX_STORAGE)
  {
    send_to_char ("How do you expect that to fit in there?\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry (ch, argument, ch)) == NULL)
  {
    send_to_char ("You do not have that item.\n\r", ch);
    return;
  }

  if (!can_drop_obj (ch, obj))
  {
    sprintf (buf, "You can't let go of it.\n\r");
    send_to_char (buf, ch);
    return;
  }

  obj_from_char (obj);
  obj_to_box (obj, ch);
  act ("$n puts $p in $s box.", ch, obj, NULL, TO_ROOM, POS_RESTING);
  act ("You put $p in your box.", ch, obj, NULL, TO_CHAR, POS_RESTING);
}


_DOFUN (do_withdraw)
{
  OBJ_DATA *obj;

  if (argument[0] == '\0')
  {
    send_to_char ("Withdraw what from your box?\n\r", ch);
    return;
  }

  obj = get_obj_list (ch, argument, ch->pcdata->box);

  if (obj == NULL)
  {
    act ("I see no $T here.", ch, NULL, argument, TO_CHAR, POS_RESTING);
    return;
  }

  obj_from_box (obj);
  obj_to_char (obj, ch);

  act ("$n gets $p from his box.", ch, obj, NULL, TO_ROOM, POS_RESTING);
  act ("You get $p from your box.", ch, obj, NULL, TO_CHAR, POS_RESTING);
}

_DOFUN (do_inventory_box)
{
  if (ch->pcdata->boxrented == 0)
  {
    send_to_char ("You dont have a box rented.\n\r", ch);
    return;
  }

  send_to_char ("Your box contains:\n\r", ch);
  show_list_to_char (ch->pcdata->box, ch, TRUE, TRUE);
}

_DOFUN (do_box)
{
  char arg1[MIL];

  argument = one_argument (argument, arg1);

  if (IS_NPC (ch))
  {
    send_to_char ("Huh?\n\r", ch);
    return;
  }

  if (!IS_SET (ch->in_room->room_flags, ROOM_DEPOSITORY))
  {
    send_to_char ("You can't do that here.\n\r", ch);
    return;
  }

  if (!str_prefix (arg1, "inventory"))
    do_inventory_box (ch, "");
  else if (!str_prefix (arg1, "deposit"))
    do_deposit (ch, argument);
  else if (!str_prefix (arg1, "withdraw"))
    do_withdraw (ch, argument);
  else if (!str_prefix (arg1, "rent"))
    do_rentbox (ch, argument);
  else
    send_to_char ("Options are: rent, deposit, withdraw, inventory.\n\r", ch);

}

void obj_to_box (OBJ_DATA * obj, CHAR_DATA * ch)
{
  obj->next_content = ch->pcdata->box;
  ch->pcdata->box = obj;
  ch->pcdata->item_box++;
  obj->carried_by = ch;
  obj->in_room = NULL;
  obj->in_obj = NULL;
}


void obj_from_box (OBJ_DATA * obj)
{
  CHAR_DATA *ch;

  if ((ch = obj->carried_by) == NULL)
  {
    bugf ("[rent.c::obj_from_box] null carried_by.");
    return;
  }

  if (ch->pcdata->box == obj)
    ch->pcdata->box = obj->next_content;
  else
  {
    OBJ_DATA *prev;

    for (prev = ch->pcdata->box; prev != NULL; prev = prev->next_content)
    {
      if (prev->next_content == obj)
      {
	prev->next_content = obj->next_content;
	break;
      }
    }

    if (prev == NULL)
      bugf ("[rent.c::obj_from_box] obj not in list.");
  }

  obj->carried_by = NULL;
  obj->next_content = NULL;
  ch->pcdata->item_box--;
  return;
}
