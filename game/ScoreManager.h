#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <QObject>
#include <QVector>
#include <QDateTime>
#include <QSqlDatabase>

class ScoreManager : public QObject
{
    Q_OBJECT

public:
    struct HighScore {
        QString playerName;
        int score;
        int level;
        int lines;
        QDateTime date;

        HighScore() : score(0), level(1), lines(0) {}

        bool operator<(const HighScore& other) const {
            return score > other.score; // 降序排列
        }
    };

    explicit ScoreManager(QObject* parent = nullptr);
    ~ScoreManager();

    // 分数管理
    void addScore(int score, int level, int lines, const QString& playerName = "Player");
    QVector<HighScore> getHighScores(int count = 10) const;
    void resetHighScores();

    // 数据库操作
    bool initDatabase();
    bool loadHighScores();
    bool saveHighScores();

private:
    QVector<HighScore> m_highScores;
    QSqlDatabase m_database;
    static const int MAX_HIGH_SCORES = 5;

    bool createTable();
};

#endif // SCOREMANAGER_H
