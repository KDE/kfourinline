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

// Header includes
#include "thememanager.h"

// General includes
// #include <typeinfo>

// Qt includes
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QRectF>

// KDE includes
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kconfiggroup.h>

// Local includes
#include "kwin4global.h"

// Constructor for the theme manager
ThemeManager::ThemeManager(const QString &themefile, QObject* parent, int initialSize)
    : QObject(parent), mRenderer(0), mConfig( 0 )
{
  mScale            = initialSize;
  mAspectRatio      = 1.0;
  mThemeFileChanged = false;
  updateTheme(themefile);
}

ThemeManager::~ThemeManager()
{
    delete mConfig;
    delete mRenderer;
}

// Register an object with the manager
void ThemeManager::registerTheme(Themeable* ob)
{
  //We want to make sure that we draw the items registered last, first.
  mObjects.prepend(ob);
}


// Unregister an object from the manager
void ThemeManager::unregisterTheme(Themeable* ob)
{
  mObjects.removeAll(ob);
}


// Check whether the theme is alright
int ThemeManager::checkTheme()
{
  // Check theme
  if (mRenderer == 0) return 1;
  return 0; // Ok
}


// Check the reason of the theme change (rescale or new theme)
bool ThemeManager::themefileChanged()
{
  return mThemeFileChanged;
}


// Force an refresh of the theme object given
void ThemeManager::updateTheme(Themeable* ob)
{
  ob->changeTheme();
}


// Update the theme file and refresh all registered objects. Used
// to really change the theme.
void ThemeManager::updateTheme(const QString &themefile)
{
  mThemeFileChanged = true;

  // Empty cache
  mPixmapCache.clear();

  // Process dirs
  QString rcfile = KStandardDirs::locate("kwin4theme", themefile);
  kDebug() << "ThemeManager LOAD with theme "<<rcfile;

  // Read config and SVG file for theme
  delete mConfig;
  mConfig = new KConfig(rcfile, KConfig::NoGlobals);
  QString svgfile = config("general").readEntry("svgfile");
  svgfile = KStandardDirs::locate("kwin4theme", svgfile);
  kDebug() << "Reading SVG master file  =" << svgfile;
  mAspectRatio     =  config("general").readEntry("aspect-ratio", 1.0);
  kDebug() << "Aspect ration =" << mAspectRatio;


  delete mRenderer;
  mRenderer = new KSvgRenderer(this);
  bool result = mRenderer->load(svgfile);
  if (!result)
  {
    delete mRenderer;
    mRenderer = 0;
    kFatal() << "Cannot open file" << svgfile;
  }
  kDebug() << "Renderer" << mRenderer<<" =" << result;

  // Notify all theme objects of a change
  foreach(Themeable *object, mObjects) {
      object->changeTheme();
  }
}


// Rescale the theme. Call all registered objects so that they can refresh.
void ThemeManager::rescale(int scale, QPoint offset)
{
  if (global_debug > 0)
     kDebug() << "THEMEMANAGER::Rescaling theme to " << scale<<" offset to " << offset;

  mThemeFileChanged = false;

  if (global_debug > 1)
  {
    if (scale==mScale)
      kDebug() <<" No scale change to" << scale << ". If this happends too often it is BAD";
  }
  //if (scale==mScale) return;
  mScale = scale;
  mOffset = offset;

  foreach(Themeable *object, mObjects) {
      object->changeTheme();
  }
}


// Retrieve the theme's scale
double ThemeManager::getScale()
{
  return (double)mScale;
}


// Retrieve the theme offset
QPoint ThemeManager::getOffset()
{
  return mOffset;
}


// Retrieve the current theme configuration file.
KConfigGroup ThemeManager::config(const QString &id)
{
   KConfigGroup grp = mConfig->group(id);
   return grp;
}


// Get a pixmap when its size is given (this can distort the image)
const QPixmap ThemeManager::getPixmap(const QString &svgid,const QSize &size)
{
  if (size.width() < 1 || size.height() < 1)
    kFatal() << "ThemeManager::getPixmap Cannot create svgid ID " << svgid << " with zero size" << size;

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
  p.end();
  pixmap = QPixmap::fromImage(image);
  if (pixmap.isNull())
    kFatal() << "ThemeManager::getPixmap Cannot load svgid ID " << svgid;

  // Cache image
  mPixmapCache[svgid] = pixmap;

  return pixmap;
}


// Get a pixmap when only width is given (this keeps the aspect ratio)
const QPixmap ThemeManager::getPixmap(const QString &svgid, double width)
{
  QRectF rect   = mRenderer->boundsOnElement(svgid);
  double factor = width/rect.width();
  QSize size    = QSize(int(width),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// Get a pixmap with original properties and a scale factor given with respect to
// another SVG item.
const QPixmap ThemeManager::getPixmap(const QString &svgid, const QString &svgref, double refwidth)
{
  QRectF refrect    = mRenderer->boundsOnElement(svgref);
  QRectF rect       = mRenderer->boundsOnElement(svgid);
  double factor     = refwidth/refrect.width();
  QSize size        = QSize(int(rect.width()*factor),  int(rect.height()*factor));
  return getPixmap(svgid, size);
}


// ========================== Themeable interface ===============================

// Constructs a themeable interface
Themeable::Themeable()
{
  mScale        = 1.0;
  mThemeManager = 0;
}


// Constructs a themeable interface given its id and the master theme manager.
// This automatically registeres the object with the manager.
Themeable::Themeable(const QString &id, ThemeManager* thememanager)
{
  mScale        = 1.0;
  mId           = id;
  mThemeManager = thememanager;
  if (!thememanager) return;
  thememanager->registerTheme(this);
}


// Destructs the themeable object
Themeable::~Themeable()
{
  if (mThemeManager) mThemeManager->unregisterTheme(this);
}

#include "thememanager.moc"

