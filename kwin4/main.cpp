/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2001 by Martin Heni
    email                : martin@heni-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kaboutdata.h>

#include "kwin4.h"

static KCmdLineOptions options[] =
{
  { "d", 0, 0},
  { "debug <level>", I18N_NOOP("Enter debug level"), 0 },
  KCmdLineLastOption
};

int global_debug;

int main(int argc, char *argv[])
{
  global_debug=0;
  KAboutData aboutData( "kwin4", I18N_NOOP("KWin4"),
                        KWIN4_VERSION,
                        I18N_NOOP("KWin4: Two player network game"),
                        KAboutData::License_GPL,
                        "(c) 1995-2000, Martin Heni");
  aboutData.addAuthor("Martin Heni",0, "martin@heni-online.de");
  aboutData.addCredit("Laura", I18N_NOOP("Beta testing"), 0);
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->isSet("debug"))
  {
    global_debug=QString(args->getOption("debug")).toInt();
    kdDebug(12010) << "Debug level set to " << global_debug << endl;
  }
  
  KApplication app(argc, argv);
  KGlobal::locale()->insertCatalogue("libkdegames");

  if (app.isRestored())
  {
    RESTORE(Kwin4App);
  }
  else
  {
    Kwin4App *kwin4 = new Kwin4App();
    app.setMainWidget(kwin4);
    kwin4->show();
  }

  return app.exec();
}

