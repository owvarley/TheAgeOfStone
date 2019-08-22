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

/***************************************************************************
 *  File: olc.c                                                            *
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"

/*
 * Local functions.
 */
AREA_DATA *get_area_data (long vnum);
void show_olc_cmds (CHAR_DATA * ch, const struct olc_cmd_type *olc_table);


/* Executed from comm.c.  Minimizes compiling when changes are made. */
bool run_olc_editor (DESCRIPTOR_DATA * d)
{
  switch (d->editor)
  {
  case ED_AREA:
    aedit (d->character, d->incomm);
    break;
  case ED_ROOM:
    redit (d->character, d->incomm);
    break;
  case ED_OBJECT:
    oedit (d->character, d->incomm);
    break;
  case ED_MOBILE:
    medit (d->character, d->incomm);
    break;
  case ED_MPCODE:
    mpedit (d->character, d->incomm);
    break;
  case ED_GUILD:
    gedit (d->character, d->incomm);
    break;
  case ED_HELP:
    hedit (d->character, d->incomm);
    break;
  case ED_GROUP:
    gredit (d->character, d->incomm);
    break;
  case ED_SKILL:
    skedit (d->character, d->incomm);
    break;
  case ED_CMD:
    cedit (d->character, d->incomm);
    break;
  default:
    return FALSE;
  }
  return TRUE;
}



char *olc_ed_name (CHAR_DATA * ch)
{
  static char buf[10];

  buf[0] = '\0';
  switch (ch->desc->editor)
  {
  case ED_AREA:
    sprintf (buf, "aEdit");
    break;
  case ED_ROOM:
    sprintf (buf, "rEdit");
    break;
  case ED_OBJECT:
    sprintf (buf, "oEdit");
    break;
  case ED_MOBILE:
    sprintf (buf, "mEdit");
    break;
  case ED_MPCODE:
    sprintf (buf, "mpEdit");
    break;
  case ED_GUILD:
    sprintf (buf, "gEdit");
    break;
  case ED_HELP:
    sprintf (buf, "hEdit");
    break;
  case ED_GROUP:
    sprintf (buf, "grEdit");
    break;
  case ED_SKILL:
    sprintf (buf, "sEdit");
    break;
  case ED_CMD:
    sprintf (buf, "cEdit");
    break;
  default:
    sprintf (buf, " ");
    break;
  }
  return buf;
}



char *olc_ed_vnum (CHAR_DATA * ch)
{
  AREA_DATA *pArea;
  ROOM_INDEX_DATA *pRoom;
  OBJ_INDEX_DATA *pObj;
  MOB_INDEX_DATA *pMob;
  MPROG_CODE *pMprog;
  CLAN_DATA *pClan;
  GROUP_DATA *pGroup;
  SKILL_DATA *pSkill;
  CMD_DATA *pCmd;

  static char buf[10];

  buf[0] = '\0';
  switch (ch->desc->editor)
  {
  case ED_AREA:
    pArea = (AREA_DATA *) ch->desc->pEdit;
    sprintf (buf, "%ld", pArea ? pArea->vnum : 0);
    break;
  case ED_ROOM:
    pRoom = ch->in_room;
    sprintf (buf, "%ld", pRoom ? pRoom->vnum : 0);
    break;
  case ED_OBJECT:
    pObj = (OBJ_INDEX_DATA *) ch->desc->pEdit;
    sprintf (buf, "%d", pObj ? pObj->vnum : 0);
    break;
  case ED_MOBILE:
    pMob = (MOB_INDEX_DATA *) ch->desc->pEdit;
    sprintf (buf, "%ld", pMob ? pMob->vnum : 0);
    break;
  case ED_MPCODE:
    pMprog = (MPROG_CODE *) ch->desc->pEdit;
    sprintf (buf, "%ld", pMprog ? pMprog->vnum : 0);
    break;
  case ED_GUILD:
    pClan = (CLAN_DATA *) ch->desc->pEdit;
    sprintf (buf, "%d",
	     clan_lookup (pClan->name) ? clan_lookup (pClan->name) : 0);
    break;
  case ED_GROUP:
    pGroup = (GROUP_DATA *) ch->desc->pEdit;
    sprintf (buf, "%d", group_lookup (pGroup->name) ?
	     group_lookup (pGroup->name) : 0);
    break;
  case ED_SKILL:
    pSkill = (SKILL_DATA *) ch->desc->pEdit;
    sprintf (buf, "%s", pSkill ? pSkill->name : "");
    break;
  case ED_CMD:
    pCmd = (CMD_DATA *) ch->desc->pEdit;
    sprintf (buf, "%s", pCmd ? pCmd->name : "");
    break;
  default:
    sprintf (buf, " ");
    break;
  }

  return buf;
}



/*****************************************************************************
 Name:		show_olc_cmds
 Purpose:	Format up the commands from given table.
 Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds (CHAR_DATA * ch, const struct olc_cmd_type *olc_table)
{
  char buf[MSL];
  char buf1[MSL];
  int cmd;
  int col;

  buf1[0] = '\0';
  col = 0;
  for (cmd = 0; olc_table[cmd].name != NULL; cmd++)
  {
    sprintf (buf, "%-15.15s", olc_table[cmd].name);
    strcat (buf1, buf);
    if (++col % 5 == 0)
      strcat (buf1, "\n\r");
  }

  if (col % 5 != 0)
    strcat (buf1, "\n\r");

  send_to_char (buf1, ch);
  return;
}



/*****************************************************************************
 Name:		show_commands
 Purpose:	Display all olc commands.
 Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands (CHAR_DATA * ch, char *argument)
{
  switch (ch->desc->editor)
  {
  case ED_AREA:
    show_olc_cmds (ch, aedit_table);
    break;
  case ED_ROOM:
    show_olc_cmds (ch, redit_table);
    break;
  case ED_OBJECT:
    show_olc_cmds (ch, oedit_table);
    break;
  case ED_MOBILE:
    show_olc_cmds (ch, medit_table);
    break;
  case ED_MPCODE:
    show_olc_cmds (ch, mpedit_table);
    break;
  case ED_GUILD:
    show_olc_cmds (ch, gedit_table);
    break;
  case ED_HELP:
    show_olc_cmds (ch, hedit_table);
    break;
  case ED_GROUP:
    show_olc_cmds (ch, gredit_table);
    break;
  case ED_SKILL:
    show_olc_cmds (ch, skedit_table);
    break;
  case ED_CMD:
    show_olc_cmds (ch, cedit_table);
    break;
  }

  return FALSE;
}



/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/

const struct olc_cmd_type cedit_table[] = {
  {"commands", show_commands},
  {"?", show_help},
  {"show", cedit_show},
  {"disable", cedit_disable},
  {"enable", cedit_enable},
  {"list", cedit_list},
  {"log", cedit_log},
  {"position", cedit_position},
  {"name", cedit_name},
  {"alternate", cedit_lookup},
  {"save", cedit_save},
  {"reload", cedit_reload},
  {"level", cedit_level},
  {NULL, 0}
};

const struct olc_cmd_type skedit_table[] = {
  {"commands", show_commands},
  {"?", show_help},
  {"flowstr", skedit_flowstr},
  {"learn", skedit_learn},
  {"list", skedit_list},
  {"target", skedit_target},
  {"position", skedit_position},
  {"wait", skedit_wait},
  {"msgs", skedit_msgs},
  {"show", skedit_show},
  {"save", skedit_save},
  {"level", skedit_levels},
  {"flags", skedit_flags},
  {"guild", skedit_guild},
  {NULL, 0}
};


const struct olc_cmd_type gedit_table[] = {
  {"commands", show_commands},
  {"create", gedit_create},
  {"delete", gedit_delete},
  {"flag", gedit_flags},
  {"list", gedit_list},
  {"ml", gedit_ml},
  {"name", gedit_name},
  {"whoname", gedit_whoname},
  {"rank", gedit_rank},
  {"rooms", gedit_rooms},
  {"show", gedit_show},
  {"skill", gedit_skill},
  {"?", show_help},
  {NULL, 0}
};

const struct olc_cmd_type gredit_table[] = {
  {"commands", show_commands},
  {"create", gredit_create},
  {"delete", gredit_delete},
  {"flag", gredit_flags},
  {"list", gredit_list},
  {"name", gredit_name},
  {"ratings", gredit_ratings},
  {"save", gredit_save},
  {"show", gredit_show},
  {"skill", gredit_skill},
  {"?", show_help},
  {NULL, 0}
};


const struct olc_cmd_type aedit_table[] = {
/*  {   command		function	}, */

  {"age", aedit_age},
  {"builder", aedit_builder},	/* s removed -- Hugin */
  {"commands", show_commands},
  {"create", aedit_create},
  {"delete", aedit_delete},
  {"flag", aedit_flag},
  {"filename", aedit_file},
  {"name", aedit_name},
  {"reset", aedit_reset},
  {"sector", aedit_sector},
  {"security", aedit_security},
  {"show", aedit_show},
  {"sounds", aedit_sound},
  {"vnum", aedit_vnum},
  {"lvnum", aedit_lvnum},
  {"uvnum", aedit_uvnum},
  {"version", aedit_version},
  {"credits", aedit_credits},

  {"?", show_help},
  {"version", show_version},

  {NULL, 0}
};



const struct olc_cmd_type redit_table[] = {
/*  {   command		function	}, */

  {"commands", show_commands},
  {"copy", redit_copy},
  {"create", redit_create},
  {"desc", redit_desc},
  {"delete", redit_delete},
  {"ed", redit_ed},
  {"format", redit_format},
  {"name", redit_name},
  {"show", redit_show},
  {"sounds", redit_sound},
  {"heal", redit_heal},
  {"opu", redit_opu},
  {"clan", redit_clan},
  {"class", redit_class},
  {"north", redit_north},
  {"south", redit_south},
  {"east", redit_east},
  {"west", redit_west},
  {"up", redit_up},
  {"down", redit_down},

  /* New reset commands. */
  {"mreset", redit_mreset},
  {"mlist", redit_mlist},
  {"rlist", redit_rlist},
  {"olist", redit_olist},
  {"mshow", redit_mshow},
  {"oshow", redit_oshow},
  {"owner", redit_owner},
  {"room", redit_room},
  {"sector", redit_sector},
  {"mplist", redit_mplist},

  {"?", show_help},
  {"version", show_version},

  {NULL, 0}
};



const struct olc_cmd_type oedit_table[] = {
/*  {   command		function	}, */

  {"commands", show_commands},
  {"copy", oedit_copy},
  {"cost", oedit_cost},
  {"create", oedit_create},
  {"addaffect", oedit_addaffect},
  {"addapply", oedit_addapply},
  {"delaffect", oedit_delaffect},
  {"ed", oedit_ed},
  {"long", oedit_long},
  {"name", oedit_name},
  {"short", oedit_short},
  {"show", oedit_show},
  {"sound", oedit_sound},
  {"v0", oedit_value0},
  {"v1", oedit_value1},
  {"v2", oedit_value2},
  {"v3", oedit_value3},
  {"v4", oedit_value4},		/* ROM */
  {"weight", oedit_weight},

  {"extra", oedit_extra},	/* ROM */
  {"wear", oedit_wear},		/* ROM */
  {"type", oedit_type},		/* ROM */
  {"material", oedit_material},	/* ROM */
  {"level", oedit_level},	/* ROM */
  {"condition", oedit_condition},	/* ROM */

  {"?", show_help},
  {"version", show_version},

  {NULL, 0}
};



const struct olc_cmd_type medit_table[] = {
/*  {   command		function	}, */

  {"alignment", medit_align},
  {"autoset", medit_autoset},
  {"commands", show_commands},
  {"copy", medit_copy},
  {"create", medit_create},
  {"desc", medit_desc},
  {"level", medit_level},
  {"long", medit_long},
  {"name", medit_name},
  {"shop", medit_shop},
  {"short", medit_short},
  {"show", medit_show},
  {"sounds", medit_sound},
  {"spec", medit_spec},

  {"sex", medit_sex},		/* ROM */
  {"act", medit_act},		/* ROM */
  {"affect", medit_affect},	/* ROM */
  {"armor", medit_ac},		/* ROM */
  {"form", medit_form},		/* ROM */
  {"part", medit_part},		/* ROM */
  {"imm", medit_imm},		/* ROM */
  {"res", medit_res},		/* ROM */
  {"vuln", medit_vuln},		/* ROM */
  {"material", medit_material},	/* ROM */
  {"off", medit_off},		/* ROM */
  {"size", medit_size},		/* ROM */
  {"hitdice", medit_hitdice},	/* ROM */
  {"opudice", medit_opudice},	/* ROM */
  {"damdice", medit_damdice},	/* ROM */
  {"race", medit_race},		/* ROM */
  {"position", medit_position},	/* ROM */
  {"wealth", medit_gold},	/* ROM */
  {"hitroll", medit_hitroll},	/* ROM */
  {"damtype", medit_damtype},	/* ROM */
  {"group", medit_group},	/* ROM */
  {"addmprog", medit_addmprog},	/* ROM */
  {"delmprog", medit_delmprog},	/* ROM */

  {"?", show_help},
  {"version", show_version},

  {NULL, 0}
};



/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/

/** Function: get_cmd_data
  * Descr   : Returns a pointer to the cmd_table of the requested command #
  * Returns : (CMD_DATA *)
  * Syntax  : (n/a)
  * Written : v1.0 1/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
CMD_DATA *get_cmd_data (int cmd)
{
  if (cmd < maxCommands - 1)
    return &cmd_table[cmd];

  return &cmd_table[0];
}

/** Function: get_clan_data
  * Descr   : Returns a pointer to the clan_table of the requested clan #
  * Returns : (CLAN_DATA *)
  * Syntax  : (n/a)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
CLAN_DATA *get_clan_data (int clan)
{
  if (clan < maxClan)
    return &clan_table[clan];

  return &clan_table[0];	/* first clan */
}

/** Function: get_skill_data
  * Descr   : Returns a pointer to the skill_table of the requested skill #
  * Returns : (SKILL_DATA *)
  * Syntax  : (n/a)
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
SKILL_DATA *get_skill_data (int skill)
{
  if (skill < maxSkill && !IS_NULLSTR(skill_table[skill].name))
    return &skill_table[skill];

  return &skill_table[0];	/* default skill */
}


/** Function: get_group_data
  * Descr   : Returns a pointer to the group_table of the requested group #
  * Returns : (GROUP_DATA *)
  * Syntax  : (n/a)
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
GROUP_DATA *get_group_data (int group)
{
  if (group <= maxGroup && !IS_NULLSTR(group_table[group].name))
    return &group_table[group];

  return &group_table[0];	/* first group */
}


/*****************************************************************************
 Name:		get_area_data
 Purpose:	Returns pointer to area with given vnum.
 Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data (long vnum)
{
  AREA_DATA *pArea;

  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    if (pArea->vnum == vnum)
      return pArea;
  }

  return 0;
}

/*****************************************************************************
 Name:		edit_done
 Purpose:	Resets builder information on completion.
 Called by:	aedit, redit, oedit, medit(olc.c)
 ****************************************************************************/
bool edit_done (CHAR_DATA * ch)
{
  ch->desc->pEdit = NULL;
  ch->desc->editor = ED_NONE;
  return FALSE;
}



/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/

/** Function: cedit
  * Descr   : Interprets commands sent while inside the command editor.
  *         : Passing appropriate commands on to editor, rest to mud.
  * Returns : (N/A)
  * Syntax  : (N/A| called by do_cedit only)
  * Written : v1.0 1/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void cedit (CHAR_DATA * ch, char *argument)
{
  CMD_DATA *pCmd = NULL;
  char command[MIL];
  char arg[MIL];

  int cmd;

  EDIT_CMD (ch, pCmd);
  memset (arg, 0, MIL);

  sprintf (arg, "%s", argument);

  argument = one_argument (argument, command);

  if (ch->level < MAX_LEVEL - 2)
  {
    send_to_char ("Insuffecient security to modify command data", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    cedit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; !IS_NULLSTR (cedit_table[cmd].name); cmd++)
  {
    if (!str_prefix (command, cedit_table[cmd].name))
    {
      if ((*cedit_table[cmd].olc_fun) (ch, argument))
		SET_BIT (pCmd->flags, CMD_CHANGED);
      return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}



/** Function: gredit
  * Descr   : Interprets commands sent while inside the group editor.
  * Returns : (N/A)
  * Syntax  : (N/A| called by do_gredit only)
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void gredit (CHAR_DATA * ch, char *argument)
{
  GROUP_DATA *pGroup;
  char command[MIL];
  char arg[MIL];

  int cmd;

  EDIT_GROUP (ch, pGroup);
  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  if (ch->level < MAX_LEVEL - 2)
  {
    send_to_char ("Insuffecient security to modify group data", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (pGroup && str_cmp (command, "delete") &&
      IS_FLAG (pGroup->flags, _DELETED))
  {
    /* remove bit so it's not deleted in the future */
    chprintf (ch, "Delete cancelled for group: %s\n\r", pGroup->name);
    REMOVE_FLAG (pGroup->flags, _DELETED);
  }

  if (command[0] == '\0')
  {
    gredit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; gredit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, gredit_table[cmd].name))
    {

      if ((*gredit_table[cmd].olc_fun) (ch, argument))
 		SET_FLAG (pGroup->flags, _CHANGED);

	  return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, command);
  return;
}


/** Function: skedit
  * Descr   : Interprets commands sent while inside the skill editor.
  *         : Passing appropriate commands on to editor, rest to mud.
  * Returns : (N/A)
  * Syntax  : (N/A| called by do_skedit only)
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void skedit (CHAR_DATA * ch, char *argument)
{
  SKILL_DATA *pSkill;
  char command[MIL];
  char arg[MIL];

  int cmd;

  EDIT_SKILL (ch, pSkill);
  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  if (ch->level < MAX_LEVEL - 2)
  {
    send_to_char ("Insuffecient security to modify skill data", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    skedit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; skedit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, skedit_table[cmd].name))
    {
      if ((*skedit_table[cmd].olc_fun) (ch, argument))
		SET_FLAG (pSkill->flags, _CHANGED);
	
	  return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}


/** Function: gedit
  * Descr   : Interprets commands sent while inside the guild editor.
  *         : Passing appropriate commands on to editor, rest to mud.
  * Returns : (N/A)
  * Syntax  : (N/A| called by do_gedit only)
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void gedit (CHAR_DATA * ch, char *argument)
{
  CLAN_DATA *pClan;
  char command[MIL];
  char arg[MIL];

  int cmd;

  EDIT_GUILD (ch, pClan);
  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  if (ch->level < MAX_LEVEL - 2)
  {
    send_to_char ("Insuffecient security to modify guild data", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    gedit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; gedit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, gedit_table[cmd].name))
    {
      if ((*gedit_table[cmd].olc_fun) (ch, argument))
		SET_FLAG (pClan->flags, _CHANGED);

	  return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}

/* Area Interpreter, called by do_aedit. */
void aedit (CHAR_DATA * ch, char *argument)
{
  AREA_DATA *pArea;
  char command[MIL];
  char arg[MIL];
  int cmd;
  int value;

  EDIT_AREA (ch, pArea);
  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  if (!IS_BUILDER (ch, pArea))
  {
    send_to_char ("AEdit:  Insufficient security to modify area.\n\r", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    aedit_show (ch, argument);
    return;
  }

  if ((value = flag_value (area_flags, command)) != NO_FLAG)
  {
    TOGGLE_BIT (pArea->area_flags, value);

    send_to_char ("Flag toggled.\n\r", ch);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; aedit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, aedit_table[cmd].name))
    {
      if ((*aedit_table[cmd].olc_fun) (ch, argument))
		SET_BIT (pArea->area_flags, AREA_CHANGED);

      return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}



/* Room Interpreter, called by do_redit. */
void redit (CHAR_DATA * ch, char *argument)
{
  AREA_DATA *pArea;
  ROOM_INDEX_DATA *pRoom;
  char arg[MSL];
  char command[MIL];
  int cmd;

  EDIT_ROOM (ch, pRoom);
  pArea = pRoom->area;

  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  if (!IS_BUILDER (ch, pArea) && get_trust (ch) < pArea->security)
  {
    send_to_char ("REdit:  Insufficient security to modify room.\n\r", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  else if (!str_cmp (command, "delete"))
  {
    redit_delete (ch, argument);
    return;
  }

  if (command[0] == '\0')
  {
    redit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; redit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, redit_table[cmd].name))
    {
      if ((*redit_table[cmd].olc_fun) (ch, argument))
		SET_BIT (pArea->area_flags, AREA_CHANGED);

      return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}



/* Object Interpreter, called by do_oedit. */
void oedit (CHAR_DATA * ch, char *argument)
{
  AREA_DATA *pArea;
  OBJ_INDEX_DATA *pObj;
  char arg[MSL];
  char command[MIL];
  int cmd;

  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  EDIT_OBJ (ch, pObj);
  pArea = pObj->area;

  if (!IS_BUILDER (ch, pArea) && get_trust (ch) < pArea->security)
  {
    send_to_char ("OEdit: Insufficient security to modify area.\n\r", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    oedit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; oedit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, oedit_table[cmd].name))
    {
      if ((*oedit_table[cmd].olc_fun) (ch, argument))
		SET_BIT (pArea->area_flags, AREA_CHANGED);

	  return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}



/* Mobile Interpreter, called by do_medit. */
void medit (CHAR_DATA * ch, char *argument)
{
  AREA_DATA *pArea;
  MOB_INDEX_DATA *pMob;
  char command[MIL];
  char arg[MSL];
  int cmd;

  smash_tilde (argument);
  strcpy (arg, argument);
  argument = one_argument (argument, command);

  EDIT_MOB (ch, pMob);
  pArea = pMob->area;

  if (!IS_BUILDER (ch, pArea))
  {
    send_to_char ("MEdit: Insufficient security to modify area.\n\r", ch);
    edit_done (ch);
    return;
  }

  if (!str_cmp (command, "done"))
  {
    edit_done (ch);
    return;
  }

  if (command[0] == '\0')
  {
    medit_show (ch, argument);
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; medit_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, medit_table[cmd].name))
    {
      if ((*medit_table[cmd].olc_fun) (ch, argument))
		SET_BIT (pArea->area_flags, AREA_CHANGED);

	  return;
    }
  }

  /* Default to Standard Interpreter. */
  interpret (ch, arg);
  return;
}



const struct editor_cmd_type editor_table[] = {
/*  {   command		function	}, */

  {"area", do_aedit},
  {"guild", do_gedit},
  {"group", do_gredit},
  {"room", do_redit},
  {"object", do_oedit},
  {"mobile", do_medit},
  {"mpcode", do_mpedit},
  {"help", do_hedit},
  {"skill", do_skedit},
  {NULL, 0,}
};


/* Entry point for all editors. */
_DOFUN (do_olc)
{
  char command[MIL];
  int cmd;

  if (IS_NPC (ch))
    return;

  argument = one_argument (argument, command);

  if (command[0] == '\0')
  {
    do_help (ch, "olc");
    return;
  }

  /* Search Table and Dispatch Command. */
  for (cmd = 0; editor_table[cmd].name != NULL; cmd++)
  {
    if (!str_prefix (command, editor_table[cmd].name))
    {
      (*editor_table[cmd].do_fun) (ch, argument);
      return;
    }
  }

  /* Invalid command, send help. */
  do_help (ch, "olc");
  return;
}


/** Function: do_gredit
  * Descr   : Places user into the group editor. Verifies security.
  * Returns : Message sent if improper security level.
  * Syntax  : gredit [group #|create]
  * Written : v1.0 7/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_gredit)
{
  GROUP_DATA *pGroup;
  int value;
  char arg[MSL];

  if (IS_NPC (ch))
    return;

  pGroup = &group_table[0];	/* set default */

  argument = one_argument (argument, arg);

  if (is_number (arg))
  {
    value = atoi (arg);
    pGroup = get_group_data (value);
    if (!pGroup->name || pGroup->name[0] == '\0')
    {
      send_to_char ("That group does not exist.\n\r", ch);
      return;
    }
  }
  else if (!str_cmp (arg, "create"))
  {
    int i = 0;

    if (ch->pcdata->security < 9)
    {
      send_to_char
	("GREdit : Insuffecient security to create new groups.\n\r", ch);
      return;
    }

    gredit_create (ch, "");

    /* since the group just created will be that last group in the
       group_table, we search backwards until we find one with a name */

    for (i = maxGroup; i > 0; i--)
      if (group_table[i].name != NULL)
	break;

    pGroup = get_group_data (i);

    ch->desc->editor = ED_GROUP;
    return;
  }

  ch->desc->pEdit = (void *) pGroup;
  ch->desc->editor = ED_GROUP;
  return;
}


/** Function: do_gedit
  * Descr   : Places user into the guild editor. Verifying Security.
  * Returns : Message if improper security level.
  * Syntax  : gedit [guild #|create]
  * Written : v1.0 3/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_gedit)
{
  CLAN_DATA *pClan;
  int value;
  char arg[MSL];

  if (IS_NPC (ch))
    return;

  pClan = &clan_table[0];	/* Storage/default */

  argument = one_argument (argument, arg);

  value = atoi (arg);
  if (value > 0)
  {
    pClan = get_clan_data (--value);

    if (!pClan || !pClan->name || pClan->name[0] == '\0')
    {
      send_to_char ("That guild does not exist.\n\r", ch);
      return;
    }

  }
  else if (!str_cmp (arg, "create"))
  {
    int i = 0;

    if (ch->pcdata->security < 9)
    {
      send_to_char ("GEdit : Insuffecient security to create new guilds.\n\r",
		    ch);
      return;
    }

    gedit_create (ch, "");

    /* since the clan just created will be that last clan in the
       clan_table, we search backwards until we find one with a name */

    for (i = maxClan; i > 0; i--)
      if (clan_table[i].name != NULL)
	break;

    pClan = get_clan_data (i);

    ch->desc->editor = ED_GUILD;
    return;
  }

  ch->desc->pEdit = (void *) pClan;
  ch->desc->editor = ED_GUILD;
  return;
}

/** Function: do_cedit
  * Descr   : Places user into the command editor. Verifying Security.
  * Returns : Message if improper security level.
  * Syntax  : cedit [cmd #|create]
  * Written : v1.0 1/00
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_cedit)
{
  CMD_DATA *pCmd = NULL;
  int value = -1;
  char arg[MSL];

  if (IS_NPC (ch))
    return;

  pCmd = &cmd_table[0];		/* Storage/default */

  argument = one_argument (argument, arg);

  if (!is_number (arg))
  {
    int i = 0;

    for (i = 0; i != maxCommands; i++)
      if (!str_prefix (arg, cmd_table[i].name))
      {
	value = i + 1;
	break;
      }
  }
  else
    value = atoi (arg);

  if (value > 0)
  {
    pCmd = get_cmd_data (--value);

    if (!pCmd || !pCmd->name || pCmd->name[0] == '\0')
    {
      send_to_char ("That command does not exist.\n\r", ch);
      return;
    }

  }

  ch->desc->pEdit = (void *) pCmd;
  ch->desc->editor = ED_CMD;
  return;
}


/** Function: do_skedit
  * Descr   : Places user into the skill editor. Verifies Security.
  * Returns : Message if improper security level.
  * Syntax  : skedit [skill #]
  * Written : v1.0 1/99
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
_DOFUN (do_skedit)
{
  SKILL_DATA *pSkill;
  int value = 0;
  char arg[MSL];

  if (IS_NPC (ch))
    return;

  pSkill = &skill_table[0];	/* set default */
  memset (arg, 0, MSL);

  argument = one_argument (argument, arg);

  if (arg[0] != '\0')
  {
    if (is_number (arg))
      value = atoi (arg) - 1;
    else
      value = skill_lookup (arg);
  }

  if (value > 0)
    pSkill = get_skill_data (value);

  if (!pSkill || !pSkill->name || pSkill->name[0] == '\0')
  {
    send_to_char ("That skill does not exist.\n\r", ch);
    return;
  }

  ch->desc->pEdit = (void *) pSkill;
  ch->desc->editor = ED_SKILL;
  return;
}



/* Entry point for editing area_data. */
_DOFUN (do_aedit)
{
  AREA_DATA *pArea;
  long value;
  char arg[MSL];

  if (IS_NPC (ch))
    return;

  pArea = ch->in_room->area;

  argument = one_argument (argument, arg);

  if (is_number (arg))
  {
    value = atol (arg);
    if (!(pArea = get_area_data (value)))
    {
      send_to_char ("That area vnum does not exist.\n\r", ch);
      return;
    }
  }
  else if (!str_cmp (arg, "create"))
  {
    if (ch->pcdata->security < 9)
    {
      send_to_char ("AEdit : You don't have proper security to create an area.\n\r",
		    ch);
      return;
    }

    aedit_create (ch, "");
    ch->desc->editor = ED_AREA;
    return;
  }

  if (!IS_BUILDER (ch, pArea))
  {
    send_to_char ("AEdit:  You don't have proper security for that area.\n\r", ch);
    return;
  }

  ch->desc->pEdit = (void *) pArea;
  ch->desc->editor = ED_AREA;
  return;
}



/* Entry point for editing room_index_data. */
_DOFUN (do_redit)
{
  ROOM_INDEX_DATA *pRoom;
  char arg1[MSL];

  if (IS_NPC (ch))
    return;

  argument = one_argument (argument, arg1);

  pRoom = ch->in_room;

  if (!str_cmp (arg1, "reset"))	/* redit reset */
  {
    if (!IS_BUILDER (ch, pRoom->area))
    {
      send_to_char ("REdit:  You don't have proper security for that.\n\r", ch);
      return;
    }

    reset_room (pRoom);
    send_to_char ("Room reset.\n\r", ch);

    return;
  }
  else
   if (!str_cmp (arg1, "create"))	/* redit create <vnum> */
  {
    if (argument[0] == '\0' || atol (argument) == 0)
    {
      send_to_char ("Syntax:  edit room create [vnum]\n\r", ch);
      return;
    }

    if (redit_create (ch, argument))	/* pEdit == nuevo cuarto */
    {
      ch->desc->editor = ED_ROOM;
      char_from_room (ch);
      char_to_room (ch, ch->desc->pEdit);
      SET_BIT (((ROOM_INDEX_DATA *) ch->desc->pEdit)->area->area_flags,
	       AREA_CHANGED);
    }

    return;
  }
  else
   if (!IS_NULLSTR (arg1))	/* redit <vnum> */
  {
    pRoom = get_room_index (atol (arg1));

    if (!pRoom)
    {
      send_to_char ("REdit : That room does not exist.\n\r", ch);
      return;
    }

    if (!IS_BUILDER (ch, pRoom->area))
    {
      send_to_char ("REdit:  You don't have proper security for that room.\n\r",
		    ch);
      return;
    }

    if (!is_room_owner (ch, pRoom) && room_is_private (pRoom)
	&& get_trust (ch) < ML - 2)
    {
      send_to_char ("That room is private right now.\n\r", ch);
      return;
    }
    else

    {
      CHAR_DATA *rch;

      for (rch = pRoom->people; rch != NULL; rch = rch->next_in_room)
      {
	if (!IS_NPC (rch)
	    && (IS_FLAG (rch->states.config, _BUILDING)
		&& get_trust (ch) < get_trust (rch)))
	{
	  send_to_char ("REdit:  Someone is currently building in that room.\n\r",
			ch);
	  return;
	}
      }
    }

    char_from_room (ch);
    char_to_room (ch, pRoom);
  }

  if (!IS_BUILDER (ch, pRoom->area))
  {
    send_to_char ("REdit : You don't have proper security for that room.\n\r", ch);
    return;
  }

  ch->desc->pEdit = (void *) pRoom;
  ch->desc->editor = ED_ROOM;

  return;
}



/* Entry point for editing obj_index_data. */
_DOFUN (do_oedit)
{
  OBJ_INDEX_DATA *pObj;
  AREA_DATA *pArea;
  char arg1[MSL];
  long value;

  if (IS_NPC (ch))
    return;

  argument = one_argument (argument, arg1);

  if (is_number (arg1))
  {
    value = atol (arg1);
    if (!(pObj = get_obj_index (value)))
    {
      send_to_char ("OEdit:  That vnum does not exist.\n\r", ch);
      return;
    }

    if (!IS_BUILDER (ch, pObj->area))
    {
      send_to_char ("OEdit:  Insuffecient security.\n\r", ch);
      return;
    }

    ch->desc->pEdit = (void *) pObj;
    ch->desc->editor = ED_OBJECT;
    return;
  }
  else
  {
    if (!str_cmp (arg1, "delete"))
      oedit_delete (ch, argument);

    if (!str_cmp (arg1, "create"))
    {
      value = atol (argument);
      if (argument[0] == '\0' || value == 0)
      {
	send_to_char ("Syntax:  edit object create [vnum]\n\r", ch);
	return;
      }

      pArea = get_vnum_area (value);

      if (!pArea)
      {
	send_to_char ("OEdit:  That vnum is not assigned an area.\n\r", ch);
	return;
      }

      if (!IS_BUILDER (ch, pArea))
      {
	send_to_char ("OEdit:  You don't have proper security for that object.\n\r",
		      ch);
	return;
      }

      if (oedit_create (ch, argument))
      {
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	ch->desc->editor = ED_OBJECT;
      }
      return;
    }
  }

  send_to_char ("OEdit:  There is no default object to edit.\n\r", ch);
  return;
}



/* Entry point for editing mob_index_data. */
_DOFUN (do_medit)
{
  MOB_INDEX_DATA *pMob;
  AREA_DATA *pArea;
  long value;
  char arg1[MSL];

  argument = one_argument (argument, arg1);

  if (IS_NPC (ch))
    return;

  if (is_number (arg1))
  {
    value = atol (arg1);
    if (!(pMob = get_mob_index (value)))
    {
      send_to_char ("MEdit:  That vnum does not exist.\n\r", ch);
      return;
    }

    if (!IS_BUILDER (ch, pMob->area))
    {
      send_to_char ("MEdit:  You don't have proper security for that mob.\n\r", ch);
      return;
    }

    ch->desc->pEdit = (void *) pMob;
    ch->desc->editor = ED_MOBILE;
    return;
  }
  else
  {
    if (!str_cmp (arg1, "delete"))
      medit_delete (ch, argument);
    else
     if (!str_cmp (arg1, "create"))
    {
      value = atol (argument);
      if (arg1[0] == '\0' || value == 0)
      {
	send_to_char ("Syntax:  edit mobile create [vnum]\n\r", ch);
	return;
      }

      pArea = get_vnum_area (value);

      if (!pArea)
      {
	send_to_char ("OEdit:  That vnum is not assigned an area.\n\r", ch);
	return;
      }

      if (!IS_BUILDER (ch, pArea))
      {
	send_to_char ("MEdit:  You don't have proper security for that mob.\n\r", ch);
	return;
      }

      if (medit_create (ch, argument))
      {
	SET_BIT (pArea->area_flags, AREA_CHANGED);
	ch->desc->editor = ED_MOBILE;
      }
      return;
    }
  }

  send_to_char ("MEdit:  There is no default mobile to edit.\n\r", ch);
  return;
}



void display_resets (CHAR_DATA * ch)
{
  ROOM_INDEX_DATA *pRoom;
  RESET_DATA *pReset;
  MOB_INDEX_DATA *pMob = NULL;
  char buf[MSL];
  char final[MSL];
  int iReset = 0;

  EDIT_ROOM (ch, pRoom);
  final[0] = '\0';

  send_to_char
    (" No.  Loads   Description   Location            Vnum     Mx Mn Ch  Description"
     "\n\r"
     "==== ======== ============= =================== ======== ========= ==========="
     "\n\r", ch);

  for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
  {
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_INDEX_DATA *pObjToIndex;
    ROOM_INDEX_DATA *pRoomIndex;

    final[0] = '\0';
    sprintf (final, "[%2d] ", ++iReset);

    switch (pReset->command)
    {
    default:
      sprintf (buf, "Bad reset command: %c.", pReset->command);
      strcat (final, buf);
      break;

    case 'M':
      if (!(pMobIndex = get_mob_index (pReset->arg1)))
      {
	sprintf (buf, "Load Mobile - Bad Mob %ld\n\r", pReset->arg1);
	strcat (final, buf);
	continue;
      }

      if (!(pRoomIndex = get_room_index (pReset->arg3)))
      {
	sprintf (buf, "Load Mobile - Bad Room %ld\n\r", pReset->arg3);
	strcat (final, buf);
	continue;
      }

      pMob = pMobIndex;
      sprintf (buf,
	       "M[%5ld] %-13.13s in room             R[%5ld] %2ld|%2ld 100 %-11.11s\n\r",
	       pReset->arg1, pMob->short_descr, pReset->arg3, pReset->arg2,
	       pReset->arg4, pRoomIndex->name);
      strcat (final, buf);

      /*
       * Check for pet shop.
       * -------------------
       */
      {
	ROOM_INDEX_DATA *pRoomIndexPrev;

	pRoomIndexPrev = get_room_index (pRoomIndex->vnum - 1);
	if (pRoomIndexPrev
	    && IS_SET (pRoomIndexPrev->room_flags, ROOM_PET_SHOP))
	  final[5] = 'P';
      }

      break;

    case 'O':
      if (!(pObjIndex = get_obj_index (pReset->arg1)))
      {
	sprintf (buf, "Load Object - Bad Object %ld\n\r", pReset->arg1);
	strcat (final, buf);
	continue;
      }

      pObj = pObjIndex;

      if (!(pRoomIndex = get_room_index (pReset->arg3)))
      {
	sprintf (buf, "Load Object - Bad Room %ld\n\r", pReset->arg3);
	strcat (final, buf);
	continue;
      }

      sprintf (buf, "O[%5ld] %-13.13s in room             "
	       "R[%5ld] %2ld|%2ld  %3d %-11.11s\n\r",
	       pReset->arg1, pObj->short_descr,
	       pReset->arg3,
	       pReset->arg2, pReset->arg4, pReset->chance, pRoomIndex->name);
      strcat (final, buf);

      break;

    case 'P':
      if (!(pObjIndex = get_obj_index (pReset->arg1)))
      {
	sprintf (buf, "Put Object - Bad Object %ld\n\r", pReset->arg1);
	strcat (final, buf);
	continue;
      }

      pObj = pObjIndex;

      if (!(pObjToIndex = get_obj_index (pReset->arg3)))
      {
	sprintf (buf, "Put Object - Bad To Object %ld\n\r", pReset->arg3);
	strcat (final, buf);
	continue;
      }

      sprintf (buf,
	       "O[%5ld] %-13.13s inside              O[%5ld] %2ld|%2ld  %3d %-11.11s\n\r",
	       pReset->arg1,
	       pObj->short_descr,
	       pReset->arg3,
	       pReset->arg2,
	       pReset->arg4, pReset->chance, pObjToIndex->short_descr);
      strcat (final, buf);

      break;

    case 'G':
    case 'E':
      if (!(pObjIndex = get_obj_index (pReset->arg1)))
      {
	sprintf (buf, "Give/Equip Object - Bad Object %ld\n\r", pReset->arg1);
	strcat (final, buf);
	continue;
      }

      pObj = pObjIndex;

      if (!pMob)
      {
	sprintf (buf, "Give/Equip Object - No Previous Mobile\n\r");
	strcat (final, buf);
	break;
      }

      if (pMob->pShop)
      {
	sprintf (buf,
		 "O[%5ld] %-13.13s in the inventory of S[%5ld]       %3d %-11.11s\n\r",
		 pReset->arg1,
		 pObj->short_descr,
		 pMob->vnum, pReset->chance, pMob->short_descr);
      }
      else
	sprintf (buf,
		 "O[%5ld] %-13.13s %-19.19s M[%5ld] %2ld|%2ld  %3d %-11.11s\n\r",
		 pReset->arg1,
		 pObj->short_descr,
		 (pReset->command == 'G') ?
		 flag_string (wear_loc_strings, WEAR_NONE)
		 : flag_string (wear_loc_strings, pReset->arg3),
		 pMob->vnum,
		 pReset->arg2,
		 pReset->arg4, pReset->chance, pMob->short_descr);
      strcat (final, buf);

      break;

      /*
       * Doors are set in rs_flags don't need to be displayed.
       * If you want to display them then uncomment the new_reset
       * line in the case 'D' in load_resets in db.c and here.
       */
    case 'D':
      pRoomIndex = get_room_index (pReset->arg1);
      sprintf (buf, "R[%5ld] %s door of %-19.19s reset to %s\n\r",
	       pReset->arg1,
	       capitalize (dir_name[pReset->arg2]),
	       pRoomIndex->name, flag_string (door_resets, pReset->arg3));
      strcat (final, buf);

      break;
      /*
       * End Doors Comment.
       */
    case 'R':
      if (!(pRoomIndex = get_room_index (pReset->arg1)))
      {
	sprintf (buf, "Randomize Exits - Bad Room %ld\n\r", pReset->arg1);
	strcat (final, buf);
	continue;
      }

      sprintf (buf, "R[%5ld] Exits are randomized in %s\n\r",
	       pReset->arg1, pRoomIndex->name);
      strcat (final, buf);

      break;
    }
    send_to_char (final, ch);
  }

  return;
}



/*****************************************************************************
 Name:		add_reset
 Purpose:	Inserts a new reset in the given index slot.
 Called by:	do_resets(olc.c).
 ****************************************************************************/
void add_reset (ROOM_INDEX_DATA * room, RESET_DATA * pReset, int index)
{
  RESET_DATA *reset;
  int iReset = 0;

  if (!room->reset_first)
  {
    room->reset_first = pReset;
    room->reset_last = pReset;
    pReset->next = NULL;
    return;
  }

  index--;

  if (index == 0)		/* First slot (1) selected. */
  {
    pReset->next = room->reset_first;
    room->reset_first = pReset;
    return;
  }

  /*
   * If negative slot( <= 0 selected) then this will find the last.
   */
  for (reset = room->reset_first; reset->next; reset = reset->next)
  {
    if (++iReset == index)
      break;
  }

  pReset->next = reset->next;
  reset->next = pReset;
  if (!pReset->next)
    room->reset_last = pReset;
  return;
}



_DOFUN (do_resets)
{
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char arg4[MIL];
  char arg5[MIL];
  char arg6[MIL];
  char arg7[MIL];

  RESET_DATA *pReset = NULL;

  argument = one_argument (argument, arg1);
  argument = one_argument (argument, arg2);
  argument = one_argument (argument, arg3);
  argument = one_argument (argument, arg4);
  argument = one_argument (argument, arg5);
  argument = one_argument (argument, arg6);
  argument = one_argument (argument, arg7);

  if (!IS_BUILDER (ch, ch->in_room->area))
  {
    send_to_char ("Resets: Invalid security for editing this area.\n\r", ch);
    return;
  }

  /*
   * Display resets in current room.
   * -------------------------------
   */
  if (arg1[0] == '\0')
  {
    if (ch->in_room->reset_first)
    {
      send_to_char ("Resets: M = mobile, R = room, O = object, "
		    "P = pet, S = shopkeeper\n\r", ch);
      display_resets (ch);
    }
    else
      send_to_char ("No resets in this room.\n\r", ch);
  }


  /*
   * Take index number and search for commands.
   * ------------------------------------------
   */
  if (is_number (arg1))
  {
    ROOM_INDEX_DATA *pRoom = ch->in_room;

    /*
     * Delete a reset.
     * ---------------
     */
    if (!str_cmp (arg2, "delete"))
    {
      int insert_loc = atoi (arg1);

      if (!ch->in_room->reset_first)
      {
	send_to_char ("No resets in this area.\n\r", ch);
	return;
      }

      if (insert_loc - 1 <= 0)
      {
	pReset = pRoom->reset_first;
	pRoom->reset_first = pRoom->reset_first->next;
	if (!pRoom->reset_first)
	  pRoom->reset_last = NULL;
      }
      else
      {
	int iReset = 0;
	RESET_DATA *prev = NULL;

	for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
	{
	  if (++iReset == insert_loc)
	    break;
	  prev = pReset;
	}

	if (!pReset)
	{
	  send_to_char ("Reset not found.\n\r", ch);
	  return;
	}

	if (prev)
	  prev->next = prev->next->next;
	else
	  pRoom->reset_first = pRoom->reset_first->next;

	for (pRoom->reset_last = pRoom->reset_first;
	     pRoom->reset_last->next;
	     pRoom->reset_last = pRoom->reset_last->next);
      }

      free_reset_data (pReset);
      send_to_char ("Reset deleted.\n\r", ch);
    }
    else
      /*
       * Set a resets chance.
       * ---------------
       */
    if (!str_cmp (arg2, "chance"))
    {
      int which = atoi (arg1);
      int iReset = 0;
      RESET_DATA *prev = NULL;
      sh_int value = -1;

      if (!ch->in_room->reset_first)
      {
	send_to_char ("No resets in this area.\n\r", ch);
	return;
      }

      if (is_number (arg3))
      {
	value = atoi (arg3);
      }

      if (value < 0 || value > 100)
      {
	send_to_char ("Syntax: reset chance % [0-100]\n\r", ch);
	return;
      }

      for (pReset = pRoom->reset_first; pReset; pReset = pReset->next)
      {
	if (++iReset == which)
	  break;
	prev = pReset;
      }

      if (!pReset)
      {
	send_to_char ("Reset not found.\n\r", ch);
	return;
      }

      pReset->chance = value;
      chprintf (ch, "Chance set to %d%%\n\r", pReset->chance);

    }
    else
      /*
       * Add a reset.
       * ------------
       */
      if ((!str_cmp (arg2, "mob") && is_number (arg3))
	  || (!str_cmp (arg2, "obj") && is_number (arg3)))
    {
      /*
       * Check for Mobile reset.
       * -----------------------
       */
      if (!str_cmp (arg2, "mob"))
      {
	if (get_mob_index (is_number (arg3) ? atol (arg3) : 1) == NULL)
	{
	  send_to_char ("That mob does not exist.\n\r", ch);
	  return;
	}
	pReset = new_reset_data ();
	pReset->command = 'M';
	pReset->arg1 = atol (arg3);
	pReset->arg2 = is_number (arg4) ? atol (arg4) : 1;	/* Max # */
	pReset->arg3 = ch->in_room->vnum;
	pReset->arg4 = is_number (arg5) ? atol (arg5) : 1;	/* Min # */
      }
      else
	/*
	 * Check for Object reset.
	 * -----------------------
	 */
      if (!str_cmp (arg2, "obj"))
      {
	pReset = new_reset_data ();
	pReset->arg1 = atol (arg3);
	/*
	 * Inside another object.
	 * ----------------------
	 */
	if (!str_prefix (arg4, "inside"))
	{
	  OBJ_INDEX_DATA *temp;

	  temp = get_obj_index (is_number (arg5) ? atol (arg5) : 1);
	  if ((temp->item_type != ITEM_CONTAINER) &&
	      (temp->item_type != ITEM_CORPSE_NPC))
	  {
	    send_to_char ("The second object is not a container.\n\r", ch);
	    return;
	  }
	  pReset->command = 'P';
	  pReset->arg2 = is_number (arg6) ? atol (arg6) : 1;
	  pReset->arg3 = is_number (arg5) ? atol (arg5) : 1;
	  pReset->arg4 = is_number (arg7) ? atol (arg7) : 1;

	}
	else
	  /*
	   * Inside the room.
	   * ----------------
	   */
	if (!str_cmp (arg4, "room"))
	{
	  if (get_obj_index (atol (arg3)) == NULL)
	  {
	    send_to_char ("That vnum does not exist.\n\r", ch);
	    return;
	  }
	  pReset->command = 'O';
	  pReset->arg2 = is_number (arg5) ? atol (arg5) : 0;
	  pReset->arg3 = ch->in_room->vnum;
	  pReset->arg4 = is_number (arg6) ? atol (arg6) : 1;

	}
	else
	  /*
	   * Into a Mobile's inventory.
	   * --------------------------
	   */
	{
	  if (flag_value (wear_loc_flags, arg4) == NO_FLAG)
	  {
	    send_to_char ("Resets: '? wear-loc'\n\r", ch);
	    return;
	  }

	  if (get_obj_index (atol (arg3)) == NULL)
	  {
	    send_to_char ("That vnum does not exit.\n\r", ch);
	    return;
	  }

	  pReset->arg1 = atol (arg3);
	  pReset->arg2 = is_number (arg5) ? atol (arg5) : 0;
	  pReset->arg3 = flag_value (wear_loc_flags, arg4);
	  pReset->arg4 = is_number (arg6) ? atol (arg6) : 0;

	  if (pReset->arg3 == WEAR_NONE)
	    pReset->command = 'G';
	  else
	    pReset->command = 'E';
	}
      }

      add_reset (ch->in_room, pReset, atol (arg1));
      SET_BIT (ch->in_room->area->area_flags, AREA_CHANGED);
      send_to_char ("Reset added.\n\r", ch);

    }
    else if (!str_cmp (arg2, "random") && is_number (arg3))
    {
      if (atol (arg3) < 1 || atol (arg3) > 6)
      {
	send_to_char ("Invalid argument.\n\r", ch);
	return;
      }
      pReset = new_reset_data ();
      pReset->command = 'R';
      pReset->arg1 = ch->in_room->vnum;
      pReset->arg2 = atol (arg3);
      add_reset (ch->in_room, pReset, atol (arg1));
      SET_BIT (ch->in_room->area->area_flags, AREA_CHANGED);
      send_to_char ("Random exits reset added.\n\r", ch);
    }
    else
    {
      send_to_char ("Syntax: RESET <number> OBJ <vnum> <wear_loc> chance\n\r",
		    ch);
      send_to_char
	("        RESET <number> OBJ <vnum> inside <vnum> [limit] [count] chance\n\r",
	 ch);
      send_to_char ("        RESET <number> OBJ <vnum> room chance\n\r", ch);
      send_to_char
	("        RESET <number> MOB <vnum> [max #x area] [max #x room]\n\r",
	 ch);
      send_to_char ("        RESET <number> DELETE\n\r", ch);
      send_to_char ("        RESET <number> RANDOM [#x exits]\n\r", ch);
    }
  }

  return;
}

_DOFUN (do_astat)
{
  char buf[MSL];
  char result[MSL * 2];

  ROOM_INDEX_DATA *pRoomIndex;
  MOB_INDEX_DATA *pMobIndex;
  OBJ_INDEX_DATA *pObjIndex;
  AREA_DATA *pArea;

  int rcount, mcount, ocount;
  int rtotal = 0, mtotal = 0, ototal = 0;
  int rdone = 0, mdone = 0, odone = 0;
  long vnum;

  if (IS_NPC (ch))
    return;

  sprintf (result, "[%-33s] [%-8s] [%-7s] [%-7s] [%-7s]\n\r",
	   "Area Name", "Last Saved", " Rooms ", "Objects", "  Mobs ");

  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    rcount = 0;
    mcount = 0;
    ocount = 0;
    rdone = 0;
    mdone = 0;
    odone = 0;

    for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
    {
      if ((pRoomIndex = get_room_index (vnum)))
      {
	rcount++;
	rtotal++;
	if (pRoomIndex->description[0] != '\0')
	  rdone++;
      }

      if ((pMobIndex = get_mob_index (vnum)) != NULL)
      {
	mcount++;
	mtotal++;
	if (pMobIndex->long_descr[0] != '\0')
	  mdone++;
      }

      if ((pObjIndex = get_obj_index (vnum)))
      {
	ocount++;
	ototal++;
	if (pObjIndex->description[0] != '\0')
	  odone++;
      }
    }

    sprintf (buf, "[%3ld] %-32s %-8s%s [%3d/%-3d] [%3d/%-3d] [%3d/%-3d]\n\r",
	     pArea->vnum,
	     pArea->name,
	     pArea->last_mod,
	     IS_SET (pArea->area_flags, AREA_CHANGED) ? "*" : " ",
	     rcount, rdone, ocount, odone, mcount, mdone);

    strcat (result, buf);
  }
  sprintf (buf, "-----------------------------------------------------------"
	   "--------------------\n\r"
	   "                                                    "
	   "%d     %d      %d\n\r", rtotal, ototal, mtotal);

  strcat (result, buf);
  send_to_char (result, ch);
  return;
}




/*****************************************************************************
 Name:		do_alist
 Purpose:	Normal command to list areas and display area information.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
_DOFUN (do_alist)
{
  char buf[MSL];
  char result[MSL * 2];		/* May need tweaking. */
  AREA_DATA *pArea;

  if (IS_NPC (ch))
    return;

  sprintf (result, "[%3s] [%-23s] (%-5s-%5s) [%-10s] %3s [%-8s] [%s]\n\r",
	   "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec",
	   "Builders", "Mod");

  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    sprintf (buf,
	     "[%3ld] %-25.25s (%-5ld-%5ld) %-12.12s [%d] [%-8.8s] [ %s ]\n\r",
	     pArea->vnum, pArea->name, pArea->min_vnum, pArea->max_vnum,
	     pArea->file_name, pArea->security, pArea->builders,
	     IS_SET (pArea->area_flags, AREA_CHANGED) ? "*" : " ");
    strcat (result, buf);
  }

  send_to_char (result, ch);
  return;
}
