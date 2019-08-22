
/*
 * @@ Objective (merc.h)
 * 
 * This file contains, or includes other header files that contain globally
 * visible, or external declarations for the WoT Mud project.
 */

#ifndef _MERC_H
#define _MERC_H

#ifdef __cplusplus
  extern "C" {
#endif


#if defined WINDOWS  || defined _WINDOWS || defined _WIN32 || defined WIN32
	#ifndef WIN32
		#define WIN32
	#endif
	#ifndef _WIN32
		#define _WIN32
	#endif
#endif

#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun


#define FALSE	 0
#define TRUE	 1

#if defined WIN32
  #define APPNAME "WoTMud.exe"
  #define crypt(s1, s2) (s1)
#endif

typedef short		int				sh_int;


#if !defined __cplusplus
	typedef unsigned	char		bool;
#endif


#if defined(WIN32)
  #include <winsock.h>
  #include <time.h>
#else
  #include <sys/time.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
#endif

#if defined(_WIN32)
  #pragma warning(disable:4018)	// '==' : signed/unsigned mismatch
  #pragma warning(disable:4024) // 'symbol' : different types for formal and actual parameter 1
  #pragma warning(disable:4047) // 'function' : 'symbol' differs in levels of indirection from 'int '
  #pragma warning(disable:4244)	// conversion from 'double ' to 'int ', possible loss of data
  #pragma warning(disable:4273) // 'symbol' : : inconsistent dll linkage.  dllexport assumed.
  #pragma warning(disable:4305)	// 'initializing' : truncation from 'const int ' to 'const char '
  #pragma warning(disable:4761) // integral size mismatch in argument; conversion supplied

  #if !defined(_export)
    #ifdef WOTMUDDLL_EXPORTS
      #define _export __declspec(dllexport)
    #else
      #define _export __declspec(dllimport)
    #endif
  #endif

#else
  #if !defined(_export)
    #define _export extern
  #endif
#endif


#include <setjmp.h>
#include "zlib.h"
#include "config.h"  	/* various mud specific options		*/
#include "gsns.h"		/* all gsns declared here			*/
#include "ansi.h"		/* ANSI color codes && config		*/
#include "enums.h"      /* Enumerated data types			*/
#include "bitopt.h"  	/* Bit array definitions			*/

/* 
 * Structure types
 */
typedef struct  cmd_type			CMD_DATA;
typedef struct  form_type			FORM_DATA;
typedef struct  skill_type			SKILL_DATA;
typedef struct  nation_data			NATION_DATA;
typedef struct	affect_data			AFFECT_DATA;
typedef struct	area_data			AREA_DATA;
typedef struct	ban_data			BAN_DATA;
typedef struct 	buf_type	 		BUFFER;
typedef struct	char_data			CHAR_DATA;
typedef struct	descriptor_data		DESCRIPTOR_DATA;
typedef struct	exit_data			EXIT_DATA;
typedef struct	extra_descr_data	EXTRA_DESCR_DATA;
typedef struct 	web_descriptor 		WEB_DESCRIPTOR;
typedef struct	help_data			HELP_DATA;
typedef struct  kill_data           KILL_DATA;
typedef struct	mob_index_data		MOB_INDEX_DATA;
typedef struct	note_data			NOTE_DATA;
typedef struct	obj_data			OBJ_DATA;
typedef struct	obj_index_data		OBJ_INDEX_DATA;
typedef struct	pc_data				PC_DATA;
typedef struct  gen_data			GEN_DATA;
typedef struct	reset_data			RESET_DATA;
typedef struct	room_index_data		ROOM_INDEX_DATA;
typedef struct	shop_data			SHOP_DATA;
typedef struct	time_info_data		TIME_INFO_DATA;
typedef struct	weather_data		WEATHER_DATA;
typedef struct  mprog_list			MPROG_LIST;
typedef struct  mprog_code			MPROG_CODE;
typedef struct  login_data			LOGIN_DATA;
typedef struct  clan_type			CLAN_DATA;
typedef struct  group_type			GROUP_DATA;
typedef	struct  name_data			NAME_DATA;
typedef struct  sound_data			SOUND_DATA;
typedef struct 	flow_types 			_flow;
typedef struct  mud_data			MUD;
typedef struct 	board_data 			BOARD_DATA;
typedef struct 	guild_rank       	RANK_DATA;
typedef struct	auction_data		AUCTION_DATA;
typedef struct  bidder_data			BIDDER_DATA;
typedef struct	update_data			UPDATE_DATA;
typedef struct	wiz_data			WIZ_DATA;

/*
 * File Read/Write Return Codes
 */
#define READ_POOR		0 /* non-critical error on read		*/
#define READ_OK 		1 /* successful load of file		*/
#define READ_ERROR		2 /* critical error on read		*/
#define WRITE_POOR		0 /* non-critical error on write	*/
#define WRITE_OK		1 /* successful write to file		*/
#define WRITE_ERROR     2 /* critical error during write	*/
#define ERROR_UNDEFINED 3 /* typically, file does not exist	*/

/*
 * Abort/Exit error definitions
 */
#define ERR_ABORT	3	// Exit with error code 3, calling abort() instead of exit()


#define A		  	1
#define B			2
#define C			4
#define D			8
#define E			16
#define F			32
#define G			64
#define H			128

#define I			256
#define J			512
#define K		    1024
#define L		 	2048
#define M			4096
#define N		 	8192
#define O			16384
#define P			32768

#define Q			65536
#define R			131072
#define S			262144
#define T			524288
#define U			1048576
#define V			2097152
#define W			4194304
#define X			8388608

#define Y			16777216
#define Z			33554432
#define aa			67108864 
#define bb			134217728
#define cc			268435456    
#define dd			536870912
#define ee			1073741824


/* 
 * Maneuver Defines
 */
#define	DEFENSIVE_ONLY			A	// Defensive maneuver only
#define OFFENSIVE_ONLY			B	// Offensive maneuver only
#define REQUIRES_SKILLED_HELP	C	// Requires at least one other with maneuver skill
#define REQUIRES_FOUR_OR_MORE	D	// Requires a group size of four or more
#define REQUIRES_MOVEMENTS		E	// Requires user to have some movement remaining
#define REQUIRES_STR_18			F	// Requires a str of 18 or more
#define PROJECTILES_ONLY		G   // Works only against projectile weapons
 
#define	REDUCE_DAMAGE_BY_HALF	A
#define REDUCE_DAMAGE_BY_THIRD  B
#define REDUCE_DAMAGE_BY_TENTH	C
#define REDUCE_DAMAGE_BY_ALL    D
#define REVERSE_DAMAGE			E	// takes damage reduced, and applies it to opponent
#define DAMAGE_OPPONENTS_WEAPON F   // takes damage reduced, and applies to opponents weapon
#define DAMAGE_OPPONENTS		G	// Does an amount of damage to opponent
#define DAMAGE_MOUNTS			H	// Apply full damage done to mounted opponents mount
#define DAMAGE_EXTRA			I	// Doubles the normal damage done, and apply to opponent
#define CANCEL_RESCUE			J   // Designed to negate an enemies' Rescue 
#define COVER_PARTY				K	// Takes damage assigned to party member
#define COMBAT_HALT             L   // Stops combat.
#define GROUPED_DMG				M   // Applies damage of assigned random group member
#define CANCEL_FLEE             N   // Negates an enemies' attempt to flee
#define MISSED_TRIP				O	// maneuver trips if maneuver-guy misses attack


/* 
 * Various array position identifiers
 */
#define GROUP_TABLE				0
#define SKILL_TABLE				1
#define TALENT_TABLE			2

#define CHANNELER 				0
#define ROGUE					1
#define WARRIOR					2

#define LIQ_WATER				0


#define MEM_MAXSKILL			0
#define MEM_MAXGROUP			1


/*
 * Command flags
 */
#define CMD_CHANGED				A
#define CMD_DELETED				B
#define CMD_DISABLED			C
#define	CMD_GUILD_ONLY			D
#define CMD_REMOVED				E

/*
 * One Power Sphere / Flow array positions
 */
#define CURRENT	 				0 
#define PERM	 				1 


/*
 * Notoriety array positions
 */
#define INFAMY					0
#define FAME					1


/* 
 * Function Type defines
 */
typedef	void DO_FUN		( CHAR_DATA *ch, char *argument );
typedef bool SPEC_FUN	( CHAR_DATA *ch );
typedef void WEAVE_FUN  ( int sn, int level, CHAR_DATA *ch, void *vo,
                          int target, char *arg, bool is_obj );
                   
#define _WEAVE( fun )  void fun(int sn, int level, CHAR_DATA *ch, void *vo, \
								int target, char *arg, bool is_obj)
			     
#define _DOFUN( fun )  void fun(CHAR_DATA *ch, char *argument)


/* 
 * Memory management.
 * Increase MAX_STRING if you have to.
 * Tune the others only if you understand what you're doing.
 */
#define	MAX_STRING			1024*10000
#define	MAX_PERM_BLOCK		131072
#define	MAX_MEM_LIST		12
#define	MAX_KEY_HASH		1031 /* A prime# */
#define MAX_STRING_LENGTH	4608
#define MAX_INPUT_LENGTH	512
#define MAX_BUF_SIZE        32768

#define PAGELEN				22
#define MSL					MAX_STRING_LENGTH
#define MIL					MAX_INPUT_LENGTH

/* 
 * Mud Compression Protocol
 */
#define TELOPT_COMPRESS		85
#define COMPRESS_BUF_SIZE	16384


/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_NATIONS 		40
#define MAX_CITIES			30
#define MAX_TALENTS			15
#define MAX_IN_GROUP		15
#define MAX_ALIAS			10
#define MAX_CLASS			3
#define MAX_RANK			20
#define MAX_DAMAGE_MESSAGE	42
#define MAX_LIQUIDS			30
#define MAX_TRADE	 		5
#define MAX_RACE_HASH       128
#define MAX_SKILL_HASH		26 
#define MAX_BOARD			10
#define MAX_GUILD 			2
#define MAX_STATS 			6
#define MAX_REROLLS     	10
#define MAX_TODO			20
#define MAX_MANEUVERS		13
#define MAX_ABILITY         18
#define MAX_IMPEDIMENT      17

/*
 * Level definitions 
 */
#define MAX_LEVEL			60
#define LEVEL_HERO			(MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		(MAX_LEVEL - 8)
#define ML      			MAX_LEVEL       /* implementor */
#define L1      			MAX_LEVEL - 1   /* coders */
#define L2      			MAX_LEVEL - 2   /* council */
#define L3      			MAX_LEVEL - 3   /* unused */
#define L4      			MAX_LEVEL - 4   /* guild imms/builders */
#define L5      			MAX_LEVEL - 5   /* guild imm assistants */
#define L6      			MAX_LEVEL - 6   /*  */
#define L7      			MAX_LEVEL - 7   /*  */
#define L8      			MAX_LEVEL - 8   /* RP imms */
#define IM      			LEVEL_IMMORTAL  /* guest imms */
#define HE      			LEVEL_HERO      /*  */

#define IMPLEMENTOR			MAX_LEVEL
#define	CREATOR				(MAX_LEVEL - 1)
#define SUPREME				(MAX_LEVEL - 2)
#define DEITY				(MAX_LEVEL - 3)
#define GOD					(MAX_LEVEL - 4)
#define IMMORTAL			(MAX_LEVEL - 5)
#define DEMI				(MAX_LEVEL - 6)
#define ANGEL				(MAX_LEVEL - 7)
#define AVATAR				(MAX_LEVEL - 8)
#define HERO				LEVEL_HERO

/*
 * Pulse defines
 * Defaults are:
 * 4 pulses per second
 * 1 tick = 1 minute
 * Areas update every 2 minutes
 * Auction updates every 30 seconds
 * Violence updates every 3 seconds
 * Mobiles update ever 4 seconds
 * Songs update ever 6 seconds
 */
#define PULSE_PER_SECOND	4
#define PULSE_VIOLENCE		( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		( 4 * PULSE_PER_SECOND)
#define PULSE_MUSIC			( 6 * PULSE_PER_SECOND)
#define PULSE_TICK			( 60 * PULSE_PER_SECOND)
#define PULSE_AREA			( 120 * PULSE_PER_SECOND)
#define PULSE_AUCTION       ( 30 * PULSE_PER_SECOND) /* 30 seconds */
#define PULSE_AGGRESS		( 3 * PULSE_PER_SECOND )
//#define PULSE_WEATHER		( 30 * PULSE_TICK )
#define PULSE_WEATHER		( 10 * PULSE_PER_SECOND )
#define PULSE_WEATHERAFF    ( 30 * PULSE_PER_SECOND )

#define VIOLENCE_PERIOD		( 3 * PULSE_TICK ) /* 3 minutes */

/* 
 * Mud State bits.
 */
#define MUD_DEFAULT			0
#define MUD_NEWLOCK			A
#define MUD_IMMLOCK			B
#define MUD_DEBUG			C
#define MUD_USE_RESOLVE		D
#define MUD_CLOSED          E
#define MUD_COPYOVER		F
#define MUD_REDIRECT		G /* see config.h for redirect url */

/*
 * Site ban bits.
 */
#define BAN_SUFFIX			A
#define BAN_PREFIX			B
#define BAN_NEWBIES			C
#define BAN_ALL				D	
#define BAN_PERMIT			E
#define BAN_PERMANENT		F

#define ERROR_PROG 	       -1
#define OWEAR_PROG 			A
#define OGET_PROG			B
#define OJUNK_PROG			C
#define OEXAM_PROG			D
#define ODROP_PROG			E
#define OSAY_PROG			F
#define ORAND_PROG			G
#define OREMOVE_PROG		H


/* 
 * Sound Triggers
 */
#define SOUND_NONE					0 
#define SOUND_SYSTEM_LOGON			1
#define SOUND_SYSTEM_QUIT			2

#define SOUND_AREA_ENTER			1
#define SOUND_AREA_LEAVE			2
#define SOUND_AREA_WAKE				3
#define SOUND_AREA_SLEEP			4
#define SOUND_AREA_DEATH			5
#define SOUND_AREA_COMBAT			6
#define SOUND_AREA_SUN_DARK			7
#define SOUND_AREA_SUN_RISE			8
#define SOUND_AREA_SUN_LIGHT		9
#define SOUND_AREA_SUN_SET			10
#define SOUND_AREA_CLOUDLESS		11
#define SOUND_AREA_CLOUDY			12
#define SOUND_AREA_RAINING			13
#define SOUND_AREA_LIGHTNING		14

#define SOUND_MOB_DEATHCRY			1
#define SOUND_MOB_FIGHTING			2
#define SOUND_MOB_LEVEL				3
#define SOUND_MOB_SLAIN				4

#define SOUND_IMM_ADVANCE			1 
#define SOUND_IMM_POOFIN			2
#define SOUND_IMM_POOFOUT			3
#define SOUND_IMM_SLAY				4
#define SOUND_IMM_PURGE				5
#define SOUND_IMM_RESTORE			6

#define SOUND_OBJ_DROP				1
#define SOUND_OBJ_GET				2
#define SOUND_OBJ_STRIKE			3
#define SOUND_OBJ_DEFEND			4
#define SOUND_OBJ_DAMAGE_90			5
#define SOUND_OBJ_DAMAGE_80			6
#define SOUND_OBJ_DAMAGE_70			7
#define SOUND_OBJ_DAMAGE_60			8
#define SOUND_OBJ_DAMAGE_50			9
#define SOUND_OBJ_DAMAGE_40			10
#define SOUND_OBJ_DAMAGE_30			11
#define SOUND_OBJ_DAMAGE_20			12
#define SOUND_OBJ_DAMAGE_10			13
#define SOUND_OBJ_DAMAGE_00			14
#define SOUND_OBJ_ENCHANT			15
#define SOUND_OBJ_DISENCHANT		16
#define SOUND_OBJ_GIVE				17

#define SOUND_ROOM_ENTER_ANY		1
#define SOUND_ROOM_ENTER_NORTH		2 
#define SOUND_ROOM_ENTER_EAST		3 
#define SOUND_ROOM_ENTER_SOUTH		4
#define SOUND_ROOM_ENTER_WEST		5
#define SOUND_ROOM_ENTER_UP			6
#define SOUND_ROOM_ENTER_DOWN		7
#define SOUND_ROOM_EXIT_ANY			8 
#define SOUND_ROOM_EXIT_NORTH		9 
#define SOUND_ROOM_EXIT_EAST		10 
#define SOUND_ROOM_EXIT_SOUTH		11
#define SOUND_ROOM_EXIT_WEST		12
#define SOUND_ROOM_EXIT_UP			13
#define SOUND_ROOM_EXIT_DOWN		14
#define SOUND_ROOM_WAKE				15
#define SOUND_ROOM_SLEEP			16
#define SOUND_ROOM_RESET			17
#define SOUND_ROOM_DEATH			18
#define SOUND_ROOM_COMBAT			19
#define SOUND_ROOM_DARK				20
#define SOUND_ROOM_LIGHT			21

/* return values for check_imm */
#define IS_NORMAL					0
#define IS_IMMUNE					1
#define IS_RESISTANT				2
#define IS_VULNERABLE				3


/* dice */
#define DICE_NUMBER 				0
#define DICE_TYPE 					1
#define DICE_BONUS					2

/*
 * Command logging types.
 */
#define LOG_NORMAL					0
#define LOG_ALWAYS					1
#define LOG_NEVER					2
 

/*
 * Well known virtual numbers.
 */
#define MOB_VNUM_FIDO				3090
#define MOB_VNUM_DISGUISE			3060
#define MOB_VNUM_WOLF				4001

#define OBJ_VNUM_SILVER_ONE			1
#define OBJ_VNUM_GOLD_ONE			2
#define OBJ_VNUM_GOLD_SOME			3
#define OBJ_VNUM_SILVER_SOME		4
#define OBJ_VNUM_COINS				5
#define OBJ_VNUM_GENERIC			6
#define OBJ_VNUM_CORPSE_NPC			10
#define OBJ_VNUM_CORPSE_PC			11
#define OBJ_VNUM_SEVERED_HEAD		12
#define OBJ_VNUM_TORN_HEART			13
#define OBJ_VNUM_SLICED_ARM			14
#define OBJ_VNUM_SLICED_LEG			15
#define OBJ_VNUM_GUTS				16
#define OBJ_VNUM_BRAINS				17
#define OBJ_VNUM_MUSHROOM			20
#define OBJ_VNUM_LIGHT_BALL			21
#define OBJ_VNUM_SPRING				22
#define OBJ_VNUM_DISC				23
#define OBJ_VNUM_PORTAL				25
#define OBJ_VNUM_ROSE				26
#define OBJ_VNUM_PIT				9070
#define OBJ_VNUM_STEAK				11041
#define OBJ_VNUM_SCHOOL_MACE		3700
#define OBJ_VNUM_SCHOOL_DAGGER		3701
#define OBJ_VNUM_SCHOOL_SWORD		3702
#define OBJ_VNUM_SCHOOL_VEST		3703
#define OBJ_VNUM_SCHOOL_SHIELD		3704
#define OBJ_VNUM_SCHOOL_BOW			3705
#define OBJ_VNUM_SCHOOL_BANNER		3716
#define OBJ_VNUM_SCHOOL_SPEAR		3717
#define OBJ_VNUM_SCHOOL_STAFF		3718
#define OBJ_VNUM_SCHOOL_AXE			3719
#define OBJ_VNUM_SCHOOL_FLAIL		3720
#define OBJ_VNUM_SCHOOL_WHIP		3721
#define OBJ_VNUM_SCHOOL_POLEARM		3722
#define OBJ_VNUM_MAP				3162
#define OBJ_VNUM_WHISTLE			2116
#define OBJ_VNUM_GAIDIN_CLOAK		6880
#define OBJ_VNUM_SWORD_AIR			280
#define OBJ_VNUM_SWORD_FIRE			281
#define OBJ_VNUM_DAGGER_AIR			282
#define OBJ_VNUM_DAGGER_FIRE		283
#define OBJ_VNUM_FOOD				283
#define OBJ_VNUM_WALL_FIRE			284
#define OBJ_VNUM_WALL_AIR			285
#define OBJ_VNUM_ARROW				286

// Weather objects
#define OBJ_VNUM_TORNADO			205

#define ROOM_ISLAND					3015

#define ROOM_VNUM_LIMBO				2
#define ROOM_VNUM_CHAT				1200
#define ROOM_VNUM_LOGIN	   			9309 
#define ROOM_VNUM_SCHOOL			3700
#define ROOM_VNUM_RENTBOX			4000
#define ROOM_VNUM_DONATION			9331

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW			(A)
#define ITEM_HUM			(B)
#define ITEM_GHOST			(C)
#define ITEM_LOCK			(D)
#define ITEM_EVIL			(E)
#define ITEM_INVIS			(F)
#define ITEM_MAGIC			(G)
#define ITEM_NODROP			(H)
#define ITEM_BLESS			(I)
#define ITEM_ANTI_GOOD		(J)
#define ITEM_ANTI_EVIL		(K)
#define ITEM_ANTI_NEUTRAL	(L)
#define ITEM_NOREMOVE		(M)
#define ITEM_INVENTORY		(N)
#define ITEM_NOPURGE		(O)
#define ITEM_ROT_DEATH		(P)
#define ITEM_LODGED			(R)
#define ITEM_VIS_DEATH		(Q)
#define ITEM_NONMETAL		(S)
#define ITEM_NOLOCATE		(T)
#define ITEM_MELT_DROP		(U)
#define ITEM_HAD_TIMER		(V)
#define ITEM_TREESUNG		(W)
#define ITEM_GUILD			(X)
#define ITEM_BURN_PROOF		(Y)
#define ITEM_NOUNCURSE		(Z)
#define ITEM_QUEST          (aa) /* Item is a quest prize. 		*/
#define ITEM_NOBREAK		(bb) /* Item never breaks      		*/
#define ITEM_HIDDEN			(cc) /* Item is reset as 'hidden' 	*/
#define ITEM_NOCOUNT		(dd) /* Item is not counted in maxitems */
#define ITEM_NOSAVE			(ee) /* Item is not saved to pfile      */

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE			(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK		(C)
#define ITEM_WEAR_BODY		(D)
#define ITEM_WEAR_HEAD		(E)
#define ITEM_WEAR_LEGS		(F)
#define ITEM_WEAR_FEET		(G)
#define ITEM_WEAR_HANDS		(H)
#define ITEM_WEAR_ARMS		(I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT		(K)
#define ITEM_WEAR_WAIST		(L)
#define ITEM_WEAR_WRIST		(M)
#define ITEM_WIELD			(N)
#define ITEM_HOLD			(O)
#define ITEM_NO_SAC			(P)
#define ITEM_WEAR_FLOAT		(Q)
#define ITEM_WEAR_SCABBARD  (R)
#define ITEM_WEAR_TATTOO    (S)
#define ITEM_WEAR_FACE      (T)
#define ITEM_WEAR_EARRING	(U)
#define ITEM_WEAR_SHOULDER	(V)
#define ITEM_WEAR_LODGED    (W)

/* weapon types */
#define WEAPON_FLAMING		(A)
#define WEAPON_FROST		(B)
#define WEAPON_VAMPIRIC		(C)
#define WEAPON_SHARP		(D)
#define WEAPON_VORPAL		(E)
#define WEAPON_TWO_HANDS	(F)
#define WEAPON_SHOCKING		(G)
#define WEAPON_POISON		(H)

/* gate flags */
#define GATE_NORMAL_EXIT	(A)
#define GATE_NOCURSE		(B)
#define GATE_GOWITH			(C)
#define GATE_BUGGY			(D)
#define GATE_RANDOM			(E)

/* furniture flags */
#define STAND_AT			(A)
#define STAND_ON			(B)
#define STAND_IN			(C)
#define SIT_AT				(D)
#define SIT_ON				(E)
#define SIT_IN				(F)
#define REST_AT				(G)
#define REST_ON				(H)
#define REST_IN				(I)
#define SLEEP_AT			(J)
#define SLEEP_ON			(K)
#define SLEEP_IN			(L)
#define PUT_AT				(M)
#define PUT_ON				(N)
#define PUT_IN				(O)
#define PUT_INSIDE			(P)
#define IS_PLACE			(S)

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		A
#define CONT_PICKPROOF		B
#define CONT_CLOSED			C
#define CONT_LOCKED			D
#define CONT_PUT_ON			E


/* 
 * Door flags
 * for door resets
 */
#define DOOR_OPEN			A
#define DOOR_CLOSED			B
#define DOOR_LOCKED			C
#define DOOR_SECRET			D 

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK			(A)
#define ROOM_NO_GATE        (B)
#define ROOM_NO_MOB			(C)
#define ROOM_INDOORS		(D)
#define ROOM_NOSNEAK		(E)
#define ROOM_PRIVATE		(J)
#define ROOM_SAFE			(K)
#define ROOM_SOLITARY		(L)
#define ROOM_PET_SHOP		(M)
#define ROOM_NO_RECALL		(N)
#define ROOM_IMP_ONLY		(O)
#define ROOM_GODS_ONLY		(P)
#define ROOM_HEROES_ONLY	(Q)
#define ROOM_NEWBIES_ONLY	(R)
#define ROOM_LAW			(S)
#define ROOM_NOWHERE		(T)
#define ROOM_RENTAL			(U)
#define ROOM_RENTED			(V)
#define ROOM_ARENA          (W)
#define ROOM_NOMAGIC		(X)
#define ROOM_STEDDING		(Y)
#define ROOM_MEN_ONLY		(Z)
#define ROOM_WOMEN_ONLY		(aa)
#define ROOM_MOUNT_SHOP     (bb)
#define ROOM_DEPOSITORY     (cc)
#define ROOM_STABLE			(dd)
// ToS Room flags
#define ROOM_PORT			(ee)

/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR	      	(A)
#define EX_CLOSED	      	(B)
#define EX_LOCKED	      	(C)
#define EX_SECRET	      	(D)
#define	EX_RANDOM	      	(E)
#define EX_PICKPROOF	    (F)
#define EX_NOPASS	      	(G)
#define EX_EASY		      	(H)
#define EX_HARD		      	(I)
#define EX_INFURIATING	    (J)
#define EX_NOCLOSE	      	(K)
#define EX_NOLOCK	      	(L)





/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 * MOBprog definitions
 */                   
#define TRIG_ACT	(A)
#define TRIG_BRIBE	(B)
#define TRIG_DEATH	(C)
#define TRIG_ENTRY	(D)
#define TRIG_FIGHT	(E)
#define TRIG_GIVE	(F)
#define TRIG_GREET	(G)
#define TRIG_GRALL	(H)
#define TRIG_KILL	(I)
#define TRIG_HPCNT	(J)
#define TRIG_RANDOM	(K)
#define TRIG_SPEECH	(L)
#define TRIG_EXIT	(M)
#define TRIG_EXALL	(N)
#define TRIG_DELAY	(O)
#define TRIG_SURR	(P)


/*
 * Utility macros.
 */
#define IS_VALID(data)				((data) != NULL && (data)->valid)
#define VALIDATE(data)				((data)->valid = TRUE)
#define INVALIDATE(data)			((data)->valid = FALSE)
#define UMIN(a, b)					((a) < (b) ? (a) : (b))
#define UMAX(a, b)					((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)				((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)					((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)					((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)			((flag) & (bit))
#define SET_BIT(var, bit)			((var) |= (bit))
#define REMOVE_BIT(var, bit)		((var) &= ~(bit))
#define IS_ANSI(ch)					(ch && !IS_NPC( ch ) && IS_FLAG((ch)->states.comm, _ANSI)) 
#define IS_NULLSTR(str)				( (str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)			( ((min) < (num)) && ((num) < (max)) )
#define IS_VALID_SKILL(skill)		( ((skill) > 0 && (skill) < maxSkill))

/* 
 * Bit Set Macros.
 */
#define IS_LINKDEAD(ch)				( IS_NPC((ch)) ? FALSE : !( (ch) && (ch)->desc ) ) 
#define IS_FLAG( flag, bit )		( ((bit)>MAXELEMS) ? 0 : ((unsigned) (flag)[ (bit) / UL_BITS ] >>  \
									(bit) % UL_BITS  & 01 ))
#define SET_FLAG(flag, bit)			( (flag)[(bit)/UL_BITS] |= 1 << (bit)%UL_BITS )
#define REMOVE_FLAG(flag, bit)		( (flag)[(bit)/UL_BITS] &= ~(1 << (bit)%UL_BITS ))

/* 
 * OLC Macros.
 */
#define SOUND_TABLE(Type)			(	(Type) == SOUND_IMMORTAL ? imm_sound_table :  \
										(Type) == SOUND_MOBILE ? mob_sound_table :    \
										(Type) == SOUND_OBJECT ? obj_sound_table :    \
										(Type) == SOUND_ROOM ? room_sound_table :     \
										(Type) == SOUND_AREA ? area_sound_table :     \
										system_sound_table )
					
#define IS_HIDDEN(ch)				(	IS_AFFECTED((ch), _AFF_SNEAK)     || \
										IS_AFFECTED((ch), _AFF_HIDE)      || \
										IS_AFFECTED((ch), _AFF_INVISIBLE) || \
										IS_AFFECTED((ch), _AFF_CLOAKED) )
#define IS_SWITCHED(ch)					( (ch)->desc->original )    
#define GET_SKILL(ch, sn)				((IS_NPC((ch))) ? 0 : ((ch)->pcdata->learned[(sn)])<101 ? (ch)->pcdata->learned[(sn)] : 100)
#define SET_SKILL(ch, sn, rate)			((IS_NPC((ch))) ? 0 : ((ch)->pcdata->learned[(sn)]=(rate)))
#define INCREASE_SKILL(ch, sn, rate)	((IS_NPC((ch))) ? 0 : ((ch)->pcdata->learned[(sn)] += (rate)))
#define DECREASE_SKILL(ch, sn, rate)	((IS_NPC((ch))) ? 0 : ((ch)->pcdata->learned[(sn)] -= (rate)))

#define TOGGLE_BIT(var, bit)			((var) ^= (bit))
#define TOGGLE_FLAG(var, bit)			((var[bit/UL_BITS]) ^= (1 << bit%UL_BITS)) 

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)				( (Mob) = (MOB_INDEX_DATA *)(Ch)->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)				( (Obj) = (OBJ_INDEX_DATA *)(Ch)->desc->pEdit )
#define EDIT_ROOM(Ch, Room)				( (Room) = (Ch)->in_room )
#define EDIT_AREA(Ch, Area)				( (Area) = (AREA_DATA *)(Ch)->desc->pEdit )
#define EDIT_GUILD(Ch, Clan)			( (Clan) = (Ch)->desc->pEdit )
#define EDIT_GROUP(Ch, Group)			( (Group) = (Ch)->desc->pEdit )
#define EDIT_MPCODE(Ch, Code)			( (Code) = (MPROG_CODE*)(Ch)->desc->pEdit )
#define EDIT_SKILL(Ch, Skill)			( (Skill) = (SKILL_DATA *)(Ch)->desc->pEdit )
#define EDIT_CMD(Ch, Cmd)				( (Cmd) = (CMD_DATA *)(Ch)->desc->pEdit )
#define IS_BUILDER(ch, Area)			( !IS_SWITCHED( (ch) ) &&                   \
										( (ch)->pcdata->security >= (Area)->security  \
										|| strstr( (Area)->builders, (ch)->name )     \
										|| strstr( (Area)->builders, "All" ) ) )


/* 
 * Character macros.
 */
#define CH(descriptor)					((descriptor)->original ? (descriptor)->original : (descriptor)->character)
#define IS_NPC(ch)						(IS_FLAG((ch)->states.player, _ACT_NPC) || ((ch)->pIndexData && (ch)->pIndexData->vnum > 0))
#define IS_IMMORTAL(ch)					(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)						(get_trust((ch)) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)			(get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)				(IS_FLAG((ch)->states.affected_by, (sn)))

#define GET_ID(ch)						(IS_NPC((ch)) ? 0 : (ch)->pcdata->id)
#define GET_AGE(ch)						((int) (17 + ((ch)->played + current_time - (ch)->logon )/72000))

#define IS_GOOD(ch)						(ch->alignment >= 350)
#define IS_EVIL(ch)						(ch->alignment <= -350)
#define IS_NEUTRAL(ch)					(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)					(ch->position > POS_SLEEPING)
#define GET_AC(ch,type)					((ch)->armor[type]  + 	\
											( IS_AWAKE(ch)	    	\
											? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))  
#define GET_HITROLL(ch)					((ch)->hitroll)
#define GET_DAMROLL(ch) 				((ch)->damroll)

#define IS_OUTSIDE(ch)					(!IS_SET((ch)->in_room->room_flags,ROOM_INDOORS))
#define WAIT_STATE(ch, npulse)			((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)			((ch)->daze = UMAX((ch)->daze, (npulse)))
#define get_carry_weight(ch)			((ch)->carry_weight + (ch)->silver/10 + (ch)->gold * 2 / 5)
#define HAS_TRIGGER(ch,trig)			(IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define MOUNTED(ch) 					((!IS_NPC((ch)) && (ch)->mount && (ch)->riding) ? (ch)->mount : NULL)
#define RIDDEN(ch) 						((IS_NPC((ch)) && (ch)->master && \
											(ch)->master->mount && \
											(ch)->master->mount == (ch)  && \
											(ch)->master->riding) ? (ch)->master : NULL) 

#define IS_DRUNK(ch)					(((ch)->pcdata->condition[_DRUNK] > 20))
#define TRUESEX(ch)						((IS_NPC(ch)) ? (ch)->sex : (ch)->pcdata->true_sex)


#define MAXFLOWSTR(ch)					( IS_FLAG(pMud.config, _DETERMINE_OPR_BY_SEX) ? ( (ch)->sex == SEX_MALE ? 8 : 6 ) : 8 )

/*
 * Macro   : OPR  (One Power Rating)
 * Descr   : Determines the total One Power Rating (Ability) of a player
 * Written : v0.1 6/23/98
 * Author  : Gary McNickle
 **/
#define OPR(Ch)		      (((Ch)->flow_str.perm[AIR]   + \
							(Ch)->flow_str.perm[EARTH] + \
							(Ch)->flow_str.perm[WATER] + \
							(Ch)->flow_str.perm[FIRE]  + \
							(Ch)->flow_str.perm[SPIRIT]))
                                 
/*
 * Macro   : TAU (Total Available Units of The One Power)
 * Descr   : Determines the total available One Power Rating of a player
 * Written : v0.1 6/23/98
 * Author  : Gary McNickle
 **/

#define TAU(Ch)		      (((Ch)->flow_str.current[AIR]   + \
							(Ch)->flow_str.current[EARTH] + \
							(Ch)->flow_str.current[WATER] + \
							(Ch)->flow_str.current[FIRE]  + \
							(Ch)->flow_str.current[SPIRIT]))


/*
 * Macro   : TPR (Total Power Requirement of a weave)
 * Descr   : Determines the total OPU requirement of a weave.
 * Written : v0.1 6/23/98
 * Author  : Gary McNickle
 **/
#define TPR(skill_table, sn)  (((skill_table)[(sn)].flow_str[AIR]    + \
								(skill_table)[(sn)].flow_str[EARTH]  + \
								(skill_table)[(sn)].flow_str[WATER]  + \
								(skill_table)[(sn)].flow_str[FIRE]   + \
								(skill_table)[(sn)].flow_str[SPIRIT]))


#define IS_LINKED(Ch)			( (Ch)->linked_to > 0 )

/* 
 * Object macros.
 */
#define CAN_WEAR(obj, part)		(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)		((obj)->item_type == ITEM_CONTAINER ? (obj)->value[4] : 100)



/* 
 * Global constants.
 */
extern	char *	const					dir_name[];
extern	const	sh_int					rev_dir[];
extern	const	struct	str_app_type	str_app[30];
extern	const	struct	int_app_type	int_app[26];
extern	const	struct	wis_app_type	wis_app[26];
extern	const	struct	dex_app_type	dex_app[26];
extern	const	struct	con_app_type	con_app[26];
extern	const	struct	weapon_type		weapon_table[];
extern  const struct  item_type		item_table[];
extern	const	struct	wiznet_type		wiznet_table[];
extern	const	struct	attack_type		attack_table[];
extern  const	struct	spec_type		spec_table[];
extern	const	struct	flag_stat_type  flag_stat_table[];
extern	const	struct	spec_type		spec_table[];
extern	const	struct	position_type	position_table[];
extern	const	struct	sex_type		sex_table[];
extern	const	struct	size_type		size_table[];

/* 
 * Global variables.
 */
extern  	struct	slay_type			slay_table[];
extern      KILL_DATA					kill_table[];
extern		NATION_DATA	  				nation_table[MAX_NATIONS];
extern 		BOARD_DATA 					boards[MAX_BOARD];
extern      char						last_command[MIL];
extern		struct	class_type			class_table[MAX_CLASS];
extern		struct	liq_type			liq_table[MAX_LIQUIDS];
extern		struct	skill_type*			skill_table;
extern		struct	talent_type			talent_table[MAX_TALENTS];
extern		struct	class_type			class_table[MAX_CLASS];
extern		struct	slay_type			slay_table[MAX_SLAY_TYPES];
extern		struct	social_type*		social_table;
extern  	struct  race_type*			race_table;
extern		struct	pc_race_type*		pc_race_table;
extern		struct	clan_type*			clan_table;
extern 		struct 	group_type*			group_table;
extern		struct  form_type*			form_table;
extern		struct	cmd_type*			cmd_table;
extern		struct	maneuver_type		maneuver_table[MAX_MANEUVERS];

extern		AFFECT_DATA*				obj_aff_list;
extern		AFFECT_DATA*				room_aff_list;
extern		AFFECT_DATA*				char_aff_list;
extern		AFFECT_DATA*				exit_aff_list;

extern		CHAR_DATA*					char_list;
extern		DESCRIPTOR_DATA*			descriptor_list;
extern		DESCRIPTOR_DATA*			d_next;
extern		OBJ_DATA*					object_list;
extern		MPROG_CODE*					mprog_list;
extern		UPDATE_DATA*				motd_list;
extern		UPDATE_DATA*				imotd_list;
extern		UPDATE_DATA*				jobs_list;
extern      AUCTION_DATA*				house_auctions;
extern      NOTE_DATA*					note_free;
extern      OBJ_DATA*					obj_free;
extern		MUD		  					pMud;
extern		time_t						current_time;
extern		bool						fLogAll;
extern		TIME_INFO_DATA				time_info;
extern		WEATHER_DATA				weather_info;
extern		bool						MOBtrigger;
extern		int							num_quotes;     
extern		int							sig_loops;
extern		int							maxSkill;
extern		int							maxClan;
extern		int 						maxSocial;
extern		int							maxRace;
extern		int							maxPCRace;
extern		int							maxGroup;
extern		int							maxForm;
extern		int							maxCommands;
extern		int							port;
extern		bool						AttemptJump;
extern		jmp_buf 					jump_env;
extern		bool						webUP;
extern		bool						merc_down;
extern		char						str_boot_time[MIL];
extern		FILE*						fpChar;

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 * United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 */
#define AREA_BACKUP_DIR	"/home/wotmud/Mud/Main/area/"	/* for backing up areas		*/
#define AREA_LIST		"area.lst"						/* List of areas			*/
#define EXE_FILE		"../src/wot"	  				/* again, for copyover		*/
#define DATA_DIR		"../data/"						/* root datafile dir		*/
#define LOG_DIR			"../data/logs/" 				/* log files	     		*/
#define GOD_DIR			"../data/gods/"  				/* list of gods 			*/
#define NOTE_DIR		"../data/notes/" 				/* note directory			*/
#define QUOTE_DIR		"../data/quotes/"				/* dir for quote files		*/
#define PLAYER_DIR		"../data/players/"				/* Player files    			*/
#define NPC_DIR			"../data/npcs/"					/* Persistant NPC files		*/
#define RST_DIR			"../data/rst/"					/* holds all .rst files		*/
#define SOUND_BASE_DIR	"" 								/* base sound directory		*/
#define TEMP_FILE		"wottmp.dat"					/* temp file    			*/
#define LAST_COMMAND	"last_command"					/* stored last command		*/
#define BUG_FILE        "bugs.txt"						/* For 'bug' and bug()		*/
#define TYPO_FILE       "typos.txt"						/* For 'typo'				*/
#define SHUTDOWN_FILE   "shutdown.dat"					/* For 'shutdown'			*/
#define BAN_FILE		"ban.dat"						/* stores banned sites		*/
#define MUSIC_FILE		"music.dat"						/* stores music data		*/
#define BANK_FILE       "bank.dat"						/* For bank info			*/
#define TIME_FILE		"time.dat"						/* Time file				*/
#define WEATHER_FILE	"weather.dat"						/* Weather file				*/
#define COPYOVER_FILE   "copyover.dat"					/* for copyover				*/
#define SOCIAL_FILE 	"social.dat"					/* for social editor		*/
#define GROUP_FILE		"groups.dat"					/* for group information	*/
#define RACE_FILE		"races.dat"						/* for race table			*/
#define LIQUID_FILE		"liquids.dat"					/* liquid data				*/
#define NAMES_FILE		"names.dat"						/* Banned names data file	*/
#define MUD_FILE		"mud.dat"						/* Mud data file			*/
#define SKILL_FILE 		"skills.dat"					/* Skill data file			*/
#define NATION_FILE 	"nation.dat"					/* For Region/Terrain data  */
#define CLASS_FILE		"class.dat"						/* Each class's data		*/
#define SLAY_FILE		"slay.dat"						/* Each of the slay types	*/
#define FORM_FILE		"forms.dat"						/* BM form data				*/
#define COMMAND_FILE	"commands.dat"					/* Command data file 		*/
#define AUCTION_FILE	"auctions.log"					/* Auction log file			*/
#define WIZ_FILE		"../data/wizlist.txt"
#define UPDATE_FILE		"../data/update.txt"



/* 
 * Structure for a maneuver
 */
struct maneuver_type
{
	char *name;
	char *toRoom;
	char *toVict;
	char *toChar;
	long type;
	long flags;
	float difficulty;
};


/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char *		name;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
    long		flags;
    char *		lookup_name;
};

/*
 * A kill structure (indexed by level).
 */
struct  kill_data
{
  sh_int              number;
  sh_int              killed;
};          

struct bidder_data
{
  CHAR_DATA  	*who;
  BIDDER_DATA	*next;
  bool		valid;
  long	     	max_bid;
  long		current_bid;
  long		opening_bid;
  long		time_opening_bid;
  long		time_last_bid;
};

struct auction_data
{
    AUCTION_DATA *next;
    CHAR_DATA	*seller;
    OBJ_DATA	*item;
    BIDDER_DATA *bidders;
    bool	valid;
    char	*notes;
    time_t	time_opened;
    time_t	time_closed;
    sh_int	number_for_sale;
    sh_int	auction_number;
    long	auction_id; 
    long	reserve_price;
    long	opening_price;
    long	bid_increment;
    long	high_bid;
    long	low_bid;
    bool	expired;
    int         going;
};


/* Data about a board */
struct board_data
{
  char *short_name; 			/* Max 8 chars								*/
  char *long_name;  			/* Explanatory text, should'nt be > 40 chars*/
  int  read_level; 				/* minimum level to see board				*/
  int  write_level;				/* minimum level to post notes				*/
  char *names;       			/* Default recipient						*/
  int  force_type; 				/* Default action (DEF_XXX)					*/
  int  purge_days; 				/* Default expiration						*/
  NOTE_DATA *note_first;		/* pointer to board's first note			*/
  bool changed; 				/* currently unused							*/
};


struct sound_data
{
  SOUND_DATA *next;
  bool   valid;
  char 	 *fname;				/* file name (indirect url) of sound file	*/
  int volume;					/* volume to play sound at					*/
  int repeats;					/* # of times to repeat sound				*/
  int priority;					/* sound priority							*/
  int trigger;					/* sound trigger 							*/
  bool music;					/* TRUE if music file, FALSE if sound file	*/
  enum _SOUND  type;			/* "TYPE" of sound							*/
  enum _ACT_TARGETS targets;	/* valid targets, as in ACT messages		*/
};


struct mud_data
{
  sh_int version;				/* version of datafile						*/
  long logins;					/* # of total logins 						*/
  long newchars;				/* # of new characters created				*/
  long newbies;					/* # of new players							*/
  int  deletions;				/* # of characters deleted by self			*/
  int  unlinks;					/* # of characters deleted by imms			*/
  long flags;					/* current mud flags						*/
  SET config;
  sh_int timer;					/* Timer for copyover						*/
  time_t last_cover;			/* last copyover time/date					*/
  SOUND_DATA *sounds;			/* System sound information					*/
  // Global mud configuration settings follow...

  char* mud_name;
  char* welcome_message;
  sh_int courageous;			// what level do you want players to be able to toggle peaceful status off/on at?
  sh_int level_no_peace;		// what level do you want to force no_peace flag to off?
  sh_int pk_limit;				// the range of levels allowable for pk
  int box_cost;					// the cost of a rentable box
  int share_value;
  sh_int MaxPCTalents;
  sh_int MaxPCSpecialties;
  bool DetermineOPRbySex;
  sh_int WaitTimer;
  sh_int ViolenceTimer;
  char *BaseSoundURL;
  int MaxCreationPoints;
  char* RedirectMSG;
  char* RedirectURL;

  float exp_mod;				/* Experience modifier						*/
};

/* structure for diabled/disallowed names */
struct name_data
{
   NAME_DATA *next;             /* ptr to next name							*/
   bool valid;                  /* is valid ptr?							*/
   char *name;                  /* disallowed name							*/
   char *reason;                /* reason disallowed						*/
   char *by;                    /* who disallowed							*/
   int  by_lvl;                 /* their level?								*/
   time_t date_stamp;           /* date disallowed							*/
};

struct  ban_data
{
    BAN_DATA *  next;
    bool        valid;
    long        ban_flags;
    sh_int      level;
    char *      name;
};

struct  update_data
{
    UPDATE_DATA *  next;
    char *      keyword;
    char *      information;
    bool	valid;
};

struct	wiz_data
{
    WIZ_DATA *	next;
    bool	valid;
    sh_int	level;
    char *	name;
    char * 	job;
    char *	email;
};

struct buf_type
{
    BUFFER *    next;
    bool        valid;
    sh_int      state;  /* error state of the buffer */
    int      	size;   /* size in k */
    char *      string; /* buffer's string */
};

struct  time_info_data
{
    int         hour;
    int         day;
	int			day_c;
    int         month;
    int         year;
	enum _SEASON season;
};

struct  weather_data
{
    int         	mmhg;
    int         	change;
	int				power;
	int				wchange;
	int				duration;
	int				prot;
    enum _WEATHER	sky;
	enum _WEATHER	old_sky;
    enum _TIME		sunlight;
};

// for function "makedrunk" 
struct struckdrunk
{
   int     min_drunk_level;
   int     number_of_rep;
   char    *replacement[11];
};

struct gsn_type
{
	char *skill;
	sh_int* gsn;
};

struct flag_type
{
    char *name;
    long bit;
    bool settable;
};

struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
};

struct size_type
{
    char *name;
};

struct	bit_type
{
	const	struct	flag_type *	table;
	char *				help;
};

        
/* 
 * Attribute bonus structures
 */
struct  str_app_type
{
    sh_int      tohit;
    sh_int      todam;
    sh_int      carry;
    sh_int      wield;
};

struct  int_app_type
{
    sh_int      learn;
};

struct  wis_app_type
{
    sh_int      practice;
};

struct  dex_app_type
{
    sh_int      defensive;
};

struct  con_app_type
{
    sh_int      hitp;
    sh_int      shock;
};

struct flow_types
{
  /* AIR EARTH WATER FIRE SPIRT */
  enum _FLOW current[5];
  enum _FLOW perm[5];
};

/* 
 * Country Specific data
 */

struct nation_data
{
  char  *nation;                /* name of nation			*/
  char  *nationid;		/* id as seen in 'PERS'			*/
  bool  selectable;             /* is country selectable at creation?	*/
  struct
  {
    char *city;
    int   attr_bonus[MAX_STATS];  /* Attribute bonus			*/
    char  *skill;                 /* name of skill these people excell at */
    bool  autoskill;              /* do they get this skill for free ? 	*/
    int   bonus;                  /* % bonus to give to above skill	*/
   } cities[MAX_CITIES];
};

/* 
 * Help data
 */
struct  help_data
{
    HELP_DATA * next;
    HELP_DATA * next_area;
    sh_int      level;
    char *      keyword;
    char *      text;
};

/*
 * Shop data
 */
struct  shop_data
{
    SHOP_DATA        *next;                  /* Next shop in list            */
    long             keeper;                 /* Vnum of shop keeper mob      */
    enum _ITEM_TYPE  buy_type [MAX_TRADE];   /* Item types shop will buy     */
    sh_int           profit_buy;             /* Cost multiplier for buying   */
    sh_int           profit_sell;            /* Cost multiplier for selling  */
    sh_int           open_hour;              /* First opening hour           */
    sh_int           close_hour;             /* First closing hour           */
};

/*
 * Character class data
 */
struct  class_type
{
    char *      name;                   /* the full name of the class 	*/
    char *      who_name;		/* Three-letter name for 'who'  */
    enum _STAT  attr_prime;             /* Prime attribute              */
    long        weapon;                 /* First weapon (vnum)          */
    sh_int      skill_adept;            /* Maximum skill level          */
    sh_int      thac0_00;               /* Thac0 for level  0           */
    sh_int      thac0_32;               /* Thac0 for level 32           */
    sh_int      hp_min;                 /* Min hp gained on leveling    */
    sh_int      hp_max;                 /* Max hp gained on leveling    */
    char *      base_group;             /* base skills gained           */
    char *      default_group;          /* default skills gained        */
};


struct item_type
{
    enum _ITEM_TYPE	type;
    char *      	name;
};

struct weapon_type
{
    char *      	name;
    long        	vnum;
    enum _WEAPON_CLASS 	type;
};

struct slay_type
{
  char *        owner;
  char *        title;
  char *        char_msg;
  char *        vict_msg;
  char *        room_msg;
};

struct wiznet_type
{
    char *      name;
    long        flag;
    int         level;
};

struct attack_type
{
    char *        name;                /* name */
    char *        noun;                /* message */
    enum _DAM_TYPE damage;             /* damage class */
};

struct race_type
{
    char *     name;                   /* call name of the race */
    bool       pc_race;                /* can be chosen by pcs */
    SET        act;                    /* act bits for the race */
    SET        aff;                    /* aff bits for the race */
    SET        off;                    /* off bits for the race */
    SET        imm;                    /* imm bits for the race */
    SET        res;                    /* res bits for the race */
    SET        vuln;                   /* vuln bits for the race */
    SET        form;                   /* default form flag for the race */
    SET        parts;                  /* default parts for the race */
};


struct pc_race_type  /* additional data for pc races */
{
    char *      name;                   /* MUST be in race_type 	   */
    char        who_name[8];
    sh_int      points;                 /* cost in points of the race 	   */
    sh_int      class_mult[MAX_CLASS];  /* exp multiplier for class, * 100 */
    char *      skills[5];              /* bonus skills for the race 	   */
    sh_int      stats[MAX_STATS];       /* starting stats 		   */
    sh_int      max_stats[MAX_STATS];   /* maximum stats 		   */
    int         defAlign;		/* Racial "default" alignment	   */
    enum _SIZE  size;                   /* racial stature 	   	   */
};


struct spec_type
{
    char *      name;                   /* special function name */
    SPEC_FUN *  function;               /* the function */
};


/* 
 * Data structure for notes
 */
struct  note_data
{
    NOTE_DATA * next;
    bool        valid;
    sh_int      type;
    char *      sender;
    char *      date;
    char *      to_list;
    char *      subject;
    char *      text;
    time_t      date_stamp;
    time_t      expire;
};

/* 
 * Affect data
 */
struct  affect_data
{
    AFFECT_DATA *        next;
    bool                 valid;
    sh_int               type;
    sh_int               level;
    sh_int               duration;
    sh_int               modifier;
    long                 bitvector;
    long		 owner;
    enum _AFFECT_TARGETS where;
    enum _APPLY_TYPES    location;
};

/*
 * Clan Rank and General data
 */
struct guild_rank
{
  char *rankname;               /* name of rank                         */
  char *skillname;              /* name of skill earned at this rank    */
};

struct clan_type
{
  SET flags;                    /* flags for guild                      */
  char *name;                   /* name of guild                        */
  char *who_name;               /* name sent for "who" command          */
  int room[3];                  /* hall/morgue/temple rooms             */
  RANK_DATA rank[MAX_RANK];     /* guilds rank system                   */
  int ml[4];                    /* mortal leader rights                 */
};

/* 
 * Player status file data (for pstat/gstat)
 */
struct login_data
{
  char          *name;                  /* doh!                         */
  char          *clan;                  /* what clan (if any) are they  */
  time_t        first_login;            /* day/time of creation         */
  time_t        last_login;             /* when did they login          */
  time_t        last_logout;            /* when did they last logout    */
  int           idle;                   /* how long have they been idle */
  time_t        last_levelled_on;       /* what day did they last lvl   */
  time_t        deletion_date;          /* date deleted (if)            */
  char          *last_killed_by;        /* which pc last killed them    */
  char          *last_player_killed;    /* which pc did they last kill  */
  int           total_pkills;           /* #of pc's they have killed    */
  int           total_pkilled;          /* #of times killed by pc's     */
};

/*
 * Data for generating characters 
 * This data structure is freed after character generation, any calls
 * that attemp to access it aftewards will cause a SIGSEGV fault.
 */
struct gen_data
{
    GEN_DATA    *next;
    bool        valid;
    bool        *skill_chosen;
    bool        *group_chosen;
    bool        talent_chosen[MAX_TALENTS];
    int         points_chosen;
    sh_int	rerolls;
};

struct  liq_type
{
    char *      liq_name;
    char *      liq_color;
    sh_int      liq_affect[5];
};

/* 
 * Extra description data for a room or object.
 */
struct  extra_descr_data
{
    EXTRA_DESCR_DATA *next;     /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};

/*
 * Forms structure
 */
struct form_type
{
  char 			*form;
  enum _FORM_TYPE	aType;
  enum _FORM_AFFECTS	fType;
  sh_int		difficulty;
  char			*nation;
  char			*toChar;
  char			*toVict;
  char			*toRoom;
};

/* 
 * Talent structure
 */
struct talent_type
{
  char          *name;
  sh_int        talent_level[MAX_CLASS];
  sh_int        rating[MAX_CLASS];
  SET           flags;
  int           app_bonus[5];
  int           flow_bonus[5];
  sh_int	*pgtn;
};

/* 
 * Skills include weaves as a particular case.
 */
struct  skill_type
{
    char *				name;						/* Name of skill                */
    sh_int				skill_level[MAX_CLASS];		/* Level needed by class        */
    sh_int				rating;						/* How hard it is to learn 1-50 */
    WEAVE_FUN 			*weave_fun;					/* Spell pointer (for spells)   */
    enum _SKILL_TARGETS target;						/* Legal targets                */
    enum _POS_STATES    minimum_position;			/* Position for caster / user   */
    sh_int				beats;						/* Waiting time after use       */
    char *				noun_damage;				/* Damage message               */
    char *				msg_off;					/* Wear off message             */
    char *				msg_obj;					/* Wear off message for obects  */
	sh_int				skill_class[MAX_CLASS];		/* Skill class					*/
	sh_int				class_points[MAX_CLASS];	/* Class points					*/
	sh_int				prereq[10];
    SET					flags;						/* Skill flags                  */
};


struct  group_type
{
    char *      name;
    sh_int      rating[MAX_CLASS];
    char *      spells[MAX_IN_GROUP];
    SET         flags;
};

struct mprog_list
{
    int                 trig_type;
    char *              trig_phrase;
    long                vnum;
    char *              code;
    MPROG_LIST *        next;
    bool                valid;
};

struct mprog_code
{
    long                vnum;
    char *              code;
	char *				author;
	char *				title;
    MPROG_CODE *        next;
};


/* 
 * Structure for a social in the socials table.
 */
struct  social_type
{
    char			name[20];
    char*			char_no_arg;
    char*			others_no_arg;
    char*			char_found;
    char*			others_found;
    char*			vict_found;
    char*			char_not_found;
    char*			char_auto;
    char*			others_auto;
};


/* 
 * Descriptor (channel) structure.
 */
struct	descriptor_data
{
    DESCRIPTOR_DATA*	next;
    DESCRIPTOR_DATA*	snoop_by;
    CHAR_DATA*			character;
    CHAR_DATA*			original;
    bool				fcommand;
    bool				valid;
    char				inbuf		[4 * MAX_INPUT_LENGTH];
    char				incomm		[MAX_INPUT_LENGTH];
    char				inlast		[MAX_INPUT_LENGTH];
    char*				host;
    char*				outbuf;
    char*				showstr_head;
    char*				showstr_point;
    char*				username;
    char**				pString;	
    unsigned char*		out_compress_buf;
    z_stream*			out_compress;
    void*				pEdit;		
    int					descriptor;
    int					repeat;
    int					outsize;
    int					outtop;
    int					editor;		
    enum _CON_STATES	connected;

    struct
    {
        char*			filename;   /* Filename being written to */
        char*			data;       /* Data being written 	 */
        short int 		lines_left; /* Lines left 		 */
        short int 		lines_sent; /* Lines sent so far 	 */
        ftp_mode 		mode;       /* FTP_xxx 		 	 */
    } ftp;
};

/*
 * Web Descriptor structure
 */
struct web_descriptor 
{
    WEB_DESCRIPTOR*		next;	
    int					fd;
    int					sin_size;
    char				request[2048];
    bool				valid;
    struct				sockaddr_in their_addr;
};


/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct	mob_index_data
{
    MOB_INDEX_DATA*		next;
    SPEC_FUN*			spec_fun;
    SHOP_DATA*			pShop;
    MPROG_LIST*			mprogs;
    AREA_DATA*			area;		
    SOUND_DATA*			sounds;		
    long				vnum;
    long				wealth;
    long				mprog_flags;
    char*				player_name;
    char*				short_descr;
    char*				long_descr;
    char*				description;
    char*				material;
    sh_int				group;
    sh_int				count;
    sh_int				killed;
    sh_int				alignment;
    sh_int				level;
    sh_int				hitroll;
    sh_int				hit[3];
    sh_int				OPR_dice[3];
    sh_int				damage[3];
    sh_int				start_pos;
    sh_int				default_pos;
    sh_int				race;
    enum _DAM_TYPE		dam_type;
    enum _AC_TYPES		ac[4];
    enum _SEX			sex;
    enum _SIZE			size;

    struct
    {
       SET				comm;
       SET				config;
       SET				player;
       SET				penalties;
       SET              wiznet;
       SET				affected_by;
       SET				form;
       SET				parts;
       SET				off;
       SET				imm;
       SET				res;
       SET				vuln;
    } states;
};

/*
 * Generic (PC/NPC) structure data
 */
struct	char_data
{
    CHAR_DATA*			next;
    CHAR_DATA*			next_in_room;
    CHAR_DATA*			master;
    CHAR_DATA*			leader;
    CHAR_DATA*			fighting;
    CHAR_DATA*			mprog_target;
    CHAR_DATA*			mount;			
    CHAR_DATA*			reply; 				
    CHAR_DATA*			pet;				
    GEN_DATA*			gen_data;			
    SPEC_FUN*			spec_fun;
    MOB_INDEX_DATA*		pIndexData;
    DESCRIPTOR_DATA*	desc;
    AFFECT_DATA*		affected;
    OBJ_DATA*			carrying;
    OBJ_DATA*			on;
    ROOM_INDEX_DATA*	in_room;
    ROOM_INDEX_DATA*	was_in_room;
    AREA_DATA*			zone;
    PC_DATA*			pcdata;
    SOUND_DATA*			sounds;
    bool				valid;
    bool				riding;
    char*				name;
    char*				short_descr;
    char*				long_descr;
    char*				description;
    char*				prompt;				
    char*				prefix;				
    char*				material;	
    long				exp;				
    sh_int				practice;			
    sh_int				train;				
    long				vstable;       		
    long				age;				
	long				vGhostRoom;
    sh_int				invis_level;		
    sh_int				incog_level;		
    sh_int				trust;				
    int					played;				
    int					lines;  			
    time_t				logon;				

    int					wtimer;
    sh_int				version;
    sh_int				group;				
    sh_int				clan;				
    sh_int				rank;
    sh_int				c_class;	
    sh_int				race;
    sh_int				level;
    sh_int				timer;				
    sh_int				wait;		
    sh_int				daze;
    sh_int				hit;
    sh_int				max_hit;
    sh_int				move;
    sh_int				max_move;
    sh_int				carry_weight;
    sh_int				carry_number;
    sh_int				saving_throw;			
    sh_int				alignment;
    sh_int				hitroll;
    sh_int				damroll;
    sh_int				armor[4];
    sh_int				wimpy;
    sh_int				damage[3];
    sh_int				start_pos;
    sh_int				default_pos;
    sh_int				mprog_delay;
	sh_int				weather_event; //1 if caught in event 0 if not
    long				attacked_from;        /* vnum of room attacked from  */
    long				from_room;            /* vnum of last room ch was in */
    long				gold;	
    long				silver;
	long				linked_to;

    _flow				flow_str;             /* current, permanent values   */

    enum _STAT			perm_stat[MAX_STATS];
    enum _STAT			mod_stat[MAX_STATS];
	enum _ABILITY       ability[MAX_ABILITY];
	enum _IMPEDIMENT    impediment[MAX_IMPEDIMENT];
    enum _SEX			sex;
    enum _POS_STATES    position;
    enum _SIZE			size;
    enum _DAM_TYPE		dam_type;

    struct
    {
       SET				comm;
       SET				config;
       SET				player;
       SET				penalties;
       SET				wiznet;
       SET				affected_by;
       SET				form;
       SET				parts;
       SET				off;
       SET				imm;
       SET				res;
       SET				vuln;
    } states;
};


/* 
 * Data which only PC's have.
 */
struct	pc_data
{
    PC_DATA*			next;
    BUFFER*				buffer;
    OBJ_DATA*			box;
    BOARD_DATA*			board;			/* The current board 	*/
    NOTE_DATA*			in_progress;
    bool				valid;
    bool*				group_known;
    bool              	confirm_delete;
    bool            	boxrented;
    char*				pwd;
    char*				bamfin;
    char*				bamfout;
    char*				title;
	char*				osdesc;
    char*				nation;			/* where are ya from?       */
    char*				city;			/* ^^			    */
    char*				hidden_as;
    char*				alias	 [MAX_ALIAS];
    char* 				alias_sub[MAX_ALIAS];
    char*				ignore	 [MAX_IGNORE]; 	/* ignoring anyone?	    */
    char*				todo	 [MAX_TODO];
    char*				r_desc;
	char*				profile;
    sh_int				perm_hit;
    sh_int				perm_move;
    sh_int*				learned;
    sh_int				talents[MAX_TALENTS];
    sh_int				points;
    sh_int          	item_box;
    int					quest_points;
    int					last_level;
    int 				security;		/* Builder security         */
    int					peaceful;		/* is character peaceful?   */
    int					insanity;		/* level of plrs insanity   */
	int					iMemSize[2];    /* Keeps track of maxName variable sizes */
    int*				known;
    int*				bonds;			/* all bonds a player has   */
    int					id;			/* */
    long     			balance;		/* banking economy system   */
    long     			shares;
    long                bounty;         	/* for bounty code	    */
    long     			rentroom;		/* for inn rentals   	    */
    long				recall_to;		/* Personal recall room     */
    long				last_auction_id;
	float				notoriety[2];
    time_t				last_note[MAX_BOARD];	/* last note for the boards */
    enum _CONDITION		condition	[4];
    enum _SEX			true_sex;
	int					PracticesGiven;

	// AoS
	char* shipname;         // Name of Captain's ship
	int fame;				// Level of Captain's fame
	int reputation;			// Level of Captain's reputation
	int morale;				// Morale of ship's crew
	int background;			// Background of Captain (Fisherman, Navy, etc)
	// End AoS Variables
};

/*
 * Prototype for an object.
 */
struct	obj_index_data
{
    OBJ_INDEX_DATA*		next;
    EXTRA_DESCR_DATA*	extra_descr;
    AFFECT_DATA*		affected;
    AREA_DATA*			area;		
    SOUND_DATA*			sounds;		
    char*				name;
    char*				short_descr;
    char*				description;
    int					vnum;
    sh_int				reset_num;
    char*				material;
    long				extra_flags;
    long				wear_flags;
    sh_int				level;
    sh_int				condition;
    sh_int				max_condition;
    sh_int				count;
    sh_int				weight;
    int					cost;
    int					value[5];
    enum _ITEM_TYPE		item_type;
};



/*
 * One object.
 */
struct	obj_data
{
    OBJ_DATA*			next;
    OBJ_DATA*			next_content;
    OBJ_DATA*			contains;
    OBJ_DATA*			in_obj;
    OBJ_DATA*			on;
    CHAR_DATA*			carried_by;
    EXTRA_DESCR_DATA*	extra_descr;
    AFFECT_DATA*		affected;
    OBJ_INDEX_DATA*		pIndexData;
    ROOM_INDEX_DATA*	in_room;
    SOUND_DATA*			sounds;
    bool				valid;
    bool				enchanted;
    char*				owner;
    
    char*				short_descr;
    char*				description;
    long				extra_flags;
    long				wear_flags;
    sh_int				weight;
    int					cost;
    sh_int				level;
	sh_int				hood;
    sh_int 				condition;
    sh_int				max_condition;
    char*				material;
    sh_int				timer;
    int					value	[5];
    enum _WEAR_LOC		wear_loc;
    enum _ITEM_TYPE		item_type;


	// New ToS Crew Fields
	int melee;
	int cannon;
	int	sealegs;
	int	knowledge;
	int	discipline;
	char* name;
	enum _CREW_TYPE	crew_type;
	// End ToS crew
	
};



/*
 * Exit data.
 */
struct	exit_data
{
    union
    {
		ROOM_INDEX_DATA*to_room;
		long			vnum;
    } u1;

    char*				keyword;
    char*				description;
    long				rs_flags;	
    long				key;
    int					exit_info;
    int					orig_door;	
    EXIT_DATA*			next;		
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct	reset_data
{
    RESET_DATA*			next;
    char				command;
    long				arg1;
    long				arg2;
    long				arg3;
    long				arg4;
    sh_int				chance;
};


/*
 * Area definition.
 */
struct	area_data
{
    AREA_DATA*			next;
    RESET_DATA*			reset_first;
    RESET_DATA*			reset_last;
    SOUND_DATA*			sounds;
    long 				min_vnum;
    long				max_vnum;
    long				vnum;		
    bool				empty;
    char*				file_name;
    char*				name;
    char*				credits;
    char*				builders;	
    char*				last_mod;   
    sh_int				age;
    sh_int				nplayer;
    sh_int				low_range;
    sh_int				high_range;
    sh_int				security;	
    sh_int				version;	
    int					area_flags;	
    enum _SECTOR_TYPES	sector_type;        
};



/*
 * Room type.
 */
struct	room_index_data
{
    ROOM_INDEX_DATA*	next;
    CHAR_DATA*			people;
    OBJ_DATA*			contents;
    EXTRA_DESCR_DATA*	extra_descr;
    AREA_DATA*			area;
    EXIT_DATA*			exit	[6];
    RESET_DATA*			reset_first;
    RESET_DATA*			reset_last;	
    SOUND_DATA*			sounds;		
    char*				name;
    char*				description;
    char*				owner;
    int					room_flags;
    long				vnum;
    sh_int				light;
    sh_int				heal_rate;
    sh_int 				opu_rate;
    sh_int				clan;
    sh_int				c_class;
    enum _SECTOR_TYPES	sector_type;
};


#include "protos.h"	/* Function prototypes		*/


/****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY	30

/* 
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    A		/* Area has been modified.           */
#define         AREA_ADDED      B		/* Area has been added to.           */
#define         AREA_LOADING    C		/* Used for counting in db.c         */
#define			AREA_DELETE		D       /* remove area from area.lst 	     */
#define 		AREA_UNUSED		E       /* dont show area in 'areas' 	     */
#define  		AREA_CLOSED		F		/* area is not available for playing */

#define MAX_DIR	6
#define NO_FLAG -99	/* Must not be used in flags or stats. */

/*
 * Global variables
 */
extern          AREA_DATA		*area_first;
extern          AREA_DATA		*area_last;
extern			AREA_DATA		*current_area;
extern  		SHOP_DATA		*shop_last;
extern			NAME_DATA		*name_first;
extern			NAME_DATA 		*name_last;
extern			HELP_DATA		*help_first;
extern			HELP_DATA		*help_last;
extern			SHOP_DATA		*shop_first;

extern          int             top_affect;
extern          int             top_area;
extern          int             top_ed;
extern          int             top_exit;
extern          int             top_help;
extern          int             top_mob_index;
extern          int             top_obj_index;
extern          int             top_reset;
extern          int             top_room;
extern          int             top_shop;
extern			int				top_weave;
extern			int				top_name;
extern			int				top_sound;

extern			long            top_vnum_mob;
extern			long            top_vnum_obj;
extern			long            top_vnum_room;
extern			int				top_web_desc;
extern			char            str_empty       [1];
extern			char			strArea[MAX_INPUT_LENGTH];
extern 			bool 			fBootDb;

extern			int				nAllocPerm;
extern			int				sAllocPerm;
extern			int				nAllocString;
extern			int				sAllocString;
extern			int				mobile_count;

extern	MOB_INDEX_DATA	*		mob_index_hash  [MAX_KEY_HASH];
extern  OBJ_INDEX_DATA	*		obj_index_hash  [MAX_KEY_HASH];
extern  ROOM_INDEX_DATA	*		room_index_hash [MAX_KEY_HASH];
extern  SKILL_DATA		*		skill_index_hash[MAX_KEY_HASH];


/*
 * flag tables 
 */
extern	const	struct	flag_type	extra_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	room_flags[];
extern	const	struct	flag_type	exit_flags[];
extern 	const	struct  flag_type	mprog_flags[];
extern	const	struct	flag_type	area_flags[];
extern	const	struct	flag_type	sector_flags[];
extern	const	struct	flag_type	door_resets[];
extern	const	struct	flag_type	wear_loc_strings[];
extern	const	struct	flag_type	wear_loc_flags[];
extern	const	struct	flag_type	res_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	vuln_flags[];
extern	const	struct	flag_type	type_flags[];
extern	const	struct	flag_type	apply_flags[];
extern	const	struct	flag_type	sex_flags[];
extern	const	struct	flag_type	furniture_flags[];
extern	const	struct	flag_type	weapon_class[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	weapon_type2[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	size_flags[];
extern	const	struct	flag_type	position_flags[];
extern	const	struct	flag_type	ac_type[];
extern	const	struct	bit_type	bitvector_type[];
extern  const   struct  flag_type	guild_flags[];
extern 	const	struct	flag_type	group_flags[];
extern  const 	struct	flag_type	skill_flags[];

extern	const	struct	flag_type	sound_type_table[];
extern	const	struct	flag_type	system_sound_table[];
extern  const	struct	flag_type	area_sound_table[];
extern	const	struct	flag_type	mob_sound_table[];
extern	const	struct	flag_type	room_sound_table[];
extern	const	struct	flag_type	obj_sound_table[];
extern	const	struct	flag_type	imm_sound_table[];

extern 	const 	struct 	flag_type 	area_flags[];
extern 	const 	struct 	flag_type	sex_flags[];
extern 	const 	struct 	flag_type	exit_flags[];
extern 	const 	struct 	flag_type	door_resets[];
extern 	const 	struct 	flag_type	room_flags[];
extern 	const 	struct 	flag_type	sector_flags[];
extern 	const 	struct 	flag_type	type_flags[];
extern 	const 	struct 	flag_type	extra_flags[];
extern 	const 	struct 	flag_type	wear_flags[];
		
extern 	const 	struct 	flag_type	guild_flags[];
extern 	const 	struct 	flag_type	group_flags[];
extern 	const 	struct 	flag_type	skill_flags[];
extern 	const 	struct 	flag_type	room_aff_flags[];
extern 	const 	struct 	flag_type	target_flags[];
extern 	const 	struct 	flag_type	mud_flags[];
extern	const	struct	flag_type	cmd_flags[];
extern			struct	gsn_type	gsn_table[];

/* New format bits */
extern	const	struct	flag_type	_config_bits[];
extern	const	struct	flag_type	_act_bits[];
extern	const	struct	flag_type	_penalty_bits[];
extern	const	struct	flag_type	_comm_bits[];
extern	const	struct	flag_type	_wiznet_bits[];
extern	const	struct	flag_type	_aff_bits[];
extern	const	struct	flag_type	_form_bits[];
extern	const	struct	flag_type	_part_bits[];
extern	const	struct	flag_type	_affected_by_bits[];
extern	const	struct	flag_type	_resistance_bits[];
extern	const	struct	flag_type	_off_bits[];
extern	const	struct	flag_type	_system_bits[];

#ifdef __cplusplus
  }
#endif


#endif
