/***************************************************************************
                          Kwin4  -  Four in a Row for KDE
                             -------------------
    begin                : March 2000 
    copyright            : (C) 1995-2000 by Martin Heni
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

#ifndef KWIN4VIEW_H
#define KWIN4VIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

// include files for Qt
#include <qwidget.h>
#include <qpixmap.h>
#include <qpainter.h>

class Kwin4Doc;

/** The Kwin4View class provides the view widget for the Kwin4App instance.	
 * The View instance inherits QWidget as a base class and represents the view object of a KTMainWindow. As Kwin4View is part of the
 * docuement-view model, it needs a reference to the document object connected with it by the Kwin4App class to manipulate and display
 * the document structure provided by the Kwin4Doc class.
 * 	
 * @author Source Framework Automatically Generated by KDevelop, (c) The KDevelop Team.
 * @version KDevelop version 0.4 code generation
 */
class Kwin4View : public QWidget
{
  Q_OBJECT
  public:
    /** Constructor for the main view */
    Kwin4View(QWidget *parent = 0, const char *name=0);
    /** Destructor for the main view */
    ~Kwin4View();

    /** returns a pointer to the document connected to the view instance. Mind that this method requires a Kwin4App instance as a parent
     * widget to get to the window document pointer by calling the Kwin4App::getDocument() method.
     *
     * @see Kwin4App#getDocument
     */
    Kwin4Doc *getDocument() const;

    /** contains the implementation for printing functionality */
    void print(QPrinter *pPrinter);
  /** Updates on of the column arrows */
  void updateArrow(int x);
  /** Updates the status area */
  void updateStatus();
  /** Updates the table area */
  void updateTable();
  /** QT Paint Event */
  /** Update XY area */
  void updateXY(int x,int y);

  void mousePressEvent ( QMouseEvent *m );

protected: // Protected methods
  /** Draw a Borderframe */
  void drawBorder(QPainter *p,QRect rect,int offset,int width,int mode);
protected: // Protected methods
  /** Draw the table */
  void drawTable(QPainter *p);
protected: // Protected methods
  /** Draw the status window */
  void drawStatus(QPainter *p);
protected: // Protected methods
  /** Draw the game board */
  void drawField(QPainter *p);
protected: // Protected methods
  /** Draw the game intro */
  void drawIntro(QPainter *p);
  void paintEvent( QPaintEvent * );
	
  private:
	
protected: // Protected methods
  /** Interface to the Paint Event */
  void Paint(QPainter *p);
};

#endif // KWIN4VIEW_H
