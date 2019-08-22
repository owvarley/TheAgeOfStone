
#if defined(WIN32)
  #include <winsock.h>
  #include <time.h>
  #include <memory.h>
#else
  #include <sys/time.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdarg.h>

#include "merc.h"
#include "WebServer.h"



char* CWebServer::szSockErr[] = {
	"OK",
	"Protocol not supported", 
	"Insuffecient memory to allocate socket", 
	"Process file table overflow error",
	"Permission to create socket denied",
	"Insuffecient memory available",
	"Insuffecient resources available",
	"Not Initialized",
	"Network down",
	"Address allready in use",
	"Address not available",
	"A blocking socket call is in progress",
	"Descriptor is not a socket",
	"Option not supported.",
	"Socket marked as no blocking, and no connections available",
	"The socket is not connected.",
	"Unknown protocol, or protocol not available.", 
	NULL
 };

CWebServer::CWebServer()
{
	m_LastError = _OK;
	m_iDesc = port + 1;
	m_Open = NULL;
	m_bDebugging = false;
}

void CWebServer::ClosePort(int port)
{
#if !defined(WIN32)
  close(port);
#else
  closesocket(port);
#endif
}

int CWebServer::ReadSocket(int socket, void* buffer, unsigned int count)
{
#if defined (WIN32)
  return recv(socket, (char*)buffer, count,0);
#else
  return read(socket, buffer, count);
#endif
}

WEB_DESCRIPTOR* CWebServer::CreateDescriptor()
{
	WEB_DESCRIPTOR* wDesc = (WEB_DESCRIPTOR*)malloc( sizeof(*wDesc) );

   	top_web_desc++;
	wDesc->next = NULL;
	wDesc->sin_size = sizeof (struct sockaddr_in);
	wDesc->request[0] = '\0';
	VALIDATE (wDesc);

	return wDesc;
}

void CWebServer::RecycleDescriptor(WEB_DESCRIPTOR *desc)
{
	if ( !desc || !desc->valid || top_web_desc < 1 )
		return;

	INVALIDATE(desc);
	free( desc );
	desc = NULL;

	top_web_desc--;
}

_WebServerError CWebServer::Update()
{
	int max_fd;
	struct timeval null_time = { 0, 0 };
	WEB_DESCRIPTOR *current, *next;
	fd_set readfds;
	
	if (!m_bRunning)
		return _OK;
	
	FD_ZERO (&readfds);
	FD_SET (m_iDesc, &readfds);
	
	max_fd = m_iDesc;
	
	/* add in all the current web descriptors */
	for (current = m_Open; current != NULL; current = current->next)
	{
		FD_SET (current->fd, &readfds);

		if (max_fd < current->fd)
			max_fd = current->fd;
	}
	
	// Determine the status of incomming sockets
	select (max_fd + 1, &readfds, NULL, NULL, &null_time);
	
	// Do we have a new socket?
	if (FD_ISSET (m_iDesc, &readfds))
	{
		int iStatus = 0;
		current = CreateDescriptor();
		m_bDebugging = true;
		
#if defined(WIN32)
		iStatus = accept (m_iDesc, (struct sockaddr *)&(current->their_addr), 
			&(current->sin_size));
#else
		iStatus = accept (m_iDesc, (struct sockaddr *)&(current->their_addr), 
			&((socklen_t)current->sin_size));
#endif
		if ( iStatus < 0 )
			return ReportError("[CWebServer::Update<accept>]", iStatus );
		else
			current->fd = iStatus;
		
		current->next = m_Open;
		m_Open = current;
	}
	
	// Do we have incomming requests on our open sockets?
	for (current = m_Open; current != NULL; current = current->next)
	{
		if (FD_ISSET (current->fd, &readfds))
		{
			char buf[1024];
			int numbytes;
			
			memset(buf, 0, 1024);
			numbytes = ReadSocket(current->fd, buf, sizeof(buf));

			if ( numbytes < 0 )
				return ReportError("[CWebServer::Update<read>]", numbytes );

			strcat (current->request, buf);
		}
	}
	
	// Do we have data waiting to send out the open sockets?
	for (current = m_Open; current != NULL; current = next)
	{
		next = current->next;
		
		// 1.x request (vernum on FIRST LINE)
		if (strstr (current->request, "HTTP/1.") && strstr (current->request, ENDREQUEST))
			HandleWebRequest(current);
		else
		// HTTP/0.9 (no ver number)
		if (!strstr (current->request, "HTTP/1.") && strchr (current->request, '\n'))
			HandleWebRequest(current);
		else 
		// We dont have a complete request yet, return.
			continue;			

		// Close the open HTTP port immediately after handling the request.
		ClosePort(current->fd);

		if (m_Open == current)
			m_Open = current->next;

		RecycleDescriptor(current);
	}				

	return m_LastError;
}

_WebServerError	CWebServer::StartServer(int iPort)
{
	int iStatus = 0, x;

	struct sockaddr_in my_addr;

	if ( (m_iDesc = socket (AF_INET, SOCK_STREAM, 0)) == -1)
		return ReportError("[CWebServer::StartServer<socket>]", m_iDesc ); // m_iDesc holds our error# if we are in linux...
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons (iPort);
	my_addr.sin_addr.s_addr = htons (INADDR_ANY);
	memset (&(my_addr.sin_zero), 0, 8);

	iStatus = setsockopt(m_iDesc, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof(x) );
	if ( iStatus < 0 )
		return ReportError("[CWebServer::StartServer<setsockopt>]", iStatus);

	iStatus = bind(m_iDesc, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
	if ( iStatus < 0 )
		return ReportError("[CWebServer::StartServer<bind>]", iStatus);

	iStatus = listen (m_iDesc, 5);
	if ( iStatus < 0 )
		return ReportError("[CWebServer::StartServer<listen>]", iStatus);

	logf ("[CWebServer::StartServer] Active on port: %d", m_iDesc);
	
	m_bRunning = TRUE;
	m_LastError = _OK;
	return m_LastError;
}

_WebServerError	CWebServer::StopServer()
{
	int iStatus = 0;

	WEB_DESCRIPTOR *current, *next;
	
	if (!m_bRunning)
		return _OK;
	
	/* Close All Current Connections */
	for (current = m_Open; current != NULL; current = next)
	{
		next = current->next;

		ClosePort(current->fd);
		RecycleDescriptor(current);
	}
	
	if ( current ) // Stop Listening
		ClosePort(current->fd);

	m_bRunning = FALSE;
	m_LastError = MapError(iStatus);
	return m_LastError;
}


char* CWebServer::FixHelp(char *name, bool restore)
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

char* CWebServer::Request(char* help)
{
	// Expect: "GET 'req' HTTP"
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
	
	return FixHelp(req, TRUE);
}

// purpose: to convert \n\r to <br>, and also to strip out any mud ansi codes.
char* CWebServer::Text2HTML(char* orig)
{
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

int CWebServer::WriteToSocket(int socket, char* fmt, ...)
{
	char buf[2 * MSL];
	va_list args;
	va_start (args, fmt);
	int len;

#if !defined(_WIN32)
	len = vsnprintf (buf, 2*MSL, fmt, args);
#else
	len = _vsnprintf(buf, 2*MSL, fmt, args);
#endif
	va_end (args);
	

	if ( len > 0 )
	  return send(socket, buf, len, 0);
	else
	  return len;
}

void CWebServer::HandleMemoryRequest(WEB_DESCRIPTOR * wdesc)
{
	extern int mobile_count, nAllocString, sAllocString, nAllocPerm, sAllocPerm;

	WriteToSocket(wdesc->fd, 
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
		pMud.mud_name,	
		WWWBKGCLR,		
		WWWTXTCLR,		
		WWWIMGSRC1, 
		WWWIMGALT1, 
		WWWIMGSRC2, 
		WWWIMGALT2);
	
	WriteToSocket(wdesc->fd, 	
		"<div align=\"center\"><center><table border=\"1\" width=\"50%%\">\n"
		"<caption><strong>Memory Status for %s.  Accurate as of: %s"
		"</strong></caption>\n", 
		pMud.mud_name, 
		(char *) ctime (&current_time));
	
	WriteToSocket(wdesc->fd, 	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Affects</TD><TD>%d</TD></TR>\n" 
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Areas</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>ExDes</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Exits</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Helps</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Socials</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Mobs</TD><TD>%d</TD></TR>\n"

		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>(In Use)</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Objects</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Resets</TD><TD>%d</TD></TR>\n"

		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Rooms</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Shops</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Sounds</TD><TD>%d</TD></TR>\n"
	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Restricted Names</TD><TD>%d</TD></TR>\n",
		top_affect, 
		top_area, 
		top_ed, 
		top_exit, 
		top_help, 
		maxSocial, 
		top_mob_index, 
		mobile_count, 
		top_obj_index, 
		top_reset, 
		top_room, 
		top_shop, 
		top_sound, 
		top_name);
	
	WriteToSocket(wdesc->fd, 	
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Strings</TD><TD>%d of %7d byes (max %d)</TD></TR>\n"
		"<TR ALIGN=LEFT VALIGN=CENTER>\n"
		"<TD>Perms</TD><TD>%d blocks of %7d bytes.</TD></TR>\n",
		nAllocString, 
		sAllocString, 
		MAX_STRING,
		nAllocPerm, 
		sAllocPerm);
	
	WriteToSocket(wdesc->fd, 	
		"</TABLE></DIV></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
}

void CWebServer::HandleHelpRequest(WEB_DESCRIPTOR * wdesc, char *help)
{
	int cnt = 0;
	HELP_DATA *pHelp;
	
	WriteToSocket(wdesc->fd, 	
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
		"</TR>\n" "</TABLE><p>&nbsp;</p><p>&nbsp;</p><p>&nbsp;</p>\n\n", 
		versionToString (VERSION),			
		pMud.mud_name,		
		WWWBKGCLR,		
		WWWTXTCLR,		
		WWWIMGSRC1, 
		WWWIMGALT1, 
		WWWIMGSRC2, 
		WWWIMGALT2);
	
	/* Help index request */
	if (help == NULL || help[0] == '\0' || !str_prefix (help, "help"))
	{
		
		WriteToSocket(wdesc->fd, 	
			"<p><br></p>\n<table border=\"1\" width=\"100%%\">\n"
			"<caption><strong>Help Index for %s. Accurate as of: %s"
			"</strong></caption>\n",
			pMud.mud_name, (char *) ctime (&current_time));
		
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
				{
					WriteToSocket(wdesc->fd, 	
						"%s"
						"    <td width=\"25%%\"><a href=%s/%s\">"
						"<font color=%s>%s</font></a></td>\n"
						"%s",
						(cnt == 1) ? "<tr>\n" : "",
						WWWREFRURL,
						FixHelp(pHelp->keyword, FALSE),
						WWWNMECOL, 
						key, (cnt == 4) ? "</tr>\n" : "");
				}				
				cnt++;
				
				if (cnt > 4)
					cnt = 1;
			}
		}
		
		WriteToSocket(wdesc->fd, 	
			"<p>&nbsp;</p><p>&nbsp;</p>\n"
			"</table>\n"
			"<p align=\"center\">[ <a href=%s>Return to Main</a> ] </p>"
			"</dd><br></dl><br></body><br></html>", WWWMAINURL);
		
	}
	
	else
	{
		char buf[MSL*2];
		bool pFound = FALSE;
		
		for (pHelp = help_first; pHelp; pHelp = pHelp->next)
		{
			if (!str_cmp (pHelp->keyword, help))
			{
				sprintf (buf,
					"<h3>[%3d]&nbsp;&nbsp;%s</h3><br><pre>%s</pre>\n<p>&nbsp;</p>\n",
					pHelp->level, pHelp->keyword, Text2HTML(pHelp->text));
				
				pFound = TRUE;
				break;
			}
		}
			
		if (!pFound)
		{
			sprintf (buf, "<center>No Such help.<br>"
				"<a href=%s/help\">"
				"[ Return to the index ]</a></center>\n", WWWREFRURL);
		}
			
		WriteToSocket(wdesc->fd, buf);
	}
	
	WriteToSocket(wdesc->fd,
		"</TABLE></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
}

void CWebServer::HandleWhoRequest(WEB_DESCRIPTOR * wdesc)
{
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	
	WriteToSocket(wdesc->fd,
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
		"<TD><FONT COLOR=%s>Title</TD>\n" "</TR>\n", 
		versionToString (VERSION),	
		pMud.mud_name,		
		WWWBKGCLR,		
		WWWTXTCLR,		
		WWWIMGSRC1, 
		WWWIMGALT1, 
		WWWIMGSRC2, 
		WWWIMGALT2, 
		pMud.mud_name,	
		WWWTBLBOR,
		WWWTBLBGCOL,
		WWWTBLFNCOL, 
		WWWNATCOL, 
		WWWGLDCOL, 
		WWWNMECOL, 
		WWWTTLCOL);
	
	
	for (d = descriptor_list; d; d = d->next)
	{
		vch = (d->original) ? d->original : d->character;
		
		if (d->connected != CON_PLAYING || vch->invis_level > 0 || IS_NPC (vch))
			continue;
		
		WriteToSocket(wdesc->fd,
			"<TR ALIGN=LEFT VALIGN=CENTER>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"<TD><FONT COLOR=%s>%s</TD>\n"
			"</TD></TR>\n",
			WWWNATCOL,	(vch->pcdata->city) ? vch->pcdata->city :
						(vch->pcdata->nation) ? vch->pcdata->nation : "Andor",
			WWWGLDCOL, 
			is_clan (vch) ? player_clan (vch) : "(none)",
			WWWNMECOL, 
			vch->name,
			WWWTTLCOL,
			!IS_NPC (vch) ? strip_codes (vch->pcdata->title) : vch->short_descr);
	}
	
   /* 
	* Copyright notice
	* With the exception of the HTML codes, or the link to Tarmon Gaidon
	* being updated on occasion the following text must not be changed, nor
	* may this copyright notice be removed.
	* --Gary McNickle  9/24/98 <gary@tarmongaidon.org>
	*/
	WriteToSocket(wdesc->fd,	
		"</TABLE></DD></CENTER></FONT>\n\n"
		"<P><BR><BR><BR><BR><BR></P>\n\n"
		"<CENTER>This page automatically generated by the WoT Mud System<br>"
		"Copyright © 1998-2001, Gary McNickle<br>"
		"<A HREF=\"telnet://tarmongaidon.org:5000\">Tarmon Gaidon</A> "
		"is the Offical home of WoT Mud.</CENTER><br>"
		"</DL>\n</BODY>\n</HTML>\n");
}


void CWebServer::HandleWebRequest(WEB_DESCRIPTOR* wdesc)
{
	char *req = NULL;
	
	if (!strstr (wdesc->request, "GET"))
	{
		WriteToSocket(wdesc->fd, "HTTP/1.0 501 Not Implemented");
		return;
	}
	else
	if (strstr (wdesc->request, "HTTP/1."))
	{
		WriteToSocket (wdesc->fd, "HTTP/1.0 200 OK\n"
			                      "Content-type: text/html\n\n");
	}
	
	req = Request(wdesc->request);
	
	if (!str_cmp (req, "wholist"))
	{
		logf("[CWebServer::HandleWebRequest] Requested: Who listing");
		HandleWhoRequest(wdesc);
	}
	else
	if (!str_cmp (req, "memstate"))
	{
		logf("[CWebServer::HandleWebRequest] Requested: Memory status");
		HandleMemoryRequest(wdesc);
	}
	else
	if (!str_cmp (req, "help"))
	{
		logf("[CWebServer::HandleWebRequest] Requested: Help index");
		HandleHelpRequest(wdesc, "");
	}
	else
	{
		logf("[CWebServer::HandleWebRequest] Requested: Help [%s]", req);
		HandleHelpRequest(wdesc, req);
	}
		
}

_WebServerError CWebServer::ReportError(char* szCaller, int iError)
{
	if ( !szCaller || szCaller[0] == '\0' )
		szCaller = "[CWebServer::ReportError]";

#if defined(WIN32)
	iError = WSAGetLastError();
#endif

	_WebServerError State = MapError(iError);

	if ( State == _OK )
		return State;

	m_LastError = State;
	bugf("%s %s.", szCaller, szSockErr[State]);
	return State;
}

_WebServerError CWebServer::MapError(int iError)
{
	_WebServerError State = _OK;

// Linux Errors
#if !defined(_WIN32)
	
	switch ( iError )
	{
		case EPROTONOSUPPORT:
			State = _EUNSUPPORTED_PROTOCOL; break;
		case ENFILE:
			State = _ENFILE; break;
		case EMFILE:
			State = _EMFILE; break;
		case EACCES:
			State = _EACCES; break;
		case ENOBUFS:
			State = _ENOBUFS; break;
		case ENOMEM:
			State = _ENOMEM; break;
		case ENOTSOCK:
		case EBADF:
			State = _ENOTSOCK; break;
		case EINVAL:
		default:
			State = _EINVAL; break;
	}	

#else
	switch ( iError )
	{
		case WSAEPROCLIM: 
			State = _ENFILE; break;
		case WSANOTINITIALISED:
			State = _ENOINIT; break;
		case WSAENETDOWN:
			State = _ENETDOWN; break;
		case WSAEACCES:
			State = _EACCES; break;
		case WSAEADDRINUSE:
			State = _EINUSE; break;
		case WSAEADDRNOTAVAIL:
			State = _ENOADDR; break;
		case WSAEMFILE:
			State = _EMFILE; break;
		case WSAENOBUFS:
			State = _ENOBUFS; break;
		case WSAEINPROGRESS:
			State = _EINPROGRESS; break;
		case WSAENOTSOCK: break;
			State = _ENOTSOCK; break;
		case WSAEOPNOTSUPP:
			State = _EOPTNOTSUP; break;
		case WSAEWOULDBLOCK:
			State = _EWOULDBLOCK; break;
		case WSAENOTCONN:
			State = _ENOTCONN; break;
		case WSAEPROTOTYPE:
		case WSAEINVAL:
		default:
			State = _EINVAL; break;
	}
#endif
	return State;
}
