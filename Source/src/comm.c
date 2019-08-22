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

#if defined(_WIN32)
  #include <winsock2.h>
  #include <sys\timeb.h>
#else
  #include <sys/time.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif


#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include "telnet.h"
#include "merc.h"
#include "recycle.h"
#include "zlib.h"

int control;
int ftp_control = -1;		/* Control socket of FTP (-1 if inactive) */


const char go_ahead_str[]	= { IAC, GA, '\0' };
const char compress_will[]	= { IAC, WILL, TELOPT_COMPRESS, '\0' };
const char compress_do[]	= { IAC, DO, TELOPT_COMPRESS, '\0' };
const char compress_dont[]	= { IAC, DONT, TELOPT_COMPRESS, '\0' };

#if	defined( _WIN32 )
//	void    gettimeofday    ( struct timeval *tp, void *tzp );

#define WOULD_HAVE_BLOCKED ( WSAGetLastError() == WSAEWOULDBLOCK )
#else
#define WOULD_HAVE_BLOCKED ( errno == EWOULDBLOCK )
#endif

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif


void close_socket( int fd )
{
#if defined(_WIN32)	
	closesocket( fd );
#else
	close ( fd );
#endif
}


void close_descriptor (DESCRIPTOR_DATA * dclose)
{
	CHAR_DATA *ch;
	
	if (dclose->outtop > 0)
		process_output (dclose, FALSE);
	
	if (dclose->snoop_by != NULL)
	{
		write_to_buffer (dclose->snoop_by,
			"Your victim has left the game.\n\r", 0);
	}
	
	{
		DESCRIPTOR_DATA *d;
		
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->snoop_by == dclose)
				d->snoop_by = NULL;
		}
	}
	
	if ((ch = dclose->character) != NULL)
	{
		logf ("Closing link to %s.", ch->name);
		
		/* get rid of rented room info */
		if (IS_FLAG (ch->states.player, _ACT_TENNANT))
		{
			ROOM_INDEX_DATA *room;
			
			REMOVE_FLAG (ch->states.player, _ACT_TENNANT);
			
			if ((room = get_room_index (ch->pcdata->rentroom)) != NULL)
			{
				REMOVE_BIT (room->room_flags, ROOM_RENTED);
				REMOVE_BIT (room->room_flags, ROOM_PRIVATE);
				room->heal_rate = room->heal_rate / 2;
				room->opu_rate = room->opu_rate / 2;
			}
		}
		
		/*
	     * [GM] Pet bug fix 
		 */
		if (ch->pet && ch->pet->in_room == NULL)
		{
			char_to_room (ch->pet, get_room_index (ROOM_VNUM_LIMBO));
			extract_char (ch->pet, TRUE);
		}
		
		/*
		 * Cut down on wiznet spam when rebooting 
		 */
		if ((dclose->connected == CON_PLAYING && !merc_down)
			|| ((dclose->connected >= CON_NOTE_TO)
			&& (dclose->connected <= CON_NOTE_FINISH)))
		{
			act ("$n has lost $s link.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
			wiznet ("Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0);
			ch->desc = NULL;
		}
		else
		{
			free_char (dclose->original ? dclose->original : dclose->character);
		}
	}
	
	if (d_next == dclose)
		d_next = d_next->next;
	
	if (dclose == descriptor_list)
	{
		descriptor_list = descriptor_list->next;
	}
	else
	{
		DESCRIPTOR_DATA *d;
		
		for (d = descriptor_list; d && d->next != dclose; d = d->next)
			;
		if (d != NULL)
			d->next = dclose->next;
		else
			bugf ("[comm.c::close_socket] NULL descriptor.");
	}
	
	if (dclose->out_compress)
	{
		deflateEnd (dclose->out_compress);
		free_mem (dclose->out_compress_buf, COMPRESS_BUF_SIZE);
		free_mem (dclose->out_compress, sizeof (z_stream));
	}
	
	close_socket(dclose->descriptor);
	free_descriptor (dclose);
	return;
}


#if !defined(_WIN32)
void Sleep( time_t milliseconds )
{
	struct timeval nap_time;
	nap_time.tv_usec = 1000 * milliseconds;
	nap_time.tv_sec  = 0;
	if ( select( 0, NULL, NULL, NULL, &nap_time ) < 0 )
	{
		switch (errno)
		{
		   case EBADF: bugf("[comm.c::Sleep] Invalid file descriptor passed to Select()");
		   break;
		   case EINTR: bugf("[comm.c::Sleep] A non-blocked signal was caught.");
		   break;
		   case EINVAL:bugf("[comm.c::Sleep] Negative \'n\' descriptor passed to Select()");
		   break; 
		   case ENOMEM:bugf("[comm.c::Sleep] Select() was unable to allocate memory for internal tables.");
		   break;
		   default:    bugf("[comm.c::Sleep] Unknown error.");
		   break;
		}
	}
}
#endif


int main (int argc, char **argv)
{
	struct timeval now_time;
	bool fCopyOver = FALSE;
	
    /* Init time. 	*/
	gettimeofday (&now_time, NULL);
	current_time = (time_t) now_time.tv_sec;
	strcpy (str_boot_time, ctime (&current_time));
	
    /* Get the port number.	*/
	port = 4000;
	if (argc > 1)
	{
		if (!is_number (argv[1]))
		{
			fprintf (stderr, "Usage: %s [port #]\n", argv[0]);
			_quit (1);
		}
		else if ((port = atoi (argv[1])) <= 1024)
		{
			fprintf (stderr, "Port number must be above 1024.\n");
			_quit (1);
		}
		
		/* Are we recovering from a copyover? */
		if (argv[2] && argv[2][0])
		{
			fCopyOver = TRUE;
			control = atoi (argv[3]);
			if (argv[4])
				ftp_control = atoi (argv[4]);
		}
		else
			fCopyOver = FALSE;
	}
	
	
	logf (BOLD "W" BOLD "o" NO_COLOR "T Version " BOLD "%s " NO_COLOR
		"loading...", BLACK, WHITE, BLUE, versionToString (VERSION));
	
	init_signals ();
	boot_db ();
	
	logf ("The Waygate #%d to %s is open.", port, pMud.mud_name);
	
	if (!fCopyOver)
	{
		control = init_socket (port);
		ftp_control = init_socket (port + 6);
//		webUP = (init_web (port + 1));
		webUP = init_web_server(port+1);
	}
	else
		copyover_recover ();
	
	if (ftp_control > -1)
		logf ("MudFTP features are active on port: %d", port + 6);
	
	game_loop_unix (control);
	shutdown_web_server ();
	
	close_socket (control);
	close_socket (ftp_control);
	
	log_string ("Normal termination of game.");
	
#if defined( _WIN32 )
	WSACleanup();
#endif
	
	_quit (0);
	return 0;
}

int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int         x  = 1; 
	int         fd;
	
#if !defined( _WIN32 )
	if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		bugf( "Init_socket: socket" );
		_quit( -1 );
	}
#else
	
	WORD    wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	int err = WSAStartup( wVersionRequested, &wsaData ); 
	
	if ( err != 0 )
	{
		bugf( "[comm.c::init_socket] Invalid or missing WS2_32.DLL" );
		_quit( -1 );
	}
	
	if ( ( fd = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		bugf( "[comm.c::init_socket] socket" );
		_quit( -1 );
	}
#endif
	
	if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof( x ) ) < 0 )
    {
		bugf("[comm.c::init_socket] SO_REUSEADDR" );
		close_socket( fd );
		_quit( -1 );
    }
	
#if defined( SO_DONTLINGER ) && !defined( SYSV )
    {
		struct	linger	ld;
		
		ld.l_onoff  = 1;
		ld.l_linger = 1000;
		
		if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
			(char *) &ld, sizeof( ld ) ) < 0 )
		{
			bugf("[comm.c::init_socket] SO_DONTLINGER" );
			close_socket( fd );
			_quit( -1 );
		}
    }
#endif
	
    sa		    = sa_zero;
#if !defined( _WIN32 )
    sa.sin_family   = AF_INET;
#else
    sa.sin_family   = PF_INET;
#endif
    sa.sin_port	    = htons( port );
	
    if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
    {
		bugf( "[comm.c::init_socket] bind" );
		close_socket( fd );
		_quit( -1 );
    }
	
    if ( listen( fd, 3 ) < 0 )
    {
		bugf( "[comm.c::init_socket] listen" );
		close_socket( fd );
		_quit( -1 );
    }
	
    return fd;
}

#define MS_PER_PULSE ( 1000 / PULSE_PER_SECOND )
// maintained by SyncGameClock; currently read only by WaitForPulse()
unsigned long last_pulse_millisec; 


void SynchronizeClock(void)
{
#if defined( _WIN32 ) 
	struct _timeb now_time;
	
	_ftime ( &now_time );
	current_time = now_time.time;
	last_pulse_millisec = now_time.millitm;
#else
	struct timeval last_time;
	
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
	last_pulse_millisec = (unsigned long) last_time.tv_usec / 1000;
#endif
}




void WaitForPulse()
{
	signed long pulse;
	signed long nap;
#if !defined(_WIN32)
	struct timeval now_time;
	
	gettimeofday( &now_time, NULL );
	pulse =  ((int) now_time.tv_sec ) - ((int) current_time );
	// Careful here of signed versus unsigned arithmetic.
	pulse *= 1000;
	pulse += ((int) now_time.tv_usec / 1000 ) - ((int) last_pulse_millisec);
	nap = MS_PER_PULSE - pulse;
#else
	struct _timeb now_time;
	
	_ftime( &now_time );
	pulse = now_time.time - current_time;
	pulse *= 1000;
	pulse += now_time.millitm - last_pulse_millisec;
#endif
	nap = MS_PER_PULSE - pulse;
	
	if ( nap > 0 )
	{
		Sleep( nap );
	}
}



void game_loop_unix (int control)
{
	static struct timeval null_time = {0,0};
	
#if !defined(_WIN32)	
	signal (SIGPIPE, SIG_IGN);
#endif
	
	
	SynchronizeClock();
	
    /*
	* Main loop 
	*/
	while (!merc_down)
	{
		fd_set in_set;
		fd_set out_set;
		fd_set exc_set;
		DESCRIPTOR_DATA *d;
		int maxdesc;
		
		/*
		* Poll all active descriptors.
		*/
		FD_ZERO (&in_set);
		FD_ZERO (&out_set);
		FD_ZERO (&exc_set);
		FD_SET (control, &in_set);
		maxdesc = control;
		
		/*
		* Setup mudFTP
		*/
		if (ftp_control > -1)
			FD_SET (ftp_control, &in_set);
		
		maxdesc = UMAX (control, ftp_control);
		
		for (d = descriptor_list; d; d = d->next)
		{
			maxdesc = UMAX (maxdesc, d->descriptor);
			FD_SET (d->descriptor, &in_set);
			FD_SET (d->descriptor, &out_set);
			FD_SET (d->descriptor, &exc_set);
		}
		
		if (select (maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0)
		{
			perror ("Game_loop: select: poll");
			_quit (1);
		}
		
		/*
		* New connection?
		*/
		if (FD_ISSET (control, &in_set))
			init_descriptor (control, NDESC_NORMAL);
		
		if (FD_ISSET (ftp_control, &in_set))
			init_descriptor (ftp_control, NDESC_FTP);
		
			/* 
			* Kick out the freaky folk
		*/
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;
			
			if (FD_ISSET (d->descriptor, &exc_set))
			{
				FD_CLR (d->descriptor, &in_set);
				FD_CLR (d->descriptor, &out_set);
				
				if (d->character && d->connected == CON_PLAYING)
					save_char_obj (d->character);
				
				d->outtop = 0;
				close_descriptor (d);
			}
		}
		
		
		/* 
		* Process Input
		* 
		* The data to mudFTP is dispatched through nanny. However, usually the
		* MUD allows one line of input per pulse: thus a 100 line file that
		* is uploaded will take 25 seconds to process. In case of a mudFTP
		* data connection we do an exception and will allow data to be parsed
		* as long as there is more of it
		*/
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;
			d->fcommand = FALSE;
			
			if (FD_ISSET (d->descriptor, &in_set))
			{
				if (d->character != NULL)
					d->character->timer = 0;
				
				if (!read_from_descriptor (d))
				{
					FD_CLR (d->descriptor, &out_set);
					if (d->character != NULL && d->connected == CON_PLAYING)
						save_char_obj (d->character);
					d->outtop = 0;
					close_descriptor (d);
					continue;
				}
			}
			
			if (d->character != NULL && d->character->daze > 0)
				--d->character->daze;
			
			if (d->character != NULL && d->character->wait > 0)
			{
				--d->character->wait;
				continue;
			}
			
			read_from_buffer (d);
			
			if (d->incomm[0] != '\0')
			{
				d->fcommand = TRUE;
				stop_idling (d->character);
				
				/* OLC */
				if (d->showstr_point)
					show_string (d, d->incomm);
				else
					if (d->pString)
						string_add (d->character, d->incomm);
					else
					{

						switch (d->connected)
						{
							
							  case CON_PLAYING:
								  if (!run_olc_editor (d))
									  substitute_alias (d, d->incomm);
								  break;
								  
							  default:
								  /*
								   * slight hack here so we can snarf 
								   * all mudftp data in one go -O
								   */
								  while (d->incomm[0])
								  {
									  nanny (d, d->incomm);
									  
									  if (d->connected != CON_FTP_DATA)
										  break;
										  
									  d->incomm[0] = '\0';
									  read_from_buffer (d);
								  }
								  break;
						}
					}

					d->incomm[0] = '\0';
			}
			
		}
		
		/*
 		 * Autonomous game motion.
		 */
		update_handler ();
		
		if (webUP)
//			handle_web ();
			update_web_server();
		
		/*
		 * Output.
		 */
		for (d = descriptor_list; d != NULL; d = d_next)
		{
			d_next = d->next;
			
			if ((d->fcommand || d->outtop > 0 || d->out_compress)
				&& FD_ISSET (d->descriptor, &out_set))
			{
				bool ok = TRUE;
				
				if (d->fcommand || d->outtop > 0)
					ok = process_output (d, TRUE);
				
				if (ok && d->out_compress)
					ok = processCompressed (d);
				
				if (!ok)
				{
					if (d->character != NULL && d->connected == CON_PLAYING)
						save_char_obj (d->character);
					d->outtop = 0;
					close_descriptor (d);
				}
			}
			
		}
		
		WaitForPulse();
		SynchronizeClock();
		
  }
  
  return;
}


void init_descriptor (int control, ndesc_t type)
{
#if defined(_WIN32)
	static unsigned long ARGP = 1;
#endif
	
	char buf[MSL];
	DESCRIPTOR_DATA *dnew;
	struct sockaddr_in sock;
	struct hostent *from;
	int desc;
	int size;
	
	size = sizeof (sock);
	getsockname (control, (struct sockaddr *) &sock, &size);
	
	if ((desc = accept (control, (struct sockaddr *) &sock, &size)) < 0)
	{
		bugf("[comm.c::init_descriptor] accept");
		return;
	}
	
#if defined( _WIN32 )
    
	if ( ioctlsocket( desc, FIONBIO, &ARGP ) != 0 )
    {
		bugf( "[comm.c::init_descriptor] ioctlsocket returned error code %d", WSAGetLastError() );
		return;
    }
#else
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
		bugf( "[comm.c::init_descriptor] fcntl: FNDELAY" );
		return;
    }
#endif
	
    /*
	* Cons a new descriptor.
	*/
	dnew = new_descriptor ();
	dnew->descriptor = desc;
	
	dnew->pEdit = NULL;		/* OLC */
	dnew->pString = NULL;	/* OLC */
	dnew->editor = 0;		/* OLC */
	
	size = sizeof (sock);
	if (getpeername (desc, (struct sockaddr *) &sock, &size) < 0)
	{
		bugf("[comm.c::init_descriptor] getpeername");
		dnew->host = str_dup ("(unknown)");
	}
	else
	{
	/** Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
		int addr;
		
		addr = ntohl (sock.sin_addr.s_addr);
		sprintf (buf, "%d.%d.%d.%d",
			(addr >> 24) & 0xFF,
			(addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF);
		
		logf ("Sock.sinaddr:  %s", buf);
		
		from = gethostbyaddr ((char *) &sock.sin_addr,
			sizeof (sock.sin_addr), AF_INET);
		
		dnew->host = str_dup (from ? from->h_name : buf);
	}
	
	/*
	* Swiftest: I added the following to ban sites.  I don't
	* endorse banning of sites, but Copper has few descriptors now
	* and some people from certain sites keep abusing access by
	* using automated 'autodialers' and leaving connections hanging.
	*
	* Furey: added suffix check by request of Nickel of HiddenWorlds.
	*/
	if (check_ban (dnew->host, BAN_ALL))
	{
		write_to_descriptor (dnew,
			"Your site has been banned from this mud.\n\r", 0);
		
		close_socket (desc);
		free_descriptor (dnew);
		return;
	}
	
    /* Init descriptor data. */
	dnew->next = descriptor_list;
	descriptor_list = dnew;
	
    /* 
	 * Login Protocol Handshaking
	 *
	 ** mccp: tell the client we support compression (v1)
	 */
	if ( IS_FLAG(pMud.config, _ENABLE_COMPRESSION) )
		write_to_buffer (dnew, compress_will, 0);
	
    /* Send the greeting. */
	if (type == NDESC_NORMAL)
	{
		extern char *help_greeting;
		
		if (help_greeting[0] == '.')
			write_to_buffer (dnew, help_greeting + 1, 0);
		else
			write_to_buffer (dnew, help_greeting, 0);
	}
    /*
	 * FTP connections are handled in ftp.c 
	 */
	else if (type == NDESC_FTP)
		greet_ftp (dnew, "");
	
	return;
}




bool read_from_descriptor (DESCRIPTOR_DATA * d)
{
	int iStart;
	
	/* Hold horses if pending command already. */
	if (d->incomm[0] != '\0')
		return TRUE;
	
	/* Check for overflow. */
	iStart = strlen (d->inbuf);
	
	if (iStart >= sizeof (d->inbuf) - 10)
	{
		logf ("%s input overflow!", d->host);
		write_to_descriptor (d, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
		return FALSE;
	}
	
    /*
	* Snarf input 
	*/
	for (;;)
	{
		int nRead;
#if defined(_WIN32)
		unsigned long nWaiting;
#endif
		
		/* There is no more space in the input buffer for now */
		if (sizeof (d->inbuf) - 10 - iStart == 0)
			break;
		
#if defined( _WIN32 )
		ioctlsocket( d->descriptor, FIONREAD, &nWaiting );
		
		if ( !nWaiting ) 
			break;
		
		nRead = recv( d->descriptor, d->inbuf + iStart, 
			UMIN( nWaiting, sizeof( d->inbuf ) - 10 - iStart ),0 );
#else
		nRead = read( d->descriptor, d->inbuf + iStart, 
			sizeof( d->inbuf ) - 10 - iStart );
#endif
		
		if (nRead > 0)
		{
			iStart += nRead;
			if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
				break;
		}
		else
			if (nRead == 0)
			{
				return FALSE;	   /* this is just a simple link lost... */
				
								   /* waste of good log space...
								   * logf("%s has lost their link.", d->character->name);
								   * return FALSE; 
				*/
			}
			
			else if (WOULD_HAVE_BLOCKED)
				break;
			else
			{
				perror ("Read_from_descriptor");
				return FALSE;
			}
	}
	
	d->inbuf[iStart] = '\0';
	return TRUE;
}



/** Transfer one line from input buffer to input line.
*/
void read_from_buffer (DESCRIPTOR_DATA * d)
{
	int i, j, k;
	bool got_n, got_r;
	
    /** Hold horses if pending command already.
	*/
	if (d->incomm[0] != '\0')
		return;
	
		/** Look for at least one new line.
	*/
	for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if (d->inbuf[i] == '\0')
			return;
	}
	
    /** Canonical input processing.
	*/
	for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if (k >= MIL - 2)
		{
			write_to_descriptor (d, "Line too long.\n\r", 0);
			
			/* skip the rest of the line */
			for (; d->inbuf[i] != '\0'; i++)
			{
				if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
					break;
			}
			d->inbuf[i] = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}
		
		if (d->inbuf[i] == '\b' && k > 0)
			--k;
		else
			if (isascii (d->inbuf[i]) && isprint (d->inbuf[i]))
				d->incomm[k++] = d->inbuf[i];
			else
				if (d->inbuf[i] == (signed char) IAC)
				{
					if (!memcmp (&d->inbuf[i], compress_do, strlen (compress_do)))
					{
						i += strlen (compress_do) - 1;
						compressStart (d);
					}
					else if (!memcmp (&d->inbuf[i], compress_dont, strlen (compress_dont)))
					{
						i += strlen (compress_dont) - 1;
						compressEnd (d);
					}
				}
	}
	
    /** Finish off the line.
	*/
	if (k == 0)
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';
	
    /** Deal with bozos with #repeat 1000 ...
	*/
	
	if (k > 1 || d->incomm[0] == '!')
	{
		if ((d->incomm[0] != '!' && strcmp (d->incomm, d->inlast))
			|| !d->character)
		{
			d->repeat = 0;
		}
		else
		{
			if (++d->repeat >= 25 && d->character && d->connected == CON_PLAYING)
			{
				logf ("%s input spamming!", d->host);
				wiznet ("Spam spam spam $N spam spam spam spam spam!",
					d->character, NULL, WIZ_SPAM, 0, get_trust (d->character));
				
				if (d->incomm[0] == '!')
					wiznet (d->inlast, d->character, NULL, WIZ_SPAM, 0,
					get_trust (d->character));
				else
					wiznet (d->incomm, d->character, NULL, WIZ_SPAM, 0,
					get_trust (d->character));
				
				d->repeat = 0;
				/**
                * SPAM warning
                * Uncomment to force player to quit
       	        
				 write_to_descriptor( d, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
				 strcpy( d->incomm, "quit" );
				 
                */
			}
		}
	}
	
	
    /** Do '!' substitution.
	*/
	if (d->incomm[0] == '!')
		strcpy (d->incomm, d->inlast);
	else
		strcpy (d->inlast, d->incomm);
	
		/*
		* Shift the input buffer.
		* mudftp: Do not compress multiple lines into just one.
	*/
	got_n = got_r = FALSE;
	
	for (; d->inbuf[i] == '\r' || d->inbuf[i] == '\n'; i++)
	{
		if (d->inbuf[i] == '\r' && got_r++)
			break;
		
		else if (d->inbuf[i] == '\n' && got_n++)
			break;
	}
	
	for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
		;
	return;
}

/** Function: process_ansi_output
* Descr   : Sends buffered output to descriptor, converting any ANSI codes
*         : along the way if character has selected ANSI.
* Returns : TRUE/FALSE based on success of output to descriptor
* Syntax  : (N/A)
* Written : v1.0 5/98
* Author  : Lope, updated by Gary McNickle <gary@tarmongaidon.org>
*/
bool process_ansi_output (DESCRIPTOR_DATA * d)
{
	char *counter;
	char output[MSL];
	char temp[MSL];
	char *work;
	bool success = TRUE;
	char *i;
	
	/* No sense in dealing with a null descriptor */
	if (d == NULL)
		return FALSE;
	
	/* Initialize output buffer, counter and working string */
	memset (output, 0, MSL);
	counter = output;
	work = d->outbuf;
	
	while (*work != '\0' && (work - d->outbuf) < d->outtop)
	{
		if ((int) (counter - output) >= MSL - 32)	/* have a full buffer */
		{
			*counter++ = '\0';
			
			if (!(success = write_to_descriptor (d, output, strlen (output))))
				break;			/* problems... */
			
			memset (output, 0, MSL);
			counter = output;		/* increment counter */
		}
		
		if (*work != ANSI_KEY)
		{
			*counter++ = *work++;
			continue;
		}
		
		/* Ok, we have an ansi request... process it */
		/* Pass the ANSI_KEY code, next should be actual color code itself */
		work++;
		
		temp[0] = '\0';		/* Initialize buffer */
		
		if ((d->character && IS_FLAG (d->character->states.config, _ANSI)) ||
			(d->original && IS_FLAG(d->original->states.config, _ANSI)) )
			switch (*work)
			{
			  case '!':
				  sprintf (temp, "%s", BLUE_BG);
				  break;
			  case '#':
				  sprintf (temp, "%s", CYAN_BG);
				  break;
			  case '$':
				  sprintf (temp, "%s", GREEN_BG);
				  break;
			  case '%':
				  sprintf (temp, "%s", MAGENTA_BG);
				  break;
			  case '^':
				  sprintf (temp, "%s", RED_BG);
				  break;
			  case '&':
				  sprintf (temp, "%s", WHITE_BG);
				  break;
			  case '*':
				  sprintf (temp, "%s", YELLOW_BG);
				  break;
			  case '+':
				  sprintf (temp, "%s", HRED_BG);
				  break;
			  case 'b':
				  sprintf (temp, NORMAL, BLUE);
				  break;
			  case 'c':
				  sprintf (temp, NORMAL, CYAN);
				  break;
			  case 'g':
				  sprintf (temp, NORMAL, GREEN);
				  break;
			  case 'm':
				  sprintf (temp, NORMAL, MAGENTA);
				  break;
			  case 'n':
				  sprintf (temp, NORMAL, BLACK);
				  break;
			  case 'r':
				  sprintf (temp, NORMAL, RED);
				  break;
			  case 'y':
				  sprintf (temp, NORMAL, YELLOW);
				  break;
			  case 'w':
				  sprintf (temp, "%s", NO_COLOR);
				  break;
			  case 'B':
				  sprintf (temp, BOLD, BLUE);
				  break;
			  case 'C':
				  sprintf (temp, BOLD, CYAN);
				  break;
			  case 'G':
				  sprintf (temp, BOLD, GREEN);
				  break;
			  case 'M':
				  sprintf (temp, BOLD, MAGENTA);
				  break;
			  case 'N':
				  sprintf (temp, BOLD, BLACK);
				  break;
			  case 'R':
				  sprintf (temp, BOLD, RED);
				  break;
			  case 'W':
				  sprintf (temp, BOLD, WHITE);
				  break;
			  case 'Y':
				  sprintf (temp, BOLD, YELLOW);
				  break;
			  case '0':
				  sprintf (temp, BLINK, BLUE);
				  break;
			  case '1':
				  sprintf (temp, BLINK, CYAN);
				  break;
			  case '2':
				  sprintf (temp, BLINK, GREEN);
				  break;
			  case '3':
				  sprintf (temp, BLINK, MAGENTA);
				  break;
			  case '4':
				  sprintf (temp, BLINK, RED);
				  break;
			  case '5':
				  sprintf (temp, BLINK, WHITE);
				  break;
			  case '6':
				  sprintf (temp, BLINK, YELLOW);
				  break;
			  case '-':
				  sprintf (temp, "%c", '~');
				  break;
			  default:
				  sprintf (temp, "%c", *work);
				  break;
			}				/* end: switch */
		
			work++;			/* pass the ansi request code */
		
			/* increment counters */
			i = temp;
			while ((*counter = *i) != '\0')
				++counter, ++i;

  }	/* end: while */
  
  success = success && (write_to_descriptor (d, output, strlen (output)));
  
  d->outtop = 0;
  return success;
  
}



/*
 * Low level output function.
 */
bool process_output (DESCRIPTOR_DATA * d, bool fPrompt)
{
	extern bool merc_down;
	
    /* Bust a prompt. */
	if (!merc_down)
	{
		if (d->showstr_point)
			write_to_buffer (d, "[Hit Return to continue]\n\r", 0);
		else if (fPrompt && d->pString && d->connected == CON_PLAYING)
			write_to_buffer (d, "> ", 2);
		else if (fPrompt && d->connected == CON_PLAYING)
		{
			CHAR_DATA *ch;
			
			ch = d->original ? d->original : d->character;
			
			if (ch != NULL)
			{
				if (!IS_FLAG (ch->states.config, _COMPACT))
					write_to_buffer (d, "\n\r", 2);
				
				if (IS_FLAG (ch->states.config, _PROMPT))
					bust_a_prompt (d->character);
				
				if (IS_FLAG (ch->states.config, _TELNET_GA))
					write_to_buffer (d, go_ahead_str, 0);
			}
		}
	}

    /*  Short-circuit if nothing to write. 	*/
	if (d->outtop == 0)
		return TRUE;
	
	/* Snoop-o-rama. 	*/
	if (d->snoop_by != NULL)
	{
		if (d->character != NULL)
			write_to_buffer (d->snoop_by, d->character->name, 0);

		write_to_buffer (d->snoop_by, "> ", 2);
		write_to_buffer (d->snoop_by, d->outbuf, d->outtop);
	}
	
	return process_ansi_output (d);
	
}

/* Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt (CHAR_DATA * ch)
{
	char buf[MSL];
	char buf2[MSL];
	const char *str;
	const char *i;
	char *point;
	char doors[MIL];
	EXIT_DATA *pexit;
	bool found;
	const char *dir_name[] = { "N", "E", "S", "W", "U", "D" };
	int door;
	
	point = buf;
	str = ch->prompt;
	
	if (str == NULL || str[0] == '\0')
	{
		sprintf (buf, "<%d`Yhp`w %d`Rab`w %d`Bmv`w> %s",
			ch->hit, OPR (ch), ch->move, ch->prefix);
		send_to_char (buf, ch);
		return;
	}
	
	memset (buf, 0, MSL);
	memset (buf2, 0, MSL);
	
	if (IS_FLAG (ch->states.config, _AFK))
		strcat (buf2, "<AFK>");
	
	if (IS_FLAG (ch->states.config, _BUILDING))
		strcat (buf2, "<Build>");
	
	if (IS_FLAG (ch->states.config, _PKQUESTOR))
		strcat (buf2, "<PKQ>");
	
	switch (ch->desc->editor)
	{
		case ED_AREA:
			strcat (buf2, "<Aedit>");
			break;
		case ED_ROOM:
			strcat (buf2, "<Redit>");
			break;
		case ED_OBJECT:
			strcat (buf2, "<Oedit>");
			break;
		case ED_MOBILE:
			strcat (buf2, "<Medit>");
			break;
		case ED_MPCODE:
			strcat (buf2, "<MPedit>");
			break;
		case ED_GUILD:
			strcat (buf2, "<Gedit>");
			break;
		case ED_GROUP:
			strcat (buf2, "<GRedit>");
			break;
		case ED_WEAVE:
			strcat (buf2, "<Wedit>");
			break;
		case ED_CMD:
			strcat (buf2, "<Cedit>");
			break;
		default:
			break;
	}
	
	if (buf2[0] != '\0');
	{
		strcat (buf2, " ");
		send_to_char (buf2, ch);
		memset (buf2, 0, MSL);
	}
	
	while (*str != '\0')
	{
		if (*str != '%')
		{
			*point++ = *str++;
			continue;
		}
		
		++str;
		
		switch (*str)
		{
			default:
				i = " ";
				break;
				
			case 'e':
				found = FALSE;
				doors[0] = '\0';
				for (door = 0; door < 6; door++)
				{
					if ((pexit = ch->in_room->exit[door]) != NULL
						&& pexit->u1.to_room != NULL
						&& (can_see_room (ch, pexit->u1.to_room)
						|| (IS_AFFECTED (ch, _AFF_INFRARED)
						&& !IS_AFFECTED (ch, _AFF_BLIND)))
						&& !IS_SET (pexit->exit_info, EX_CLOSED)
						&& !IS_SET (pexit->exit_info, EX_SECRET))
					{
						found = TRUE;
						strcat (doors, dir_name[door]);
					}
				}
				if (!found)
					strcat (buf, "none");
				sprintf (buf2, "%s", doors);
				i = buf2;
				break;
			case 'c':
				sprintf (buf2, "%s", "\n\r");
				i = buf2;
				break;
			case 'h':
				sprintf (buf2, "%d", ch->hit);
				i = buf2;
				break;
			case 'H':
				sprintf (buf2, "%d", ch->max_hit);
				i = buf2;
				break;
			case 'a':
				sprintf (buf2, "%d", TAU (ch));
				i = buf2;
				break;
			case 'A':
				sprintf (buf2, "%d", OPR (ch));
				i = buf2;
				break;
			case 'v':
				sprintf (buf2, "%d", ch->move);
				i = buf2;
				break;
			case 'V':
				sprintf (buf2, "%d", ch->max_move);
				i = buf2;
				break;
			case 'x':
				sprintf (buf2, "%ld", ch->exp);
				i = buf2;
				break;
			case 'X':
				if (ch->desc->original == NULL)
				{
					sprintf (buf2, "%ld", IS_NPC (ch) ? 0 :
				(ch->level + 1) * exp_per_level (ch,
					ch->pcdata->points) -
					ch->exp);
				i = buf2;
				break;
				}
			case 'g':
				sprintf (buf2, "%ld", ch->gold);
				i = buf2;
				break;
			case 's':
				sprintf (buf2, "%ld", ch->silver);
				i = buf2;
				break;
			case 'l':
				if (ch->level > 9)
					sprintf (buf2, "%d", ch->alignment);
				else
					sprintf (buf2, "%s", IS_GOOD (ch) ? "good" : IS_EVIL (ch) ?
					"evil" : "neutral");
				i = buf2;
				break;
			case 'r':
				if (ch->in_room != NULL)
					sprintf (buf2, "%s",
					((!IS_NPC (ch) && IS_FLAG (ch->states.config, _HOLYLIGHT)) ||
					(!IS_AFFECTED (ch, _AFF_BLIND)
					&& !room_is_dark (ch->in_room))) ? ch->in_room->name : "darkness");
				else
					sprintf (buf2, " ");
				i = buf2;
				break;
			case 'R':
				if (IS_IMMORTAL (ch) && ch->in_room != NULL)
					sprintf (buf2, "%ld", ch->in_room->vnum);
				else
					sprintf (buf2, " ");
				i = buf2;
				break;
			case 'z':
				if (IS_IMMORTAL (ch) && ch->in_room != NULL)
					sprintf (buf2, "%s", ch->in_room->area->name);
				else
					sprintf (buf2, " ");
				i = buf2;
				break;
			case '%':
				sprintf (buf2, "%%");
				i = buf2;
				break;
			case 'o':
				sprintf (buf2, "%s", olc_ed_name (ch));
				i = buf2;
				break;
			case 'O':
				sprintf (buf2, "%s", olc_ed_vnum (ch));
				i = buf2;
				break;
    }
    ++str;
    while ((*point = *i) != '\0')
		++point, ++i;
  }
  write_to_buffer (ch->desc, buf, point - buf);
  
  if (ch->prefix[0] != '\0')
	  write_to_buffer (ch->desc, ch->prefix, 0);
  return;
}



/* Append onto an output buffer. */
void write_to_buffer (DESCRIPTOR_DATA * d, const char *txt, int length)
{
    /* Find length in case caller didn't. 	*/
	if (length <= 0)
		length = strlen (txt);
	
    /* Initial \n\r if needed. 	*/
	if (d->outtop == 0 && !d->fcommand)
	{
		d->outbuf[0] = '\n';
		d->outbuf[1] = '\r';
		d->outtop = 2;
	}
	
    /* Expand the buffer as needed. */
	while (d->outtop + length >= d->outsize)
	{
		char *outbuf;
		
		if (d->outsize >= 32000)
		{
			bugf("[comm.c::write_to_buffer] Buffer overflow. Closing.\n\r");
			close_descriptor (d);
			return;
		}
		outbuf = alloc_mem (2 * d->outsize);
		strncpy (outbuf, d->outbuf, d->outtop);
		free_mem (d->outbuf, d->outsize);
		d->outbuf = outbuf;
		d->outsize *= 2;
	}
	
    /* Copy */
	strncpy (d->outbuf + d->outtop, txt, length);
	d->outtop += length;
	return;
}



/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor (DESCRIPTOR_DATA * d, char *txt, int length)
{
	int iStart = 0;
	int nWrite = 0;
	int nBlock = 0;
	
	if (d->out_compress)
		return writeCompressed (d, txt, length);
	else
	{
		if (length <= 0)
			length = strlen (txt);
		
		for (iStart = 0; iStart < length; iStart += nWrite)
		{
			nBlock = UMIN (length - iStart, 4096);
#ifdef _WIN32 
			if ((nWrite = send (d->descriptor, txt + iStart, nBlock, 0)) < 0)
#else
				if ((nWrite = write (d->descriptor, txt + iStart, nBlock)) < 0)
#endif
				{
					perror ("Write_to_descriptor");
					return FALSE;
				}
		}
		
		return TRUE;
	}
}

/** Function: check_parse_name
  * Descr   : Parses a name, checking for acceptability.
  * Returns : TRUE/FALSE if name is ok or not.
  * Syntax  : (N/A)
  * Written : v1.1 3/98
  * Author  : Rom Consortium updated by: Gary McNickle <gary@tarmongaidon.org>
  */
bool check_parse_name (char *name)
{
	
	if (name_is_banned (name))
		return FALSE;

 	/* Length restrictions. */
	if (strlen (name) < 2)
		return FALSE;
	
   /* 
	* Alphanumerics only.
  	* Lock out lill twits.
	*/
	{
		char *pc;
		int total_caps = 0;
		bool fIll = TRUE, adjcaps = FALSE, cleancaps = FALSE;
		
		for (pc = name; *pc != '\0'; pc++)
		{
			if (!is_allowed (*pc))
				return FALSE;
			
			if (isupper (*pc))	/* ugly anti-caps hack */
			{
				if (adjcaps)
					cleancaps = TRUE;
				total_caps++;
				adjcaps = TRUE;
			}
			else
				adjcaps = FALSE;
			
			if (LOWER (*pc) != 'i' && LOWER (*pc) != 'l')
				fIll = FALSE;
		}
		
		if (fIll)
			return FALSE;
		
		if (cleancaps || (total_caps > (strlen (name)) / 2 && strlen (name) < 3))
			return FALSE;
	}
	
	/* Prevent players from naming themselves after mobs. */
	{
		extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
		MOB_INDEX_DATA *pMobIndex;
		int iHash;
		
		for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
		{
			for (pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL;
			pMobIndex = pMobIndex->next)
			{
				if (is_name (name, pMobIndex->player_name))
					return FALSE;
			}
		}
	}
	
	
	return TRUE;
} /* end: parse_name */



/* 
 * Look for link-dead player to reconnect.
 */
bool check_reconnect (DESCRIPTOR_DATA * d, char *name, bool fConn)
{
	CHAR_DATA *ch;
	
	for (ch = char_list; ch != NULL; ch = ch->next)
	{
		if (!IS_NPC (ch)
			&& (!fConn || ch->desc == NULL)
			&& !str_cmp (d->character->name, ch->name))
		{
			if (fConn == FALSE)
			{
				free_string (d->character->pcdata->pwd);
				d->character->pcdata->pwd = str_dup (ch->pcdata->pwd);
			}
			else
			{
				free_char (d->character);
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;
				send_to_char ("Reconnecting. Type replay to see missed tells.\n\r",
					ch);
				act ("$n has reconnected.", ch, NULL, NULL, TO_ROOM, POS_RESTING);
				
				logf ("%s@%s reconnected.", ch->name, d->host);
				wiznet ("$N groks the fullness of $S link.",
					ch, NULL, WIZ_LINKS, 0, 0);
				d->connected = CON_PLAYING;
				/* Inform the character of a note in progress and the possbility
				 * of continuation!
				 */
				if (ch->pcdata->in_progress)
					send_to_char
					("You have a note in progress. Type NWRITE to continue it.\n\r",
					ch);
			}
			return TRUE;
		}
	}
	
	return FALSE;
}



/* 
 * Check if already playing.
 */
bool check_playing (DESCRIPTOR_DATA * d, char *name)
{
	DESCRIPTOR_DATA *dold;
	
	for (dold = descriptor_list; dold; dold = dold->next)
	{
		if (dold != d
			&& dold->character != NULL
			&& dold->connected != CON_GET_NAME
			&& dold->connected != CON_GET_OLD_PASSWORD
			&& !str_cmp (name, dold->original
			? dold->original->name : dold->character->name))
		{
			write_to_buffer (d, "That character is already playing.\n\r", 0);
			write_to_buffer (d, "Do you wish to connect anyway (Y/N)?", 0);
			d->connected = CON_BREAK_CONNECT;
			return TRUE;
		}
	}
	
	return FALSE;
}



void stop_idling (CHAR_DATA * ch)
{
	if (ch == NULL
		|| ch->desc == NULL
		|| ch->desc->connected != CON_PLAYING
		|| ch->was_in_room == NULL
		|| ch->in_room != get_room_index (ROOM_VNUM_LIMBO))
		return;
	
	ch->timer = 0;
	char_from_room (ch);
	char_to_room (ch, ch->was_in_room);
	ch->was_in_room = NULL;
	act ("$n has returned from the void.", ch, NULL, NULL, TO_ROOM, POS_RESTING);

	return;
}


/* Write to one char. */
void send_to_char (char *txt, CHAR_DATA *ch)
{
	if (txt != NULL && ch->desc != NULL)
	{
		write_to_buffer (ch->desc, txt, strlen (txt));
	}
	return;
}


/* Send a page to one char. */
void page_to_char (char *txt, CHAR_DATA * ch)
{
	
	if (txt == NULL || ch->desc == NULL)
		
		if (ch->lines == 0)
		{
			send_to_char (txt, ch);
			return;
		}
		
		if (ch->desc->showstr_head &&
			(strlen (txt) + strlen (ch->desc->showstr_head) + 1) < 32000)
		{
			char *temp =
				alloc_mem (strlen (txt) + strlen (ch->desc->showstr_head) + 1);
			strcpy (temp, ch->desc->showstr_head);
			strcat (temp, txt);
			
			ch->desc->showstr_point = temp +
				(ch->desc->showstr_point - ch->desc->showstr_head);
			free_mem (ch->desc->showstr_head, strlen (ch->desc->showstr_head) + 1);
			ch->desc->showstr_head = temp;
		}
		else
		{
			if (ch->desc->showstr_head)
				free_mem (ch->desc->showstr_head, strlen (ch->desc->showstr_head) + 1);
			
			ch->desc->showstr_head = alloc_mem (strlen (txt) + 1);
			strcpy (ch->desc->showstr_head, txt);
			ch->desc->showstr_point = ch->desc->showstr_head;
			show_string (ch->desc, "");
		}
}


/*
 * string pager 
 */
void show_string (struct descriptor_data *d, char *input)
{
	char buffer[4 * MSL];
	char buf[MIL];
	register char *scan, *chk;
	int lines = 0, toggle = 1;
	int show_lines;
	
	one_argument (input, buf);
	
	if (buf[0] != '\0')
	{
		if (d->showstr_head)
		{
			free_mem (d->showstr_head, strlen (d->showstr_head));
			d->showstr_head = 0;
		}
		
		d->showstr_point = 0;
		return;
	}
	
	if (d->character)
		show_lines = d->character->lines;
	else
		show_lines = 0;
	
	for (scan = buffer;; scan++, d->showstr_point++)
	{
		if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
			&& (toggle = -toggle) < 0)
			lines++;
		
		else
			if (!*scan || (show_lines > 0 && lines >= show_lines))
			{
				*scan = '\0';
				write_to_buffer (d, buffer, strlen (buffer));
				
				for (chk = d->showstr_point; isspace (*chk); chk++);
				{
					if (!*chk)
					{
						if (d->showstr_head)
						{
							free_mem (d->showstr_head, strlen (d->showstr_head));
							d->showstr_head = 0;
						}
						
						d->showstr_point = 0;
					}
				}
				return;
			}
	}
	return;
}


/* quick sex fixer */
void fix_sex (CHAR_DATA * ch)
{
	if (ch->sex < 0 || ch->sex > 2)
		ch->sex = IS_NPC (ch) ? 0 : ch->pcdata->true_sex;
}

void act (const char *format, CHAR_DATA * ch, const void *arg1,
		  const void *arg2, int type, int min_pos)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };
	static char *const parts[] = { "chest", "head", "neck", "face", "arm", "leg", 
									"forearm", "foot", "hand", "ear", "finger", "eye" };
	
	char buf[MSL];
	char fname[MIL];
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	char *i = NULL;
	char *point;
	int iSafety = 0;
	
    /* Discard null and zero-length messages. */
	if (format == NULL || format[0] == '\0')
		return;
	
	/* discard null rooms and chars */
	if (ch == NULL || ch->in_room == NULL)
		return;
	
	to = ch->in_room->people;
	if (type == TO_VICT)
	{
		if (vch == NULL)
		{
			bugf ("[comm.c::act] null vch with TO_VICT.");
			return;
		}
		
		if (vch->in_room == NULL)
			return;
		
		to = vch->in_room->people;
	}
	
	for (; to != NULL; to = to->next_in_room)
	{
		if ( ++iSafety > 500 )
		  break;

		if (IS_NPC (to) && to->desc == NULL && !HAS_TRIGGER (to, TRIG_ACT))
			continue;
		else
			if (!IS_NPC (to) && to->desc == NULL)
				continue;
			
			/* place code */
			if ((type == TO_PLACE) &&
				((to == ch) || (!ch->on || !to->on) || (ch->on != to->on)))
				continue;
			
			if ((type == TO_CHAR) && to != ch)
				continue;
			if (type == TO_VICT && (to != vch || to == ch))
				continue;
			if (type == TO_ROOM && (to == ch || !in_same_world(to, ch)) )
				continue;
			if (type == TO_NOTVICT && (to == ch || to == vch || !in_same_world(to,ch)))
				continue;
			
			/* make a special exemption for wounded positions */
			if (to->position == POS_MORTAL ||
				to->position == POS_STUNNED || to->position == POS_INCAP)
			{
				if (min_pos != POS_RESTING && min_pos > to->position)
					continue;
			}
			else
				if (min_pos > to->position)
					continue;
				
				point = buf;
				str = format;
				
				while (*str != '\0')
				{
					if (*str != '$')
					{
						*point++ = *str++;
						continue;
					}
					++str;
					
					if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
					{
						bugf ("[comm.c::act] missing arg2 for code %d.", *str);
						i = " <@@@> ";
					}
					else
					{
						switch (*str)
						{
							default:
								{
									char buf[3];
									
									buf[0] = '$';
									buf[1] = *str;
									buf[2] = '\0';
									i = str_dup (buf);
								}
								break;
							/* Thx Alex for 't' idea */
							case '!':
								i = "\n\r";
								break;
							case 'B':
								i = parts[number_range (0, 11)];
								break;
							case 't':
								i = (char *) arg1;
								break;
							case 'T':
								i = (char *) arg2;
								break;
							case 'n':
								i = PERS (ch, to);
								break;
							case 'N':
								i = PERS (vch, to);
								break;
							case 'e':
								i = he_she[URANGE (0, ch->sex, 2)];
								break;
							case 'E':
								i = he_she[URANGE (0, vch->sex, 2)];
								break;
							case 'm':
								i = him_her[URANGE (0, ch->sex, 2)];
								break;
							case 'M':
								i = him_her[URANGE (0, vch->sex, 2)];
								break;
							case 's':
								i = his_her[URANGE (0, ch->sex, 2)];
								break;
							case 'S':
								i = his_her[URANGE (0, vch->sex, 2)];
								break;
								
							case 'p':
								i = can_see_obj (to, obj1) ? obj1->short_descr : "something";
								break;
								
							case 'P':
								i = can_see_obj (to, obj2) ? obj2->short_descr : "something";
								break;
								
							case 'd':
								if (arg2 == NULL || ((char *) arg2)[0] == '\0')
								{
									i = "door";
								}
								else
								{
									one_argument ((char *) arg2, fname);
									i = fname;
								}
								break;
							}
					}
					
					if (IS_NULLSTR (i))
					{
						i = "*bug -- report me*";
						bugf("[comm.c::act] NULL 'i' variable.");
					}
					
					++str;
					while ((*point = *i) != '\0')
						++point, ++i;
				}
				
				*point++ = '\n';
				*point++ = '\r';
				*point = '\0';
				buf[0] = UPPER (buf[0]);
				
				if (to->desc)
				{
					if (to->desc->connected == CON_PLAYING)
						write_to_buffer (to->desc, buf, point - buf);
				}
				
				else
					if (MOBtrigger &&
						(!IS_FLAG (to->states.config, _AFK)
						|| !IS_FLAG (to->states.config, _BUILDING)))
						mp_act_trigger (buf, to, ch, arg1, arg2, TRIG_ACT);
  }
  return;
}
