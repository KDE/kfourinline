#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H
/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Qt includes
#include <QObject>
#include <QSvgRenderer>
#include <QHash>
#include <kconfig.h>

class ThemeManager;

class Themable 
{
   public:
     Themable(QString id, ThemeManager* thememanager);
     virtual ~Themable();
     QString id() {return mId;}
     ThemeManager* thememanager() {return mThemeManager;}
     double getScale() {return mScale;}
     void setScale(double scale)  {mScale = scale;}

     virtual void changeTheme() =0;

   private:
     QString mId;
     ThemeManager* mThemeManager;
     double mScale;
};



/**
 * The graphics theme manager
 */
class ThemeManager : public QObject
{

  public:
    /** Constructor for the object.
     */
    ThemeManager(QString themefile, int scale, QObject* parent);

    const QPixmap getPixmap(QString svgid, QSize size);
    const QPixmap getPixmap(QString svgid, double width);
    const QPixmap getPixmap(QString svgid, QString svgref, double refwidth);
    double getScale();
    KConfig* config(QString id);

    void registerTheme(Themable* ob);
    void unregisterTheme(Themable* ob);
    void updateTheme(Themable* ob);
    void rescale(int scale);

   private:
     QSvgRenderer* mRenderer;
     QHash<Themable*,int> mObjects;
     KConfig* mConfig;
     int mScale;
};


#endif
