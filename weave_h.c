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
 * @@ Objective (weave_h.c)
 *
 * This file contains the various support functions for channeling (casting)
 * weaves (spells). Most all of the actual weaves will be found in weaves.c
 *
 * Note: This file uses various macros found in merc.h. A description of
 * these follows;
 *
 * OPU = (not a macro) An 'OPU' is a single unit of The One Power. Our
 *   system is based on the premise that Rand has (as of book 7) an OPR
 *   (sum of the OPU's he has in each flow) of 38. From a players point
 *   of view, this is synonomyous with a single point of Ability.
 * OPR = A players total, permanent 'One Power Rating'
 * TAU = A players total current, available OPU (one power units, ie: Ability)
 * TPR = The total OPU requirement of a particular weave.
 *  Each weave has 5 flows that may have a value. The TPR is the sum of
 *  these 5 flows.
 * MAXFLOWSTR, if 'determine flow str max by sex' is used, (is by default)
 *   Then this macro will return 6 for women, or 8 for men, and represents
 *   the maximum flow strength for each flow that a player is capable of
 *   having. This gives a maximum OPR of 40 for men, and 35 for women.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"




bool is_linked(CHAR_DATA * ach, CHAR_DATA * bch)
{
	if (ach == NULL || bch == NULL)
		return FALSE;
	
	if ( ach->linked_to <= 0 || bch->linked_to <= 0 )
		return FALSE;
	
	return ach->linked_to == bch->linked_to;
}

int LTAU(CHAR_DATA *ch, int what_sphere)
{
  int tau, angreal, sa_angreal;

  angreal = get_angreal_power(ch);
  if (angreal == -1)
	return 0;
  sa_angreal = angreal / 100;
  if (angreal > 99)
    angreal -= sa_angreal * 100;
  sa_angreal++;
  if (what_sphere == ALL)
  {
    tau = ch->flow_str.current[AIR] + 
	  	  ch->flow_str.current[EARTH] + 
	  	  ch->flow_str.current[WATER] + 
		  ch->flow_str.current[FIRE] + 
		  ch->flow_str.current[SPIRIT];
    return tau * sa_angreal + 5 * angreal;
  }
  return ch->flow_str.current[what_sphere] * sa_angreal + angreal;

}

int LPA(CHAR_DATA *ch, int what_sphere)
{
	int count=0;
	CHAR_DATA *vch = NULL, *vch_next = NULL;
	
	if (what_sphere == ALL)
	{
		count = LTAU(ch, ALL);
		if ( ch->linked_to > 0 )
		{
			for (vch = char_list; vch; vch = vch_next )
			{
				vch_next = vch->next;
				if ( ch != vch && is_linked(ch, vch) )
					count += LTAU(vch, ALL);
			}
		}
	}
	else
	{
		count = LTAU(ch, what_sphere);
		if ( ch->linked_to > 0 )
		{
			for (vch = char_list; vch; vch = vch_next )
			{
				vch_next = vch->next;
				if ( ch != vch && is_linked(ch, vch) )
					count += LTAU(vch,what_sphere);
			}
		}
	}
	return count;
}

CHAR_DATA *link_holder(CHAR_DATA *whos)
{
	return ( whos->linked_to > 0 ) ? get_char_by_id(whos->linked_to) : whos;
}

/** Function: do_link
  * Descr   : 
  *         : 
  *         : 
  * Returns : void
  * Syntax  : 
  * Written : v1.0 7/00
  * Author  : Gary McNickle <gary@tarmongaidon.org> (borrowed from do_group)
  * Note    : Link information is not saved to the pfile. If the player disconnects,
  *         : they should lose the link contact.
  */
_DOFUN (do_link)
{
  char arg[MIL], arg2[MIL];
  int link_count;
  int male_count;
  int ch_skill, vi_skill;
  CHAR_DATA *victim;
  CHAR_DATA *gch;
  CHAR_DATA *leader;

  argument = one_argument (argument, arg);
  one_argument (argument, arg2);
  leader = (ch->linked_to > 0) ? get_char_by_id (ch->linked_to) : ch;

  ch_skill = ch->pcdata->learned[skill_lookup ("link")];
  if (ch_skill < 1)
  {
    send_to_char ("Link?  Whats that?\n\r", ch);
    return;
  }

  if (arg[0] == '\0' || (!str_cmp (arg, "release") && arg2[0] == '\0'))
  {
    if (ch != leader && !str_cmp (arg, "release"))
    {
      send_to_char ("Only the controller of the circle can release it.\n\r",
		    ch);
      return;
    }

    for (gch = char_list, link_count = 0; gch != NULL; gch = gch->next)
    {
      if (is_linked (gch, ch))
      {
		link_count++;
		if (!str_cmp (arg, "release"))
		{
		  if (gch != ch)
		  {
			gch->linked_to = 0;
			gch->move -= gch->move / 2;
			act ("$n abruptly releases $s circle.",
			  ch, NULL, gch, TO_VICT, POS_DEAD);
		  }
		}
		else
		{
		  if (leader == gch)
		    chprintf (ch,
		      "[%2d*`W%12s`w]  `C%1d  `N%1d  `B%1d  `R%1d  `W%1d`w\n\r",
		      gch->level, capitalize (PERS (gch, ch)),
		      gch->flow_str.current[AIR],
		      gch->flow_str.current[EARTH],
		      gch->flow_str.current[WATER],
		      gch->flow_str.current[FIRE],
		      gch->flow_str.current[SPIRIT]);
	      else
	        chprintf (ch,
		      "[%2d %12s]  `C%1d  `N%1d  `B%1d  `R%1d  `W%1d`w\n\r",
		      gch->level, capitalize (PERS (gch, ch)),
		      gch->flow_str.current[AIR],
		      gch->flow_str.current[EARTH],
		      gch->flow_str.current[WATER],
		      gch->flow_str.current[FIRE],
		      gch->flow_str.current[SPIRIT]);
		}
      }
    }
    if (link_count < 1)
    {
      send_to_char ("You are not linked with anyone.\n\r", ch);
      return;
    }
    if (str_cmp (arg, "release"))
      chprintf (ch,
		"totals available:`C%3d`N%3d`B%3d`R%3d`W%3d\n\r",
		LPA (leader, AIR),
		LPA (leader, EARTH),
		LPA (leader, WATER),
		LPA (leader, FIRE),
		LPA (leader, SPIRIT));
    else
    {
      send_to_char ("You release your circle.\n\r", ch);
      ch->linked_to = 0;
    }
    return;
  }

  if (ch->position <= POS_MORTAL)
  {
    send_to_char ("You are hurt far to bad for that.\n\r", ch);
    return;
  }

  male_count=0;
  for (gch = char_list, link_count = 0; gch != NULL; gch = gch->next)
  {
    if (gch->linked_to == GET_ID (leader))
    {
      link_count++;
      if (gch->pcdata->true_sex == SEX_MALE)
        male_count++;
    }
  }

  if (!str_cmp (arg, "remove"))
  {
    victim = get_char_room (ch, arg2);
    if (victim == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
    if (ch == leader && ch != victim && !IS_NPC(victim))
    {
      link_count--;
      victim->linked_to = 0;
      act ("$n removes you from $s circle.",
	   ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You remove $N from your circle.",
	   ch, NULL, victim, TO_CHAR, POS_DEAD);
      if (link_count < 2)
      {
	    ch->linked_to = 0;
	    send_to_char ("You release your circle.\n\r", ch);
	    return;
      }
      for (gch = char_list; gch != NULL; gch = gch->next)
	    if (gch->linked_to == GET_ID (ch) && 
			gch != ch && gch != victim)
	      act ("$N was just added to the circle.",
	       gch, NULL, victim, TO_CHAR, POS_DEAD);
    }
    else
      send_to_char ("You can't do that.\n\r", ch);
    return;
  }

  if (!str_cmp (arg, "pass"))
  {
    one_argument (argument, arg2);
    victim = get_char_room (ch, arg2);
    if (victim == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
    if (ch == leader && ch != victim && !IS_NPC(victim))
    {
      vi_skill = victim->pcdata->learned[skill_lookup ("link")];
      if (number_percent () + 100 > vi_skill + ch_skill)
      {
	    ch->move -= ch->move / 10;
	    victim->move -= victim->move / 10;
	    act ("$n tries to pass you control of $s circle.",
	      ch, NULL, victim, TO_VICT, POS_DEAD);
	    act ("You fail to pass $N control of your circle.",
	      ch, NULL, victim, TO_CHAR, POS_DEAD);
		check_improve (ch, skill_lookup ("link"), FALSE, 1);
		return;
      }
      victim->linked_to = GET_ID (victim);
      ch->linked_to = GET_ID (victim);

      act ("$n passes control of $s circle to you.",
	        ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You pass control of your circle to $N.",
	        ch, NULL, victim, TO_CHAR, POS_DEAD);
 	  check_improve (ch, skill_lookup ("link"), TRUE, 1);

      affect_strip (ch, gsn_seize);
      affect_strip (ch, gsn_embrace);

      for (gch = char_list; gch != NULL; gch = gch->next)
      {
  	    if (gch->linked_to == GET_ID (ch))
		{
	      gch->linked_to = GET_ID (victim);
	      act ("Control of the circle was just passed to $N.",
	            gch, NULL, victim, TO_CHAR, POS_DEAD);
		}
      }
    }
    else
      send_to_char ("You can't do that.\n\r", ch);
    return;
  }

  if ((ch->pcdata->true_sex == SEX_MALE && link_count < 2) ||
      (ch->pcdata->true_sex == SEX_FEMALE && link_count > 13))
  {
    send_to_char ("You can't do that.\n\r", ch);
    return;
  }

  victim = get_char_room (ch, arg);
  if (victim == NULL)
  {
    send_to_char ("They aren't here.\n\r", ch);
    return;
  }

  if (ch->linked_to > 0)
  {
    if (ch != leader)
    {
      send_to_char ("You can't do that.\n\r", ch);
      return;
    }
  }
  else
  {
    if (ch != victim)
    {
      send_to_char ("You must first prepare yourself for the circle.\n\r",
		    ch);
      return;
    }
  }

  if (victim->linked_to > 0)
  {
    if (ch == get_char_by_id (victim->linked_to))
    {
      if (victim == ch)
      {
	    send_to_char ("You can't do that.\n\r", ch);
	    return;
      }
      else
      {
	    send_to_char ("They ARE linked in your circle.\n\r", ch);
	    return;
      }
    }
    else
    {
      send_to_char ("They are not receptive.\n\r", ch);
      return;
    }
  }

  if (victim != ch)
  {
    if (IS_NPC(victim) ||
		is_affected (victim, gsn_seize) || 
		is_affected (victim, gsn_embrace))
    {
      send_to_char ("They are not receptive.", ch);
      return;
    }
  }
  else if (link_count == 0)
  {
    if (!is_affected (victim, gsn_embrace))
    {
      send_to_char ("You are unable to start the circle!\n\r", ch);
      return;
    }
  }

  if (IS_FLAG (victim->states.config, _NOFOLLOW))
  {
    send_to_char ("They are not receptive.\n\r", ch);
    return;
  }

  if (victim != ch)
  {
    if (number_percent () + 100 >
	    victim->pcdata->learned[gsn_seize] +
	    victim->pcdata->learned[gsn_embrace] +
	    ch_skill)
    {
      ch->move -= ch->move / 10;
      victim->move -= victim->move / 10;
      act ("$n tries to bring you into $s circle.",
	   ch, NULL, victim, TO_VICT, POS_DEAD);
      act ("You fail to bring $N into your circle.",
	   ch, NULL, victim, TO_CHAR, POS_DEAD);
	  check_improve (ch, skill_lookup ("link"), FALSE, 1);
      return;
    }
    vi_skill = victim->pcdata->learned[skill_lookup ("link")];
    victim->linked_to = GET_ID (ch);
	if (vi_skill < 25 && victim->practice > 0 &&
		number_percent () < get_curr_stat (victim, STAT_INT) +
							get_curr_stat (victim, STAT_AGL))
	{
      send_to_char ("You see how linking works!\n\r", victim);
	  victim->pcdata->learned[skill_lookup ("link")] = 25;
		victim->practice = victim->practice - 1;
	}
	check_improve (ch, skill_lookup ("link"), TRUE, 2);

    act ("You join $n's circle.", ch, NULL, victim, TO_VICT, POS_RESTING);
    act ("$N joins your circle.", ch, NULL, victim, TO_CHAR, POS_DEAD);

    for (gch = char_list; gch != NULL; gch = gch->next)
      if (gch->linked_to == GET_ID (ch) && gch != ch && gch != victim)
	    act ("$N has just been brought into the circle.",
	     gch, NULL, victim, TO_CHAR, POS_DEAD);
  }
  else
  {
    if (number_percent () / 2 + 50 >
	ch->pcdata->learned[gsn_seize] + ch->pcdata->learned[gsn_embrace])
    {
      ch->move -= ch->move / 10;
      act ("You fail to prepare yourself to control a circle.",
	   ch, NULL, victim, TO_CHAR, POS_DEAD);
      return;
    }
    victim->linked_to = GET_ID (ch);

    act ("$n starts to concentrate.",
	 ch, NULL, victim, TO_NOTVICT, POS_RESTING);
    act ("You prepare yourself to control a circle.",
	 ch, NULL, victim, TO_CHAR, POS_DEAD);
  }
  return;
}

/* Changing any of the following three values drastically alters
 * the output generated.  The goal here is to find the median
 * values that we need. (MaxR, MaxF, Div)
 */

/**
 * max_rand formulae taken from:
 *
 *     In Numerical Recipes in C: The Art of Scientific Computing
 *     (William  H.  Press, Brian P. Flannery, Saul A. Teukolsky,
 *     William T.  Vetterling;  New  York:  Cambridge  University
 *     Press,  1990 (1st ed, p. 207))
 */

int max_rand (int max_random)
{
	 int value;
	 
	 value =
		 1 + (unsigned int) ((float) max_random * rand () / (RAND_MAX + 1.0));
	 return value;
}
 
int roll_dice (int MaxF)
{
	 int die;
	 sh_int MaxR = 32;
	 sh_int Div = 4;
	 
	 if (MaxF == 6)
	 {
		 MaxR = 36;
		 Div = 6;
	 }
	 
	 /**
	  * Why divide the value returned from random?
	  * In hopes of gaining a lower value, closer to our
	  * preferred 'mean'.
	  */
	 
	 while ((die = (int) (max_rand (MaxR) / Div)) > MaxF)
		 ;
	 
	 return die;
}




/* Important note: Each of these functions directly affects the flow_str
 * stored within CH. None of them returns a value that can be manipulated.
 */

/** Function: refresh_OP
  * Descr   : Refreshes the players OPR within each flow, determined by
  *         : priority. Highest gets best refresh.  If "restore" is true,
  *         : set's each flow_str to PERM strengths.
  * Returns : n/a
  * Syntax  : refresh_OP( ch, full restore? )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Updated : 01/99 FW
  */
void refresh_OP (CHAR_DATA * ch, bool restore)
{
	int i = 0;
	float adjust = 1.0;
	int meditate = 0;
	int base = 0;
	int total;
	bool gained;
	
	// no need to do anything in this case...
	if ( TAU(ch) == OPR(ch) )
		return;
	
	/* do a full restore of ability and go home */
	if (restore)
	{
		memcpy (ch->flow_str.current, ch->flow_str.perm, sizeof (ch->flow_str.perm));
		return;
	}
	
	/* adjust bonus according to players position */
	switch (ch->position)
	{
		case POS_SLEEPING:
			adjust = 2.0;
			break;
		case POS_SITTING:
			adjust = 1.25;
			break;
		case POS_RESTING:
			adjust = 1.50;
			break;
		case POS_DEAD:
			adjust = -1;
			break;
		case POS_MORTAL:
			adjust = -1;
			break;
		default:
			break;
	}
	
	/* less if poisoned or plagued */
	if (IS_AFFECTED (ch, _AFF_POISON))
		adjust -= .25;

	if (IS_AFFECTED (ch, _AFF_PLAGUE))
		adjust -= .50;
	
	/* more possible for meditation skill */
	meditate = get_skill (ch, gsn_meditation);

	if (meditate > number_percent ())
	{
		if (adjust > 0)
			adjust += meditate / 100;
		else
			adjust += meditate / 200;

		check_improve (ch, gsn_meditation, TRUE, 8);
	}
	
	if (adjust < 0)		/* no gain */
		return;
	
	/* divide any bonus by 2 if they have lost more than half their hp */
	if (adjust > 0 && (ch->hit < ch->max_hit / 2))
		adjust /= 2;
	
	base = 3 - ch->c_class;
	/* base recovery = 3 for channelers, 2 for thieves and 1 for warriors */
	
	/* determine total ability gain this round */
	total = (int) (base + (base * adjust) + (OPR (ch) / 10));
	
	/* base + adjustment, rounded down, + 1/10th total OPR */
	gained = FALSE;
	i = 0;
	while (total > 0)
	{
		if (ch->flow_str.current[i] < ch->flow_str.perm[i])
		{
			ch->flow_str.current[i]++;
			gained = TRUE;
			total--;
		}
		i++;
		if (i > 4)
		{
			if (gained)
			{
				i = 0;
				gained = FALSE;
			}
			else
				total = 0;
		}
	}
	
	return;
}


/** Function: determine_OP
  * Descr   : Determines a new characters/mobs base OPR and sets each flow
  *         : accordingly.  Does not consider talents/specialties. If a
  *         : percentage other than 100 is given, then their base is adjusted
  *         : according to this percentage.
  * Returns : n/a
  * Syntax  : determine_OP( ch/mob, percentage base to give )
  * Written : v1.0 6/98
  * Author  : Frank Williams and Gary McNickle
  * Note    : Franks method
  */
void determine_OP (CHAR_DATA * ch, int percentage)
{
	/*         max opr   mean opr   max flow  mean flow
	 * male       40         9           8       1.8
	 * female     30         6           6       1.2

	// this routine is no longer used, op added in creation

	int i;
	int rolls = 6;
	int maxf = 8;
	int intwis;

	return;

	if ( IS_FLAG(pMud.config, _DETERMINE_OPR_BY_SEX) && ch->sex == SEX_FEMALE )
		maxf = 6;
	
	if (!can_channel (ch))
	{
		memset (ch->flow_str.perm, 0, sizeof (ch->flow_str.perm));
		memset (ch->flow_str.current, 0, sizeof (ch->flow_str.current));
		return;
	}
	
	for (i = 0; i < 5; i++)
		ch->flow_str.perm[i] = dice (1, maxf);
	
	// give higher average scores with fewer rolls 
	intwis = get_curr_stat (ch, STAT_INT) + get_curr_stat (ch, STAT_WIS);
	if (intwis > 33)
		rolls--;
	if (intwis > 36)
		rolls--;
	if (intwis > 39)
		rolls--;
	
	for (i = 0; i < rolls; i++)
		ch->flow_str.perm[SPIRIT] =
		UMIN (ch->flow_str.perm[SPIRIT], dice (1, maxf));
	
	// chance of 0 ability in opposing types 
	// average higher ability in aligned types
	if (ch->sex == SEX_FEMALE)
	{
		for (i = 0; i < rolls; i++)
		{
			ch->flow_str.perm[AIR] = UMIN (ch->flow_str.perm[AIR], dice (1, maxf));
			ch->flow_str.perm[WATER] = UMIN (ch->flow_str.perm[WATER], dice (1, maxf));
			ch->flow_str.perm[FIRE] = UMIN (ch->flow_str.perm[FIRE], dice (1, maxf + 1) - 1);
			ch->flow_str.perm[EARTH] = UMIN (ch->flow_str.perm[EARTH], dice (1, maxf + 1) - 1);
		}
	}
	else
	{
		for (i = 0; i < rolls; i++)
		{
			ch->flow_str.perm[FIRE] = UMIN (ch->flow_str.perm[FIRE], dice (1, maxf));
			ch->flow_str.perm[EARTH] = UMIN (ch->flow_str.perm[EARTH], dice (1, maxf));
			ch->flow_str.perm[AIR] = UMIN (ch->flow_str.perm[AIR], dice (1, maxf + 1) - 1);
			ch->flow_str.perm[WATER] = UMIN (ch->flow_str.perm[WATER], dice (1, maxf + 1) - 1);
		}
	}
	
	// bonus for specialty
	if ( !IS_NPC(ch) )
		for (i = 0; i < MAX_TALENTS; i++)
		{
			if ((ch->pcdata->talents[i] > 0) && (!str_cmp (talent_table[i].name, "air")))
				ch->flow_str.perm[AIR] = UMIN (ch->flow_str.perm[AIR] + 1, maxf);
			else
				if ((ch->pcdata->talents[i] > 0) && (!str_cmp (talent_table[i].name, "earth")))
					ch->flow_str.perm[EARTH] = UMIN (ch->flow_str.perm[EARTH] + 1, maxf);
				else
					if ((ch->pcdata->talents[i] > 0) && (!str_cmp (talent_table[i].name, "fire")))
						ch->flow_str.perm[FIRE] = UMIN (ch->flow_str.perm[FIRE] + 1, maxf);
					else
						if ((ch->pcdata->talents[i] > 0) && (!str_cmp (talent_table[i].name, "water")))
							ch->flow_str.perm[WATER] = UMIN (ch->flow_str.perm[WATER] + 1, maxf);
						else
							if ((ch->pcdata->talents[i] > 0) && (!str_cmp (talent_table[i].name, "spirit")))
								ch->flow_str.perm[SPIRIT] = UMIN (ch->flow_str.perm[SPIRIT] + 1, maxf);
		}
		
		memcpy (ch->flow_str.current, ch->flow_str.perm,
			sizeof (ch->flow_str.perm));
// will remove function code soon
*/
		return;
}

/** Function: deduct_OP
  * Descr   : Deducts the required flow strength of a skill/weave from
  *         : a players TPA. Alternately adjusts movement if weave is
  *         : instantaneous.  If percentage is given, deducts that percentage.
  * Returns : n/a
  * Syntax  : deduct_OP(ch/mob, flow_str array to deduct, percentage amt )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void deduct_OP (CHAR_DATA * ch, int sn, int percentage)
{
   return;
}


/** Function: increase_OP
  * Descr   : Increases the players total OPR rating by adding to each
  *         : flow, based on flow priority.  If perm is set, increases
  *         : permanent array values.
  * Returns : n/a
  * Syntax  : incrase_OP( ch/mob, amount to increase, permanently? )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  *	Updated	: 1/23/99 Frank Williams
  */
void increase_OP (CHAR_DATA * ch, int amount, bool perm)
{
	int i = 0;
	int lowestvalue = 0;
	int lowestflow = 0;
	int maxf = 8;
	bool foundone = TRUE;
	
	
	if (amount < 0)
	{
		decrease_OP (ch, amount, perm);
		return;
	}

	if ( IS_FLAG(pMud.config, _DETERMINE_OPR_BY_SEX) && ch->sex == SEX_FEMALE )
		maxf = 6;
	
	if (perm) /* up to maxf for perm */
	{
		while ((amount > 0) && foundone)
		{
			foundone = FALSE;
			lowestvalue = maxf;
			for (i = 0; i < 5; i++)
			{
				if (ch->flow_str.perm[i] < lowestvalue)
				{
					lowestvalue = ch->flow_str.perm[i];
					lowestflow = i;
					foundone = TRUE;
				}
			}
			if (foundone)
			{
				amount--;
				ch->flow_str.perm[lowestflow]++;
				ch->flow_str.current[lowestflow]++;
			}
		}
		return;
	}
	
	/* for temp, dont go over perm */
	while ((amount > 0) && foundone)
	{
		foundone = FALSE;
		lowestvalue = 99;
		for (i = 0; i < 5; i++)
		{
			if ((ch->flow_str.current[i] < lowestvalue) &&
				(ch->flow_str.current[i] < ch->flow_str.perm[i]))
			{
				lowestvalue = ch->flow_str.current[i];
				lowestflow = i;
				foundone = TRUE;
			}
		}
		if (foundone)
		{
			amount--;
			ch->flow_str.current[lowestflow]++;
		}
	}
	return;
}

/** Function: decrease_OP
  * Descr   : Decreases the players total OPR rating by decreasing each flow.
  *         : If perm is set, adjusts PERM array.
  * Returns : n/a
  * Syntax  : decrease_OP( ch, amount to decrease, permanently? )
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Updated : 1/23/99 Frank Williams
  */
void decrease_OP (CHAR_DATA * ch, int amount, bool perm)
{
	int i = 0;
	int highestvalue = 0;
	int highestflow = 0;
	bool foundone = TRUE;
	
	if (amount < 0)
	{
		while (amount < 0)
		{
			amount++;
			i++;
		}
		amount = i;
		i = 0;
	}
	
	if (perm) /* also adjust current if perm goes lower than current */
	{
		while ((amount > 0) && foundone)
		{
			foundone = FALSE;
			highestvalue = 0;
			for (i = 0; i < 5; i++)
			{
				if (ch->flow_str.perm[i] > highestvalue)
				{
					highestvalue = ch->flow_str.perm[i];
					highestflow = i;
					foundone = TRUE;
				}
			}
			if (foundone)
			{
				amount--;
				ch->flow_str.perm[highestflow]--;
				if (ch->flow_str.current[highestflow] >
					ch->flow_str.perm[highestflow])
					ch->flow_str.current[highestflow] = ch->flow_str.perm[highestflow];
			}
		}
		return;
	}
	
	while ((amount > 0) && foundone)
	{
		foundone = FALSE;
		highestvalue = 0;
		for (i = 0; i < 5; i++)
		{
			if (ch->flow_str.current[i] > highestvalue)
			{
				highestvalue = ch->flow_str.current[i];
				highestflow = i;
				foundone = TRUE;
			}
		}
		if (foundone)
		{
			amount--;
			ch->flow_str.current[highestflow]--;
		}
	}
	return;
}

/** Function: set_OP
  * Descr   : Set's the players/mobs opr by setting each flow.
  * Returns : n/a
  * Syntax  : set_OP ( ch, which_flow? air, earth, fire, water, spirit )
  * Note    : a 'which' of -1 == both current and perm flowstr,
  *         :               0 == current only
  *         :               1 == perm only
  * Written : v1.0 6/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void set_OP (CHAR_DATA * ch, int which, int air, int earth, int fire,
	     int water, int spirit)
{

	if (which <= 0)		/* set current */
	{
		if (air >= 0)
			ch->flow_str.current[AIR] = air;
		if (earth >= 0)
			ch->flow_str.current[EARTH] = earth;
		if (water >= 0)
			ch->flow_str.current[WATER] = water;
		if (fire >= 0)
			ch->flow_str.current[FIRE] = fire;
		if (spirit >= 0)
			ch->flow_str.current[SPIRIT] = spirit;
	}
	
	if (which > 0 || which == -1)
	{				/* set perm */
		if (air >= 0)
			ch->flow_str.perm[AIR] = air;
		if (earth >= 0)
			ch->flow_str.perm[EARTH] = earth;
		if (water >= 0)
			ch->flow_str.perm[WATER] = water;
		if (fire >= 0)
			ch->flow_str.perm[FIRE] = fire;
		if (spirit >= 0)
			ch->flow_str.perm[SPIRIT] = spirit;
	}
	
	return;
}

int find_spell (CHAR_DATA * ch, const char *name)
{
	/* finds a spell the character can cast if possible */
	int sn, found = -1;
	
	if (IS_NPC (ch))
		return skill_lookup (name);
	
	for (sn = 0; sn < maxSkill; sn++)
	{
		if (skill_table[sn].name == NULL)
			continue;
		
		if (LOWER (name[0]) == LOWER (skill_table[sn].name[0])
			&& !str_prefix (name, skill_table[sn].name))
		{
			if (found == -1)
				found = sn;
			if (get_skill (ch, sn) > 0)
				return sn;
		}
	}
	return found;
}

/* new: now shows the glow if of proper sex
* 012299FW fixed so this works (was looking too big) and added more cases
*/
void say_spell (CHAR_DATA * ch, int sn_cast)
{
	CHAR_DATA *rch;
	int sn = -1;
	int diff = 0;
	bool check_learn = FALSE;
	int increase_amount;
	
	for (rch = ch->in_room->people; rch; rch = rch->next_in_room)
	{
		if (rch != ch && can_channel (rch))
		{
			char *pwr;
			
			pwr = "about the same strength as";
			diff = OPR (ch) - OPR (rch);
			
			if (diff > 20)
				pwr = "VASTLY more powerful than";
			else if (diff < -20)
				pwr = "VASTLY less powerful than";
			else if (diff > 15)
				pwr = "very much stronger than";
			else if (diff < -15)
				pwr = "very much weaker than";
			else if (diff > 10)
				pwr = "much stronger than";
			else if (diff < -10)
				pwr = "much weaker than";
			else if (diff > 5)
				pwr = "stronger than";
			else if (diff < -5)
				pwr = "weaker than";
			else if (diff > 1)
				pwr = "a little bit stronger than";
			else if (diff < 0)
				pwr = "a little bit weaker than";
			
			if ((rch->sex == ch->sex) && (ch != rch))
			{
				switch (ch->sex)
				{
					case SEX_MALE:
						{
							sn = gsn_seize;
							if (sn > 0 && rch->position > POS_SLEEPING)
							{
								chprintf (rch,
									"You feel a surge in Saidin as %s channels\n\r",
									PERS (ch, rch));
								
								/* men only sense this if they are seizing saidin */
								if (is_affected (rch, sn))
								{
									check_learn = TRUE;
									chprintf (rch,
									"You intuitively sense that %s is %s you in the"
									" One Power\n\r", PERS (ch, rch), pwr);
								}
							}
							break;
						}
						
					case SEX_FEMALE:
						{
							sn = gsn_embrace;
							if (sn > 0 && rch->position > POS_SLEEPING)
							{
								chprintf (rch,
									"You see a bright nimbus flare around %s as"
									" she channels\n\r", PERS (ch, rch));
								
								/* women feel this even if they have not embraced saidar */
								chprintf (rch,
									"You intuitively sense that %s is %s you in the One Power\n\r",
									PERS (ch, rch), pwr);
								if (is_affected (rch, sn))
									check_learn=TRUE;
							}
							break;
						}
					default:
						break;
			
				} /* end of switch */
				if (IS_NPC (ch) || IS_NPC (rch))
					check_learn=FALSE;
				if (check_learn)
				{
					check_learn=FALSE;
					{
						if (GET_SKILL (rch, sn_cast) == 0)
						{
							if (rch->practice < 1)
							{
								chprintf (rch, "You can almost see how to form the weave!\n\r", PERS (ch, rch));
								return;
							}
							rch->practice = rch->practice -1;
						}
						chprintf (rch, "You see how %s formed the weave!\n\r", PERS (ch, rch));
						if (50 - GET_SKILL (rch, sn_cast) < OPR (rch))
							increase_amount = 50 - GET_SKILL (rch, sn_cast);
						else
							increase_amount = OPR (rch);
						if (increase_amount < 1)
						  increase_amount = 1;
						INCREASE_SKILL (rch, sn_cast, increase_amount);
					}
				}
			}
		}
	}
	return;
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell (int level, CHAR_DATA * victim, int dam_type)
{
  int save = 0;

  if (victim == NULL)
    return FALSE;

  save = 50 + (victim->level - level) * 5 - victim->saving_throw * 2;
  if (IS_AFFECTED (victim, _AFF_BERSERK))
    save += victim->level / 2;

  switch (check_immune (victim, dam_type))
  {
	  case IS_IMMUNE:
		return TRUE;
	  case IS_RESISTANT:
		save += 2;
		break;
	  case IS_VULNERABLE:
		save -= 2;
		break;
  }

  if (!IS_NPC (victim))		
  {
    if (dam_type == DAM_WEAVE && can_channel (victim))
    {

		if ( is_affected (victim, gsn_seize) || is_affected (victim, gsn_embrace) )
			save = URANGE (5, 50 + get_skill (victim, skill_lookup("slice")), 100);
		else
		    save = URANGE (5, get_skill (victim, skill_lookup("slice")) / 2, 50);

      if (is_clan (victim) && IS_FLAG (clan_table[victim->clan].flags, _ASHAMAN))
		save += 10;

      return number_percent () < save;
    }

    return number_percent () <= (victim->perm_stat[STAT_AGL] + victim->perm_stat[STAT_DEX]) / 2;
  }


  save = URANGE (5, save, 50);
  return number_percent () < save;
}

/* RT save for dispels */
bool saves_dispel (int dis_level, int spell_level, int duration)
{
  int save;

  if (duration == -1) // should be very hard to dispel perm affects
    spell_level += 5;
  

  save = 50 + (spell_level - dis_level) * 5;
  save = URANGE (5, save, 95);
  return number_percent () < save;
}

/* co-routine for dispel magic and sever */
bool check_dispel (int dis_level, CHAR_DATA * victim, int sn)
{
	AFFECT_DATA *af;
	
	if (is_affected (victim, sn))
	{
		for (af = victim->affected; af != NULL; af = af->next)
		{
			if (af->type == sn)
			{
				if (!saves_dispel (dis_level, af->level, af->duration)
					|| dis_level == -1)
				{
					affect_strip (victim, sn);
					if (skill_table[sn].msg_off)
					{
						send_to_char (skill_table[sn].msg_off, victim);
						send_to_char ("\n\r", victim);
					}
					return TRUE;
				}
				else
					af->level--;
			}
		}
	}
	return FALSE;
}

/** Function: check_burn
  * Descr   : Checks to see if the character's ability to channel has been
  *         : altered by thier use of a weave/talent that required more of
  *         : The One Power than the ch has the Ability to use...
  *         : This check is based on ch->level, TPR -vs- TAU and the
  *         : level of the weave/talent they tried to channel...
  * Returns : TRUE/FALSE if they can still channel the weave.
  * Syntax  : check_burn ( ch in question, gsn of the weave, level of the weave)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * Updated : 01/22/99 FW Modified for variable type of burn depending on
  *         : the cause
  */
bool check_burn (CHAR_DATA * ch, int sn)
{

	return FALSE;

}				/* end check_burn */

void do_channel (CHAR_DATA * ch, char *argument, bool insane)
{
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	void *vo;
	int sn;
	enum _GEN_TARGETS target;
	sh_int sex;
	
	/* Switched NPC's can cast spells, but others can't. */
	if (IS_NPC (ch) && ch->desc == NULL)
		return;
	
	sex = TRUESEX (ch);
	
	argument = one_argument (argument, arg1);
	one_argument (argument, arg2);
	
	if (!can_channel (ch))
	{
		send_to_char ("You can't channel!.\n\r", ch);
		return;
	}
	
	if ( ch->linked_to > 0 && ch != get_char_by_id(ch->linked_to) )
	{
		chprintf(ch, "%s is the only person in the link who can channel!\n\r",
			PERS (get_char_by_id(ch->linked_to), ch));
		return;
	}
	
	if (arg1[0] == '\0')
	{
		send_to_char ("Cast which what where?\n\r", ch);
		return;
	}
	
	if (IS_AFFECTED (ch, _AFF_PAINED) && (number_range (0, 100) > 25))
	{
		send_to_char
			("You suddenly double over in a fresh onslaught of pain!\n\r", ch);
		return;
	}
	
	if (!str_cmp (arg1, "list") || !str_cmp (arg1, "all"))
	{
		do_spells (ch, "all");
		return;
	}
	
	if (ch->position < POS_RESTING && !insane)
	{
		send_to_char ("Not in your state!\n\r", ch);
		return;
	}
	
	if (IS_FLAG(pMud.config, _ENFORCE_EMBRACE) && !IS_NPC (ch) && !IS_IMMORTAL (ch))
	{
		if (sex == SEX_MALE)
			sn = gsn_seize;
		else
			sn = gsn_embrace;
		
		if (!is_affected (ch, sn))
		{
			if (sex == SEX_MALE)
				send_to_char ("You have not seized Saidin!", ch);
			else
				send_to_char ("You must open yourself up to Saidar first!", ch);
			return;
		}
	}
	
	if ((sn = find_spell (ch, arg1)) < 0
	|| skill_table[sn].weave_fun == NULL
	|| ((GET_SKILL (ch, sn) < 1 && !insane)))
	{
		send_to_char ("You don't know any weaves of that name.\n\r", ch);
		return;
	}
	
	if (ch->position < skill_table[sn].minimum_position && !insane)
	{
		send_to_char ("You can't concentrate enough.\n\r", ch);
		return;
	}
	

    // Locate targets.
	victim = NULL;
	obj = NULL;
	vo = NULL;
	target = TARGET_NONE;
	
	if (insane)
	{
		target = TAR_INSANE;
		if ((vo = get_random_char (ch)) == NULL)
			vo = ch;
		victim = vo;
	}
	else
	{	
		switch (skill_table[sn].target)
		{
			default:
				bugf("[weave_h.c::do_channel] bad target for sn %d.", sn);
				return;
				
			case TAR_IGNORE:
				break;
				
			case TAR_CHAR_OFFENSIVE:
				if (arg2[0] == '\0')
				{
					if ((victim = ch->fighting) == NULL)
					{
						send_to_char ("weave it on whom?\n\r", ch);
						return;
					}
				}
				else
				{
					if ((victim = get_char_room (ch, arg2)) == NULL)
					{
						send_to_char ("They aren't here.\n\r", ch);
						return;
					}
				}
				
				if (ch == victim)
				{
					send_to_char ("You can't do that to yourself.\n\r", ch);
					return;
				}
				
				if (!IS_NPC (ch))
				{
					if (is_safe (ch, victim) && victim != ch)
					{
						send_to_char ("Not on that target.\n\r", ch);
						return;
					}
				}
				
				if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master == victim)
				{
					send_to_char ("You can't do that on your own follower.\n\r", ch);
					return;
				}
				
				vo = (void *) victim;
				target = TARGET_CHAR;
				break;
				
			case TAR_CHAR_DEFENSIVE:
				if (arg2[0] == '\0')
				{
					victim = ch;
				}
				else
				{
					if ((victim = get_char_room (ch, arg2)) == NULL)
					{
						send_to_char ("They aren't here.\n\r", ch);
						return;
					}
				}
				
				vo = (void *) victim;
				target = TARGET_CHAR;
				break;
				
			case TAR_CHAR_SELF:
				if (arg2[0] != '\0' && !is_name (arg2, ch->name))
				{
					send_to_char ("You cannot weave this on another.\n\r", ch);
					return;
				}
				
				vo = (void *) ch;
				target = TARGET_CHAR;
				break;
				
			case TAR_OBJ_INV:
				if (arg2[0] == '\0')
				{
					send_to_char ("What should the weave be placed upon?\n\r", ch);
					return;
				}
				
				if ((obj = get_obj_carry (ch, arg2, ch)) == NULL)
				{
					send_to_char ("You are not carrying that.\n\r", ch);
					return;
				}
				
				vo = (void *) obj;
				target = TARGET_OBJ;
				break;
				
			case TAR_OBJ_CHAR_OFF:
				if (arg2[0] == '\0')
				{
					if ((victim = ch->fighting) == NULL)
					{
						send_to_char ("Who or what should the weave be placed on?\n\r", ch);
						return;
					}
					
					target = TARGET_CHAR;
				}
				else
				if ((victim = get_char_room (ch, arg2)) != NULL)
				{
					target = TARGET_CHAR;
				}
				
				if (target == TARGET_CHAR)	/* check the sanity of the attack */
				{
					
					if (IS_AFFECTED (ch, _AFF_COMPULSION) && ch->master == victim)
					{
						send_to_char ("You can't do that on your own follower.\n\r", ch);
						return;
					}
					
					vo = (void *) victim;
				}
				else
				if ((obj = get_obj_here (ch, arg2)) != NULL)
				{
					vo = (void *) obj;
					target = TARGET_OBJ;
				}
				else
					
				{
					send_to_char ("You don't see that here.\n\r", ch);
					return;
				}
				break;
						
			case TAR_OBJ_CHAR_DEF:
				if (arg2[0] == '\0')
				{
					vo = (void *) ch;
					target = TARGET_CHAR;
				}
				else
				if ((victim = get_char_room (ch, arg2)) != NULL)
				{
					vo = (void *) victim;
					target = TARGET_CHAR;
				}
				else
				if ((obj = get_obj_carry (ch, arg2, ch)) != NULL)
				{
					vo = (void *) obj;
					target = TARGET_OBJ;
				}
				else
					
				{
					send_to_char ("You don't see that here.\n\r", ch);
					return;
				}
				break;
		}
	}

		/*
		 * Figure the skill level here independently because we dont want to check
		 * against the level of the weave vs the characters level.
		 */
		if (!insane && number_percent () >
			(IS_NPC (ch) ? get_skill (ch, sn) : GET_SKILL (ch, sn)))
		{
			send_to_char ("You can't remember the exact formation of the weave.\n\r",
				ch);
			check_improve (ch, sn, FALSE, 1);
			WAIT_STATE (ch, skill_table[sn].beats / 2);
		}
		else
			
		{
			/* Deduct requred OPU from player if necessary to do so */
			deduct_OP (ch, sn, 100);
			
			if (IS_NPC (ch) || ch->c_class == CHANNELER)
				/* class has spells */
				(*skill_table[sn].weave_fun) (sn, ch->level, ch, vo, target, argument, FALSE);
			else
				
				(*skill_table[sn].weave_fun) (sn, 3 * ch->level / 4, ch, vo, target, argument, FALSE);

			check_improve (ch, sn, TRUE, 1);
			say_spell (ch, sn );
			WAIT_STATE (ch, skill_table[sn].beats);
		}
		
		if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
		|| (skill_table[sn].target == TAR_OBJ_CHAR_OFF
		&& target == TARGET_CHAR)) && victim != ch
		&& !IS_AFFECTED (victim, _AFF_BOUND_AIR)
		&& !IS_AFFECTED (victim, _AFF_CAPTURED) && victim->master != ch)
		{
			CHAR_DATA *vch;
			CHAR_DATA *vch_next;
			
			for (vch = ch->in_room->people; vch; vch = vch_next)
			{
				vch_next = vch->next_in_room;
				if (victim == vch && victim->fighting == NULL)
				{
					multi_hit (victim, ch, TYPE_UNDEFINED);
					break;
				}
			}
		}
		
		return;

}

void do_cast (CHAR_DATA * ch, char *argument)
{
  do_channel (ch, argument, FALSE);
}

/* Cast spells at targets using a magical object. */
void obj_cast_spell (int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj)
{
	void *vo;
	int target = TARGET_NONE;
	
	if (sn <= 0)
		return;
	
	if (!IS_VALID_SKILL(sn) || skill_table[sn].weave_fun == 0)
	{
		bugf("[weave_h.c::obj_cast_spell] bad sn %d.", sn);
		return;
	}
	
	switch (skill_table[sn].target)
	{
		default:
			bugf("[weave_h.c::obj_cast_spell] bad target for sn %d.", sn);
			return;
			
		case TAR_IGNORE:
			vo = NULL;
			break;
			
		case TAR_CHAR_OFFENSIVE:
			if (victim == NULL)
				victim = ch->fighting;
			if (victim == NULL)
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			if (is_safe (ch, victim) && ch != victim)
			{
				send_to_char ("Something isn't right...\n\r", ch);
				return;
			}
			vo = (void *) victim;
			target = TARGET_CHAR;
			break;
			
		case TAR_CHAR_DEFENSIVE:
		case TAR_CHAR_SELF:
			if (victim == NULL)
				victim = ch;
			vo = (void *) victim;
			target = TARGET_CHAR;
			break;
			
		case TAR_OBJ_INV:
			if (obj == NULL)
			{
				send_to_char ("You can't do that.\n\r", ch);
				return;
			}
			vo = (void *) obj;
			target = TARGET_OBJ;
			break;
			
		case TAR_OBJ_CHAR_OFF:
			if (victim == NULL && obj == NULL)
			{
				if (ch->fighting != NULL)
				{
					victim = ch->fighting;
				}
				else
				{
					send_to_char ("You can't do that.\n\r", ch);
					return;
				}
			}
			
			if (victim != NULL)
			{
				
				vo = (void *) victim;
				target = TARGET_CHAR;
			}
			else
			{
				vo = (void *) obj;
				target = TARGET_OBJ;
			}
			break;
			
		case TAR_OBJ_CHAR_DEF:
			if (victim == NULL && obj == NULL)
			{
				vo = (void *) ch;
				target = TARGET_CHAR;
			}
			else if (victim != NULL)
			{
				vo = (void *) victim;
				target = TARGET_CHAR;
			}
			else
			{
				vo = (void *) obj;
				target = TARGET_OBJ;
			}
			
			break;
	}

	if ( skill_table[sn].weave_fun != NULL )
	  (*skill_table[sn].weave_fun) (sn, level, ch, vo, target, "", TRUE);
	else
		bugf("[weave_h.c::obj_cast_spell] NULL weave function for weave '%s'!",
		     skill_table[sn].name);

	if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
	|| (skill_table[sn].target == TAR_OBJ_CHAR_OFF
	&& target == TARGET_CHAR)) && victim != ch && victim->master != ch)
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
		
		for (vch = ch->in_room->people; vch; vch = vch_next)
		{
			vch_next = vch->next_in_room;
			if (victim == vch && victim->fighting == NULL)
			{
				multi_hit (victim, ch, TYPE_UNDEFINED);
				break;
			}
		}
	}
	
	return;

}

