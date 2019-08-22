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


#if defined _XOPEN_SOURCE
  #undef _XOPEN_SOURCE
#endif

#define IN_SIGNALS_C

#if !defined(WIN32)
  #include <sys/time.h>
  #include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>
#include "merc.h"

#undef IN_SIGNALS_C


char *const sig_name[] = {
  "(null)",		"SIGHUP",	"SIGINT",		"SIGQUIT",
  "SIGILL",		"SIGTRAP",	"SIGABRT",		"SIGBUS",
  "SIGFPE",		"SIGKILL",	"SIGUSR1",		"SIGSEGV",
  "SIGUSR2",	"SIGPIPE",	"SIGALRM",		"SIGTERM",
  "SIGSTKFLT",	"SIGCHLD",	"SIGCONT",		"SIGSTOP",
  "SIGTSTP",	"SIGTTIN",	"SIGTTOU",		"SIGURG",
  "SIGXCPU",	"SIGXFSZ",	"SIGVTALRM",	"SIGPROF",
  "SIGWINCH",	"SIGPOLL",	"SIGIO",		"SIGPWR"
};

volatile sig_atomic_t fatal_error_in_process = 0;

/* Copyover note:
 * Whenever a program is executed from exec (as in the case of copyover)
 * all signals from the previous execution of the mud return to their 
 * SIG_DFL (default) signal handler, so we need to reinstall our handlers
 * again.
 */


/** Function: signal_handler
  * Descr   : Our signal handler, to shutdown gracefully (if possible)
  *         : upon reciept of a signal signifying we should shutdown.
  * Returns : void
  * Syntax  : (n/a)
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void signal_handler (int sig)
{
  /* Since our 'shutdown_wgrace' is not reentrant, and may (likely) access
   * non-volatile global variables, be damn sure it's not called twice.
   */

  if (fatal_error_in_process)
    raise (sig);
  else
    fatal_error_in_process = TRUE;

  /* Log the signal */
  logf ("SIGNAL -- #%d (%s).", sig,
	(sig > 0 && sig < 33) ? sig_name[sig] : "unknown");

  switch (sig)
  {
  case SIGTERM:
  case SIGINT:
  case SIGSEGV:
    /* Attempt a graceful shutdown  */
    shutdown_wgrace (sig);
    raise (sig);
    break;

#if !defined(WIN32)
  case SIGUSR1:
    /* Log Copyover request, and initiate copyover */
    fatal_error_in_process = FALSE;
    logf ("SIGNAL -- Received console copyover request.");
    do_copyover (NULL, "60 per CONSOLE request.");
    break;
#endif
  default:
    logf ("SIGNAL -- Received Unexpected Signal: [#%d:%s].", sig,
	  (sig > 0 && sig < 33) ? sig_name[sig] : "(unknown)");
    raise (sig);
    break;
  }

}

/** Function: init_signals
  * Descr   : Initialize signals that we trap, setting up a handler for them.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  * NOTE    : By default, signals sent to sighandler are blocked until the 
  *         : handler returns, but other signals are not blocked. We need 
  *         : to block any other signal we recieve until we are done trying
  *         : to be graceful...
  */
void init_signals ()
{

#if defined(WIN32)
	signal(SIGTERM,  signal_handler);
	signal(SIGSEGV,  signal_handler);
	signal(SIGABRT,  signal_handler);
	signal(SIGFPE,	 signal_handler);
	signal(SIGILL,	 signal_handler);
#else

	struct sigaction sigact;

  /* NOTE: We inherit any current signal actions by default.  Dont install
   * a signal handler for any ignored signals.
   */

  sigaction (SIGTERM, NULL, &sigact);
  if (sigact.sa_handler != SIG_IGN)
  {
	sigemptyset (&sigact.sa_mask);
	sigaddset (&sigact.sa_mask, SIGABRT);
	sigaddset (&sigact.sa_mask, SIGPIPE);
	sigaddset (&sigact.sa_mask, SIGUSR1);
	sigaddset (&sigact.sa_mask, SIGSEGV);
	sigaddset (&sigact.sa_mask, SIGBUS);
	sigact.sa_handler = signal_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction (SIGTERM, &sigact, NULL);

  }
  else
	logf ("Init: Signal SIGTERM ignored.");

  sigaction (SIGBUS, NULL, &sigact);
  if (sigact.sa_handler != SIG_IGN)
  {
	sigemptyset (&sigact.sa_mask);
	sigaddset (&sigact.sa_mask, SIGUSR1);
	sigaddset (&sigact.sa_mask, SIGABRT);
	sigaddset (&sigact.sa_mask, SIGPIPE);
	sigaddset (&sigact.sa_mask, SIGSEGV);
	sigaddset (&sigact.sa_mask, SIGTERM);
	sigact.sa_handler = signal_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction (SIGBUS, &sigact, NULL);

  }
  else
	logf ("Init: Signal SIGBUS ignored.");

  sigaction (SIGINT, NULL, &sigact);
  if (sigact.sa_handler != SIG_IGN)
  {
	sigemptyset (&sigact.sa_mask);
	sigaddset (&sigact.sa_mask, SIGTERM);
	sigaddset (&sigact.sa_mask, SIGPIPE);
	sigaddset (&sigact.sa_mask, SIGUSR1);
	sigaddset (&sigact.sa_mask, SIGSEGV);
	sigaddset (&sigact.sa_mask, SIGBUS);
	sigact.sa_handler = signal_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction (SIGABRT, &sigact, NULL);
  }
  else
	logf ("Init: Signal SIGABRT ignored.");

  sigaction (SIGSEGV, NULL, &sigact);
  if (sigact.sa_handler != SIG_IGN)
  {
	sigemptyset (&sigact.sa_mask);
	sigaddset (&sigact.sa_mask, SIGTERM);
	sigaddset (&sigact.sa_mask, SIGUSR1);
	sigaddset (&sigact.sa_mask, SIGPIPE);
	sigaddset (&sigact.sa_mask, SIGABRT);
	sigaddset (&sigact.sa_mask, SIGBUS);
	sigact.sa_handler = signal_handler;
	sigact.sa_flags = SA_RESETHAND;
	sigaction (SIGSEGV, &sigact, NULL);
  }
  else
	logf ("Init: Signal SIGSEGV ignored.");

  /* Setup a copyover (60 sec) signal */
  sigaction (SIGUSR1, NULL, &sigact);
  sigemptyset (&sigact.sa_mask);
  sigaddset (&sigact.sa_mask, SIGABRT);
  sigaddset (&sigact.sa_mask, SIGPIPE);
  sigaddset (&sigact.sa_mask, SIGSEGV);
  sigaddset (&sigact.sa_mask, SIGBUS);
  sigaddset (&sigact.sa_mask, SIGTERM);
  sigact.sa_handler = signal_handler;
  sigact.sa_flags = 0;
  sigaction (SIGUSR1, &sigact, NULL);

  logf ("Init: Signals initialized.");

#endif
}


/** Function: shutdown_wgrace
  * Descr   : Upon receipt of a fatal signal, attempt a graceful shutdown.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 10/98
  * Author  : Gary McNickle <gary@tarmongaidon.org>
  */
void shutdown_wgrace (int sig)
{
  FILE *fp;
  extern bool merc_down;
  DESCRIPTOR_DATA *d, *d_next;
  CHAR_DATA *c, *c_next;
  char buf[MIL];

  merc_down = TRUE;

  shutdown_web_server ();		/* be sure we close the webserver port  */
  save_mud_data ();				/* Save the pMud data                   */
  do_asave (NULL, "");			/* Attempt to save any changed areas    */

  /* Create a shutdown file to halt re-start via shell cmd */
  if (sig == SIGTERM || sig == SIGABRT || sig == SIGINT)
  {
    sprintf (buf, "%s%s", DATA_DIR, SHUTDOWN_FILE);
    if ((fp = fopen (buf, "a")) != NULL)
    {
      sprintf (buf, "Shutdown forced via %s signal.\n",
	       (sig > 0 && sig < 33) ? sig_name[sig] : "(unknown)");
      fprintf (fp, buf);
      fclose (fp);
    }
  }

  sprintf (buf, "%s%s", DATA_DIR, LAST_COMMAND);

  /* Attempt to log the last command given. */
  if ((fp = fopen (buf, "a")) != NULL)
  {
    fprintf (fp, "%s\n", last_command);
    fclose (fp);
  }

  /* Notify players of impending crash, and save all pfiles   */
  for (d = descriptor_list; d != NULL; d = d_next)
  {
    if (d->character)
    {
      if (d->connected == CON_PLAYING)
      {
		if (sig == SIGTERM || sig == SIGABRT)
		{
		  logf ("Received %s signal -- Shutting down.",
			(sig > 0 && sig < 33) ? sig_name[sig] : "(unknown)");

		  chprintf(d->character, "\n\r-*- System Shutdown forced... Saving data. -*-\n\r");
		}
		else
		  chprintf(d->character, "\n\r-*- System Crash... Saving your data. -*-\n\r");
      }
      save_char_obj(d->character);
    }
    d_next = d->next;
    close_descriptor (d);
  }

  /* Attempt to save any persistant mobiles */
  for ( c = char_list; c != NULL; c = c_next )
  {
	  c_next = c->next;

	  if ( IS_NPC(c) && 
		 (IS_FLAG(c->states.player, _ACT_PERSISTANT) || IS_FLAG(c->pIndexData->states.player, _ACT_PERSISTANT)) )
	  {
		  // We have a persistant mobile, attempt to save it...
		  save_char_obj(c);
	  }
  }

  fatal_error_in_process = TRUE;

  raise (sig);
}
