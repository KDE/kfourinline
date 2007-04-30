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
// #include <typeinfo>

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

// Constructor for the theme manager
ThemeManager::ThemeManager(QString themefile, QObject* parent, int initialSize)
    : QObject(parent)
{
  mScale = initialSize;
  updateTheme(themefile);
}


// Register an object with the manager
void ThemeManager::registerTheme(Themable* ob)
{
  mObjects[ob] = 1;
}


// Unregister an object from the manager
void ThemeManager::unregisterTheme(Themable* ob)
{
  mObjects.remove(ob);
}


// Force an refresh of the theme object given
void ThemeManager::updateTheme(Themable* ob)
{
  ob->changeTheme();
}


// Update the theme file and refresh all registered objects. Used 
// to really change the theme.
void ThemeManager::updateTheme(QString themefile)
{
  // Empty cache
  mPixmapCache.clear();

  // Process dirs
  QString rcfile = KStandardDirs::locate("data", themefile);
  kDebug() << "ThemeManager LOAD with theme "<<rcfile << endl;

  // Read config and SVG file for theme
  mConfig = new KConfig(rcfile, KConfig::NoGlobals);
  QString svgfile = config("general").readEntry("svgfile");
  svgfile = KStandardDirs::locate("data", svgfile);
  kDebug() << "Reading SVG master file  = " << svgfile << endl;


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


// Rescale the theme. Call all registed objects so that they can refresh.
void ThemeManager::rescale(int scale)
{
  if (scale==mScale) return;
  mScale = scale;

  QHashIterator<Themable*, int> it(mObjects);
  while (it.hasNext())
  {
      it.next();
      Themable* ob = it.key();
      ob->changeTheme();
  }
}


// Retreive the theme's scale
double ThemeManager::getScale()
{
  return (double)mScale;
}  


// Retreive the current theme configuration file.
KConfigGroup ThemeManager::config(QString id)
{
   KConfigGroup grp = mConfig->group(id); 
   return grp;
}


// Get a pixmap when its size is given (this can distort the image)
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

  // Create new image
  QImage image(size, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  QPainter p(&image);
  mRenderer->render(&p, svgid);
  pixmap = QPixmap::fromImage(image);
  if (pixmap.isNull())
    kFatal() << "ThemeManager::getPixmap Cannot load svgid ID " << svgid << endl;

  // Cache image
  mPixmapCache[svgid] = pixmap;

  return pixmap;
}


// Get a pixmap when only width is given (this keeps the aspect ratio)
const QPixmap ThemeManager::getPixmap(QString svgid, double width)
{
  QRectF rect   = mRenderer->boundsOnElement(svgid);
  double factor = width/rect.width();
  QSize size    = QSize(int(width),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// Get a pixmap with original properties and a scale factor given with respect to
// another SVG item.
const QPixmap ThemeManager::getPixmap(QString svgid, QString svgref, double refwidth)
{
  QRectF refrect    = mRenderer->boundsOnElement(svgref);
  QRectF rect       = mRenderer->boundsOnElement(svgid);
  double factor     = refwidth/refrect.width();
  QSize size        = QSize(int(rect.width()*factor),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// ========================== Themable interface ===============================

// Constructs a themable interface
Themable::Themable()
{
  mScale        = 1.0;
  mThemeManager = 0;
}


// Constructs a themeable interface given its id and the master theme manager. 
// This automatically registeres the object with the manager.
Themable::Themable(QString id, ThemeManager* thememanager)
{
  mScale        = 1.0;
  mId           = id;
  mThemeManager = thememanager;
  if (!thememanager) return;
  thememanager->registerTheme(this);
}


// Destructs the themeable object
Themable::~Themable()
{
  if (mThemeManager) mThemeManager->unregisterTheme(this);
}



