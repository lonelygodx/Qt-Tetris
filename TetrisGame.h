#ifndef TETRISGAME_H
#define TETRISGAME_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include "GameEngine.h"
#include "InputHandler.h"
#include "ScoreManager.h"
#include <QWidget>

class GameEngine;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    void setGameEngine(GameEngine* engine);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    GameEngine* m_engine;

    // 绘制方法
    void drawGameField(QPainter& painter);
    void drawGhostBlock(QPainter& painter);  // 新增：绘制幽灵方块
    void drawCurrentBlock(QPainter& painter);
};

class NextBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NextBlockWidget(QWidget* parent = nullptr);
    void setNextBlock(const Block& block);
    void setCellSize(int size) { m_cellSize = size; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Block m_nextBlock;
    int m_cellSize;
};

class HoldBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HoldBlockWidget(QWidget* parent = nullptr);
    void setHoldBlock(const Block& block);
    void setCellSize(int size) { m_cellSize = size; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Block m_holdBlock;
    int m_cellSize;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    //void onGameStateChanged(GameEngine::GameState newState, GameEngine::GameState oldState);
    void onGameStateChanged(GameEngine::GameState newState);
    void onGameStatsUpdated(const GameStats& stats);
    void startNewGame();
    void showHighScores();
    void showHelp();
    void onNextBlockChanged();
    void onHoldBlockChanged();

private:
    void setupUI();
    // 创建菜单界面
    void createMenuWidget();
    // 创建游戏界面
    void createGameScreen();
    // 创建暂停界面
    void createPauseWidget();
    // 设置样式
    void setupStyles();
    // 创建右侧信息面板
    QWidget* createInfoPanel();
    void setupGameConnections();
    void setupConnections();
    void setupBasicConnections();
    void initializeSystems();
    void handleGameOver();

    // 核心系统组件
    QScopedPointer<GameEngine> m_gameEngine;
    QScopedPointer<InputHandler> m_inputHandler;
    QScopedPointer<ScoreManager> m_scoreManager;

    // UI组件
    QStackedWidget* m_stackedWidget;
    GameWidget* m_gameWidget;
    QWidget* m_menuWidget;
    QWidget* m_pauseWidget;

    // 菜单控件
    QLabel* m_scoreLabel;
    QLabel* m_levelLabel;
    QLabel* m_linesLabel;
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_highScoresButton;

    // 预览组件
    NextBlockWidget* m_nextBlockWidget;

    // 暂存组件
    HoldBlockWidget* m_holdBlockWidget;

    // 布局相关成员
    QWidget* m_gameScreen;           // 游戏界面主窗口
    QHBoxLayout* m_mainGameLayout;   // 主游戏布局（水平）
    QVBoxLayout* m_gameAreaLayout;   // 游戏区域布局（垂直）
    QVBoxLayout* m_infoPanelLayout;  // 信息面板布局（垂直）
};

#endif // TETRISGAME_H
