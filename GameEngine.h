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
    enum GameState {
        STATE_STOPPED,
        STATE_RUNNING,
        STATE_PAUSED,
        STATE_GAME_OVER
    };

    explicit GameEngine(QObject* parent = nullptr);
    ~GameEngine();

    // 游戏控制
    bool initialize(int width = 10, int height = 20);
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void endGame();

    // 方块操作
    bool moveLeft();
    bool moveRight();
    bool moveDown();
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

    // 幽灵方块配置
    void setGhostBlockEnabled(bool enabled) { m_ghostEnabled = enabled; }
    bool isGhostBlockEnabled() const { return m_ghostEnabled; }

signals:
    void gameStateChanged(GameState newState);
    void gameStatsUpdated(const GameStats& stats);
    void gameFieldChanged();
    void currentBlockChanged();
    void nextBlockChanged();
    void holdBlockChanged();

private slots:
    void updateGame();

private:
    // 游戏逻辑
    void spawnNewBlock();
    void placeCurrentBlock();
    void lockCurrentBlock();
    int clearCompletedLines();
    void updateGameStats(int linesCleared);
    void calculateScore(int linesCleared);
    void updateLevel();

    // 辅助方法
    bool isValidPosition(const Block& block, int dx = 0, int dy = 0) const;
    void resetGameStats();

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
    QTimer* m_gameTimer;

    // 动态下落相关
    float m_fallProgress;      // 下落进度 0.0 - 1.0
    bool m_fastDrop;           // 是否快速下落
    int m_fallSpeed;           // 下落速度 (ms)
    int m_fastFallSpeed;       // 快速下落速度 (ms)
    qint64 m_lastUpdateTime;   // 上次更新时间

    // 系统组件
    QScopedPointer<BlockFactory> m_blockFactory;

    bool m_ghostEnabled;  // 是否显示幽灵方块
};

#endif // GAMEENGINE_H
