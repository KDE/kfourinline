/*
    This file is part of the KDE games kwin4 program
    SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWIN4_GAMESEQUENCE_H
#define KWIN4_GAMESEQUENCE_H

// KDEGames
#define USE_UNSTABLE_LIBKDEGAMESPRIVATE_API
#include <libkdegamesprivate/kgame/kgamesequence.h>

class KWin4Doc;

class KWin4GameSequence : public KGameSequence
{
    Q_OBJECT

public:
    explicit KWin4GameSequence(KWin4Doc *game);
    ~KWin4GameSequence() override;

public:
    KPlayer *nextPlayer(KPlayer *last, bool exclusive = true) override;
    int checkGameOver(KPlayer *player) override;

private:
    KWin4Doc *const m_game;
};

#endif
