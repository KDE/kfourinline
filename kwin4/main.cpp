/***************************************************************************
                          FILENAME|  -  description
                             -------------------
    begin                : Sun Mar 26 12:50:12 CEST 2000
    copyright            : (C) |1995-2000 by Martin Heni
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
#include <stdio.h>
#include <qstring.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>
#include <kaboutdata.h>
#include <stdlib.h>

#include "kwin4.h"


static KCmdLineOptions options[] =
{
  { "d", 0, 0},
  { "debug <level>", I18N_NOOP("Enter debug level"), 0 },
  { 0, 0, 0 }
};
int global_debug;

int main(int argc, char *argv[])
{
  global_debug=0;
  KAboutData *aboutData=new KAboutData( "kwin4", I18N_NOOP("KWin4"),
                        KWIN4_VERSION,
                        I18N_NOOP("KWin4: Two player network game"),
                        KAboutData::License_GPL,
                        "(c) 1995-2000, Martin Heni");
  aboutData->addAuthor("Martin Heni",0, "martin@heni-online.de");
  KCmdLineArgs::init( argc, argv, aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  /* command line handling */
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->isSet("debug"))
  {
    global_debug=atoi(QString(args->getOption("debug")).latin1() );
    kdDebug() << "Debug level set to " << global_debug << endl;
  }
  args->clear();

   //  KApplication app(argc, argv, "kwin4");
   KApplication app;
   KGlobal::locale()->insertCatalogue("libkdegames");
    
   /*
   QStringList list=app.dirs()->allTypes();
   QStringList list=app.dirs()->findDirs("data","kwin4");
          // print it out
        for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
            printf( "%s \n", (*it).latin1() );
        }
   */ 


  if (app.isRestored())
  {
    RESTORE(Kwin4App);
  }
  else 
  {
    Kwin4App *kwin4 = new Kwin4App();
    kwin4->show();
  }

  int result=app.exec();
  delete aboutData;
  return result;
}  
