
/** 
 * Bit definitions
 */
#define UL_BITS				32
#define MAXELEMS			64
#define NOT_SET				MAXELEMS+1
	
typedef unsigned long  			SET[MAXELEMS/UL_BITS];


/**
 * Configuration bits 
 * Used in char_data for configuration settings
 */
#define _ANSI				1
#define _COMPACT			2
#define _BRIEF				3
#define _PROMPT				4
#define _COMBINE			5
#define _TELNET_GA			6
#define _SHOW_AFFECTS		7
#define _SHOW_EMPTY			8
#define _SOUNDON			9
#define _ECHO				10
#define _NOWEATHER			12
#define _NOHUNGER			13
#define _QUIET             	14
#define _DEAF            	15
#define _BUILDING			16
#define _SNOOP_PROOF		17
#define _AFK				18
#define _AUTOASSIST			19
#define _AUTOEXIT			20
#define _AUTOLOOT			21
#define _AUTOSAC			22
#define _AUTOGOLD			23
#define _AUTOSPLIT			24
#define _MUDFTP				25
#define _NOFOLLOW			26
#define _HOLYLIGHT			27
#define _COMPRESSION		28
#define _SHOW_DAMAGE		29
#define _CANLOOT			30
#define _PKQUESTOR			31
#define _RPQUESTOR			32
#define _SHOWGUILD			33
#define _WHO_LEVEL			34
#define _WHO_WHERE			35
#define _WHO_SEX				36

/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define _ACT_NPC				1	/* Auto set for mobs	*/
#define _ACT_CHANNELER			2
#define _ACT_ROGUE				3
#define _ACT_WARRIOR			4
#define _ACT_SENTINEL	    	5	/* Stays in one room	*/
#define _ACT_SCAVENGER	    	6	/* Picks up objects	*/
#define _ACT_MOUNT             	7
#define _ACT_AGGRESSIVE			8    	/* Attacks PC's		*/
#define _ACT_STAY_AREA			9       /* Won't leave area	*/
#define _ACT_WIMPY				10
#define _ACT_PET				11	/* Auto set for pets	*/
#define _ACT_TRAIN				12	/* Can train PC's	*/
#define _ACT_PRACTICE			13	/* Can practice PC's	*/
#define _ACT_NOCHANNELLING    	14
#define _ACT_STABLEHAND			15
#define _ACT_NOALIGN			16
#define _ACT_NOPURGE			17
#define _ACT_OUTDOORS			18
#define _ACT_INDOORS			19
#define _ACT_HEALER				20
#define _ACT_GAIN				21
#define _ACT_UPDATE_ALWAYS		22
#define _ACT_BANKER				23
#define _ACT_MORTAL_LEADER		24
#define _ACT_BLADEMASTER		25
#define _ACT_WOLFBROTHER		26
#define _ACT_INSANE				27
#define _ACT_TENNANT			28
#define _ACT_RESTRINGER			29
#define _ACT_ASSIST_ALL     	30
#define _ACT_ASSIST_ALIGN	    31
#define _ACT_ASSIST_RACE       	32
#define _ACT_ASSIST_PLAYERS    	33
#define _ACT_ASSIST_GUARD      	34
#define _ACT_ASSIST_VNUM		35
#define _ACT_ASSASSIN			36
#define _ACT_UNKILLABLE			37
#define _ACT_PERSISTANT			38

/**
 * Generic Flags
 * (Only applicable to Guild/Talent/Weave/Group flags)
 */
#define _CHANGED			1
#define _DELETED			2


/** 
 * Guild flags
 */
#define _INDEPENDENT 			3 /* a "loner" guild */
#define _WOLF				4 
#define _IMMORTAL			5 /* immortal only clan */
#define _GAIDIN				6
#define _AIEL				7
#define _SEDAI				8
#define _WHITECLOAK        		9
#define _THIEF_TAKER       		10
#define _HAILENE			11
#define _WANSHO				12
#define _TINKER				13
#define _SHADAR				14
#define _COMPANIONS			15
#define _ILLUMINATOR			16
#define _GLEEMAN			17
#define _ASHAMAN			18
#define _REDHAND			19
#define _HUNTER				20
#define _BLIGHT_WATCH			21
#define _QUEENS_GUARD			22
#define _YOUNGLINGS			23
#define _KIN				24
#define _SALIDAR			25
#define _2RIVERS_COMPANIONS		26
#define _CHA_FAILE			27
#define _SEAFOLK			28
#define _SHADOW				29
#define _TOWERGUARD			30
#define _DRAGONSWORN		31
#define _ANY				32

/**
 * Talent/Weave/Group flags 
 */
#define _IS_TALENT				3
#define _IS_SPECIALTY			4
#define _GROUP_ONLY            	5
#define _GUILD_ANY				6
#define _NO_CHARGE				7
#define _REQ_CHANNEL           	8
#define	_AFF_FLOW				9 
#define _AFF_END				10
#define _WEAVE_SPECIALTY		11
#define _COMBAT_SPECIALTY		12
#define _REQ_SPEC_AIR			13
#define _REQ_SPEC_EARTH			14
#define _REQ_SPEC_WATER			15
#define _REQ_SPEC_FIRE			16
#define _REQ_SPEC_SPIRIT		17
#define	_REQ_TALENT_DREAMING	18
#define _REQ_TALENT_TRAVELLING	19
#define _REQ_TALENT_HEALING		20
#define _REQ_TALENT_SINGING     21
#define _REQ_RACE_HUMAN			22
#define _REQ_RACE_OGIER			23
#define _REQ_RACE_TROLLOC		24
#define _REQ_RACE_FADE			25
#define _REQ_RACE_DRAGHKAR		26
#define _NOT_GAINABLE			27
#define _REQ_ROGUE	28
#define _REQ_WARRIOR	29
#define _REQ_CHANNELER	30


/**
 * Penalty bits
 */
#define _PERMIT             	1
#define _LOG					2
#define _DENY					3
#define _FREEZE					4
#define _NOWIZ              	5
#define _NOAUCTION          	6
#define _NOGOSSIP           	7
#define _NOQUESTION         	8
#define _NOCLAN					9
#define _NOQUOTE				10
#define _NOOOC              	11
#define _NOEMOTE				12
#define _NOSHOUT				13
#define _NOTELL					14
#define _NOCHANNELS				15
#define _NOBOND             	16
#define _NOGRATS				17
#define _NOGUILD				18
#define _NOPROMOTE				19
#define _NOPK					20
#define _NOLOOT					21
#define _NOFATALS				22
#define _NORECALL				23
#define _NOBONDING				24

/**
 * Communication bits
 */
#define _OOC					1
#define _SHOUTS					2
#define _TELLS					3
#define _GRATS					4
#define _QUESTION				5
#define _ANSWER					6
#define _GUILD					7
#define _QUOTES					8
#define _AUCTION				9
#define _BOND					10
#define _MINION					11
#define _GOSSIP					12
#define _QUOTE					13
#define _WIZ					14
#define _TOWER_TALK				15

/** 
 * Wiznet bits
 */
#define	WIZ_ON					1
#define WIZ_TICKS				2
#define WIZ_LOGINS				3
#define WIZ_SITES				4
#define WIZ_LINKS				5
#define WIZ_DEATHS				6
#define WIZ_RESETS				7
#define WIZ_MOBDEATHS			8
#define WIZ_FLAGS				9
#define WIZ_PENALTIES			10
#define WIZ_SACCING				11
#define WIZ_LEVELS				12
#define WIZ_SECURE				13
#define WIZ_SWITCHES			14
#define WIZ_SNOOPS				15
#define WIZ_RESTORE				16
#define WIZ_LOAD				17
#define WIZ_NEWBIE				18
#define WIZ_PREFIX				19
#define WIZ_SPAM				20
#define WIZ_BUGS				21
#define	WIZ_TRACE				22
#define WIZ_RESTRING			23

/**
 * Form bits
 */

/* Body form */
#define _EDIBLE             	1
#define _POISONOUS             	2
#define _MAGICAL            	3
#define _INSTANT_DECAY      	4
 
/* actual form */
#define _ANIMAL             	5
#define _SENTIENT           	6
#define _CONSTRUCT          	7
#define _MIST               	8  /* Kept only for Mashadar 	*/
#define _INTANGIBLE         	9  /* Keep for use in TAR?	*/
 
#define _BIPED              	10
#define _INSECT             	11
#define _SPIDER             	12
#define _CRUSTACEAN         	13
#define _WORM               	14
#define _MAMMAL             	15
#define _BIRD               	16
#define _REPTILE            	17
#define _SNAKE              	18
#define _DRAGON             	19
#define _AMPHIBIAN          	20
#define _FISH               	21
#define _COLD_BLOOD				22	
#define _HORSE					23
#define _OTHER					24
 
/* body parts */
#define _HEAD               	1
#define _ARMS               	2
#define _LEGS               	3
#define _HEART              	4
#define _BRAINS             	5
#define _GUTS               	6
#define _HANDS              	7
#define _FEET               	8
#define _FINGERS            	9
#define _EAR                	10
#define _EYE					11
#define _LONG_TONGUE        	12
#define _FINS               	13
#define _WINGS              	14
#define _TAIL               	15
/* for combat */
#define _CLAWS              	16
#define _FANGS              	17
#define _HORNS              	18
#define _SCALES             	19
#define _TUSKS					20
#define _HAIR					21

/**
 * Bits for 'affected_by'.
 * Kept their AFF prefix to avoid conflicts
 */
#define _AFF_BLIND				1
#define _AFF_INVISIBLE			2
#define _AFF_DETECT_EVIL		3
#define _AFF_DETECT_INVIS		4
#define _AFF_DETECT_WEAVE		5
#define _AFF_DETECT_HIDDEN		6
#define _AFF_DETECT_GOOD		7
#define _AFF_INFRARED			9
#define _AFF_SENSE_EVIL			10	/* Gaidin/Aes Sedai	*/
#define _AFF_POISON				11
#define _AFF_PROTECT_EVIL		12
#define _AFF_PROTECT_GOOD		13
#define _AFF_SNEAK				14
#define _AFF_HIDE				15
#define _AFF_SLEEP				16
#define _AFF_COMPULSION			17
#define _AFF_FLYING				18	
#define _AFF_HASTE				19
#define _AFF_CALM				20
#define _AFF_PLAGUE				21
#define _AFF_WEAKEN				22
#define _AFF_DARK_VISION		23	/* Sei'car		*/
#define _AFF_BERSERK			24
#define _AFF_SWIM				25
#define _AFF_REGENERATION      	26
#define _AFF_SLOW				27
#define _AFF_EMBRACED			28
#define _AFF_CAPTURED			29
#define _AFF_BOUND_AIR			30
#define _AFF_BLIND_AIR			31
#define _AFF_GAG_AIR			32
#define _AFF_FERAL				33	/* Sei'car		*/
#define _AFF_WARFARE			34	/* Blightwatch		*/
#define _AFF_MARTYRDOM			35	/* Blightwatch		*/
#define _AFF_CLOAKED			36	/* Gaidin		*/
#define _AFF_AIR_SHIELD			37
#define _AFF_EARTH_SHIELD		38
#define _AFF_FIRE_SHIELD		39
#define _AFF_SPIRIT_SHIELD		40
#define _AFF_WATER_SHIELD		41
#define _AFF_FROST_SHIELD		42
#define _AFF_PASS_DOOR			43
#define _AFF_PAINED				44
#define _AFF_DREAMING			45
#define _AFF_SHIELDING			46
#define _AFF_DEAF				47
#define _AFF_MUTE				48
#define _AFF_WOUNDED			49
#define _AFF_LACERATED			50
// Taos affects added
#define _AFF_COLD				51
#define _AFF_HYPOTHERMIA		52

/** 
 * Bits for imm/res/vuln
 */
#define _COMPULSION					1
#define _ONEPOWER					2
#define _WEAPON						3
#define _BASH						4
#define _PIERCE						5
#define _SLASH						6
#define _FIRE						7
#define _COLD						8
#define _LIGHTNING					9
#define _ACID						10
#define _POISON						11
#define _NEGATIVE					12
#define _GOOD						13
#define _ENERGY						14
#define _MENTAL						15
#define _DISEASE					16
#define _DROWNING					17
#define _LIGHT						18
#define _PUSH						19 
#define _DISARM						20

/**
 * Bits for Offensive states
 * Kept their OFF prefix to avoid conflicts
 */
#define _OFF_AREA_ATTACK         	1
#define _OFF_BACKSTAB            	2
#define _OFF_BASH                	3
#define _OFF_BERSERK             	4
#define _OFF_DISARM              	5
#define _OFF_DODGE               	6
#define _OFF_FADE                	7
#define _OFF_FAST                	8
#define _OFF_KICK                	9
#define _OFF_KICK_DIRT           	10
#define _OFF_PARRY               	11
#define _OFF_RESCUE              	12
#define _OFF_TAIL                	13
#define _OFF_TRIP                	14
#define _OFF_CRUSH					15
 
 /**
 * Extra flags.
 * Used in #OBJECTS.
 */
#define _GLOW						1
#define _HUM						2
#define _LOCK						3
#define _EVIL						4
#define _INVIS						5
#define _WEAVES						6
#define _NODROP						7
#define _ANTI_GOOD					8
#define _ANTI_EVIL					9
#define _NOREMOVE					10
#define _INVENTORY					11
#define _NOPURGE					12
#define _ROT_DEATH					13
#define _LODGED						14
#define _VIS_DEATH					15
#define _NONMETAL					16
#define _NOLOCATE					17
#define _MELT_DROP					18
#define _HAD_TIMER					19
#define _SELL_EXTRACT				20
#define _BURN_PROOF					21
#define _QUEST						22 /* Item is a quest prize.	     */
#define _NOBREAK					23 /* Item never breaks     	     */
#define _HIDDEN						24 /* Item is reset as 'hidden'	     */
#define _NOCOUNT					25 /* Item is not counted in maxitems*/
#define _NOSAVE						26 /* Item is not saved to pfile     */
#define _NOSAC						27 /* Item cannot be sacrificed	     */

/**
 * Wear bits
 * Used in #OBJECTS.
 */	
#define _TAKE						1
#define _WEAR_FINGER				2
#define _WEAR_NECK					3
#define _WEAR_BODY					4
#define _WEAR_HEAD					5
#define _WEAR_LEGS					6
#define _WEAR_FEET					7
#define _WEAR_HANDS					8
#define _WEAR_ARMS					9
#define _WEAR_SHIELD				10
#define _WEAR_ABOUT					11
#define _WEAR_WAIST					12
#define _WEAR_WRIST					13
#define _WIELD						14
#define _HOLD						15
#define _WEAR_FLOAT					17
#define _WEAR_SCABBARD				18
#define _WEAR_TATTOO				19
#define _WEAR_FACE					20
#define _WEAR_EARRING				21
#define _WEAR_SHOULDER				22
#define _WEAR_LODGED				23

/** 
 * Weapon bits
 */
#define _WEAPON_FLAMING				1
#define _WEAPON_AIR					2
#define _WEAPON_SHARP				3
#define _WEAPON_TWO_HANDS			4
#define _WEAPON_POISON				5

/**
 * Gate bits 
 */
#define _GATE_NORMAL_EXIT			1
#define _GATE_GOWITH				2
#define _GATE_BUGGY					3
#define _GATE_RANDOM				4

/**
 * Furniture flags 
 */
#define _STAND_AT					1
#define _STAND_ON					2
#define _STAND_IN					3
#define _SIT_AT						4
#define _SIT_ON						5
#define _SIT_IN						6
#define _REST_AT					7
#define _REST_ON					8	
#define _REST_IN					9
#define _SLEEP_AT					10
#define _SLEEP_ON					11
#define _SLEEP_IN					12
#define _PUT_AT						13
#define _PUT_ON						14
#define _PUT_IN						15
#define _PUT_INSIDE					16
#define _IS_PLACE					17

/**
 * Room flags.
 * Used in #ROOMS.
 */
#define _DARK						1
#define _NO_GATE            		2
#define _NO_MOB						3
#define _INDOORS					4
#define _PRIVATE					5
#define _SAFE						6
#define _SOLITARY					7
#define _PET_SHOP					8
#define _NO_RECALL					9
#define _IMP_ONLY					10
#define _IMM_ONLY					11
#define _NEWBIES_ONLY				12
#define _LAW						13
#define _NOWHERE					14
#define _RENTAL						15
#define _RENTED						16
#define _ARENA              		17
#define _NOMAGIC					18
#define _STEDDING					19
#define _MEN_ONLY					20
#define _WOMEN_ONLY					21
#define _MOUNT_SHOP         		22
#define _DEPOSITORY         		23
#define _STABLE						24

/**
 * Exit flags.
 * Used in #ROOMS.
 */
#define _ISDOOR	      				1
#define _CLOSED	      				2
#define _LOCKED	      				3
#define _SECRET	      				4
#define	_RANDOM	      				5
#define _PICKPROOF	      			6
#define _NOPASS	      				7
#define _EASY						8
#define _HARD						9
#define _INFURIATING	      		10
#define _NOCLOSE	      			11
#define _NOLOCK	      				12

