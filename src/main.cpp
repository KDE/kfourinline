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

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

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
  KAboutData aboutData( "kfourinline", 0, ki18n("KFourInLine"),
                        KWIN4_VERSION,
                        ki18n("KFourInLine: Two player board game"),
                        KAboutData::License_GPL,
                        ki18n("(c) 1995-2007, Martin Heni"),
                        KLocalizedString(), "http://games.kde.org/kfourinline" );
  aboutData.addAuthor(ki18n("Martin Heni"),ki18n("Game design and code"), "kde@heni-online.de");
  aboutData.addAuthor(ki18n("Johann Ollivier Lapeyre"),ki18n("Graphics"), "johann.ollivierlapeyre@gmail.com");
  aboutData.addAuthor(ki18n("Eugene Trounev"),ki18n("Graphics"), "eugene.trounev@gmail.com");
  aboutData.addAuthor(ki18n("Benjamin Meyer"), ki18n("Code Improvements"));
  KCmdLineArgs::init( argc, argv, &aboutData );

  KCmdLineOptions options;
  options.add("d");
  options.add("debug <level>", ki18n("Enter debug level"));
  options.add("skipintro", ki18n("Skip intro animation"));
  options.add("demo", ki18n("Run game in demo (autoplay) mode"));
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  // Check for debug command line option
  if (args->isSet("debug"))
  {
    global_debug=QString(args->getOption("debug")).toInt();
    kDebug(12010) << "Debug level set to" << global_debug;
  }
  // Check for debug command line option
  if (args->isSet("skipintro"))
  {
    global_skip_intro = true;
    kDebug(12010) << "Skip intro cmd line chosen" << global_skip_intro;
  }
  // Check for debug command line option
  if (args->isSet("demo"))
  {
    global_demo_mode = true;
    kDebug(12010) << "Running in demo mode" << global_demo_mode;
  }
 
  args->clear(); 
  // Start application
  KApplication application(true);
  KGlobal::locale()->insertCatalog("libkdegames");

  // Start session
  if (application.isSessionRestored())
  {
    RESTORE(KWin4App);
  }
  else
  {
    KWin4App *kwin4 = new KWin4App();
    kwin4->show();
  }

  return application.exec();
}

