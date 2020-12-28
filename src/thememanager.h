/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

// KF
#include <KConfig>
// Qt
#include <QHash>
#include <QObject>
#include <QPoint>
#include <QSvgRenderer>


class ThemeManager;

/** Objects which are connected to the theme manger must inherit
  * from this class. Doing so enables the items to be refreshed
  * by the theme manager and allows them to retrieve theme data
  * from a configuration theme file.
  */
class Themeable
{
   public:
     /** Default constructor for the interface
      */
     Themeable();

     /** Constructor for the interface given the theme item unique ID string
       * and a reference to the theme manager. The ID string is used to refer
       * to the group in the configuration file.
       * @param id           The user defined theme id
       * @param thememanager The used theme manager
       */
     Themeable(const QString &id, ThemeManager* thememanager);

     /** Destructor
      */
     virtual ~Themeable();

     /** Retrieve the ID of the object.
      *  @return The ID.
      */
     QString id() {return mId;}

     /** Retrieve the associated theme manager of this object.
      *  @return The theme manager.
      */
     ThemeManager* thememanager() {return mThemeManager;}

     /** Retrieve the current scale (maximum extension) of the theme.
       * @return The current scale.
       */
     double getScale() {return mScale;}

     /** Set the current scale for the object.
      *  @param scale The new scale.
      */
     void setScale(double scale)  {mScale = scale;}

     /** Main theme notification method. This method is called in the
       * Themeable object to indicate a theme change. The object needs
       * to overwrite this and respond with a properly scaled redraw.
       */
     virtual void changeTheme() = 0;

   private:
     // The theme ID
     QString mId;

     // The theme manager
     ThemeManager* mThemeManager;

     // The current scale for the object (maximum extension)
     double mScale;
};



/**
  * The graphics theme manager. The theme manager holds a list of all Themeable
  * objects and notifies them in the event of a theme change so that the objects
  * can then redraw. It also allows access to the theme configuration file and
  * reads pixmaps from an SVG file given a certain size or scale. The pixmaps
  * are cached so that the same pixmap is retrieved from the cache on not rendered
  * again.
  */
class ThemeManager : public QObject
{
  Q_OBJECT
  public:
    /** Constructor for the theme manager.
      * @param themefile   The theme configuration file
      * @param parent      The parent object
      * @param initialSize Initial theme size, can be arbitrary.
      */
  ThemeManager(const QString &themefile, QObject* parent, int initialSize = 1);
    ~ThemeManager();

    /** Load a pixmap from the SVG theme file. Its filename is given in the
      * "general" section of the theme file as "svgfile". The pixmap is scaled
      * to the given size.
      * @param svgid  The ID of the SVG item to be rendered as pixmap
      * @param size   The size of the resulting pixmap
      * @return The new pixmap.
      */
    const QPixmap getPixmap(const QString &svgid, QSize size);

    /** Load a pixmap from the SVG theme file. Its filename is given in the
      * "general" section of the theme file as "svgfile". The pixmap is scaled
      * to the given width. The height is relative to the width as given in the SVG
      * file.
      * @param svgid  The ID of the SVG item to be rendered as pixmap
      * @param width  The width of the resulting pixmap
      * @return The new pixmap.
      */
    const QPixmap getPixmap(const QString &svgid, double width);

     /** Load a pixmap from the SVG theme file. Its filename is given in the
      * "general" section of the theme file as "svgfile". The pixmap is scaled
      * with reference to another SVG item. This allows to generate a set of pixmaps
      * with related sizes.
      * @param svgid     The ID of the SVG item to be rendered as pixmap
      * @param svgref    The ID of the SVG item used as width reference
      * @param refwidth  The width of the resulting pixmap in relation to the reference item
      * @return The new pixmap.
      */
    const QPixmap getPixmap(const QString &svgid, const QString &svgref, double refwidth);

    /** Retrieve the current scale of the theme.
      * @return The scale.
      */
    double getScale();

    /** Retrieve the theme offset.
      * @return The offset.
      */
    QPoint getOffset();

    /** Retrieve the current theme configuration object.
      * @return The configuration object.
      */
    KConfigGroup config(const QString &id);

    /** Register an object with the theme manager.
      * @param ob The object to be registered.
      */
    void registerTheme(Themeable* ob);

    /** Unregister an object with the theme manager.
      * @param ob The object to be unregistered.
      */
    void unregisterTheme(Themeable* ob);

    /** Forces an update to a theme objects. That is its
      * changeTheme() method is called.
      * @param ob The object to be updated.
      */
    void updateTheme(Themeable* ob);

    /** Forces an update to all theme objects. That is their
      * changeTheme() method is called. Before this a (new)
      * theme file is loaded and all cached pixmaps deleted. This
      * is used to really change one theme over to another one.
      * @param themefile The theme file to load
      */
    void updateTheme(const QString &themefile);

    /** Change the scale of the theme and update all registered
      * theme objects.
      * @param scale The new scale (maximum extension)
      * @param offset The new offset of the theme (left upper corner)
      */
    void rescale(int scale, QPoint offset);

    /** Retrieve the theme's apsect ratio. This is stored as
      * 'aspect-ratio' key in the 'general' group of the theme.
      * @return The aspect ratio (x/y).
      */
    double aspectRatio() {return mAspectRatio;}
    
    /** Retrieve the translated theme's color for specified player. Colors are stored
      * as 'colorNamePlayer0' and 'colorNamePlayer1' key in the 'general'
      * group of the theme.
      * @return The color name for player.
      */
    QString colorNamePlayer(int player) {return mColorNamePlayer[player];}

    /** Check whether the theme is properly initialized.
      * @return 0 if everything is alright
      */
    int checkTheme();

     /** Check whether a changeTheme call was due to themefile change
       * or a rescaling.
       */
     bool themefileChanged();


   private:
     // The used SVG rendered
     QSvgRenderer* mRenderer;

     // Storage of all theme objects
     QList<Themeable*> mObjects;

     // The cache of all pixmap objects [id,pixmap]
     QHash<QString,QPixmap> mPixmapCache;

     // The theme configuration file
     KConfig* mConfig;

     // The current theme scale
     int mScale;

     // The current offset
     QPoint mOffset;

     // The aspect ration
     double mAspectRatio;
     
     // Color names for players.
     QString mColorNamePlayer[2];

     // Type of theme change
     bool mThemeFileChanged;
};


#endif
