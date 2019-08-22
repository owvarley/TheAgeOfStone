#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#ifndef _PROTOS_H
#define _PROTOS_H

#if defined(_WIN32)
  #pragma once	

  void gettimeofday( struct timeval *t , void *tz);
  #if !defined(_export)
    #define _export __declspec( dllimport )
  #endif
#else
  #if !defined(_export)
    #define _export extern
  #endif
#endif

/* c++wrapper.cpp */
bool init_web_server(int iPort);
void update_web_server();
void shutdown_web_server();


/* maneuvers.c */
_export void do_maneuver(CHAR_DATA *ch, char *argument);
bool execute_maneuver(CHAR_DATA *ch, CHAR_DATA *vch, int maneuver, char *argument);
int lookup_maneuver(char *maneuver);
bool validate_requirements(CHAR_DATA *ch, CHAR_DATA *vch, int maneuver, bool report);
int check_maneuver(CHAR_DATA *ch, long maneuver);




/* act_comm.c */
_export void do_pkquest(CHAR_DATA *ch, char *argument);
_export void do_rpquest(CHAR_DATA *ch, char *argument);
_export void do_delete(CHAR_DATA *ch, char *argument);
_export void do_channels(CHAR_DATA *ch, char *argument);
_export void do_deaf(CHAR_DATA *ch, char *argument);
_export void do_quiet(CHAR_DATA *ch, char *argument);
_export void do_building(CHAR_DATA *ch, char *argument);
_export void do_afk(CHAR_DATA *ch, char *argument);
_export void do_replay(CHAR_DATA *ch, char *argument);
_export void do_color(CHAR_DATA *ch, char *argument);
_export void do_clantalk(CHAR_DATA *ch, char *argument);
_export void do_bondtalk(CHAR_DATA *ch, char *argument);
_export void do_immtalk(CHAR_DATA *ch, char *argument);
void talk_auction(char *argument);
bool is_ignoring(CHAR_DATA *sender, CHAR_DATA *receiver);
void public_ch(CHAR_DATA *ch, char *argument, char *type, const int bitname);
_export void do_sound(CHAR_DATA *ch, char *argument);
_export void do_ooc(CHAR_DATA *ch, char *argument);
_export void do_gossip(CHAR_DATA *ch, char *argument);
_export void do_grats(CHAR_DATA *ch, char *argument);
_export void do_quote(CHAR_DATA *ch, char *argument);
_export void do_question(CHAR_DATA *ch, char *argument);
_export void do_answer(CHAR_DATA *ch, char *argument);
int emphatic(char *argument);
void say_as_you_will(CHAR_DATA *ch, char *argument, char *type);
_export void do_dirsay(CHAR_DATA *ch, char *argument);
_export void do_whisper(CHAR_DATA *ch, char *argument);
void comm_say(CHAR_DATA *ch, char *argument, char *type);
_export void do_osay(CHAR_DATA *ch, char *argument);
_export void do_say(CHAR_DATA *ch, char *argument);
_export void do_shout(CHAR_DATA *ch, char *argument);
_export void do_tell(CHAR_DATA *ch, char *argument);
_export void do_towertalk(CHAR_DATA *ch, char *argument);
_export void do_reply(CHAR_DATA *ch, char *argument);
_export void do_yell(CHAR_DATA *ch, char *argument);
_export void do_emote(CHAR_DATA *ch, char *argument);
_export void do_pmote(CHAR_DATA *ch, char *argument);
_export void do_typo(CHAR_DATA *ch, char *argument);
_export void do_quit(CHAR_DATA *ch, char *argument);
_export void do_save(CHAR_DATA *ch, char *argument);
_export void do_follow(CHAR_DATA *ch, char *argument);
void add_follower(CHAR_DATA *ch, CHAR_DATA *master, bool bNotify);
void stop_follower(CHAR_DATA *ch);
void nuke_pets(CHAR_DATA *ch);
void die_follower(CHAR_DATA *ch);
_export void do_order(CHAR_DATA *ch, char *argument);
_export void do_group(CHAR_DATA *ch, char *argument);
_export void do_split(CHAR_DATA *ch, char *argument);
_export void do_gtell(CHAR_DATA *ch, char *argument);
_export void do_tsay(CHAR_DATA *ch, char *argument);
_export void do_temote(CHAR_DATA *ch, char *argument);
bool is_same_group(CHAR_DATA *ach, CHAR_DATA *bch);
char *makedrunk(char *string, CHAR_DATA *ch);
_export void do_ignore(CHAR_DATA *ch, char *argument);

/* act_info.c */
_export void do_bounties(CHAR_DATA *ch, char *argument);
_export void do_mudftp(CHAR_DATA *ch, char *argument);
_export void do_gllist(CHAR_DATA *ch, char *argument);
_export void do_talents(CHAR_DATA *ch, char *argument);
_export void do_config(CHAR_DATA *ch, char *argument);
_export void do_glance(CHAR_DATA *ch, char *argument);
_export void do_greet(CHAR_DATA *ch, char *argument);
bool knows_char(CHAR_DATA *ch, CHAR_DATA *victim);
char *nation_id(CHAR_DATA *ch);
char *PERS(CHAR_DATA *ch, CHAR_DATA *viewer);
void add_known(CHAR_DATA *ch, CHAR_DATA *victim);
_export void do_places(CHAR_DATA *ch, char *argument);
char *format_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool fShort);
void show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing);
void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch);
void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch, bool showEQ);
void show_char_to_char(CHAR_DATA *list, CHAR_DATA *ch);
bool check_blind(CHAR_DATA *ch);
_export void do_scroll(CHAR_DATA *ch, char *argument);
_export void do_socials(CHAR_DATA *ch, char *argument);
_export void do_motd(CHAR_DATA *ch, char *argument);
_export void do_imotd(CHAR_DATA *ch, char *argument);
_export void do_jobs(CHAR_DATA *ch, char *argument);
_export void do_rules(CHAR_DATA *ch, char *argument);
_export void do_story(CHAR_DATA *ch, char *argument);
void modify_wizlist(CHAR_DATA *ch, char* arg1, char* arg2, char* arg3);
_export void do_wizlist(CHAR_DATA *ch, char *argument);
_export void do_autolist(CHAR_DATA *ch, char *argument);
_export void do_autoassist(CHAR_DATA *ch, char *argument);
_export void do_autoexit(CHAR_DATA *ch, char *argument);
_export void do_autogold(CHAR_DATA *ch, char *argument);
_export void do_autoloot(CHAR_DATA *ch, char *argument);
_export void do_autosac(CHAR_DATA *ch, char *argument);
_export void do_autosplit(CHAR_DATA *ch, char *argument);
_export void do_brief(CHAR_DATA *ch, char *argument);
_export void do_compact(CHAR_DATA *ch, char *argument);
_export void do_show(CHAR_DATA *ch, char *argument);
_export void do_prompt(CHAR_DATA *ch, char *argument);
_export void do_combine(CHAR_DATA *ch, char *argument);
_export void do_fixnote(CHAR_DATA *ch, char *argument);
_export void do_nofollow(CHAR_DATA *ch, char *argument);
_export void do_look(CHAR_DATA *ch, char *argument);
_export void do_read(CHAR_DATA *ch, char *argument);
_export void do_examine(CHAR_DATA *ch, char *argument);
_export void do_exits(CHAR_DATA *ch, char *argument);
_export void do_worth(CHAR_DATA *ch, char *argument);
_export void do_score(CHAR_DATA *ch, char *argument);
_export void do_affects(CHAR_DATA *ch, char *argument);
_export void do_time(CHAR_DATA *ch, char *argument);
_export void do_weather(CHAR_DATA *ch, char *argument);
_export void do_help(CHAR_DATA *ch, char *argument);
_export void do_whois(CHAR_DATA *ch, char *argument);
_export void do_who(CHAR_DATA *ch, char *argument);
_export void do_count(CHAR_DATA *ch, char *argument);
_export void do_inventory(CHAR_DATA *ch, char *argument);
_export void do_equipment(CHAR_DATA *ch, char *argument);
_export void do_compare(CHAR_DATA *ch, char *argument);
_export void do_credits(CHAR_DATA *ch, char *argument);
_export void do_range(CHAR_DATA *ch, char *argument);
_export void do_where(CHAR_DATA *ch, char *argument);
_export void do_consider(CHAR_DATA *ch, char *argument);
void set_title(CHAR_DATA *ch, char *title);
_export void do_title(CHAR_DATA *ch, char *argument);
_export void do_rdesc(CHAR_DATA *ch, char *argument);
_export void do_description(CHAR_DATA *ch, char *argument);
_export void do_report(CHAR_DATA *ch, char *argument);
_export void do_practice(CHAR_DATA *ch, char *argument);
_export void do_wimpy(CHAR_DATA *ch, char *argument);
_export void do_password(CHAR_DATA *ch, char *argument);
int scan_room(CHAR_DATA *ch, const ROOM_INDEX_DATA *room, char *buf);
_export void do_scan(CHAR_DATA *ch, char *argument);
_export void do_lore(CHAR_DATA *ch, char *argument);
char *condition_string( int percent );
void change_wizlist(CHAR_DATA *ch, bool add, int level, char *argument, char *job, char *email);
void update_wizlist(CHAR_DATA *ch, int level, char *job, char *email);
void save_wizlist(void);
void load_wizlist(void);

/* act_move.c */
void move_char(CHAR_DATA *ch, int door, bool follow);
_export void do_push_drag(CHAR_DATA *ch, char *argument, char *verb);
_export void do_push(CHAR_DATA *ch, char *argument);
_export void do_drag(CHAR_DATA *ch, char *argument);
_export void do_north(CHAR_DATA *ch, char *argument);
_export void do_east(CHAR_DATA *ch, char *argument);
_export void do_south(CHAR_DATA *ch, char *argument);
_export void do_west(CHAR_DATA *ch, char *argument);
_export void do_up(CHAR_DATA *ch, char *argument);
_export void do_down(CHAR_DATA *ch, char *argument);
int find_door(CHAR_DATA *ch, char *arg);
_export void do_open(CHAR_DATA *ch, char *argument);
_export void do_close(CHAR_DATA *ch, char *argument);
bool has_key(CHAR_DATA *ch, int key);
_export void do_lock(CHAR_DATA *ch, char *argument);
_export void do_unlock(CHAR_DATA *ch, char *argument);
_export void do_pick(CHAR_DATA *ch, char *argument);
_export void do_stand(CHAR_DATA *ch, char *argument);
_export void do_rest(CHAR_DATA *ch, char *argument);
_export void do_sit(CHAR_DATA *ch, char *argument);
_export void do_sleep(CHAR_DATA *ch, char *argument);
_export void do_raisehood(CHAR_DATA *ch, char *argument);
_export void do_lowerhood(CHAR_DATA *ch, char *argument);
_export void do_lowerhood(CHAR_DATA *ch, char *argument);
_export void do_wake(CHAR_DATA *ch, char *argument);
_export void do_sneak(CHAR_DATA *ch, char *argument);
_export void do_hide(CHAR_DATA *ch, char *argument);
_export void do_visible(CHAR_DATA *ch, char *argument);
_export void do_recall(CHAR_DATA *ch, char *argument);
ROOM_INDEX_DATA *get_random_room(CHAR_DATA *ch);
_export void do_enter(CHAR_DATA *ch, char *argument);
_export void do_stalk(CHAR_DATA *ch, char *argument);

/* act_obj.c */
_export void do_donate(CHAR_DATA *ch, char *argument);
_export void do_draw(CHAR_DATA *ch, char *argument);
_export void do_dislodge(CHAR_DATA *ch, char *argument);
_export void do_restring(CHAR_DATA *ch, char *argument);
bool can_loot(CHAR_DATA *ch, OBJ_DATA *obj);
_export void do_loot(CHAR_DATA *ch, char *argument);
void get_obj(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container);
_export void do_get(CHAR_DATA *ch, char *argument);
_export void do_put(CHAR_DATA *ch, char *argument);
_export void do_drop(CHAR_DATA *ch, char *argument);
_export void do_give(CHAR_DATA *ch, char *argument);
_export void do_butcher(CHAR_DATA *ch, char *argument);
_export void do_envenom(CHAR_DATA *ch, char *argument);
_export void do_fill(CHAR_DATA *ch, char *argument);
_export void do_pour(CHAR_DATA *ch, char *argument);
_export void do_drink(CHAR_DATA *ch, char *argument);
_export void do_eat(CHAR_DATA *ch, char *argument);
bool remove_obj(CHAR_DATA *ch, int iWear, bool fReplace);
void wear_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace);
_export void do_wear(CHAR_DATA *ch, char *argument);
_export void do_remove(CHAR_DATA *ch, char *argument);
_export void do_sacrifice(CHAR_DATA *ch, char *argument);
_export void do_quaff(CHAR_DATA *ch, char *argument);
_export void do_steal(CHAR_DATA *ch, char *argument);
CHAR_DATA *find_keeper(CHAR_DATA *ch);
void obj_to_keeper(OBJ_DATA *obj, CHAR_DATA *ch);
OBJ_DATA *get_obj_keeper(CHAR_DATA *ch, CHAR_DATA *keeper, char *argument);
int get_cost(CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy);
CHAR_DATA *find_stableboy(CHAR_DATA *ch);
_export void do_stable(CHAR_DATA *ch, char *argument);
_export void do_buy(CHAR_DATA *ch, char *argument);
_export void do_list(CHAR_DATA *ch, char *argument);
_export void do_sell(CHAR_DATA *ch, char *argument);
_export void do_value(CHAR_DATA *ch, char *argument);

/* act_wiz.c */
_export void do_mudcfg(CHAR_DATA *ch, char *argument);
_export void do_odata(CHAR_DATA *ch, char *argument);
_export void do_redirect(CHAR_DATA *ch, char *argument);
_export void do_repop(CHAR_DATA *ch, char *argument);
_export void do_state(CHAR_DATA *ch, char *argument);
void display_poof(CHAR_DATA *ch, char *arg);
_export void do_wiznet(CHAR_DATA *ch, char *argument);
void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj, long flag, long flag_skip, int min_level);
_export void do_outfit(CHAR_DATA *ch, char *argument);
_export void do_nochannels(CHAR_DATA *ch, char *argument);
_export void do_smote(CHAR_DATA *ch, char *argument);
_export void do_bamfin(CHAR_DATA *ch, char *argument);
_export void do_bamfout(CHAR_DATA *ch, char *argument);
_export void do_deny(CHAR_DATA *ch, char *argument);
_export void do_disconnect(CHAR_DATA *ch, char *argument);
_export void do_echo(CHAR_DATA *ch, char *argument);
_export void do_recho(CHAR_DATA *ch, char *argument);
_export void do_zecho(CHAR_DATA *ch, char *argument);
_export void do_pecho(CHAR_DATA *ch, char *argument);
ROOM_INDEX_DATA *find_location(CHAR_DATA *ch, char *arg);
_export void do_transfer(CHAR_DATA *ch, char *argument);
_export void do_at(CHAR_DATA *ch, char *argument);
_export void do_goto(CHAR_DATA *ch, char *argument);
_export void do_violate(CHAR_DATA *ch, char *argument);
_export void do_stat(CHAR_DATA *ch, char *argument);
_export void do_rstat(CHAR_DATA *ch, char *argument);
_export void do_ostat(CHAR_DATA *ch, char *argument);
_export void do_mstat(CHAR_DATA *ch, char *argument);
_export void do_vnum(CHAR_DATA *ch, char *argument);
_export void do_mfind(CHAR_DATA *ch, char *argument);
_export void do_ofind(CHAR_DATA *ch, char *argument);
_export void do_owhere(CHAR_DATA *ch, char *argument);
_export void do_mwhere(CHAR_DATA *ch, char *argument);
_export void do_copyover(CHAR_DATA *ch, char *argument);
void copyover_state(CHAR_DATA *ch, char *argument);
void copyover_recover(void);
_export void do_reboo(CHAR_DATA *ch, char *argument);
_export void do_reboot(CHAR_DATA *ch, char *argument);
_export void do_shutdow(CHAR_DATA *ch, char *argument);
_export void do_shutdown(CHAR_DATA *ch, char *argument);
_export void do_protect(CHAR_DATA *ch, char *argument);
_export void do_snoop(CHAR_DATA *ch, char *argument);
_export void do_switch(CHAR_DATA *ch, char *argument);
_export void do_return(CHAR_DATA *ch, char *argument);
bool obj_check(CHAR_DATA *ch, OBJ_DATA *obj);
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone);
_export void do_disguise(CHAR_DATA *ch, char *argument);
_export void do_clone(CHAR_DATA *ch, char *argument);
_export void do_load(CHAR_DATA *ch, char *argument);
_export void do_mload(CHAR_DATA *ch, char *argument);
_export void do_oload(CHAR_DATA *ch, char *argument);
_export void do_purge(CHAR_DATA *ch, char *argument);
_export void do_advance(CHAR_DATA *ch, char *argument);
_export void do_trust(CHAR_DATA *ch, char *argument);
_export void do_restore(CHAR_DATA *ch, char *argument);
_export void do_freeze(CHAR_DATA *ch, char *argument);
_export void do_log(CHAR_DATA *ch, char *argument);
_export void do_noemote(CHAR_DATA *ch, char *argument);
_export void do_noshout(CHAR_DATA *ch, char *argument);
_export void do_notell(CHAR_DATA *ch, char *argument);
_export void do_peace(CHAR_DATA *ch, char *argument);
_export void do_wizlock(CHAR_DATA *ch, char *argument);
_export void do_closemud(CHAR_DATA *ch, char *argument);
_export void do_newlock(CHAR_DATA *ch, char *argument);
_export void do_slookup(CHAR_DATA *ch, char *argument);
_export void do_set(CHAR_DATA *ch, char *argument);
_export void do_sset(CHAR_DATA *ch, char *argument);
_export void do_mset(CHAR_DATA *ch, char *argument);
_export void do_string(CHAR_DATA *ch, char *argument);
_export void do_oset(CHAR_DATA *ch, char *argument);
_export void do_rset(CHAR_DATA *ch, char *argument);
_export void do_sockets(CHAR_DATA *ch, char *argument);
_export void do_force(CHAR_DATA *ch, char *argument);
_export void do_invis(CHAR_DATA *ch, char *argument);
_export void do_incognito(CHAR_DATA *ch, char *argument);
_export void do_holylight(CHAR_DATA *ch, char *argument);
_export void do_prefix(CHAR_DATA *ch, char *argument);
_export void do_flag(CHAR_DATA *ch, char *argument);
_export void do_rename(CHAR_DATA *ch, char *argument);
_export void do_olevel(CHAR_DATA *ch, char *argument);
_export void do_mlevel(CHAR_DATA *ch, char *argument);
_export void do_otype(CHAR_DATA *ch, char *argument);
_export void do_todo(CHAR_DATA *ch, char *argument);
_export void do_update(CHAR_DATA *ch, char *argument);
void load_updates(void);
void save_updates(void);

/* affect_h.c */
void affect_enchant(OBJ_DATA *obj);
void affect_modify(CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd);
AFFECT_DATA *affect_find(AFFECT_DATA *paf, int sn);
void affect_check(CHAR_DATA *ch, int where, int vector);
void affect_to_char(CHAR_DATA *ch, AFFECT_DATA *paf);
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf);
void affect_remove(CHAR_DATA *ch, AFFECT_DATA *paf);
void affect_remove_obj(OBJ_DATA *obj, AFFECT_DATA *paf);
void affect_strip(CHAR_DATA *ch, int sn);
bool is_affected(CHAR_DATA *ch, int sn);
void affect_join(CHAR_DATA *ch, AFFECT_DATA *paf);

/* alias.c */
void substitute_alias(DESCRIPTOR_DATA *d, char *argument);
_export void do_alias(CHAR_DATA *ch, char *argument);
_export void do_unalias(CHAR_DATA *ch, char *argument);

/* ansi.c */
int count_codes(char *str);
bool contains_ansi(char *str);
void cursor_up(CHAR_DATA *ch, int rows);
void cursor_down(CHAR_DATA *ch, int rows);
void cursor_forward(CHAR_DATA *ch, int cols);
void cursor_back(CHAR_DATA *ch, int cols);
void save_cursor(CHAR_DATA *ch);
void restore_cursor(CHAR_DATA *ch);
void erase_eol(CHAR_DATA *ch);
void erase_xyeol(CHAR_DATA *ch, int col, int row);
_export void beep_char(CHAR_DATA *ch, char *argument);
void goto_xy(CHAR_DATA *ch, int col, int row);
void clear_screen(CHAR_DATA *ch);
void send_to_char_xy(CHAR_DATA *ch, int col, int row, char *str);
void send_to_char_center_xy(CHAR_DATA *ch, int row, char *str);

/* areas.c */
void load_bank(void);

/* Weather.c */
void load_time(void);
void time_update(void);
void weather_create(void);
bool load_weather(void);
void weatherstate_update(void);
void tornado(void);
void hurricane(void);
void lightning(void);
void aff_tornado(CHAR_DATA *ch);
void move_weather (OBJ_DATA *obj);
void weather_change(int power, int change);
_export void do_predictweather(CHAR_DATA *ch, char *argument);
bool leads_to (ROOM_INDEX_DATA * from, ROOM_INDEX_DATA * to);



void load_mobiles(FILE *fp);
void load_objects(FILE *fp);
void load_area(FILE *fp);
void assign_area_vnum(long vnum);
void new_reset(ROOM_INDEX_DATA *pR, RESET_DATA *pReset);
void load_resets(FILE *fp);
void load_rooms(FILE *fp);
void load_shops(FILE *fp);
void load_specials(FILE *fp);
void fix_exits(void);
void load_mobprogs(FILE *fp);
void fix_mobprogs(void);

/* auction.c */
_export void do_auction(CHAR_DATA *ch, char *argument);
void auction_update(void);
long advatoi(const char *s);
long parsebet(const long currentbet, const char *argument);

/* ban.c */
void save_bans(void);
void load_bans(void);
bool check_ban(char *site, int type);
void ban_site(CHAR_DATA *ch, char *argument, bool fPerm);
_export void do_ban(CHAR_DATA *ch, char *argument);
_export void do_permban(CHAR_DATA *ch, char *argument);
_export void do_allow(CHAR_DATA *ch, char *argument);

/* bit_h.c */
int bit_value(const struct flag_type *flag_table, const char *flag);
int flag_pos(const struct flag_type *table, char *flag);
bool is_stat(const struct flag_type *flag_table);
int flag_value(const struct flag_type *flag_table, char *argument);
char *bit_string( const struct flag_type *flag_table, SET bits );
char *bit_flags (const struct flag_type *flag_table, long bit);
char *flag_string(const struct flag_type *flag_table, int bits);
char *who_flags(CHAR_DATA *vch);

/* board.c */
void announce_note(CHAR_DATA *sender, BOARD_DATA *board, NOTE_DATA *note);
void free_note(NOTE_DATA *note);
NOTE_DATA *new_note(void);
void finish_note(BOARD_DATA *board, NOTE_DATA *note);
int board_number(const BOARD_DATA *board);
int board_lookup(const char *name);
void save_notes(void);
void load_boards(void);
bool is_note_to(CHAR_DATA *ch, NOTE_DATA *note);
int unread_notes(CHAR_DATA *ch, BOARD_DATA *board);
_export void do_nread(CHAR_DATA *ch, char *argument);
_export void do_nclear(CHAR_DATA *ch, char *argument);
_export void do_note(CHAR_DATA *ch, char *argument);
_export void do_board(CHAR_DATA *ch, char *argument);
void personal_message(const char *sender, const char *to, const char *subject, const int expire_days, const char *text);
void make_note(const char *board_name, const char *sender, const char *to, const char *subject, const int expire_days, const char *text);
void handle_con_note_to(DESCRIPTOR_DATA *d, char *argument);
void handle_con_note_subject(DESCRIPTOR_DATA *d, char *argument);
void handle_con_note_expire(DESCRIPTOR_DATA *d, char *argument);
void note_edit(CHAR_DATA *ch, char **pString);
void note_append(CHAR_DATA *ch, char **pString);
void handle_con_note_text(DESCRIPTOR_DATA *d, char *argument);

/* combat.c */
int  check_form(enum _FORM_TYPE aType, enum _FORM_AFFECTS fType);
void disarm(CHAR_DATA *attacker, CHAR_DATA *defender);
_export void do_berserk(CHAR_DATA *ch, char *argument);
_export void do_bash(CHAR_DATA *ch, char *argument);
_export void do_trip(CHAR_DATA *ch, char *argument);
_export void do_kill(CHAR_DATA *ch, char *argument);
_export void do_murder(CHAR_DATA *ch, char *argument);
bool calculate_initiative (CHAR_DATA * player);
_export void do_rescue(CHAR_DATA *ch, char *argument);
_export void do_kick(CHAR_DATA *ch, char *argument);
_export void do_disarm(CHAR_DATA *ch, char *argument);
void mob_chase(CHAR_DATA *ch);
_export void do_flee(CHAR_DATA *ch, char *argument);
_export void do_surrender(CHAR_DATA *ch, char *argument);
_export void do_fire(CHAR_DATA *ch, char *argument);
_export void do_hurl(CHAR_DATA *ch, char *argument);

/* comm.c */
#if defined(WIN32)
  char *LastErrorStr(void);
#endif
int main(int argc, char **argv);
int init_socket(int port);
void game_loop_unix(int control);
void init_descriptor(int control, ndesc_t type);
void close_descriptor (DESCRIPTOR_DATA * dclose);
void close_socket( int fd );
bool read_from_descriptor(DESCRIPTOR_DATA *d);
void read_from_buffer(DESCRIPTOR_DATA *d);
bool process_ansi_output(DESCRIPTOR_DATA *d);
bool process_output(DESCRIPTOR_DATA *d, bool fPrompt);
void bust_a_prompt(CHAR_DATA *ch);
void write_to_buffer(DESCRIPTOR_DATA *d, const char *txt, int length);
bool write_to_descriptor(DESCRIPTOR_DATA *d, char *text, int length);
bool check_parse_name(char *name);
bool check_reconnect(DESCRIPTOR_DATA *d, char *name, bool fConn);
bool check_playing(DESCRIPTOR_DATA *d, char *name);
void stop_idling(CHAR_DATA *ch);
char *force_wrap(char *oldstring);
void send_to_char(char *txt, CHAR_DATA *ch);
void page_to_char(char *txt, CHAR_DATA *ch);
void show_string(struct descriptor_data *d, char *input);
void fix_sex(CHAR_DATA *ch);
void act(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos);

/* country.c */
_export int nation_count(void);
_export int city_count(void);
_export int city_number(char *argument);
_export int national_number(char *argument);
_export int nation_lookup(char *argument);
_export char *country_lookup(char *argument);
_export char *city_lookup(char *argument);

/* data_h.c */
char *errmsg(int err);
char *append_tilde(char *str);
_export bool save_countries(void);
_export bool load_commands(void);
_export bool save_commands(void);
_export bool save_mud_data(void);
_export bool load_mud_data(void);
_export bool save_names(void);
_export bool load_names(void);
_export bool save_weaves(void);
_export bool load_weaves(void);
_export bool load_liquids(void);
_export bool save_liquids(void);
_export bool load_pc_races(FILE *fp);
_export bool load_races(void);
_export bool save_races(void);
_export bool load_groups(void);
_export bool save_groups(void);
_export bool load_quotes(void);
_export bool save_skill_data(void);
_export bool load_skill_data(void);
_export bool save_class_data(void);
_export bool load_class_data(void);
_export bool save_slay_data(void);
_export bool load_slay_data(void);
_export bool load_countries(void);
_export bool save_guilds(void);
_export bool load_guilds(void);
_export bool save_forms(void);
_export bool load_forms(void);

/* dharvest.c */
OBJ_DATA *qgen(CHAR_DATA *ch, char *argument);
int eq_cost(OBJ_DATA *pObj);
int pobj_eq_cost(OBJ_INDEX_DATA *pObjIdx);
void version_note(long version, bool critical, char *note);
void show_invlist_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShowNothing);
_export void do_dream(CHAR_DATA *ch, char *argument);
_export void do_treesing(CHAR_DATA *ch, char *argument);
_export void last_saved_on(CHAR_DATA *ch, char *argument);
_export void do_fix_weaves(CHAR_DATA *ch, char *argument);
_export void do_qgen(CHAR_DATA *ch, char *argument);
_export void do_xlist(CHAR_DATA *ch, char *argument);
_export void do_invlist(CHAR_DATA *ch, char *argument);
_export void do_unlink_char(CHAR_DATA *ch, char *argument);
_export void do_guild_list(CHAR_DATA *ch, char *arugment);
void last_saved_on(CHAR_DATA *ch, char *argument);
_export void do_msgs(CHAR_DATA *ch, char *argument);
void system_msg(char *msg);
_export void do_checkname(CHAR_DATA *ch, char *argument);
_export void do_petname(CHAR_DATA *ch, char *argument);
_export void do_chown(CHAR_DATA *ch, char *argument);
_export void do_rental(CHAR_DATA *ch, char *argument);
_export void do_checkout(CHAR_DATA *ch, char *argument);
_export void do_setwolf(CHAR_DATA *ch, char *arguments);
_export void do_pflags(CHAR_DATA *ch, char *arguments);
void random_insanity(CHAR_DATA *ch);
_export void do_quotes(CHAR_DATA *ch, char *argument);
_export void do_idle(CHAR_DATA *ch, char *argument);
_export void do_peaceful(CHAR_DATA *ch, char *argument);
_export void do_pload(CHAR_DATA *ch, char *argument);
_export void do_punload(CHAR_DATA *ch, char *argument);
_export void do_release_top(CHAR_DATA *ch, char *argument);
_export void do_seize_top(CHAR_DATA *ch, char *argument);
_export void do_slay(CHAR_DATA *ch, char *argument);
_export void do_bonus(CHAR_DATA *ch, char *argument);
_export void do_vfind(CHAR_DATA *ch, char *argument);

/* disable.c */
_export void do_disable(CHAR_DATA *ch, char *argument);
bool check_disabled(const struct cmd_type *command);
void load_disabled(void);
void save_disabled(void);

/* economy.c */
void convert_coinage(CHAR_DATA *ch, int fee);
_export void do_bank(CHAR_DATA *ch, char *argument);

/* effects.c */
void acid_effect(void *vo, int level, int dam, int target, long owner);
void cold_effect(void *vo, int level, int dam, int target, long owner);
void fire_effect(void *vo, int level, int dam, int target, long owner);
void poison_effect(void *vo, int level, int dam, int target, long owner);
void shock_effect(void *vo, int level, int dam, int target, long owner);

/* fight_h.c */
void violence_update(void);
bool valid_level(CHAR_DATA *ch, CHAR_DATA *victim, bool report);
bool can_fight(CHAR_DATA *ch, bool announce);
void check_assist(CHAR_DATA *ch, CHAR_DATA *victim);
void multi_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt);
void make_attack(CHAR_DATA *ch, CHAR_DATA *victim, int dt);
void one_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool secondary);
bool damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type, bool show, bool check_safe);
void show_dammessage(CHAR_DATA *ch, CHAR_DATA *victim, int dam);
bool apply_damage (CHAR_DATA *ch, CHAR_DATA *victim, int damroll);
bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim);
void update_pos(CHAR_DATA *victim);
void set_fighting(CHAR_DATA *ch, CHAR_DATA *victim);
void stop_fighting(CHAR_DATA *ch, bool fBoth);
void make_corpse(CHAR_DATA *ch);
void death_cry(CHAR_DATA *ch);
void return_ch_to_play(CHAR_DATA *ch);
void raw_kill(CHAR_DATA *victim, bool destroy, char *why_destroyed);
void group_gain(CHAR_DATA *ch, CHAR_DATA *victim);
int xp_compute(CHAR_DATA *gch, CHAR_DATA *victim, int nMembers);
void dam_message(CHAR_DATA *ch, CHAR_DATA *victim, 
                 int dam, int dt, bool immune, int form);

/* gain.c */
void advance_level(CHAR_DATA *ch, bool hide, bool gainLevel);
void gain_exp(CHAR_DATA *ch, int gain);
int hit_gain(CHAR_DATA *ch);
int opu_gain(CHAR_DATA *ch);
int move_gain(CHAR_DATA *ch);
void gain_condition(CHAR_DATA *ch, int iCond, int value);

/* gdskill.c */
bool is_bonded_to(CHAR_DATA *ch, CHAR_DATA *to);
void add_bond(CHAR_DATA *ch, CHAR_DATA *vch);
void delete_bonds(CHAR_DATA *ch, CHAR_DATA *from);
_export void do_warfare(CHAR_DATA *ch, char *argument);
_export void do_martyrdom(CHAR_DATA *ch, char *argument);
_export void do_cloak(CHAR_DATA *ch, char *argument);
_export void do_tiganza(CHAR_DATA *ch, char *argument);
_export void do_wrath(CHAR_DATA *ch, char *argument);
_export void do_blood(CHAR_DATA *ch, char *argument);
_export void do_unbond(CHAR_DATA *ch, char *argument);
_export void do_bond(CHAR_DATA *ch, char *argument);
_export void do_speardance(CHAR_DATA *ch, char *argument);
_export void do_call_wolf(CHAR_DATA *ch, char *argument);
_export void do_reachout(CHAR_DATA *ch, char *argument);
_export void do_circle(CHAR_DATA *ch, char *argument);
_export void do_second(CHAR_DATA *ch, char *argument);
_export void do_bounty(CHAR_DATA *ch, char *argument);
_export void do_entrance_cue(CHAR_DATA *ch, char *argument);
_export void do_exit_cue(CHAR_DATA *ch, char *argument);
_export void do_feral(CHAR_DATA *ch, char *argument);

/* guild.c */
bool is_leader(CHAR_DATA *ch);
bool can_guild(CHAR_DATA *ch);
bool can_deguild(CHAR_DATA *ch);
bool can_promote(CHAR_DATA *ch);
bool can_demote(CHAR_DATA *ch);
bool is_clan(CHAR_DATA *ch);
bool is_same_clan(CHAR_DATA *ch, CHAR_DATA *victim);
_export int clan_lookup(const char *name);
char *player_rank(CHAR_DATA *ch);
char *player_clan(CHAR_DATA *ch);
_export void do_promote(CHAR_DATA *ch, char *argument);
_export void do_guild(CHAR_DATA *ch, char *argument);
_export void do_rostor(CHAR_DATA *ch, char *argument);
void get_roster (CHAR_DATA *ch, FILE *fp, int clan);
void roster_change(CHAR_DATA *victim, int clan, bool add);

/* handler.c */
CHAR_DATA *get_char_by_id(long id);
bool in_same_world(CHAR_DATA *ch, CHAR_DATA *vch);
void add_sets(SET dest, SET source);
void set_init( SET s );
void assign_set(SET dest, SET source);
void extract_mpcode( MPROG_CODE *mpcode );
bool can_channel(CHAR_DATA *ch);
bool is_friend(CHAR_DATA *ch, CHAR_DATA *victim);
bool knows_group(CHAR_DATA* ch, int group);
int count_users(OBJ_DATA *obj);
enum _WEAPON_CLASS weapon_type(const char *name);
char *item_name(int item_type);
char *weapon_name(int weapon_type);
int check_immune(CHAR_DATA *ch, int dam_type);
int get_skill(CHAR_DATA *ch, int sn);
int get_weapon_sn(CHAR_DATA *ch, bool secondary);
int get_weapon_skill(CHAR_DATA *ch, int sn);
void reset_char(CHAR_DATA *ch);
int get_trust(CHAR_DATA *ch);
int get_age(CHAR_DATA *ch);
int get_curr_stat(CHAR_DATA *ch, int stat);
int get_max_stat(CHAR_DATA *ch, int stat);
int get_max_train(CHAR_DATA *ch, int stat);
int can_carry_n(CHAR_DATA *ch);
int can_carry_w(CHAR_DATA *ch);
bool is_name(char *str, char *namelist);
bool is_exact_name(char *str, char *namelist);
void char_from_room(CHAR_DATA *ch);
void char_to_room(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex);
void obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch);
void obj_from_char(OBJ_DATA *obj);
int apply_ac(OBJ_DATA *obj, int iWear, int type);
OBJ_DATA *get_eq_char(CHAR_DATA *ch, int iWear);
int get_angreal_power(CHAR_DATA *ch);
void equip_char(CHAR_DATA *ch, OBJ_DATA *obj, int iWear);
void unequip_char(CHAR_DATA *ch, OBJ_DATA *obj);
int count_obj_list(OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list);
void obj_from_room(OBJ_DATA *obj);
void obj_to_room(OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex);
void obj_to_obj(OBJ_DATA *obj, OBJ_DATA *obj_to);
void obj_from_obj(OBJ_DATA *obj);
void extract_obj(OBJ_DATA *obj);
void extract_char(CHAR_DATA *ch, bool fPull);
CHAR_DATA *get_char_area(CHAR_DATA *ch, char *argument);
CHAR_DATA *get_char_room(CHAR_DATA *ch, char *argument);
CHAR_DATA *get_char_world(CHAR_DATA *ch, char *argument);
OBJ_DATA *get_obj_type(OBJ_DATA *list, OBJ_INDEX_DATA * pObjIndex);
OBJ_DATA *get_obj_list(CHAR_DATA *ch, char *argument, OBJ_DATA *list);
OBJ_DATA *get_obj_carry(CHAR_DATA *ch, char *argument, CHAR_DATA *viewer);
OBJ_DATA *get_obj_wear(CHAR_DATA *ch, char *argument);
OBJ_DATA *get_obj_wear_vnum(CHAR_DATA *ch, long vnum);
OBJ_DATA *get_obj_here(CHAR_DATA *ch, char *argument);
OBJ_DATA *get_obj_world(CHAR_DATA *ch, char *argument);
void deduct_cost(CHAR_DATA *ch, int cost);
OBJ_DATA *create_money(int gold, int silver);
int get_obj_number(OBJ_DATA *obj);
int get_obj_weight(OBJ_DATA *obj);
int get_true_weight(OBJ_DATA *obj);
bool room_is_dark(ROOM_INDEX_DATA *pRoomIndex);
bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room);
bool room_is_private(ROOM_INDEX_DATA *pRoomIndex);
bool can_see_room(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex);
bool can_see(CHAR_DATA *ch, CHAR_DATA *victim);
bool can_see_obj(CHAR_DATA *ch, OBJ_DATA *obj);
bool can_drop_obj(CHAR_DATA *ch, OBJ_DATA *obj);
bool is_full_name(const char *str, char *namelist);
long player_silver(CHAR_DATA *ch);
void amt_to_player(CHAR_DATA *ch, long amt);
bool amt_from_player(CHAR_DATA *ch, long amt);
char * short_to_name (char *short_desc);

/* hunt.c */
_export void do_hunt(CHAR_DATA *ch, char *argument);

/* interp.c */
void interpret(CHAR_DATA *ch, char *argument);
bool check_social(CHAR_DATA *ch, char *command, char *argument);
bool is_number(char *arg);
int number_argument(char *argument, char *arg);
int mult_argument(char *argument, char *arg);
char *parse_arguments(char *argument, char *arg_first, bool keepCaps);
char *one_argument(char *argument, char *arg_first);
char *One_argument(char *argument, char *arg_first);
_export void do_commands(CHAR_DATA *ch, char *argument);
_export void do_wizhelp(CHAR_DATA *ch, char *argument);
void substitute_alias(DESCRIPTOR_DATA *d, char *argument);
_export void do_alias(CHAR_DATA *ch, char *argument);
_export void do_unalias(CHAR_DATA *ch, char *argument);

/* lookup.c */
int form_lookup(char *form);
int form_lookup_sleazy(char *form);
int flag_lookup(const char *name, const struct flag_type *flag_table);
int flag_lookup_exact(const char *name, const struct flag_type *flag_table);
HELP_DATA *help_lookup(char *keyword);
int skill_lookup(const char *name);
int reference_skill(const char *skill);
int group_lookup(const char *name);
int talent_lookup(const char *name);
enum _POS_STATES position_lookup(const char *name);
enum _SEX sex_lookup(const char *name);
int size_lookup(const char *name);
int material_lookup(const char *name);
void init_race_hash_table(void);
void init_skill_hash_table(void);
int race_lookup(const char *race);
int liq_lookup(const char *name);
int weapon_lookup(const char *name);
int item_lookup(const char *name);
int attack_lookup(const char *name);
long wiznet_lookup(const char *name);
int class_lookup(const char *name);

/* main.c */
void _quit(int iError);
void boot_db(void);
void load_helps(FILE *fp, char *fname);
CHAR_DATA *create_mobile(MOB_INDEX_DATA *pMobIndex);
void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone);
OBJ_DATA *create_object(OBJ_INDEX_DATA *pObjIndex, int level);
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone);
void clear_char(CHAR_DATA *ch);
char *get_extra_descr(const char *name, EXTRA_DESCR_DATA *ed);
MOB_INDEX_DATA *get_mob_index(long vnum);
OBJ_INDEX_DATA *get_obj_index(long vnum);
ROOM_INDEX_DATA *get_room_index(long vnum);
MPROG_CODE *get_mprog_index(long vnum);
char fread_letter(FILE *fp);
int fread_number(FILE *fp);
long fread_long(FILE *fp);
long fread_flag(FILE *fp);
long flag_convert(char letter);
char *fread_string(FILE *fp);
char *fread_string_eol(FILE *fp);
void fread_to_eol(FILE *fp);
char *fread_word(FILE *fp);
float fread_float(FILE *fp);
void *alloc_mem(int sMem);
void free_mem(void *pMem, int sMem);
void *alloc_perm(int sMem);
char *str_dup(const char *str);
void free_string(char *pstr);
_export void do_areas(CHAR_DATA *ch, char *argument);
_export void do_memory(CHAR_DATA *ch, char *argument);
_export void do_dump(CHAR_DATA *ch, char *argument);
int number_fuzzy(int number);
int number_range(int from, int to);
int skill_roll (CHAR_DATA *ch, int sn);
int random_dice (int number, int size, int bonus);
int number_percent(void);
int number_door(void);
int number_bits(int width);
void init_mm(void);
long number_mm(void);
int dice(int number, int size);
int interpolate(int level, int value_00, int value_32);
void smash_tilde(char *str);
bool str_cmp(const char *astr, const char *bstr);
bool str_prefix(const char *astr, const char *bstr);
bool str_infix(const char *astr, const char *bstr);
bool str_suffix(const char *astr, const char *bstr);
char *capitalize(const char *str);
void append_file(CHAR_DATA *ch, char *file, char *str);
void chprintf(CHAR_DATA *ch, char *fmt, ...);
void printf_to_buf (BUFFER *temp, char *fmt, ...);
void bugf(char *fmt, ...);
void logf(char *fmt, ...);
void _trace(bool logTrace, char *fmt, ...);
void log_string(const char *str);
void fread_bits ( FILE *fp, SET bit_array );
int  fread_bit_array_bit ( FILE *fp );
char *print_bits(SET bit);
void tail_chain(void);

/* mob_cmds.c */
void mob_interpret(CHAR_DATA *ch, char *argument);
char *mprog_type_to_name(int type);
_export void do_mob(CHAR_DATA *ch, char *argument);
_export void do_mpstat(CHAR_DATA *ch, char *argument);
_export void do_mpdump(CHAR_DATA *ch, char *argument);
_export void do_mpgecho(CHAR_DATA *ch, char *argument);
_export void do_mpzecho(CHAR_DATA *ch, char *argument);
_export void do_mpasound(CHAR_DATA *ch, char *argument);
_export void do_mpkill(CHAR_DATA *ch, char *argument);
_export void do_mpassist(CHAR_DATA *ch, char *argument);
_export void do_mpjunk(CHAR_DATA *ch, char *argument);
_export void do_mpechoaround(CHAR_DATA *ch, char *argument);
_export void do_mpechoat(CHAR_DATA *ch, char *argument);
_export void do_mpecho(CHAR_DATA *ch, char *argument);
_export void do_mpmload(CHAR_DATA *ch, char *argument);
_export void do_mpoload(CHAR_DATA *ch, char *argument);
_export void do_mppurge(CHAR_DATA *ch, char *argument);
_export void do_mpgoto(CHAR_DATA *ch, char *argument);
_export void do_mpat(CHAR_DATA *ch, char *argument);
_export void do_mptransfer(CHAR_DATA *ch, char *argument);
_export void do_mpgtransfer(CHAR_DATA *ch, char *argument);
_export void do_mpforce(CHAR_DATA *ch, char *argument);
_export void do_mpgforce(CHAR_DATA *ch, char *argument);
_export void do_mpvforce(CHAR_DATA *ch, char *argument);
_export void do_mpcast(CHAR_DATA *ch, char *argument);
_export void do_mpdamage(CHAR_DATA *ch, char *argument);
_export void do_mpremember(CHAR_DATA *ch, char *argument);
_export void do_mpforget(CHAR_DATA *ch, char *argument);
_export void do_mpdelay(CHAR_DATA *ch, char *argument);
_export void do_mpcancel(CHAR_DATA *ch, char *argument);
_export void do_mpcall(CHAR_DATA *ch, char *argument);
_export void do_mpflee(CHAR_DATA *ch, char *argument);
_export void do_mpotransfer(CHAR_DATA *ch, char *argument);
_export void do_mpremove(CHAR_DATA *ch, char *argument);
_export void do_mpclone(CHAR_DATA *ch, char *argument);


_export void do_forage(CHAR_DATA *ch, char *argument);

/* mob_prog.c */
int keyword_lookup(const char **table, char *keyword);
int num_eval(int lval, int oper, int rval);
CHAR_DATA *get_random_char(CHAR_DATA *mob);
int count_people_room(CHAR_DATA *mob, int iFlag);
int get_order(CHAR_DATA *ch);
bool has_item(CHAR_DATA *ch, long vnum, sh_int item_type, bool fWear);
bool get_mob_vnum_room(CHAR_DATA *ch, long vnum);
bool get_obj_vnum_room(CHAR_DATA *ch, long vnum);
long cmd_eval(long vnum, char *line, int check, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, CHAR_DATA *rch);
void expand_arg(char *buf, const char *format, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, CHAR_DATA *rch);
void program_flow(long pvnum, char *source, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2);
void mp_act_trigger(char *argument, CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, int type);
bool mp_percent_trigger(CHAR_DATA *mob, CHAR_DATA *ch, const void *arg1, const void *arg2, int type);
void mp_bribe_trigger(CHAR_DATA *mob, CHAR_DATA *ch, int amount);
bool mp_exit_trigger(CHAR_DATA *ch, int dir);
void mp_give_trigger(CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj);
void mp_greet_trigger(CHAR_DATA *ch);
void mp_hprct_trigger(CHAR_DATA *mob, CHAR_DATA *ch);
/* mount.c */
int mount_success(CHAR_DATA *ch, CHAR_DATA *mount, int canattack);
_export void do_mount(CHAR_DATA *ch, char *argument);
_export void do_dismount(CHAR_DATA *ch, char *argument);
/* music.c */
void song_update(void);
void load_songs(void);
_export void do_play(CHAR_DATA *ch, char *argument);
/* names.c */
bool name_is_banned(char *name);
_export void show_banned_list(CHAR_DATA *ch, char *argument);
_export void ban_name(CHAR_DATA *ch, char *argument);
void unban_name(CHAR_DATA *ch, char *argument);
/* nanny.c */
bool get_ability(CHAR_DATA *ch, char *argument);
bool get_impediment(CHAR_DATA *ch, char *argument);
enum _STAT roll_stat(CHAR_DATA *ch, int stat);
void announce_new_player(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_choose_color(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_roll_stats(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_roll_sstats(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_get_new_abilities(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_get_new_class(DESCRIPTOR_DATA *d);
void setstate_default_choice(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_get_new_port(DESCRIPTOR_DATA *d, CHAR_DATA *ch);
void setstate_get_new_sex(DESCRIPTOR_DATA *d);
void setstate_confirm_new_name(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool count_impediment (CHAR_DATA *ch);
bool count_ability (CHAR_DATA *ch);
bool get_name_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_abilities(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_old_password_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool break_connect_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool confirm_new_name_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_password_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool confirm_new_password_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool choose_color_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool choose_channel_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_sex_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_city_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_port_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_class_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool roll_stats_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool get_new_sdesc (DESCRIPTOR_DATA * d, char *argument, CHAR_DATA *ch);
bool roll_sstats_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool default_choice_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool pick_weapon_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
bool gen_groups_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
void read_motd_state(DESCRIPTOR_DATA *d, char *argument, CHAR_DATA *ch);
void nanny(DESCRIPTOR_DATA *d, char *argument);

/* olc.c */
bool run_olc_editor(DESCRIPTOR_DATA *d);
char *olc_ed_name(CHAR_DATA *ch);
char *olc_ed_vnum(CHAR_DATA *ch);
bool show_commands(CHAR_DATA *ch, char *argument);
CMD_DATA *get_cmd_data(int cmd);
CLAN_DATA *get_clan_data(int clan);
SKILL_DATA *get_skill_data(int skill);
GROUP_DATA *get_group_data(int group);
AREA_DATA *get_area_data(long vnum);
bool edit_done(CHAR_DATA *ch);
void gredit(CHAR_DATA *ch, char *argument);
void skedit(CHAR_DATA *ch, char *argument);
void gedit(CHAR_DATA *ch, char *argument);
void aedit(CHAR_DATA *ch, char *argument);
void redit(CHAR_DATA *ch, char *argument);
void oedit(CHAR_DATA *ch, char *argument);
void medit(CHAR_DATA *ch, char *argument);
void wedit(CHAR_DATA *ch, char *argument);
_export void do_olc(CHAR_DATA *ch, char *argument);
_export void do_gredit(CHAR_DATA *ch, char *argument);
_export void do_gedit(CHAR_DATA *ch, char *argument);
_export void do_skedit(CHAR_DATA *ch, char *argument);
_export void do_aedit(CHAR_DATA *ch, char *argument);
_export void do_redit(CHAR_DATA *ch, char *argument);
_export void do_oedit(CHAR_DATA *ch, char *argument);
_export void do_medit(CHAR_DATA *ch, char *argument);
_export void do_wedit(CHAR_DATA *ch, char *argument);
_export void do_cedit(CHAR_DATA *ch, char *argument);
void display_resets(CHAR_DATA *ch);
void add_reset(ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index);
_export void do_resets(CHAR_DATA *ch, char *argument);
_export void do_astat(CHAR_DATA *ch, char *argument);
_export void do_alist(CHAR_DATA *ch, char *argument);

/* olc_act.c */
bool show_version(CHAR_DATA *ch, char *argument);
void show_flag_cmds(CHAR_DATA *ch, const struct flag_type *flag_table);
void show_skill_cmds(CHAR_DATA *ch, int tar);
void show_spec_cmds(CHAR_DATA *ch);
bool show_help(CHAR_DATA *ch, char *argument);
void show_sounds(CHAR_DATA *ch, SOUND_DATA *Sound);
bool redit_sound(CHAR_DATA *ch, char *argument);
bool redit_delete(CHAR_DATA *ch, char *argument);
bool redit_rlist(CHAR_DATA *ch, char *argument);
bool redit_mlist(CHAR_DATA *ch, char *argument);
bool redit_olist(CHAR_DATA *ch, char *argument);
bool redit_mshow(CHAR_DATA *ch, char *argument);
bool redit_oshow(CHAR_DATA *ch, char *argument);
bool check_range(long lower, long upper);
AREA_DATA *get_vnum_area(long vnum);
bool aedit_flag(CHAR_DATA *ch, char *argument);
bool aedit_sector(CHAR_DATA *ch, char *argument);
bool aedit_version(CHAR_DATA *ch, char *argument);
bool aedit_sound(CHAR_DATA *ch, char *argument);
bool aedit_delete(CHAR_DATA *ch, char *argument);
bool aedit_show(CHAR_DATA *ch, char *argument);
bool aedit_reset(CHAR_DATA *ch, char *argument);
bool aedit_create(CHAR_DATA *ch, char *argument);
bool aedit_name(CHAR_DATA *ch, char *argument);
bool aedit_credits(CHAR_DATA *ch, char *argument);
bool aedit_file(CHAR_DATA *ch, char *argument);
bool aedit_age(CHAR_DATA *ch, char *argument);
bool aedit_security(CHAR_DATA *ch, char *argument);
bool aedit_builder(CHAR_DATA *ch, char *argument);
bool aedit_vnum(CHAR_DATA *ch, char *argument);
bool aedit_lvnum(CHAR_DATA *ch, char *argument);
bool aedit_uvnum(CHAR_DATA *ch, char *argument);
bool redit_show(CHAR_DATA *ch, char *argument);
bool redit_copy(CHAR_DATA *ch, char *argument);
bool change_exit(CHAR_DATA *ch, char *argument, int door);
bool redit_north(CHAR_DATA *ch, char *argument);
bool redit_south(CHAR_DATA *ch, char *argument);
bool redit_east(CHAR_DATA *ch, char *argument);
bool redit_west(CHAR_DATA *ch, char *argument);
bool redit_up(CHAR_DATA *ch, char *argument);
bool redit_down(CHAR_DATA *ch, char *argument);
bool redit_ed(CHAR_DATA *ch, char *argument);
bool redit_create(CHAR_DATA *ch, char *argument);
bool redit_name(CHAR_DATA *ch, char *argument);
bool redit_desc(CHAR_DATA *ch, char *argument);
bool redit_heal(CHAR_DATA *ch, char *argument);
bool redit_opu(CHAR_DATA *ch, char *argument);
bool redit_clan(CHAR_DATA *ch, char *argument);
bool redit_class(CHAR_DATA *ch, char *argument);
bool redit_format(CHAR_DATA *ch, char *argument);
bool redit_mreset(CHAR_DATA *ch, char *argument);
bool redit_mplist(CHAR_DATA *ch, char *argument);
int wear_loc(int bits, int count);
int wear_bit(int loc);
void show_obj_values(CHAR_DATA *ch, OBJ_INDEX_DATA *obj);
bool set_obj_values(CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument);
bool oedit_delete(CHAR_DATA *ch, char *argument);
bool oedit_sound(CHAR_DATA *ch, char *argument);
bool oedit_show(CHAR_DATA *ch, char *argument);
bool oedit_copy(CHAR_DATA *ch, char *argument);
bool oedit_addaffect(CHAR_DATA *ch, char *argument);
bool oedit_addapply(CHAR_DATA *ch, char *argument);
bool oedit_delaffect(CHAR_DATA *ch, char *argument);
bool oedit_name(CHAR_DATA *ch, char *argument);
bool oedit_short(CHAR_DATA *ch, char *argument);
bool oedit_long(CHAR_DATA *ch, char *argument);
bool set_value(CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value);
bool oedit_values(CHAR_DATA *ch, char *argument, int value);
bool oedit_value0(CHAR_DATA *ch, char *argument);
bool oedit_value1(CHAR_DATA *ch, char *argument);
bool oedit_value2(CHAR_DATA *ch, char *argument);
bool oedit_value3(CHAR_DATA *ch, char *argument);
bool oedit_value4(CHAR_DATA *ch, char *argument);
bool oedit_weight(CHAR_DATA *ch, char *argument);
bool oedit_cost(CHAR_DATA *ch, char *argument);
bool oedit_create(CHAR_DATA *ch, char *argument);
bool oedit_ed(CHAR_DATA *ch, char *argument);
bool oedit_extra(CHAR_DATA *ch, char *argument);
bool oedit_wear(CHAR_DATA *ch, char *argument);
bool oedit_type(CHAR_DATA *ch, char *argument);
bool oedit_material(CHAR_DATA *ch, char *argument);
bool oedit_level(CHAR_DATA *ch, char *argument);
bool oedit_condition(CHAR_DATA *ch, char *argument);
bool medit_sound(CHAR_DATA *ch, char *argument);
bool medit_delete(CHAR_DATA *ch, char *argument);
bool medit_autoset(CHAR_DATA *ch, char *argument);
bool medit_show(CHAR_DATA *ch, char *argument);
bool medit_copy(CHAR_DATA *ch, char *argument);
bool medit_create(CHAR_DATA *ch, char *argument);
bool medit_spec(CHAR_DATA *ch, char *argument);
bool medit_damtype(CHAR_DATA *ch, char *argument);
bool medit_align(CHAR_DATA *ch, char *argument);
bool medit_level(CHAR_DATA *ch, char *argument);
bool medit_desc(CHAR_DATA *ch, char *argument);
bool medit_long(CHAR_DATA *ch, char *argument);
bool medit_short(CHAR_DATA *ch, char *argument);
bool medit_name(CHAR_DATA *ch, char *argument);
bool medit_shop(CHAR_DATA *ch, char *argument);
bool medit_sex(CHAR_DATA *ch, char *argument);
bool medit_act(CHAR_DATA *ch, char *argument);
bool medit_affect(CHAR_DATA *ch, char *argument);
bool medit_ac(CHAR_DATA *ch, char *argument);
bool medit_form(CHAR_DATA *ch, char *argument);
bool medit_part(CHAR_DATA *ch, char *argument);
bool medit_imm(CHAR_DATA *ch, char *argument);
bool medit_res(CHAR_DATA *ch, char *argument);
bool medit_vuln(CHAR_DATA *ch, char *argument);
bool medit_material(CHAR_DATA *ch, char *argument);
bool medit_off(CHAR_DATA *ch, char *argument);
bool medit_size(CHAR_DATA *ch, char *argument);
bool medit_hitdice(CHAR_DATA *ch, char *argument);
bool medit_opudice(CHAR_DATA *ch, char *argument);
bool medit_damdice(CHAR_DATA *ch, char *argument);
bool medit_race(CHAR_DATA *ch, char *argument);
bool medit_position(CHAR_DATA *ch, char *argument);
bool medit_gold(CHAR_DATA *ch, char *argument);
bool medit_hitroll(CHAR_DATA *ch, char *argument);
void show_liqlist(CHAR_DATA *ch);
void show_damlist(CHAR_DATA *ch);
bool medit_group(CHAR_DATA *ch, char *argument);
bool redit_owner(CHAR_DATA *ch, char *argument);
bool medit_addmprog(CHAR_DATA *ch, char *argument);
bool medit_delmprog(CHAR_DATA *ch, char *argument);
bool redit_room(CHAR_DATA *ch, char *argument);
bool redit_sector(CHAR_DATA *ch, char *argument);
void unlink_reset(ROOM_INDEX_DATA *pRoom, RESET_DATA *pReset);
void unlink_obj_index(OBJ_INDEX_DATA *pObj);
void unlink_room_index(ROOM_INDEX_DATA *pRoom);
void unlink_mob_index(MOB_INDEX_DATA *pMob);
SOUND_DATA *edit_sound(CHAR_DATA *ch, SOUND_DATA *sPtr, char *argument, const struct flag_type *flag_table);

/* olc_group.c */
bool gredit_save(CHAR_DATA *ch, char *argument);
bool gredit_list(CHAR_DATA *ch, char *argument);
bool gredit_delete(CHAR_DATA *ch, char *argument);
bool gredit_show(CHAR_DATA *ch, char *argument);
bool gredit_create(CHAR_DATA *ch, char *argument);
bool gredit_flags(CHAR_DATA *ch, char *argument);
bool gredit_name(CHAR_DATA *ch, char *argument);
bool gredit_ratings(CHAR_DATA *ch, char *argument);
bool gredit_skill(CHAR_DATA *ch, char *argument);

/* olc_guild.c */
bool gedit_flags(CHAR_DATA *ch, char *argument);
bool gedit_rank(CHAR_DATA *ch, char *argument);
bool gedit_skill(CHAR_DATA *ch, char *argument);
bool gedit_create(CHAR_DATA *ch, char *argument);
bool gedit_list(CHAR_DATA *ch, char *argument);
bool gedit_show(CHAR_DATA *ch, char *argument);
bool gedit_name(CHAR_DATA *ch, char *argument);
bool gedit_whoname(CHAR_DATA *ch, char *argument);
bool gedit_rooms(CHAR_DATA *ch, char *argument);
bool gedit_ml(CHAR_DATA *ch, char *argument);

/* olc_help.c */
bool hedit_show(CHAR_DATA *ch, char *argument);
bool hedit_level(CHAR_DATA *ch, char *argument);
bool hedit_keyword(CHAR_DATA *ch, char *argument);
bool hedit_new(CHAR_DATA *ch, char *argument);
bool hedit_text(CHAR_DATA *ch, char *argument);
void hedit(CHAR_DATA *ch, char *argument);
_export void do_hedit(CHAR_DATA *ch, char *argument);
bool hedit_delete(CHAR_DATA *ch, char *argument);
bool hedit_list(CHAR_DATA *ch, char *argument);

/* olc_mpcode.c */
void mpedit(CHAR_DATA *ch, char *argument);
_export void do_mpedit(CHAR_DATA *ch, char *argument);
bool mpedit_create(CHAR_DATA *ch, char *argument);
bool mpedit_show(CHAR_DATA *ch, char *argument);
bool mpedit_code(CHAR_DATA *ch, char *argument);
bool mpedit_author(CHAR_DATA *ch, char *argument);
bool mpedit_title(CHAR_DATA *ch, char *argument);
bool mpedit_copy(CHAR_DATA *ch, char *argument);
bool mpedit_delete(CHAR_DATA *ch, char *argument);

/* olc_save.c */
int load_npc(CHAR_DATA *mob);
char *fix_string(const char *str);
bool save_helps(bool backup);
void save_area_list(bool backup);
char *fwrite_flag(long flags, char buf[]);
void save_mobprogs(FILE *fp, AREA_DATA *pArea);
void save_mobile(FILE *fp, MOB_INDEX_DATA *pMobIndex);
void save_mobiles(FILE *fp, AREA_DATA *pArea);
void save_object(FILE *fp, OBJ_INDEX_DATA *pObjIndex);
void save_objects(FILE *fp, AREA_DATA *pArea);
void save_rooms(FILE *fp, AREA_DATA *pArea);
void save_specials(FILE *fp, AREA_DATA *pArea);
void save_door_resets(FILE *fp, AREA_DATA *pArea);
void save_resets(FILE *fp, AREA_DATA *pArea);
void save_shops(FILE *fp, AREA_DATA *pArea);
void save_area(AREA_DATA *pArea, bool backup);
_export void do_asave(CHAR_DATA *ch, char *argument);

/* olc_skill.c */
bool skedit_flowstr(CHAR_DATA *ch, char *argument);
bool skedit_learn(CHAR_DATA *ch, char *argument);
bool skedit_levels(CHAR_DATA *ch, char *argument);
bool skedit_target(CHAR_DATA *ch, char *argument);
bool skedit_position(CHAR_DATA *ch, char *argument);
bool skedit_wait(CHAR_DATA *ch, char *argument);
bool skedit_msgs(CHAR_DATA *ch, char *argument);
bool skedit_show(CHAR_DATA *ch, char *argument);
bool skedit_save(CHAR_DATA *ch, char *argument);
bool skedit_flags(CHAR_DATA *ch, char *argument);
bool skedit_guild(CHAR_DATA *ch, char *argument);
bool skedit_list(CHAR_DATA *ch, char *argument);

/* olc_weave.c */
bool weave_exists(char *arg);
bool wedit_flowstr(CHAR_DATA *ch, char *argument);
bool wedit_delete(CHAR_DATA *ch, char *argument);
bool wedit_learn(CHAR_DATA *ch, char *argument);
bool wedit_target(CHAR_DATA *ch, char *argument);
bool wedit_position(CHAR_DATA *ch, char *argument);
bool wedit_wait(CHAR_DATA *ch, char *argument);
bool wedit_msgs(CHAR_DATA *ch, char *argument);
bool wedit_create(CHAR_DATA *ch, char *argument);
bool wedit_show(CHAR_DATA *ch, char *argument);
bool wedit_save(CHAR_DATA *ch, char *argument);
bool wedit_flags(CHAR_DATA *ch, char *argument);
bool wedit_name(CHAR_DATA *ch, char *argument);
bool wedit_affects(CHAR_DATA *ch, char *argument);
bool wedit_special(CHAR_DATA *ch, char *argument);
bool wedit_list(CHAR_DATA *ch, char *argument);

/* recycle.c */
AUCTION_DATA *new_auction(void);
void free_auction(AUCTION_DATA *auction);
BIDDER_DATA *new_bidder(void);
void free_bidder(BIDDER_DATA *bidder);
WEB_DESCRIPTOR *new_web_desc(void);
void free_web_desc(WEB_DESCRIPTOR *desc);
HELP_DATA *new_help(void);
void free_help(HELP_DATA *help);
BAN_DATA *new_ban(void);
void free_ban(BAN_DATA *ban);
DESCRIPTOR_DATA *new_descriptor(void);
void free_descriptor(DESCRIPTOR_DATA *d);
GEN_DATA *new_gen_data(void);
void free_gen_data(GEN_DATA *gen);
EXTRA_DESCR_DATA *new_extra_descr(void);
void free_extra_descr(EXTRA_DESCR_DATA *ed);
SOUND_DATA *new_sound(void);
void free_sound(SOUND_DATA *sound);
NAME_DATA *new_name(void);
void free_name(NAME_DATA *Nm);
AFFECT_DATA *new_affect(void);
void free_affect(AFFECT_DATA *af);
OBJ_DATA *new_obj(void);
void free_obj(OBJ_DATA *obj);
CHAR_DATA *new_char(void);
void free_char(CHAR_DATA *ch);
PC_DATA *new_pcdata(void);
void free_pcdata(PC_DATA *pcdata);
long get_auction_id(void);
int get_pc_id(void);
long get_mob_id(void);
int get_size(int val);
BUFFER *new_buf(void);
BUFFER *new_buf_size(int size);
void free_buf(BUFFER *buffer);
bool add_buf(BUFFER *buffer, char *string);
void clear_buf(BUFFER *buffer);
char *buf_string(BUFFER *buffer);
MPROG_LIST *new_mprog(void);
void free_mprog(MPROG_LIST *mp);
RESET_DATA *new_reset_data(void);
void free_reset_data(RESET_DATA *pReset);
AREA_DATA *new_area(void);
void free_area(AREA_DATA *pArea);
EXIT_DATA *new_exit(void);
void free_exit(EXIT_DATA *pExit);
ROOM_INDEX_DATA *new_room_index(void);
void free_room_index(ROOM_INDEX_DATA *pRoom);
SHOP_DATA *new_shop(void);
void free_shop(SHOP_DATA *pShop);
OBJ_INDEX_DATA *new_obj_index(void);
void free_obj_index(OBJ_INDEX_DATA *pObj);
MOB_INDEX_DATA *new_mob_index(void);
void free_mob_index(MOB_INDEX_DATA *pMob);
MPROG_CODE *new_mpcode(void);
void free_mpcode(MPROG_CODE *pMcode);
UPDATE_DATA *new_udata(void);
void free_udata(UPDATE_DATA *udata);
WIZ_DATA * new_wiz(void);
void free_wiz(WIZ_DATA *wiz);

/* rent.c */
_export void do_rentbox(CHAR_DATA *ch, char *argument);
_export void do_deposit(CHAR_DATA *ch, char *argument);
_export void do_withdraw(CHAR_DATA *ch, char *argument);
_export void do_inventory_box(CHAR_DATA *ch, char *argument);
_export void do_box(CHAR_DATA *ch, char *argument);
void obj_to_box(OBJ_DATA *obj, CHAR_DATA *ch);
void obj_from_box(OBJ_DATA *obj);

/* Sails.c */
_export void do_roster(CHAR_DATA *ch, char *argument);

/* save.c */
char *print_flags(int flag);
SOUND_DATA *load_sound(FILE *fp, SOUND_DATA *sPtr);
void save_sounds(FILE *fp, SOUND_DATA *sPtr, char *Key);
void save_char_obj(CHAR_DATA *ch);
void fwrite_char(CHAR_DATA *ch, FILE *fp);
void fwrite_obj(CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, bool is_box);
int load_char_obj(DESCRIPTOR_DATA *d, char *name, bool bIsNPC);
void fread_char(CHAR_DATA *ch, FILE *fp, bool bIsNPC);
void fread_pet(CHAR_DATA *ch, FILE *fp);
void fread_obj(CHAR_DATA *ch, FILE *fp, bool is_box);

/* sedit.c */
void load_social(FILE *fp, struct social_type *social);
void load_social_table(void);
void save_social(const struct social_type *s, FILE *fp);
bool save_social_table(void);
int social_lookup(const char *name);
_export void do_sedit(CHAR_DATA *ch, char *argument);

/* signals.c */
void signal_handler(int sig);
void init_signals(void);
void shutdown_wgrace(int sig);

/* skill_h.c */
bool make_skill_roll(int iRating, float mod);
bool meets_flags(CHAR_DATA *ch, SET flags);
bool meets_reqs(CHAR_DATA *ch, int sn, bool check_level);
bool bad_selection(CHAR_DATA *ch, int table, int sn, bool report);
bool over_limit(CHAR_DATA *ch, int gn, bool report);
_export void do_spells(CHAR_DATA *ch, char *argument);
_export void do_skills(CHAR_DATA *ch, char *argument);
void list_group_costs(CHAR_DATA *ch);
void list_group_chosen(CHAR_DATA *ch);
long exp_per_level(CHAR_DATA *ch, int points);
bool parse_gen_groups(CHAR_DATA *ch, char *argument);
_export void do_groups(CHAR_DATA *ch, char *argument);
void check_improve(CHAR_DATA *ch, int sn, bool success, int multiplier);
void gn_add(CHAR_DATA *ch, int gn);
void gn_remove(CHAR_DATA *ch, int gn);
void group_add(CHAR_DATA *ch, const char *name, bool deduct);
void group_remove(CHAR_DATA *ch, const char *name);
_export void do_train(CHAR_DATA *ch, char *argument);
_export void do_gain(CHAR_DATA *ch, char *argument);

/* skills.c */
_export void do_lacerate(CHAR_DATA *ch, char *argument);
_export void do_treat(CHAR_DATA *ch, char *argument);
_export void do_heal(CHAR_DATA *ch, char *argument);
_export void do_search(CHAR_DATA *ch, char *argument);
bool check_read_stone(CHAR_DATA *ch);
_export void do_mend(CHAR_DATA *ch, char *argument);
_export void do_hone(CHAR_DATA *ch, char *argument);
_export void do_engrave(CHAR_DATA *ch, char *argument);

/* sort.c */
int srt_socials(const void *p1, const void *p2);
int srt_cmds(const void *p1, const void *p2);
int srt_skills(const void *p1, const void *p2);
void sort_commands(void);
int sort_skills(void);
int sort_socials(void);
int srt_helps(const void *p1, const void *p2);
int sort_helps(void);

/* sound_h.c */
char *strsnd(SOUND_DATA *sound);
bool snd_trigger(CHAR_DATA *ch, SOUND_DATA *sPtr, int type, int trigger, bool send);
void snd_door_trigger(CHAR_DATA *ch, int door, bool enter);

/* special.c */
SPEC_FUN *spec_lookup(const char *name);
char *spec_name(SPEC_FUN *function);
bool spec_patrolman(CHAR_DATA *ch);
bool spec_nasty(CHAR_DATA *ch);
bool spec_cast_mage(CHAR_DATA *ch);
bool spec_fido(CHAR_DATA *ch);
bool spec_guard(CHAR_DATA *ch);
bool spec_janitor(CHAR_DATA *ch);
bool spec_poison(CHAR_DATA *ch);
bool spec_thief(CHAR_DATA *ch);

/* string.c */
const char *versionToString(int version);
bool is_allowed(int c);
char *fix_name(char *name, bool restore);
char *pad(char *str, int count, char dir);
char *strip_codes(char *str);
void string_edit(CHAR_DATA *ch, char **pString);
void string_append(CHAR_DATA *ch, char **pString);
char *string_replace(char *orig, char *old, char *s_new);
void string_add(CHAR_DATA *ch, char *argument);
char *format_string(char *oldstring);
char *first_arg(char *argument, char *arg_first, bool fCase);
char *string_unpad(char *argument);
char *string_proper(char *argument);
char *string_linedel(char *string, int line);
char *string_lineadd(char *string, char *newstr, int line);
char *numlineas(char *string);
char *del_last_line(char *string);
char *nolineas(char *string);
char *strcap_color(int length,char *string, ... );
int strlen_color( char *argument );

/* Taos_skills.c */

/* update.c */
void area_update(bool force);
void reset_room(ROOM_INDEX_DATA *pRoom);
void reset_area(AREA_DATA *pArea);
void mobile_update(void);
void weather_update(void);
void bank_update(void);
void char_update(void);
void obj_update(void);
void aggr_update(void);
void update_handler(void);

/* weave_h.c */
bool is_linked(CHAR_DATA * ach, CHAR_DATA * bch);
_export void do_link(CHAR_DATA *ch, char *argument);
_export void do_nearly(CHAR_DATA * ch, char *argument);
int LTAU(CHAR_DATA *ch, int what_sphere);
int LPA(CHAR_DATA *ch, int what_sphere);
int max_rand(int max_random);
int roll_dice(int MaxF);
void refresh_OP(CHAR_DATA *ch, bool restore);
void determine_OP_0(CHAR_DATA *ch, int percentage);
void determine_OP(CHAR_DATA *ch, int percentage);
void deduct_OP(CHAR_DATA *ch, int sn, int percentage);
void increase_OP(CHAR_DATA *ch, int amount, bool perm);
void decrease_OP(CHAR_DATA *ch, int amount, bool perm);
void set_OP(CHAR_DATA *ch, int which, int air, int earth, int fire, int water, int spirit);
int find_spell(CHAR_DATA *ch, const char *name);
void say_spell(CHAR_DATA *ch, int sn_cast);
bool saves_spell(int level, CHAR_DATA *victim, int dam_type);
bool saves_dispel(int dis_level, int spell_level, int duration);
bool check_dispel(int dis_level, CHAR_DATA *victim, int sn);
bool check_burn(CHAR_DATA *ch, int sn);
_export void do_channel(CHAR_DATA *ch, char *argument, bool insane);
_export void do_cast(CHAR_DATA *ch, char *argument);
void obj_cast_spell(int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj);



/* mccp.c */
_export void do_compress(CHAR_DATA *ch, char *argument);
bool compressStart(DESCRIPTOR_DATA *desc);
bool compressEnd(DESCRIPTOR_DATA *desc);
bool processCompressed(DESCRIPTOR_DATA *desc);
bool writeCompressed(DESCRIPTOR_DATA *desc, char *txt, int length);

/* mudftp.c */
void	handle_ftp_data		(DESCRIPTOR_DATA *, const char *);
void	handle_ftp_auth		(DESCRIPTOR_DATA *, const char *);
void	handle_ftp_command	(DESCRIPTOR_DATA *, const char *);
bool	ftp_push       		(DESCRIPTOR_DATA *d);
void 	greet_ftp		(DESCRIPTOR_DATA *d, const char *argument);


_export void weave_armor(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);
_export void weave_bless(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_blindness(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_call_lightning(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_calm(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_sever(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_shielding(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_slice(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_cause_critical(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);  
_export void weave_cause_light(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_cause_serious(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_chain_lightning(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj); 
_export void weave_compell(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_continual_light(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj); 
_export void weave_cloud_dancing(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_create_spring(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_restore_sight(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_cure_critical(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_clear_disease(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_cure_light(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_clear_poison(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_cure_serious(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_detect_evil(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_detect_good(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_detect_hidden(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_detect_invis(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_detect_weave(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_detect_poison(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_shatter_weave(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_earthquake(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_align_armor(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_align_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_energy_drain(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_fireball(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_ward_fire(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_flame_blast(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_floating_disc(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_fly(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);				
_export void weave_frenzy(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_travel(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_enhance_strength(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);
_export void weave_harm(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_haste(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_heal(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_delve(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_invisibility(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_lightning_bolt(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_locate_object(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_mass_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_mass_invisibility(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);
_export void weave_align_phase(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_plague(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_poison(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_skim(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_ward_evil(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_ward_good(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_refresh(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_sleep(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_slow(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_weaken(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_recall(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_sanctum(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_fire_breath(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_rolling_ring(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_flameblade(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_airblade(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_fire_whip(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_air_whip(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_bind(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_gag(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);				
_export void weave_blind(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_create_light(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_remove_curse(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_pummel(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_squeeze(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_firebar(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_steelcloth(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_erode(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_pain(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			
_export void weave_bladestorm(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_air_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_earth_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_fire_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_frost_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);	
_export void weave_ice_shard(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_dehydrate(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);		
_export void weave_scald(int sn, int level, CHAR_DATA *ch, void *vo, int target, char *arg, bool is_obj);			


#endif


#ifdef __cplusplus
}
#endif
