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
 * @@ Copyright (Jon Booth 'Eod')
 *    Original Author
 * @@ Copyright (Yago Diaz <yago@cerberus.uab.es>)
 *    Ported to Rom 2.4b4
 * @@ Contributors (Bogardan <jordi@cerberus.uab.es>)
 */

/**
 * @@ Objective (mount.c)
 * 
 * Contains the mount, and mount support code. Much of this has been
 * repaired, and much changed from the original. WoT does not support mounting
 * player characters.
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


int mount_success (CHAR_DATA * ch, CHAR_DATA * mount, int canattack)
{
  int percent;
  int success;
  int iSkill = skill_lookup("riding");

  percent = number_percent () + (ch->level < mount->level ?
				 (mount->level - ch->level) * 3 :
				 (mount->level - ch->level) * 2);

  if (is_clan (ch) && IS_FLAG (clan_table[ch->clan].flags, _WHITECLOAK))
    percent += 25;

  if (!IS_NPC (ch) && !str_cmp (ch->pcdata->nation, "Saldaea"))
    percent += 25;

  if (!ch->fighting)
    percent -= 25;

  if (!IS_NPC (ch) && IS_DRUNK (ch))
  {
    percent += ch->pcdata->learned[ iSkill ] / 2;
    send_to_char ("Due to your being under the influence, "
		  "riding seems a bit difficult...\n\r", ch);
  }

  success = percent - ch->pcdata->learned[iSkill];

  if (success <= 0)
  {
    check_improve (ch, iSkill, TRUE, 1);
    return (1);
  }
  else
  {
    check_improve (ch, iSkill, FALSE, 1);

    if (success >= 10 && MOUNTED (ch) == mount)
    {
      act ("You lose control and fall off of $N.",
	   ch, NULL, mount, TO_CHAR, POS_RESTING);
      act ("$n loses control and falls off of $N.",
	   ch, NULL, mount, TO_ROOM, POS_RESTING);
      act ("$n loses control and falls off of you.",
	   ch, NULL, mount, TO_VICT, POS_RESTING);

      ch->riding = FALSE;
      mount->riding = FALSE;

      if (ch->position > POS_STUNNED)
	ch->position = POS_SITTING;

      ch->hit -= 5;
      update_pos (ch);
    }

    if (success >= 40 && canattack)
    {
      act ("$N doesn't like the way you've been treating $M.",
	   ch, NULL, mount, TO_CHAR, POS_RESTING);
      act ("$N doesn't like the way $n has been treating $M.",
	   ch, NULL, mount, TO_ROOM, POS_RESTING);
      act ("You don't like the way $n has been treating you.",
	   ch, NULL, mount, TO_VICT, POS_RESTING);

      act ("$N snarls and attacks you!",
	   ch, NULL, mount, TO_CHAR, POS_RESTING);
      act ("$N snarls and attacks $n!",
	   ch, NULL, mount, TO_ROOM, POS_RESTING);
      act ("You snarl and attack $n!", ch, NULL, mount, TO_VICT, POS_RESTING);

      damage (mount, ch, number_range (1, mount->level),
	      gsn_kick, DAM_BASH, FALSE, TRUE);

    }
  }

  return (0);
}

_DOFUN (do_mount)
{
  char arg[MIL];
  char buf[MIL];
  CHAR_DATA *mount;

  argument = one_argument (argument, arg);

  if (IS_NPC (ch))
    return;

  if (arg[0] == '\0' && ch->mount && ch->mount->in_room == ch->in_room)
  {
    mount = ch->mount;
  }
  else if (!(mount = get_char_room (ch, arg)))
  {
    send_to_char ("Mount what?\n\r", ch);
    return;
  }

  if (!IS_NPC (ch) && !ch->pcdata->learned[ skill_lookup("riding") ])
  {
    send_to_char ("You don't know how to ride!\n\r", ch);
    return;
  }

  if (!IS_NPC (mount) || !IS_FLAG (mount->states.player, _ACT_MOUNT))
  {
    sprintf (buf, "You can't ride that.\n\r");
    send_to_char (buf, ch);
    return;
  }

  if (mount->level - 5 > ch->level)
  {
    send_to_char ("That beast is too powerful for you to ride.", ch);
    return;
  }

  if ((mount->master) && (mount->master != ch))
  {
    sprintf (buf, "%s belongs to %s, not you.\n\r",
	     mount->short_descr, mount->master->name);
    send_to_char (buf, ch);
    return;
  }

  if (mount->position < POS_STANDING)
  {
    send_to_char ("Your mount must be standing.\n\r", ch);
    return;
  }

  if (RIDDEN (mount))
  {
    send_to_char ("This beast is already ridden.\n\r", ch);
    return;
  }
  else if (MOUNTED (ch))
  {
    send_to_char ("You are already riding.\n\r", ch);
    return;
  }

  if (!mount_success (ch, mount, TRUE))
  {
    send_to_char ("You fail to mount the beast.\n\r", ch);
    return;
  }

  act ("You hop on $N's back.", ch, NULL, mount, TO_CHAR, POS_RESTING);
  act ("$n hops on $N's back.", ch, NULL, mount, TO_NOTVICT, POS_RESTING);
  act ("$n hops on your back!", ch, NULL, mount, TO_VICT, POS_RESTING);

  ch->mount = mount;
  ch->riding = TRUE;

  affect_strip (ch, gsn_sneak);
  REMOVE_FLAG (ch->states.affected_by, _AFF_SNEAK);
  affect_strip (ch, gsn_hide);
  REMOVE_FLAG (ch->states.affected_by, _AFF_HIDE);
}

_DOFUN (do_dismount)
{
  CHAR_DATA *mount;

  if (MOUNTED (ch))
  {
    mount = MOUNTED (ch);

    act ("You dismount from $N.", ch, NULL, mount, TO_CHAR, POS_RESTING);
    act ("$n dismounts from $N.", ch, NULL, mount, TO_NOTVICT, POS_RESTING);
    act ("$n dismounts from you.", ch, NULL, mount, TO_VICT, POS_RESTING);

    ch->riding = FALSE;
  }
  else
  {
    send_to_char ("You aren't mounted.\n\r", ch);
    return;
  }
}
