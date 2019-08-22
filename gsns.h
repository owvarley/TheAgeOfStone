
#ifdef IN_MAIN_C
  #ifdef GSN
  #undef GSN
  #endif
#define GSN(gsn) sh_int gsn

#else
  #ifdef GSN
  #undef GSN
  #endif
#define GSN(gsn) extern sh_int gsn
#endif

/*
 * Global Skill Numbers 
 */

/* Weapon skills */
GSN( gsn_axe );
GSN( gsn_dagger );
GSN( gsn_flail );
GSN( gsn_mace );
GSN( gsn_polearm );
GSN( gsn_spear );
GSN( gsn_staff );
GSN( gsn_sword );
GSN( gsn_whip );
GSN( gsn_bow );


/* Combat skills */
GSN( gsn_lacerate );
GSN( gsn_maneuvers );
GSN( gsn_auto_dodge );
GSN( gsn_fourth_attack );
GSN( gsn_fifth_attack );
GSN( gsn_dual_wield );
GSN( gsn_speardance );
GSN( gsn_backstab );
GSN( gsn_dodge );
GSN( gsn_hide );
GSN( gsn_sneak );
GSN( gsn_disarm );
GSN( gsn_enhanced_damage );
GSN( gsn_kick );
GSN( gsn_parry );
GSN( gsn_rescue );
GSN( gsn_second_attack );
GSN( gsn_third_attack );
GSN( gsn_shield_block );
GSN( gsn_bash );
GSN( gsn_berserk );
GSN( gsn_dirt );
GSN( gsn_hand_to_hand );
GSN( gsn_trip );
GSN( gsn_fast_healing );
GSN( gsn_meditation );
GSN( gsn_seize );
GSN( gsn_wrath );
GSN( gsn_treat );
GSN( gsn_embrace );
GSN( gsn_riposte );
GSN( gsn_critical );
GSN( gsn_brawl );
GSN( gsn_cloak );
GSN( gsn_frenzy );
GSN( gsn_feral );
GSN( gsn_warfare );
GSN( gsn_martyrdom );
GSN( gsn_guardcrush );
GSN( gsn_tumbling );
GSN( gsn_juggling );
GSN( gsn_engrave );
GSN( gsn_mend );
GSN( gsn_hone );
GSN( gsn_hurl );

/* Talents and Specialties */
GSN( gtn_healing );
GSN( gtn_travelling );
GSN( gtn_tree_singing );
GSN( gtn_dreaming );
GSN( gtn_unarmed_combat );
GSN( gtn_knives );
GSN( gtn_physical_defense );
GSN( gtn_defensive_weaves );
GSN( gtn_air );
GSN( gtn_earth );
GSN( gtn_water );
GSN( gtn_fire );
GSN( gtn_spirit );
GSN( gtn_acrobatics );
GSN( gtn_blacksmithing );


GSN( gsn_forage );
