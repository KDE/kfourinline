/***************************************************************************
                          KRSocket.cpp  -  description
                             -------------------
    begin                : Tue May 2 2000
    copyright            : (C) 2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Tue Apr 4 2000
    copyright            : (C) |1995-2000 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1997 Torben Weis (weis@kde.org)
 *
 *  $Id$
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
// on Linux/libc5, this includes linux/socket.h where SOMAXCONN is defined
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/un.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include "KRSocket.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
// defines MAXDNAME under Solaris
#include <arpa/nameser.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#ifdef HAVE_SYSENT_H
#include <sysent.h>
#endif

#if TIME_WITH_SYS_TIME
#include <time.h>
#endif

// Play it safe, use a reasonable default, if SOMAXCONN was nowhere defined.
#ifndef SOMAXCONN
#warning Your header files do not seem to support SOMAXCONN
#define SOMAXCONN 5
#endif

#include <qapplication.h>
#include <qsocketnotifier.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108 // this is the value, I found under Linux
#endif

KRServerSocket::KRServerSocket( const char *_path, int optname, int value, int level) :
  notifier( 0L ), sock( -1 )
{
  domain = PF_UNIX;
  
  if ( !init ( _path,optname,value,level ) )
  {
    fatal("Error constructing PF_UNIX domain server socket\n");
    return;
  }
    
  notifier = new QSocketNotifier( sock, QSocketNotifier::Read );
  connect( notifier, SIGNAL( activated(int) ), this, SLOT( slotAccept(int) ) );
}

KRServerSocket::KRServerSocket( const char *_path ) :
  notifier( 0L ), sock( -1 )
{
  domain = PF_UNIX;
  
  if ( !init ( _path ) )
  {
    fatal("Error constructing PF_UNIX domain server socket\n");
    return;
  }
    
  notifier = new QSocketNotifier( sock, QSocketNotifier::Read );
  connect( notifier, SIGNAL( activated(int) ), this, SLOT( slotAccept(int) ) );
}

KRServerSocket::KRServerSocket( unsigned short int _port ) :
  notifier( 0L ), sock( -1 )
{
  domain = PF_INET;

  if ( !init ( _port ) )
  {
    // fatal("Error constructing\n");
    return;
  }
    
  notifier = new QSocketNotifier( sock, QSocketNotifier::Read );
  connect( notifier, SIGNAL( activated(int) ), this, SLOT( slotAccept(int) ) );
}

KRServerSocket::KRServerSocket( unsigned short int _port,int optname,int value,int level ) :
  notifier( 0L ), sock( -1 )
{
  domain = PF_INET;

  if ( !init ( _port,optname,value,level ) )
  {
    // fatal("Error constructing\n");
    return;
  }
    
  notifier = new QSocketNotifier( sock, QSocketNotifier::Read );
  connect( notifier, SIGNAL( activated(int) ), this, SLOT( slotAccept(int) ) );
}

bool KRServerSocket::init( const char *_path )
{
  return init(_path,0);
}

bool KRServerSocket::init( const char *_path,int optname,int value,int level )
{
  if ( domain != PF_UNIX )
    return false;
  
  int l = strlen( _path );
  if ( l > UNIX_PATH_MAX - 1 )
  {      
    warning( "Too long PF_UNIX domain name '%s'\n",_path);
    return false;
  }  
    
  sock = ::socket( PF_UNIX, SOCK_STREAM, 0 );
  if (sock < 0)
  {
    warning( "Could not create socket\n");
    return false;
  }
  // Heni - 05042000
  if (optname>0)
  {
   socklen_t len=sizeof(value);
   if (-1==setsockopt(sock,level,optname,&value,len ))
   {
	   warning("Could not set socket options.\n");
   }
  }
  // end Heni

  unlink(_path );   

  struct sockaddr_un name;
  name.sun_family = AF_UNIX;
  strcpy( name.sun_path, _path );
    
  if ( bind( sock, (struct sockaddr*) &name,sizeof( name ) ) < 0 )
  {
    warning("Could not bind to socket..\n");
    ::close( sock );
    sock = -1;
    return false;
  }
  
  if ( chmod( _path, 0600) < 0 )
  {
    warning("Could not setupt premissions for server socket\n");
    ::close( sock );
    sock = -1;
    return false;
  }
               
  if ( listen( sock, SOMAXCONN ) < 0 )
  {
    warning("Error listening on socket\n");
    ::close( sock );
    sock = -1;
    return false;
  }

  return true;
}

bool KRServerSocket::init( unsigned short int _port )
{
  return init(_port,0);
}
bool KRServerSocket::init( unsigned short int _port,int optname,int value,int level )
{
  if ( 
#ifdef INET6
      ( domain != PF_INET6 ) &&
#endif
      ( domain != PF_INET  ) )
    return false;
 
  sock = ::socket( domain, SOCK_STREAM, 0 );
  if (sock < 0)
  {
    warning( "Could not create socket\n");
    return false;
  }
  // Heni - 05042000
  if (optname>0)
  {
   socklen_t len=sizeof(value);
   if (-1==setsockopt(sock,level,optname,&value,len ))
   {
	   warning("Could not set socket options.\n");
   }
  }
  // end Heni

  if (domain == AF_INET) {

    sockaddr_in name;
    
    name.sin_family = domain;
    name.sin_port = htons( _port );
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( bind( sock, (struct sockaddr*) &name,sizeof( name ) ) < 0 ) {
	  warning("Could not bind to socket.\n");
	  ::close( sock );
	  sock = -1;
	  return false;
    }
  }
#ifdef INET6
  else if (domain == AF_INET6) {
    sockaddr_in6 name;

    name.sin6_family = domain;
    name.sin6_flowinfo = 0;
    name.sin6_port = htons(_port);
    memcpy(&name.sin6_addr, &in6addr_any, sizeof(in6addr_any));

    if ( bind( sock, (struct sockaddr*) &name,sizeof( name ) ) < 0 ) {
	  warning("Could not bind to socket!\n");
	  ::close( sock );
	  sock = -1;
	  return false;
    }
  }
#endif

  if ( listen( sock, SOMAXCONN ) < 0 )
    {
	  warning("Error listening on socket\n");
	  ::close( sock );
	  sock = -1;
	  return false;
    }

  return true;
}

unsigned short int KRServerSocket::port()
{
  if ( domain != PF_INET )
    return false;

  ksockaddr_in name; ksize_t len = sizeof(name);
  getsockname(sock, (struct sockaddr *) &name, &len);
  return ntohs(get_sin_port(name));
}

unsigned long KRServerSocket::ipv4_addr()
{
  if ( domain != PF_INET )
    return 0;
  
  sockaddr_in name; ksize_t len = sizeof(name);
  getsockname(sock, (struct sockaddr *) &name, &len);
  if (name.sin_family == AF_INET) // It's IPv4
    return ntohl(name.sin_addr.s_addr);
#ifdef INET6
  else if (name.sin_family == AF_INET6) // It's IPv6 Ah.
    return 0;
#endif
  else // We dunno what it is
    return 0;
}

void KRServerSocket::slotAccept( int )
{
  if ( domain == PF_INET )
  {      
    ksockaddr_in clientname;
    int new_sock;
    
    ksize_t size = sizeof(clientname);
    
    if ((new_sock = accept (sock, (struct sockaddr *) &clientname, &size)) < 0)
    {
      warning("Error accepting\n");
      return;
    }

    emit accepted( new KSocket( new_sock ) );
  }
  else if ( domain == PF_UNIX )
  {      
    struct sockaddr_un clientname;
    int new_sock;
    
    ksize_t size = sizeof(clientname);
    
    if ((new_sock = accept (sock, (struct sockaddr *) &clientname, &size)) < 0)
    {
      warning("Error accepting\n");
      return;
    }

    emit accepted( new KSocket( new_sock ) );
  }
}

KRServerSocket::~KRServerSocket()
{
  if ( notifier )
	delete notifier; 
  
  close( sock ); 
}

#include "KRSocket.moc"
