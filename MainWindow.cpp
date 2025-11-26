#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>
#include "MainWindow.h"

// MainWindow 实现
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_gameWidget(nullptr)
    , m_menuWidget(nullptr)
    , m_pauseWidget(nullptr)
    , m_scoreLabel(nullptr)
    , m_levelLabel(nullptr)
    , m_linesLabel(nullptr)
    , m_startButton(nullptr)
    , m_pauseButton(nullptr)
    , m_highScoresButton(nullptr)
    , m_nextBlockWidget(nullptr)
    , m_gameScreen(nullptr)
    , m_mainGameLayout(nullptr)
    , m_gameAreaLayout(nullptr)
    , m_infoPanelLayout(nullptr)
{
    setWindowIcon(QIcon(":/resources/icons/games_tetris.png"));

    // 1. 先初始化系统组件（包括游戏引擎）
    initializeSystems();

    // 2. 创建UI
    setupUI();

    // 3. 设置游戏相关的信号槽连接
    setupGameConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initializeSystems()
{
    // 创建游戏引擎
    m_gameEngine.reset(new GameEngine());

    // 初始化游戏引擎
    m_gameEngine->initialize();

    // 创建输入处理器
    m_inputHandler.reset(new InputHandler());

    // 创建分数管理器
    m_scoreManager.reset(new ScoreManager());
}

void MainWindow::setupUI()
{
    setWindowTitle("俄罗斯方块");
    setFixedSize(800, 700);

    // 创建主堆叠窗口
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // 创建菜单界面
    createMenuWidget();

    // 创建游戏界面
    createGameScreen();

    // 创建暂停界面
    createPauseWidget();

    // 设置样式
    setupStyles();

    // 连接基本UI信号槽（不依赖游戏引擎）
    setupBasicConnections();
}

void MainWindow::setupGameConnections()
{
    if (!m_gameEngine) {
        qDebug() << "CRITICAL ERROR: Game engine is still null in setupGameConnections!";
        // 尝试重新初始化
        initializeSystems();
        // 重新初始化失败
        if (!m_gameEngine) {
            qDebug() << "Failed to initialize game engine, aborting game connections";
            return;
        }
    }

    // 连接游戏引擎信号
    connect(m_gameEngine.data(), &GameEngine::gameStateChanged, this, &MainWindow::onGameStateChanged);
    // 连接游戏数据更新信号
    connect(m_gameEngine.data(), &GameEngine::gameStatsUpdated, this, &MainWindow::onGameStatsUpdated);
    // 连接下一个方块变化信号
    connect(m_gameEngine.data(), &GameEngine::nextBlockChanged, this, &MainWindow::onNextBlockChanged);
    // 连接Hold方块变化信号
    connect(m_gameEngine.data(), &GameEngine::holdBlockChanged, this, &MainWindow::onHoldBlockChanged);
    // 连接输入处理器
    if (m_inputHandler) {
        connect(m_inputHandler.data(), &InputHandler::actionTriggered,
                this, [this](InputHandler::GameAction action) {
                    if (!m_gameEngine) {
                        qDebug() << "ERROR: Game engine is null in input handler!";
                        return;
                    }
                    switch (action) {
                    case InputHandler::ACTION_MOVE_LEFT:
                        m_gameEngine->moveLeft();
                        break;
                    case InputHandler::ACTION_MOVE_RIGHT:
                        m_gameEngine->moveRight();
                        break;
                    case InputHandler::ACTION_SOFT_DROP:
                        m_gameEngine->softDrop();
                        break;
                    case InputHandler::ACTION_HARD_DROP:
                        m_gameEngine->hardDrop();
                        break;
                    case InputHandler::ACTION_ROTATE_CW:
                        m_gameEngine->rotateClockwise();
                        break;
                    case InputHandler::ACTION_ROTATE_CCW:
                        m_gameEngine->rotateCounterClockwise();
                        break;
                    case InputHandler::ACTION_HOLD:
                        m_gameEngine->holdBlock();
                        break;
                    case InputHandler::ACTION_PAUSE:
                        if (m_gameEngine->getGameState() == GameEngine::STATE_RUNNING) {
                            m_gameEngine->pauseGame();
                        } else if (m_gameEngine->getGameState() == GameEngine::STATE_PAUSED) {
                            m_gameEngine->resumeGame();
                        }
                        break;
                    case InputHandler::ACTION_RESTART:
                        m_gameEngine->restartGame();
                        break;
                    default:
                        break;
                    }
                });
    }

    // 处理按键释放（特别是软下落释放）
    connect(m_inputHandler.data(), &InputHandler::actionReleased,
            this, [this](InputHandler::GameAction action) {
                if (!m_gameEngine) {
                    qDebug() << "ERROR: Game engine is null in input release handler!";
                    return;
                }
                switch (action) {
                case InputHandler::ACTION_SOFT_DROP:
                    m_gameEngine->stopSoftDrop();  // 停止软下落
                    break;
                default:
                    // 其他动作的释放不需要特殊处理
                    break;
                }
            });
}

void MainWindow::createMenuWidget()
{
    m_menuWidget = new QWidget(this);
    QVBoxLayout* menuLayout = new QVBoxLayout(m_menuWidget);

    m_startButton = new QPushButton("开始游戏", this);
    m_highScoresButton = new QPushButton("高分榜", this);
    QPushButton* helpButton = new QPushButton("帮助", this);
    QPushButton* quitButton = new QPushButton("退出", this);

    // 设置菜单按钮样式
    QString buttonStyle = "QPushButton { "
                          "background-color: #3498db; "
                          "color: white; "
                          "border: none; "
                          "padding: 10px; "
                          "font-size: 16px; "
                          "border-radius: 5px; "
                          "min-width: 150px; "
                          "}"
                          "QPushButton:hover { "
                          "background-color: #2980b9; "
                          "}";

    m_startButton->setStyleSheet(buttonStyle);
    m_highScoresButton->setStyleSheet(buttonStyle);
    helpButton->setStyleSheet(buttonStyle);
    quitButton->setStyleSheet(buttonStyle);

    menuLayout->addStretch();
    menuLayout->addWidget(m_startButton);
    menuLayout->addWidget(m_highScoresButton);
    menuLayout->addWidget(helpButton);
    menuLayout->addWidget(quitButton);
    menuLayout->addStretch();
    menuLayout->setAlignment(Qt::AlignCenter);

    m_stackedWidget->addWidget(m_menuWidget);
}

void MainWindow::createGameScreen()
{
    // 创建游戏主界面
    m_gameScreen = new QWidget(this);
    m_mainGameLayout = new QHBoxLayout(m_gameScreen);

    // 创建左侧游戏区域
    QWidget* gameArea = new QWidget(this);
    m_gameAreaLayout = new QVBoxLayout(gameArea);

    m_gameWidget = new GameWidget(this);

    // 在创建游戏部件后立即设置游戏引擎
    if (m_gameEngine) {
        m_gameWidget->setGameEngine(m_gameEngine.data());
    } else {
        qDebug() << "ERROR: Game engine is null when setting to game widget";
    }

    m_gameAreaLayout->addWidget(m_gameWidget);
    m_gameAreaLayout->setAlignment(m_gameWidget, Qt::AlignCenter);

    // 创建右侧信息面板
    QWidget* infoPanel = createInfoPanel();

    // 添加到主布局
    m_mainGameLayout->addWidget(gameArea);
    m_mainGameLayout->addWidget(infoPanel);

    // 设置背景
    gameArea->setStyleSheet("background-color: rgba(20, 20, 20, 200);");

    m_stackedWidget->addWidget(m_gameScreen);
}

QWidget* MainWindow::createInfoPanel()
{
    QWidget* infoPanel = new QWidget(this);
    infoPanel->setFixedWidth(220);
    m_infoPanelLayout = new QVBoxLayout(infoPanel);

    // 游戏状态信息
    m_scoreLabel = new QLabel("分数: 0", this);
    m_levelLabel = new QLabel("等级: 1", this);
    m_linesLabel = new QLabel("行数: 0", this);

    // 设置信息标签样式
    QString labelStyle = "QLabel { color: white; font-size: 16px; font-weight: bold; padding: 5px; }";
    m_scoreLabel->setStyleSheet(labelStyle);
    m_levelLabel->setStyleSheet(labelStyle);
    m_linesLabel->setStyleSheet(labelStyle);

    // Hold方块预览
    QLabel* holdLabel = new QLabel("暂存方块:", this);
    holdLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; padding: 5px; }");

    m_holdBlockWidget = new HoldBlockWidget(this);

    // 初始化Hold方块预览窗口
    if (m_holdBlockWidget) {
        m_holdBlockWidget->setHoldBlock(m_gameEngine->getHoldBlock());
    }

    // 下一个方块预览
    QLabel* nextLabel = new QLabel("下一个方块:", this);
    nextLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; padding: 5px; }");

    m_nextBlockWidget = new NextBlockWidget(this);

    // 初始化预览窗口
    if (m_gameEngine) {
        m_nextBlockWidget->setNextBlock(m_gameEngine->getNextBlock());
    }

    // 控制说明
    QLabel* controlsLabel = new QLabel("控制说明:", this);
    controlsLabel->setStyleSheet("QLabel { color: white; font-size: 14px; font-weight: bold; padding: 5px; margin-top: 20px; }");

    QLabel* controlsText = new QLabel(
        "←→或AD : 左右移动\n"
        "↑或W : 顺时针旋转\n"
        "Z : 逆时针旋转\n"
        "↓或S : 软降落\n"
        "空格 : 硬降落\n"
        "C : 暂存方块\n"
        "P : 暂停/继续\n"
        "R : 重新开始", this);
    controlsText->setStyleSheet("QLabel { color: lightgray; font-size: 12px; padding: 5px; line-height: 1.5; }");

    // 添加到信息面板布局
    m_infoPanelLayout->addWidget(m_scoreLabel);
    m_infoPanelLayout->addWidget(m_levelLabel);
    m_infoPanelLayout->addWidget(m_linesLabel);
    m_infoPanelLayout->addSpacing(10);
    // 添加下一个方块预览
    m_infoPanelLayout->addWidget(nextLabel);
    m_infoPanelLayout->addWidget(m_nextBlockWidget);
    m_infoPanelLayout->addSpacing(10);
    // 添加Hold方块预览
    m_infoPanelLayout->addWidget(holdLabel);
    m_infoPanelLayout->addWidget(m_holdBlockWidget);
    m_infoPanelLayout->addSpacing(10);
    // 控制说明信息面板
    m_infoPanelLayout->addWidget(controlsLabel);
    m_infoPanelLayout->addWidget(controlsText);
    m_infoPanelLayout->addStretch();

    // 设置信息面板背景
    infoPanel->setStyleSheet("background-color: rgba(40, 40, 40, 200); border-radius: 10px;");

    return infoPanel;
}

void MainWindow::createPauseWidget()
{
    m_pauseWidget = new QWidget(this);
    QVBoxLayout* pauseLayout = new QVBoxLayout(m_pauseWidget);

    m_pauseButton = new QPushButton("继续游戏", this);
    QPushButton* menuButton = new QPushButton("返回主菜单", this);

    // 设置暂停界面按钮样式
    QString pauseButtonStyle = "QPushButton { "
                               "background-color: #e74c3c; "
                               "color: white; "
                               "border: none; "
                               "padding: 10px; "
                               "font-size: 16px; "
                               "border-radius: 5px; "
                               "min-width: 150px; "
                               "}"
                               "QPushButton:hover { "
                               "background-color: #c0392b; "
                               "}";

    m_pauseButton->setStyleSheet(pauseButtonStyle);
    menuButton->setStyleSheet(pauseButtonStyle);

    pauseLayout->addStretch();
    pauseLayout->addWidget(m_pauseButton);
    pauseLayout->addWidget(menuButton);
    pauseLayout->addStretch();
    pauseLayout->setAlignment(Qt::AlignCenter);

    m_stackedWidget->addWidget(m_pauseWidget);
}

void MainWindow::setupStyles()
{
    // 设置主窗口背景
    setStyleSheet("MainWindow { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2c3e50, stop:1 #34495e); }");
}

void MainWindow::setupBasicConnections()
{
    // 连接菜单按钮信号
    if (m_startButton) {
        connect(m_startButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
    } else {
        qDebug() << "ERROR: Start button is null!";
    }

    if (m_highScoresButton) {
        connect(m_highScoresButton, &QPushButton::clicked, this, &MainWindow::showHighScores);
    }

    // 连接其他按钮
    QList<QPushButton*> buttons = m_menuWidget->findChildren<QPushButton*>();

    for (QPushButton* button : std::as_const(buttons)) {
        if (button->text() == "帮助") {
            connect(button, &QPushButton::clicked, this, &MainWindow::showHelp);
        } else if (button->text() == "退出") {
            connect(button, &QPushButton::clicked, this, &QApplication::quit);
        }
    }

    // 连接暂停界面按钮
    if (m_pauseButton) {
        connect(m_pauseButton, &QPushButton::clicked, this, [this]() {
            if (m_gameEngine) {
                m_gameEngine->resumeGame();
            }
        });
    }

    QList<QPushButton*> pauseButtons = m_pauseWidget->findChildren<QPushButton*>();
    for (QPushButton* button : std::as_const(pauseButtons)) {
        if (button->text() == "返回主菜单") {
            connect(button, &QPushButton::clicked, this, [this]() {
                if (m_gameEngine) {
                    m_gameEngine->endGame();
                }
                m_stackedWidget->setCurrentWidget(m_menuWidget);
            });
        }
    }
}

// 处理下一个方块变化的槽函数
void MainWindow::onNextBlockChanged()
{
    if (m_nextBlockWidget && m_gameEngine) {
        m_nextBlockWidget->setNextBlock(m_gameEngine->getNextBlock());
    }
}

// 处理Hold方块变化的槽函数
void MainWindow::onHoldBlockChanged()
{
    if (m_holdBlockWidget && m_gameEngine) {
        m_holdBlockWidget->setHoldBlock(m_gameEngine->getHoldBlock());
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (!m_inputHandler->processKeyEvent(event)) {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (!m_inputHandler->processKeyEvent(event)) {
        QMainWindow::keyReleaseEvent(event);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
        // 可以在这里保存游戏状态或设置
}

void MainWindow::onGameStateChanged(GameEngine::GameState newState)
{
    switch (newState) {
    case GameEngine::STATE_STOPPED:
        m_stackedWidget->setCurrentWidget(m_menuWidget);
        break;
    case GameEngine::STATE_RUNNING:
        m_stackedWidget->setCurrentWidget(m_gameScreen);
        m_gameWidget->setFocus();
        break;
    case GameEngine::STATE_PAUSED:
        m_stackedWidget->setCurrentWidget(m_pauseWidget);
        break;
    case GameEngine::STATE_GAME_OVER:
        handleGameOver();
        break;
    }

    m_gameWidget->update();
}

void MainWindow::handleGameOver()
{
    if (!m_gameEngine) {
        qDebug() << "ERROR: Game engine is null in game over handler";
        return;
    }

    // 获取游戏统计
    GameStats stats = m_gameEngine->getGameStats();

    // 添加到高分榜
    if (m_scoreManager) {
        m_scoreManager->addScore(stats.score, stats.level, stats.linesCleared);
    }

    // 显示游戏结束对话框
    QMessageBox::information(this, "游戏结束",
                             QString("游戏结束!\n得分: %1\n等级: %2\n消除行数: %3")
                                 .arg(stats.score)
                                 .arg(stats.level)
                                 .arg(stats.linesCleared));

    // 返回主菜单
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentWidget(m_menuWidget);
    }
}

void MainWindow::onGameStatsUpdated(const GameStats& stats)
{
    if (m_scoreLabel) {
        m_scoreLabel->setText(QString("分数: %1").arg(stats.score));
    }
    if (m_levelLabel) {
        m_levelLabel->setText(QString("等级: %1").arg(stats.level));
    }
    if (m_linesLabel) {
        m_linesLabel->setText(QString("行数: %1").arg(stats.linesCleared));
    }
}

void MainWindow::startNewGame()
{
    if (!m_gameEngine) {
        qDebug() << "ERROR: Game engine is null!";
        return;
    }

    m_gameEngine->startGame();
}

void MainWindow::showHighScores()
{
    QVector<ScoreManager::HighScore> highscores = m_scoreManager->getHighScores(10);

    QString scoreText = "高分榜:\n\n";
    for (int i = 0; i < highscores.size(); ++i) {
        const auto& score = highscores[i];
        scoreText += QString("%1. %2\n 分数：%3\n 等级：%4\n 行数：%5\n 日期：%6\n\n")
                         .arg(i + 1)
                         .arg(score.playerName)
                         .arg(score.score)
                         .arg(score.level)
                         .arg(score.lines)
                         .arg(score.date.toString("yyyy-MM-dd"));
    }

    QMessageBox::information(this, "高分榜", scoreText);
}

void MainWindow::showHelp()
{
    QString helpText =
        "游戏控制:\n"
        "←或A →或D : 左右移动\n"
        "↑或W : 顺时针旋转\n"
        "Z : 逆时针旋转\n"
        "↓或S : 软降落\n"
        "空格 : 硬降落\n"
        "C : 暂存方块\n"
        "P : 暂停/继续\n"
        "R : 重新开始\n\n"
        "游戏规则:\n"
        "- 消除完整的行来获得分数\n"
        "- 等级越高，方块下落速度越快\n"
        "- 一次消除多行会获得额外分数";

    QMessageBox::information(this, "游戏帮助", helpText);
}

