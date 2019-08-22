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
 * @@ Objective (economy.c)
 *
 * This file provides the WoT Mud banking system and support code. Allowing
 * players to deposit/withdraw/transfer funds, and if defined, purchase stock.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

void convert_coinage (CHAR_DATA * ch, int fee)
{
  long gold;
  long silver;

  silver = ch->silver;
  gold = ch->gold;

  if (fee > 0)
    while (silver < fee)
    {
      if (gold < 1)
      {
	send_to_char ("You can't afford the fee!\n\r", ch);
	return;
      }
      gold -= 1;
      silver += 100;
    }
  silver -= fee;

  while (silver >= 100)
  {
    silver -= 100;
    gold += 1;
  }

  ch->gold = UMAX (0, gold);
  ch->silver = UMAX (0, silver);

  send_to_char ("Thank you.\n\r", ch);
  return;
}

_DOFUN (do_bank)
{
  CHAR_DATA *mob;
  char buf[MSL];
  char arg1[MIL];
  char arg2[MIL];

  if (IS_NPC (ch))
  {
    send_to_char ("Banking Services are only available to players!\n\r", ch);
    return;
  }

  /* Check for mob with act->banker */
  for (mob = ch->in_room->people; mob; mob = mob->next_in_room)
  {
    if (IS_NPC (mob) && IS_FLAG (mob->states.player, _ACT_BANKER))
      break;
  }

  if (mob == NULL)
  {
    send_to_char ("You can't do that here.\n\r", ch);
    return;
  }

  if ((time_info.hour < 9) || (time_info.hour > 17))
  {
    send_to_char ("The bank is closed, it is open from 9 to 5.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char ("Bank Options:\n\r\n\r"
		  "Bank balance  : Displays your balance.\n\r"
		  "Bank deposit  : Deposit gold into your account.\n\r"
		  "Bank withdraw : Withdraw gold from your account.\n\r", ch);
	if ( IS_FLAG(pMud.config, _ALLOW_BANK_TRANSFERS) )
	{
		send_to_char ("Bank transfer : Transfer gold to account.\n\r"
			  "Bank buy #    : Buy # shares (in developement)\n\r"
			  "Bank sell #   : Sell # shares (in developement)\n\r"
			  "Bank check    : Check the current rates of the shares. (in developement)\n\r",
			  ch);
	}
    return;
  }

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if (!str_prefix (arg1, "convert"))
  {
    if (ch->silver < 100)
      act ("But... you don't have enough silver to convert to gold.",
	   mob, NULL, ch, TO_VICT, POS_RESTING);
    else
      convert_coinage (ch, ch->silver / 20 /* 5% fee */ );
    return;
  }

  /* Now work out what to do... */
  if (!str_prefix (arg1, "balance"))
  {
    sprintf (buf, "Your current balance is: %ld GP.\n\r",
	     ch->pcdata->balance);
    act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
    return;
  }

  if (!str_prefix (arg1, "deposit"))
  {
    long amount;

    if (is_number (arg2))
    {
      amount = atol (arg2);
      if (amount > ch->gold)
      {
	sprintf (buf,
		 "How can you deposit %ld GP when you"
		 " only have %ld?\n\r", amount, ch->gold);

	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (amount < 0)
      {
	act ("Only positive figures are allowed.",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      ch->gold = UMAX (0, ch->gold - amount);
      ch->pcdata->balance = UMAX (0, ch->pcdata->balance + amount);

      sprintf (buf,
	       "You deposit %ld GP.  Your new balance is %ld GP.\n\r",
	       amount, ch->pcdata->balance);

      send_to_char (buf, ch);
      return;
    }
  }

  /* We only allow transfers if this is true... so define it... */

  if (IS_FLAG(pMud.config, _ALLOW_BANK_TRANSFERS) && !str_prefix (arg1, "transfer"))
  {
    long amount;
    CHAR_DATA *victim;

    if (is_number (arg2))
    {
      amount = atol (arg2);
      if (amount > ch->pcdata->balance)
      {
	sprintf (buf, "How can you transfer %ld GP when your balance"
		 " is %ld?\n\r", amount, ch->pcdata->balance);

	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (amount < 0)
      {
	act ("Only positive figures are allowed.",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (!(victim = get_char_world (ch, argument)))
      {
	sprintf (buf, "%s doesn't have a bank account.\n\r", argument);
	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (IS_NPC (victim))
      {
	act ("You can only transfer money to players.",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      ch->pcdata->balance = UMAX (0, ch->pcdata->balance - amount);
      victim->pcdata->balance = UMAX (0, victim->pcdata->balance + amount);

      sprintf (buf, "You transfer %ld GP. Your new balance is"
	       " %ld GP.\n\r", amount, ch->pcdata->balance);

      send_to_char (buf, ch);
      sprintf (buf,
	       "[BANK] %s has transferred %ld gold's to your account.\n\r",
	       ch->name, amount);
      send_to_char (buf, victim);
      return;
    }
  }

  if (!str_prefix (arg1, "withdraw"))
  {
    long amount;

    if (is_number (arg2))
    {
      amount = atol (arg2);
      if (amount > ch->pcdata->balance)
      {
	sprintf (buf, "How can you withdraw %ld GP when your"
		 " balance is %ld?\n\r", amount, ch->pcdata->balance);
	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (amount < 0)
      {
	act ("Only positive figures are allowed.",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      ch->pcdata->balance = UMAX (0, ch->pcdata->balance - amount);
      ch->gold = UMAX (0, ch->gold + amount);

      sprintf (buf, "You withdraw %ld GP.  Your new balance is %ld GP.\n\r",
	       amount, ch->pcdata->balance);
      send_to_char (buf, ch);
      return;
    }
  }

  /* If you want to have an invest option... define BANK_INVEST */
  if (IS_FLAG(pMud.config, _ALLOW_BANK_INVESTING) && !str_prefix (arg1, "buy"))
  {
    long amount;

    if (is_number (arg2))
    {
      amount = atol (arg2);
      pMud.share_value = UMAX (10, pMud.share_value);

      if ((amount * pMud.share_value) > ch->pcdata->balance)
      {
	sprintf (buf, "%ld shares will cost you %ld, get more money.\n\r",
		 amount, (amount * pMud.share_value));
	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (amount < 0)
      {
	act ("If you want to sell shares you have to say so...",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      ch->pcdata->balance =
	UMAX (0, ch->pcdata->balance - (amount * pMud.share_value));
      ch->pcdata->shares = UMAX (0, ch->pcdata->shares + amount);

      sprintf (buf,
	       "You buy %ld shares for %ld GP, you now have %ld shares.\n\r",
	       amount, (amount * pMud.share_value), ch->pcdata->shares);
      act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
      return;
    }
  }

  if (!str_prefix (arg1, "sell"))
  {
    long amount;

    if (is_number (arg2))
    {
      amount = atol (arg2);
      if (amount > ch->pcdata->shares)
      {
	sprintf (buf, "You only have %ld shares.\n\r", ch->pcdata->shares);
	act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      if (amount < 0)
      {
	act ("If you want to buy shares you have to say so...",
	     mob, NULL, ch, TO_VICT, POS_RESTING);
	return;
      }

      ch->pcdata->balance =
	UMAX (0, ch->pcdata->balance + (amount * pMud.share_value));
      ch->pcdata->shares = UMAX (0, ch->pcdata->shares - amount);

      sprintf (buf,
	       "You sell %ld shares for %ld GP, you now have %ld shares.\n\r",
	       amount, (amount * pMud.share_value), ch->pcdata->shares);
      act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
      return;
    }
  }

  if (!str_prefix (arg1, "check"))
  {
    pMud.share_value = UMAX (0, pMud.share_value);

    sprintf (buf, "The current shareprice is %d.\n\r", pMud.share_value);
    act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);

    if (ch->pcdata->shares)
    {
      sprintf (buf,
	       "You currently have %ld shares, (%d a share)"
	       " worth a total of %ld gold.\n\r",
	       ch->pcdata->shares, pMud.share_value,
	       (ch->pcdata->shares * pMud.share_value));
      act (buf, mob, NULL, ch, TO_VICT, POS_RESTING);
    }
    return;
  }

  act ("I don't know what you mean.", mob, NULL, ch, TO_VICT, POS_RESTING);
  do_bank (ch, "");		/* Generate Instructions */
  return;
}
