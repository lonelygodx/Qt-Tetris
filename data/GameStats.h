#ifndef GAMESTATS_H
#define GAMESTATS_H
#include <QDateTime>

struct GameStats {
    int score;
    int level;
    int linesCleared;
    int currentCombo;
    int totalPieces;
    QDateTime startTime;
    int gameDuration; // 秒数

    GameStats() : score(0), level(1), linesCleared(0),
        currentCombo(0), totalPieces(0), gameDuration(0) {
    }

    void reset() {
        score = 0;
        level = 1;
        linesCleared = 0;
        currentCombo = 0;
        totalPieces = 0;
        gameDuration = 0;
        startTime = QDateTime();
    }
};

#endif // GAMESTATS_H
