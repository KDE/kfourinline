/*
    This file is part of the KDE games library
    SPDX-FileCopyrightText: 2001 Martin Heni (kde at heni-online.de)
    SPDX-FileCopyrightText: 2001 Andreas Beckermann (b_mann@gmx.de)

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef __KGAMEPROPERTYARRAY_H_
#define __KGAMEPROPERTYARRAY_H_

// own
#include "kfourinline_debug.h"
// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamemessage.h>
#include <libkdegamesprivate/kgame/kgameproperty.h>
#include <libkdegamesprivate/kgame/kgamepropertyhandler.h>
// Qt
#include <QDataStream>
#include <QVector>

/**
 * \class KGamePropertyArray kgamepropertyarray.h <KGamePropertyArray>
 */
template<class type>
class KGamePropertyArray : public QVector<type>, public KGamePropertyBase
{
public:
  KGamePropertyArray() :QVector<type>(), KGamePropertyBase()
  {
    //qCDebug(KFOURINLINE_LOG) << "KGamePropertyArray init";
  }

  KGamePropertyArray( int size )
  {
    resize(size);
  }

  KGamePropertyArray( const KGamePropertyArray<type> &a ) : QVector<type>(a)
  {
  }

  bool  resize( int size )
  {
    if (size!= QVector<type>::size())
    {
      bool a=true;
      QByteArray b;
      QDataStream s(&b, QIODevice::WriteOnly);
      KGameMessage::createPropertyCommand(s,KGamePropertyBase::IdCommand,id(),CmdResize);
      s << size ;
      if (policy()==PolicyClean || policy()==PolicyDirty)
      {
        if (mOwner)
        {
          mOwner->sendProperty(s);
        }
      }
      if (policy()==PolicyLocal || policy()==PolicyDirty)
      {
        extractProperty(b);
//        a=QMemArray<type>::resize(size);// FIXME: return value!
      }
      return a;
    }
    else return true;
  }

  void setAt(int i,type data)
  {
    QByteArray b;
    QDataStream s(&b, QIODevice::WriteOnly);
    KGameMessage::createPropertyCommand(s,KGamePropertyBase::IdCommand,id(),CmdAt);
    s << i ;
    s << data;
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      if (mOwner)
      {
        mOwner->sendProperty(s);
      }
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      extractProperty(b);
    }
    //qCDebug(KFOURINLINE_LOG) << "KGamePropertyArray setAt send COMMAND for id="<<id() << "type=" << 1 << "at(" << i<<")="<<data;
  }

  const type& at( int i ) const
  {
    return QVector<type>::at(i);
  }

  const type& operator[]( int i ) const
  {
    return QVector<type>::operator[](i);
  }

  type& operator[]( int i )
  {
    return QVector<type>::operator[](i);
  }

  KGamePropertyArray<type> &operator=(const KGamePropertyArray<type> &a)
  {
    return assign(a);
  }

  bool  truncate( int pos )
  {
    return resize(pos);
  }

  bool  fill( const type &data, int size = -1 )
  {
    bool r=true;
    QByteArray b;
    QDataStream s(&b, QIODevice::WriteOnly);
    KGameMessage::createPropertyCommand(s,KGamePropertyBase::IdCommand,id(),CmdFill);
    s << data;
    s << size ;
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      if (mOwner)
      {
        mOwner->sendProperty(s);
      }
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      extractProperty(b);
//      r=QMemArray<type>::fill(data,size);//FIXME: return value!
    }
    return r;
  }

  KGamePropertyArray<type>& assign( const KGamePropertyArray<type>& a )
  {
// note: send() has been replaced by sendProperty so it might be broken now!
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      sendProperty();
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      QVector<type>::assign(a);
    }
    return *this;
  }
  KGamePropertyArray<type>& assign( const type *a, int n )
  {
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      sendProperty();
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      QVector<type>::assign(a,n);
    }
    return *this;
  }
  KGamePropertyArray<type>& duplicate( const KGamePropertyArray<type>& a )
  {
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      sendProperty();
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      QVector<type>::duplicate(a);
    }
    return *this;
  }
  KGamePropertyArray<type>& duplicate( const type *a, int n )
  {
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      sendProperty();
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      QVector<type>::duplicate(a,n);
    }
    return *this;
  }
  KGamePropertyArray<type>& setRawData( const type *a, int n )
  {
    if (policy()==PolicyClean || policy()==PolicyDirty)
    {
      sendProperty();
    }
    if (policy()==PolicyLocal || policy()==PolicyDirty)
    {
      QVector<type>::setRawData(a,n);
    }
    return *this;
  }

  void load(QDataStream& s) override
  {
    //qCDebug(KFOURINLINE_LOG) << "KGamePropertyArray load" << id();
    type data;
    for (int i=0; i<QVector<type>::size(); i++)
    {
      s >> data;
      QVector<type>::replace(i,data);
    }
    if (isEmittingSignal())
    {
      emitSignal();
    }
  }
  void save(QDataStream &s) override
  {
    //qCDebug(KFOURINLINE_LOG) << "KGamePropertyArray save "<<id();
    for (int i=0; i<QVector<type>::size(); i++)
    {
      s << at(i);
    }
  }

  void command(QDataStream &stream,int msgid, bool isSender) override
  {
    Q_UNUSED(isSender);
    KGamePropertyBase::command(stream, msgid);
    //qCDebug(KFOURINLINE_LOG) << "Array id="<<id()<<" got command ("<<msgid<<") !!!";
    switch(msgid)
    {
      case CmdAt:
      {
        uint i;
        type data;
        stream >> i >> data;
        QVector<type>::replace( i, data );
        //qCDebug(KFOURINLINE_LOG) << "CmdAt:id="<<id()<<" i="<<i<<" data="<<data;
        if (isEmittingSignal())
        {
          emitSignal();
        }
        break;
      }
      case CmdResize:
      {
        uint size;
        stream >> size;
        //qCDebug(KFOURINLINE_LOG) << "CmdResize:id="<<id()<<" oldsize="<<QMemArray<type>::size()<<" newsize="<<size;
        if (( uint )QVector<type>::size() != size)
        {
          QVector<type>::resize(size);
        }
        break;
      }
      case CmdFill:
      {
        int size;
        type data;
        stream >> data >> size;
        //qCDebug(KFOURINLINE_LOG) << "CmdFill:id="<<id()<<"size="<<size;
        QVector<type>::fill(data,size);
        if (isEmittingSignal())
        {
          emitSignal();
        }
        break;
      }
      case CmdSort:
      {
        //qCDebug(KFOURINLINE_LOG) << "CmdSort:id="<<id();
        std::sort( this->begin(), this->end() );
        break;
      }
      default:
        qCCritical(KFOURINLINE_LOG) << "Error in KPropertyArray::command: Unknown command" << msgid;
        break;
    }
  }
protected:
  void extractProperty(const QByteArray& b)
  {
	QByteArray _b(b);
    QDataStream s(&_b, QIODevice::ReadOnly);
    int cmd;
    int propId;
    KGameMessage::extractPropertyHeader(s, propId);
    KGameMessage::extractPropertyCommand(s, propId, cmd);
    command(s, cmd, true);
  }

};

#endif
