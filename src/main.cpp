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
  KCmdLineLastOption
};


// DEbug level for the program
int global_debug;


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
  
  // Start application
  KApplication application(true);
  KGlobal::locale()->insertCatalog("libkdegames");

  // Start session
  if (application.isSessionRestored())
  {
    RESTORE(Kwin4App);
  }
  else
  {
    Kwin4App *kwin4 = new Kwin4App();
    kwin4->show();
  }

  return application.exec();
}

