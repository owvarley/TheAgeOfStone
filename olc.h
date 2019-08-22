
/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define OLCVER	"ILAB Online Creation Toolkit for WoT v0.1 -- July '98\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4b4a\n\r"	\
                "     Por Ivan Toledo (pvillanu@choapa.cic.userena.cl)\n\r" \
                "     Modified for WoT Mud by Gary McNickle (gary@tarmongaidon.com)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
				"     (Port a ROM 2.4 - Nov 2, 1996)\n\r"\
				"     (Port for WoT 0.1 - July 24, 1998)\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"



/*
 * New typedefs.
 */
typedef	bool OLC_FUN		(CHAR_DATA *ch, char *argument);
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun


/*
 * Interpreter Prototypes
 */
void    aedit           (CHAR_DATA *ch, char *argument);
void    redit           (CHAR_DATA *ch, char *argument);
void    medit           (CHAR_DATA *ch, char *argument);
void    oedit           (CHAR_DATA *ch, char *argument);
void	mpedit		(CHAR_DATA *ch, char *argument);
void	gedit		(CHAR_DATA *ch, char *argument);
void	hedit		(CHAR_DATA *ch, char *argument);
void	gredit		(CHAR_DATA *ch, char *argument);
void	wedit		(CHAR_DATA *ch, char *argument);
void    skedit		(CHAR_DATA *ch, char *argument);
void 	cedit		(CHAR_DATA *ch, char *argument);

/*
 * OLC Constants
 */
#define MAX_MOB	5		/* Default maximum number for resetting mobs */



/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char * const	name;
    OLC_FUN *		olc_fun;
};



/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};




/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	gedit_table[];
extern const struct olc_cmd_type	hedit_table[];
extern const struct olc_cmd_type        gredit_table[];
extern const struct olc_cmd_type	wedit_table[];
extern const struct olc_cmd_type	skedit_table[];
extern const struct olc_cmd_type	cedit_table[];


/*
 * General Functions
 */
bool show_commands		(CHAR_DATA *ch, char *argument);
bool show_help			(CHAR_DATA *ch, char *argument);
bool edit_done			(CHAR_DATA *ch);
bool show_version		(CHAR_DATA *ch, char *argument);





DECLARE_OLC_FUN( cedit_show	);
DECLARE_OLC_FUN( cedit_delete	);
DECLARE_OLC_FUN( cedit_undelete );
DECLARE_OLC_FUN( cedit_position );
DECLARE_OLC_FUN( cedit_list	);
DECLARE_OLC_FUN( cedit_log	);
DECLARE_OLC_FUN( cedit_disable  );
DECLARE_OLC_FUN( cedit_enable	);
DECLARE_OLC_FUN( cedit_name	);
DECLARE_OLC_FUN( cedit_lookup   );
DECLARE_OLC_FUN( cedit_save	);
DECLARE_OLC_FUN( cedit_reload	);
DECLARE_OLC_FUN( cedit_level	);

/*
 * Skill Editor prototypes
 * by: Gary McNickle
 */
DECLARE_OLC_FUN( skedit_show   	);
DECLARE_OLC_FUN( skedit_save   	);
DECLARE_OLC_FUN( skedit_flags   );
DECLARE_OLC_FUN( skedit_learn	);
DECLARE_OLC_FUN( skedit_target	);
DECLARE_OLC_FUN( skedit_position);
DECLARE_OLC_FUN( skedit_wait	);
DECLARE_OLC_FUN( skedit_msgs	);
DECLARE_OLC_FUN( skedit_list	);
DECLARE_OLC_FUN( skedit_flowstr	);
DECLARE_OLC_FUN( skedit_guild   );
DECLARE_OLC_FUN( skedit_levels  );

/*
 * Weave Editor prototypes
 * by: Gary McNickle
 */

DECLARE_OLC_FUN( wedit_create 	);
DECLARE_OLC_FUN( wedit_show   	);
DECLARE_OLC_FUN( wedit_delete 	);
DECLARE_OLC_FUN( wedit_save   	);
DECLARE_OLC_FUN( wedit_flags    );
DECLARE_OLC_FUN( wedit_name	);
DECLARE_OLC_FUN( wedit_learn	);
DECLARE_OLC_FUN( wedit_target	);
DECLARE_OLC_FUN( wedit_position	);
DECLARE_OLC_FUN( wedit_wait	);
DECLARE_OLC_FUN( wedit_msgs	);
DECLARE_OLC_FUN( wedit_affects	);
DECLARE_OLC_FUN( wedit_special  );
DECLARE_OLC_FUN( wedit_list	);
DECLARE_OLC_FUN( wedit_flowstr	);

/*
 * Group Editor prototypes
 * by: Gary McNickle
 */

DECLARE_OLC_FUN( gredit_create  );
DECLARE_OLC_FUN( gredit_list    );
DECLARE_OLC_FUN( gredit_flags   );
DECLARE_OLC_FUN( gredit_name    );
DECLARE_OLC_FUN( gredit_ratings );
DECLARE_OLC_FUN( gredit_skill   );
DECLARE_OLC_FUN( gredit_show	);
DECLARE_OLC_FUN( gredit_save    );
DECLARE_OLC_FUN( gredit_delete  );

/* Editor de helps */
DECLARE_OLC_FUN( hedit_keyword	);
DECLARE_OLC_FUN( hedit_text	);
DECLARE_OLC_FUN( hedit_new	);
DECLARE_OLC_FUN( hedit_level	);
DECLARE_OLC_FUN( hedit_delete	);
DECLARE_OLC_FUN( hedit_show	);
DECLARE_OLC_FUN( hedit_list	);

/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN( aedit_sector   );
DECLARE_OLC_FUN( aedit_version  );
DECLARE_OLC_FUN( aedit_show	);
DECLARE_OLC_FUN( aedit_create	);
DECLARE_OLC_FUN( aedit_delete   );
DECLARE_OLC_FUN( aedit_name	);
DECLARE_OLC_FUN( aedit_file	);
DECLARE_OLC_FUN( aedit_flag	);
DECLARE_OLC_FUN( aedit_age	);
DECLARE_OLC_FUN( aedit_reset	);
DECLARE_OLC_FUN( aedit_security	);
DECLARE_OLC_FUN( aedit_builder	);
DECLARE_OLC_FUN( aedit_vnum	);
DECLARE_OLC_FUN( aedit_lvnum	);
DECLARE_OLC_FUN( aedit_uvnum	);
DECLARE_OLC_FUN( aedit_credits	);
DECLARE_OLC_FUN( aedit_sound	);

/** Guild Editor Prototypes
  */
DECLARE_OLC_FUN( gedit_flags	);
DECLARE_OLC_FUN( gedit_rank	);
DECLARE_OLC_FUN( gedit_skill	);
DECLARE_OLC_FUN( gedit_show	);
DECLARE_OLC_FUN( gedit_name	);
DECLARE_OLC_FUN( gedit_whoname	);
DECLARE_OLC_FUN( gedit_create	);
DECLARE_OLC_FUN( gedit_delete	);
DECLARE_OLC_FUN( gedit_list	);
DECLARE_OLC_FUN( gedit_rooms    );
DECLARE_OLC_FUN( gedit_ml       );

/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN( redit_copy	);
DECLARE_OLC_FUN( redit_delete   );
DECLARE_OLC_FUN( redit_show	);
DECLARE_OLC_FUN( redit_create	);
DECLARE_OLC_FUN( redit_name	);
DECLARE_OLC_FUN( redit_desc	);
DECLARE_OLC_FUN( redit_ed	);
DECLARE_OLC_FUN( redit_format	);
DECLARE_OLC_FUN( redit_north	);
DECLARE_OLC_FUN( redit_south	);
DECLARE_OLC_FUN( redit_east	);
DECLARE_OLC_FUN( redit_west	);
DECLARE_OLC_FUN( redit_up	);
DECLARE_OLC_FUN( redit_down	);
DECLARE_OLC_FUN( redit_mreset	);
DECLARE_OLC_FUN( redit_sound	);
DECLARE_OLC_FUN( redit_mlist	);
DECLARE_OLC_FUN( redit_rlist	);
DECLARE_OLC_FUN( redit_olist	);
DECLARE_OLC_FUN( redit_mshow	);
DECLARE_OLC_FUN( redit_oshow	);
DECLARE_OLC_FUN( redit_heal	);
DECLARE_OLC_FUN( redit_opu	);
DECLARE_OLC_FUN( redit_clan	);
DECLARE_OLC_FUN( redit_class	);
DECLARE_OLC_FUN( redit_owner	);
DECLARE_OLC_FUN( redit_room	);
DECLARE_OLC_FUN( redit_sector	);
DECLARE_OLC_FUN( redit_mplist	);


/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN( oedit_sound    );
DECLARE_OLC_FUN( oedit_copy	);
DECLARE_OLC_FUN( oedit_delete   );
DECLARE_OLC_FUN( oedit_show	);
DECLARE_OLC_FUN( oedit_create	);
DECLARE_OLC_FUN( oedit_name	);
DECLARE_OLC_FUN( oedit_short	);
DECLARE_OLC_FUN( oedit_long	);
DECLARE_OLC_FUN( oedit_value0	);
DECLARE_OLC_FUN( oedit_value1	);
DECLARE_OLC_FUN( oedit_value2	);
DECLARE_OLC_FUN( oedit_value3	);
DECLARE_OLC_FUN( oedit_value4	);  
DECLARE_OLC_FUN( oedit_weight	);
DECLARE_OLC_FUN( oedit_cost	);
DECLARE_OLC_FUN( oedit_ed	);
DECLARE_OLC_FUN( oedit_extra    );  
DECLARE_OLC_FUN( oedit_wear     );  
DECLARE_OLC_FUN( oedit_type     );  
DECLARE_OLC_FUN( oedit_affect   );  
DECLARE_OLC_FUN( oedit_material	);  
DECLARE_OLC_FUN( oedit_level    );  
DECLARE_OLC_FUN( oedit_condition);  
DECLARE_OLC_FUN( oedit_delaffect);
DECLARE_OLC_FUN( oedit_addapply );
DECLARE_OLC_FUN( oedit_addaffect);

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN( medit_copy     );
DECLARE_OLC_FUN( medit_delete   );
DECLARE_OLC_FUN( medit_show	);
DECLARE_OLC_FUN( medit_create	);
DECLARE_OLC_FUN( medit_name	);
DECLARE_OLC_FUN( medit_short	);
DECLARE_OLC_FUN( medit_long	);
DECLARE_OLC_FUN( medit_shop	);
DECLARE_OLC_FUN( medit_desc	);
DECLARE_OLC_FUN( medit_level	);
DECLARE_OLC_FUN( medit_align	);
DECLARE_OLC_FUN( medit_spec	);

DECLARE_OLC_FUN( medit_sex	);  
DECLARE_OLC_FUN( medit_act	);  
DECLARE_OLC_FUN( medit_affect	);  
DECLARE_OLC_FUN( medit_ac	);  
DECLARE_OLC_FUN( medit_form	);  
DECLARE_OLC_FUN( medit_part	);  
DECLARE_OLC_FUN( medit_imm	);  
DECLARE_OLC_FUN( medit_res	);  
DECLARE_OLC_FUN( medit_vuln	);  
DECLARE_OLC_FUN( medit_material	);  
DECLARE_OLC_FUN( medit_off	);  
DECLARE_OLC_FUN( medit_size	);  
DECLARE_OLC_FUN( medit_sound	);
DECLARE_OLC_FUN( medit_hitdice	);  
DECLARE_OLC_FUN( medit_opudice	);  
DECLARE_OLC_FUN( medit_damdice	);  
DECLARE_OLC_FUN( medit_race	);  
DECLARE_OLC_FUN( medit_position	);  
DECLARE_OLC_FUN( medit_gold	);  
DECLARE_OLC_FUN( medit_hitroll	);  
DECLARE_OLC_FUN( medit_damtype	);  
DECLARE_OLC_FUN( medit_group	);  
DECLARE_OLC_FUN( medit_addmprog	);  
DECLARE_OLC_FUN( medit_delmprog	);  
DECLARE_OLC_FUN( medit_autoset  );  


/* Mobprog editor */
DECLARE_OLC_FUN( mpedit_create	);
DECLARE_OLC_FUN( mpedit_code	);
DECLARE_OLC_FUN( mpedit_show	);
DECLARE_OLC_FUN( mpedit_author	);
DECLARE_OLC_FUN( mpedit_title	);
DECLARE_OLC_FUN( mpedit_copy    );
DECLARE_OLC_FUN( mpedit_delete	);

