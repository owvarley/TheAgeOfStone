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

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"


/* for position */
const struct position_type position_table[] = {
  {"dead", "dead"},
  {"mortally wounded", "mort"},
  {"incapacitated", "incap"},
  {"stunned", "stun"},
  {"sleeping", "sleep"},
  {"resting", "rest"},
  {"sitting", "sit"},
  {"fighting", "fight"},
  {"standing", "stand"},
  {NULL, NULL}
};

/* for sex */
const struct sex_type sex_table[] = {
  {"none"},
  {"male"},
  {"female"},
  {"either"},
  {NULL}
};

/* for sizes */
const struct size_type size_table[] = {
  {"tiny"},
  {"small"},
  {"medium"},
  {"large"},
  {"huge",},
  {"giant"},
  {NULL}
};

/* various flag tables */

/* sound class flags */
const struct flag_type sound_type_table[] = {
  {"system", SOUND_SYSTEM, TRUE},
  {"area", SOUND_AREA, TRUE},
  {"mobile", SOUND_MOBILE, TRUE},
  {"object", SOUND_OBJECT, TRUE},
  {"room", SOUND_ROOM, TRUE},
  {"immortal", SOUND_IMMORTAL, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type system_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"logon", SOUND_SYSTEM_LOGON, TRUE},
  {"quit", SOUND_SYSTEM_QUIT, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type area_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"enter", SOUND_AREA_ENTER, TRUE},
  {"leave", SOUND_AREA_LEAVE, TRUE},
  {"wake", SOUND_AREA_WAKE, TRUE},
  {"sleep", SOUND_AREA_SLEEP, TRUE},
  {"death", SOUND_AREA_DEATH, TRUE},
  {"combat", SOUND_AREA_COMBAT, TRUE},
  {"nighttime", SOUND_AREA_SUN_DARK, TRUE},
  {"sunrise", SOUND_AREA_SUN_RISE, TRUE},
  {"daytime", SOUND_AREA_SUN_LIGHT, TRUE},
  {"sunset", SOUND_AREA_SUN_SET, TRUE},
  {"clear", SOUND_AREA_CLOUDLESS, TRUE},
  {"cloudy", SOUND_AREA_CLOUDY, TRUE},
  {"raining", SOUND_AREA_RAINING, TRUE},
  {"storm", SOUND_AREA_LIGHTNING, TRUE},
  {NULL, 0, FALSE}

};

const struct flag_type mob_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"deathcry", SOUND_MOB_DEATHCRY, TRUE},
  {"fighting", SOUND_MOB_FIGHTING, TRUE},
  {"level", SOUND_MOB_LEVEL, TRUE},
  {NULL, 0, FALSE}

};

const struct flag_type imm_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"advance", SOUND_IMM_ADVANCE, TRUE},
  {"poofin", SOUND_IMM_POOFIN, TRUE},
  {"poofout", SOUND_IMM_POOFOUT, TRUE},
  {"slay", SOUND_IMM_SLAY, TRUE},
  {"purge", SOUND_IMM_PURGE, TRUE},
  {"restore", SOUND_IMM_RESTORE, TRUE},
  {NULL, 0, FALSE}

};

const struct flag_type obj_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"dropped", SOUND_OBJ_DROP, TRUE},
  {"taken", SOUND_OBJ_GET, TRUE},
  {"strikes", SOUND_OBJ_STRIKE, TRUE},
  {"defends", SOUND_OBJ_DEFEND, TRUE},
  {"enchanted", SOUND_OBJ_ENCHANT, TRUE},
  {"disenchanted", SOUND_OBJ_DISENCHANT, TRUE},
  {"give", SOUND_OBJ_GIVE, TRUE},
  {"cond90", SOUND_OBJ_DAMAGE_90, TRUE},
  {"cond80", SOUND_OBJ_DAMAGE_80, TRUE},
  {"cond70", SOUND_OBJ_DAMAGE_70, TRUE},
  {"cond60", SOUND_OBJ_DAMAGE_60, TRUE},
  {"cond50", SOUND_OBJ_DAMAGE_50, TRUE},
  {"cond40", SOUND_OBJ_DAMAGE_40, TRUE},
  {"cond30", SOUND_OBJ_DAMAGE_30, TRUE},
  {"cond20", SOUND_OBJ_DAMAGE_20, TRUE},
  {"cond10", SOUND_OBJ_DAMAGE_10, TRUE},
  {"cond00", SOUND_OBJ_DAMAGE_00, TRUE},
  {NULL, 0, FALSE}

};


const struct flag_type room_sound_table[] = {
  {"none", SOUND_NONE, FALSE},
  {"enter_any", SOUND_ROOM_ENTER_ANY, TRUE},
  {"enter_north", SOUND_ROOM_ENTER_NORTH, TRUE},
  {"enter_east", SOUND_ROOM_ENTER_EAST, TRUE},
  {"enter_south", SOUND_ROOM_ENTER_SOUTH, TRUE},
  {"enter_west", SOUND_ROOM_ENTER_WEST, TRUE},
  {"enter_up", SOUND_ROOM_ENTER_UP, TRUE},
  {"enter_down", SOUND_ROOM_ENTER_DOWN, TRUE},
  {"exit_any", SOUND_ROOM_EXIT_ANY, TRUE},
  {"exit_north", SOUND_ROOM_EXIT_NORTH, TRUE},
  {"exit_east", SOUND_ROOM_EXIT_EAST, TRUE},
  {"exit_south", SOUND_ROOM_EXIT_SOUTH, TRUE},
  {"exit_west", SOUND_ROOM_EXIT_WEST, TRUE},
  {"exit_up", SOUND_ROOM_EXIT_UP, TRUE},
  {"exit_down", SOUND_ROOM_EXIT_DOWN, TRUE},
  {"wake", SOUND_ROOM_WAKE, TRUE},
  {"sleep", SOUND_ROOM_SLEEP, TRUE},
  {"reset", SOUND_ROOM_RESET, TRUE},
  {"death", SOUND_ROOM_DEATH, TRUE},
  {"combat", SOUND_ROOM_COMBAT, TRUE},
  {"dark", SOUND_ROOM_DARK, TRUE},
  {"light", SOUND_ROOM_LIGHT, TRUE},
  {NULL, 0, FALSE}

};


/* mud state flags */
const struct flag_type mud_flags[] = {
  {"Normal", MUD_DEFAULT, TRUE},
  {"NewLocked", MUD_NEWLOCK, TRUE},
  {"ImmLocked", MUD_IMMLOCK, TRUE},
  {"DebugMode", MUD_DEBUG, TRUE},
  {"Redirect", MUD_REDIRECT, TRUE},
  {"ResolveOn", MUD_USE_RESOLVE, TRUE},
  {"Closed", MUD_CLOSED, TRUE},
  {"Copyover", MUD_COPYOVER, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type guild_flags[] = {
  {"Independent", _INDEPENDENT, TRUE},
  {"Gaidin", _GAIDIN, TRUE},
  {"Wolfkin", _WOLF, TRUE},
  {"Modified", _CHANGED, FALSE},
  {"Delete", _DELETED, TRUE},
  {"Immortal", _IMMORTAL, TRUE},
  {"Aes Sedai", _SEDAI, TRUE},
  {"Aiel", _AIEL, TRUE},
  {"Whitecloak", _WHITECLOAK, TRUE},
  {"Thief Taker", _THIEF_TAKER, TRUE},
  {"Hailene", _HAILENE, TRUE},
  {"Wansho", _WANSHO, TRUE},
  {"Tinker", _TINKER, TRUE},
  {"Shadar", _SHADAR, TRUE},
  {"Companion", _COMPANIONS, TRUE},
  {"Illuminator", _ILLUMINATOR, TRUE},
  {"Gleeman", _GLEEMAN, TRUE},
  {"Asha'Man", _ASHAMAN, TRUE},
  {"Red Hand", _REDHAND, TRUE},
  {"Hunter", _HUNTER, TRUE},
  {"Blight Watch", _BLIGHT_WATCH, TRUE},
  {"Queen's Guard", _QUEENS_GUARD, TRUE},
  {"Younglings", _YOUNGLINGS, TRUE},
  {"Kin", _KIN, TRUE},
  {"Salidar", _SALIDAR, TRUE},
  {"Two Rivers Companions", _2RIVERS_COMPANIONS, TRUE},
  {"Cha Faile", _CHA_FAILE, TRUE},
  {"Atha'an Miere", _SEAFOLK, TRUE},
  {"Shadow", _SHADOW, TRUE},
  {"Towerguard", _TOWERGUARD, TRUE},
  {"Dragonsworn", _DRAGONSWORN, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type skill_flags[] = {
  {"NoCharge", _NO_CHARGE, TRUE},
  {"Talent", _IS_TALENT, TRUE},
  {"Specialty", _IS_SPECIALTY, TRUE},
  {"Aff_End", _AFF_END, TRUE},
  {"Req_Channel", _REQ_CHANNEL, TRUE},
  {"Req_Spec_Air", _REQ_SPEC_AIR, TRUE},
  {"Req_Spec_Earth", _REQ_SPEC_EARTH, TRUE},
  {"Req_Spec_Water", _REQ_SPEC_WATER, TRUE},
  {"Req_Spec_Fire", _REQ_SPEC_FIRE, TRUE},
  {"Req_Spec_Spirit", _REQ_SPEC_SPIRIT, TRUE},
  {"Req_Talent_Dreaming", _REQ_TALENT_DREAMING, TRUE},
  {"Req_Talent_Travelling", _REQ_TALENT_TRAVELLING, TRUE},
  {"Req_Talent_Healing", _REQ_TALENT_HEALING, TRUE},
  {"Req_Talent_Singing", _REQ_TALENT_SINGING, TRUE},
  {"Req_Race_Human", _REQ_RACE_HUMAN, TRUE},
  {"Req_Race_Ogier", _REQ_RACE_OGIER, TRUE},
  {"Req_Race_Trolloc", _REQ_RACE_TROLLOC, TRUE},
  {"Req_Race_Fade", _REQ_RACE_FADE, TRUE},
  {"Req_Race_Draghkar", _REQ_RACE_DRAGHKAR, TRUE},
  {"Combat_Specialty", _COMBAT_SPECIALTY, TRUE},
  {"Weave_Speciality", _WEAVE_SPECIALTY, TRUE},
  {"Group_Only", _GROUP_ONLY, TRUE},
  {"Changed", _CHANGED, FALSE},
  {"NotGainable",	_NOT_GAINABLE, TRUE},
  {"Deleted", _DELETED, FALSE},
  {"Req_Rogue", _REQ_ROGUE, TRUE},
  {"Req_Warrior", _REQ_WARRIOR, TRUE},
  {"Req_Channeler", _REQ_CHANNELER, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type group_flags[] = {
  {"Changed", _CHANGED, FALSE},
  {"Deleted", _DELETED, FALSE},
  {"NoCharge", _NO_CHARGE, TRUE},
  {"Req_Channel", _REQ_CHANNEL, TRUE},
  {"Req_Spec_Air", _REQ_SPEC_AIR, TRUE},
  {"Req_Spec_Earth", _REQ_SPEC_EARTH, TRUE},
  {"Req_Spec_Water", _REQ_SPEC_WATER, TRUE},
  {"Req_Spec_Fire", _REQ_SPEC_FIRE, TRUE},
  {"Req_Spec_Spirit", _REQ_SPEC_SPIRIT, TRUE},
  {"Req_Talent_Dreaming", _REQ_TALENT_DREAMING, TRUE},
  {"Req_Talent_Travelling", _REQ_TALENT_TRAVELLING, TRUE},
  {"Req_Talent_Healing", _REQ_TALENT_HEALING, TRUE},
  {"Req_Talent_Singing", _REQ_TALENT_SINGING, TRUE},
  {"Req_Race_Human", _REQ_RACE_HUMAN, TRUE},
  {"Req_Race_Ogier", _REQ_RACE_OGIER, TRUE},
  {"Req_Race_Trolloc", _REQ_RACE_TROLLOC, TRUE},
  {"Req_Race_Fade", _REQ_RACE_FADE, TRUE},
  {"Req_Race_Draghkar", _REQ_RACE_DRAGHKAR, TRUE},
  {"NotGainable",	_NOT_GAINABLE, TRUE},
  {NULL, 0, FALSE}
};


const struct flag_type mprog_flags[] = {
  {"act", TRIG_ACT, TRUE},
  {"bribe", TRIG_BRIBE, TRUE},
  {"death", TRIG_DEATH, TRUE},
  {"entry", TRIG_ENTRY, TRUE},
  {"fight", TRIG_FIGHT, TRUE},
  {"give", TRIG_GIVE, TRUE},
  {"greet", TRIG_GREET, TRUE},
  {"grall", TRIG_GRALL, TRUE},
  {"kill", TRIG_KILL, TRUE},
  {"hpcnt", TRIG_HPCNT, TRUE},
  {"random", TRIG_RANDOM, TRUE},
  {"speech", TRIG_SPEECH, TRUE},
  {"exit", TRIG_EXIT, TRUE},
  {"exall", TRIG_EXALL, TRUE},
  {"delay", TRIG_DELAY, TRUE},
  {"surr", TRIG_SURR, TRUE},
  {NULL, 0, TRUE}
};

const struct flag_type area_flags[] = {
  {"none", AREA_NONE, FALSE},
  {"unused", AREA_UNUSED, TRUE},
  {"changed", AREA_CHANGED, TRUE},
  {"added", AREA_ADDED, TRUE},
  {"loading", AREA_LOADING, FALSE},
  {"closed", AREA_CLOSED, TRUE},
  {NULL, 0, 0}
};



const struct flag_type sex_flags[] = {
  {"male", SEX_MALE, TRUE},
  {"female", SEX_FEMALE, TRUE},
  {"neutral", SEX_NEUTRAL, TRUE},
  {"random", 3, TRUE},		/* ROM */
  {"none", SEX_NEUTRAL, TRUE},
  {NULL, 0, 0}
};



const struct flag_type exit_flags[] = {
  {"door", EX_ISDOOR, TRUE},
  {"closed", EX_CLOSED, TRUE},
  {"locked", EX_LOCKED, TRUE},
  {"secret", EX_SECRET, TRUE},
  {"pickproof", EX_PICKPROOF, TRUE},
  {"nopass", EX_NOPASS, TRUE},
  {"easy", EX_EASY, TRUE},
  {"hard", EX_HARD, TRUE},
  {"infuriating", EX_INFURIATING, TRUE},
  {"noclose", EX_NOCLOSE, TRUE},
  {"nolock", EX_NOLOCK, TRUE},
  {"random", EX_RANDOM, TRUE},
  {NULL, 0, 0}
};



const struct flag_type door_resets[] = {
  {"open", DOOR_OPEN, TRUE},
  {"closed", DOOR_CLOSED, TRUE},
  {"locked", DOOR_LOCKED, TRUE},
  {"secret", DOOR_SECRET, TRUE},
  {NULL, 0, FALSE}
};


const struct flag_type room_flags[] = {
  {"dark", ROOM_DARK, TRUE},
  {"no_mob", ROOM_NO_MOB, TRUE},
  {"indoors", ROOM_INDOORS, TRUE},
  {"private", ROOM_PRIVATE, TRUE},
  {"safe", ROOM_SAFE, TRUE},
  {"solitary", ROOM_SOLITARY, TRUE},
  {"pet_shop", ROOM_PET_SHOP, TRUE},
  {"mount_shop", ROOM_MOUNT_SHOP, TRUE},
  {"no_recall", ROOM_NO_RECALL, TRUE},
  {"imp_only", ROOM_IMP_ONLY, TRUE},
  {"gods_only", ROOM_GODS_ONLY, TRUE},
  {"heroes_only", ROOM_HEROES_ONLY, TRUE},
  {"newbies_only", ROOM_NEWBIES_ONLY, TRUE},
  {"law", ROOM_LAW, TRUE},
  {"nowhere", ROOM_NOWHERE, TRUE},
  {"for_rent", ROOM_RENTAL, TRUE},
  {"no_onepower", ROOM_NOMAGIC, TRUE},
  {"stedding", ROOM_STEDDING, TRUE},
  {"men_only", ROOM_MEN_ONLY, TRUE},
  {"women_only", ROOM_WOMEN_ONLY, TRUE},
  {"no_gate", ROOM_NO_GATE, TRUE},
  {"depository", ROOM_DEPOSITORY, TRUE},
  {"stable", ROOM_STABLE, TRUE},
  {"nosneak", ROOM_NOSNEAK, TRUE},
  {NULL, 0, 0}
};



const struct flag_type sector_flags[] = {
  {"unused", SECT_UNUSED, TRUE},
  {"inside", SECT_INSIDE, TRUE},
  {"city", SECT_CITY, TRUE},
  {"jungle", SECT_JUNGLE, TRUE},
  {"forest", SECT_FOREST, TRUE},
  {"hills", SECT_HILLS, TRUE},
  {"mountain", SECT_MOUNTAIN, TRUE},
  {"water", SECT_WATER, TRUE},
  {"swamp", SECT_SWAMP, TRUE},
  {"vantage", SECT_VANTAGE, TRUE},
  {"desert", SECT_DESERT, TRUE},
  {NULL, 0, 0}
};

const struct flag_type type_flags[] = {
  {"trash", ITEM_NONE, FALSE},
  {"light", ITEM_LIGHT, TRUE},
  {"weapon", ITEM_WEAPON, TRUE},
  {"treasure", ITEM_TREASURE, TRUE},
  {"armor", ITEM_ARMOR, TRUE},
  {"potion", ITEM_POTION, TRUE},
  {"furniture", ITEM_FURNITURE, TRUE},
  {"clothing", ITEM_CLOTHING, TRUE},
  {"hoodedcloak", ITEM_HCLOAK, TRUE},
  {"trash", ITEM_TRASH, TRUE},
  {"container", ITEM_CONTAINER, TRUE},
  {"drinkcontainer", ITEM_DRINK_CON, TRUE},
  {"key", ITEM_KEY, TRUE},
  {"food", ITEM_FOOD, TRUE},
  {"money", ITEM_MONEY, TRUE},
  {"boat", ITEM_BOAT, TRUE},
  {"npccorpse", ITEM_CORPSE_NPC, FALSE},
  {"pc corpse", ITEM_CORPSE_PC, FALSE},
  {"fountain", ITEM_FOUNTAIN, TRUE},
  {"herb", ITEM_HERB, TRUE},
  {"protect", ITEM_PROTECT, TRUE},
  {"map", ITEM_MAP, TRUE},
  {"portal", ITEM_PORTAL, TRUE},
  {"waygate", ITEM_WAYGATE, TRUE},
  {"roomkey", ITEM_ROOM_KEY, TRUE},
  {"gem", ITEM_GEM, TRUE},
  {"jewelry", ITEM_JEWELRY, TRUE},
  {"musicbox", ITEM_JUKEBOX, TRUE},
  {"wand", ITEM_WAND, FALSE},
  {"staff", ITEM_STAFF, FALSE},
  {"waygate marker", ITEM_GATEMARKER, TRUE},
  {"quiver", ITEM_QUIVER, TRUE},
  {"arrow", ITEM_ARROW, TRUE},
  {"angreal", ITEM_ANGREAL, TRUE},
  {"crewman", ITEM_CREW, TRUE},
  {NULL, 0, 0}
};


const struct flag_type extra_flags[] = {
  {"glow", ITEM_GLOW, TRUE},
  {"hum", ITEM_HUM, TRUE},
  {"ghost", ITEM_GHOST, TRUE},
  {"lock", ITEM_LOCK, TRUE},
  {"evil", ITEM_EVIL, TRUE},
  {"invis", ITEM_INVIS, TRUE},
  {"weave", ITEM_MAGIC, TRUE},
  {"nodrop", ITEM_NODROP, TRUE},
  {"bless", ITEM_BLESS, TRUE},
  {"antigood", ITEM_ANTI_GOOD, TRUE},
  {"antievil", ITEM_ANTI_EVIL, TRUE},
  {"antineutral", ITEM_ANTI_NEUTRAL, TRUE},
  {"noremove", ITEM_NOREMOVE, TRUE},
  {"inventory", ITEM_INVENTORY, TRUE},
  {"nopurge", ITEM_NOPURGE, TRUE},
  {"rotdeath", ITEM_ROT_DEATH, TRUE},
  {"visdeath", ITEM_VIS_DEATH, TRUE},
  {"nonmetal", ITEM_NONMETAL, TRUE},
  {"meltdrop", ITEM_MELT_DROP, TRUE},
  {"hadtimer", ITEM_HAD_TIMER, TRUE},
  {"treesung", ITEM_TREESUNG, TRUE},
  {"burnproof", ITEM_BURN_PROOF, TRUE},
  {"guild_item", ITEM_GUILD, TRUE},
  {"quest_item", ITEM_QUEST, TRUE},
  {"neverbreak", ITEM_NOBREAK, TRUE},
  {"hidden", ITEM_HIDDEN, TRUE},
  {"nocount", ITEM_NOCOUNT, TRUE},
  {"nosave", ITEM_NOSAVE, TRUE},
  {"lodged", ITEM_LODGED, TRUE},
  {NULL, 0, 0}
};



const struct flag_type wear_flags[] = {
  {"take", ITEM_TAKE, TRUE},
  {"finger", ITEM_WEAR_FINGER, TRUE},
  {"neck", ITEM_WEAR_NECK, TRUE},
  {"body", ITEM_WEAR_BODY, TRUE},
  {"head", ITEM_WEAR_HEAD, TRUE},
  {"legs", ITEM_WEAR_LEGS, TRUE},
  {"feet", ITEM_WEAR_FEET, TRUE},
  {"hands", ITEM_WEAR_HANDS, TRUE},
  {"arms", ITEM_WEAR_ARMS, TRUE},
  {"shield", ITEM_WEAR_SHIELD, TRUE},
  {"about", ITEM_WEAR_ABOUT, TRUE},
  {"waist", ITEM_WEAR_WAIST, TRUE},
  {"wrist", ITEM_WEAR_WRIST, TRUE},
  {"wield", ITEM_WIELD, TRUE},
  {"hold", ITEM_HOLD, TRUE},
  {"nosac", ITEM_NO_SAC, TRUE},
  {"wearfloat", ITEM_WEAR_FLOAT, TRUE},
  {"face", ITEM_WEAR_FACE, TRUE},
  {"scabbard", ITEM_WEAR_SCABBARD, TRUE},
  {"tattoo", ITEM_WEAR_TATTOO, TRUE},
  {"earring", ITEM_WEAR_EARRING, TRUE},
  {"shoulder", ITEM_WEAR_SHOULDER, TRUE},
  {"lodged", ITEM_WEAR_LODGED, FALSE},
  {NULL, 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] = {
  {"none", APPLY_NONE, TRUE},
  {"strength", APPLY_STR, TRUE},
  {"dexterity", APPLY_DEX, TRUE},
  {"intelligence", APPLY_INT, TRUE},
  {"wisdom", APPLY_WIS, TRUE},
  {"constitution", APPLY_CON, TRUE},
  {"sex", APPLY_SEX, TRUE},
  {"class", APPLY_CLASS, TRUE},
  {"level", APPLY_LEVEL, TRUE},
  {"age", APPLY_AGE, TRUE},
  {"height", APPLY_HEIGHT, TRUE},
  {"weight", APPLY_WEIGHT, TRUE},
  {"hp", APPLY_HIT, TRUE},
  {"move", APPLY_ENDURANCE, TRUE},
  {"gold", APPLY_GOLD, TRUE},
  {"experience", APPLY_EXP, TRUE},
  {"ac", APPLY_AC, TRUE},
  {"hitroll", APPLY_HITROLL, TRUE},
  {"damroll", APPLY_DAMROLL, TRUE},
  {"saves", APPLY_SAVES, TRUE},
#if defined(AllowSaveTypes)
  {"savingpara", APPLY_SAVING_PARA, TRUE},
  {"savingrod", APPLY_SAVING_ROD, TRUE},
  {"savingpetri", APPLY_SAVING_PETRI, TRUE},
  {"savingbreath", APPLY_SAVING_BREATH, TRUE},
#endif
  {"savingweave", APPLY_SAVING_SPELL, TRUE},
  {"weaveaffect", APPLY_SPELL_AFFECT, FALSE},
  {"flow_air", APPLY_AIR, TRUE},
  {"flow_earth", APPLY_EARTH, TRUE},
  {"flow_water", APPLY_WATER, TRUE},
  {"flow_fire", APPLY_FIRE, TRUE},
  {"flow_spirit", APPLY_SPIRIT, TRUE},
  {NULL, 0, 0}
};



/*
 * What is seen.
 * Note: the "as/around/on" messages here are used when displaying
 * eq/objs to characters.  Also, the order that the locations are 
 * listed here, is the order that will be shown to characters when
 * the do an "eq" or "look (who)" command.  --Gary
 */
const struct flag_type wear_loc_strings[] = {
  {"<as a light>           ", WEAR_LIGHT, TRUE},
  {"<floating nearby>      ", WEAR_FLOAT, TRUE},
  {"<over the head>        ", WEAR_HEAD, TRUE},
  {"<around the face>      ", WEAR_FACE, TRUE},
  {"<in the left ear>      ", WEAR_EARRING_L, TRUE},
  {"<in the right ear>     ", WEAR_EARRING_R, TRUE},
  {"<around the neck>      ", WEAR_NECK_1, TRUE},
  {"<around the neck>      ", WEAR_NECK_2, TRUE},
  {"<about the shoulders>  ", WEAR_ABOUT, TRUE},
  {"<slung across the back>", WEAR_SHOULDER, TRUE},
  {"<around the body>      ", WEAR_BODY, TRUE},
  {"<around the waist>     ", WEAR_WAIST, TRUE},
  {"<on the arms>          ", WEAR_ARMS, TRUE},
  {"<tattooed on the arm>  ", WEAR_TATTOO, TRUE},
  {"<as a shield>          ", WEAR_SHIELD, TRUE},
  {"<on the hands>         ", WEAR_HANDS, TRUE},
  {"<on the left wrist>    ", WEAR_WRIST_L, TRUE},
  {"<on the right wrist>   ", WEAR_WRIST_R, TRUE},
  {"<on the left finger>   ", WEAR_FINGER_L, TRUE},
  {"<on the right finger>  ", WEAR_FINGER_R, TRUE},
  {"<around the waist>     ", WEAR_SCABBARD, TRUE},
  {"<primary weapon>       ", WEAR_WIELD, TRUE},
  {"<secondary weapon>     ", WEAR_SECONDARY, FALSE},
  {"<on the legs>          ", WEAR_LEGS, TRUE},
  {"<on the feet>          ", WEAR_FEET, TRUE},
  {"<held in the hands>    ", WEAR_HOLD, TRUE},
  {"<in the inventory>     ", WEAR_NONE, TRUE},
  {"(lodged in a leg)      ", WEAR_LODGED_L, FALSE},
  {"(lodged in an arm)     ", WEAR_LODGED_A, FALSE},
  {"(lodged in a rib)      ", WEAR_LODGED_R, FALSE},
  {NULL, 0, FALSE}
};


const struct flag_type wear_loc_flags[] = {
  {"none", WEAR_NONE, TRUE},
  {"light", WEAR_LIGHT, TRUE},
  {"lfinger", WEAR_FINGER_L, TRUE},
  {"rfinger", WEAR_FINGER_R, TRUE},
  {"neck1", WEAR_NECK_1, TRUE},
  {"neck2", WEAR_NECK_2, TRUE},
  {"body", WEAR_BODY, TRUE},
  {"head", WEAR_HEAD, TRUE},
  {"legs", WEAR_LEGS, TRUE},
  {"feet", WEAR_FEET, TRUE},
  {"hands", WEAR_HANDS, TRUE},
  {"arms", WEAR_ARMS, TRUE},
  {"shield", WEAR_SHIELD, TRUE},
  {"about", WEAR_ABOUT, TRUE},
  {"waist", WEAR_WAIST, TRUE},
  {"lwrist", WEAR_WRIST_L, TRUE},
  {"rwrist", WEAR_WRIST_R, TRUE},
  {"wielded", WEAR_WIELD, TRUE},
  {"hold", WEAR_HOLD, TRUE},
  {"floating", WEAR_FLOAT, TRUE},
  {"right ear", WEAR_EARRING_R, TRUE},
  {"left ear", WEAR_EARRING_L, TRUE},
  {"face", WEAR_FACE, TRUE},
  {"belt", WEAR_SCABBARD, TRUE},
  {"inked", WEAR_TATTOO, TRUE},
  {"lodge_leg", WEAR_LODGED_L, FALSE},
  {"lodge_arm", WEAR_LODGED_A, FALSE},
  {"lodge_rib", WEAR_LODGED_R, FALSE},
  {NULL, 0, 0}
};

const struct flag_type container_flags[] = {
  {"closeable", 1, TRUE},
  {"pickproof", 2, TRUE},
  {"closed", 4, TRUE},
  {"locked", 8, TRUE},
  {"puton", 16, TRUE},
  {NULL, 0, 0}
};

/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/




const struct flag_type ac_type[] = {
  {"pierce", AC_PIERCE, TRUE},
  {"bash", AC_BASH, TRUE},
  {"slash", AC_SLASH, TRUE},
  {"exotic", AC_EXOTIC, TRUE},
  {NULL, 0, 0}
};


const struct flag_type size_flags[] = {
  {"small", SIZE_SMALL, TRUE},
  {"medium", SIZE_MEDIUM, TRUE},
  {"large", SIZE_LARGE, TRUE},
  {NULL, 0, 0},
};


const struct flag_type weapon_class[] = {
  {"exotic", WEAPON_EXOTIC, TRUE},
  {"sword", WEAPON_SWORD, TRUE},
  {"dagger", WEAPON_DAGGER, TRUE},
  {"spear", WEAPON_SPEAR, TRUE},
  {"staff", WEAPON_STAFF, TRUE},
  {"mace", WEAPON_MACE, TRUE},
  {"axe", WEAPON_AXE, TRUE},
  {"flail", WEAPON_FLAIL, TRUE},
  {"whip", WEAPON_WHIP, TRUE},
  {"polearm", WEAPON_POLEARM, TRUE},
  {"bow", WEAPON_BOW, TRUE},
  {NULL, 0, 0}
};


const struct flag_type weapon_type2[] = {
  {"flaming", WEAPON_FLAMING, TRUE},
  {"frost", WEAPON_FROST, TRUE},
  {"vampiric", WEAPON_VAMPIRIC, TRUE},
  {"sharp", WEAPON_SHARP, TRUE},
  {"vorpal", WEAPON_VORPAL, TRUE},
  {"twohands", WEAPON_TWO_HANDS, TRUE},
  {"shocking", WEAPON_SHOCKING, TRUE},
  {"poison", WEAPON_POISON, TRUE},
  {NULL, 0, 0}
};

const struct flag_type position_flags[] = {
  {"dead", POS_DEAD, FALSE},
  {"mortal", POS_MORTAL, FALSE},
  {"incap", POS_INCAP, FALSE},
  {"stunned", POS_STUNNED, FALSE},
  {"sleeping", POS_SLEEPING, TRUE},
  {"resting", POS_RESTING, TRUE},
  {"sitting", POS_SITTING, TRUE},
  {"fighting", POS_FIGHTING, FALSE},
  {"standing", POS_STANDING, TRUE},
  {NULL, 0, 0}
};

const struct flag_type target_flags[] = {
  {"ignore", TAR_IGNORE, TRUE},
  {"char_offensive", TAR_CHAR_OFFENSIVE, TRUE},
  {"char_defensive", TAR_CHAR_DEFENSIVE, TRUE},
  {"char_self", TAR_CHAR_SELF, TRUE},
  {"obj_inventory", TAR_OBJ_INV, TRUE},
  {"obj_char_defensive", TAR_OBJ_CHAR_DEF, TRUE},
  {"obj_char_offensive", TAR_OBJ_CHAR_OFF, TRUE},
  {NULL, 0, 0}
};

const struct flag_type portal_flags[] = {
  {"normal_exit", GATE_NORMAL_EXIT, TRUE},
  {"go_with", GATE_GOWITH, TRUE},
  {"buggy", GATE_BUGGY, TRUE},
  {"random", GATE_RANDOM, TRUE},
  {NULL, 0, 0}
};

const struct flag_type furniture_flags[] = {
  {"stand_at", STAND_AT, TRUE},
  {"stand_on", STAND_ON, TRUE},
  {"stand_in", STAND_IN, TRUE},
  {"sit_at", SIT_AT, TRUE},
  {"sit_on", SIT_ON, TRUE},
  {"sit_in", SIT_IN, TRUE},
  {"rest_at", REST_AT, TRUE},
  {"rest_on", REST_ON, TRUE},
  {"rest_in", REST_IN, TRUE},
  {"sleep_at", SLEEP_AT, TRUE},
  {"sleep_on", SLEEP_ON, TRUE},
  {"sleep_in", SLEEP_IN, TRUE},
  {"put_at", PUT_AT, TRUE},
  {"put_on", PUT_ON, TRUE},
  {"put_in", PUT_IN, TRUE},
  {"put_inside", PUT_INSIDE, TRUE},
  {"is_place", IS_PLACE, TRUE},
  {NULL, 0, 0}
};

const struct flag_type cmd_flags[] = {
  {"changed", CMD_CHANGED, FALSE},
  {"deleted", CMD_DELETED, FALSE},
  {"disabled", CMD_DISABLED, TRUE},
  {"guild_only", CMD_GUILD_ONLY, TRUE},
  {"removed", CMD_REMOVED, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type apply_types[] = {
  {"affects", TO_AFFECTS, TRUE},
  {"object", TO_OBJECT, TRUE},
  {"immune", TO_IMMUNE, TRUE},
  {"resist", TO_RESIST, TRUE},
  {"vuln", TO_VULN, TRUE},
  {"weapon", TO_WEAPON, TRUE},
  {"room", TO_ROOM_AFF, FALSE},
  {NULL, 0, TRUE}
};

const struct bit_type bitvector_type[] = {
  {_affected_by_bits, "affect"},
  {apply_flags, "object"},
  {_resistance_bits, "imm"},
  {_resistance_bits, "res"},
  {_resistance_bits, "vuln"},
  {weapon_type2, "weapon"},
  {NULL, "",}
};

const struct flag_type _config_bits[] = {
  {"ansi", _ANSI, TRUE},
  {"empty", _SHOW_EMPTY, TRUE},
  {"compact", _COMPACT, TRUE},
  {"brief", _BRIEF, TRUE},
  {"combine", _COMBINE, TRUE},
  {"telnet", _TELNET_GA, TRUE},
  {"affects", _SHOW_AFFECTS, TRUE},
  {"echo", _ECHO, TRUE},
  {"weather", _NOWEATHER, TRUE},
  {"hunger", _NOHUNGER, TRUE},
  {"quiet", _QUIET, TRUE},
  {"deaf", _DEAF, TRUE},
  {"afk", _AFK, TRUE},
  {"follow", _NOFOLLOW, TRUE},
  {"prompt", _PROMPT, TRUE},
  {"mudftp", _MUDFTP, TRUE},
  {"sound", _SOUNDON, TRUE},
  {"building", _BUILDING, FALSE},
  {"holylight", _HOLYLIGHT, FALSE},
  {"damage", _SHOW_DAMAGE, TRUE},
  {"snoop_proof", _SNOOP_PROOF, FALSE},
  {"autoassist", _AUTOASSIST, TRUE},
  {"autoexit", _AUTOEXIT, TRUE},
  {"autoloot", _AUTOLOOT, TRUE},
  {"autosac", _AUTOSAC, TRUE},
  {"autogold", _AUTOGOLD, TRUE},
  {"autosplit", _AUTOSPLIT, TRUE},
  {"compression", _COMPRESSION, FALSE},
  {"canloot", _CANLOOT, FALSE},
  {"guild", _SHOWGUILD, TRUE},
  {"wholevel", _WHO_LEVEL, FALSE},
  {"whowhere", _WHO_WHERE, FALSE},
  {"whostat", _WHO_SEX, FALSE},
  {NULL, 0, FALSE}

};


const struct flag_type _act_bits[] = {
  {"npc", _ACT_NPC, FALSE},
  {"assassin", _ACT_ASSASSIN, TRUE},
  {"sentinel", _ACT_SENTINEL, TRUE},
  {"scavenger", _ACT_SCAVENGER, TRUE},
  {"aggressive", _ACT_AGGRESSIVE, TRUE},
  {"mount", _ACT_MOUNT, TRUE},
  {"stay_area", _ACT_STAY_AREA, TRUE},
  {"wimpy", _ACT_WIMPY, TRUE},
  {"pet", _ACT_PET, TRUE},
  {"train", _ACT_TRAIN, TRUE},
  {"practice", _ACT_PRACTICE, TRUE},
  {"channeler", _ACT_CHANNELER, TRUE},
  {"rogue", _ACT_ROGUE, TRUE},
  {"warrior", _ACT_WARRIOR, TRUE},
  {"noalign", _ACT_NOALIGN, TRUE},
  {"nopurge", _ACT_NOPURGE, TRUE},
  {"outdoors", _ACT_OUTDOORS, TRUE},
  {"indoors", _ACT_INDOORS, TRUE},
  {"healer", _ACT_HEALER, TRUE},
  {"gain", _ACT_GAIN, TRUE},
  {"update_always", _ACT_UPDATE_ALWAYS, TRUE},
  {"banker", _ACT_BANKER, TRUE},
  {"nochanneling", _ACT_NOCHANNELLING, FALSE},
  {"stablehand", _ACT_STABLEHAND, TRUE},
  {"mortalleader", _ACT_MORTAL_LEADER, FALSE},
  {"blademaster", _ACT_BLADEMASTER, TRUE},
  {"wolfbrother", _ACT_WOLFBROTHER, TRUE},
  {"insane", _ACT_INSANE, TRUE},
  {"tennant", _ACT_TENNANT, FALSE},
  {"restringer", _ACT_RESTRINGER, TRUE},
  {"assist_all", _ACT_ASSIST_ALL, TRUE},
  {"assist_align", _ACT_ASSIST_ALIGN, TRUE},
  {"assist_race", _ACT_ASSIST_RACE, TRUE},
  {"assist_guard", _ACT_ASSIST_GUARD, TRUE},
  {"assist_vnum", _ACT_ASSIST_VNUM, TRUE},
  {"unkillable", _ACT_UNKILLABLE, FALSE},
  {"persistant", _ACT_PERSISTANT, TRUE},
  {NULL, 0, FALSE}

};

const struct flag_type _penalty_bits[] = {
  {"permit", _PERMIT, TRUE},
  {"log", _LOG, TRUE},
  {"deny", _DENY, TRUE},
  {"freeze", _FREEZE, TRUE},
  {"nowiz", _NOWIZ, TRUE},
  {"noauction", _NOAUCTION, TRUE},
  {"nogossip", _NOGOSSIP, TRUE},
  {"noquestion", _NOQUESTION, TRUE},
  {"noanswer", _NOQUESTION, TRUE},
  {"noclan", _NOCLAN, TRUE},
  {"noquote", _NOQUOTE, TRUE},
  {"noooc", _NOOOC, TRUE},
  {"noemote", _NOEMOTE, TRUE},
  {"noshout", _NOSHOUT, TRUE},
  {"notell", _NOTELL, TRUE},
  {"nobond", _NOBOND, TRUE},
  {"nograts", _NOGRATS, TRUE},
  {"noguild", _NOGUILD, TRUE},
  {"nopromote", _NOPROMOTE, TRUE},
  {"nopk", _NOPK, TRUE},
  {"noloot", _NOLOOT, TRUE},
  {"nofatal", _NOFATALS, TRUE},
  {"norecall", _NORECALL, TRUE},
  {"nobonding", _NOBONDING, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type _comm_bits[] = {
  {"ooc", _OOC, TRUE},
  {"shouts", _SHOUTS, TRUE},
  {"tells", _TELLS, TRUE},
  {"grats", _GRATS, TRUE},
  {"question", _QUESTION, TRUE},
  {"guild", _GUILD, TRUE},
  {"quotes", _QUOTES, TRUE},
  {"auction", _AUCTION, TRUE},
  {"bond", _BOND, TRUE},
  {"minion", _MINION, TRUE},
  {"gossip", _GOSSIP, TRUE},
  {"quote", _QUOTE, TRUE},
  {"wiznet", _WIZ, FALSE},
  {"towertalk", _TOWER_TALK, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type _wiznet_bits[] = {
  {"on", WIZ_ON, TRUE},
  {"ticks", WIZ_TICKS, TRUE},
  {"logins", WIZ_LOGINS, TRUE},
  {"sites", WIZ_SITES, TRUE},
  {"links", WIZ_LINKS, TRUE},
  {"deaths", WIZ_DEATHS, TRUE},
  {"resets", WIZ_RESETS, TRUE},
  {"mobdeaths", WIZ_MOBDEATHS, TRUE},
  {"flags", WIZ_FLAGS, TRUE},
  {"penalties", WIZ_PENALTIES, TRUE},
  {"saccing", WIZ_SACCING, TRUE},
  {"levels", WIZ_LEVELS, TRUE},
  {"secure", WIZ_SECURE, TRUE},
  {"switches", WIZ_SWITCHES, TRUE},
  {"snoops", WIZ_SNOOPS, TRUE},
  {"restores", WIZ_RESTORE, TRUE},
  {"loads", WIZ_LOAD, TRUE},
  {"newbies", WIZ_NEWBIE, TRUE},
  {"prefix", WIZ_PREFIX, TRUE},
  {"spam", WIZ_SPAM, TRUE},
  {"bugs", WIZ_BUGS, TRUE},
  {"trace",WIZ_TRACE, TRUE},
  {NULL, 0, FALSE}
};


const struct flag_type _form_bits[] = {
  {"edible", _EDIBLE, TRUE},
  {"poisonous", _POISONOUS, TRUE},
  {"magical", _MAGICAL, TRUE},
  {"instant_decay", _INSTANT_DECAY, TRUE},
  {"animal", _ANIMAL, TRUE},
  {"sentient", _SENTIENT, TRUE},
  {"construct", _CONSTRUCT, TRUE},
  {"mist", _MIST, TRUE},
  {"intangible", _INTANGIBLE, TRUE},
  {"biped", _BIPED, TRUE},
  {"insect", _INSECT, TRUE},
  {"spider", _SPIDER, TRUE},
  {"crustacean", _CRUSTACEAN, TRUE},
  {"worm", _WORM, TRUE},
  {"mammal", _MAMMAL, TRUE},
  {"bird", _BIRD, TRUE},
  {"reptile", _REPTILE, TRUE},
  {"snake", _SNAKE, TRUE},
  {"dragon", _DRAGON, TRUE},
  {"amphibian", _AMPHIBIAN, TRUE},
  {"fish", _FISH, TRUE},
  {"cold blooded", _COLD_BLOOD, TRUE},
  {"horse", _HORSE, TRUE},
  {NULL, 0, FALSE}
};


const struct flag_type _part_bits[] = {
  {"head", _HEAD, TRUE},
  {"arms", _ARMS, TRUE},
  {"legs", _LEGS, TRUE},
  {"heart", _HEART, TRUE},
  {"brains", _BRAINS, TRUE},
  {"guts", _GUTS, TRUE},
  {"hands", _HANDS, TRUE},
  {"feet", _FEET, TRUE},
  {"fingers", _FINGERS, TRUE},
  {"ear", _EAR, TRUE},
  {"eye", _EYE, TRUE},
  {"long_tongue", _LONG_TONGUE, TRUE},
  {"fins", _FINS, TRUE},
  {"wings", _WINGS, TRUE},
  {"tail", _TAIL, TRUE},
  {"claws", _CLAWS, TRUE},
  {"fangs", _FANGS, TRUE},
  {"horns", _HORNS, TRUE},
  {"scales", _SCALES, TRUE},
  {"tusks", _TUSKS, TRUE},
  {"hair", _HAIR, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type _affected_by_bits[] = {
  {"blind", _AFF_BLIND, TRUE},
  {"invisible", _AFF_INVISIBLE, TRUE},
  {"detect_evil", _AFF_DETECT_EVIL, TRUE},
  {"detect_good", _AFF_DETECT_GOOD, TRUE},
  {"detect_invis", _AFF_DETECT_INVIS, TRUE},
  {"detect_weave", _AFF_DETECT_WEAVE, TRUE},
  {"detect_hidden", _AFF_DETECT_HIDDEN, TRUE},
  {"infrared", _AFF_INFRARED, TRUE},
  {"sense_evil", _AFF_SENSE_EVIL, TRUE},
  {"poison", _AFF_POISON, TRUE},
  {"protection evil", _AFF_PROTECT_EVIL, TRUE},
  {"protection good", _AFF_PROTECT_GOOD, TRUE},
  {"sneak", _AFF_SNEAK, TRUE},
  {"hide", _AFF_HIDE, TRUE},
  {"sleep", _AFF_SLEEP, TRUE},
  {"compulsion", _AFF_COMPULSION, TRUE},
  {"flying", _AFF_FLYING, TRUE},
  {"haste", _AFF_HASTE, TRUE},
  {"calm", _AFF_CALM, TRUE},
  {"plague", _AFF_PLAGUE, TRUE},
  {"weaken", _AFF_WEAKEN, TRUE},
  {"dark vision", _AFF_DARK_VISION, TRUE},
  {"berserk", _AFF_BERSERK, TRUE},
  {"swim", _AFF_SWIM, TRUE},
  {"regeneration", _AFF_REGENERATION, TRUE},
  {"slow", _AFF_SLOW, TRUE},
  {"embraced", _AFF_EMBRACED, TRUE},
  {"captured", _AFF_CAPTURED, TRUE},
  {"bound_air", _AFF_BOUND_AIR, TRUE},
  {"blind_air", _AFF_BLIND_AIR, TRUE},
  {"gag_air", _AFF_GAG_AIR, TRUE},
  {"feral", _AFF_FERAL, TRUE},
  {"warfare", _AFF_WARFARE, TRUE},
  {"martyrdom", _AFF_MARTYRDOM, TRUE},
  {"cloaked", _AFF_CLOAKED, TRUE},
  {"air_shield", _AFF_AIR_SHIELD, TRUE},
  {"earth_shield", _AFF_EARTH_SHIELD, TRUE},
  {"fire_shield", _AFF_FIRE_SHIELD, TRUE},
  {"frost_shield", _AFF_FROST_SHIELD, TRUE},
  {"passdoor", _AFF_PASS_DOOR, TRUE},
  {"pain", _AFF_PAINED, TRUE},
  {"shielding", _AFF_SHIELDING},
  {"deaf", _AFF_DEAF},
  {"mute", _AFF_MUTE},
  {"major wound", _AFF_WOUNDED},
  {"lacerated", _AFF_LACERATED},
  {"dreaming",	_AFF_DREAMING},
  {NULL, 0, FALSE}
};


const struct flag_type _resistance_bits[] = {
  {"compulsion", _COMPULSION, TRUE},
  {"onepower", _ONEPOWER, TRUE},
  {"all_weapons", _WEAPON, TRUE},
  {"bash", _BASH, TRUE},
  {"pierce", _PIERCE, TRUE},
  {"slash", _SLASH, TRUE},
  {"fire", _FIRE, TRUE},
  {"cold", _COLD, TRUE},
  {"lightning", _LIGHTNING, TRUE},
  {"acid", _ACID, TRUE},
  {"poison", _POISON, TRUE},
  {"negative", _NEGATIVE, FALSE},
  {"good", _GOOD, FALSE},
  {"energy", _ENERGY, FALSE},
  {"mental", _MENTAL, TRUE},
  {"disease", _DISEASE, TRUE},
  {"drowning", _DROWNING, TRUE},
  {"light", _LIGHT, TRUE},
  {"pushing", _PUSH, TRUE},
  {"disarm", _DISARM, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type _off_bits[] = {
  {"area_attack", _OFF_AREA_ATTACK, TRUE},
  {"backstab", _OFF_BACKSTAB, TRUE},
  {"bash", _OFF_BASH, TRUE},
  {"berserk", _OFF_BERSERK, TRUE},
  {"disarm", _OFF_DISARM, TRUE},
  {"dodge", _OFF_DODGE, TRUE},
  {"fade", _OFF_FADE, TRUE},
  {"fast", _OFF_FAST, TRUE},
  {"kick", _OFF_KICK, TRUE},
  {"dirt_kick", _OFF_KICK_DIRT, TRUE},
  {"parry", _OFF_PARRY, TRUE},
  {"rescue", _OFF_RESCUE, TRUE},
  {"tail", _OFF_TAIL, TRUE},
  {"trip", _OFF_TRIP, TRUE},
  {"crush", _OFF_CRUSH, TRUE},
  {NULL, 0, FALSE}
};

const struct flag_type _system_bits[] = {
	{"enforce_embrace", _ENFORCE_EMBRACE, TRUE},
	{"check_burn",		_CHECK_BURN, TRUE},
	{"allow_kill_stealing", _ALLOW_KILL_STEALING, TRUE},
	{"who_class", _ALLOW_WHO_CLASS, TRUE},
	{"who_guild", _ALLOW_WHO_GUILD, TRUE},
	{"who_race", _ALLOW_WHO_RACE, TRUE},
	{"who_level", _ALLOW_WHO_LEVEL, TRUE},
	{"who_nation", _ALLOW_WHO_NATION, TRUE},
	{"guildonly_who", _GUILD_ONLY_IN_WHO, TRUE},
	{"save_delay", _SAVE_DELAY, TRUE},
	{"use_drunk", _USE_DRUNK, TRUE},
	{"use_ignore", _USE_IGNORE, TRUE},
	{"use_rdesc", _ALLOW_RDESC, TRUE},
	{"allow_bank_transfers", _ALLOW_BANK_TRANSFERS, TRUE},
	{"allow_bank_investing", _ALLOW_BANK_INVESTING, TRUE},
	{"count_loot", _COUNT_LOOT, TRUE},
	{"destroy_pk_loser", _DESTROY_LOSER, TRUE},
	{"ban_pk_loser_name", _BAN_LOSER_NAME, TRUE},
	{"force_customize", _FORCE_CUSTOMIZE, TRUE},
	{"determine_opr_by_sex", _DETERMINE_OPR_BY_SEX, TRUE},
	{"use_morgues", _USE_MORGUES, TRUE},
	{"use_old_gossip", _USE_OLD_GOSSIP, TRUE},
	{"allow_permakill", _ALLOW_PK, TRUE},
	{"health_affects_dmg", _MODIFY_DAMAGE_BY_HEALTH, TRUE},
	{"enable_compression", _ENABLE_COMPRESSION, TRUE},
	{NULL, 0, FALSE}
};

struct gsn_type gsn_table[] = {
//	{"axe", &gsn_axe },
//	{"dagger", &gsn_dagger },
//	{"flail", &gsn_flail },
//	{"mace", &gsn_mace },
//	{"polearm", &gsn_polearm },
//	{"spear", &gsn_spear },
//	{"staff", &gsn_staff },
	{"sword", &gsn_sword },
	{"forage", &gsn_forage },
//	{"whip", &gsn_whip },
//	{"bow", &gsn_bow },
//	{"lacerate", &gsn_lacerate },
//	{"auto dodge", &gsn_auto_dodge },
//	{"fourth attack", &gsn_fourth_attack },
//	{"dual wield", &gsn_dual_wield },
//	{"speardance", &gsn_speardance },
//	{"backstab", &gsn_backstab },
	{"dodge", &gsn_dodge },
//	{"hide", &gsn_hide },
//	{"sneak", &gsn_sneak },
//	{"disarm", &gsn_disarm },
//	{"enhanced damage", &gsn_enhanced_damage },
//	{"kick", &gsn_kick },
	{"parry", &gsn_parry },
//	{"rescue", &gsn_rescue },
//	{"second attack", &gsn_second_attack },
//	{"third attack", &gsn_third_attack },
	{"shield block", &gsn_shield_block },
//	{"bash", &gsn_bash },
//	{"berserk", &gsn_berserk },
//	{"dirt kick", &gsn_dirt },
//	{"hand to hand", &gsn_hand_to_hand },
//	{"trip", &gsn_trip },
//	{"fast healing", &gsn_fast_healing },
//	{"meditation", &gsn_meditation },
//	{"seize", &gsn_seize },
//	{"wrath", &gsn_wrath },
//	{"embrace", &gsn_embrace },
//	{"riposte", &gsn_riposte },
//	{"treat", &gsn_treat },
//	{"critical strike", &gsn_critical },
//	{"brawl", &gsn_brawl },
//	{"cloak", &gsn_cloak },
//	{"frenzy", &gsn_frenzy },
//	{"feral", &gsn_feral },
//	{"warfare", &gsn_warfare },
//	{"martyrdom", &gsn_martyrdom },
//	{"guardcrush", &gsn_guardcrush },
//	{"tumbling", &gsn_tumbling },
//	{"juggling", &gsn_juggling },
//	{"hone", &gsn_hone },
//	{"mend", &gsn_mend },
//	{"engrave", &gsn_engrave },
//	{"hurl", &gsn_hurl },
	{NULL, NULL}
};
