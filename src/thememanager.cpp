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

// General includes
#include <typeinfo>

// Qt includes
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QRectF>

// KDE includes
#include <kdebug.h>
#include <kstandarddirs.h>

// Local includes
#include "thememanager.h"

// Constructor for the view
ThemeManager::ThemeManager(QString themefile, QObject* parent, int initialSize)
    : QObject(parent)
{
  mScale = initialSize;
  updateTheme(themefile);
  kDebug() << "Scale " << mScale<< endl;
}


void ThemeManager::registerTheme(Themable* ob)
{
  mObjects[ob] = 1;
}

void ThemeManager::unregisterTheme(Themable* ob)
{
  mObjects.remove(ob);
}

void ThemeManager::updateTheme(Themable* ob)
{
  ob->changeTheme();
}

void ThemeManager::updateTheme(QString themefile)
{
  // Empty cache
  mPixmapCache.clear();

  // Process dirs
  QString rcfile = KStandardDirs::locate("data", themefile);
  kDebug() << "ThemeManager LOAD with theme "<<rcfile << endl;

  // Read config and SVG file for theme
  mConfig = new KConfig(rcfile, KConfig::NoGlobals);
  QString svgfile = config("general")->readEntry("svgfile");
  svgfile = KStandardDirs::locate("data", svgfile);
  kDebug() << "Reading SVG master file " << svgfile << endl;


  mRenderer = new QSvgRenderer(this);
  bool result = mRenderer->load(svgfile);
  if (!result) 
  {
    kFatal() << "Cannot open file " << svgfile << endl;
  }
  kDebug() << "Renderer " << mRenderer<<" = " << result << endl;

  // Notify all theme objects of a change
  QHashIterator<Themable*, int> it(mObjects);
  while (it.hasNext())
  {
      it.next();
      Themable* ob = it.key();
      ob->changeTheme();
  }
}

void ThemeManager::rescale(int scale)
{
  if (scale==mScale) return;
  mScale = scale;
  kDebug() << "Rescale to " << scale<<endl;

  QHashIterator<Themable*, int> it(mObjects);
  while (it.hasNext())
  {
      it.next();
      Themable* ob = it.key();
      ob->changeTheme();
  }
}

double ThemeManager::getScale()
{
  return (double)mScale;
}  


KConfig* ThemeManager::config(QString id)
{
   mConfig->setGroup(id);
   return mConfig;
}


// Get pixmap when size is given (can distort image)
const QPixmap ThemeManager::getPixmap(QString svgid, QSize size)
{
  if (size.width() < 1 || size.height() < 1) 
    kFatal() << "ThemeManager::getPixmap Cannot create svgid ID " << svgid << " with zero size " << size << endl;
  
  QPixmap pixmap;

  //  Cached pixmap?
  if (mPixmapCache.contains(svgid))
  {
    pixmap = mPixmapCache[svgid]; 
    if (pixmap.size() == size) 
    {
      return pixmap;
    }
  }

  QImage image(size, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  QPainter p(&image);
  mRenderer->render(&p, svgid);
  pixmap = QPixmap::fromImage(image);
  if (pixmap.isNull()) kFatal() << "ThemeManager::getPixmap Cannot load svgid ID " << svgid << endl;

  mPixmapCache[svgid] = pixmap;


  return pixmap;
}

// Get pixmap when only width is given (keeps aspect ratio)
const QPixmap ThemeManager::getPixmap(QString svgid, double width)
{
  QRectF rect   = mRenderer->boundsOnElement(svgid);
  double factor = width/rect.width();
  QSize size    = QSize(int(width),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}

// Get pixmap with original properties and a scale factor
const QPixmap ThemeManager::getPixmap(QString svgid, QString svgref, double refwidth)
{
  QRectF refrect    = mRenderer->boundsOnElement(svgref);
  QRectF rect       = mRenderer->boundsOnElement(svgid);
  double factor     = refwidth/refrect.width();
  QSize size        = QSize(int(rect.width()*factor),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


Themable::Themable()
{
  mScale        = 1.0;
  mThemeManager = 0;
  kDebug() << "CONSTRUCT DEFAULT THEMABLE for "<< typeid(this).name() << endl;
}

Themable::Themable(QString id, ThemeManager* thememanager)
{
  mScale        = 1.0;
  mId           = id;
  mThemeManager = thememanager;
  if (!thememanager) return;
  thememanager->registerTheme(this);
}

Themable::~Themable()
{
  // kDebug() << this << "unregister " << endl;
  if (mThemeManager) mThemeManager->unregisterTheme(this);
}



