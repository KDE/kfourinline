/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2006 Martin Heni <kde@heni-online.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "introsprite.h"

// own
#include "kfourinline_debug.h"
// Std
#include <cmath>


#define sign(x) ( (x)>0?1:((x)<0?-1:0) )

/**
 * Store an animation script command. The command represents certain possible
 * animation sub mentods. The possible methods are listed in the enum Cmd
 */
class AnimationCommand
{
  public:
    /**
     * The supported commands.
     */
    enum Cmd {SHOW, HIDE, PAUSE, POSITION, LINEAR_DURATION, CIRCLE_DURATION, MANHATTEN};
    /**
     * Construct an animation command
     */
    AnimationCommand() 
    {
      cmd       = PAUSE;
      duration  = 0;
      radius    = 0.0;
      velocity  = 0.0;
    }
    /** The command type.
     */
    Cmd cmd;
    /** How long does the command take [ms].
     */
    int duration;
    /** The start position [0..1, 0..1].
     */
    QPointF start;
    /** The end position [0..1, 0..1].
     */
    QPointF end;
    /** The radius  [0..1] (if necessary).
     */
    double radius;
    /** The velocity [0...1000] (if necessary).
      */
    double velocity;
};


// Constructor for the sprite
IntroSprite::IntroSprite(const QString &id, ThemeManager* theme,  int no, QGraphicsScene* scene)
    :  Themeable(id, theme), PixmapSprite(no, scene)
{
  hide();
  mDeltaT = 0;
  mDelayT = 0;
  mNo     = no;
  mTime.restart();
  mDebugTime.restart();
  mStartAnimation = true;
  if (theme) theme->updateTheme(this);
}


// Destructor
IntroSprite::~IntroSprite()
{
  // Clear animation list (deletes objects)
  clearAnimation();
}


// Change the theme
void IntroSprite::changeTheme()
{
  PixmapSprite::changeTheme();
}


// Clear the animation script
void IntroSprite::clearAnimation(bool restartTime)
{
  qDeleteAll(mAnimList);
  mAnimList.clear();

  // Reset time?
  if (restartTime)
  {
    mTime.restart();
    mDeltaT = 0;
    mDelayT = 0;
  }
  mStartAnimation = true;
}


// Add a show sprite command
AnimationCommand* IntroSprite::addShow()
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::SHOW;
  cmd->duration = 0;
  cmd->start=previousStart();
  cmd->end=previousEnd();
  mAnimList.append(cmd);
  return cmd;
}


// Add a hide sprite command
AnimationCommand* IntroSprite::addHide()
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::HIDE;
  cmd->duration = 0;
  cmd->start=previousStart();
  cmd->end=previousEnd();
  mAnimList.append(cmd);
  return cmd;
}


// Add a relative pause
AnimationCommand* IntroSprite::addPause(int duration)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::PAUSE;
  cmd->duration = duration;
  cmd->start=previousStart();
  cmd->end=previousEnd();
  mAnimList.append(cmd);
  return cmd;
}


// Add an absolute pause...wait until the given time
AnimationCommand* IntroSprite::addWait(int duration)
{
  int currentDuration = animationDuration();
  if (currentDuration < duration)
  {
    AnimationCommand* cmd = new AnimationCommand();
    cmd->cmd = AnimationCommand::PAUSE;
    cmd->duration = duration-currentDuration;
    cmd->start=previousStart();
    cmd->end=previousEnd();
    mAnimList.append(cmd);
    return cmd;
  }
  return nullptr;
}


// Add a set position command
AnimationCommand* IntroSprite::addPosition(QPointF start)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::POSITION;
  cmd->duration = 0;
  cmd->start=start;
  cmd->end=start;
  mAnimList.append(cmd);
  return cmd;
}


// Add a linear movement
AnimationCommand* IntroSprite::addLinear(QPointF start, QPointF end, int duration)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::LINEAR_DURATION;
  cmd->duration = duration;
  cmd->start=start;
  cmd->end=end;
  mAnimList.append(cmd);
  return cmd;
}


// Add a relative linear movement, that is, starting from the previous position
AnimationCommand* IntroSprite::addRelativeLinear(QPointF end, int duration)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::LINEAR_DURATION;
  cmd->duration = duration;
  cmd->start=previousEnd();
  cmd->end=end;
  mAnimList.append(cmd);
  return cmd;
}


// Add a circle movement
AnimationCommand* IntroSprite::addCircle(QPointF start, double radius, int duration)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::CIRCLE_DURATION;
  cmd->duration = duration;
  cmd->start=start;
  cmd->end=start;
  cmd->radius=radius;
  mAnimList.append(cmd);
  return cmd;
}


// Add a relative manhatten move
AnimationCommand* IntroSprite::addRelativeManhatten(QPointF end, double velocity)
{
  AnimationCommand* cmd = new AnimationCommand();
  cmd->cmd = AnimationCommand::MANHATTEN;
  cmd->start = previousEnd();
  cmd->end=end;
  cmd->velocity=velocity;
  qreal dtx = fabs(cmd->end.x()-cmd->start.x())/cmd->velocity*1000.0;
  qreal dty = fabs(cmd->end.y()-cmd->start.y())/cmd->velocity*1000.0;
  cmd->duration = int(dtx+dty+1.0);
  mAnimList.append(cmd);
  return cmd;
}


// Retrieve the last commands start position or the current position if none exists
QPointF IntroSprite::previousStart()
{
  AnimationCommand* previous = mAnimList.last();
  if (previous != nullptr) return previous->start;
  else return QPointF(x()/getScale(), y()/getScale());
}


// Retrieve the last commands end position or the current position if none exists
QPointF IntroSprite::previousEnd()
{
  AnimationCommand* previous = mAnimList.last();
  if (previous != nullptr) return previous->end;
  return QPointF(x()/getScale(), y()/getScale());
}


// Retrieve the duration of the given animation command
int IntroSprite::duration(AnimationCommand* anim)
{
  return anim->duration;
}


// Retrieve the overall duration of the animation
int IntroSprite::animationDuration()
{
  int dura = 0;
  for (int i=0; i<mAnimList.size(); ++i)
  { 
    dura += mAnimList[i]->duration;
  }
  return dura;
}


// Globally delay the animation
void IntroSprite::delayAnimation(int duration)
{
  mDelayT += duration;
}


// CanvasItem advance method 
void IntroSprite::advance(int phase)
{
  // Ignore phase 0 (collisions)
  if (phase == 0)
  {
    QGraphicsItem::advance(phase);
    return ;
  }

  // No animation pending
  if (mAnimList.isEmpty())
  {
    QGraphicsItem::advance(phase);
    return ;
  }

  // First time animation start
  if (mStartAnimation)
  {
    mTime.restart();
    mStartAnimation = false;
  }
  int elapsed     = mTime.elapsed()+mDeltaT-mDelayT;
  if (elapsed<0) elapsed = 0;


  // Get the current/first command
  AnimationCommand* anim = mAnimList.first();

  // Execute commands who were passed in the time since the
  // last call
  while (anim != nullptr &&  anim->duration <= elapsed)
  {
      executeCmd(anim, anim->duration);
      mAnimList.removeFirst();
      elapsed -= anim->duration;
      if (!mAnimList.isEmpty()) anim = mAnimList.first();
      else anim = nullptr;
  }

  // Handle current command
  if (anim != nullptr)
  {
    executeCmd(anim, elapsed);
  }

  // Restart timer and store remaining time
  mTime.restart();
  mDeltaT = elapsed;
  mDelayT = 0;

  // Parent advance
  QGraphicsItem::advance(phase);

}


// Execute the given animation command. The given time is the time elapsed 
// since start of the animation sequence.
void IntroSprite::executeCmd(AnimationCommand* anim, int elapsed)
{
  if (anim == nullptr) return;

  // Scale
  double scale = this->getScale();

  // Pause command
  if (anim->cmd == AnimationCommand::PAUSE)
  {
    // Do nothing
  }
  // Show sprite command
  if (anim->cmd == AnimationCommand::SHOW)
  {
    show();
  }
  // Hide sprite command
  if (anim->cmd == AnimationCommand::HIDE)
  {
    hide();
  }
  // Set position command
  if (anim->cmd == AnimationCommand::POSITION)
  {
    qreal x   = anim->end.x();
    qreal y   = anim->end.y();
    setPos(x*scale, y*scale);
  }
  // Linear move command
  if (anim->cmd == AnimationCommand::LINEAR_DURATION)
  {
    double qt = double(elapsed)/double(anim->duration);
    qreal x   = anim->start.x() + qt*(anim->end.x()-anim->start.x());
    qreal y   = anim->start.y() + qt*(anim->end.y()-anim->start.y());
    setPos(x*scale, y*scale);
  }
  // Circle move command
  if (anim->cmd == AnimationCommand::CIRCLE_DURATION)
  {
    double qt   = double(elapsed)/double(anim->duration);
    double sign = 1.0;
    if (anim->start.x() > 0.5) sign = -1.0; // Direction of turn
    qreal cx = anim->start.x(); 
    qreal cy = anim->start.y();
    qreal x = cx + anim->radius*sin(sign*qt*2.0*M_PI);
    qreal y = cy-anim->radius + anim->radius*cos(sign*qt*2.0*M_PI);
    setPos(x*scale, y*scale);
  }
  // Manhatten move command
  if (anim->cmd == AnimationCommand::MANHATTEN)
  {
     if ( (fabs(anim->end.x()-x()/scale) <= 0.00001 && 
         fabs(anim->end.y()-y()/scale) <= 0.00001 ) ||
         (elapsed >= anim->duration) )
     {
       setPos(anim->end.x()*scale, anim->end.y()*scale);
     }
     // x-edge
     else if (fabs(anim->end.x()-x()/scale) > 0.00001)
     {
       qreal x = anim->start.x() + sign(anim->end.x()-anim->start.x())*anim->velocity*double(elapsed)/1000.0;
       // Finished?
       if ( (anim->end.x() > anim->start.x() && x >= anim->end.x()) ||
            (anim->end.x() < anim->start.x() && x <= anim->end.x()) )
       {
         x = anim->end.x();
       }
       setPos(x*scale, anim->start.y()*scale);
     }
     // y-edge
     else
     {
       qreal dtx = fabs(anim->end.x()-anim->start.x())/anim->velocity*1000.0;
       qreal y = anim->start.y() + sign(anim->end.y()-anim->start.y())*anim->velocity*double(elapsed-dtx)/1000.0;
       // Finished?
       if ( (anim->end.y() > anim->start.y() && y >= anim->end.y()) ||
            (anim->end.y() < anim->start.y() && y <= anim->end.y()) )
       {
         y = anim->end.y();
       }
       setPos(anim->end.x()*scale, y*scale);
     }
  }

}

