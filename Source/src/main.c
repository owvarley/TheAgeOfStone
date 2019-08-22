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
 * @@ Objective (main.c)
 * 
 * This file contains the primary functions and support code run during
 * the bootup process of the mud. Also, it contains memory and fread_
 * functions used throughout the mud. (With the exception of recycle.c)
 * Finally, main.c contains the global declaration of variables declared
 * as external in merc.h as well as some OS dependent defines.
 */


#define _GNU_SOURCE

#define IN_MAIN_C

#if !defined(WIN32)
  #include <sys/time.h>
  #include <unistd.h>
  #include <sys/resource.h>
#else
  #include <process.h>
  #include <sys/timeb.h>
#endif

#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>

#include "merc.h"
#include "recycle.h"
#include "gsns.h"
#undef IN_MAIN_C


#define MAGIC_NUM 52571214	/* Magic number for memory allocation */

struct social_type *social_table;
struct race_type *race_table;
struct pc_race_type *pc_race_table;
struct clan_type *clan_table;
struct group_type *group_table;
struct form_type *form_table;
struct cmd_type *cmd_table;
struct skill_type *skill_table;

struct liq_type liq_table[MAX_LIQUIDS];
struct class_type class_table[MAX_CLASS];
struct slay_type slay_table[MAX_SLAY_TYPES];
struct maneuver_type maneuver_table[MAX_MANEUVERS];

extern OBJ_DATA *obj_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern PC_DATA *pcdata_free;
extern AFFECT_DATA *affect_free;
extern NAME_DATA *name_free;
extern AREA_DATA *area_first;
extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];

extern int newmobs;
extern int newobjs;
extern int top_mob_index;
extern int top_obj_index;
extern int top_affect;
extern int top_ed;

MUD pMud;

FILE *fpErr = NULL;
FILE *fpChar = NULL;

NATION_DATA nation_table[MAX_NATIONS];
KILL_DATA kill_table[MAX_LEVEL];
TIME_INFO_DATA time_info;
WEATHER_DATA weather_info;
AUCTION_DATA *house_auctions;
HELP_DATA *help_first;
HELP_DATA *help_last;
SHOP_DATA *shop_first;
SHOP_DATA *shop_last;
NOTE_DATA *note_free;
MPROG_CODE *mprog_list;
UPDATE_DATA *motd_list = NULL;
UPDATE_DATA *imotd_list = NULL;
UPDATE_DATA *jobs_list = NULL;
CHAR_DATA *char_list;
OBJ_DATA *object_list;
AFFECT_DATA *obj_aff_list;
AFFECT_DATA *room_aff_list;
AFFECT_DATA *char_aff_list;
AFFECT_DATA *exit_aff_list;

AREA_DATA *area_first;
AREA_DATA *area_last;
AREA_DATA *current_area;
NAME_DATA *name_first;
NAME_DATA *name_last;
DESCRIPTOR_DATA *descriptor_list;	/* All open descriptors         */
DESCRIPTOR_DATA *d_next;	/* Next descriptor in loop      */

MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
SKILL_DATA *skill_index_hash[MAX_KEY_HASH];

char str_empty[1];
char str_boot_time[MIL];
char strArea[MIL];
char *string_hash[MAX_KEY_HASH];
char *help_greeting;
char *string_space;
char *top_string;

jmp_buf jump_env;
time_t current_time;		/* time of this pulse          */

bool god;					/* All new chars are gods!     */
bool merc_down;				/* Shutdown                    */
bool MOBtrigger = TRUE;		/* act() switch                */
bool AttemptJump = FALSE;
bool webUP = FALSE;
bool fBootDb;

int num_quotes;
int sig_loops = 0;
int maxSkill;
int maxClan;
int maxSocial;
int maxRace;
int maxPCRace;
int maxGroup;
int maxForm;
int maxCommands;

int port = 4000;

int nAllocString;
int sAllocString;

int nAllocPerm;
int sAllocPerm;

int top_affect;
int top_area;
int top_ed;
int top_exit;
int top_help;
int top_mob_index;
int top_obj_index;
int top_reset;
int top_room;
int top_shop;
long top_vnum_room;
long top_vnum_mob;
long top_vnum_obj;
int top_name;
int top_sound;
int top_mprog_index;
int top_web_desc;
int mobile_count = 0;
int top_auction;

void *rgFreeList[MAX_MEM_LIST];
const int rgSizeList[MAX_MEM_LIST] = {
	16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536 - 64
};


/*
 * Semi-locals.
 */
bool fBootDb;
FILE *fpArea;



/*
 * abort() does not flush streams, and exit() cant be relied on to 
 * flush everything for some reason...
 * So, flush and close all of our important files before exiting.
 */
void _quit(int iError)
{

  // flush the log file
  if ( fpErr ) 
  {
	  fflush(fpErr);
	  fclose(fpErr);
  }

  if ( stderr )
  {
    fflush(stderr);
    fclose(stderr);
  }


  if ( fpArea ) // flush any open area file
  {
	  fflush(fpArea);
	  fclose(fpArea);
  }

  if ( fpChar ) // flush any open pfile...
  {
	  fflush(fpChar);
	  fclose(fpChar);
  } 

  if ( iError == ERR_ABORT )
  {
	  abort();
  }
  else
    exit(iError);
}

#if defined(_WIN32)
char *LastErrorStr(void)
{
	LPVOID lpMsgBuf;
	
	FormatMessage(  FORMAT_MESSAGE_ALLOCATE_BUFFER
		|FORMAT_MESSAGE_FROM_SYSTEM
		|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);
	
	return (char *)lpMsgBuf;
}


void gettimeofday( struct timeval *t , void *tz)
{
	struct timeb timebuffer;
	ftime (& timebuffer);
	t->tv_sec = timebuffer.time;
	t->tv_usec = timebuffer.millitm * 1000;
}
#endif

void setup_coresize(void)
{
#if !defined(_WIN32)
	struct rlimit limit;

     getrlimit( RLIMIT_CORE, &limit );
     limit.rlim_cur = limit.rlim_max;

     if ( setrlimit( RLIMIT_CORE , &limit ) == -1 )
     {
         bugf("[main.c::setup_coresize] Error while setting limit. ");
         _quit(1);
     }
#endif
}


// be careful to call this AFTER the skill table is loaded and AFTER the skill_hash_table
// has been initialized.
void init_gsns(void)
{

	// The design decision here was that;
	// a) All weapon skills would have a gsn
	// b) All combat skills would have a gsn
	// c) Any skill regularly checked by the code would have a gsn (such as those
	//    referenced in can_see
   int i = 0;
   int value = 0;
   
   for (i = 0; !IS_NULLSTR(gsn_table[i].skill); i++)
   {
	   value = reference_skill(gsn_table[i].skill);
	   *gsn_table[i].gsn = value;
	   if ( !IS_VALID_SKILL(value) )
		   bugf("[main.c::init_gsns] No GSN found for skill '%s'! this WILL cause a crash eventually!.",
		     gsn_table[i].skill);
   }
}


/*
 * Big mama top level function.
 */
void boot_db ()
{
	sh_int srt_skills = 0;

    setup_coresize();

    /*
	 * Init some data space stuff.
	 */
	{
		if ((string_space = calloc (1, MAX_STRING)) == NULL)
		{
			bugf ("[main.c::boot_db] can't alloc %d string space.", MAX_STRING);
			_quit (1);
		}
		top_string = string_space;
		fBootDb = TRUE;
	}
	
    /*
	 * Init random number generator.
	 */
	{
		init_mm ();
	}
	
    /*
	 * Initialize the global auction system.
	 */
	house_auctions = new_auction ();
	
    /*
	 * Set time and weather.
	 */
	{
		long lhour;
		
		lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
		time_info.hour = lhour % 24;

		// Load the time from time.dat
		load_time();
			
				
		if (time_info.hour < 5)
			weather_info.sunlight = SUN_DARK;
		else if (time_info.hour < 6)
			weather_info.sunlight = SUN_RISE;
		else if (time_info.hour < 19)
			weather_info.sunlight = SUN_LIGHT;
		else if (time_info.hour < 20)
			weather_info.sunlight = SUN_SET;
		else
			weather_info.sunlight = SUN_DARK;
		
	}
	
    /**
	 * Read data files before we call "load_rooms"
	 * Be sure we sort the skill table before assigning gsns.
	 * Assign gsn's for skills which have them.
	 */
	load_mud_data ();
	load_skill_data ();
	load_names ();
	load_weather();
	load_guilds ();
	load_races ();
	load_groups ();
	load_liquids ();
	load_forms ();
	load_commands ();
	load_quotes ();
	load_social_table ();
	load_slay_data ();
	load_bans ();
	load_bank ();
	load_songs ();
	logf ("Loading area files...");
	
	/* Initialize hash tables */
	init_race_hash_table ();
	init_skill_hash_table();
	init_gsns();

	srt_skills = sort_skills ();
	
	{
		int sn;
		
		for (sn = 0; sn < MAX_TALENTS; sn++)
		{
			if (talent_table[sn].pgtn != NULL)
				*talent_table[sn].pgtn = sn;

			SET_FLAG(talent_table[sn].flags, _NOT_GAINABLE);
		}
	}
	
    /*
	 * Read in all the area files.
	 */
	{
		FILE *fpList;
		
		if ((fpList = fopen (AREA_LIST, "r")) == NULL)
		{
			bugf("[main.c::boot_db()] Unable to open area list file '%s' for reading.", AREA_LIST);
			_quit (1);
		}
		
		for (;;)
		{
			strcpy (strArea, fread_word (fpList));
			if (strArea[0] == '$')
				break;
			
			if (strArea[0] == '-')
			{
				fpArea = stdin;
			}
			else
			{
				if ((fpArea = fopen (strArea, "r")) == NULL)
				{
					bugf("[main.c::boot_db] Unable to open area file '%s' for reading.", strArea);
					_quit (1);
				}
			}
			
			current_area = NULL;
			
			for (;;)
			{
				char *word;
				
				if (fread_letter (fpArea) != '#')
				{
					bugf("[main.c::boot_db] # not found.");
					_quit (1);
				}
				
				word = fread_word (fpArea);
				
				if (word[0] == '$')
					break;
				
				else if (!str_cmp (word, "AREADATA"))
					load_area (fpArea);
				else if (!str_cmp (word, "HELPS"))
					load_helps (fpArea, strArea);
				else if (!str_cmp (word, "MOBILES"))
					load_mobiles (fpArea);
				else if (!str_cmp (word, "MOBPROGS"))
					load_mobprogs (fpArea);
				else if (!str_cmp (word, "OBJECTS"))
					load_objects (fpArea);
				else if (!str_cmp (word, "RESETS"))
					load_resets (fpArea);
				else if (!str_cmp (word, "ROOMS"))
					load_rooms (fpArea);
				else if (!str_cmp (word, "SHOPS"))
					load_shops (fpArea);
				else if (!str_cmp (word, "SPECIALS"))
					load_specials (fpArea);
				else
				{
					bugf("[main.c::boot_db] bad section name.");
					_quit (1);
				}
			}
			
			if (fpArea != stdin)
				fclose (fpArea);
			fpArea = NULL;
		}
		fclose (fpList);
	}
	
	/*
	 * Fix up exits.
	 * Declare db booting over.
	 * Reset all areas once.
	 * Load up the songs, notes, ban and other data files.
	 */
	{
		fix_exits ();
		fix_mobprogs ();
		fBootDb = FALSE;
		area_update (TRUE);
		load_boards ();		
		log_string ("Loading Wizlist.");
		load_wizlist ();
		log_string ("Loading Update Files...");
		load_updates ();
		save_notes ();
		logf (BOLD "Sorted %d skills." NO_COLOR, GREEN, srt_skills);
		logf (BOLD "Sorted %d socials." NO_COLOR, GREEN, sort_socials ());
		logf (BOLD "Sorted %d helps." NO_COLOR, GREEN, sort_helps ());
		logf (BOLD "Stored %d Areas, %d Rooms, %d Mobs & %d Objects" NO_COLOR,
			CYAN, top_area, top_room, top_mob_index, top_obj_index);
		
		logf (BOLD "Finished.  Taos is now accepting incoming clients." NO_COLOR);


		// switch the log to a file....
		#if defined(_WIN32)	
		{
			struct timeval t;
			int tz = 0;
			char dstamp[1024];

			gettimeofday(&t, (void*)tz);

			memset(dstamp, 0, 1024);
			sprintf(dstamp, "%s%ld.log",  LOG_DIR, t.tv_sec);

			fpErr = freopen(dstamp, "w+", stderr);
		}
		#endif



	    /*
		 * Initialize some sets
		 * For gods sake, move this to a data file!
		 */

		// 0 = healing
		set_init (talent_table[0].flags);
		SET_FLAG (talent_table[0].flags, _IS_TALENT);
		
		// travelling
		set_init (talent_table[1].flags);
		SET_FLAG (talent_table[1].flags, _IS_TALENT);
		SET_FLAG (talent_table[1].flags, _REQ_CHANNEL);
		
		// tree_singing
		set_init (talent_table[2].flags);
		SET_FLAG (talent_table[2].flags, _IS_TALENT);
		SET_FLAG (talent_table[2].flags, _REQ_RACE_OGIER);
		
		// dreaming
		set_init (talent_table[3].flags);
		SET_FLAG (talent_table[3].flags, _IS_TALENT);
		
		// unarmed combat
		set_init (talent_table[4].flags);
		SET_FLAG (talent_table[4].flags, _REQ_ROGUE);
		SET_FLAG (talent_table[4].flags, _REQ_WARRIOR);
		SET_FLAG (talent_table[4].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[4].flags, _COMBAT_SPECIALTY);
		
		// knives
		set_init (talent_table[5].flags);
		SET_FLAG (talent_table[5].flags, _REQ_ROGUE);
		SET_FLAG (talent_table[5].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[5].flags, _COMBAT_SPECIALTY);
		
		// physical defense
		set_init (talent_table[6].flags);
		SET_FLAG (talent_table[6].flags, _REQ_WARRIOR);
		SET_FLAG (talent_table[6].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[6].flags, _COMBAT_SPECIALTY);
		
		// defensive weaves
		set_init (talent_table[7].flags);
		SET_FLAG (talent_table[7].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[7].flags, _REQ_CHANNEL);
		SET_FLAG (talent_table[7].flags, _WEAVE_SPECIALTY);
		
		// weaving air
		set_init (talent_table[8].flags);
		SET_FLAG (talent_table[8].flags, _REQ_CHANNELER);
		SET_FLAG (talent_table[8].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[8].flags, _WEAVE_SPECIALTY);
		SET_FLAG (talent_table[8].flags, _REQ_CHANNEL);
		
		// weaving earth
		set_init (talent_table[9].flags);
		SET_FLAG (talent_table[9].flags, _REQ_CHANNELER);
		SET_FLAG (talent_table[9].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[9].flags, _WEAVE_SPECIALTY);
		SET_FLAG (talent_table[9].flags, _REQ_CHANNEL);
		
		// weaving fire
		set_init (talent_table[10].flags);
		SET_FLAG (talent_table[10].flags, _REQ_CHANNELER);
		SET_FLAG (talent_table[10].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[10].flags, _WEAVE_SPECIALTY);
		SET_FLAG (talent_table[10].flags, _REQ_CHANNEL);
		
		// weaving water
		set_init (talent_table[11].flags);
		SET_FLAG (talent_table[11].flags, _REQ_CHANNELER);
		SET_FLAG (talent_table[11].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[11].flags, _WEAVE_SPECIALTY);
		SET_FLAG (talent_table[11].flags, _REQ_CHANNEL);
		
		// weaving spirit
		set_init (talent_table[12].flags);
		SET_FLAG (talent_table[12].flags, _REQ_CHANNELER);
		SET_FLAG (talent_table[12].flags, _IS_SPECIALTY);
		SET_FLAG (talent_table[12].flags, _WEAVE_SPECIALTY);
		SET_FLAG (talent_table[12].flags, _REQ_CHANNEL);
		
		// acrobatics
		set_init (talent_table[13].flags);
		SET_FLAG (talent_table[13].flags, _REQ_ROGUE);
		SET_FLAG (talent_table[13].flags, _IS_TALENT);

		// blacksmithing
		set_init (talent_table[14].flags);
		SET_FLAG (talent_table[14].flags, _REQ_WARRIOR);
		SET_FLAG (talent_table[14].flags, _IS_TALENT);
	}
	
	return;
}





/*
 * Snarf a help section.
 */
void load_helps (FILE * fp, char *fname)
{
	HELP_DATA *pHelp;
	int level;
	char *keyword;
	
	for (;;)
	{
		
		level = fread_number (fp);
		keyword = fread_string (fp);
		
		if (keyword[0] == '$')
			break;
		
		pHelp = new_help ();
		pHelp->level = level;
		pHelp->keyword = keyword;
		
		pHelp->text = fread_string (fp);
		
		if (!str_cmp (pHelp->keyword, "greeting"))
			help_greeting = pHelp->text;
		
		if (help_first == NULL)
			help_first = pHelp;
		
		if (help_last != NULL)
			help_last->next = pHelp;
		
		help_last = pHelp;
		pHelp->next = NULL;
		
		pHelp->next_area = NULL;
		
	}
	
	return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile (MOB_INDEX_DATA * pMobIndex)
{
	CHAR_DATA *mob;
	int i;
	AFFECT_DATA af;
	
	mobile_count++;
	
	if (pMobIndex == NULL)
	{
		bugf("[main.c::create_mobile] NULL pMobIndex.");
		return NULL;
	}
	
	mob = new_char ();
	
	mob->pIndexData = pMobIndex;
	
	mob->name = str_dup (pMobIndex->player_name);	
	mob->short_descr = str_dup (pMobIndex->short_descr);	
	mob->long_descr = str_dup (pMobIndex->long_descr);	
	mob->description = str_dup (pMobIndex->description);	
	mob->spec_fun = pMobIndex->spec_fun;
	mob->prompt = NULL;
	mob->mprog_target = NULL;
	
	if (pMobIndex->wealth == 0)
	{
		mob->silver = 0;
		mob->gold = 0;
	}
	else
	{
		long wealth;
		
		wealth = number_range (pMobIndex->wealth / 2, 3 * pMobIndex->wealth / 2);
		mob->gold = number_range (wealth / 200, wealth / 100);
		mob->silver = wealth - (mob->gold * 100);
	}
	
	/* old format no longer supported */
	
	/* read from prototype */
	set_init (mob->states.comm);
	add_sets (mob->states.comm, pMobIndex->states.comm);
	set_init (mob->states.config);
	add_sets (mob->states.config, pMobIndex->states.config);
	set_init (mob->states.player);
	add_sets (mob->states.player, pMobIndex->states.player);
	set_init (mob->states.penalties);
	add_sets (mob->states.penalties, pMobIndex->states.penalties);
	set_init (mob->states.wiznet);
	add_sets (mob->states.wiznet, pMobIndex->states.wiznet);
	set_init (mob->states.affected_by);
	add_sets (mob->states.affected_by, pMobIndex->states.affected_by);
	set_init (mob->states.form);
	add_sets (mob->states.form, pMobIndex->states.form);
	set_init (mob->states.parts);
	add_sets (mob->states.parts, pMobIndex->states.parts);
	set_init (mob->states.off);
	add_sets (mob->states.off, pMobIndex->states.off);
	set_init (mob->states.imm);
	add_sets (mob->states.imm, pMobIndex->states.imm);
	set_init (mob->states.res);
	add_sets (mob->states.res, pMobIndex->states.res);
	set_init (mob->states.vuln);
	add_sets (mob->states.vuln, pMobIndex->states.vuln);
	
	mob->group = pMobIndex->group;
	mob->alignment = pMobIndex->alignment;
	mob->level = pMobIndex->level;
	mob->hitroll = pMobIndex->hitroll;
	mob->damroll = pMobIndex->damage[DICE_BONUS];
	mob->max_hit = dice (pMobIndex->hit[DICE_NUMBER],
		pMobIndex->hit[DICE_TYPE])
		+ pMobIndex->hit[DICE_BONUS];
	mob->hit = mob->max_hit;
	
	determine_OP (mob, 100);
	
	mob->damage[DICE_NUMBER] = pMobIndex->damage[DICE_NUMBER];
	mob->damage[DICE_TYPE] = pMobIndex->damage[DICE_TYPE];
	mob->dam_type = pMobIndex->dam_type;
	
	if (mob->dam_type == 0)
		switch (number_range (1, 3))
    {
    case (1):
		mob->dam_type = 3;
		break;			/* slash */
    case (2):
		mob->dam_type = 7;
		break;			/* pound */
    case (3):
		mob->dam_type = 11;
		break;			/* pierce */
    }
	
	for (i = 0; i < 4; i++)
		mob->armor[i] = pMobIndex->ac[i];
	
	mob->start_pos = pMobIndex->start_pos;
	mob->default_pos = pMobIndex->default_pos;
	mob->sex = pMobIndex->sex;
	
	if (mob->sex > 2 || mob->sex < 0)	/* random sex */
		mob->sex = number_range (1, 2);
	
	mob->race = pMobIndex->race;
	mob->size = pMobIndex->size;
	mob->material = str_dup (pMobIndex->material);
	
	/* computed on the spot */
	
	for (i = 0; i < MAX_STATS; i++)
		mob->perm_stat[i] = UMIN (25, 11 + mob->level / 4);
	
	if (IS_FLAG (mob->states.player, _ACT_WARRIOR))
	{
		mob->perm_stat[STAT_STR] += 3;
		mob->perm_stat[STAT_INT] -= 1;
		mob->perm_stat[STAT_CON] += 2;
	}
	
	if (IS_FLAG (mob->states.player, _ACT_ROGUE))
	{
		mob->perm_stat[STAT_DEX] += 3;
		mob->perm_stat[STAT_INT] += 1;
		mob->perm_stat[STAT_AGL] += 2;
	}
	
	if (IS_FLAG (mob->states.player, _ACT_CHANNELER))
	{
		mob->perm_stat[STAT_INT] += 3;
		mob->perm_stat[STAT_STR] -= 1;
		mob->perm_stat[STAT_DEX] += 1;
	}
	
	if (IS_FLAG (mob->states.player, _OFF_FAST))
		mob->perm_stat[STAT_DEX] += 2;
	
	mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
	mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;
	
	/* let's get some spell action */
	if (IS_AFFECTED (mob, _AFF_AIR_SHIELD))
	{
		af.where = TO_AFFECTS;
		af.type = skill_lookup("air shield");
		af.level = mob->level;
		af.duration = -1;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.owner = 0;
		af.bitvector = _AFF_AIR_SHIELD;
		affect_to_char (mob, &af);
	}
	
	if (IS_AFFECTED (mob, _AFF_HASTE))
	{
		af.where = TO_AFFECTS;
		af.type = skill_lookup("haste");
		af.level = mob->level;
		af.duration = -1;
		af.location = APPLY_DEX;
		af.owner = 0;
		af.modifier = 1 + (mob->level >= 18) + (mob->level >= 25) +
			(mob->level >= 32);
		af.bitvector = _AFF_HASTE;
		affect_to_char (mob, &af);
	}
	
	if (IS_AFFECTED (mob, _AFF_PROTECT_EVIL))
	{
		af.where = TO_AFFECTS;
		af.type = skill_lookup ("ward evil");
		af.level = mob->level;
		af.duration = -1;
		af.location = APPLY_SAVES;
		af.modifier = -1;
		af.owner = 0;
		af.bitvector = _AFF_PROTECT_EVIL;
		affect_to_char (mob, &af);
	}
	
	if (IS_AFFECTED (mob, _AFF_PROTECT_GOOD))
	{
		af.where = TO_AFFECTS;
		af.type = skill_lookup ("ward good");
		af.level = mob->level;
		af.duration = -1;
		af.location = APPLY_SAVES;
		af.modifier = -1;
		af.owner = 0;
		af.bitvector = _AFF_PROTECT_GOOD;
		affect_to_char (mob, &af);
	}
	
	mob->position = mob->start_pos;
	
	if (IS_FLAG (mob->states.player, _ACT_CHANNELER) && mob->spec_fun == 0)
		mob->spec_fun = spec_lookup ("spec_cast_mage");
	else if (IS_FLAG (mob->states.player, _ACT_ROGUE) && mob->spec_fun == 0)
		mob->spec_fun = spec_lookup ("spec_thief");

	if ( IS_FLAG(mob->pIndexData->states.player, _ACT_PERSISTANT) ||
		 IS_FLAG(mob->states.player, _ACT_PERSISTANT) )
	{
		logf("Loading persistant mobile {#%d:%s}", mob->pIndexData->vnum, mob->name);
		load_npc(mob);
	}

	/* link the mob to the world list */
	mob->next = char_list;
	char_list = mob;
	pMobIndex->count++;
	return mob;
}

/* duplicate a mobile exactly -- except inventory */
void clone_mobile (CHAR_DATA * parent, CHAR_DATA * clone)
{
	int i;
	AFFECT_DATA *paf;
	
	if (parent == NULL || clone == NULL )
		return;
	
	/* start fixing values */
	clone->name = str_dup (parent->name);
	clone->version = parent->version;
	
	if ( !IS_NPC(parent) )
	{
		clone->long_descr = str_dup(parent->name);
		clone->short_descr = str_dup (parent->name);
	}
	else
	{
		clone->long_descr = str_dup (parent->long_descr);
		clone->short_descr = str_dup (parent->short_descr);
	}

	clone->description = str_dup (parent->description);
	clone->group = parent->group;
	clone->sex = parent->sex;
	clone->c_class = parent->c_class;
	clone->race = parent->race;
	clone->level = parent->level;
	clone->trust = 0;
	clone->timer = parent->timer;
	clone->wait = parent->wait;
	clone->hit = parent->hit;
	clone->max_hit = parent->max_hit;
	clone->flow_str = parent->flow_str;
	clone->move = parent->move;
	clone->max_move = parent->max_move;
	clone->gold = parent->gold;
	clone->silver = parent->silver;
	clone->exp = parent->exp;
	
	
	set_init (clone->states.comm);
	add_sets (clone->states.comm, parent->states.comm);
	set_init (clone->states.config);
	add_sets (clone->states.config, parent->states.config);
	set_init (clone->states.player);
	add_sets (clone->states.player, parent->states.player);
	set_init (clone->states.penalties);
	add_sets (clone->states.penalties, parent->states.penalties);
	set_init (clone->states.wiznet);
	add_sets (clone->states.wiznet, parent->states.wiznet);
	set_init (clone->states.affected_by);
	add_sets (clone->states.affected_by, parent->states.affected_by);
	set_init (clone->states.form);
	add_sets (clone->states.form, parent->states.form);
	set_init (clone->states.parts);
	add_sets (clone->states.parts, parent->states.parts);
	set_init (clone->states.off);
	add_sets (clone->states.off, parent->states.off);
	set_init (clone->states.imm);
	add_sets (clone->states.imm, parent->states.imm);
	set_init (clone->states.res);
	add_sets (clone->states.res, parent->states.res);
	set_init (clone->states.vuln);
	add_sets (clone->states.vuln, parent->states.vuln);
	
	clone->invis_level = parent->invis_level;
	clone->position = parent->position;
	clone->practice = parent->practice;
	clone->train = parent->train;
	clone->saving_throw = parent->saving_throw;
	clone->alignment = parent->alignment;
	clone->hitroll = parent->hitroll;
	clone->damroll = parent->damroll;
	clone->wimpy = parent->wimpy;
	clone->size = parent->size;
	clone->material = str_dup (parent->material);
	clone->dam_type = parent->dam_type;
	clone->start_pos = parent->start_pos;
	clone->default_pos = parent->default_pos;
	clone->spec_fun = parent->spec_fun;
	
	for (i = 0; i < 4; i++)
		clone->armor[i] = parent->armor[i];
	
	for (i = 0; i < MAX_STATS; i++)
	{
		clone->perm_stat[i] = parent->perm_stat[i];
		clone->mod_stat[i] = parent->mod_stat[i];
	}
	
	for (i = 0; i < 3; i++)
		clone->damage[i] = parent->damage[i];
	
	/* now add the affects */
	for (paf = parent->affected; paf != NULL; paf = paf->next)
		affect_to_char (clone, paf);
	
}




/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object (OBJ_INDEX_DATA * pObjIndex, int level)
{
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	int i;
	
	if (pObjIndex == NULL)
	{
		bugf("[main.c::create_object] NULL pObjIndex.");
		return NULL;
	}
	
	obj = new_obj ();
	
	obj->pIndexData = pObjIndex;
	obj->in_room = NULL;
	obj->enchanted = FALSE;
	obj->level = (level < 1) ? pObjIndex->level : level;
	obj->wear_loc = -1;
	
	obj->name = str_dup (pObjIndex->name);	
	obj->short_descr = str_dup (pObjIndex->short_descr);	
	obj->description = str_dup (pObjIndex->description);	
	obj->material = str_dup (pObjIndex->material);
	obj->item_type = pObjIndex->item_type;
	obj->extra_flags = pObjIndex->extra_flags;
	obj->wear_flags = pObjIndex->wear_flags;
	obj->value[0] = pObjIndex->value[0];
	obj->value[1] = pObjIndex->value[1];
	obj->value[2] = pObjIndex->value[2];
	obj->value[3] = pObjIndex->value[3];
	obj->value[4] = pObjIndex->value[4];
	obj->weight = pObjIndex->weight;
	obj->condition = pObjIndex->condition;
	obj->max_condition = pObjIndex->max_condition;
	
	if (level == -1)
		obj->cost = pObjIndex->cost;
	else
		obj->cost = number_fuzzy (10)
		* number_fuzzy (level) * number_fuzzy (level);
	
	/*
	 * Mess with object properties.
	 */
	switch (obj->item_type)
	{
	default:
		bugf("[main.c::create_object] vnum %d bad type. ... Setting to type \"Trash\"", pObjIndex->vnum);
		obj->item_type = ITEM_TRASH;
		break;
		
	case ITEM_LIGHT:
		if (obj->value[2] == 999)
			obj->value[2] = -1;
		break;
		
	case ITEM_FURNITURE:
	case ITEM_TRASH:
	case ITEM_CONTAINER:
	case ITEM_DRINK_CON:
	case ITEM_KEY:
	case ITEM_FOOD:
	case ITEM_BOAT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FOUNTAIN:
	case ITEM_MAP:
	case ITEM_CLOTHING:
	case ITEM_HCLOAK:
	case ITEM_PORTAL:
	case ITEM_WAYGATE:
		obj->cost /= 5;
		break;
		
	case ITEM_PROTECT:
	case ITEM_TATTOO:
	case ITEM_TREASURE:
	case ITEM_ROOM_KEY:
	case ITEM_GEM:
	case ITEM_JEWELRY:
		break;
		
	case ITEM_JUKEBOX:
		for (i = 0; i < 5; i++)
			obj->value[i] = -1;
		break;
		
//  wand/staff will be ter'angreal at some point
	case ITEM_WAND:
	case ITEM_STAFF:
		break;
		
	case ITEM_ANGREAL:
	case ITEM_QUIVER:
	case ITEM_ARROW:
	case ITEM_WEAPON:
		break;
		
	case ITEM_ARMOR:
		break;
		
	case ITEM_POTION:
	case ITEM_HERB:
		break;
		
	case ITEM_MONEY:
		break;
	}
	
	for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
		if (paf->location == APPLY_SPELL_AFFECT)
			affect_to_obj (obj, paf);
		
		obj->next = object_list;
		object_list = obj;
		pObjIndex->count++;
		
		return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object (OBJ_DATA * parent, OBJ_DATA * clone)
{
	int i;
	AFFECT_DATA *paf;
	EXTRA_DESCR_DATA *ed, *ed_new;
	
	if (parent == NULL || clone == NULL)
		return;
	
	/* start fixing the object */
	clone->name = str_dup (parent->name);
	clone->short_descr = str_dup (parent->short_descr);
	clone->description = str_dup (parent->description);
	clone->item_type = parent->item_type;
	clone->extra_flags = parent->extra_flags;
	clone->wear_flags = parent->wear_flags;
	clone->weight = parent->weight;
	clone->cost = parent->cost;
	clone->level = parent->level;
	clone->condition = parent->condition;
	clone->max_condition = parent->max_condition;
	clone->material = str_dup (parent->material);
	clone->timer = parent->timer;
	
	for (i = 0; i < 5; i++)
		clone->value[i] = parent->value[i];
	
	/* affects */
	clone->enchanted = parent->enchanted;
	
	for (paf = parent->affected; paf != NULL; paf = paf->next)
		affect_to_obj (clone, paf);
	
	/* extended desc */
	for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
	{
		ed_new = new_extra_descr ();
		ed_new->keyword = str_dup (ed->keyword);
		ed_new->description = str_dup (ed->description);
		ed_new->next = clone->extra_descr;
		clone->extra_descr = ed_new;
	}
	
}



/*
 * Clear a new character.
 */
void clear_char (CHAR_DATA * ch)
{
	static CHAR_DATA ch_zero;
	int i;
	
	*ch = ch_zero;
	ch->name = &str_empty[0];
	ch->short_descr = &str_empty[0];
	ch->long_descr = &str_empty[0];
	ch->description = &str_empty[0];
	if ( !IS_NPC(ch) )
	  ch->pcdata->profile = &str_empty[0];
	ch->prompt = &str_empty[0];
	ch->logon = current_time;
	ch->lines = PAGELEN;
	for (i = 0; i < 4; i++)
		ch->armor[i] = 100;
	ch->position = POS_STANDING;
	ch->hit = 20;
	ch->max_hit = 20;
	ch->level = 1;
	determine_OP (ch, 100);
	ch->move = 100;
	ch->max_move = 100;
	ch->on = NULL;
	for (i = 0; i < MAX_STATS; i++)
	{
		ch->perm_stat[i] = 13;
		ch->mod_stat[i] = 0;
	}
	return;
}

/*
 * Get an extra description from a list.
 */
char *get_extra_descr (const char *name, EXTRA_DESCR_DATA * ed)
{
	for (; ed != NULL; ed = ed->next)
	{
		if (is_name ((char *) name, ed->keyword))
			return ed->description;
	}
	return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index (long vnum)
{
	MOB_INDEX_DATA *pMobIndex;
	
	for (pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH];
	pMobIndex != NULL; pMobIndex = pMobIndex->next)
	{
		if (pMobIndex->vnum == vnum)
			return pMobIndex;
	}
	
	if (fBootDb)
	{
		bugf("[main.c::get_mob_index] bad vnum %ld.", vnum);
		_quit (1);
	}
	
	return NULL;
}



/*
 * Translates obj virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index (long vnum)
{
	OBJ_INDEX_DATA *pObjIndex;
	
	for (pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH];
	pObjIndex != NULL; pObjIndex = pObjIndex->next)
	{
		if (pObjIndex->vnum == vnum)
			return pObjIndex;
	}
	
	if (fBootDb)
	{
		bugf("[main.c::get_obj_index] bad vnum %ld.", vnum);
		_quit (1);
	}
	
	return NULL;
}


/*
 * Translates room virtual number to its room index struct.
 *  Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index (long vnum)
{
	int count = 0;

	ROOM_INDEX_DATA *pRoomIndex;

	for (pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH];
	pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
	{
		++count;
		if (pRoomIndex->vnum == vnum)
		{
//			_trace(FALSE, "[MAIN.C::get_room_index()] Took %d iterations to locate desired room.", count);
			return pRoomIndex;
		}
	}
	
//	if (fBootDb)
//		bugf ("[main.c::get_room_index] [%s]: bad vnum %ld.", current_area ? current_area->file_name : "<none>", vnum);

	return NULL;
}

MPROG_CODE *get_mprog_index (long vnum)
{
	MPROG_CODE *prg;
	for (prg = mprog_list; prg; prg = prg->next)
	{
		if (prg->vnum == vnum)
			return (prg);
	}
	return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter (FILE * fp)
{
	char c;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	return c;
}



/*
 * Read a number from a file.
 */
int fread_number (FILE * fp)
{
	int number;
	bool sign;
	char c;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	number = 0;
	
	sign = FALSE;
	if (c == '+')
	{
		c = getc (fp);
	}
	else if (c == '-')
	{
		sign = TRUE;
		c = getc (fp);
	}
	
	if (!isdigit (c))
	{
		bugf ("[main.c::fread_number] expected a digit, received a '%c'", c);
		if (AttemptJump)		/* bad pfile */
		{
			longjmp (jump_env, 1);
		}
		else
			_quit(ERR_ABORT);
	}
	
	while (isdigit (c))
	{
		number = number * 10 + c - '0';
		c = getc (fp);
	}
	
	if (sign)
		number = 0 - number;
	
	if (c == '|')
		number += fread_number (fp);
	else if (c != ' ')
		ungetc (c, fp);
	
	return number;
}

/*
 * Read a long integer from a file.
 */
long fread_long (FILE * fp)
{
	long number;
	bool sign;
	char c;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	number = 0;
	
	sign = FALSE;
	if (c == '+')
	{
		c = getc (fp);
	}
	else if (c == '-')
	{
		sign = TRUE;
		c = getc (fp);
	}
	
	if (!isdigit (c))
	{
		bugf ("[main.c::fread_long] expected a digit, received a '%c'", c);
		if (AttemptJump)		/* bad pfile */
		{
			longjmp (jump_env, 1);
		}
		else
			_quit(ERR_ABORT);
	}
	
	while (isdigit (c))
	{
		number = number * 10 + c - '0';
		c = getc (fp);
	}
	
	if (sign)
		number = 0 - number;
	
	if (c == '|')
		number += fread_number (fp);
	else if (c != ' ')
		ungetc (c, fp);
	
	return number;
}


long fread_flag (FILE * fp)
{
	int number;
	char c;
	bool negative = FALSE;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	if (c == '-')
	{
		negative = TRUE;
		c = getc (fp);
	}
	
	number = 0;
	
	if (!isdigit (c))
	{
		while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
		{
			number += flag_convert (c);
			c = getc (fp);
		}
	}
	
	while (isdigit (c))
	{
		number = number * 10 + c - '0';
		c = getc (fp);
	}
	
	if (c == '|')
		number += fread_flag (fp);
	
	else if (c != ' ')
		ungetc (c, fp);
	
	if (negative)
		return -1 * number;
	
	return number;
}

long flag_convert (char letter)
{
	long bitsum = 0;
	char i;
	
	if ('A' <= letter && letter <= 'Z')
	{
		bitsum = 1;
		for (i = letter; i > 'A'; i--)
			bitsum *= 2;
	}
	else if ('a' <= letter && letter <= 'z')
	{
		bitsum = 67108864;		/* 2^26 */
		for (i = letter; i > 'a'; i--)
			bitsum *= 2;
	}
	
	return bitsum;
}




/*
* Read and allocate space for a string from a file.
* These strings are read-only and shared.
* Strings are hashed:
*   each string prepended with hash pointer to prev string,
*   hash code is simply the string length.
*   this function takes 40% to 50% of boot-up time.
*/
char *fread_string (FILE * fp)
{
	char *plast;
	char c;
	
	plast = top_string + sizeof (char *);
	if (plast > &string_space[MAX_STRING - MSL])
	{
		bugf ("[main.c::fread_string] MAX_STRING [%d] exceeded", MAX_STRING);
		if (AttemptJump)		/* bad pfile */
		{
			longjmp (jump_env, 1);
		}
		else
			_quit(ERR_ABORT);
	}
	
	/*
	* Skip blanks.
	* Read first char.
	*/
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	if ((*plast++ = c) == '~')
		return &str_empty[0];
	
	for (;;)
	{
    /*
	* Back off the char type lookup,
	*   it was too dirty for portability.
	*   -- Furey
		*/
		
		switch (*plast = getc (fp))
		{
		default:
			plast++;
			break;
			
		case EOF:
			bugf ("[main.c::fread_string] EOF reached.");
			if (AttemptJump)		/* bad pfile */
			{
				longjmp (jump_env, 1);
			}
			else
				_quit(ERR_ABORT);
			break;
			
		case '\n':
			plast++;
			*plast++ = '\r';
			break;
			
		case '\r':
			break;
			
		case '~':
			plast++;
			{
				union
				{
					char *pc;
					char rgc[sizeof (char *)];
				}
				u1;
				int ic;
				int iHash;
				char *pHash;
				char *pHashPrev;
				char *pString;
				
				plast[-1] = '\0';
				iHash = UMIN (MAX_KEY_HASH - 1, plast - 1 - top_string);
				for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
				{
					for (ic = 0; ic < sizeof (char *); ic++)
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof (char *);
					
					if (top_string[sizeof (char *)] == pHash[0]
						&& !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
						return pHash;
				}
				
				if (fBootDb)
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for (ic = 0; ic < sizeof (char *); ic++)
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;
					
					nAllocString += 1;
					sAllocString += top_string - pString;
					return pString + sizeof (char *);
				}
				else
				{
					return str_dup (top_string + sizeof (char *));
				}
			}
		}
	}
}

char *fread_string_eol (FILE * fp)
{
	static bool char_special[256 - EOF];
	char *plast;
	char c;
	
	if (char_special[EOF - EOF] != TRUE)
	{
		char_special[EOF - EOF] = TRUE;
		char_special['\n' - EOF] = TRUE;
		char_special['\r' - EOF] = TRUE;
	}
	
	plast = top_string + sizeof (char *);
	if (plast > &string_space[MAX_STRING - MSL])
	{
		bugf ("[main.c::fread_string_eol] MAX_STRING [%d] exceeded", MAX_STRING);
		if (AttemptJump)		/* bad pfile */
		{
			longjmp (jump_env, 1);
		}
		else
			_quit(ERR_ABORT);
	}
	
	/*
	* Skip blanks.
	* Read first char.
	*/
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	if ((*plast++ = c) == '\n')
		return &str_empty[0];
	
	for (;;)
	{
		if (!char_special[(*plast++ = getc (fp)) - EOF])
			continue;
		
		switch (plast[-1])
		{
		default:
			break;
			
		case EOF:
			bugf ("[main.c::fread_string_eol] EOF reached.");
			if (AttemptJump)		/* bad pfile */
			{
				longjmp (jump_env, 1);
			}
			else
				_quit(ERR_ABORT);
			break;
			
			
		case '\n':
		case '\r':
			{
				union
				{
					char *pc;
					char rgc[sizeof (char *)];
				}
				u1;
				int ic;
				int iHash;
				char *pHash;
				char *pHashPrev;
				char *pString;
				
				plast[-1] = '\0';
				iHash = UMIN (MAX_KEY_HASH - 1, plast - 1 - top_string);
				for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
				{
					for (ic = 0; ic < sizeof (char *); ic++)
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof (char *);
					
					if (top_string[sizeof (char *)] == pHash[0]
						&& !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
						return pHash;
				}
				
				if (fBootDb)
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for (ic = 0; ic < sizeof (char *); ic++)
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;
					
					nAllocString += 1;
					sAllocString += top_string - pString;
					return pString + sizeof (char *);
				}
				else
				{
					return str_dup (top_string + sizeof (char *));
				}
			}
		}
	}
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol (FILE * fp)
{
	char c;
	
	do
	{
		c = getc (fp);
	}
	
	while (c != '\n' && c != '\r' && c != EOF);
	do
	{
		c = getc (fp);
	}
	
	while (c == '\n' || c == '\r');
	ungetc (c, fp);
	
	return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word (FILE * fp)
{
	static char word[MIL];
	char *pword;
	char cEnd;
	
	do
	{
		cEnd = getc (fp);
	}
	while (isspace (cEnd));
	
	if (cEnd == '\'' || cEnd == '"')
	{
		pword = word;
	}
	else
	{
		word[0] = cEnd;
		pword = word + 1;
		cEnd = ' ';
	}
	
	for (; pword < word + MIL; pword++)
	{
		*pword = getc (fp);
		if (cEnd == ' ' ? isspace (*pword) : *pword == cEnd)
		{
			if (cEnd == ' ')
				ungetc (*pword, fp);
			*pword = '\0';
			return word;
		}
	}
	
	bugf ("[main.c::fread_word] word too long.");

	if (AttemptJump)		/* bad pfile */
	{
		longjmp (jump_env, 1);
	}
	
	_quit(ERR_ABORT);
	return 0; // pesky warnings...
}

/*
* Allocate some ordinary memory,
*   with the expectation of freeing it someday.
*/
void *alloc_mem (int sMem)
{
	void *pMem;
	int *magic;
	int iList;
	
	sMem += sizeof (*magic);
	
	for (iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if (sMem <= rgSizeList[iList])
			break;
	}
	
	if (iList == MAX_MEM_LIST)
	{
		bugf("[main.c::alloc_mem] size %d too large.", sMem);
		_quit(1);
	}
	
	if (rgFreeList[iList] == NULL)
	{
		pMem = alloc_perm (rgSizeList[iList]);
	}
	else
	{
		pMem = rgFreeList[iList];
		rgFreeList[iList] = *((void **) rgFreeList[iList]);
	}
	
	magic = (int *) pMem;
	*magic = MAGIC_NUM;
	//  pMem += sizeof (*magic);
	pMem = (void*) (((char*)pMem) + sizeof (*magic));
	
	return pMem;
}



/*
* Free some memory.
* Recycle it back onto the free list for blocks of that size.
*/
void free_mem (void *pMem, int sMem)
{
	int iList;
	int *magic;
	
	//  pMem -= sizeof (*magic);
	pMem = (void*) (((char*)pMem) - sizeof (*magic));	
	magic = (int *) pMem;
	

	if ( !magic )
	{
		bugf("[main.c::free_mem] Attempt to recycle NULL memory.");
		return;
	}

	if (*magic != MAGIC_NUM)
	{
		bugf ("[main.c::free_mem] Attempt to recyle invalid memory of size %d. [%s]", sMem, 
		(char *) pMem + sizeof (*magic));
		return;
	}
	
	*magic = 0;
	sMem += sizeof (*magic);
	
	for (iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if (sMem <= rgSizeList[iList])
			break;
	}
	
	if (iList == MAX_MEM_LIST)
	{
		bugf("[main.c::free_mem] size %d too large.", sMem);
		_quit (1);
	}
	
	*((void **) pMem) = rgFreeList[iList];
	rgFreeList[iList] = pMem;
	
	return;
}


/*
* Allocate some permanent memory.
* Permanent memory is never freed,
* pointers into it may be copied safely.
*/
void *alloc_perm (int sMem)
{
	static char *pMemPerm;
	static int iMemPerm;
	void *pMem;
	
	while (sMem % sizeof (long) != 0)
		sMem++;
	if (sMem > MAX_PERM_BLOCK)
	{
		bugf("[main.c::alloc_perm] %d too large.", sMem);
		_quit(1);
	}
	
	if (pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK)
	{
		iMemPerm = 0;
		if ((pMemPerm = calloc (1, MAX_PERM_BLOCK)) == NULL)
		{
			bugf("[main.c::alloc_perm] Calloc failed.");
			_quit(1);
		}
	}
	
	pMem = pMemPerm + iMemPerm;
	iMemPerm += sMem;
	nAllocPerm += 1;
	sAllocPerm += sMem;
	return pMem;
}



/*
* Duplicate a string into dynamic memory.
* Fread_strings are read-only and shared.
*/
char *str_dup (const char *str)
{
	char *str_new;
	
	if (IS_NULLSTR(str))
		return &str_empty[0];
	
	if (str >= string_space && str < top_string)
		return (char *) str;
	
	str_new = alloc_mem (strlen (str) + 1);
	strcpy (str_new, str);
	return str_new;
}



/*
* Free a string.
* Null is legal here to simplify callers.
* Read-only shared strings are not touched.
*/
void free_string (char *pstr)
{
	if (pstr == NULL
		|| pstr == &str_empty[0] || (pstr >= string_space && pstr < top_string))
		return;
	
	free_mem (pstr, strlen (pstr) + 1);
	return;
}



_DOFUN (do_areas)
{
	AREA_DATA *pArea1;
	AREA_DATA *pArea2;
	int iArea;
	int iAreaHalf;
	
	
	iAreaHalf = (top_area + 1) / 2;
	pArea1 = area_first;
	pArea2 = area_first;
	
	for (iArea = 0; iArea < iAreaHalf; iArea++)
		pArea2 = pArea2->next;
	
	for (iArea = 0; iArea < iAreaHalf; iArea++)
	{
		chprintf (ch, "%-43.43s%-37.37s\n\r",
			(IS_SET (pArea1->area_flags, AREA_CLOSED)) ? "<under construction>" :
			pArea1->credits,
			(pArea2 != NULL && !IS_SET (pArea2->area_flags, AREA_CLOSED)) ? pArea2->credits : "<under construction>");
		
		pArea1 = pArea1->next;
		if (pArea2 != NULL)
			pArea2 = pArea2->next;
	}
	
	return;
}



_DOFUN (do_memory)
{
	char buf[MSL];
	
	sprintf (buf, "Affects %5d\n\r", top_affect);
	send_to_char (buf, ch);
	sprintf (buf, "Areas   %5d\n\r", top_area);
	send_to_char (buf, ch);
	sprintf (buf, "ExDes   %5d\n\r", top_ed);
	send_to_char (buf, ch);
	sprintf (buf, "Exits   %5d\n\r", top_exit);
	send_to_char (buf, ch);
	sprintf (buf, "Helps   %5d\n\r", top_help);
	send_to_char (buf, ch);
	sprintf (buf, "Socials %5d\n\r", maxSocial);
	send_to_char (buf, ch);
	sprintf (buf, "Mobs    %5d\n\r", top_mob_index);
	send_to_char (buf, ch);
	sprintf (buf, "(in use)%5d\n\r", mobile_count);
	send_to_char (buf, ch);
	sprintf (buf, "Objs    %5d\n\r", top_obj_index);
	send_to_char (buf, ch);
	sprintf (buf, "Resets  %5d\n\r", top_reset);
	send_to_char (buf, ch);
	sprintf (buf, "Rooms   %5d\n\r", top_room);
	send_to_char (buf, ch);
	sprintf (buf, "Shops   %5d\n\r", top_shop);
	send_to_char (buf, ch);
	sprintf (buf, "Names   %5d\n\r", top_name);
	send_to_char (buf, ch);
	sprintf (buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
		nAllocString, sAllocString, MAX_STRING);
	send_to_char (buf, ch);
	
	sprintf (buf, "Perms   %5d blocks  of %7d bytes.\n\r",
		nAllocPerm, sAllocPerm);
	
	/* sounds */
	chprintf (ch, "Sounds  %4d\n\r", top_sound);
	
	send_to_char (buf, ch);
	
	return;
}

#if defined(_WIN32)
  #pragma warning(disable:4101)
#endif
_DOFUN (do_dump)
{
	int count, count2, num_pcs, aff_count;
	CHAR_DATA *fch;
	MOB_INDEX_DATA *pMobIndex;
	PC_DATA *pc;
	OBJ_DATA *obj;
	OBJ_INDEX_DATA *pObjIndex;
	ROOM_INDEX_DATA *room;
	EXIT_DATA *exit;
	DESCRIPTOR_DATA *d;
	AFFECT_DATA *af;
	NAME_DATA *name;
	SOUND_DATA *sound;
	FILE *fp;
	long vnum; 
	int nMatch = 0;
	
	/* open file */
	fp = fopen ("mem.dmp", "w");
	
	/* report use of data structures */
	
	num_pcs = 0;
	aff_count = 0;
	
	/* mobile prototypes */
	fprintf (fp, "MobProt	%4d (%8d bytes)\n",
		top_mob_index, top_mob_index * (sizeof (*pMobIndex)));
	
	/* mobs */
	count = 0;
	count2 = 0;
	for (fch = char_list; fch != NULL; fch = fch->next)
	{
		count++;
		if (fch->pcdata != NULL)
			num_pcs++;
		for (af = fch->affected; af != NULL; af = af->next)
			aff_count++;
	}
	for (fch = char_free; fch != NULL; fch = fch->next)
		count2++;
	
	fprintf (fp, "Mobs	%4d (%8d bytes), %2d free (%d bytes)\n",
		count, count * (sizeof (*fch)), count2, count2 * (sizeof (*fch)));
	
	/* pcdata */
	count = 0;
	for (pc = pcdata_free; pc != NULL; pc = pc->next)
		count++;
	
	fprintf (fp, "Pcdata	%4d (%8d bytes), %2d free (%d bytes)\n",
		num_pcs, num_pcs * (sizeof (*pc)), count, count * (sizeof (*pc)));
	
	/* descriptors */
	count = 0;
	count2 = 0;
	for (d = descriptor_list; d != NULL; d = d->next)
		count++;
	for (d = descriptor_free; d != NULL; d = d->next)
		count2++;
	
	fprintf (fp, "Descs	%4d (%8d bytes), %2d free (%d bytes)\n",
		count, count * (sizeof (*d)), count2, count2 * (sizeof (*d)));
	
	/* object prototypes */
	for (vnum = 0; nMatch < top_obj_index; vnum++)
		if ((pObjIndex = get_obj_index (vnum)) != NULL)
		{
			for (af = pObjIndex->affected; af != NULL; af = af->next)
				aff_count++;
			nMatch++;
		}
		
		fprintf (fp, "ObjProt	%4d (%8d bytes)\n",
			top_obj_index, top_obj_index * (sizeof (*pObjIndex)));
		
		
		/* objects */
		count = 0;
		count2 = 0;
		for (obj = object_list; obj != NULL; obj = obj->next)
		{
			count++;
			for (af = obj->affected; af != NULL; af = af->next)
				aff_count++;
		}
		for (obj = obj_free; obj != NULL; obj = obj->next)
			count2++;
		
		fprintf (fp, "Objs	%4d (%8d bytes), %2d free (%d bytes)\n",
			count, count * (sizeof (*obj)), count2, count2 * (sizeof (*obj)));
		
		/* affects */
		count = 0;
		for (af = affect_free; af != NULL; af = af->next)
			count++;
		
		fprintf (fp, "Affects	%4d (%8d bytes), %2d free (%d bytes)\n",
			aff_count, aff_count * (sizeof (*af)), count,
			count * (sizeof (*af)));
		
		/* rooms */
		fprintf (fp, "Rooms	%4d (%8d bytes)\n",
			top_room, top_room * (sizeof (*room)));
		
		/* exits */
		fprintf (fp, "Exits	%4d (%8d bytes)\n",
			top_exit, top_exit * (sizeof (*exit)));
		
		/* names */
		fprintf (fp, "Names   %4d (%8d bytes)\n",
			top_name, top_name * (sizeof (*name)));
		
		/* sounds */
		fprintf (fp, "Sounds  %4d (%8d bytes)\n",
			top_sound, top_sound * (sizeof (*sound)));
		
		fclose (fp);
		
		/* start printing out mobile data */
		fp = fopen ("mob.dmp", "w");
		
		fprintf (fp, "\nMobile Analysis\n");
		fprintf (fp, "---------------\n");
		nMatch = 0;
		for (vnum = 0; nMatch < top_mob_index; vnum++)
			if ((pMobIndex = get_mob_index (vnum)) != NULL)
			{
				nMatch++;
				fprintf (fp, "#%-4ld %3d active %3d killed     %s\n",
					pMobIndex->vnum, pMobIndex->count,
					pMobIndex->killed, pMobIndex->short_descr);
			}
			fclose (fp);
			
			/* start printing out object data */
			fp = fopen ("obj.dmp", "w");
			
			fprintf (fp, "\nObject Analysis\n");
			fprintf (fp, "---------------\n");
			nMatch = 0;
			for (vnum = 0; nMatch < top_obj_index; vnum++)
				if ((pObjIndex = get_obj_index (vnum)) != NULL)
				{
					nMatch++;
					fprintf (fp, "#%-4d %3d active %3d reset      %s\n",
						pObjIndex->vnum, pObjIndex->count,
						pObjIndex->reset_num, pObjIndex->short_descr);
				}
				
				/* close file */
				fclose (fp);
}
#if defined(_WIN32)
  #pragma warning(default: 4101)
#endif



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy (int number)
{
	switch (number_bits (2))
	{
	case 0:
		number -= 1;
		break;
	case 3:
		number += 1;
		break;
	}
	
	return UMAX (1, number);
}

// 
// Make a skill roll
// Determines the 1dSkill roll
//

int skill_roll (CHAR_DATA *ch, int sn)
{
	int skroll;
	int sk_r1, sk_r2, sk_r3;
	int sec_roll = 0;
	int thd_roll = 0;
	int value = 0;

	// Error trapping incase they have a skill above level 30
	if (ch->pcdata->learned[sn] > 30)
	{
		logf("[main.c::skill_roll] $n has a skill above 30", ch);
		return 0;
	}

	// First case: The skill is level 10 or below
	// This case is a 1dSkill level roll
	if (ch->pcdata->learned[sn] >= 10)
	skroll = random_dice(1, ch->pcdata->learned[sn], 0);

	// Second case: The skills is between level 11 and level 20
	// This case we roll a 1d10 roll and a 1dLevel - 10 roll and take the highest value
	if (ch->pcdata->learned[sn] > 10 && ch->pcdata->learned[sn] <= 20)
	{
		// Need to calculate second dice upper limit
		sec_roll = ch->pcdata->learned[sn] - 10;
		// First roll, 1d10
		sk_r1 = random_dice(1, 10, 0);
		// Second roll 1dSkill - 10
		sk_r2 = random_dice(1, sec_roll, 0);
	
	// Take the highest value
	if (sk_r1 > sk_r2)
		value = sk_r1;
	else
		value = sk_r2;
	}
	// Third case: The skill is between level 21 and 30
	// Here we roll: two 1d10s and a 1dSkill - 20 taking the highest value
	if (ch->pcdata->learned[sn] > 20 && ch->pcdata->learned[sn] <= 30)
	{
		// Calculating the third dice upper limit
		thd_roll = ch->pcdata->learned[sn] - 20;
		// First roll, 1d10
		sk_r1 = random_dice(1, 10, 0);
		// Second roll, 1d10
		sk_r2 = random_dice(1, 10, 0);
		// Third roll, 1dSkill - 20
		sk_r3 = random_dice(1, thd_roll, 0);

	// Take the highest value
	if (sk_r1 > sk_r2)
		value = sk_r1;
	else
		value = sk_r2;
	if (sk_r3 > value)
		value = sk_r3;
	}

	// Ok now we return the calculated value
	// This value will be between 1 and 10

	return value;
}

//
// Generates a more detailed dice roll
// Syntax: random_dice(number, size, bonus)
// i.e. 1d10 + 4 = random_dice(1, 10, 4)
//
int random_dice (int number, int size, int bonus)
{
  int roll;
  int rolls;
  int value;

  value = 0;

    for (rolls = 1; rolls <= number; ++rolls)
	{ 
	  roll = number_range(1, size);
	  value = value + roll;
	  roll = 0;
	}

  return value + bonus;
}
	 
/*
 * Generate a random number.
 */
int number_range (int from, int to)
{
	int power;
	int number;
	
	if (from == 0 && to == 0)
		return 0;
	
	if ((to = to - from + 1) <= 1)
		return from;
	
	for (power = 2; power < to; power <<= 1)
		;
	
	while ((number = number_mm () & (power - 1)) >= to)
		;
	
	return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent (void)
{
	int percent;
	
	while ((percent = number_mm () & (128 - 1)) > 99)
		;
	
	return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door (void)
{
	int door;
	
	while ((door = number_mm () & (8 - 1)) > 5)
		;
	
	return door;
}

int number_bits (int width)
{
	return number_mm () & ((1 << width) - 1);
}




/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */

/* I noticed streaking with this random number generator, so I switched
back to the system srandom call.  If this doesn't work for you, 
define OLD_RAND to use the old system -- Alander */

#if defined (OLD_RAND)
  static int rgiState[2 + 55];
#endif

void init_mm ()
{
#if defined (OLD_RAND)
	int *piState;
	int iState;
	
	piState = &rgiState[2];
	
	piState[-2] = 55 - 55;
	piState[-1] = 55 - 24;
	
	piState[0] = ((int) current_time) & ((1 << 30) - 1);
	piState[1] = 1;
	for (iState = 2; iState < 55; iState++)
	{
		piState[iState] = (piState[iState - 1] + piState[iState - 2])
			& ((1 << 30) - 1);
	}
#else
  #if !defined(_WIN32)
	srandom (time (NULL) ^ getpid ());
  #else
	srand(time(NULL)^_getpid());
  #endif
#endif

	return;
}



long number_mm (void)
{
#if defined (OLD_RAND)
	int *piState;
	int iState1;
	int iState2;
	int iRand;
	
	piState = &rgiState[2];
	iState1 = piState[-2];
	iState2 = piState[-1];
	iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
	piState[iState1] = iRand;
	if (++iState1 == 55)
		iState1 = 0;
	if (++iState2 == 55)
		iState2 = 0;
	piState[-2] = iState1;
	piState[-1] = iState2;
	return iRand >> 6;
#else
  #if !defined(_WIN32)
 	 return random () >> 6;
  #else
	 return rand() >> 6;
  #endif
#endif
}


/*
 * Roll some dice.
 */
int dice (int number, int size)
{
	int idice;
	int sum;
	
	switch (size)
	{
	case 0:
		return 0;
	case 1:
		return number;
	}
	
	for (idice = 0, sum = 0; idice < number; idice++)
		sum += number_range (1, size);
	
	return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate (int level, int value_00, int value_32)
{
	return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde (char *str)
{
	for (; *str != '\0'; str++)
	{
		if (*str == '~')
			*str = '-';
	}
	
	return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp (const char *astr, const char *bstr)
{
	if (astr == NULL)
	{
		bugf ("[main.c::str_cmp] null astr. Bstr=[%s]", bstr ? bstr : "null");
		return TRUE;
	}
	
	if (bstr == NULL)
	{
		bugf ("[main.c::str_cmp] null bstr. Astr=[%s]", astr ? astr : "null");
		return TRUE;
	}
	
	for (; *astr || *bstr; astr++, bstr++)
	{
		if (LOWER (*astr) != LOWER (*bstr))
			return TRUE;
	}
	
	return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix (const char *astr, const char *bstr)
{
	if (IS_NULLSTR (astr))
		return TRUE;
	
	if (IS_NULLSTR (bstr))
		return TRUE;
	
	for (; *astr; astr++, bstr++)
	{
		if (LOWER (*astr) != LOWER (*bstr))
			return TRUE;
	}
	
	return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix (const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;
	int ichar;
	char c0;
	
	if (astr == NULL)
	{
		bugf ("[main.c::str_infix] null astr. Bstr=[%s]", bstr ? bstr : "null");
		return TRUE;
	}
	
	if (bstr == NULL)
	{
		bugf ("[main.c::str_infix] null bstr. Astr=[%s]", astr ? astr : "null");
		return TRUE;
	}
	
	if ((c0 = LOWER (astr[0])) == '\0')
		return FALSE;
	
	sstr1 = strlen (astr);
	sstr2 = strlen (bstr);
	
	for (ichar = 0; ichar <= sstr2 - sstr1; ichar++)
	{
		if (c0 == LOWER (bstr[ichar]) && !str_prefix (astr, bstr + ichar))
			return FALSE;
	}
	
	return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix (const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;
	
	if (astr == NULL)
	{
		bugf ("[main.c::str_sufix] null astr. Bstr=[%s]", bstr ? bstr : "null");
		return TRUE;
	}
	
	if (bstr == NULL)
	{
		bugf ("[main.c::str_sufix] null bstr. Astr=[%s]", astr ? astr : "null");
		return TRUE;
	}
	
	sstr1 = strlen (astr);
	sstr2 = strlen (bstr);
	if (sstr1 <= sstr2 && !str_cmp (astr, bstr + sstr2 - sstr1))
		return FALSE;
	else
		return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize (const char *str)
{
	static char strcap[MSL];
	int i;
	
	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = LOWER (str[i]);
	strcap[i] = '\0';
	strcap[0] = UPPER (strcap[0]);
	return strcap;
}


/*
 * Append a string to a file.
 */
void append_file (CHAR_DATA * ch, char *file, char *str)
{
	FILE *fp;
	
	if ((ch && IS_NPC (ch)) || str[0] == '\0')
		return;
	
	if ((fp = fopen (file, "a")) == NULL)
	{
		bugf("[main.c::append_file] Unable to open file '%s' for writing.", file);
		if (ch)
			send_to_char ("Could not open the file!\n\r", ch);
	}
	else
	{
		if (ch)
			fprintf (fp, "[%5ld] %s: %s\n",
			ch->in_room ? ch->in_room->vnum : 0, ch->name, str);
		else
			fprintf (fp, "System Call: %s\n", str);
		
		fclose (fp);
	}
	
	return;
}


void chprintf(CHAR_DATA * ch, char *fmt, ...)
{
	char buf[MSL];
	va_list args;
	va_start (args, fmt);
#if !defined(_WIN32)
	vsnprintf (buf, MSL, fmt, args);
#else
	_vsnprintf(buf, MSL, fmt, args);
#endif
	va_end (args);
	
	send_to_char (buf, ch);
}

void printf_to_buf (BUFFER * temp, char *fmt, ...)
{
	char buf[MSL];
	va_list args;
	va_start (args, fmt);
#if !defined(_WIN32)
	vsnprintf (buf, MSL, fmt, args);
#else
	_vsnprintf(buf, MSL, fmt, args);
#endif
	va_end (args);
	
	add_buf (temp, buf);
}

void bugf (char *fmt, ...)
{
	char buf[MSL], buf2[MSL];
	va_list args;
	va_start (args, fmt);
#if !defined(_WIN32)
	vsnprintf (buf, MSL, fmt, args);
#else
	_vsnprintf(buf, MSL, fmt, args);
#endif
	va_end (args);
	
	if (fpArea != NULL)
	{
		int iLine;
		int iChar;
		
		if (fpArea == stdin)
		{
			iLine = 0;
		}
		else
		{
			iChar = ftell (fpArea);
			fseek (fpArea, 0, 0);
			for (iLine = 0; ftell (fpArea) < iChar; iLine++)
			{
				while (getc (fpArea) != '\n')
					;
			}
			fseek (fpArea, iChar, 0);
		}
		
		sprintf (buf, "[*****] FILE: %s LINE: %d", strArea, iLine);
		log_string (buf);
	}
	
	sprintf(buf2, "[*BUG*] %s", buf);
	log_string (buf2);
	wiznet (buf2, NULL, NULL, WIZ_BUGS, 0, 0);
}

void logf (char *fmt, ...)
{
	char buf[2 * MSL];
	va_list args;
	va_start (args, fmt);
#if !defined(_WIN32)
	vsnprintf (buf, 2*MSL, fmt, args);
#else
	_vsnprintf(buf, 2*MSL, fmt, args);
#endif
	va_end (args);
	
	log_string (buf);
}

#define debugTRACE

void _trace(bool logTrace, char *fmt, ...)
{
	char buf[2 * MSL];
	va_list args;
	va_start (args, fmt);

	#if !defined(debugTRACE)
		return;
	#endif

	#if !defined(_WIN32)
		vsnprintf (buf, 2*MSL, fmt, args);
	#else
		_vsnprintf(buf, 2*MSL, fmt, args);
	#endif
	va_end (args);
	
	wiznet (buf, NULL, NULL, WIZ_TRACE, 0, 0);

	if ( logTrace )
	  log_string (buf);
}



/*
 * Writes a string to the log.
 */
void log_string (const char *str)
{
	char *strtime;
	char buf[1024];
	
	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';

	sprintf(buf, "%s : %s\n", strtime, str);


#ifdef _WIN32
	OutputDebugString(buf);
#endif

	if ( fpErr )
  	  fprintf (fpErr, buf);
	else
  	  fprintf (stderr, buf);
	return;
}

int bit_array_convert (char letter)
{
	int bitpos = 0;
	
	if ('A' <= letter && letter <= 'Z')
		bitpos = letter - 65;
	else if ('a' <= letter && letter <= 'f')
		bitpos = letter - 71;
	
	return bitpos;
}

void fread_bits (FILE * fp, SET bit_array)
{
	int number = 0, in_byte = 0;
	char c;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	if (!isdigit (c) || c != 0 || c != 'x' || c != '|')
	{
		while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'y') || (c == '0') || (c == '|'))
		{
			if (c == '0')
			{
				c = getc (fp);
				continue;
			}
			if (c == 'x' || c == '|')
			{
				++in_byte;
				c = getc (fp);
				continue;
			}
			number = bit_array_convert (c);
			SET_BIT (bit_array[in_byte], (1 << (number)));
			c = getc (fp);
		}
	}
	
	else if (c != ' ')
		ungetc (c, fp);
	
}

int fread_bit_array_bit (FILE * fp)
{
	int number = NOT_SET, in_byte = 0;
	char c;
	
	do
	{
		c = getc (fp);
	}
	while (isspace (c));
	
	if (!isdigit (c) || c != 0 || c != 'x' || c != '|')
	{
		while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'y') || (c == '0') || (c == '|'))
		{
			if (c == '0')
			{
				c = getc (fp);
				continue;
			}
			if (c == 'x' || c == '|')
			{
				++in_byte;
				c = getc (fp);
				continue;
			}
			number = bit_array_convert (c) + in_byte * UL_BITS;
			c = getc (fp);
		}
	}
	else if (c != ' ')
		ungetc (c, fp);
	
	return number;
	
}

char *print_bits (SET bits)
{
	int count, x, pos = 0;
	static char buf[52];
	unsigned long flag;
	bool next = FALSE;
	
	for (x = 0; x < MAXELEMS / UL_BITS; x++)
	{
		flag = bits[x];
		
		for (count = 0; count < UL_BITS; count++)
		{
			if (IS_SET (flag, 1 << count))
			{
				if (count < 26)
					buf[pos] = 'A' + count;
				else
					buf[pos] = 'a' + (count - 26);
				pos++;
			}
		}
		if (!next)
		{
			next = TRUE;
			buf[pos++] = '|';
		}
		else
		{
			next = FALSE;
			if (x == MAXELEMS / UL_BITS - 1)
				buf[pos++] = '\0';
			else
				buf[pos++] = '|';
		}
		
	}
	
	return buf;
}


char *print_flags (int flag)
{
	int count, pos = 0;
	static char buf[52];
	
	
	for (count = 0; count < 32; count++)
	{
		if (IS_SET (flag, 1 << count))
		{
			if (count < 26)
				buf[pos] = 'A' + count;
			else
				buf[pos] = 'a' + (count - 26);
			pos++;
		}
	}
	
	if (pos == 0)
	{
		buf[pos] = '0';
		pos++;
	}
	
	buf[pos] = '\0';
	
	return buf;
}


/*
* This function is here to aid in debugging.
* If the last expression in a function is another function call,
* gcc likes to generate a JMP instead of a CALL.
* This is called "tail chaining."
* It hoses the debugger call stack for that call.
* So I make this the last call in certain critical functions,
* where I really need the call stack to be right for debugging!
*
* If you don't understand this, then LEAVE IT ALONE.
* Don't remove any calls to tail_chain anywhere.
*
* -- Furey
*/
void tail_chain (void)
{
	return;
}


float fread_float(FILE *fp)
{
	char number[MSL]; 
	int counter = 0;
	char c;
	

	memset(number, 0, MSL);

	do
	{
		c = getc (fp);
	}
	while (isspace (c));

	if ( ( !isdigit(c) && (c != '.' && c != '-' && c != '+') ) )
	{
		bugf ("[main.c::fread_float] expected a digit, received a '%c'", c);
		if (AttemptJump)		/* bad pfile */
		{
			longjmp (jump_env, 1);
		}
		else
			_quit(ERR_ABORT);
	}
	
	while ( (isdigit (c) || c == '.' || c == '+' || c == '-' ) )
	{
		number[counter++] = c;
		c = getc (fp);

		if (counter > MSL-1)
			break;
	}
	
	
    if (c != ' ')
		ungetc (c, fp);
	
	return atof(number);
}
