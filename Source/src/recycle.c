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
//#include <sys/time.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"


/*
 * Globals
 */
extern int top_reset;
extern int top_area;
extern int top_exit;
extern int top_ed;
extern int top_room;
extern int top_mprog_index;
extern int top_weave;
extern int top_sound;
extern int top_auction;

AUCTION_DATA *auction_free;
AUCTION_DATA *auction_last;
BIDDER_DATA *bidder_free;
BIDDER_DATA *bidder_last;
HELP_DATA *help_free;
HELP_DATA *help_last;
AREA_DATA *area_free;
EXTRA_DESCR_DATA *extra_descr_free;
EXIT_DATA *exit_free;
ROOM_INDEX_DATA *room_index_free;
OBJ_INDEX_DATA *obj_index_free;
SHOP_DATA *shop_free;
MOB_INDEX_DATA *mob_index_free;
RESET_DATA *reset_free;
NAME_DATA *name_free;
CHAR_DATA *char_free;
BAN_DATA *ban_free;
DESCRIPTOR_DATA *descriptor_free;
GEN_DATA *gen_data_free;
AFFECT_DATA *affect_free;
OBJ_DATA *obj_free;
MPROG_CODE *mpcode_free;
MPROG_LIST *mprog_free;
PC_DATA *pcdata_free;
BUFFER *buf_free;
WEB_DESCRIPTOR *web_desc_free;


BIDDER_DATA *new_bidder (void)
{
  BIDDER_DATA *bidder;

  if (bidder_free == NULL)
  {
    bidder = alloc_perm (sizeof (*bidder));
  }
  else
  {
    bidder = bidder_free;
    bidder_free = bidder_free->next;
  }

  bidder->max_bid = 0;
  bidder->current_bid = 0;
  bidder->opening_bid = 0;
  bidder->time_opening_bid = 0;

  VALIDATE (bidder);
  return bidder;
}

void free_bidder (BIDDER_DATA * bidder)
{
  if (!IS_VALID (bidder))
    return;

  bidder->next = bidder_free;
  bidder_free = bidder;

  INVALIDATE (bidder);
}


AUCTION_DATA *new_auction (void)
{
  AUCTION_DATA *auction;

  if (auction_free == NULL)
  {
    auction = alloc_perm (sizeof (*auction));
    top_auction++;
  }
  else
  {
    auction = auction_free;
    auction_free = auction_free->next;
  }

  auction->seller = NULL;
  auction->bidders = NULL;
  auction->item = NULL;
  auction->next = NULL;
  auction->notes = &str_empty[0];
  auction->time_opened = 0;
  auction->time_closed = 0;
  auction->number_for_sale = 0;
  auction->auction_number = top_auction;
  auction->auction_id = -1;
  auction->reserve_price = 0;
  auction->opening_price = 0;
  auction->high_bid = 0;
  auction->low_bid = 0;
  auction->expired = TRUE;
  auction->going = 0;
  auction->bid_increment = 1;

  VALIDATE (auction);
  return auction;
}

void free_auction (AUCTION_DATA * auction)
{
  BIDDER_DATA *bidder, *next_bidder;

  if (!IS_VALID (auction))
    return;

  if (auction->notes)
    free_string (auction->notes);

  for (bidder = auction->bidders; bidder != NULL; bidder = next_bidder)
  {
    next_bidder = bidder->next;
    free_bidder (bidder);
  }

  auction->bidders = NULL;
  auction->expired = TRUE;
  auction->seller = NULL;
  auction->item = NULL;

  auction->next = auction_free;
  auction_free = auction;
  INVALIDATE (auction);
}


WEB_DESCRIPTOR *new_web_desc (void)
{
  WEB_DESCRIPTOR *desc;

  if (web_desc_free)
  {
	desc = web_desc_free;
	web_desc_free = web_desc_free->next;
  }
  else
  {
    desc = alloc_perm (sizeof (*desc));
    top_web_desc++;
  }

  desc->next = NULL;
  VALIDATE (desc);

  return desc;
}

void free_web_desc (WEB_DESCRIPTOR * desc)
{
  if (!IS_VALID (desc))
    return;

  desc->next = web_desc_free;
  web_desc_free = desc;
  INVALIDATE (desc);
}


HELP_DATA *new_help (void)
{
  HELP_DATA *help;

  if (help_free)
  {
    help = help_free;
    help_free = help_free->next;
  }
  else
  {
    help = alloc_perm (sizeof (*help));
    top_help++;
  }

  help->next = NULL;

  return help;
}

/* stuff for recycling udata structures */
UPDATE_DATA *udata_free;

UPDATE_DATA *new_udata (void)
{
  static UPDATE_DATA udata_zero;
  UPDATE_DATA *udata;

  if (udata_free == NULL)
    udata = alloc_perm (sizeof (*udata));
  else
  {
    udata = udata_free;
    udata_free = udata_free->next;
  }

  *udata = udata_zero;
  udata->keyword = &str_empty[0];
  udata->information = &str_empty[0];
  VALIDATE (udata);
  return udata;
}

void free_udata (UPDATE_DATA * udata)
{
  if (!IS_VALID (udata))
    return;

  free_string (udata->keyword);
  free_string (udata->information);

  udata->next = udata_free;
  udata_free = udata;
  INVALIDATE (udata);
}

/* stuff for recycling wizlist structures */
WIZ_DATA *wiz_free;

WIZ_DATA *new_wiz (void)
{
  static WIZ_DATA wiz_zero;
  WIZ_DATA *wiz;

  if (wiz_free == NULL)
    wiz = alloc_perm (sizeof (*wiz));
  else
  {
    wiz = wiz_free;
    wiz_free = wiz_free->next;
  }

  *wiz = wiz_zero;
  wiz->name = &str_empty[0];
  wiz->email = &str_empty[0];
  wiz->job = &str_empty[0];
  VALIDATE (wiz);
  return wiz;
}

void free_wiz (WIZ_DATA * wiz)
{
  if (!IS_VALID (wiz))
    return;

  free_string (wiz->name);
  free_string (wiz->email);
  free_string (wiz->job);

  wiz->next = wiz_free;
  wiz_free = wiz;
  INVALIDATE (wiz);
}

void free_help (HELP_DATA * help)
{
  free_string (help->keyword);
  free_string (help->text);
  help->next = help_free;
  help_free = help;
  top_help--;
}



BAN_DATA *new_ban (void)
{
  static BAN_DATA ban_zero;
  BAN_DATA *ban;

  if (ban_free == NULL)
    ban = alloc_perm (sizeof (*ban));
  else
  {
    ban = ban_free;
    ban_free = ban_free->next;
  }

  *ban = ban_zero;
  ban->name = &str_empty[0];
  VALIDATE (ban);
  return ban;
}

void free_ban (BAN_DATA * ban)
{
  if (!IS_VALID (ban))
    return;

  free_string (ban->name);

  ban->next = ban_free;
  ban_free = ban;
  INVALIDATE (ban);
}


DESCRIPTOR_DATA *new_descriptor (void)
{
  static DESCRIPTOR_DATA d_zero;
  DESCRIPTOR_DATA *d;

  if (descriptor_free == NULL)
    d = alloc_perm (sizeof (*d));
  else
  {
    d = descriptor_free;
    descriptor_free = descriptor_free->next;
  }

  *d = d_zero;

  d->connected = CON_GET_NAME;
  d->showstr_head = NULL;
  d->showstr_point = NULL;
  d->outsize = 2000;
  d->outbuf = alloc_mem (d->outsize);


  VALIDATE (d);
  return d;
}

void free_descriptor (DESCRIPTOR_DATA * d)
{
  if (!IS_VALID (d))
    return;

  free_string (d->host);
  free_string (d->username);
  free_string (d->ftp.data);
  free_string (d->ftp.filename);
  free_mem (d->outbuf, d->outsize);
  d->next = descriptor_free;
  descriptor_free = d;
  INVALIDATE (d);
}


GEN_DATA *new_gen_data (void)
{
  static GEN_DATA gen_zero;
  GEN_DATA *gen;

  if (gen_data_free == NULL)
    gen = alloc_perm (sizeof (*gen));
  else
  {
    gen = gen_data_free;
    gen_data_free = gen_data_free->next;
  }
  *gen = gen_zero;

  gen->rerolls = 0;
  gen->skill_chosen = alloc_mem (sizeof (bool) * (maxSkill));
  gen->group_chosen = alloc_mem (sizeof (bool) * (maxGroup));
  
  memset (gen->skill_chosen, 0, maxSkill);
  memset (gen->group_chosen, 0, maxGroup);

  VALIDATE (gen);
  return gen;
}

void free_gen_data (GEN_DATA * gen)
{
  if (!IS_VALID (gen))
    return;

  free_mem (gen->skill_chosen, sizeof(gen->skill_chosen));
  free_mem (gen->group_chosen, sizeof(gen->group_chosen));

  gen->next = gen_data_free;
  gen_data_free = gen;

  INVALIDATE (gen);
}


EXTRA_DESCR_DATA *new_extra_descr (void)
{
  EXTRA_DESCR_DATA *ed;

  if (extra_descr_free == NULL)
  {
    ed = alloc_perm (sizeof (*ed));
    top_ed++;
  }
  else
  {
    ed = extra_descr_free;
    extra_descr_free = extra_descr_free->next;
  }

  ed->keyword = &str_empty[0];
  ed->description = &str_empty[0];

  VALIDATE (ed);
  return ed;
}

void free_extra_descr (EXTRA_DESCR_DATA * ed)
{
  if (!IS_VALID (ed))
    return;

  free_string (ed->keyword);
  free_string (ed->description);

  ed->next = extra_descr_free;
  extra_descr_free = ed;
  top_ed--;

  INVALIDATE (ed);
}

SOUND_DATA *sound_free;

SOUND_DATA *new_sound (void)
{
  static SOUND_DATA sound_zero;
  SOUND_DATA *sound;

  if (sound_free == NULL)
  {
    sound = alloc_perm (sizeof (*sound));
    top_sound++;
  }
  else

  {
    sound = sound_free;
    sound_free = sound_free->next;
  }

  *sound = sound_zero;

  sound->fname = &str_empty[0];
  sound->volume = 100;
  sound->repeats = 1;
  sound->priority = 50;
  sound->type = -1;
  sound->trigger = 0;
  sound->music = FALSE;
  sound->next = NULL;

  VALIDATE (sound);
  return sound;

}

void free_sound (SOUND_DATA * sound)
{
  if (!IS_VALID (sound))
    return;

  if (sound->fname)
    free_string (sound->fname);


  sound->next = sound_free;
  sound_free = sound;
  top_sound--;

  INVALIDATE (sound);
}


NAME_DATA *new_name (void)
{
  static NAME_DATA Nm_zero;
  NAME_DATA *Nm;

  if (name_free == NULL)
  {
    Nm = alloc_perm (sizeof (*Nm));
    top_name++;
  }
  else
  {
    Nm = name_free;
    name_free = name_free->next;
  }

  *Nm = Nm_zero;

  Nm->name = &str_empty[0];
  Nm->reason = &str_empty[0];
  Nm->by = &str_empty[0];
  Nm->by_lvl = 0;
  Nm->date_stamp = 0;
  Nm->next = NULL;

  VALIDATE (Nm);
  return Nm;
}

void free_name (NAME_DATA * Nm)
{
  if (!IS_VALID (Nm))
    return;

  if (Nm->name)
    free_string (Nm->name);

  if (Nm->reason)
    free_string (Nm->reason);

  if (Nm->by)
    free_string (Nm->by);


  Nm->next = name_free;
  name_free = Nm;
  top_name--;

  INVALIDATE (Nm);
}




AFFECT_DATA *new_affect (void)
{
  static AFFECT_DATA af_zero;
  AFFECT_DATA *af;

  if (affect_free == NULL)
  {
    af = alloc_perm (sizeof (*af));
    top_affect++;
  }
  else
  {
    af = affect_free;
    affect_free = affect_free->next;
  }

  *af = af_zero;

  af->owner = 0;

  VALIDATE (af);
  return af;
}


void free_affect (AFFECT_DATA * af)
{
  if (!IS_VALID (af))
    return;

  af->next = affect_free;
  affect_free = af;

  INVALIDATE (af);
}


OBJ_DATA *new_obj (void)
{
  static OBJ_DATA obj_zero;
  OBJ_DATA *obj;

  if (obj_free == NULL)
    obj = alloc_perm (sizeof (*obj));
  else
  {
    obj = obj_free;
    obj_free = obj_free->next;
  }

  *obj = obj_zero;

  obj->sounds = NULL;
  memset(obj->value, 0, 5*sizeof(int));

  VALIDATE (obj);
  return obj;
}

void free_obj (OBJ_DATA * obj)
{
  AFFECT_DATA *paf, *paf_next;
  EXTRA_DESCR_DATA *ed, *ed_next;

  if (!IS_VALID (obj))
    return;

  for (paf = obj->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    free_affect (paf);
  }
  obj->affected = NULL;

  for (ed = obj->extra_descr; ed != NULL; ed = ed_next)
  {
    ed_next = ed->next;
    free_extra_descr (ed);
  }
  obj->extra_descr = NULL;

  if (obj->sounds != NULL)
    free_sound (obj->sounds);

  free_string (obj->name);
  free_string (obj->description);
  free_string (obj->short_descr);
  free_string (obj->owner);

  obj->next = obj_free;
  obj_free = obj;

  INVALIDATE (obj);
}



CHAR_DATA *new_char (void)
{
  static CHAR_DATA ch_zero;
  CHAR_DATA *ch;

  if (char_free == NULL)
    ch = alloc_perm (sizeof (*ch));
  else
  {
    ch = char_free;
    char_free = char_free->next;
  }

  *ch = ch_zero;
  ch->name = &str_empty[0];
  ch->short_descr = &str_empty[0];
  ch->long_descr = &str_empty[0];
  ch->description = &str_empty[0];
  ch->prompt = &str_empty[0];
  ch->prefix = &str_empty[0];
  ch->logon = current_time;
  ch->lines = PAGELEN;
  ch->sounds = NULL;
  ch->version = VERSION;
  ch->level = 0;
  ch->wtimer = 0;
  ch->age = 18 * 365;
  ch->position = POS_STANDING;
  ch->hit = 20;
  ch->max_hit = 20;
  ch->silver = 200;

  memset(ch->armor, 100, 4);
  memset(ch->flow_str.perm, 0, 5);
  memset(ch->flow_str.current,0, 5);
  memset(ch->perm_stat, 13, MAX_STATS);
  memset(ch->mod_stat, 0, MAX_STATS);

  ch->move = 100;
  ch->max_move = 100;
  ch->attacked_from = 0;
  ch->clan = 0;
  ch->wtimer = 0;
  ch->vGhostRoom = -1;

  set_init (ch->states.comm);
  set_init (ch->states.config);
  set_init (ch->states.player);
  set_init (ch->states.penalties);
  set_init (ch->states.wiznet);
  set_init (ch->states.affected_by);
  set_init (ch->states.form);
  set_init (ch->states.parts);
  set_init (ch->states.off);
  set_init (ch->states.imm);
  set_init (ch->states.res);
  set_init (ch->states.vuln);

  ch->race = race_lookup ("human");
  ch->prompt = str_dup ("%c(`c%h`whp `c%a`wab `c%v`wmv)%c ");



  VALIDATE (ch);
  return ch;
}


void free_char (CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;

  if (!IS_VALID (ch))
    return;

  if (IS_NPC (ch))
    mobile_count--;

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj (obj);
  }

  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    affect_remove (ch, paf);
  }

  if (ch->sounds != NULL)
    free_sound (ch->sounds);

  free_string (ch->name);
  free_string (ch->short_descr);
  free_string (ch->long_descr);
  free_string (ch->description);
  free_string (ch->prompt);
  free_string (ch->prefix);

  if (ch->pcdata != NULL)	/* not a mob is it? */
    free_pcdata (ch->pcdata);

  ch->next = char_free;
  char_free = ch;

  INVALIDATE (ch);
  return;
}

PC_DATA *new_pcdata (void)
{

  static PC_DATA pcdata_zero;
  PC_DATA *pcdata;

  if (pcdata_free == NULL)
    pcdata = alloc_perm (sizeof (*pcdata));
  else
  {
    pcdata = pcdata_free;
    pcdata_free = pcdata_free->next;
  }

  *pcdata = pcdata_zero;

  // Ok, 'maxSkill' and 'maxGroup' can change dynamically while a player is
  // connected.  We need to start using this iMemSize value to reference these instead
  // or we are going to start having problems when we change the size of these fields.

  pcdata->iMemSize[MEM_MAXSKILL] = maxSkill;
  pcdata->iMemSize[MEM_MAXGROUP] = maxGroup;

  pcdata->known = malloc(sizeof (int));
  pcdata->bonds = malloc(sizeof (int));
  pcdata->learned = alloc_mem(sizeof (sh_int) * pcdata->iMemSize[MEM_MAXSKILL]);
  pcdata->group_known = alloc_mem(sizeof (bool) * pcdata->iMemSize[MEM_MAXGROUP]);

  pcdata->balance = 0;		
  pcdata->shares = 0;		
  pcdata->peaceful = TRUE;	
  pcdata->insanity = 0;		
  pcdata->rentroom = 0;		
  pcdata->known[0] = 0;
  pcdata->bonds[0] = 0;
  pcdata->r_desc = &str_empty[0];
  pcdata->hidden_as = &str_empty[0];
  pcdata->profile = &str_empty[0];
  pcdata->last_auction_id = 0;
  pcdata->buffer = new_buf ();
  pcdata->confirm_delete = FALSE;
  pcdata->board = &boards[0];	/* default */
  pcdata->pwd = &str_empty[0];
  pcdata->bamfin = &str_empty[0];
  pcdata->bamfout = &str_empty[0];
  pcdata->title = &str_empty[0];
  pcdata->nation = str_dup ("Andor");
  pcdata->city = str_dup ("");
  pcdata->condition[_THIRST] = 48;
  pcdata->condition[_FULL] = 48;
  pcdata->condition[_HUNGER] = 48;
  pcdata->boxrented = 0;
  pcdata->item_box = 0;
  pcdata->security = 0;	
  pcdata->bounty = 0;
  pcdata->insanity = 0;
  pcdata->rentroom = 0;
  pcdata->peaceful = TRUE;
  pcdata->recall_to = -1;
  pcdata->quest_points = 0;

  memset (pcdata->learned, 0, sizeof(sh_int)*pcdata->iMemSize[MEM_MAXSKILL]);
  memset (pcdata->group_known, 0, sizeof(bool)*pcdata->iMemSize[MEM_MAXGROUP]);
  memset (pcdata->talents, 0, MAX_TALENTS);
  memset (pcdata->notoriety, 0, 2);
  memset (pcdata->alias, 0, MAX_ALIAS);
  memset (pcdata->alias_sub, 0, MAX_ALIAS);
  memset (pcdata->todo, 0, MAX_TODO);
  memset (pcdata->ignore, 0, MAX_IGNORE);

  VALIDATE (pcdata);

  return pcdata;
}


void free_pcdata (PC_DATA * pcdata)
{
  int alias;
 
  if (!IS_VALID (pcdata))
    return;

  free_string (pcdata->pwd);
  free_string (pcdata->bamfin);
  free_string (pcdata->bamfout);
  free_string (pcdata->title);
  free_string (pcdata->r_desc);
  free_string (pcdata->nation);
  free_string (pcdata->city);
  free_string (pcdata->hidden_as);
  free_string (pcdata->profile);
  free_buf (pcdata->buffer);


  for (alias = 0; alias < MAX_ALIAS; alias++)
  {
    if (pcdata->alias[alias] != NULL)
      free_string (pcdata->alias[alias]);
    if (pcdata->alias_sub[alias] != NULL)
      free_string (pcdata->alias_sub[alias]);
  }

  free(pcdata->known);
  free(pcdata->bonds);
  free_mem(pcdata->learned, sizeof(sh_int)*pcdata->iMemSize[MEM_MAXSKILL]);
  free_mem(pcdata->group_known, sizeof(bool)*pcdata->iMemSize[MEM_MAXGROUP]);

  pcdata->next = pcdata_free;
  pcdata_free = pcdata;

  INVALIDATE (pcdata);
  return;
}




/* stuff for setting ids */
long last_mob_id;
long last_auction_id;

long get_auction_id (void)
{
  static long last_auction_id;

  if (last_auction_id >= current_time)
    last_auction_id++;
  else
    last_auction_id = current_time;

  return last_auction_id;
}

int get_pc_id (void)
{
  static int last_player_id;

  if (last_player_id >= (int) current_time)
    last_player_id++;
  else
    last_player_id = (int) current_time;

  return last_player_id;
}

long get_mob_id (void)
{
  last_mob_id++;
  return last_mob_id;
}

/* buffer sizes */
const int buf_size[MAX_BUF_LIST] = {
  16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, MAX_BUF_SIZE
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size (int val)
{
  int i;

  for (i = 0; i < MAX_BUF_LIST; i++)
    if (buf_size[i] >= val)
    {
      return buf_size[i];
    }

  return -1;
}

BUFFER *new_buf ()
{
  BUFFER *buffer;

  if (buf_free == NULL)
    buffer = alloc_perm (sizeof (*buffer));
  else
  {
    buffer = buf_free;
    buf_free = buf_free->next;
  }

  buffer->next = NULL;
  buffer->state = BUFFER_SAFE;
  buffer->size = get_size (BASE_BUF);

  buffer->string = alloc_mem (buffer->size);
  buffer->string[0] = '\0';
  VALIDATE (buffer);

  return buffer;
}

BUFFER *new_buf_size (int size)
{
  BUFFER *buffer;

  if (buf_free == NULL)
    buffer = alloc_perm (sizeof (*buffer));
  else
  {
    buffer = buf_free;
    buf_free = buf_free->next;
  }

  buffer->next = NULL;
  buffer->state = BUFFER_SAFE;
  buffer->size = get_size (size);
  if (buffer->size == -1)
  {
    bugf ("[recycle.c::new_buf_size] buffer size %d too large.", size);
    _quit(1);
  }
  buffer->string = alloc_mem (buffer->size);
  buffer->string[0] = '\0';
  VALIDATE (buffer);

  return buffer;
}


void free_buf (BUFFER * buffer)
{
  if (!IS_VALID (buffer))
    return;

  free_mem (buffer->string, buffer->size);
  buffer->string = NULL;
  buffer->size = 0;
  buffer->state = BUFFER_FREED;

  buffer->next = buf_free;
  buf_free = buffer;

  INVALIDATE (buffer);
}


bool add_buf (BUFFER * buffer, char *string)
{
  int len;
  char *oldstr;
  int oldsize;

  oldstr = buffer->string;
  oldsize = buffer->size;

  if (buffer->state == BUFFER_OVERFLOW)	/* don't waste time on bad strings! */
    return FALSE;

  len = strlen (buffer->string) + strlen (string) + 1;

  while (len >= buffer->size)	/* increase the buffer size */
  {
    buffer->size = get_size (buffer->size + 1);
    {
      if (buffer->size == -1)	/* overflow */
      {
	buffer->size = oldsize;
	buffer->state = BUFFER_OVERFLOW;
	bugf ("[recycle.c::add_buf] buffer overflow past size %d", buffer->size);
	return FALSE;
      }
    }
  }

  if (buffer->size != oldsize)
  {
    buffer->string = alloc_mem (buffer->size);

    strcpy (buffer->string, oldstr);
    free_mem (oldstr, oldsize);
  }

  strcat (buffer->string, string);
  return TRUE;
}


void clear_buf (BUFFER * buffer)
{
  buffer->string[0] = '\0';
  buffer->state = BUFFER_SAFE;
}


char *buf_string (BUFFER * buffer)
{
  return buffer->string;
}


MPROG_LIST *new_mprog (void)
{
  static MPROG_LIST mp_zero;
  MPROG_LIST *mp;

  if (mprog_free == NULL)
    mp = alloc_perm (sizeof (*mp));
  else
  {
    mp = mprog_free;
    mprog_free = mprog_free->next;
  }

  *mp = mp_zero;
  mp->vnum = 0;
  mp->trig_type = 0;
  mp->code = str_dup ("");
  VALIDATE (mp);
  return mp;
}

void free_mprog (MPROG_LIST * mp)
{
  if (!IS_VALID (mp))
    return;

  mp->next = mprog_free;
  mprog_free = mp;

  INVALIDATE (mp);
}

RESET_DATA *new_reset_data (void)
{
  RESET_DATA *pReset;

  if (!reset_free)
  {
    pReset = alloc_perm (sizeof (*pReset));
    top_reset++;
  }
  else
  {
    pReset = reset_free;
    reset_free = reset_free->next;
  }

  pReset->next = NULL;
  pReset->command = 'X';
  pReset->arg1 = 0;
  pReset->arg2 = 0;
  pReset->arg3 = 0;
  pReset->arg4 = 0;
  pReset->chance = 100;

  return pReset;
}



void free_reset_data (RESET_DATA * pReset)
{
  pReset->next = reset_free;
  reset_free = pReset;
  return;
}



AREA_DATA *new_area (void)
{
  AREA_DATA *pArea;
  char buf[MIL];

  if (!area_free)
  {
    pArea = alloc_perm (sizeof (*pArea));
    top_area++;
  }
  else
  {
    pArea = area_free;
    area_free = area_free->next;
  }

  pArea->next = NULL;
  pArea->name = str_dup ("New area");
  pArea->area_flags = AREA_ADDED;
  pArea->security = 1;
  pArea->builders = str_dup ("None");
  pArea->min_vnum = 0;
  pArea->max_vnum = 0;
  pArea->age = 0;
  pArea->nplayer = 0;
  pArea->empty = TRUE;		/* ROM patch */
  sprintf (buf, "area%ld.are", pArea->vnum);
  pArea->file_name = str_dup (buf);
  pArea->vnum = top_area - 1;
  pArea->version = 5;
  pArea->sounds = NULL;
  pArea->sector_type = 0;
  return pArea;
}



void free_area (AREA_DATA * pArea)
{
  free_string (pArea->name);
  free_string (pArea->file_name);
  free_string (pArea->builders);
  free_string (pArea->credits);

  if (pArea->sounds)
    free_sound (pArea->sounds);

  pArea->next = area_free->next;
  area_free = pArea;
  return;
}



EXIT_DATA *new_exit (void)
{
  EXIT_DATA *pExit;

  if (!exit_free)
  {
    pExit = alloc_perm (sizeof (*pExit));
    top_exit++;
  }
  else
  {
    pExit = exit_free;
    exit_free = exit_free->next;
  }

  pExit->u1.to_room = NULL;	/* ROM OLC */
  pExit->next = NULL;
  pExit->exit_info = 0;
  pExit->key = 0;
  pExit->keyword = &str_empty[0];
  pExit->description = &str_empty[0];
  pExit->rs_flags = 0;

  return pExit;
}



void free_exit (EXIT_DATA * pExit)
{
  free_string (pExit->keyword);
  free_string (pExit->description);

  pExit->next = exit_free;
  exit_free = pExit;
  return;
}


ROOM_INDEX_DATA *new_room_index (void)
{
  ROOM_INDEX_DATA *pRoom;
  int door;

  if (!room_index_free)
  {
    pRoom = alloc_perm (sizeof (*pRoom));
    top_room++;
  }
  else
  {
    pRoom = room_index_free;
    room_index_free = room_index_free->next;
  }

  pRoom->next = NULL;
  pRoom->people = NULL;
  pRoom->contents = NULL;
  pRoom->extra_descr = NULL;
  pRoom->area = NULL;
  pRoom->sounds = NULL;

  for (door = 0; door < MAX_DIR; door++)
    pRoom->exit[door] = NULL;

  pRoom->name = &str_empty[0];
  pRoom->description = &str_empty[0];
  pRoom->owner = &str_empty[0];
  pRoom->vnum = 0;
  pRoom->room_flags = 0;
  pRoom->light = 0;
  pRoom->sector_type = 0;
  pRoom->clan = -1;
  pRoom->c_class = -1;
  pRoom->heal_rate = 100;
  pRoom->opu_rate = 100;

  return pRoom;
}



void free_room_index (ROOM_INDEX_DATA * pRoom)
{
  int door;
  EXTRA_DESCR_DATA *pExtra, *wExtra;
  RESET_DATA *pReset, *wReset;

  free_string (pRoom->name);
  free_string (pRoom->description);
  free_string (pRoom->owner);

  for (door = 0; door < MAX_DIR; door++)
  {
    if (pRoom->exit[door])
    {
      free_exit (pRoom->exit[door]);
      pRoom->exit[door] = NULL;
    }

  }

  for (pExtra = pRoom->extra_descr; pExtra; pExtra = wExtra)
  {
    wExtra = pExtra->next;
    free_extra_descr (pExtra);
  }

  for (pReset = pRoom->reset_first; pReset; pReset = wReset)
  {
    wReset = pReset->next;
    free_reset_data (pReset);
  }

  if (pRoom->sounds != NULL)
    free_sound (pRoom->sounds);

  pRoom->next = room_index_free;
  room_index_free = pRoom;
  return;
}



SHOP_DATA *new_shop (void)
{
  SHOP_DATA *pShop;
  int buy;

  if (!shop_free)
  {
    pShop = alloc_perm (sizeof (*pShop));
    top_shop++;
  }
  else
  {
    pShop = shop_free;
    shop_free = shop_free->next;
  }

  pShop->next = NULL;
  pShop->keeper = 0;

  for (buy = 0; buy < MAX_TRADE; buy++)
    pShop->buy_type[buy] = 0;

  pShop->profit_buy = 100;
  pShop->profit_sell = 100;
  pShop->open_hour = 0;
  pShop->close_hour = 23;

  return pShop;
}

void free_shop (SHOP_DATA * pShop)
{
  pShop->next = shop_free;
  shop_free = pShop;
  return;
}



OBJ_INDEX_DATA *new_obj_index (void)
{
  OBJ_INDEX_DATA *pObj;
  int value;

  if (!obj_index_free)
  {
    pObj = alloc_perm (sizeof (*pObj));
    top_obj_index++;
  }
  else
  {
    pObj = obj_index_free;
    obj_index_free = obj_index_free->next;
  }

  pObj->next = NULL;
  pObj->extra_descr = NULL;
  pObj->affected = NULL;
  pObj->area = NULL;
  pObj->name = str_dup ("no name");
  pObj->short_descr = str_dup ("(no short description)");
  pObj->description = str_dup ("(no description)");
  pObj->vnum = 0;
  pObj->item_type = ITEM_TRASH;
  pObj->extra_flags = 0;
  pObj->wear_flags = 0;
  pObj->count = 0;
  pObj->weight = 0;
  pObj->cost = 0;
  pObj->material = str_dup ("unknown");	/* ROM */
  pObj->condition = 100;	/* ROM */
  pObj->max_condition = 100;
  pObj->sounds = NULL;

  for (value = 0; value < 5; value++)	/* 5 - ROM */
    pObj->value[value] = 0;

  return pObj;
}


void free_obj_index (OBJ_INDEX_DATA * pObj)
{
  EXTRA_DESCR_DATA *pExtra, *wExtra;
  AFFECT_DATA *pAf, *wAf;

  free_string (pObj->name);
  free_string (pObj->short_descr);
  free_string (pObj->description);

  for (pAf = pObj->affected; pAf; pAf = wAf)
  {
    wAf = pAf->next;
    free_affect (pAf);
  }

  for (pExtra = pObj->extra_descr; pExtra; pExtra = wExtra)
  {
    wExtra = pExtra->next;
    free_extra_descr (pExtra);
  }

  if (pObj->sounds)
    free_sound (pObj->sounds);

  pObj->next = obj_index_free;
  obj_index_free = pObj;
  return;
}



MOB_INDEX_DATA *new_mob_index (void)
{
  MOB_INDEX_DATA *pMob;

  if (!mob_index_free)
  {
    pMob = alloc_perm (sizeof (*pMob));
    top_mob_index++;
  }
  else
  {
    pMob = mob_index_free;
    mob_index_free = mob_index_free->next;
  }

  pMob->next = NULL;
  pMob->spec_fun = NULL;
  pMob->pShop = NULL;
  pMob->area = NULL;
  pMob->sounds = NULL;
  pMob->player_name = str_dup ("no name");
  pMob->short_descr = str_dup ("(no short description)");
  pMob->long_descr = str_dup ("(no long description)\n\r");
  pMob->description = &str_empty[0];
  pMob->vnum = 0;
  pMob->count = 0;
  pMob->killed = 0;
  pMob->sex = 0;
  pMob->level = 0;
  pMob->alignment = 0;
  pMob->hitroll = 0;
  pMob->race = race_lookup ("human");
  pMob->material = str_dup ("unknown");	/* -- Hugin */
  pMob->size = SIZE_MEDIUM;	/* ROM patch -- Hugin */
  pMob->ac[AC_PIERCE] = 0;	/* ROM patch -- Hugin */
  pMob->ac[AC_BASH] = 0;	/* ROM patch -- Hugin */
  pMob->ac[AC_SLASH] = 0;	/* ROM patch -- Hugin */
  pMob->ac[AC_EXOTIC] = 0;	/* ROM patch -- Hugin */
  pMob->hit[DICE_NUMBER] = 0;	/* ROM patch -- Hugin */
  pMob->hit[DICE_TYPE] = 0;	/* ROM patch -- Hugin */
  pMob->hit[DICE_BONUS] = 0;	/* ROM patch -- Hugin */
  pMob->damage[DICE_NUMBER] = 0;	/* ROM patch -- Hugin */
  pMob->damage[DICE_TYPE] = 0;	/* ROM patch -- Hugin */
  pMob->damage[DICE_NUMBER] = 0;	/* ROM patch -- Hugin */
  pMob->start_pos = POS_STANDING;	/*  -- Hugin */
  pMob->default_pos = POS_STANDING;	/*  -- Hugin */
  pMob->wealth = 0;
  pMob->OPR_dice[DICE_NUMBER] = 0;
  pMob->OPR_dice[DICE_TYPE] = 0;
  pMob->OPR_dice[DICE_BONUS] = 0;

  set_init(pMob->states.player);
  SET_FLAG(pMob->states.player, _ACT_NPC);
  set_init (pMob->states.comm);
  set_init (pMob->states.config);
  set_init (pMob->states.player);
  set_init (pMob->states.penalties);
  set_init (pMob->states.wiznet);
  set_init (pMob->states.affected_by);
  set_init (pMob->states.form);
  set_init (pMob->states.parts);
  set_init (pMob->states.off);
  set_init (pMob->states.imm);
  set_init (pMob->states.res);
  set_init (pMob->states.vuln);

  return pMob;
}


void free_mob_index (MOB_INDEX_DATA * pMob)
{
  MPROG_LIST *list, *mp_next;

  free_string (pMob->player_name);
  free_string (pMob->short_descr);
  free_string (pMob->long_descr);
  free_string (pMob->description);

  if (pMob->sounds != NULL)
    free_sound (pMob->sounds);

  for (list = pMob->mprogs; list; list = mp_next)
  {
    mp_next = list->next;
    free_mprog (pMob->mprogs);
  }

  if (pMob->pShop)
    free_shop (pMob->pShop);

  pMob->next = mob_index_free;
  mob_index_free = pMob;
  return;
}


MPROG_CODE *new_mpcode (void)
{
  MPROG_CODE *NewCode;

  if (!mpcode_free)
  {
    NewCode = alloc_perm (sizeof (*NewCode));
    top_mprog_index++;
  }
  else
  {
    NewCode = mpcode_free;
    mpcode_free = mpcode_free->next;
  }

  NewCode->vnum = 0;
  NewCode->code = str_dup ("");
  NewCode->author = str_dup ("");
  NewCode->title = str_dup ("");
  NewCode->next = NULL;

  return NewCode;
}

void free_mpcode (MPROG_CODE * pMcode)
{
  free_string (pMcode->code);
  free_string (pMcode->author);
  free_string (pMcode->title);
  pMcode->next = mpcode_free;
  mpcode_free = pMcode;
  return;
}
