/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 1995-2007 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

/** \mainpage KWin4 API Documentation
  *
  * \section intro_sec Introduction
  *
  * This is the API documentation for the KDE game 'kwin4'.
  *
  * \section design_sec Design
  *
  * The design diagram shows the dependencies for the key classes of
  * the KWin4 program.
  * The coloring of the classes shows roughly their function in the 
  * groups (program, document and engine, display, QGraphics and KGame library).
  *
  * \image html kwin4classes.png "Class diagram for KWin4"
  *
  * 
  */

// own
#include "kwin4.h"
#include "kfourinline_debug.h"
#include "kfourinline_version.h"
// KF
#include <KAboutData>
#include <KCrash>
#include <Kdelibs4ConfigMigrator>
#include <KLocalizedString>
// Qt
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

// Debug level for the program
int global_debug = 0;
// Skip intro
bool global_skip_intro = false;
// Demo (autoplay mode)
bool global_demo_mode  = false;


// Main function
int main(int argc, char *argv[])
{
  global_debug = 0;
    
  // Fixes blurry icons with fractional scaling
  QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  QApplication app(argc, argv);

    Kdelibs4ConfigMigrator migrate(QStringLiteral("kfourinline"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kfourinlinerc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kfourinlineui.rc"));
    migrate.migrate();

  KLocalizedString::setApplicationDomain("kfourinline");
  KAboutData aboutData( QStringLiteral("kfourinline"), i18n("KFourInLine"),
                        QStringLiteral(KFOURINLINE_VERSION_STRING),
                        i18n("KFourInLine: Two player board game"),
                        KAboutLicense::GPL,
                        i18n("(c) 1995-2007, Martin Heni"));
  aboutData.addAuthor(i18n("Martin Heni"),i18n("Game design and code"), QStringLiteral("kde@heni-online.de"));
  aboutData.addAuthor(i18n("Johann Ollivier Lapeyre"),i18n("Graphics"), QStringLiteral("johann.ollivierlapeyre@gmail.com"));
  aboutData.addAuthor(i18n("Eugene Trounev"),i18n("Graphics"), QStringLiteral("eugene.trounev@gmail.com"));
  aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Code Improvements"));
  aboutData.setHomepage(QStringLiteral("https://kde.org/applications/games/org.kde.kfourinline"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
  parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("d") << QStringLiteral("debug"), i18n("Enter debug level"), QStringLiteral("level")));
  parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("skipintro"), i18n("Skip intro animation")));
  parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("demo"), i18n("Run game in demo (autoplay) mode")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

  app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kfourinline")));
  /* command line handling */

  // Check for debug command line option
  if (parser.isSet(QStringLiteral("debug")))
  {
    global_debug=QString(parser.value(QStringLiteral("debug"))).toInt();
    qCDebug(KFOURINLINE_LOG) << "Debug level set to" << global_debug;
  }
  // Check for debug command line option
  if (parser.isSet(QStringLiteral("skipintro")))
  {
    global_skip_intro = true;
    qCDebug(KFOURINLINE_LOG) << "Skip intro cmd line chosen" << global_skip_intro;
  }
  // Check for debug command line option
  if (parser.isSet(QStringLiteral("demo")))
  {
    global_demo_mode = true;
    qCDebug(KFOURINLINE_LOG) << "Running in demo mode" << global_demo_mode;
  }
 
   
  // Start application
  // Start session
  if (app.isSessionRestored())
  {
    kRestoreMainWindows<KWin4App>();
  }
  else
  {
    KWin4App *kwin4 = new KWin4App();
    kwin4->show();
  }

  return app.exec();
}

