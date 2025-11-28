#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QWidget>

#include "GameEngine.h"
#include "InputHandler.h"
#include "ScoreManager.h"
#include "GameWidget.h"

class GameEngine;

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
    // 游戏状态事件
    void onGameStateChanged(GameEngine::GameState newState);
    void onGameStatsUpdated(const GameStats& stats);
    // 游戏控制事件
    void startNewGame();
    void showHighScores();
    void showHelp();
    // 游戏中事件
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

#endif // MAINWINDOW_H
