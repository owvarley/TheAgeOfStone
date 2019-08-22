/**
 * @@ Copyright (Wheel of Time Project)
 *
 * Wheel of Time (WoT) Mud System v0.1a
 * Copyright (c) 1998, by Gary McNickle <gary@tarmongaidon.org>
 * Based on ROM Mud, v2.4b4 by Russ Taylor
 *
 * By using any of the WoT source code, you agree to follow the terms of
 * usage and liscensing detailed in the file "liscense.txt" as well as any
 * and all liscensing required for the ROM system.
 *
 * The author(s) retain full copyright to this package and are granting you a
 * non-exclusive right to use it.  Any distrobutions or changes to this code
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
 * @@ Copyright (1998, Defiant -- Rob Siemborski -- mud@towers.crusoe.net)
 */

#if defined(WIN32)
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "merc.h"

#define WWWBKGCLR 	"\"#000000\""
#define WWWTXTCLR 	"\"#FFFFFF\""
#define WWWREFRURL  "\"http://tarmongaidon.org:5001"
/*
#define WWWREFRURL  "\"http://localhost:5121"
*/
#define WWWMAINURL  "default.htm"
#define WWWIMGSRC1	"\"http://www.tarmongaidon.org/images/wotbanner.jpg\""
#define WWWIMGSRC2	"\"http://www.tarmongaidon.org/images/flame2.jpg\""
#define WWWIMGALT1	"\"Wheel of Time Project\""
#define WWWIMGALT2	"\"Flaming Dragon, Copyright 1997 by Dragonluv.\""

#define WWWTBLBOR	1				/* Border size                  */
#define WWWTBLBGCOL	"\"#000000\""	/* Table Background color       */
#define WWWTBLFNCOL	"\"#FFFFFF\""	/* Table Font color             */
#define WWWLVLCOL	"\"#FFFFFF\""	/* Level Color                  */
#define WWWCLSCOL	"\"#FFFFFF\""	/* Class Color                  */
#define WWWNATCOL	"\"#FFFFFF\""	/* Race Color                   */
#define WWWGLDCOL	"\"#FFFFFF\""	/* Guild Color                  */
#define WWWTTLCOL	"\"#FFFFFF\""	/* Title Color                  */
#define WWWNMECOL	"\"#FFFFFF\""	/* Name Color                   */


/* Prototypes */
int send_buf (int fd, const char *buf);
void handle_web_request (WEB_DESCRIPTOR * wdesc);
void handle_web_who_request (WEB_DESCRIPTOR * wdesc);
void handle_web_help_request (WEB_DESCRIPTOR * wdesc, char *help);
void handle_web_memory_request (WEB_DESCRIPTOR * wdesc);

/* The mark of the end of a HTTP/1.x request */
const char ENDREQUEST[5] = { 13, 10, 13, 10, 0 };	/* (CRLFCRLF) */

/* Locals */
WEB_DESCRIPTOR *web_descs;
int sockfd;

bool init_web (int port)
{
	struct sockaddr_in my_addr;
	
	web_descs = NULL;
	
	logf ("Web server is active on port: %d", port);
	
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror ("web-socket");
		return FALSE;
	}
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons (port);
	my_addr.sin_addr.s_addr = htons (INADDR_ANY);
	memset (&(my_addr.sin_zero), 0, 8);
	
	if ((bind (sockfd, (struct sockaddr *) &my_addr, sizeof (struct sockaddr)))
		== -1)
	{
		perror ("web-bind");
		return FALSE;
	}
	
	/* Only listen for 5 connects at once, do we really need more? */
	listen (sockfd, 5);
	return TRUE;
}

struct timeval ZERO_TIME = { 0, 0 };

void handle_web (void)
{
	int max_fd;
	WEB_DESCRIPTOR *current, *prev, *next;
	fd_set readfds;
	
	if (!webUP)
		return;
	
	FD_ZERO (&readfds);
	FD_SET (sockfd, &readfds);
	
	/* it *will* be atleast sockfd */
	max_fd = sockfd;
	
	/* add in all the current web descriptors */
	for (current = web_descs; current != NULL; current = current->next)
	{
		FD_SET (current->fd, &readfds);
		if (max_fd < current->fd)
			max_fd = current->fd;
	}
	
	/* Wait for ONE descriptor to have activity */
	select (max_fd + 1, &readfds, NULL, NULL, &ZERO_TIME);
	
	if (FD_ISSET (sockfd, &readfds))
	{
		/* NEW CONNECTION -- INIT & ADD TO LIST */
		current = new_web_desc ();
		current->sin_size = sizeof (struct sockaddr_in);
		current->request[0] = '\0';
		
		if (
			(current->fd =
			accept (sockfd, (struct sockaddr *) &(current->their_addr),
			&(current->sin_size))) == -1)
		{
			perror ("web-accept");
			_quit (1);
		}
		
		current->next = web_descs;
		web_descs = current;
		
		/* END ADDING NEW DESC */
	}
	
	/* DATA IN! */
	for (current = web_descs; current != NULL; current = current->next)
	{
		if (FD_ISSET (current->fd, &readfds))	/* We Got Data! */
		{
			char buf[1024];
			int numbytes;
			
#if !defined(_WIN32)
			if ((numbytes = read (current->fd, buf, sizeof (buf))) == -1)
#else
				if ((numbytes = recv (current->fd, buf, sizeof (buf), 0)) == -1)
#endif
				{
					perror ("web-read");
					_quit (1);
				}
				
				buf[numbytes] = '\0';
				
				strcat (current->request, buf);
		}
	}				/* DONE WITH DATA IN */
	
	/* DATA OUT */
	for (current = web_descs; current != NULL; current = next)
	{
		next = current->next;
		
		/* 1.x request (vernum on FIRST LINE) */
		if (strstr (current->request, "HTTP/1.")
			&& strstr (current->request, ENDREQUEST))
			handle_web_request (current);
		else
			/* HTTP/0.9 (no ver number) */
			if (!strstr (current->request, "HTTP/1.")
				&& strchr (current->request, '\n'))
				handle_web_request (current);
			else
			{
				continue;			/* Don't have full request yet! */
			}
			
#if defined(_WIN32)
			closesocket (current->fd);
#else
			close(current->fd);
#endif
			
			if (web_descs == current)
			{
				web_descs = current->next;
			}
			else
			{
				for (prev = web_descs; prev->next != current; prev = prev->next)
					;			/* Just ititerate through the list */
				prev->next = current->next;
				web_descs = prev;
			}
			
			free_web_desc (current);
	}				/* END DATA-OUT */
}

/* Generic Utility Function */

int send_buf (int fd, const char *buf)
{
	return send (fd, buf, strlen (buf), 0);
}

char *fix_help (char *name, bool restore)
{
	char buf[MIL];
	int i = 0;
	
	memset (buf, 0, MIL);
	memcpy (buf, name, strlen (name));
	
	if (restore)
	{
		for (i = 0; buf[i] != '\0'; i++)
			switch (buf[i])
		{
      case '!':
		  buf[i] = '\'';
		  break;
      case '%':
		  buf[i] = ' ';
		  break;
      default:
		  break;
		}
	}
	else
		
	{
		for (i = 0; buf[i] != '\0'; i++)
			switch (buf[i])
		{
      case '\'':
		  buf[i] = '!';
		  break;
      case ' ':
		  buf[i] = '%';
		  break;
      default:
		  break;
		}
	}
	
	return str_dup (buf);
	
}


char *request (char *help)
{
	/* Expect: "GET 'req' HTTP" */
	
	char arg[MSL];
	char req[MSL];
	
	memset (arg, 0, MSL);
	memset (req, 0, MSL);
	
	help = one_argument (help, arg);
	
	while (!strstr (arg, "http"))
	{
		if (!strstr (arg, "http") && !strstr (arg, "get"))
		{
			if (req[0] != '\0')
			{
				char tmp[MSL];
				
				strcpy (tmp, req);
				sprintf (req, "%s ", tmp);
			}
			
			if (arg[0] == '/')
				strcat (req, &arg[1]);
			else
				strcat (req, arg);
		}
		
		help = one_argument (help, arg);
	}
	
	return fix_help (req, TRUE);
}

char *txt2html (char *orig)
{
/* purpose: to convert \n\r to <br>, and also to strip out any 
* mud ansi codes.
	*/
	
	char buf[MSL];
	int i = -1;
	
	memset (buf, 0, MSL);
	
	while (*orig != '\0')
	{
		if (i >= MSL)
			return strip_codes (buf);
		
		if (*orig == '<')
		{
			strcat (buf, "&lt;");
			i += 4;
		}
		else if (*orig == '>')
		{
			strcat (buf, "&gt;");
			i += 4;
		}
		else
			if (*orig != '\r')
				buf[++i] = *orig;
			
			orig++;
	}
	
	return strip_codes (buf);
}


void handle_web_request (WEB_DESCRIPTOR * wdesc)
{
	/* process request */
	/* are we using HTTP/1.x? If so, write out header stuff.. */
	char *req = NULL;
	
	if (!strstr (wdesc->request, "GET"))
	{
		send_buf (wdesc->fd, "HTTP/1.0 501 Not Implemented");
		return;
	}
	else
		if (strstr (wdesc->request, "HTTP/1."))
		{
			send_buf (wdesc->fd, "HTTP/1.0 200 OK\n");
			send_buf (wdesc->fd, "Content-type: text/html\n\n");
		}
		
		req = request (wdesc->request);
		
		/* Handle the actual request */
		if (!str_cmp (req, "wholist"))
		{
			log_string ("Web Hit: WHOLIST");
			handle_web_who_request (wdesc);
		}
		else
			if (!str_cmp (req, "memstate"))
			{
				log_string ("Web Hit: Memory");
				handle_web_memory_request (wdesc);
			}
			else
				if (!str_cmp (req, "help"))
				{
					log_string ("Web Hit: Help Index");
					handle_web_help_request (wdesc, "");
				}
				else
					
				{
					logf ("Web Hit: Help [%s]", req);
					handle_web_help_request (wdesc, req);
				}
				
}

void shutdown_web (void)
{
	WEB_DESCRIPTOR *current, *next;
	
	if (!webUP)
		return;
	
	/* Close All Current Connections */
	for (current = web_descs; current != NULL; current = next)
	{
		next = current->next;
#if defined(_WIN32)
		closesocket (current->fd);
#else
		close(current->fd);
#endif
		free_web_desc (current);
	}
	
	if ( current )
		/* Stop Listening */
#if defined(_WIN32)
		closesocket (current->fd);
#else
	close(current->fd);
#endif
}

void handle_web_memory_request (WEB_DESCRIPTOR * wdesc)
{
	char buf[MSL];
	extern int mobile_count, nAllocString, sAllocString, nAllocPerm, sAllocPerm;
	
	sprintf (buf,
		"<HTML>\n"
		"<HEAD>\n"
		"   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
		"   <META NAME=\"Author\" CONTENT=\"Gary McNickle\">\n"
		"   <meta name=\"GENERATOR\" content=\"WoT Mud Project v%s\">\n"
		"   <TITLE>Memory Status for %s</TITLE>\n"
		"</HEAD>\n\n"
		"<body bgcolor=%s text=%s>\n\n"
		"<DL>"
		"<DD ALIGN=\"center\"><TABLE BORDER=\"0\">\n"
		"<TR>\n"
		"  <TD><img src=%s alt=%s></TD>"
		"  <TD><img src=%s alt=%s></TD>"
		"</TR>\n" "</TABLE><p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p>\n\n", versionToString (VERSION),	/* for generator content        */
		pMud.mud_name,		/* for page title header        */
		WWWBKGCLR,		/* html body background color   */
		WWWTXTCLR,		/* html body text color         */
		WWWIMGSRC1, WWWIMGALT1, WWWIMGSRC2, WWWIMGALT2);
	
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<div align=\"center\"><center><table border=\"1\" width=\"50%%\">\n"
		"<caption><strong>Memory Status for %s.  Accurate as of: %s"
		"</strong></caption>\n", pMud.mud_name, (char *) ctime (&current_time));
	
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Affects</TD><TD>%d</TD></TR>\n", top_affect);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Areas</TD><TD>%d</TD></TR>\n", top_area);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>ExDes</TD><TD>%d</TD></TR>\n", top_ed);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Exits</TD><TD>%d</TD></TR>\n", top_exit);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Helps</TD><TD>%d</TD></TR>\n", top_help);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Socials</TD><TD>%d</TD></TR>\n", maxSocial);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Mobs</TD><TD>%d</TD></TR>\n", top_mob_index);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>(In Use)</TD><TD>%d</TD></TR>\n", mobile_count);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Objects</TD><TD>%d</TD></TR>\n", top_obj_index);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Resets</TD><TD>%d</TD></TR>\n", top_reset);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Rooms</TD><TD>%d</TD></TR>\n", top_room);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Shops</TD><TD>%d</TD></TR>\n", top_shop);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Sounds</TD><TD>%d</TD></TR>\n", top_sound);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Restricted Names</TD><TD>%d</TD></TR>\n", top_name);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Strings</TD><TD>%d of %7d byes (max %d)</TD></TR>\n",
		nAllocString, sAllocString, MAX_STRING);
	send_buf (wdesc->fd, buf);
	
	sprintf (buf,
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Perms</TD><TD>%d blocks of %7d bytes.</TD></TR>\n",
		nAllocPerm, sAllocPerm);
	send_buf (wdesc->fd, buf);
	
	
	sprintf (buf,
		"</TABLE></DIV></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
	
	send_buf (wdesc->fd, buf);
}

void handle_web_help_request (WEB_DESCRIPTOR * wdesc, char *help)
{
	char buf[MSL];
	int cnt = 0;
	HELP_DATA *pHelp;
	
	sprintf (buf,
		"<HTML>\n"
		"<HEAD>\n"
		"   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
		"   <META NAME=\"Author\" CONTENT=\"Gary McNickle\">\n"
		"   <meta name=\"GENERATOR\" content=\"WoT Mud Project v%s\">\n"
		"   <TITLE>Help Files for %s</TITLE>\n"
		"</HEAD>\n\n"
		"<body bgcolor=%s text=%s>\n\n"
		"<DL>"
		"<DD ALIGN=\"center\"><TABLE BORDER=\"0\">\n"
		"<TR>\n"
		"  <TD><img src=%s alt=%s></TD>"
		"  <TD><img src=%s alt=%s></TD>"
		"</TR>\n" "</TABLE><p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p>\n\n", versionToString (VERSION),	/* for generator content        */
		pMud.mud_name,		/* for page title header        */
		WWWBKGCLR,		/* html body background color   */
		WWWTXTCLR,		/* html body text color         */
		WWWIMGSRC1, WWWIMGALT1, WWWIMGSRC2, WWWIMGALT2);
	
	send_buf (wdesc->fd, buf);
	
	/* Help index request */
	if (help == NULL || help[0] == '\0' || !str_prefix (help, "help"))
	{
		
		sprintf (buf,
			"<p><br></p>\n<table border=\"1\" width=\"100%%\">\n"
			"<caption><strong>Help Index for %s. Accurate as of: %s"
			"</strong></caption>\n",
			pMud.mud_name, (char *) ctime (&current_time));
		
		send_buf (wdesc->fd, buf);
		
		cnt = 1;
		
		for (pHelp = help_first; pHelp; pHelp = pHelp->next)
		{
			char *str = pHelp->keyword;
			char key[MSL];
			
			memset (key, 0, MSL);
			
			while (str[0] != '\0')
			{
				str = one_argument (str, key);
				
				if (key[0] != '\0')
					sprintf (buf,
					"%s"
					"    <td width=\"25%%\"><a href=%s/%s\">"
					"<font color=%s>%s</font></a></td>\n"
					"%s",
					(cnt == 1) ? "<tr>\n" : "",
					WWWREFRURL,
					fix_help (pHelp->keyword, FALSE),
					WWWNMECOL, key, (cnt == 4) ? "</tr>\n" : "");
				
				send_buf (wdesc->fd, buf);
				cnt++;
				
				if (cnt > 4)
					cnt = 1;
			}
		}
		
		sprintf (buf,
			"<p>&nbsp;</p><p>&nbsp;</p>\n"
			"</table>\n"
			"<p align=\"center\">[ <a href=%s>Return to Main</a> ] </p>"
			"</dd><br></dl><br></body><br></html>", WWWMAINURL);
		
		send_buf (wdesc->fd, buf);
		
	}				/* End: Help Index Request */
	
	else
		
	{
		bool pFound = FALSE;
		
		for (pHelp = help_first; pHelp; pHelp = pHelp->next)
			if (!str_cmp (pHelp->keyword, help))
			{
				sprintf (buf,
					"<h3>[%3d]&nbsp;&nbsp;%s</h3><br><pre>%s</pre>\n<p>&nbsp;</p>\n",
					pHelp->level, pHelp->keyword, txt2html (pHelp->text));
				
				pFound = TRUE;
				break;
			}
			
			if (!pFound)
			{
				sprintf (buf, "<center>No Such help.<br>"
					"<a href=%s/help\">"
					"[ Return to the index ]</a></center>\n", WWWREFRURL);
			}
			
			send_buf (wdesc->fd, buf);
	}
	
	sprintf (buf,
		"</TABLE></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
	
	send_buf (wdesc->fd, buf);
}

void handle_web_who_request (WEB_DESCRIPTOR * wdesc)
{
	CHAR_DATA *vch;
	char output[MSL];
	DESCRIPTOR_DATA *d;
	
	sprintf (output,
		"<HTML>\n"
		"<HEAD>\n"
		"   <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
		"   <META NAME=\"Author\" CONTENT=\"Gary McNickle\">\n"
		"   <meta name=\"GENERATOR\" content=\"WoT Mud Project v%s\">\n"
		"   <TITLE>Who's online at %s</TITLE>\n"
		"</HEAD>\n\n"
		"<body bgcolor=%s text=%s>\n\n"
		"<DL>"
		"<DD ALIGN=\"center\"><TABLE BORDER=\"0\">\n"
		"<TR>\n"
		"  <TD><img src=%s alt=%s></TD>\n"
		"  <TD><img src=%s alt=%s></TD>\n"
		"</TR>\n"
		"</TABLE>\n\n"
		"<P>&nbsp;</P>\n\n"
		"<H1><CENTER>Who's Online at %s</CENTER></H1><BR>\n\n"
		"<CENTER><TABLE BORDER=%d BGCOLOR=%s>\n"
		"<TR  FONT COLOR=%s ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD><FONT COLOR=%s>Nationality</TD>\n"
		"<TD><FONT COLOR=%s>Guild</TD>\n"
		"<TD><FONT COLOR=%s>Name</TD>\n"
		"<TD><FONT COLOR=%s>Title</TD>\n" "</TR>\n", versionToString (VERSION),	/* for generator content        */
		pMud.mud_name,		/* for page title header        */
		WWWBKGCLR,		/* html body background color   */
		WWWTXTCLR,		/* html body text color         */
		WWWIMGSRC1, WWWIMGALT1, WWWIMGSRC2, WWWIMGALT2, pMud.mud_name,	/* for displayed title          */
		WWWTBLBOR,
		WWWTBLBGCOL,
		WWWTBLFNCOL, WWWNATCOL, WWWGLDCOL, WWWNMECOL, WWWTTLCOL);
	
	send_buf (wdesc->fd, output);
	
	for (d = descriptor_list; d; d = d->next)
	{
		vch = (d->original) ? d->original : d->character;
		
		if (d->connected != CON_PLAYING || vch->invis_level > 0 || IS_NPC (vch))
		{
			continue;
		}
		
		
		sprintf (output,
			"<TR ALIGN=LEFT VALIGN=CENTER>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"</TD></TR>\n",
			WWWNATCOL, (vch->pcdata->city) ? vch->pcdata->city :
		(vch->pcdata->nation) ? vch->pcdata->nation : "Andor",
			WWWGLDCOL, is_clan (vch) ? player_clan (vch) : "(none)",
			WWWNMECOL, vch->name,
			WWWTTLCOL,
			!IS_NPC (vch) ? strip_codes (vch->pcdata->title) : vch->short_descr);
		
		send_buf (wdesc->fd, output);
	}
	
	/* 
	* Copyright notice
	* With the exception of the HTML codes, or the link to Tarmon Gaidon
	* being updated on occasion the following text must not be changed, nor
	* may this copyright notice be removed.
	* --Gary McNickle  9/24/98 <gary@tarmongaidon.org>
	*/
	sprintf (output,
		"</TABLE></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
	
	send_buf (wdesc->fd, output);
}
