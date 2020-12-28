/*
    SPDX-FileCopyrightText: 2005, 2009 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kfontutils.h"

// own
#include "kfourinline_debug.h"
// Qt
#include <QFont>
// Std
#include <limits>

qreal KFontUtils::adaptFontSize(QGraphicsTextItem* string, qreal width, qreal height, qreal maxFontSize, qreal minFontSize, qreal precision)
{
    qreal size = maxFontSize;
    QRectF boundingRect;
    // Guard for avoiding possible infinite loop
    qreal lowerBound = 0;
    qreal higherBound = std::numeric_limits<double>::max();
    int count = 0;

    // Loop until found a point size that makes the text fit in the rectangle but
    // don't have more than precision pixels of unused space in any axys.
    while (count < 20) {
        QFont f = string->font();
        f.setPointSizeF(size);
        string->setFont(f);
        string->setTextWidth(width);
        boundingRect = string->boundingRect();
        // Error painting the text, return -1
        if (boundingRect.width() == 0 || boundingRect.height() == 0) {
            return -1;
        }
        // Text doesn't fit in rectangle or has too much unused space, adjust size
        else if (boundingRect.height() > height || boundingRect.height() < height - precision) {

            if (boundingRect.width() > width || boundingRect.height() > height)
              higherBound = qMin(higherBound, size);
            else
              lowerBound = qMax(lowerBound, size);

            if (lowerBound > 0 && higherBound < std::numeric_limits<double>::max()) {
              size = (lowerBound + higherBound) / 2;
            }
            else {
              size = (height / boundingRect.height()) * size;
            }

            count ++;
        }
        // Text fits correctly
        else {
            break;
        }
    }

    // Assure to return a value between minimum and maximum values.
    if (size < minFontSize)
      return minFontSize;
    else if (size > maxFontSize)
      return maxFontSize;
    else
      return size;
}

qreal KFontUtils::adaptFontSize(QGraphicsTextItem* text, const QSizeF &availableSize, qreal maxFontSize, qreal minFontSize, qreal precision)
{
    return adaptFontSize(text, availableSize.width(), availableSize.height(), maxFontSize, minFontSize, precision);
}
