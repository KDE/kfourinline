/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kwin4gamesequence.h"

// own
#include "kwin4doc.h"

KWin4GameSequence::KWin4GameSequence(KWin4Doc *game)
    : m_game(game)
{
}

KWin4GameSequence::~KWin4GameSequence() = default;

KPlayer *KWin4GameSequence::nextPlayer(KPlayer *last, bool exclusive)
{
    return m_game->doNextPlayer(last, exclusive);
}

int KWin4GameSequence::checkGameOver(KPlayer *player)
{
    return m_game->doCheckGameOver(player);
}

#include "moc_kwin4gamesequence.cpp"
