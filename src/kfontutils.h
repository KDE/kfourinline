/*
    SPDX-FileCopyrightText: 2005, 2009 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KFONTUTILS_H
#define KFONTUTILS_H

// Qt
#include <QGraphicsTextItem>

class QSizeF;

namespace KFontUtils
{
    /** Modifiers for the adaptFontSize function */
    enum AdaptFontSizeOption {
        NoFlags = 0x01 /** No modifier */,
        DoNotAllowWordWrap = 0x02  /** Do not use word wrapping */
    };
    Q_DECLARE_FLAGS(AdaptFontSizeOptions, AdaptFontSizeOption)

    /** Helper function that calculates the biggest font size (in points) used
        drawing a centered text using word wrapping.
        @param text The QGraphicsTextItem you want to draw
        @param width The available width for drawing
        @param height The available height for drawing
        @param maxFontSize The maximum font size (in points) to consider
        @param minFontSize The minimum font size (in points) to consider
        @param flags The modifiers for how the text is painted
        @param precision The maximum unused space (in pixels) in any axys
        @return The calculated biggest font size (in points) that draws the text
                in the given dimensions. Cannot return smaller than minFontSize neither
                bigger than maxFontSize. Can return -1 on error.
        @since KDE 4.7
    */
    qreal adaptFontSize(QGraphicsTextItem* text,
                                     qreal width,
                                     qreal height,
                                     qreal maxFontSize = 28.0,
                                     qreal minFontSize = 1.0,
                                     qreal precision = 1.0);

    /** Convenience function for adaptFontSize that accepts a QSizeF instead two qreals
        @since KDE 4.7
    */
    qreal adaptFontSize(QGraphicsTextItem* text,
                                     const QSizeF &availableSize,
                                     qreal maxFontSize = 28.0,
                                     qreal minFontSize = 1.0,
                                     qreal precision = 1.0);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(KFontUtils::AdaptFontSizeOptions)

#endif

