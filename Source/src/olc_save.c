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

/**************************************************************************
 *  File: olc_save.c                                                       *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"

#define DIF(a,b) (~((~a)|(b)))

/*****************************************************************************
 Name:            fix_string
 Purpose:         Returns a string without \r and ~.
****************************************************************************/
char *fix_string (const char *str)
{
  static char strfix[MSL];
  int i = 0;

  if (str == NULL)
    return '\0';

  memset (strfix, 0, MSL);

  while (*str != '\0')
  {
    if (*str != '\r' && *str != '~')
      strfix[i++] = *str;
    str++;
  }

  return strfix;
}


bool save_helps (bool backup)
{
  FILE *fp;
  HELP_DATA *help = help_first;
  char fname[MIL];

  sprintf (fname, "%s%s", (backup) ? AREA_BACKUP_DIR : "", "help.hlp");

  if ((fp = fopen (fname, "w")) == NULL)
  {
    bugf ("[olc_save.c::save_helps] Error opening file %s for writing.", fname);
    return FALSE;
  }
  else
  {
    fprintf (fp, "#HELPS\n\n");

    for (; help; help = help->next)
    {
      fprintf (fp, "%d %s~\n", help->level, help->keyword);
      fprintf (fp, "%s~\n\n", fix_string (help->text));
    }

    fprintf (fp, "-1 $~\n\n#$\n");
  }

  fclose (fp);

  return TRUE;
}


/*****************************************************************************
 Name:		save_area_list
 Purpose:	Saves the listing of files to be loaded at startup.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area_list (bool backup)
{
  FILE *fp;
  AREA_DATA *pArea;
  char filename[80];

  memset (filename, 0, 80);

  if (backup)
    sprintf (filename, "%s%s", AREA_BACKUP_DIR, "area.lst");
  else
    sprintf (filename, "%s", "area.lst");

  if ((fp = fopen (filename, "w")) == NULL)
  {
    bugf ("[save.c::save_area_list] failed to open %s.", filename);
    perror ("area.lst");
  }
  else
  {
    /*
     * Add any help files that need to be loaded at
     * startup to this section.
     */
    fprintf (fp, "help.hlp\n");

    for (pArea = area_first; pArea; pArea = pArea->next)
    {
      if (!IS_SET (pArea->area_flags, AREA_DELETE))
	fprintf (fp, "%s\n", pArea->file_name);
    }

    fprintf (fp, "$\n");
    fclose (fp);
  }

  if (backup)
    save_area_list (FALSE);	/* save a copy in the actual area dir. */

  return;
}


/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 * 
 * buf[] must hold at least 32+1 characters.
 *
 * -- Hugin
 */
char *fwrite_flag (long flags, char buf[])
{
  char offset;
  char *cp;

  buf[0] = '\0';

  if (flags == 0)
  {
    strcpy (buf, "0");
    return buf;
  }

  /* 32 -- number of bits in a long */

  for (offset = 0, cp = buf; offset < 32; offset++)
    if (flags & ((long) 1 << offset))
    {
      if (offset <= 'Z' - 'A')
	*(cp++) = 'A' + offset;
      else
	*(cp++) = 'a' + offset - ('Z' - 'A' + 1);
    }

  *cp = '\0';

  return buf;
}

void save_mobprogs (FILE * fp, AREA_DATA * pArea)
{
  MPROG_CODE *pMprog;
  long i;

  fprintf (fp, "#MOBPROGS\n");

  for (i = pArea->min_vnum; i <= pArea->max_vnum; i++)
  {
    if ((pMprog = get_mprog_index (i)) != NULL)
    {
      fprintf (fp, "#%ld\n", i);
      fprintf (fp, "%s~\n", fix_string (pMprog->code));
      fprintf (fp, "%s~\n", fix_string (pMprog->author));
      fprintf (fp, "%s~\n", fix_string (pMprog->title));
    }
  }

  fprintf (fp, "#0\n\n");
  return;
}





/*****************************************************************************
 Name:		save_mobile
 Purpose:	Save one mobile to file, new format -- Hugin
 Called by:	save_mobiles (below).
 ****************************************************************************/
void save_mobile (FILE * fp, MOB_INDEX_DATA * pMobIndex)
{
  MPROG_LIST *pMprog;

  fprintf (fp, "#%ld\n", pMobIndex->vnum);
  fprintf (fp, "%s~\n", pMobIndex->player_name);
  fprintf (fp, "%s~\n", pMobIndex->short_descr);
  fprintf (fp, "%s~\n", fix_string (pMobIndex->long_descr));
  fprintf (fp, "%s~\n", fix_string (pMobIndex->description));
  fprintf (fp, "%s~\n", race_table[pMobIndex->race].name);
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.player));
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.affected_by));
  fprintf (fp, "%d %d\n", pMobIndex->alignment, pMobIndex->group);
  fprintf (fp, "%d ", pMobIndex->level);
  fprintf (fp, "%d ", pMobIndex->hitroll);
  fprintf (fp, "%dd%d+%d ",
	   pMobIndex->hit[DICE_NUMBER],
	   pMobIndex->hit[DICE_TYPE], pMobIndex->hit[DICE_BONUS]);
  fprintf (fp, "%dd%d+%d ",
	   pMobIndex->OPR_dice[DICE_NUMBER],
	   pMobIndex->OPR_dice[DICE_TYPE], pMobIndex->OPR_dice[DICE_BONUS]);
  fprintf (fp, "%dd%d+%d ",
	   pMobIndex->damage[DICE_NUMBER],
	   pMobIndex->damage[DICE_TYPE], pMobIndex->damage[DICE_BONUS]);
  fprintf (fp, "%s\n", attack_table[pMobIndex->dam_type].name);
  fprintf (fp, "%d %d %d %d\n",
	   pMobIndex->ac[AC_PIERCE] / 10,
	   pMobIndex->ac[AC_BASH] / 10,
	   pMobIndex->ac[AC_SLASH] / 10, pMobIndex->ac[AC_EXOTIC] / 10);

  fprintf (fp, "%s\n", print_bits (pMobIndex->states.off));
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.imm));
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.res));
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.vuln));

  fprintf (fp, "%s %s %s %ld\n",
	   position_table[pMobIndex->start_pos].short_name,
	   position_table[pMobIndex->default_pos].short_name,
	   sex_table[pMobIndex->sex].name, pMobIndex->wealth);
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.form));
  fprintf (fp, "%s\n", print_bits (pMobIndex->states.parts));
  fprintf (fp, "%s ", size_table[pMobIndex->size].name);
  fprintf (fp, "%s\n", IS_NULLSTR (pMobIndex->material) ?
	   pMobIndex->material : "unknown");

  for (pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next)
  {
    fprintf (fp, "M %s %ld %s~\n",
	     mprog_type_to_name (pMprog->trig_type), pMprog->vnum,
	     pMprog->trig_phrase);
  }

  if (pMobIndex->sounds)
    save_sounds (fp, pMobIndex->sounds, "Sound");

  if ( IS_FLAG(pMobIndex->states.player, _ACT_PERSISTANT) )
  {
		CHAR_DATA *ch = create_mobile( pMobIndex );

		if ( ch )
			do_save(ch,"");
  }

  return;
}


/*****************************************************************************
 Name:		save_mobiles
 Purpose:	Save #MOBILES secion of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_mobiles (FILE * fp, AREA_DATA * pArea)
{
  long i;
  MOB_INDEX_DATA *pMob;

  fprintf (fp, "#MOBILES\n");

  for (i = pArea->min_vnum; i <= pArea->max_vnum; i++)
  {
    if ((pMob = get_mob_index (i)))
      save_mobile (fp, pMob);
  }

  fprintf (fp, "#0\n\n\n\n");
  return;
}





/*****************************************************************************
 Name:		save_object
 Purpose:	Save one object to file.
                new ROM format saving -- Hugin
 Called by:	save_objects (below).
 ****************************************************************************/
void save_object (FILE * fp, OBJ_INDEX_DATA * pObjIndex)
{
  char letter;
  AFFECT_DATA *pAf;
  EXTRA_DESCR_DATA *pEd;
  char buf[MSL];

  fprintf (fp, "#%d\n", pObjIndex->vnum);
  fprintf (fp, "%s~\n", pObjIndex->name);
  fprintf (fp, "%s~\n", pObjIndex->short_descr);
  fprintf (fp, "%s~\n", fix_string (pObjIndex->description));
  fprintf (fp, "%s~\n", pObjIndex->material);
  fprintf (fp, "%s ", item_name (pObjIndex->item_type));
  fprintf (fp, "%s ", fwrite_flag (pObjIndex->extra_flags, buf));
  fprintf (fp, "%s\n", fwrite_flag (pObjIndex->wear_flags, buf));

/*
 *  Using fwrite_flag to write most values gives a strange
 *  looking area file, consider making a case for each
 *  item type later.
 */

  switch (pObjIndex->item_type)
  {
  default:
    fprintf (fp, "%s ", fwrite_flag (pObjIndex->value[0], buf));
    fprintf (fp, "%s ", fwrite_flag (pObjIndex->value[1], buf));
    fprintf (fp, "%s ", fwrite_flag (pObjIndex->value[2], buf));
    fprintf (fp, "%s ", fwrite_flag (pObjIndex->value[3], buf));
    fprintf (fp, "%s\n", fwrite_flag (pObjIndex->value[4], buf));
    break;

  case ITEM_DRINK_CON:
  case ITEM_FOUNTAIN:
    fprintf (fp, "%d %d '%s' %d %d\n",
	     pObjIndex->value[0],
	     pObjIndex->value[1],
	     liq_table[pObjIndex->value[2]].liq_name,
	     pObjIndex->value[3], pObjIndex->value[4]);
    break;

  case ITEM_CONTAINER:
    fprintf (fp, "%d %s %d %d %d\n",
	     pObjIndex->value[0],
	     fwrite_flag (pObjIndex->value[1], buf),
	     pObjIndex->value[2], pObjIndex->value[3], pObjIndex->value[4]);
    break;

  case ITEM_WEAPON:
    fprintf (fp, "%s %d %d %s %s\n",
	     weapon_name (pObjIndex->value[0]),
	     pObjIndex->value[1],
	     pObjIndex->value[2],
	     attack_table[pObjIndex->value[3]].name,
	     fwrite_flag (pObjIndex->value[4], buf));
    break;

  case ITEM_HERB:
  case ITEM_POTION:
    fprintf (fp, "%d '%s' '%s' '%s' '%s'\n", 
		 pObjIndex->value[0] > 0 ? pObjIndex->value[0] : 0, 
		 IS_VALID_SKILL(pObjIndex->value[1]) ? skill_table[pObjIndex->value[1]].name : "",
		 IS_VALID_SKILL(pObjIndex->value[2]) ? skill_table[pObjIndex->value[3]].name : "",
		 IS_VALID_SKILL(pObjIndex->value[3]) ? skill_table[pObjIndex->value[3]].name : "",
		 IS_VALID_SKILL(pObjIndex->value[4]) ? skill_table[pObjIndex->value[4]].name : "");
    break;

  case ITEM_STAFF:
  case ITEM_WAND:
    fprintf (fp, "%d %d %d '%s' %d\n",
	     pObjIndex->value[0],
	     pObjIndex->value[1],
	     pObjIndex->value[2],
		 IS_VALID_SKILL(pObjIndex->value[3]) ? skill_table[pObjIndex->value[3]].name : "",
		 pObjIndex->value[4]);
    break;
  case ITEM_QUIVER:
    fprintf (fp, "%d %d %d 0 0\n",
	     pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2]);
    break;

  case ITEM_ARROW:
  case ITEM_ANGREAL:
    fprintf (fp, "%d %d %d 0 0\n",
	     pObjIndex->value[0], pObjIndex->value[1], pObjIndex->value[2]);
    break;

  }

  fprintf (fp, "%d ", pObjIndex->level);
  fprintf (fp, "%d ", pObjIndex->weight);
  fprintf (fp, "%d ", pObjIndex->cost);

  if (pObjIndex->condition > 90)
    letter = 'P';
  else if (pObjIndex->condition > 75)
    letter = 'G';
  else if (pObjIndex->condition > 50)
    letter = 'A';
  else if (pObjIndex->condition > 25)
    letter = 'W';
  else if (pObjIndex->condition > 10)
    letter = 'D';
  else if (pObjIndex->condition > 0)
    letter = 'B';
  else
    letter = 'R';

  fprintf (fp, "%c\n", letter);

  for (pAf = pObjIndex->affected; pAf; pAf = pAf->next)
  {
    if (pAf->where == TO_OBJECT || pAf->bitvector == 0)
      fprintf (fp, "A\n%d %d %d\n",
	       pAf->location, pAf->modifier, pAf->duration);
    else
    {
      fprintf (fp, "F\n");

      switch (pAf->where)
      {
      case TO_AFFECTS:
	fprintf (fp, "A ");
	break;
      case TO_IMMUNE:
	fprintf (fp, "I ");
	break;
      case TO_RESIST:
	fprintf (fp, "R ");
	break;
      case TO_VULN:
	fprintf (fp, "V ");
	break;
      case TO_WEAPON:
	fprintf (fp, "W ");
	break;
      case TO_ROOM_AFF:
	fprintf (fp, "M ");
	break;
      default:
	bugf ("[olc_save.c::save_object] Invalid Affect->where");
	break;
      }

      fprintf (fp, "%d %d %d %s\n",
	       pAf->location,
	       pAf->modifier,
	       pAf->duration, fwrite_flag (pAf->bitvector, buf));
    }
  }

  for (pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next)
  {
    fprintf (fp, "E\n%s~\n%s~\n", pEd->keyword,
	     fix_string (pEd->description));
  }

  if (pObjIndex->sounds)
    save_sounds (fp, pObjIndex->sounds, "Sound");

  return;
}




/*****************************************************************************
 Name:		save_objects
 Purpose:	Save #OBJECTS section of an area file.
 Called by:	save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_objects (FILE * fp, AREA_DATA * pArea)
{
  long i;
  OBJ_INDEX_DATA *pObj;

  fprintf (fp, "#OBJECTS\n");

  for (i = pArea->min_vnum; i <= pArea->max_vnum; i++)
  {
    if ((pObj = get_obj_index (i)))
      save_object (fp, pObj);
  }

  fprintf (fp, "#0\n\n\n\n");
  return;
}





/*****************************************************************************
 Name:		save_rooms
 Purpose:	Save #ROOMS section of an area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_rooms (FILE * fp, AREA_DATA * pArea)
{
  ROOM_INDEX_DATA *pRoomIndex;
  EXTRA_DESCR_DATA *pEd;
  EXIT_DATA *pExit;
  unsigned int iHash;
  int door;

  fprintf (fp, "#ROOMS\n");
  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
  {
    for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
	 pRoomIndex = pRoomIndex->next)
    {
      if (pRoomIndex->area == pArea)
      {
	fprintf (fp, "#%ld\n", pRoomIndex->vnum);
	fprintf (fp, "%s~\n", pRoomIndex->name);
	fprintf (fp, "%s~\n", fix_string (pRoomIndex->description));
	fprintf (fp, "0 ");
	fprintf (fp, "%d ", pRoomIndex->room_flags);
	fprintf (fp, "%d\n", pRoomIndex->sector_type);

	for (pEd = pRoomIndex->extra_descr; pEd; pEd = pEd->next)
	{
	  fprintf (fp, "E\n%s~\n%s~\n", pEd->keyword,
		   fix_string (pEd->description));
	}

	for (door = 0; door < MAX_DIR; door++)	/* I hate this! */
	{

	  if ((pExit = pRoomIndex->exit[door]) && pExit->u1.to_room)
	  {

	    /* HACK : TO PREVENT EX_LOCKED etc without EX_ISDOOR
	     * to stop booting the mud */
	    if (IS_SET (pExit->rs_flags, EX_CLOSED)
		|| IS_SET (pExit->rs_flags, EX_LOCKED)
		|| IS_SET (pExit->rs_flags, EX_PICKPROOF)
		|| IS_SET (pExit->rs_flags, EX_NOPASS)
		|| IS_SET (pExit->rs_flags, EX_EASY)
		|| IS_SET (pExit->rs_flags, EX_HARD)
		|| IS_SET (pExit->rs_flags, EX_INFURIATING)
		|| IS_SET (pExit->rs_flags, EX_NOCLOSE)
		|| IS_SET (pExit->rs_flags, EX_NOLOCK))
	      SET_BIT (pExit->rs_flags, EX_ISDOOR);
	    else
	      REMOVE_BIT (pExit->rs_flags, EX_ISDOOR);

	    fprintf (fp, "D%d\n", pExit->orig_door);
	    fprintf (fp, "%s~\n", fix_string (pExit->description));
	    fprintf (fp, "%s~\n", pExit->keyword);
	    fprintf (fp, "%ld %ld %ld\n", pExit->rs_flags,
		     pExit->key, pExit->u1.to_room->vnum);
	  }
	}

	if (pRoomIndex->sounds)
	  save_sounds (fp, pRoomIndex->sounds, "Z");

	if (pRoomIndex->opu_rate != 100 || pRoomIndex->heal_rate != 100)
	  fprintf (fp, "M %d H %d\n", pRoomIndex->opu_rate,
		   pRoomIndex->heal_rate);

	if (pRoomIndex->clan > 0)
	  fprintf (fp, "C %s~\n", clan_table[pRoomIndex->clan].name);

	if (pRoomIndex->c_class >= 0)
	  fprintf (fp, "L %s~\n", class_table[pRoomIndex->c_class].name);

	if (!IS_NULLSTR (pRoomIndex->owner))
	  fprintf (fp, "O %s~\n", pRoomIndex->owner);

	fprintf (fp, "S\n");
      }
    }
  }
  fprintf (fp, "#0\n\n\n\n");
  return;
}



/*****************************************************************************
 Name:		save_specials
 Purpose:	Save #SPECIALS section of area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_specials (FILE * fp, AREA_DATA * pArea)
{
  int iHash;
  MOB_INDEX_DATA *pMobIndex;

  fprintf (fp, "#SPECIALS\n");

  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
  {
    for (pMobIndex = mob_index_hash[iHash]; pMobIndex;
	 pMobIndex = pMobIndex->next)
    {
      if (pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun)
      {
	fprintf (fp, "M %ld %s\n", pMobIndex->vnum,
		 spec_name (pMobIndex->spec_fun));
      }
    }
  }

  fprintf (fp, "S\n\n\n\n");
  return;
}



/*
 * This function is obsolete.  It it not needed but has been left here
 * for historical reasons.  It is used currently for the same reason.
 *
 * I don't think it's obsolete in ROM -- Hugin.
 */
void save_door_resets (FILE * fp, AREA_DATA * pArea)
{
  int iHash;
  ROOM_INDEX_DATA *pRoomIndex;
  EXIT_DATA *pExit = NULL;
  int door;
  char buf[MSL];

  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
  {
    for (pRoomIndex = room_index_hash[iHash]; pRoomIndex;
	 pRoomIndex = pRoomIndex->next)
    {
      if (pRoomIndex->area == pArea)
      {
	for (door = 0; door < MAX_DIR; door++)
	{

	  if ((pExit = pRoomIndex->exit[door])
	      && pExit->u1.to_room
	      && (IS_SET (pExit->rs_flags, EX_CLOSED)
		  || IS_SET (pExit->rs_flags, EX_LOCKED)))
	  {

	    memset (buf, 0, MSL);
	    sprintf (buf, "%s %s %s",
		     IS_SET (pExit->rs_flags, EX_CLOSED) ? "closed" : "",
		     IS_SET (pExit->rs_flags, EX_LOCKED) ? "and locked" : "",
		     IS_SET (pExit->rs_flags, EX_SECRET) ? "secret" : "");

	    fprintf (fp, "D %ld %d %ld 0 0\n",
		     pRoomIndex->vnum, pExit->orig_door, pExit->rs_flags);
	  }
	}
      }
    }
  }
  return;
}




/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_resets (FILE * fp, AREA_DATA * pArea)
{
  RESET_DATA *pReset;
  ROOM_INDEX_DATA *pRoom;
  int iHash;

  fprintf (fp, "#RESETS\n");

  save_door_resets (fp, pArea);

  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
  {
    for (pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next)
    {
      if (pRoom->area == pArea)
      {
	for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
	{
	  switch (pReset->command)
	  {
	  default:
	    bugf ("[olc_save.c::save_resets] bad command %c.", pReset->command);
	    break;

	    /* christ, talk about a clusterfuck... */

	  case 'D':
	    break;
	  case 'E':
	  case 'G':
	  case 'M':
	  case 'O':
	  case 'P':
	  case 'R':
	    fprintf (fp, "%c %ld %ld %ld %ld %d\n",
		     UPPER (pReset->command),
		     pReset->arg1,
		     pReset->arg2,
		     pReset->arg3, pReset->arg4, pReset->chance);

	    break;

	  }			/* End switch                   */
	}			/* End loop through resets      */
      }				/* End if correct area          */
    }				/* End for pRoom                */
  }				/* End for iHash                */

  fprintf (fp, "S\n\n\n\n");
  return;
}



/*****************************************************************************
 Name:		save_shops
 Purpose:	Saves the #SHOPS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_shops (FILE * fp, AREA_DATA * pArea)
{
  SHOP_DATA *pShopIndex;
  MOB_INDEX_DATA *pMobIndex;
  int iTrade;
  int iHash;

  fprintf (fp, "#SHOPS\n");

  for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
  {
    for (pMobIndex = mob_index_hash[iHash]; pMobIndex;
	 pMobIndex = pMobIndex->next)
    {
      if (pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop)
      {
	pShopIndex = pMobIndex->pShop;

	fprintf (fp, "%ld ", pShopIndex->keeper);
	for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
	{
	  if (pShopIndex->buy_type[iTrade] != 0)
	  {
	    fprintf (fp, "%d ", pShopIndex->buy_type[iTrade]);
	  }
	  else
	    fprintf (fp, "0 ");
	}
	fprintf (fp, "%d %d ", pShopIndex->profit_buy,
		 pShopIndex->profit_sell);
	fprintf (fp, "%d %d\n", pShopIndex->open_hour,
		 pShopIndex->close_hour);
      }
    }
  }

  fprintf (fp, "0\n\n\n\n");
  return;
}



/*****************************************************************************
 Name:		save_area
 Purpose:	Save an area, note that this format is new.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area (AREA_DATA * pArea, bool backup)
{
  char *TEMPFILE = "fp_buffer";
  FILE *fp;
  time_t now;
  struct tm *saved;
  char tdate[11];
  char filename[80];
  char buf[256];

  memset (filename, 0, 80);

  if (backup)
    sprintf (filename, "%s%s", AREA_BACKUP_DIR, pArea->file_name);
  else
    sprintf (filename, "%s", pArea->file_name);

  if (!(fp = fopen (TEMPFILE, "w")))
  {
    bugf ("[olc_save.c::save_area] Error opening file (%s) for writing", filename);
    return;
  }


  time (&now);
  saved = localtime (&now);

  sprintf (tdate, "%02d/%02d/%02d",
	   saved->tm_mon, saved->tm_mday, saved->tm_year);

  if (IS_SET (pArea->area_flags, AREA_CHANGED))
    pArea->last_mod = str_dup (tdate);


  fprintf (fp, "#AREADATA\n");
  fprintf (fp, "Name %s~\n", pArea->name);
  fprintf (fp, "Builders %s~\n", fix_string (pArea->builders));
  fprintf (fp, "VNUMs %ld %ld\n", pArea->min_vnum, pArea->max_vnum);
  fprintf (fp, "Sector %d\n", pArea->sector_type);
  fprintf (fp, "Credits %s~\n", pArea->credits);
  fprintf (fp, "Security %d\n", pArea->security);
  fprintf (fp, "LastSaved %s~\n", pArea->last_mod);
  fprintf (fp, "Flags %s\n", fwrite_flag(pArea->area_flags, buf));
  fprintf (fp, "Version %d\n", pArea->version < 1 ? 1 : pArea->version);

  if (pArea->sounds)
    save_sounds (fp, pArea->sounds, "Sound");

  fprintf (fp, "End\n\n\n\n");

  save_mobiles (fp, pArea);
  save_objects (fp, pArea);
  save_rooms (fp, pArea);
  save_specials (fp, pArea);
  save_resets (fp, pArea);
  save_shops (fp, pArea);
  save_mobprogs (fp, pArea);
  fprintf (fp, "#$\n");

  fclose (fp);

  /* The vast majority of our errors while saving area files, never
   * make it this far. We get a SIGSEGV somewhere above. So, there's
   * little if any need to trap errors up to this point. If we make it
   * this far, were doing well...
   */

  if (rename (TEMPFILE, filename) == -1)
  {
    bugf ("[olc_save.c::save_area] Unable to rename %s to %s. Error# [%d]",
	  TEMPFILE, filename, errno);
    return;
  }

  if (backup)			/* Save a copy in the area directory too! */
    save_area (pArea, FALSE);

  return;
}


/*****************************************************************************
 Name:		do_asave
 Purpose:	Entry point for saving area data.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_asave (CHAR_DATA * ch, char *argument)
{
  char arg1[30];
  char arg2[30];
  AREA_DATA *pArea;
  FILE *fp;
  int value;
  bool backup;
  int sec = 0;

  fp = NULL;

  if (!ch)			/* Do an autosave */
    sec = 9;
  else if (!IS_NPC (ch))
    sec = ch->pcdata->security;
  else
    sec = 0;

  smash_tilde (argument);
  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);

  backup = (!str_cmp (arg2, "backup"));

  if (arg1[0] == '\0')
  {
    if (!ch || !ch->desc)
      return;

    send_to_char ("Syntax:\n\r", ch);
    send_to_char ("  asave <vnum>   - saves a particular area\n\r", ch);
    send_to_char ("  asave list     - saves the area.lst file\n\r", ch);
    send_to_char ("  asave area     - saves the area being edited\n\r", ch);
    send_to_char ("  asave changed  - saves all changed zones\n\r", ch);
    send_to_char ("  asave world    - saves the world! (db dump)\n\r", ch);
    if (ch->level >= L2)
      send_to_char
	("  asave (world/changed/list) backup    - saves everything "
	 "in the backup dir.\n\r", ch);

    send_to_char ("\n\r", ch);
    return;
  }

  /* Snarf the value (which need not be numeric). */
  value = atoi (arg1);
  if (!(pArea = get_area_data (value)) && is_number (arg1))
  {
    if (ch)
      send_to_char ("That area does not exist.\n\r", ch);
    return;
  }

  /* Save area of given vnum. */
  /* ------------------------ */

  if (is_number (arg1))
  {
    if (!IS_BUILDER (ch, pArea))
    {
      if (ch)
	send_to_char ("You are not a builder for this area.\n\r", ch);
      return;
    }
    save_area_list (backup);
    save_area (pArea, backup);
    return;
  }

  /* Save the world, only authorized areas. */
  /* -------------------------------------- */

  if (!str_cmp ("world", arg1))
  {
    save_area_list (backup);

    for (pArea = area_first; pArea; pArea = pArea->next)
    {
      /* Builder must be assigned this area. */
      if ((ch && (!IS_BUILDER (ch, pArea) && ch->level < ML)))
	continue;

      save_area (pArea, backup);

      REMOVE_BIT (pArea->area_flags, AREA_CHANGED);
    }

    if (ch)
      send_to_char ("You saved the world.\n\r", ch);

    if (sec > 8)
      save_helps (backup);

    return;
  }

  /* Save changed areas, only authorized areas. */
  /* ------------------------------------------ */

  if (!str_cmp ("changed", arg1))
  {
    char buf[MIL];

    save_area_list (backup);

    if (ch)
      send_to_char ("Saved zones:\n\r", ch);
    else
      log_string ("Saved zones:");

    sprintf (buf, "None.\n\r");

    for (pArea = area_first; pArea; pArea = pArea->next)
    {
      /* Builder must be assigned this area. */
      if (!IS_BUILDER (ch, pArea))
	continue;

      /* Save changed areas. */
      if (IS_SET (pArea->area_flags, AREA_CHANGED))
      {
	save_area (pArea, backup);
	sprintf (buf, "%35s - '%s'\n\r", pArea->name, pArea->file_name);
	if (ch)
	  send_to_char (buf, ch);
	else
	  log_string (buf);

	REMOVE_BIT (pArea->area_flags, AREA_CHANGED);
      }
    }
    if (!str_cmp (buf, "None.\n\r"))
    {
      if (ch)
	send_to_char (buf, ch);
      else
	log_string ("None.");
    }
    return;
  }

  /* Save the area.lst file. */
  /* ----------------------- */
  if (!str_cmp (arg1, "list"))
  {
    save_area_list (backup);
    return;
  }

  /* Save area being edited, if authorized. */
  /* -------------------------------------- */
  if (!str_cmp (arg1, "area"))
  {
    if (!ch || !ch->desc)
      return;

    /* Is character currently editing. */
    if (ch->desc->editor == ED_NONE)
    {
      send_to_char ("You are not editing an area, "
		    "therefore an area vnum is required.\n\r", ch);
      return;
    }

    /* Find the area to save. */
    switch (ch->desc->editor)
    {
    case ED_AREA:
      pArea = (AREA_DATA *) ch->desc->pEdit;
      break;
    case ED_ROOM:
      pArea = ch->in_room->area;
      break;
    case ED_OBJECT:
      pArea = ((OBJ_INDEX_DATA *) ch->desc->pEdit)->area;
      break;
    case ED_MOBILE:
      pArea = ((MOB_INDEX_DATA *) ch->desc->pEdit)->area;
      break;
    default:
      pArea = ch->in_room->area;
      break;
    }

    if (!IS_BUILDER (ch, pArea))
    {
      send_to_char ("You are not a builder for this area.\n\r", ch);
      return;
    }

    save_area_list (backup);
    save_area (pArea, backup);
    REMOVE_BIT (pArea->area_flags, AREA_CHANGED);
    send_to_char ("Area saved.\n\r", ch);
    return;
  }

  /* Show correct syntax. */
  /* -------------------- */
  if (ch)
    do_asave (ch, "");
  return;
}
