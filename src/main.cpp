/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2007 by Martin Heni
    email                : kde@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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

static KCmdLineOptions options[] =
{
  { "d", 0, 0},
  { "debug <level>", I18N_NOOP("Enter debug level"), 0 },
  { "skipintro", I18N_NOOP("Skip intro animation"), 0 },
  { "demo", I18N_NOOP("Run game in demo (autoplay) mode"), 0 },
  KCmdLineLastOption
};


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
  KAboutData aboutData( "kwin4", I18N_NOOP("KWin4"),
                        KWIN4_VERSION,
                        I18N_NOOP("KWin4: Two player board game"),
                        KAboutData::License_GPL,
                        "(c) 1995-2007, Martin Heni");
  aboutData.addAuthor("Martin Heni",I18N_NOOP("Game design and code"), "kde@heni-online.de");
  aboutData.addAuthor("Johann Ollivier Lapeyre",I18N_NOOP("Graphics"), "johann.ollivierlapeyre@gmail.com");
  aboutData.addAuthor("Benjamin Meyer", I18N_NOOP("Code Improvements"), 0);
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  // Check for debug command line option
  if (args->isSet("debug"))
  {
    global_debug=QString(args->getOption("debug")).toInt();
    kDebug(12010) << "Debug level set to " << global_debug << endl;
  }
  // Check for debug command line option
  if (args->isSet("skipintro"))
  {
    global_skip_intro = true;
    kDebug(12010) << "Skip intro cmd line choosen " << global_skip_intro << endl;
  }
  // Check for debug command line option
  if (args->isSet("demo"))
  {
    global_demo_mode = true;
    kDebug(12010) << "Running in demo mode " << global_demo_mode << endl;
  }
  
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

