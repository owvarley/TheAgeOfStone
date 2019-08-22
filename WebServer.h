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


#if defined(WIN32)
  #include <io.h>
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


/* The mark of the end of a HTTP/1.x request */
const char ENDREQUEST[5] = { 13, 10, 13, 10, 0 };	/* (CRLFCRLF) */

#define WWWBKGCLR 	"\"#000000\""
#define WWWTXTCLR 	"\"#FFFFFF\""
#define WWWREFRURL  "\"http://tarmongaidon.org:5001"
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
// #define WWWREFRURL  "\"http://localhost:5121"



typedef enum _WebServerError
{
	_OK,
	_EUNSUPPORTED_PROTOCOL,
	_ENFILE,
	_EMFILE,
	_EACCES,
	_ENOBUFS,
	_ENOMEM,
	_EINVAL,
	_ENOINIT,
	_ENETDOWN,
	_EINUSE,
	_ENOADDR,
	_EINPROGRESS,
	_ENOTSOCK,
	_EOPTNOTSUP,
	_EWOULDBLOCK,
	_ENOTCONN,
	_LAST_ERROR
};

class CWebServer 
{

public: // Methods

	// Constructors
	CWebServer();
	virtual ~CWebServer() {};

	virtual _WebServerError		StartServer(int iPort);
	virtual _WebServerError		StopServer();

	inline	_WebServerError		LastError() { return m_LastError; }
	inline  bool				IsRunning() { return m_bRunning; }

	virtual _WebServerError		Update();

	inline void	ClosePort(int port);
	inline int	ReadSocket(int socket, void* buffer, unsigned int count);

private: // Members
	// Requests
	void						HandleMemoryRequest(WEB_DESCRIPTOR * wdesc);
	void						HandleHelpRequest(WEB_DESCRIPTOR * wdesc, char *help);
	void						HandleWhoRequest(WEB_DESCRIPTOR * wdesc);
	void						HandleWebRequest(WEB_DESCRIPTOR* desc);

	int							WriteToSocket(int socket, char* fmt, ...);
	virtual	_WebServerError		ReportError(char* szCaller, int iError);
	virtual	_WebServerError		MapError(int iError);

	// Helpers
	char*						FixHelp(char* name, bool bRestore);
	char*						Request(char* help);
	char*						Text2HTML(char* orig);

	// Memory handling
	void						RecycleDescriptor(WEB_DESCRIPTOR* desc);
	WEB_DESCRIPTOR*				CreateDescriptor();


public: // Data

	static char* szSockErr[];
	_WebServerError m_LastError;

private: 
	bool			m_bRunning;
	bool			m_bDebugging;
	int				m_iDesc;
	WEB_DESCRIPTOR* m_Open;
};
