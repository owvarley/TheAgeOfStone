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
 * @@ Objective (c++wrapper.c)
 *
 * This file contains wrapper functions for the few C++ classes that WoTMud uses.
 * Occassionaly, we will need to access a c++ class from one of the core .c files. Unfortunately,
 * since these do not compile (under linux) with c++ defined, and attempting to force them to
 * is problematic, wrapping these functions under a .cpp file is the easist workaround.
 */

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

static CWebServer Web;

bool init_web_server(int iPort)
{
  return ( Web.StartServer(iPort) == (_WebServerError)_OK );
}

void update_web_server()
{
  if ( Web.IsRunning() )
    Web.Update();
}

void shutdown_web_server()
{
  if ( Web.IsRunning() )
	Web.StopServer();
}

