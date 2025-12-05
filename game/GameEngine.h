#ifndef GAMEENGINE_H
#define GAMEENGINE_H
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include "GameField.h"
#include "Block.h"
#include "BlockFactory.h"
#include "GameStats.h"

class GameEngine : public QObject
{
    Q_OBJECT

public:
    // 游戏运行状态
    enum GameState {
        STATE_STOPPED,
        STATE_RUNNING,
        STATE_PAUSED,
        STATE_GAME_OVER
    };

    explicit GameEngine(QObject* parent = nullptr);
    ~GameEngine();

    // 游戏控制
    bool initialize();
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void endGame();

    // 方块操作
    bool moveLeft();
    bool moveRight();
    bool rotateClockwise();
    bool rotateCounterClockwise();
    void softDrop();
    void stopSoftDrop();
    void hardDrop();
    void holdBlock();

    // 游戏状态查询
    GameState getGameState() const { return m_gameState; }
    const GameStats& getGameStats() const { return m_gameStats; }
    const GameField& getGameField() const { return m_gameField; }
    const Block& getCurrentBlock() const { return m_currentBlock; }
    const Block& getNextBlock() const { return m_nextBlock; }
    const Block& getHoldBlock() const { return m_holdBlock; }
    bool canHold() const { return m_canHold; }

    // 动态下落相关
    float getFallProgress() const { return m_fallProgress; }
    bool isFastDropping() const { return m_fastDrop; }

    // 幽灵方块相关
    Block getGhostBlock() const;

signals:
    void gameStateChanged(GameEngine::GameState newState);
    void gameStatsUpdated(const GameStats& stats);
    void gameFieldChanged();
    void currentBlockChanged();
    void nextBlockChanged();
    void holdBlockChanged();

    // 更新等级信号
    void updateNewLevel(int level);

private slots:
    void updateGame();

private:
    // 游戏逻辑
    void extracted(bool &canSpawn, QVector<Position> &cells);
    void spawnNewBlock();                   // 生成新方块
    void placeCurrentBlock();               // 放置方块
    void lockCurrentBlock();                // 锁定方块
    int clearCompletedLines();              // 消除所有完整行
    void updateGameStats(int linesCleared); // 更新游戏数据
    void calculateScore(int linesCleared);  // 计算得分
    void updateLevel();                     // 更新游戏等级

    // 辅助方法
    bool isValidPosition(const Block &block, int dx = 0, int dy = 0) const; // 检测位置是否合法
    void resetGameStats();                  // 重置游戏数据

    // 幽灵方块计算
    Position calculateGhostPosition() const;

    // 成员变量
    GameState m_gameState;
    GameField m_gameField;
    Block m_currentBlock;
    Block m_nextBlock;
    Block m_holdBlock;
    bool m_canHold;
    GameStats m_gameStats;

    // 计时器
    QTimer *m_gameTimer;

    // 动态下落相关
    float m_fallProgress;    // 下落进度 0.0 - 1.0
    bool m_fastDrop;         // 是否快速下落
    int m_fallSpeed;         // 下落速度 (ms/cell)
    int m_fastFallSpeed;     // 快速下落速度 (ms/cell)
    qint64 m_lastUpdateTime; // 上次更新时间

    // 系统组件
    QScopedPointer<BlockFactory> m_blockFactory;
};

#endif // GAMEENGINE_H
