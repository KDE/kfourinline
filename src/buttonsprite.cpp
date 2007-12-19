/*
   This file is part of the KDE games kwin4 program
   Copyright (c) 2006 Martin Heni <kde@heni-online.de>

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

// Header includes
#include "buttonsprite.h"

// General includes
#include <math.h>

// Qt includes
#include <QFont>
#include<QGraphicsSceneMouseEvent>

// KDE includes
#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kconfiggroup.h>

// Constructor for the score sprite
ButtonSprite::ButtonSprite(bool pushButton, const QString &id, ThemeManager* theme, int no, QGraphicsScene* scene)
           :  Themeable(id, theme), PixmapSprite(no, scene)
{

  mButtonPressed = false;
  mHover         = false;
  mPushButton    = pushButton;

  mSignal = new SpriteNotify(this);

  setAcceptedMouseButtons(Qt::NoButton);
  setAcceptsHoverEvents(false);

  // Create sub sprites
  mText  = new QGraphicsTextItem(this, scene);
  mText->hide();

  // Redraw us
  if (theme) theme->updateTheme(this);
}



// Destructor
ButtonSprite::~ButtonSprite()
{
  // Clean up
  delete mText;
}


// Redraw the theme.
void ButtonSprite::changeTheme()
{
  // The main display is handled by the parent
  PixmapSprite::changeTheme();
  
  // Retrieve our size
  double width  = this->boundingRect().width();
  double height = this->boundingRect().height();

  // Retrieve theme data
  KConfigGroup config = thememanager()->config(id());

  // Text available?
  if (mText->isVisible())
  {
    // Calculate proper font size
    double fontHeight = config.readEntry("fontHeight", 1.0);
    fontHeight *= height;
    double fontWidth = config.readEntry("fontWidth", 1.0);
    fontWidth *= width;
    
    // Retrieve font color
    QColor fontColor;
    fontColor = config.readEntry("fontColor", QColor(Qt::white));

    // Create and set current font
    QFont font;
    font.setPixelSize(int(fontHeight));

    // Set font and color for all text items
    mText->setFont(font);   
    mText->setDefaultTextColor(fontColor);   
    mText->setTextWidth(fontWidth);   

    // Set position of sub sprites
    double textWidth  = mText->boundingRect().width();
    double textHeight = mText->boundingRect().height();
    mText->setPos((width-textWidth)/2.0, (height-textHeight)/2.0);
  }
}


// QGI advance method 
void ButtonSprite::advance(int phase)
{
  // Advance time and animation etc
  PixmapSprite::advance(phase);
}


// Set the button status (on or off)
void ButtonSprite::setStatus(const bool status)
{
  mButtonPressed = status;
  changeFrame();
}


// Get the button status (on or off)
bool ButtonSprite::status() const
{
  return mButtonPressed;
}


// Store and display the button text
void ButtonSprite::setText(const QString &s)
{
  if (s.isNull()) 
  {
    mText->hide();
  }
  else
  {
    //mText->setPlainText(s);
    // Center
    mText->setHtml("<div align=\"center\">"+s+"</div>");
    mText->show();
    thememanager()->updateTheme(this);
  }
}


// Adjust the frame depending on the state
void ButtonSprite::changeFrame()
{
  int frame = 0;
  if (mButtonPressed) frame = 2;
  if (mHover) frame += 1;
  // Hover ignored for pressed pushbutton
  if (mPushButton && mButtonPressed) frame = 2;
  setFrame(frame);
}


// Hover enter event recevied
void ButtonSprite::hoverEnterEvent(QMouseEvent* /*event*/)  
{
  mHover = true;
  changeFrame();
}


// Hover leave event received
void ButtonSprite::hoverLeaveEvent(QMouseEvent* /*event*/)
{
  mHover = false;
  if (mPushButton) mButtonPressed = false;
  changeFrame();
}


// Mouse press event received
void ButtonSprite::mousePressEvent(QMouseEvent* /*event*/)
{
  if (mPushButton) 
  {
    mButtonPressed = !mButtonPressed;
    if (mButtonPressed) mSignal->emitSignal(number()|0x100);
    else mSignal->emitSignal(number());
  }
  changeFrame();
}


// Mouse release event received
void ButtonSprite::mouseReleaseEvent(QMouseEvent* event)
{
  QPointF p = mapFromScene (QPointF(event->pos()) );
  // kDebug() << "ButtonSprite::mouseReleaseEvent contains?"<< contains(p);
  if (!contains(p)) 
  {
    mHover = false;
    if (mPushButton) mButtonPressed = false;
  }
  else
  {
    if (!mPushButton)
    {
      mButtonPressed = !mButtonPressed;
      if (mButtonPressed) mSignal->emitSignal(number()|0x100);
      else mSignal->emitSignal(number());

    }
    else mButtonPressed = false;
  }
  changeFrame();
}
