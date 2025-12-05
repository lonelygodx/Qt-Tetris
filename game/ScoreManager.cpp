#include "ScoreManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <algorithm>
#include <QCoreApplication>
#include "GameConfig.h"

ScoreManager::ScoreManager(QObject* parent)
    : QObject(parent)
{
    // 初始化数据库
    if (!initDatabase()) {
        qWarning() << "Failed to initialize database";
    }

    loadHighScores();
}

ScoreManager::~ScoreManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool ScoreManager::initDatabase()
{
    // 获取应用程序数据目录
    //QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/data";

    // 使用应用程序所在目录
    QString dataPath = QCoreApplication::applicationDirPath() + "/data";

    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString dbPath = dataPath + "/tetris_scores.db";

    // 设置数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qWarning() << "Cannot open database:" << m_database.lastError().text();
        return false;
    }

    // 创建表
    return createTable();
}

bool ScoreManager::createTable()
{
    QSqlQuery query;
    QString createTableSql =
        "CREATE TABLE IF NOT EXISTS highscores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "playerName TEXT NOT NULL, "
        "score INTEGER NOT NULL, "
        "level INTEGER NOT NULL, "
        "lines INTEGER NOT NULL, "
        "date TEXT NOT NULL"
        ")";

    if (!query.exec(createTableSql)) {
        qWarning() << "Create table failed:" << query.lastError().text();
        return false;
    }

    return true;
}

void ScoreManager::addScore(int score, int level, int lines, const QString& playerName)
{
    HighScore newScore;
    newScore.playerName = playerName;
    newScore.score = score;
    newScore.level = level;
    newScore.lines = lines;
    newScore.date = QDateTime::currentDateTime();

    // 插入到数据库
    QSqlQuery query;
    query.prepare("INSERT INTO highscores (playerName, score, level, lines, date) "
                  "VALUES (:playerName, :score, :level, :lines, :date)");
    query.bindValue(":playerName", newScore.playerName);
    query.bindValue(":score", newScore.score);
    query.bindValue(":level", newScore.level);
    query.bindValue(":lines", newScore.lines);
    query.bindValue(":date", newScore.date.toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Insert score failed:" << query.lastError().text();
        return;
    }

    // 更新内存中的高分列表
    m_highScores.append(newScore);

    // 排序并保留前N名
    std::sort(m_highScores.begin(), m_highScores.end());
    if (m_highScores.size() > MAX_HIGH_SCORES) {
        m_highScores.resize(MAX_HIGH_SCORES);

        // 从数据库中删除超出限制的低分记录
        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM highscores WHERE id NOT IN ("
                            "SELECT id FROM highscores ORDER BY score DESC LIMIT :limit)");
        deleteQuery.bindValue(":limit", MAX_HIGH_SCORES);
        if (!deleteQuery.exec()) {
            qWarning() << "Delete excess scores failed:" << deleteQuery.lastError().text();
        }
    }
}

QVector<ScoreManager::HighScore> ScoreManager::getHighScores(int count) const
{
    count = qMin(count, m_highScores.size());
    return QVector<HighScore>(m_highScores.constBegin(), m_highScores.constBegin() + count);
}

void ScoreManager::resetHighScores()
{
    QSqlQuery query;
    if (!query.exec("DELETE FROM highscores")) {
        qWarning() << "Reset highscores failed:" << query.lastError().text();
        return;
    }

    m_highScores.clear();
}

bool ScoreManager::loadHighScores()
{
    QSqlQuery query("SELECT playerName, score, level, lines, date FROM highscores "
                    "ORDER BY score DESC LIMIT " + QString::number(MAX_HIGH_SCORES));

    if (!query.exec()) {
        qWarning() << "Load highscores failed:" << query.lastError().text();
        return false;
    }

    m_highScores.clear();

    while (query.next()) {
        HighScore score;
        score.playerName = query.value("playerName").toString();
        score.score = query.value("score").toInt();
        score.level = query.value("level").toInt();
        score.lines = query.value("lines").toInt();
        score.date = QDateTime::fromString(query.value("date").toString(), Qt::ISODate);

        m_highScores.append(score);
    }

    return true;
}

bool ScoreManager::saveHighScores()
{
    // 对于SQLite，数据在插入时已经保存，这个方法主要用于兼容性
    // 如果需要强制保存，可以调用数据库的提交（但SQLite默认自动提交）
    return true;
}
