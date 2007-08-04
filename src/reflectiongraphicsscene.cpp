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
#include "reflectiongraphicsscene.h"

#include <QPainter>
#include <QRectF>
#include <QGraphicsItem>
#include <QTransform>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QLinearGradient>
#include <klocale.h>
#include <QTime>

ReflectionGraphicsScene::ReflectionGraphicsScene(QObject * parent) : QGraphicsScene(parent)
{
	mX = mY = mWidth = mHeight = 0;
}

ReflectionGraphicsScene::~ReflectionGraphicsScene()
{
}

void ReflectionGraphicsScene::setReflection(int x, int y, int width, int height) {
	mX = x;
	mY = y;
	mWidth = width;
	mHeight = height;

	QPoint p1, p2;
	p2.setY(mHeight);
	mGradient = QLinearGradient(p1, p2);
        mGradient.setColorAt(0, QColor(0, 0, 0, 100));
        mGradient.setColorAt(1, Qt::transparent);

	kDebug() << "Set reflection "<< x << " " << y << " " << width << " " << height ;

	mGradientPixmap = QPixmap(mWidth, mHeight);
	mGradientPixmap.fill(Qt::transparent);
	QPainter p( &mGradientPixmap );
	p.fillRect(0,0,mWidth, mHeight, mGradient);
	p.end();
}

void ReflectionGraphicsScene::drawItems(QPainter *painter, int numItems,
		                        QGraphicsItem *items[],
					const QStyleOptionGraphicsItem options[],
					QWidget *widget)
{
	QTime time;
	time.start();

	QPixmap image(painter->viewport().size());
	image.fill(Qt::transparent);

	{
		QPainter imagePainter(&image);

		for (int i = 0; i < numItems; ++i) {
			// Draw the item
			imagePainter.save();
			QMatrix sceneMatrix = items[i]->sceneMatrix();
			imagePainter.setMatrix(sceneMatrix, true);
			items[i]->paint(&imagePainter, &options[i], widget);
			
//			if(sceneMatrix
			

			imagePainter.restore();
		}

		painter->drawPixmap( 0,0,image);
	}
	kDebug() << "1 : " << time.elapsed();
	if(mWidth == 0) return; // No reflections
	
	image = image.copy(mX,mY-mHeight,mWidth,mHeight);
	kDebug() << "2 : " << time.elapsed();
	image = image.transformed(QMatrix(1, 0, 0, -1, 0, mHeight), Qt::FastTransformation);
	kDebug() << "3 : " << time.elapsed();

	QPainter p (&image);
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
	kDebug() << "4 : " << time.elapsed();
	p.drawPixmap(0,0,mGradientPixmap);
	p.end();
	painter->drawPixmap(mX,mY, image);
	kDebug() << "5 : " << time.elapsed();
}

#include "reflectiongraphicsscene.moc"

