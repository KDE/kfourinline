/***************************************************************************
                          KRSocket.h  -  description
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
#ifndef KRSOCK_H
#define KRSOCK_H

#include <qobject.h>
#include <sys/types.h>
// we define STRICT_ANSI to get rid of some warnings in glibc
#ifndef __STRICT_ANSI__
#define __STRICT_ANSI__
#define _WE_DEFINED_IT_
#endif
#include <sys/socket.h>
#ifdef _WE_DEFINED_IT_
#undef __STRICT_ANSI__
#undef _WE_DEFINED_IT_
#endif

#include <sys/un.h>

#include <netinet/in.h>
class QSocketNotifier;

#ifdef INET6
typedef sockaddr_in6 ksockaddr_in;
#define KSOCK_DEFAULT_DOMAIN PF_INET6
#else
typedef sockaddr_in ksockaddr_in;
#define KSOCK_DEFAULT_DOMAIN PF_INET
#endif

#include <ksock.h>

class KRServerSocketPrivate;


/**
 * Monitor a port for incoming TCP/IP connections.
 *
 * You can use a KRServerSocket to listen on a port for incoming
 * connections. When a connection arrived in the port, a KSocket
 * is created and the signal accepted is raised. Make sure you
 * always connect to this signal. If you dont the ServerSocket will
 * create new KSocket's and no one will delete them!
 *
 * If socket() is -1 or less the socket was not created properly.
 *
 * @author Torben Weis <weis@stud.uni-frankfurt.de>
 * @version $Id$
 * @short Monitor a port for incoming TCP/IP connections.
*/
class KRServerSocket : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param _port	the port number to monitor for incoming connections.
     */
    KRServerSocket( unsigned short int _port );
    KRServerSocket( unsigned short int _port, int optname, int value=1, int level=SOL_SOCKET);

    /**
     * Creates a UNIX domain server socket.
     */
    KRServerSocket( const char *_path );
    KRServerSocket( const char *_path, int optname, int value=1, int level=SOL_SOCKET);
  
    /** 
     * Destructor. Closes the socket if it was not already closed.
     */
    ~KRServerSocket();
    
    /** 
     * Get the file descriptor assoziated with the socket.
     */
    int socket() const { return sock; }

    /** 
     * Returns the port number which is being monitored.
     */
    unsigned short int port();

    /** 
     * The address.
     */
    unsigned long ipv4_addr();

public slots: 
    /** 
     * Called when someone connected to our port.
     */
    virtual void slotAccept( int );

signals:
    /**
     * A connection has been accepted.
     * It is your task to delete the KSocket if it is no longer needed.
     */
    void accepted( KSocket* );

protected:
    bool init( short unsigned int );
    bool init( const char *_path );
    bool init( const char *_path, int optname, int value=1, int level=SOL_SOCKET);
    bool init( short unsigned int, int optname, int value=1, int level=SOL_SOCKET);
  
    /** 
     * Notifies us when there is something to read on the port.
     */
    QSocketNotifier *notifier;
    
    /** 
     * The file descriptor for this socket. sock may be -1.
     * This indicates that it is not connected.
     */    
    int sock;  

    int domain;

private:
    KRServerSocket(const KRServerSocket&);
    KRServerSocket& operator=(const KRServerSocket&);

    KRServerSocketPrivate *d;
};


// Here are a whole bunch of hackish macros that allow one to
// get at the correct member of ksockaddr_in

#ifdef INET6
#define get_sin_addr(x) x.sin6_addr
#define get_sin_port(x) x.sin6_port
#define get_sin_family(x) x.sin6_family
#define get_sin_paddr(x) x->sin6_addr
#define get_sin_pport(x) x->sin6_port
#define get_sin_pfamily(x) x->sin6_family
#else
#define get_sin_addr(x) x.sin_addr
#define get_sin_port(x) x.sin_port
#define get_sin_family(x) x.sin_family
#define get_sin_paddr(x) x->sin_addr
#define get_sin_pport(x) x->sin_port
#define get_sin_pfamily(x) x->sin_family
#endif

#endif
