/**
 * @@ Copyright (Wheel of Time Project)
 *
 * Wheel of Time (WoT) Mud System v0.1a
 * Copyright (c) 1998, by Gary McNickle <gary@tarmongaidon.org>
 * Based on ROM Mud, v2.4b4 by Russ Taylor
 *
 * By using any of the WoT source code, you agree to follow the terms of
 * usage and liscensing detailed in the file "liscense.wot" as well as any
 * and all liscensing required for ROM, Merc, and Diku Mud systems.
 */

/**
 * @@ Objective (enums.h)
 * 
 * By enumerating many of the standard types used in WoT, I hope to
 * make debugging problems a bit simpler. The ability to 'print' a 
 * value and see it's text (word) name in gdb is a great help.
 * Further, as with any other form of typecasting, this should help
 * to keep instances of assigning the wrong value down to a minimum.
 * At least this way, the value assigned *must* be one of the legal
 * enumerations.
 */

/** 
 * Mud FTP extras for init_descriptor
 */
typedef enum 	{FTP_NORMAL, FTP_PUSH, FTP_PUSH_WAIT} ftp_mode;
typedef enum    {NDESC_NORMAL, NDESC_FTP} ndesc_t;

enum _FLOW
{
	ALL			=	-1,
	AIR			=	0,
	EARTH		=	1,
	WATER		=	2,
	FIRE		=	3,
	SPIRIT		=	4
};

enum _MUD_CONFIG
{
	_NONE						= 0,
	_ENFORCE_EMBRACE			= 1,
	_CHECK_BURN					= 2,
	_ALLOW_KILL_STEALING		= 3,
	_ALLOW_WHO_CLASS			= 4,
	_ALLOW_WHO_GUILD			= 5,
	_ALLOW_WHO_RACE				= 6,
	_ALLOW_WHO_LEVEL			= 7,
	_ALLOW_WHO_NATION			= 8,
	_GUILD_ONLY_IN_WHO			= 9,
	_SAVE_DELAY					= 10,
	_USE_DRUNK					= 11,
	_USE_IGNORE					= 12,
	_ALLOW_BANK_TRANSFERS		= 13,
	_ALLOW_BANK_INVESTING		= 14,
	_COUNT_LOOT					= 15,
	_DESTROY_LOSER				= 16,
	_BAN_LOSER_NAME				= 17,
	_ALLOW_RDESC				= 18,
	_FORCE_CUSTOMIZE			= 19,
	_DETERMINE_OPR_BY_SEX		= 20,
	_USE_MORGUES				= 21,
	_USE_OLD_GOSSIP				= 22,
	_ALLOW_PK					= 23,
	_MODIFY_DAMAGE_BY_HEALTH    = 24,
	_ENABLE_COMPRESSION			= 25
};

enum _CONDITION
{
	_DRUNK						= 0,
	_FULL						= 1,
	_THIRST						= 2,
	_HUNGER						= 3
};

enum _PCRACES
{
	HUMAN			=	0,
	OGIER			=	1,
	TROLLOC			=	2,
	FADE			=	3,
	DRAGHKAR		=	4
};

/** 
 * Sword form enums
 */
enum _FORM_TYPE
{
  SF_ATTACK			=	0,
  SF_DEFEND			=	1,
  SF_EITHER			=	2
};

enum _FORM_AFFECTS
{
  SF_NONE			= 	0,
  SF_AVOID			=	1,
  SF_RIPOSTE		=	2,
  SF_DAMAGE			=	3,
  SF_CRITICAL		=	4,
  SF_SIMUL			=	5,
  SF_EXTRA			=	6,
  SF_DISARM			=	7,
  SF_WEAPONBREAK	=	8,
  SF_KNOCKDOWN		=	9,
  SF_BLIND			=	10,
  SF_MULTI			=	11
};
 
/**
 * Weapon classes 
 */
enum _WEAPON_CLASS
{
 WEAPON_UNDEFINED	=  -1,
 WEAPON_EXOTIC		= 	0,
 WEAPON_SWORD		= 	1,
 WEAPON_DAGGER		= 	2,
 WEAPON_SPEAR		= 	3,
 WEAPON_MACE		= 	4,
 WEAPON_AXE			= 	5,
 WEAPON_FLAIL		= 	6,
 WEAPON_WHIP		= 	7,	
 WEAPON_POLEARM		= 	8,
 WEAPON_STAFF		= 	9,
 WEAPON_BOW			= 	10
};


/**
 * Sound Types
 */
enum _SOUND
{
 SOUND_SYSTEM		= 	0,
 SOUND_AREA			= 	1,
 SOUND_MOBILE 		= 	2,
 SOUND_OBJECT		= 	3,
 SOUND_ROOM			= 	4, 
 SOUND_IMMORTAL		= 	5
};

/**
 * Time and weather
 */
enum _TIME
{
 SUN_DARK			= 	0,
 SUN_RISE			= 	1,
 SUN_LIGHT			= 	2,
 SUN_SET			= 	3
};	

enum _WEATHER
{
 SKY_CLOUDLESS		= 	0,
 SKY_CLOUDY			= 	1,
 SKY_RAINING		= 	2,
 SKY_LIGHTNING		= 	3,
 SKY_HAIL			=   4,
 SKY_SUNNY			=	5,
 SKY_SNOW			=	6,
 SKY_FOGGY			=	7
};

enum _SEASON
{
 SEASON_WINTER		= 0,
 SEASON_SPRING		= 1,
 SEASON_SUMMER		= 2,
 SEASON_AUTUMN		= 3
};
/** 
 * Connected state for a channel.
 * Negative states show states where player is 'not' playing, ie: it's not 
 * a good idea to send text to them, or show them in who/www.
 */
enum _CON_STATES
{
 CON_GET_NAME			= 	-1,
 CON_GET_OLD_PASSWORD	= 	-2,
 CON_CONFIRM_NEW_NAME	= 	-3,
 CON_GET_NEW_PASSWORD	= 	-4,
 CON_CONFIRM_NEW_PASSWORD= 	-5,
 CON_GET_NEW_RACE		= 	-6,
 CON_GET_NEW_SEX		= 	-7,
 CON_ROLL_SSTATS		= 	-8,
 CON_GET_ALIGNMENT		= 	-9,
 CON_GET_NEW_ABILITIES	= 	-10, 
 CON_GEN_GROUPS			= 	-11, 
 CON_PICK_WEAPON		= 	-12,
 CON_BREAK_CONNECT		= 	-15,
 CON_COPYOVER_RECOVER	= 	-16,
 CON_ROLL_STATS			= 	-17,
 CON_GET_NEW_PORT   	= 	-18,
 CON_CHOOSE_CHANNEL		= 	-19,
 CON_GET_NEW_SIZE		= 	-20,
 CON_GET_COLOR			= 	-21,
 CON_GET_NEW_CITY		= 	-22,
 CON_GET_NEW_SHIPNAME  	= 	-23,
 CON_PLAYING			= 	0,
 CON_NOTE_TO			= 	1,
 CON_NOTE_SUBJECT		= 	2,
 CON_NOTE_EXPIRE		= 	3,
 CON_NOTE_TEXT			= 	4,
 CON_NOTE_FINISH		= 	5,
 CON_READ_IMOTD			= 	6,
 CON_READ_MOTD			= 	7,
 CON_FTP_COMMAND		=	8,
 CON_FTP_DATA			=	9,
 CON_FTP_AUTH			=	10
};	

/**
 * TO target types for act.
 */
enum _ACT_TARGETS
{
 TO_ROOM			=	0,
 TO_NOTVICT			=	1,
 TO_VICT			=	2,	
 TO_CHAR			=	3,
 TO_ALL				=	4,
 TO_PLACE			=	5
};


/** 
 * 'where' target definitions (affects)
 */
enum _AFFECT_TARGETS
{
 TO_AFFECTS			=	0,	
 TO_OBJECT			=	1,		
 TO_IMMUNE			=	2,	
 TO_RESIST			=	3,	
 TO_VULN			=	4,	
 TO_WEAPON			=	5,	
 TO_ROOM_AFF		=	6	
};

/**
 * Skill/Weave Targets 
 */
enum _SKILL_TARGETS
{
 TAR_IGNORE			=	0,		    
 TAR_CHAR_OFFENSIVE	=	1,	    
 TAR_CHAR_DEFENSIVE	=	2,	    
 TAR_CHAR_SELF		=	3,		    
 TAR_OBJ_INV		=	4,		    
 TAR_OBJ_CHAR_DEF	=	5,	    
 TAR_OBJ_CHAR_OFF	=	6,	    
 TAR_INSANE 		=	7
};

/** 
 * Generic Targets
 */
enum _GEN_TARGETS
{
 TARGET_CHAR		= 	0,
 TARGET_OBJ			= 	1,
 TARGET_ROOM		= 	2,
 TARGET_NONE		= 	3
};

/**
 * Damage classes 
 */
enum _DAM_TYPE
{
 DAM_NONE			=	0,
 DAM_BASH			=	1,
 DAM_PIERCE			=	2,
 DAM_SLASH			=	3,
 DAM_FIRE			=	4,
 DAM_COLD			=	5,
 DAM_LIGHTNING		=	6,
 DAM_ACID			=	7,
 DAM_POISON			=	8,
 DAM_NEGATIVE		=	9,
 DAM_HOLY			=	10,
 DAM_ENERGY			=	11,
 DAM_MENTAL			=	12,
 DAM_DISEASE		=	13,
 DAM_DROWNING		=	14,
 DAM_LIGHT			=	15,	
 DAM_OTHER			=	16,
 DAM_HARM			=	17,
 DAM_CHARM			=	18,
 DAM_SOUND			=	19,
 DAM_WEAVE			=	20,
 DAM_WHIP			=	21,
 DAM_CHOP			=	22,
 DAM_AIR			=	23,
 DAM_EARTH			=	24
};

/**
 * Sex types
 */
enum _SEX
{
 SEX_INVALID		=  -1, 
 SEX_NEUTRAL		= 	0,
 SEX_MALE			= 	1,
 SEX_FEMALE			= 	2
};

/** 
 * Mob and PC sizes
 */
enum _SIZE
{
 SIZE_SMALL			=	0,
 SIZE_MEDIUM		=	1,
 SIZE_LARGE			=	2,

};

/**
 * Item types.
 */
enum _ITEM_TYPE
{
 ITEM_NONE			=     0,
 ITEM_LIGHT			=     1,
	
 ITEM_WAND			=     3,
 ITEM_STAFF			=     4,
 ITEM_WEAPON		=     5,

 ITEM_TREASURE		=     8,
 ITEM_ARMOR			=     9,
 ITEM_POTION		=    10,
 ITEM_CLOTHING		=    11,
 ITEM_FURNITURE		=    12,
 ITEM_TRASH			=    13,
 ITEM_CONTAINER		=    15,
 ITEM_DRINK_CON		=    17,
 ITEM_KEY			=    18,
 ITEM_FOOD			=    19,
 ITEM_MONEY			=    20,
 ITEM_BOAT			=    22,
 ITEM_CORPSE_NPC	=    23,
 ITEM_CORPSE_PC		=    24,
 ITEM_FOUNTAIN		=    25,
 ITEM_HERB			=    26,
 ITEM_PROTECT		=    27,
 ITEM_MAP			=    28,
 ITEM_PORTAL		=    29,
 ITEM_ROOM_KEY		=    31,
 ITEM_GEM			=    32,
 ITEM_JEWELRY		=    33,
 ITEM_JUKEBOX		=    34,
 ITEM_TATTOO		=    35,
 ITEM_WAYGATE		=    36,
 ITEM_GATEMARKER	=    37,
 ITEM_QUIVER		=    38,
 ITEM_ARROW			=    39,
 ITEM_ANGREAL		=	 40,
 ITEM_HCLOAK        =    41,
 // ToS
 ITEM_CREW          =    42

};

/**
 * Apply types (for affects).
 */
enum _APPLY_TYPES
{
 APPLY_NONE			=	0,	      
 APPLY_STR			=	1,
 APPLY_DEX			=	2,
 APPLY_INT			=	3,
 APPLY_WIS			=	4,
 APPLY_CON			=	5,
 APPLY_SEX			=	6,
 APPLY_CLASS		=	7,
 APPLY_LEVEL		=	8,
 APPLY_AGE			=	9,
 APPLY_HEIGHT		=	10,
 APPLY_WEIGHT		=	11,
 APPLY_ABILITY	 	=	12,
 APPLY_HIT			=	13,
 APPLY_ENDURANCE	=	14,
 APPLY_GOLD			=	15,
 APPLY_EXP			=	16,
 APPLY_AC			=	17,
 APPLY_HITROLL		=	18,
 APPLY_DAMROLL		=	19,
 APPLY_SAVES		=	20,
 APPLY_SAVING_SPELL	=	21,
 APPLY_SPELL_AFFECT	=	22,
 APPLY_AIR			=	23,
 APPLY_EARTH		=	24,
 APPLY_WATER		=	25,
 APPLY_FIRE			=	26,
 APPLY_SPIRIT		=	27
};

/**
 * Position States
 */
enum _POS_STATES
{
  POS_INVALID       =	-1,
  POS_DEAD			=	0,
  POS_MORTAL		=	1,
  POS_INCAP			=	2,
  POS_STUNNED		=	3,
  POS_SLEEPING		=	4,
  POS_RESTING		=	5,
  POS_SITTING		=	6,
  POS_FIGHTING		=	7,
  POS_WORKING		=   8,
  POS_PLAYING		=	9,
  POS_STANDING		=	10
};

/**
 * AC types 
 */
enum _AC_TYPES
{
 AC_PIERCE			=	0,
 AC_BASH			=	1,
 AC_SLASH			=	2,
 AC_EXOTIC			=	3
};	

/**
 * Connected states for OLC editors.
 */
enum _OLC_EDITOR
{
 ED_NONE			=	0,
 ED_AREA			=	1,
 ED_ROOM			=	2,
 ED_OBJECT			=	3,
 ED_MOBILE			=	4,
 ED_MPCODE			=	5,
 ED_GUILD      		= 	6,
 ED_HELP			=	7,
 ED_GROUP			=	8,
 ED_WEAVE			=	9,
 ED_SKILL			=	10,
 ED_CMD				=	11,
 ED_AUCTION			=	12,
 ED_TREESING		=   13,
 ED_QGEN			=	14
};

/**
 * Directions.
 */
enum _DIRECTIONS
{
 DIR_NORTH			=	 0,
 DIR_EAST			=	 1,
 DIR_SOUTH			=	 2,
 DIR_WEST			=	 3,
 DIR_UP				=	 4,
 DIR_DOWN			=	 5
};	

/**
 * Equpiment wear locations.
 */
enum _WEAR_LOC
{
 WEAR_NONE			=	-1,
 WEAR_LIGHT			=	0,
 WEAR_FINGER_L		=	1,	
 WEAR_FINGER_R		=	2,
 WEAR_NECK_1		=	3,
 WEAR_NECK_2		=	4,
 WEAR_BODY			=	5,
 WEAR_HEAD			=	6,
 WEAR_LEGS			=	7,
 WEAR_FEET			=	8,
 WEAR_HANDS			=	9,
 WEAR_ARMS			=	10,
 WEAR_SHIELD		=	11,
 WEAR_ABOUT			=	12,
 WEAR_WAIST			=	13,
 WEAR_WRIST_L		=	14,
 WEAR_WRIST_R		=	15,
 WEAR_WIELD			=	16,
 WEAR_HOLD			=	17,
 WEAR_FLOAT			=	18,
 WEAR_SECONDARY		=	19,
 WEAR_SCABBARD  	=	20,
 WEAR_FACE      	=	21,
 WEAR_EARRING_L		=	22,
 WEAR_EARRING_R 	=	23,
 WEAR_TATTOO    	=	24,
 WEAR_LODGED_R		= 	25,
 WEAR_LODGED_L		=	26,
 WEAR_LODGED_A		=	27,
 WEAR_SHOULDER		=	28,
 MAX_WEAR			=	29
};

/**
 * Area and room terrain (sector) types
 */
enum _SECTOR_TYPES
{
 SECT_INSIDE		= 	0,
 SECT_CITY			= 	1,
 SECT_FOREST		= 	2,
 SECT_HILLS			= 	3,
 SECT_MOUNTAIN		=   4,
 SECT_WATER			= 	5,
 SECT_UNUSED		= 	6, 
 SECT_DESERT		= 	7,
 SECT_GRASSLAND     =   8,
 SECT_JUNGLE		=   9,
 SECT_VANTAGE		=	10,
 SECT_RIVER			=	11,
 SECT_SWAMP			=	12,
 SECT_ROAD			=	13,
 SECT_OCEAN			=	14,
 SECT_BEACH			=	15,
 SECT_MAX			= 	16
};

/**
 * Statistic identifiers
 */
enum _STAT
{
 STAT_STR 			=	0,
 STAT_INT			=	1,
 STAT_AGL			=	2,
 STAT_DEX			=	3,
 STAT_CON			=	4,
 STAT_SEN			=	5
};


/*
 * Abilitiy identifiers
 */

enum _ABILITY
{ 

 ABILITY_FASTFEET           = 1,
 ABILITY_STEPSOFT			= 2,
 ABILITY_WORDLYWISE			= 3,
 ABILITY_SHARPSENSES		= 4,
 ABILITY_LEADERSHIP			= 5,
 ABILITY_MASTERTOUCH		= 6,
 ABILITY_LONGWIND			= 7,
 ABILITY_DEADEYE			= 8,
 ABILITY_IGTERRAIN			= 9,
 ABILITY_IGTEMP				= 10,
 ABILITY_IGPAIN				= 11,
 ABILITY_PREDWEATHER		= 12,
 ABILITY_IGNOREWEATHER		= 13,
 ABILITY_PERPITCH			= 14,
 ABILITY_DRUMPLAYING		= 15,
 ABILITY_FLUTEPLAYING		= 16,
 ABILITY_HORNPLAYING		= 17,
 ABILITY_HARPPLAYING		= 18

};


/*
 * Impediment identifiers
 */

enum _IMPEDIMENT
{ 

 IMPEDIMENT_PRIDE		= 1,
 IMPEDIMENT_GLUTTONY	= 2,
 IMPEDIMENT_SELFISHNESS	= 3,
 IMPEDIMENT_VENGEANCE	= 4,
 IMPEDIMENT_ANTISOCIAL	= 5,
 IMPEDIMENT_SHOWOFF		= 6,
 IMPEDIMENT_ALCOHOLISM	= 7,
 IMPEDIMENT_PREJUDICED	= 8,
 IMPEDIMENT_CRIPPLED	= 9,
 IMPEDIMENT_LONELINESS	= 10,
 IMPEDIMENT_PARANOIA	= 11,
 IMPEDIMENT_SPEECHPROB	= 12,
 IMPEDIMENT_MUTE		= 13,
 IMPEDIMENT_DEAF		= 14,
 IMPEDIMENT_LAZINESS	= 15,
 IMPEDIMENT_CONTROLFREAK= 16,
 IMPEDIMENT_INSOLENCE	= 17

};



	

	

	
	



	
		


