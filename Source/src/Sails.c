/** 
 * @@ Copyright (The Age of sail Project)
 *
 * The Age of sail (TaoS): Version v0.2 Alpha
 * Copyright (c) 2002, by Owen Varley <owen@interactiontraining.com>
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

// ToS Commands
/*
_DOFUN (do_dock)
{
	OBJ_DATA *boat;

	if (!IS_SET (to_room->room_flags, ROOM_PORT)
	{
		send_to_char("You must be in a port to dock your ship!\n\r", ch);
	    return;
	}
	else
	{
		// Create a boat for the player's ship so other players know who is in port
		boat = create_object(get_obj_index (OBJ_VNUM_BOAT), 0);
		// Put the boat in the port
		obj_to_room (boat, ch->in_room);

		// Set the boat up
		strcpy(boat->name, ch->pcdata->shipname);
		strcpy(boat->short_descr, ch->pcdata->shipname);
		strcpy(boat->description, ch->pcdata->shipname);

		// Move the player to the Port
		

}
 */

_DOFUN (do_roster)
{
	OBJ_DATA *obj;

	 
	if (ch->desc == NULL)
		return;
	
	/*
	* Format the list of objects.
	*/
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->item_type == ITEM_CREW)
		{
			send_to_char("Crew :)\n\r", ch);
		}
		else
		{
			send_to_char("Not Crew?\n\r", ch);
		}

	}

	return;

}
