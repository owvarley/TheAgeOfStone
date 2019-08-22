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
 * @@ Objective (const.c)
 *
 * This file is slowly being phased out.  Much of the little that remains
 * here should be moved to tables.c.  In any case, currently, the most
 * important aspect of this file is that it contains the skill_table.
 */


#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"




/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table catagorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] = {
	/*  {	structure		stat	}, */
	{area_flags, FALSE},
	{sex_flags, TRUE},
	{exit_flags, FALSE},
	{door_resets, TRUE},
	{room_flags, FALSE},
	{sector_flags, TRUE},
	{type_flags, TRUE},
	{extra_flags, FALSE},
	{wear_flags, FALSE},
	{_act_bits, FALSE},
	{_affected_by_bits, FALSE},
	{apply_flags, TRUE},
	{wear_loc_flags, TRUE},
	{wear_loc_strings, TRUE},
	{container_flags, FALSE},
	{_form_bits, FALSE},
	{_part_bits, FALSE},
	{ac_type, TRUE},
	{size_flags, TRUE},
	{position_flags, TRUE},
	{_off_bits, FALSE},
	{_resistance_bits, FALSE},
	{weapon_class, TRUE},
	{weapon_type2, FALSE},
	{apply_types, TRUE},
	{guild_flags, TRUE},
	{skill_flags, TRUE},
	{group_flags, TRUE},
	{mprog_flags, TRUE},
	{sound_type_table, TRUE},
	{system_sound_table, TRUE},
	{area_sound_table, TRUE},
	{mob_sound_table, TRUE},
	{room_sound_table, TRUE},
	{obj_sound_table, TRUE},
	{imm_sound_table, TRUE},
	{0, 0}
};



/* item type list */
const struct item_type item_table[] = {
	{ITEM_LIGHT, "light"},
	{ITEM_WEAPON, "weapon"},
	{ITEM_TREASURE, "treasure"},
	{ITEM_ARMOR, "armor"},
	{ITEM_POTION, "potion"},
	{ITEM_CLOTHING, "clothing"},
	{ITEM_HCLOAK, "hoodedcloak"},
	{ITEM_FURNITURE, "furniture"},
	{ITEM_TRASH, "trash"},
	{ITEM_CONTAINER, "container"},
	{ITEM_DRINK_CON, "drink"},
	{ITEM_KEY, "key"},
	{ITEM_FOOD, "food"},
	{ITEM_MONEY, "money"},
	{ITEM_BOAT, "boat"},
	{ITEM_CORPSE_NPC, "npc_corpse"},
	{ITEM_CORPSE_PC, "pc_corpse"},
	{ITEM_FOUNTAIN, "fountain"},
	{ITEM_HERB, "herb"},
	{ITEM_PROTECT, "protect"},
	{ITEM_MAP, "map"},
	{ITEM_PORTAL, "portal"},
	{ITEM_WAYGATE, "waygate"},
	{ITEM_ROOM_KEY, "room_key"},
	{ITEM_GEM, "gem"},
	{ITEM_JEWELRY, "jewelry"},
	{ITEM_JUKEBOX, "musicbox"},
	{ITEM_WAND, "wand"},
	{ITEM_STAFF, "staff"},
	{ITEM_QUIVER, "quiver"},
	{ITEM_ARROW, "arrow"},
	{ITEM_ANGREAL, "angreal"},
	{ITEM_CREW, "crewman"},
	{0, NULL}
};

/* weapon selection table */
const struct weapon_type weapon_table[] = {
	{"sword", OBJ_VNUM_SCHOOL_SWORD, WEAPON_SWORD},
	{"spear", OBJ_VNUM_SCHOOL_SPEAR, WEAPON_SPEAR},
	{"mace", OBJ_VNUM_SCHOOL_MACE, WEAPON_MACE},
	{"dagger", OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER},
	{"axe", OBJ_VNUM_SCHOOL_AXE, WEAPON_AXE},
	{"staff", OBJ_VNUM_SCHOOL_STAFF, WEAPON_STAFF},
	{"flail", OBJ_VNUM_SCHOOL_FLAIL, WEAPON_FLAIL},
	{"whip", OBJ_VNUM_SCHOOL_WHIP, WEAPON_WHIP},
	{"polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM},
	{"bow", OBJ_VNUM_SCHOOL_BOW, WEAPON_BOW},
	{NULL, 0, 0}
};



/* wiznet table and prototype for future flag setting */
const struct wiznet_type wiznet_table[] = {
	{"on", WIZ_ON, IM},
	{"prefix", WIZ_PREFIX, IM},
	{"ticks", WIZ_TICKS, IM},
	{"logins", WIZ_LOGINS, IM},
	{"sites", WIZ_SITES, L6},
	{"links", WIZ_LINKS, L7},
	{"newbies", WIZ_NEWBIE, IM},
	{"spam", WIZ_SPAM, L5},
	{"deaths", WIZ_DEATHS, IM},
	{"resets", WIZ_RESETS, L6},
	{"mobdeaths", WIZ_MOBDEATHS, L6},
	{"flags", WIZ_FLAGS, L5},
	{"penalties", WIZ_PENALTIES, L5},
	{"saccing", WIZ_SACCING, L5},
	{"levels", WIZ_LEVELS, IM},
	{"load", WIZ_LOAD, L2},
	{"restore", WIZ_RESTORE, L2},
	{"snoops", WIZ_SNOOPS, L2},
	{"switches", WIZ_SWITCHES, L2},
	{"secure", WIZ_SECURE, L1},
	{"bugs", WIZ_BUGS, L4},
	{"trace", WIZ_TRACE, L6},
	{"restring", WIZ_RESTRING, L4},
	{NULL, 0, 0}
};

/* attack table  -- not very organized :( */
const struct attack_type attack_table[MAX_DAMAGE_MESSAGE] = {
	{"none", "hit", -1},
	{"slice", "slice", DAM_SLASH},
	{"stab", "stab", DAM_PIERCE},
	{"slash", "slash", DAM_SLASH},
	{"whip", "whip", DAM_SLASH},
	{"claw", "claw", DAM_SLASH},
	{"blast", "blast", DAM_BASH},
	{"pound", "pound", DAM_BASH},
	{"crush", "crush", DAM_BASH},
	{"grep", "grep", DAM_SLASH},
	{"bite", "bite", DAM_PIERCE},
	{"pierce", "pierce", DAM_PIERCE},
	{"suction", "suction", DAM_BASH},
	{"beating", "beating", DAM_BASH},
	{"digestion", "digestion", DAM_ACID},
	{"charge", "charge", DAM_BASH},
	{"slap", "slap", DAM_BASH},
	{"punch", "punch", DAM_BASH},
	{"wrath", "wrath", DAM_ENERGY},
	{"weave", "weave", DAM_ENERGY},
	{"divine", "weave", DAM_HOLY},
	{"cleave", "cleave", DAM_SLASH},
	{"scratch", "scratch", DAM_PIERCE},
	{"peck", "peck", DAM_PIERCE},
	{"peckb", "peck", DAM_BASH},
	{"chop", "chop", DAM_SLASH},
	{"sting", "sting", DAM_PIERCE},
	{"smash", "smash", DAM_BASH},
	{"shbite", "shocking bite", DAM_LIGHTNING},
	{"flbite", "flaming bite", DAM_FIRE},
	{"frbite", "freezing bite", DAM_COLD},
	{"acbite", "acidic bite", DAM_ACID},
	{"chomp", "chomp", DAM_PIERCE},
	{"drain", "life drain", DAM_NEGATIVE},
	{"thrust", "thrust", DAM_PIERCE},
	{"slime", "slime", DAM_ACID},
	{"shock", "shock", DAM_LIGHTNING},
	{"thwack", "thwack", DAM_BASH},
	{"flame", "flame", DAM_FIRE},
	{"chill", "chill", DAM_COLD},
	{"thrash", "thrash", DAM_BASH},
	{NULL, NULL, 0}
};


/*
 * Attribute bonus tables.
 */
const struct str_app_type str_app[30] = {
	{-5, -4, 0, 0},
	{-5, -4, 3, 1},
	{-3, -2, 3, 2},
	{-3, -1, 10, 3},
	{-2, -1, 25, 4},
	{-2, -1, 55, 5},
	{-1, 0, 80, 6},
	{-1, 0, 90, 7},
	{0, 0, 100, 8},
	{0, 0, 100, 9},
	{0, 0, 115, 10},
	{0, 0, 115, 11},
	{0, 0, 130, 12},
	{0, 0, 130, 13},
	{0, 1, 140, 14},
	{1, 1, 150, 15},
	{1, 2, 165, 16},
	{2, 3, 180, 22},
	{2, 3, 200, 25},
	{3, 4, 225, 30},
	{3, 5, 250, 35},
	{4, 6, 300, 40},
	{4, 6, 350, 45},
	{5, 7, 400, 50},
	{5, 8, 450, 55},
	{6, 9, 500, 60}, /* 25 */
	{7, 10, 600, 65},
	{8, 11, 700, 70},
	{9, 12, 800, 75},
	{10, 13, 900, 80}  /* 29 */
};



const struct int_app_type int_app[26] = {
	{3},
	{5},
	{7},
	{8},
	{9},
	{10},
	{11},
	{12},
	{13},
	{15},
	{17},
	{19},
	{22},
	{25},
	{28},
	{31},
	{34},
	{37},
	{40},
	{44},
	{49},
	{55},
	{60},
	{70},
	{80},
	{85}
};



const struct wis_app_type wis_app[26] = {
	{0}, /*  0 */
	{0}, /*  1 */
	{0},
	{0}, /*  3 */
	{0},
	{1}, /*  5 */
	{1},
	{1},
	{1},
	{1},
	{1}, /* 10 */
	{1},
	{1},
	{1},
	{1},
	{2}, /* 15 */
	{2},
	{2},
	{3}, /* 18 */
	{3},
	{3}, /* 20 */
	{3},
	{4},
	{4},
	{4},
	{5}	 /* 25 */
};



const struct dex_app_type dex_app[26] = {
	{60},  /* 0 */
	{50},  /* 1 */
	{50},
	{40},
	{30},
	{20},  /* 5 */
	{10},
	{0},
	{0},
	{0},
	{0},   /* 10 */
	{0},
	{0},
	{0},
	{0},
	{-10}, /* 15 */
	{-15},
	{-20},
	{-30},
	{-40},
	{-50}, /* 20 */
	{-60},
	{-75},
	{-90},
	{-105},
	{-120} /* 25 */
};


const struct con_app_type con_app[26] = {
	{-4, 20},			/*  0 */
	{-3, 25},			/*  1 */
	{-2, 30},
	{-2, 35},			/*  3 */
	{-1, 40},
	{-1, 45},			/*  5 */
	{-1, 50},
	{0, 55},
	{0, 60},
	{0, 65},
	{0, 70},			/* 10 */
	{0, 75},
	{0, 80},
	{0, 85},
	{0, 88},
	{1, 90},			/* 15 */
	{2, 95},
	{2, 97},
	{3, 99},			/* 18 */
	{3, 99},
	{4, 99},			/* 20 */
	{4, 99},
	{5, 99},
	{6, 99},
	{10, 99},
	{14, 99}			/* 25 */
};



/*
 * The Talent table
 */
struct talent_type talent_table[MAX_TALENTS] = {
	/* Talents */
	{
		"healing",
		{1, 1, 1},
		{10, 10, 10},
		{_IS_TALENT, 0},
		{0, 0, 0, 0, 1},
		{0, 0, 0, 0, 1},
		&gtn_healing
	},

	{
		"travelling",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		&gtn_travelling
	},

	{
		"tree singing",
		{53, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		&gtn_tree_singing
	},

	{
		"dreaming",
		{1, 1, 1},
		{10, 12, 12},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		&gtn_dreaming
	},

	/* Specialties */
	{
		"unarmed combat",
		{-1, 1, 1},
		{0, 10, 10},
		{_IS_SPECIALTY | _COMBAT_SPECIALTY, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0},
		&gtn_unarmed_combat
	},

	{
		"knives",
		{-1, 1, -1},
		{0, 10, 0},
		{_IS_SPECIALTY | _COMBAT_SPECIALTY, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0},
		&gtn_knives
	},

	{
		"physical defense",
		{-1, -1, 1},
		{0, 0, 10},
		{_IS_SPECIALTY | _COMBAT_SPECIALTY, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0},
		&gtn_physical_defense
	},

	{
		"defensive weaves",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0},
		&gtn_defensive_weaves
	},

	{
		"weaving air",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0},
		&gtn_air
	},

	{
		"weaving earth",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0},
		&gtn_earth
	},

	{
		"weaving water",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0},
		&gtn_water
	},

	{
		"weaving fire",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0},
		&gtn_fire
	},

	{
		"weaving spirit",
		{1, 1, 1},
		{10, 10, 10},
		{0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1},
		&gtn_spirit
	},
		
	{
		"acrobatics",
		{-1, 1, -1},
		{0, 10, 0},
		{_IS_TALENT, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0},
		&gtn_acrobatics
	},

	{
		"blacksmithing",
		{-1, -1, 1},
		{0, 0, 10},
		{_IS_TALENT, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0},
		&gtn_blacksmithing
	}
};

