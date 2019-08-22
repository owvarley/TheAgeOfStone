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
 * @@ Objective (auction.c)
 * 
 * To provide an auction system that can handle more than one item for sale at once. With this
 * system, each player can auction as many items at once as they like. I've not included any
 * limits here, though I can see how they may become necessary if this becomes very popular.
 *
 * Thanks to Erwin for his parse_bet function.
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "merc.h"


#define ALL     -1
#define SUMMARY 0
#define DETAIL  1


/** Function: announce_auction
  * Descr   : Displays a string to the entire mud. Anyone with the auction channel active
  *         : should see it.
  * Returns : n/a
  * Syntax  : announc_auction( text )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
void announce_auction (char *text)
{
  DESCRIPTOR_DATA *d, *d_next;
  CHAR_DATA *ch;

  for (d = descriptor_list; d != NULL; d = d_next)
  {
    d_next = d->next;

    ch = d->original ? d->original : d->character;

    if (d->connected == CON_PLAYING
	&& IS_FLAG (ch->states.comm, _AUCTION)
	&& !IS_FLAG (ch->states.config, _QUIET))
	{
      chprintf (ch, "[%s] %s%s`w", AUCTIONH, AUCTIONC, text);
	}
  }
}

/** Function: is_valid_auction
  * Descr   : Given an auction #, determines if that auction has expired or not
  * Returns : the auction data corresponding to the auction #
  * Syntax  : is_valid_auction( auction number in question )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
AUCTION_DATA *is_valid_auction (int number)
{
  AUCTION_DATA *pAuc;

  for (pAuc = house_auctions; pAuc != NULL; pAuc = pAuc->next)
    if (pAuc && !pAuc->expired && pAuc->auction_number == number)
      return pAuc;

  return NULL;
}


/** Function: number_bids
  * Descr   : Returns the number of valid bids a particular auction has received.
  * Returns : see description
  * Syntax  : number_bids( auction data in question )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
int number_bids (AUCTION_DATA * auction)
{
  int count = 0;
  BIDDER_DATA *bidders;

  for (bidders = auction->bidders; bidders != NULL; bidders = bidders->next)
    if (bidders->opening_bid > 0)
      count++;

  return count;
}

/** Function: winning_bidder
  * Descr   : Determines the current (if any) winning bidder of a given auction
  * Returns : Bidder data of winner
  * Syntax  : winning_bidder( auction data in question )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
BIDDER_DATA *winning_bidder (AUCTION_DATA * auction)
{
  BIDDER_DATA *winner = NULL;
  BIDDER_DATA *bidder = NULL;
  long last = 0;

  for (bidder = auction->bidders; bidder != NULL; bidder = bidder->next)
  {
    if (!bidder)
      break;

    if (bidder->max_bid > last && bidder->max_bid >= auction->reserve_price)
      winner = bidder;

    last = bidder->max_bid;
  }

  return winner;
}

/** Function: minimum_bid
  * Descr   : Determines the minimum valid bid on an ongoing auction
  * Returns : Minimum bid required to bid on this item
  * Syntax  : minimum_bid( auction data in question )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
long minimum_bid (AUCTION_DATA * auction)
{
  if (auction->high_bid < auction->opening_price)
    return auction->opening_price;
  else
    return auction->high_bid + auction->bid_increment;
}


/** Function: find_bidder
  * Descr   : Determines if a given player (CH) has bid on a particular auction or not
  * Returns : TRUE if 'ch' is bidding on auction in question, FALSE if not
  * Syntax  : find_bidder( ch in question, auction in question )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
bool find_bidder (CHAR_DATA * ch, AUCTION_DATA * auction)
{
  BIDDER_DATA *bidder = NULL;

  for (bidder = auction->bidders; bidder != NULL; bidder = bidder->next)
  {
    if (!bidder)
      break;

    if (bidder->who == ch)
      return TRUE;
  }

  return FALSE;
}


/** Function: show_auctions
  * Descr   : Displays output of all current auctions to player.
  * Returns : n/a
  * Syntax  : show_auctions( to who, auction # [detail] )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
void show_auctions (CHAR_DATA * ch, char *argument)
{
  AUCTION_DATA *pAuc, *pAuc_next;
  CHAR_DATA *whose = NULL;
  int Count = 0;
  int AucID = -1;
  char arg1[MIL], arg2[MIL];
 
  char *header =
    "//-| Auctions |-----------------------------------------------------------//\n\r";
  
  char *seperator =
    "//------------------------------------------------------------------------//\n\r";

  bool Detail = FALSE;
  bool iBids = FALSE;
  bool iWins = FALSE;
  bool iLoss = FALSE;
  bool iAll = FALSE;


  memset (arg1, 0, MIL);
  memset (arg2, 0, MIL);

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  if (arg1[0] == '#')
  {
    AucID = atoi (arg2);
    if (AucID < 0)
    {
      send_to_char ("Invalid auction number.\n\r", ch);
      return;
    }
  }
  else
   if (!str_prefix (arg1, "detail") || !str_prefix (arg2, "detail"))
    Detail = TRUE;

  if (!str_prefix (arg1, "bids"))
    iBids = TRUE;
  else if (!str_prefix (arg1, "winning"))
    iWins = TRUE;
  else if (!str_prefix (arg1, "losing"))
    iLoss = TRUE;
  else if (!str_prefix (arg1, "all"))
    iAll = TRUE;
  else
    whose = get_char_world (ch, arg1);


  /* header... */
  send_to_char (header, ch);

  for (pAuc = house_auctions; pAuc != NULL; pAuc = pAuc_next)
  {
    pAuc_next = pAuc->next;

    if (pAuc && pAuc->seller && !pAuc->expired)
    {
      char flags[100];
      BIDDER_DATA *winner = NULL;

      winner = winning_bidder (pAuc);

      if (!iAll)
      {
		if (AucID > 0 && pAuc->auction_number != AucID)
		  continue;
		else if (iWins && (winner && winner->who != ch))
		  continue;
		else
		  if (iLoss && (find_bidder (ch, pAuc) && (winner && winner->who != ch)))
		  continue;
		else if (iBids && !find_bidder (ch, pAuc))
		  continue;
		else if (whose && pAuc->seller != whose)
		  continue;
      }

      Count++;

      sprintf (flags, "%s`w%s`w%s`w",
		  IS_OBJ_STAT(pAuc->item, ITEM_QUEST)  ? "`RQ" : "-",
	       (pAuc->reserve_price > pAuc->high_bid) ? "`GR" : "-",
	       (minimum_bid (pAuc) > player_silver (ch)) ? "`YG" : "-");

      chprintf (ch,
		      "// %-3d %-15.15s %-41.41s `CO:%6d`w //\n\r"
		      "// %s %-3d %-44.44s `YM:%6d `RH:%6d`w //\n\r%s",
		      pAuc->auction_number,
		      PERS (pAuc->seller, ch),
		      strip_codes (pAuc->item->short_descr),
		      pAuc->opening_price,
		      flags,
		      number_bids (pAuc),
		      (winner
		       && winner->who) ? PERS (winner->who,
					       ch) : "No Winners yet",
		      minimum_bid (pAuc), pAuc->high_bid, seperator);

      if (Detail)		/* Show item detail */
      {
		(*skill_table[ skill_lookup("delve") ].weave_fun)
		  (0, LEVEL_HERO - 1, ch, pAuc->item, 0, "", TRUE);
		
		send_to_char (seperator, ch);
      }

    }

  }

  /* footer... */
  chprintf (ch,
		  "//-| Total %d -------------------------------------------------------------//\n\r",
		  Count);
}

/** Function: log_auction
  * Descr   : Logs auction data to AUCTION_FILE log
  * Returns : n/a
  * Syntax  : log_auction( auction data )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
void log_auction (AUCTION_DATA * pAuc)
{
  FILE *fp = NULL;
  char *strtime;
  char fname[MIL];

  sprintf (fname, "%s%s", LOG_DIR, AUCTION_FILE);

  if (!(fp = fopen (fname, "a")))
  {
      bugf("[auction.c::log_auction] [%s] Could not open data file %s.",
	  errmsg (errno), fname);
    return;
  }

  strtime = ctime (&current_time);
  strtime[strlen (strtime) - 1] = '\0';

  fprintf (fp, "%s | [v: %d] [a: %ld] [p: %d] %s was sold by %s to %s for $%ld gold.\n",
	   strtime,
	   pAuc->item->pIndexData->vnum,
	   pAuc->auction_id,
	   eq_cost(pAuc->item),
	   strip_codes (pAuc->item->short_descr),
	   pAuc->seller->name,
	   winning_bidder (pAuc)->who->name, pAuc->high_bid);

  fclose (fp);
}

/** Function: auction_update
  * Descr   : Updates each item currently being sold
  * Returns : n/a
  * Syntax  : void
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
void auction_update (void)
{
  AUCTION_DATA *pAuc;
  char buf[MSL];

  for (pAuc = house_auctions; pAuc != NULL; pAuc = pAuc->next)
  {
    if (pAuc->item != NULL && !pAuc->expired)
    {

      switch (++pAuc->going)
      {
		  case 1:
		  case 2:
			{
			  if (pAuc->high_bid > 0)
				sprintf (buf, "#%3d] '%25.25s'%s going %s for %ld gold.\n\r",
					 pAuc->auction_number,
					 pAuc->item->short_descr,
					 AUCTIONC,
					 pAuc->going == 1 ? "once" : "twice", pAuc->high_bid);
			  else
				sprintf (buf, "#%3d] '%25.25s'%s has no bids yet.\n\r",
					 pAuc->auction_number, pAuc->item->short_descr, AUCTIONC);

			  announce_auction (buf);
			  break;
			}
		  case 3:
			{
			  pAuc->expired = TRUE;
			  pAuc->time_closed = current_time;

			  if (pAuc->high_bid > 0)
			  {
				CHAR_DATA *winner = NULL;

				if (winning_bidder (pAuc) != NULL)
				  winner = winning_bidder (pAuc)->who;

				if (!winner)
				{
				  sprintf (buf,
					   "#%3d] '%25.25s'%s no bids met the reserve price. Auction over.\n\r",
					   pAuc->auction_number, pAuc->item->short_descr,
					   AUCTIONC);

				  announce_auction (buf);

				  obj_to_char (pAuc->item, pAuc->seller);

				  act("An auctioneers aide appears before you and returns your $p",
					pAuc->seller, pAuc->item, NULL, TO_CHAR, POS_RESTING);
				  act ("An auctioneers aid appears before $n, and hands $m $p",
				    pAuc->seller, pAuc->item, NULL, TO_ROOM, POS_RESTING);

				  return;
				}

				sprintf (buf, "#%3d] '%25.25s'%s sold to %s for %ld gold!\n\r",
					 pAuc->auction_number,
					 pAuc->item->short_descr,
					 AUCTIONC, winner->name, pAuc->high_bid);

				announce_auction (buf);
				obj_to_char (pAuc->item, winner);

				act ("An auctioneers aide appears before you and hands you $p",
				 winner, pAuc->item, NULL, TO_CHAR, POS_RESTING);
				act ("An auctioneers aid appears before $n, and hands $m $p",
				 winner, pAuc->item, NULL, TO_ROOM, POS_RESTING);

				amt_to_player (winner, (long)(pAuc->high_bid * .90));
				log_auction (pAuc);
			  }
			  else
			  /* not sold! */
			  {
				sprintf (buf,
					 "No bids were received for auction #%d, item removed.\n\r",
					 pAuc->auction_number);

				announce_auction (buf);

				act ("An auctioneers aid appears before you and returns your $p",
				 pAuc->seller, pAuc->item, NULL, TO_CHAR, POS_RESTING);
				act ("An auctioneers aid appears before $n, and hands $m $p.",
				 pAuc->seller, pAuc->item, NULL, TO_ROOM, POS_RESTING);

				obj_to_char (pAuc->item, pAuc->seller);
			  }
			}
	  }	/* switch */
    } /* if valid auction */
  }	/* end: for loop through auctions */
}


/** Function: outbid
  * Descr   : Determines if a bidders bid on a particular auction is outbid or not.
  *			: Updates proxy bids appropriately.
  * Returns : TRUE if the players bid was outbid by another (via proxy)
  * Syntax  : outbid(who's bidding, on which auction item, how much is the bid)
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
bool outbid (CHAR_DATA * ch, AUCTION_DATA * auction, long bid)
{
  bool winner = TRUE;
  BIDDER_DATA *bids;

  for (bids = auction->bidders; bids != NULL; bids = bids->next)

    if (bids->max_bid > bid && bids->max_bid >= auction->high_bid)
    {
      /* Ok, someone elses proxy bid is higher than ours, so update their
       * bids as well... */
      winner = FALSE;

      if (bid + auction->bid_increment <= bids->max_bid)
		bids->current_bid = bid + auction->bid_increment;
      else
		bids->current_bid = bids->max_bid;

      if (bids->current_bid > auction->high_bid)
		auction->high_bid = bids->current_bid;

      bids->time_last_bid = current_time;
    }

  return !(winner);
}

/** Function: add_bidder
  * Descr   : Adds a bidder (or updates his data) to an auction
  * Returns : n/a
  * Syntax  : add_bidder(bidder, auction bidding on, amount of bid)
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
void add_bidder (CHAR_DATA * ch, AUCTION_DATA * auction, long bid)
{
  BIDDER_DATA *bidder;

  /* is this character allready on the bidder list? */
  for (bidder = auction->bidders; bidder != NULL; bidder = bidder->next)
    if (bidder->who == ch)
    {
      /* Yep, allready here...  update data */
      if (bidder->max_bid < bid)
		bidder->max_bid = bid;

      bidder->current_bid = bid;
      bidder->time_last_bid = current_time;

      if (bidder->time_opening_bid == 0)
		bidder->time_opening_bid = current_time;

      return;
    }

  /* no, not here eh? */
  bidder = new_bidder ();
  bidder->who = ch;
  bidder->max_bid = bid;
  bidder->current_bid = bid;
  bidder->opening_bid = bid;
  bidder->time_last_bid = current_time;
  bidder->time_opening_bid = current_time;

  bidder->next = auction->bidders;
  auction->bidders = bidder;
}


/** Function: do_auction
  * Descr   : Main auction command parser. See details below
  * Returns : n/a
  * Syntax  : do_auction( see below )
  * Written : v1.0 4/00
  * Author  : Gary McNickle <rhuarc@tarmongaidon.org>
  */
_DOFUN (do_auction)
{
  AUCTION_DATA *pAuc = NULL;
  char arg[MIL];

  /* Syntax
   *
   * show         Defaults to detail of last item bid on, or all if no bids
   *    - #       Show detail of the specified item
   *    - detail  Show detail for all items (default is summary)        
   *    - bids    Show detail for all items currently bidding on
   *    - winning Show detail for all items currently winning bids on
   *    - losing  Show detail for all items currently losing bids on
   *    - 'name'  Show all auctions by a specific person (detail)
   *    - all     Show summary of all auctions (default action)
   *
   *  sell [item] Put an item up for auction
   *    - reserve Set the reserve price, if any
   *    - # [#]   How many of these items are up for sale?
   *    - open    Set the opening bid
   *    - inc     Set the minimum bid increment
   *    - commit  Commit the item to sale
   *
   *  bid <#> <amt>
   *    
   *  cancel/stop
   */


  if (IS_NPC (ch))
  {
    send_to_char ("Mobs may not participate in auctions.\n\r", ch);
    return;
  }

  argument = one_argument (argument, arg);

  if (ch->desc->editor != ED_NONE 
	  && (ch->desc->editor != ED_AUCTION && !str_prefix (arg, "stop")))
  {
    send_to_char ("Please leave OLC before participating in an auction.\n\r", ch);
    return;
  }

  if (!arg || arg[0] == '\0' || !str_prefix (arg, "show"))
  {
    show_auctions (ch, argument);
    return;
  }

  /* Otherwise, parse the arguments... */
  if (!str_cmp (arg, "on"))
  {
    if (!IS_FLAG (ch->states.comm, _AUCTION))
      SET_FLAG (ch->states.comm, _AUCTION);

    send_to_char ("Your auction channel is now ON.\n\r", ch);
    return;
  }
  else if (!str_cmp (arg, "off"))
  {
    if (IS_FLAG (ch->states.comm, _AUCTION))
      REMOVE_FLAG (ch->states.comm, _AUCTION);

    send_to_char ("Your auction channel is now OFF.\n\r", ch);
    return;
  }

  while (arg[0] != '\0')
  {

    if (!str_prefix (arg, "help"))
    {
      do_help (ch, "auction");
      return;
    }

    if (!str_prefix (arg, "bid"))
    {
      int auc = 0;
      long bid = 0;
      long min = 0;

      argument = one_argument (argument, arg);

      if (!arg || (auc = atoi (arg)) < 0 || !is_number (arg))
      {
		send_to_char("And just which auction would you like to bid on PLEASE?\n\r", ch);
		return;
      }

      if ((pAuc = is_valid_auction (auc)) == NULL)
      {
		send_to_char
		  ("I cant seem to find that item, how much did you say you would give for it? *grin*\n\r",
		   ch);
		return;
      }

      if (pAuc->seller == ch)
      {
		send_to_char
		  ("Sorry, but sellers are forbidden to bid on their own auctions!\n\r",ch);
		return;
      }

      argument = one_argument (argument, arg);

      bid = parsebet (pAuc->high_bid, arg);
      min = minimum_bid (pAuc);

      if (bid == 0)
      {
		send_to_char ("Hey, we don't just GIVE things away here!\n\r", ch);
		return;
      }
      else
       if (bid < min)
      {
		chprintf (ch,
				"The minimum bid for this item is %d %s, bid higher!\n\r",
				min, (min > 100) ? "gold" : "silver");
		return;
      }

      if (bid > player_silver (ch))
      {
		send_to_char ("You dont have that much on you!\n\r", ch);
		return;
		  }

      /* Check proxies... */
      if (!outbid (ch, pAuc, bid))
      {
		chprintf (ch, "YOU have the highest bid for %s!\n\r",
				pAuc->item->short_descr);

		if (pAuc->reserve_price > bid)
		  send_to_char ("Your bid does not meet the reserve price.\n\r", ch);
		else
 		if (pAuc->reserve_price > 0 && pAuc->reserve_price <= bid)
		  send_to_char ("Your bid meets or exceeds the reserve price!\n\r", ch);

		/* Only add successful bidders to bidder list */
		add_bidder (ch, pAuc, bid);
		pAuc->high_bid = bid;
		pAuc->going = 0;
		amt_from_player (ch, bid);

		return;
      }
      else;

      send_to_char ("Syntax error, see help auction.\n\r", ch);
      return;

    } /* end: bid */

    if (!str_prefix (arg, "reserve"))
    {
      long price = 0;

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;
      if (!pAuc || ch->desc->editor != ED_AUCTION)
      {
		send_to_char ("You must begin an auction first.\n\r", ch);
		return;
      }

      argument = one_argument (argument, arg);

      price = parsebet (pAuc->reserve_price, arg);
      if (price < 1)
      {
		send_to_char ("Sell it for HOW much? *bog*\n\r", ch);
		return;
      }
      if (price < pAuc->opening_price)
      {
		send_to_char
		  ("Reserve prices must be higher than opening prices!\n\r", ch);
		return;
      }

      pAuc->reserve_price = price;
      chprintf (ch, "O.K. %s will sell for a minimum of %d %s\n\r",
		      pAuc->item->short_descr,
		      pAuc->reserve_price,
		      (pAuc->reserve_price > 99) ? "gold" : "silver");
      return;

    }

    else if (!str_prefix (arg, "open"))
    {
      long price = 0;

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;
      if (!pAuc || ch->desc->editor != ED_AUCTION)
      {
		send_to_char ("You must begin an auction first.\n\r", ch);
		return;
      }

      argument = one_argument (argument, arg);

      price = parsebet (pAuc->opening_price, arg);

      if (price < 1)
      {
		send_to_char ("Start the bidding for HOW much? *bog*\n\r", ch);
		return;
      }

      pAuc->opening_price = price;
      chprintf (ch, "O.K. %s will begin selling for %d %s\n\r",
		      pAuc->item->short_descr,
		      pAuc->opening_price,
		      (pAuc->opening_price > 99) ? "gold" : "silver");

      return;
    }				/* end: open */

    else if (!str_prefix (arg, "increment"))
    {
      long inc = 0;

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;
      if (!pAuc || ch->desc->editor != ED_AUCTION)
      {
		send_to_char ("You must begin an auction first.\n\r", ch);
		return;
      }

      argument = one_argument (argument, arg);

      inc = parsebet (pAuc->bid_increment, arg);

      if (inc <= 0)
      {
		send_to_char ("Increment the bidding by HOW much? *bog*\n\r", ch);
		return;
      }

      pAuc->bid_increment = inc;
      chprintf (ch, "O.K. %s will accept bid increments of +%d %s\n\r",
		      pAuc->item->short_descr,
		      pAuc->bid_increment,
		      (pAuc->bid_increment > 99) ? "gold" : "silver");

      return;
    }				/* end: inc */

    else
     if (!str_prefix (arg, "commit"))
    {

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;
      if (!pAuc || ch->desc->editor != ED_AUCTION)
      {
		send_to_char ("You must begin an auction first.\n\r", ch);
		return;
      }

      if (pAuc->opening_price <= 0)
      {
		send_to_char
		  ("Let me get this straight... you want me to GIVE this away?\n\r",
		   ch);
		return;
      }

      pAuc->expired = FALSE;

      pAuc->next = house_auctions;
      house_auctions = pAuc;

      ch->desc->pEdit = NULL;
      ch->desc->editor = ED_NONE;

      send_to_char ("Your item is now on the block!\n\r", ch);
      return;

    }				/* end: commit */

    else if (!str_cmp ("#", arg))
    {
      long value = 0;
      char buf[MIL];

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;
      if (!pAuc || ch->desc->editor != ED_AUCTION)
      {
		send_to_char ("You must begin an auction first.\n\r", ch);
		return;
      }

      argument = one_argument (argument, arg);

      if (!arg || (value = atol (arg)) < 0 || !is_number (arg))
      {
		send_to_char ("HOW many are you selling?\n\r", ch);
		return;
      }

      sprintf (buf, "%ld.%s", value, pAuc->item->short_descr);

      if (!get_obj_carry (ch, buf, ch))
      {
		send_to_char ("But you don't have that many!\n\r", ch);
		return;
      }

      pAuc->number_for_sale = (short)value;

      value = (pAuc->reserve_price > 0) ? pAuc->reserve_price : pAuc->opening_price;

      chprintf (ch,
		      "O.K. You are selling %d %s's for a minimum of %d %s each.\n\r",
		      pAuc->number_for_sale, pAuc->item->short_descr,
		      (value) ? "gold" : "silver");

      return;
    }

    else if (!str_prefix (arg, "clear"))
    {
      pAuc = (AUCTION_DATA *) ch->desc->pEdit;

      if (!pAuc)
      {
		send_to_char ("Uh huh, sure thing.\n\r", ch);
		return;
      }

      if (pAuc->item != NULL)
      {
		chprintf (ch, "Your %s has been returned to you.\n\r",
				pAuc->item->short_descr);

		obj_to_char (pAuc->item, pAuc->seller);
      }

      free_auction (pAuc);
      ch->desc->pEdit = NULL;
      send_to_char ("Auction data cleared.\n\r", ch);
      return;
    }

    else if (!str_prefix ("sell", arg))
    {
      OBJ_DATA *item = NULL;
      char arg2[MIL], arg3[MIL];
      long open = -1;

      pAuc = (AUCTION_DATA *) ch->desc->pEdit;

      if (pAuc != NULL)
      {
		send_to_char ("Will you PLEASE decide what you want to sell?\n\r",
				  ch);
		return;
      }

      memset (arg2, 0, MIL);
      memset (arg3, 0, MIL);

      argument = one_argument (argument, arg2);
      argument = one_argument (argument, arg3);

      if ((item = get_obj_list (ch, arg2, ch->carrying)) == NULL)
      {
		send_to_char ("You aren't carrying that!\n\r", ch);
		return;
      }

      if (!IS_NULLSTR (arg3) && is_number (arg3))
		open = parsebet (0, arg3);

      if (IS_SET (item->extra_flags, ITEM_GUILD) )
      {
		send_to_char ("Guild items may not be auctioned, shame on you!\n\r", ch);
		return;
      }
      if (IS_SET (item->extra_flags, ITEM_GHOST) )
      {
		send_to_char ("Items found in the Dreamworld may not be auctioned.\n\r", ch);
		return;
      }
      else
      if (IS_SET (item->wear_flags, ITEM_WEAR_TATTOO))
      {
		send_to_char ("How do you propose to remove THAT?\n\r", ch);
		return;
      }
      else
       if (IS_SET (item->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL (ch))
      {
		act ("You can't remove $p.", ch, item, NULL, TO_CHAR, POS_RESTING);
		return;
      }

      /* Here is where we assign the actual auction item... */
      pAuc = new_auction ();

      pAuc->seller = ch;
      pAuc->item = item;
      pAuc->auction_id = get_auction_id ();

      if (open > 0)
		pAuc->opening_price = open;

      ch->desc->pEdit = (void *) pAuc;
      ch->desc->editor = ED_AUCTION;

      obj_from_char (item);

      if (open > 0)
		chprintf (ch,
			"Ok, I'll start the bidding for %s at $%d gold.\n\r",
			pAuc->item->short_descr, pAuc->opening_price);
      else
		send_to_char ("Ok, now what?\n\r", ch);

    }	/* end: sell */

    argument = one_argument (argument, arg);

  }	/* end: while loop through arguments */

  return;
}





/*
 * util function, converts an 'advanced' ASCII-number-string into a number.
 * Used by parsebet() but could also be used by do_give or do_wimpy.
 *  
 * Advanced strings can contain 'k' (or 'K') and 'm' ('M') in them, not just
 * numbers. The letters multiply whatever is left of them by 1,000 and
 * 1,000,000 respectively. Example:
 *
 * 14k = 14 * 1,000 = 14,000
 * 23m = 23 * 1,000,0000 = 23,000,000
 * If any digits follow the 'k' or 'm', the are also added, but the number
 * which they are multiplied is divided by ten, each time we get one left. This
 * is best illustrated in an example :)
 * 
 * 14k42 = 14 * 1000 + 14 * 100 + 2 * 10 = 14420
 *
 * Of course, it only pays off to use that notation when you can skip many 0's.
 * There is not much point in writing 66k666 instead of 66666, except maybe
 * when you want to make sure that you get 66,666.
 *
 * More than 3 (in case of 'k') or 6 ('m') digits after 'k'/'m' are automatically
 * disregarded. Example:
 *
 * 14k1234 = 14,123
 *
 * If the number contains any other characters than digits, 'k' or 'm', the
 * function returns 0. It also returns 0 if 'k' or 'm' appear more than
 * once.
 *
**/

long advatoi (const char *s)
{
  char string[MIL];		/* a buffer to hold a copy of the argument       */
  char *stringptr = string;	/* a pointer to the buffer so we can move around */
  char tempstring[2];		/* a small temp buffer to pass to atoi           */
  long number = 0;		/* a number to be returned                       */
  long multiplier = 0;		/* multiplier used to get the extra digits right */


  strcpy (string, s);		/* working copy */

  while (isdigit (*stringptr))	/* as long as the current character is a digit */
  {
    strncpy (tempstring, stringptr, 1);	/* copy first digit         */
    number = (number * 10) + atoi (tempstring);	/* add to current number    */
    stringptr++;		/* advance                  */
  }

  switch (UPPER (*stringptr))
  {
	  case 'K':
		multiplier = 1000;
		number *= multiplier;
		stringptr++;
		break;
	  case 'M':
		multiplier = 1000000;
		number *= multiplier;
		stringptr++;
		break;
	  case '\0':
		break;
	  default:
		return 0;			/* not k nor m nor NUL - return 0! */
  }

  while (isdigit (*stringptr) && (multiplier > 1))
    /* if any digits follow k/m, add those too */
  {
    /* copy first digit */
    strncpy (tempstring, stringptr, 1);
    /* the further we get to right, the less are the digit 'worth' */
    multiplier = multiplier / 10;
    number = number + (atoi (tempstring) * multiplier);
    stringptr++;
  }

  if (*stringptr != '\0' && !isdigit (*stringptr))
    /* a non-digit character was found, other than NUL
     * If a digit is found, it means the multiplier is 1 - i.e. extra
     * digits that just have to be ignore, liked 14k4443 -> 3 is ignored 
     */
    return 0;


  return (number);
}


/*
 *  This function allows the following kinds of bets to be made:
 * 
 * Absolute bet
 * ============
 *
 * bet 14k, bet 50m66, bet 100k
 *
 * Relative bet
 * ============
 *
 * These bets are calculated relative to the current bet. The '+' symbol adds
 * a certain number of percent to the current bet. The default is 25, so
 * with a current bet of 1000, bet + gives 1250, bet +50 gives 1500 etc.
 * Please note that the number must follow exactly after the +, without any
 * spaces!
 *
 * The '*' or 'x' bet multiplies the current bet by the number specified,
 * defaulting to 2. If the current bet is 1000, bet x  gives 2000, bet x10
 * gives 10,000 etc.
 *
**/

long parsebet (const long currentbet, const char *argument)
{

  long newbet = 0;		/* a variable to temporarily hold the new bet */
  char string[MIL];		/* a buffer to modify the bet string */
  char *stringptr = string;	/* a pointer we can move around */

  strcpy (string, argument);	/* make a work copy of argument */


  if (*stringptr)		/* check for an empty string */
  {

    if (isdigit (*stringptr))	/* first char is a digit assume e.g. 433k */
      newbet = advatoi (stringptr);	/* parse and set newbet to that value */

    else if (*stringptr == '+')	/* add ?? percent */
    {
      if (strlen (stringptr) == 1)	/* only + specified, assume default */
		newbet = (currentbet * 125) / 100;	/* default: add 25% */
      else			/* cut off the first char */
		newbet = (currentbet * (100 + atoi (++stringptr))) / 100;
    }
    else
    {
      printf ("considering: * x \n\r");

      if ((*stringptr == '*') || (*stringptr == 'x')) /* multiply */
      {
		if (strlen (stringptr) == 1) /* only x specified, assume default */
		  newbet = currentbet * 2;	/* default: twice */
		else	/* user specified a number */
		  newbet = currentbet * atoi (++stringptr);	/* chop first char */
      }
    }
  }

  return newbet; /* return the calculated bet */
}
