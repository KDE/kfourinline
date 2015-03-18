/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 1995-2007 Martin Heni <kde@heni-online.de>

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

#include <KAboutData>


#include "kfourinline_debug.h"
#include <KLocalizedString>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <Kdelibs4ConfigMigrator>
#include "kwin4.h"

#define KWIN4_VERSION "v1.40"


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
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kfourinline"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kfourinlinerc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kfourinlineui.rc"));
    migrate.migrate();

    QApplication app(argc, argv);

  KAboutData aboutData( "kfourinline", i18n("KFourInLine"),
                        KWIN4_VERSION,
                        i18n("KFourInLine: Two player board game"),
                        KAboutLicense::GPL,
                        i18n("(c) 1995-2007, Martin Heni"),
                        "http://games.kde.org/kfourinline" );
  aboutData.addAuthor(i18n("Martin Heni"),i18n("Game design and code"), "kde@heni-online.de");
  aboutData.addAuthor(i18n("Johann Ollivier Lapeyre"),i18n("Graphics"), "johann.ollivierlapeyre@gmail.com");
  aboutData.addAuthor(i18n("Eugene Trounev"),i18n("Graphics"), "eugene.trounev@gmail.com");
  aboutData.addAuthor(i18n("Benjamin Meyer"), i18n("Code Improvements"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
  parser.addOption(QCommandLineOption(QStringList() << QLatin1String("d") << QLatin1String("debug"), i18n("Enter debug level"), QLatin1String("level")));
  parser.addOption(QCommandLineOption(QStringList() << QLatin1String("skipintro"), i18n("Skip intro animation")));
  parser.addOption(QCommandLineOption(QStringList() << QLatin1String("demo"), i18n("Run game in demo (autoplay) mode")));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

  app.setWindowIcon(QIcon::fromTheme(QLatin1String("kfourinline")));
  /* command line handling */

  // Check for debug command line option
  if (parser.isSet("debug"))
  {
    global_debug=QString(parser.value("debug")).toInt();
    qCDebug(KFOURINLINE_LOG) << "Debug level set to" << global_debug;
  }
  // Check for debug command line option
  if (parser.isSet("skipintro"))
  {
    global_skip_intro = true;
    qCDebug(KFOURINLINE_LOG) << "Skip intro cmd line chosen" << global_skip_intro;
  }
  // Check for debug command line option
  if (parser.isSet("demo"))
  {
    global_demo_mode = true;
    qCDebug(KFOURINLINE_LOG) << "Running in demo mode" << global_demo_mode;
  }
 
   
  // Start application
  // Start session
  if (app.isSessionRestored())
  {
    RESTORE(KWin4App);
  }
  else
  {
    KWin4App *kwin4 = new KWin4App();
    kwin4->show();
  }

  return app.exec();
}

