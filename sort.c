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
 * @@ Objective (sort.c)
 *
 * NOTE: Sorting the command table can be a dangerous thing. Many of the
 * commands in the table are in a specific order, and sorting it will have
 * an affect on these commands.
 * ALSO: Sorting the skill table.  In db.c there's a section in the boot_db()
 * function which determines and sets the gsns.  Be SURE you sort the skill
 * table BEFORE assigning gsns!
 *
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"

#ifdef Key
#undef Key
#endif

#define Key(A)	(A)
#define less(A, B) (Key(A) < Key(B))
#define exch(A, B) { HELP_DATA t = A; A = B; B = t; }
#define compexch(A, B) if (less(B, A)) exch(A, B)

/*
int strcasecmp(char *s1, char *s2)
{
   char b1[MSL], b2[MSL];
   int i;
   
   memset(b1, 0, MSL);
   memset(b2, 0, MSL);
	
   sprintf(b1, "%s", s1);
   sprintf(b2, "%s", s2);
   
   for (i=0; b1[i] != '\0'; i++)
     b1[i] = LOWER(b1[i]);
     
   for (i=0; b2[i] != '\0'; i++)
    b2[i] = LOWER(b2[i]);
    
   return strcmp(b1, b2);
}
*/

int srt_socials (const void *p1, const void *p2)
{
  struct social_type soc1 = *(struct social_type *) p1;
  struct social_type soc2 = *(struct social_type *) p2;

  if (soc1.name[0] == '\0')
    return 2;
  else if (soc2.name[0] == '\0')
    return 1;

  return (strcmp (soc1.name, soc2.name));
}

int srt_cmds (const void *p1, const void *p2)
{
  struct cmd_type cmd1 = *(struct cmd_type *) p1;
  struct cmd_type cmd2 = *(struct cmd_type *) p2;

  if (cmd1.name == NULL)
    return 2;
  else if (cmd2.name == NULL)
    return 1;

  return (strcmp (cmd1.name, cmd2.name));
}

int srt_skills (const void *p1, const void *p2)
{
  struct skill_type sk1;
  struct skill_type sk2;

  sk1 = *(struct skill_type *) p1;
  sk2 = *(struct skill_type *) p2;

  if (sk1.name == NULL)
    return 2;
  else if (sk2.name == NULL)
    return 1;

  return (strcmp (sk1.name, sk2.name));
}


void sort_commands (void)
{
  logf ("Size of command table: %d elements.", maxCommands);

  /* qsort command table */

  qsort ((void *) cmd_table, maxCommands, sizeof (cmd_table[0]), srt_cmds);
}

int sort_skills (void)
{
  qsort ((void *) skill_table,
	 sizeof (skill_table) / sizeof (skill_table[0]),
	 sizeof (skill_table[0]), srt_skills);

  return sizeof (skill_table) / sizeof (skill_table[0]);
}

int sort_socials (void)
{
  int i;

  /* qsort social table */

  for (i = 0; social_table[i].name != NULL && social_table[i].name[0] != '\0';
       i++)
    if (social_table[i].name == NULL)
      break;

  qsort ((void *) social_table, i, sizeof (social_table[0]), srt_socials);

  return i;
}


int srt_helps (const void *p1, const void *p2)
{
  struct help_data hlp1 = *(struct help_data *) p1;
  struct help_data hlp2 = *(struct help_data *) p2;

  if (hlp1.keyword[0] == '\0')
    return 2;
  else if (hlp2.keyword[0] == '\0')
    return 1;
  else
    return (strcmp (hlp1.keyword, hlp2.keyword));
}

void hsort(HELP_DATA h[], int l, int r)
{
//  int i, j;
  
//  for(i=l+1;i<=r;i++)
 //   for(j=i;j>l;j--)
 //   if ( _stricmp(h[j-1].keyword, h[j].keyword) > 1 )
 //     exch(h[j-1], h[j]);
}




int sort_helps (void)
{
  struct help_data *helps;
  HELP_DATA *pHelp;
  HELP_DATA *pMark;
  int i = 0;
  int iMark = top_help;

  helps = calloc( top_help, sizeof( helps[0] ) );

  for (pHelp = help_first; pHelp; pHelp = pHelp->next)
    helps[i++] = *pHelp;

//  hsort(helps, 0, top_help-1);

  qsort( helps, top_help-1, sizeof( helps[0] ), srt_helps); 
//  for(i=0; i<top_help;i++)
//  logf("Help #%d is %s", i, helps[i].keyword);

  for (pHelp = help_first; pHelp; pHelp = pMark)
  {
    pMark = pHelp->next;
    if (pHelp)
      free_help (pHelp);
  }

  help_first = NULL;
  help_last = NULL;

  for (i = 0; i < iMark; i++)
  {
    pHelp = new_help ();

    memcpy (pHelp, &helps[i], sizeof (helps[0]));

    if (help_first == NULL)
      help_first = pHelp;
		
    if (help_last != NULL)
      help_last->next = pHelp;
		
     help_last = pHelp;
     pHelp->next = NULL;
		
     pHelp->next_area = NULL;
  }

  top_help = iMark;
  free (helps);
  return iMark;

}
