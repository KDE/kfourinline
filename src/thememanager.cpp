/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "thememanager.h"

// own
#include "kfourinline_debug.h"
#include "kwin4global.h"
// KF
#include <KConfigGroup>
#include <KLocalizedString>
// Qt
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QRectF>
#include <QStandardPaths>

// Constructor for the theme manager
ThemeManager::ThemeManager(const QString &themefile, QObject *parent, int initialSize)
    : QObject(parent)
    , mRenderer()
    , mConfig()
{
    mScale = initialSize;
    mAspectRatio = 1.0;
    mThemeFileChanged = false;
    updateTheme(themefile);
}

ThemeManager::~ThemeManager()
{
    delete mConfig;
    delete mRenderer;
}

// Register an object with the manager
void ThemeManager::registerTheme(Themeable *ob)
{
    // We want to make sure that we draw the items registered last, first.
    mObjects.prepend(ob);
}

// Unregister an object from the manager
void ThemeManager::unregisterTheme(Themeable *ob)
{
    mObjects.removeAll(ob);
}

// Check whether the theme is alright
int ThemeManager::checkTheme()
{
    // Check theme
    if (mRenderer == nullptr)
        return 1;
    return 0; // Ok
}

// Check the reason of the theme change (rescale or new theme)
bool ThemeManager::themefileChanged()
{
    return mThemeFileChanged;
}

// Force an refresh of the theme object given
void ThemeManager::updateTheme(Themeable *ob)
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
    QString rcfile = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("themes/") + themefile);
    qCDebug(KFOURINLINE_LOG) << "ThemeManager LOAD with theme " << rcfile;

    // Read config and SVG file for theme
    delete mConfig;
    mConfig = new KConfig(rcfile, KConfig::NoGlobals);
    QString svgfile = config(QStringLiteral("general")).readEntry("svgfile");
    svgfile = QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("themes/") + svgfile);
    qCDebug(KFOURINLINE_LOG) << "Reading SVG master file  =" << svgfile;
    mAspectRatio = config(QStringLiteral("general")).readEntry("aspect-ratio", 1.0);
    qCDebug(KFOURINLINE_LOG) << "Aspect ratio =" << mAspectRatio;
    mColorNamePlayer[0] = i18nc("Player 0 color", config(QStringLiteral("general")).readEntry("colorNamePlayer0").toUtf8().constData());
    qCDebug(KFOURINLINE_LOG) << "Player 0 color name =" << mColorNamePlayer[0];
    mColorNamePlayer[1] = i18nc("Player 1 color", config(QStringLiteral("general")).readEntry("colorNamePlayer1").toUtf8().constData());
    qCDebug(KFOURINLINE_LOG) << "Player 1 color name =" << mColorNamePlayer[1];

    delete mRenderer;
    mRenderer = new QSvgRenderer(this);
    bool result = mRenderer->load(svgfile);
    if (!result) {
        delete mRenderer;
        mRenderer = nullptr;
        qCCritical(KFOURINLINE_LOG) << "Cannot open file" << svgfile;
    }
    qCDebug(KFOURINLINE_LOG) << "Renderer" << mRenderer << " =" << result;

    // Notify all theme objects of a change
    for (Themeable *object : std::as_const(mObjects)) {
        object->changeTheme();
    }
}

// Rescale the theme. Call all registered objects so that they can refresh.
void ThemeManager::rescale(int scale, QPoint offset)
{
    if (global_debug > 0)
        qCDebug(KFOURINLINE_LOG) << "THEMEMANAGER::Rescaling theme to " << scale << " offset to " << offset;

    mThemeFileChanged = false;

    if (global_debug > 1) {
        if (scale == mScale)
            qCDebug(KFOURINLINE_LOG) << " No scale change to" << scale << ". If this happens too often it is BAD";
    }
    // if (scale==mScale) return;
    mScale = scale;
    mOffset = offset;

    for (Themeable *object : std::as_const(mObjects)) {
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
const QPixmap ThemeManager::getPixmap(const QString &svgid, QSize size)
{
    if (size.width() < 1 || size.height() < 1)
        qCCritical(KFOURINLINE_LOG) << "ThemeManager::getPixmap Cannot create svgid ID " << svgid << " with zero size" << size;

    const qreal dpr = qApp->devicePixelRatio();
    size *= dpr;

    //  Cached pixmap?
    if (mPixmapCache.contains(svgid)) {
        const QPixmap pixmap = mPixmapCache[svgid];
        if (pixmap.size() == size) {
            return pixmap;
        }
    }

    // Create new image
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    mRenderer->render(&p, svgid);
    p.end();
    pixmap.setDevicePixelRatio(dpr);
    if (pixmap.isNull())
        qCCritical(KFOURINLINE_LOG) << "ThemeManager::getPixmap Cannot load svgid ID " << svgid;

    // Cache image
    mPixmapCache[svgid] = pixmap;

    return pixmap;
}

// Get the size when only width is given (this keeps the aspect ratio)
QSize ThemeManager::pixmapSize(const QString &svgid, double width) const
{
    QRectF rect = mRenderer->boundsOnElement(svgid);
    double factor = width / rect.width();
    return QSize(int(width), int(rect.height() * factor));
}

// Get the size with original properties and a scale factor given with respect to
// another SVG item.
QSize ThemeManager::pixmapSize(const QString &svgid, const QString &svgref, double refwidth)
{
    QRectF refrect = mRenderer->boundsOnElement(svgref);
    QRectF rect = mRenderer->boundsOnElement(svgid);
    double factor = refwidth / refrect.width();
    return QSize(int(rect.width() * factor), int(rect.height() * factor));
}

// ========================== Themeable interface ===============================

// Constructs a themeable interface
Themeable::Themeable()
{
    mScale = 1.0;
    mThemeManager = nullptr;
}

// Constructs a themeable interface given its id and the master theme manager.
// This automatically registers the object with the manager.
Themeable::Themeable(const QString &id, ThemeManager *thememanager)
{
    mScale = 1.0;
    mId = id;
    mThemeManager = thememanager;
    if (!thememanager)
        return;
    thememanager->registerTheme(this);
}

// Destructs the themeable object
Themeable::~Themeable()
{
    if (mThemeManager)
        mThemeManager->unregisterTheme(this);
}

#include "moc_thememanager.cpp"
