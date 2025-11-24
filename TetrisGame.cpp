#include "TetrisGame.h"
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>

// GameWidget 实现
GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
{
    setFixedSize(300, 600);
    setFocusPolicy(Qt::StrongFocus);
}

void GameWidget::setGameEngine(GameEngine* engine)
{
    m_engine = engine;

    if (m_engine) {
        connect(m_engine, &GameEngine::gameFieldChanged, this, QOverload<>::of(&QWidget::update));
        connect(m_engine, &GameEngine::currentBlockChanged, this, QOverload<>::of(&QWidget::update));
    } else {
        qDebug() << "ERROR: Game engine is null in setGameEngine";
    }
}

void GameWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (!m_engine) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor(20, 20, 20));

    // 绘制游戏场地
    const auto& field = m_engine->getGameField();
    int cellSize = 30;

    // 绘制网格
    painter.setPen(QPen(QColor(40, 40, 40), 1));
    for (int x = 0; x <= field.getWidth(); ++x) {
        painter.drawLine(x * cellSize, 0, x * cellSize, field.getHeight() * cellSize);
    }
    for (int y = 0; y <= field.getHeight(); ++y) {
        painter.drawLine(0, y * cellSize, field.getWidth() * cellSize, y * cellSize);
    }

    // 绘制已放置的方块
    drawGameField(painter);

    // 绘制幽灵方块
    if (m_engine->getGameState() == GameEngine::STATE_RUNNING) {
        drawGhostBlock(painter);
    }

    // 绘制当前方块
    if (m_engine->getGameState() == GameEngine::STATE_RUNNING) {
        drawCurrentBlock(painter);
    }
}

void GameWidget::drawGameField(QPainter& painter)
{
    if (!m_engine) return;

    const auto& field = m_engine->getGameField();
    int cellSize = 30;

    // 绘制已放置的方块
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            if (!field.isCellEmpty(x, y)) {
                QColor color = field.getCellColor(x, y);

                // 绘制方块主体
                painter.fillRect(x * cellSize, y * cellSize, cellSize, cellSize, color);

                // 绘制高光效果
                painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
                painter.drawLine(x * cellSize, y * cellSize, (x + 1) * cellSize, y * cellSize);
                painter.drawLine(x * cellSize, y * cellSize, x * cellSize, (y + 1) * cellSize);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(0, 0, 0, 100), 2));
                painter.drawLine((x + 1) * cellSize, y * cellSize, (x + 1) * cellSize, (y + 1) * cellSize);
                painter.drawLine(x * cellSize, (y + 1) * cellSize, (x + 1) * cellSize, (y + 1) * cellSize);

                // 绘制内部细节
                painter.setPen(QPen(QColor(255, 255, 255, 50), 1));
                painter.drawRect(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4);
            }
        }
    }
}

void GameWidget::drawGhostBlock(QPainter& painter)
{
    if (!m_engine || m_engine->getGameState() != GameEngine::STATE_RUNNING) return;

    // 获取幽灵方块
    Block ghostBlock = m_engine->getGhostBlock();

    // 如果幽灵方块位置与当前方块位置相同（已经在底部），则不绘制
    if (ghostBlock.getPosition().y == m_engine->getCurrentBlock().getPosition().y) {
        return;
    }

    auto cells = ghostBlock.getOccupiedCells();
    QColor ghostColor = ghostBlock.getColor();

    // 设置幽灵方块的颜色（半透明）
    ghostColor.setAlpha(80);

    int cellSize = 30;

    // 绘制幽灵方块
    for (const auto& cell : cells) {
        if (cell.y >= 0) {
            // 绘制虚影
            painter.fillRect(cell.x * cellSize, cell.y * cellSize, cellSize, cellSize, ghostColor);
        }
    }

    // 可选：绘制从当前方块到幽灵方块的连线
    // if (cells.size() > 0) {
    //     const auto& currentBlock = m_engine->getCurrentBlock();
    //     auto currentCells = currentBlock.getOccupiedCells();

    //     if (currentCells.size() > 0 && cells.size() > 0) {
    //         // 找到当前方块和幽灵方块的中心点
    //         Position currentCenter = currentCells[0];
    //         Position ghostCenter = cells[0];

    //         // 计算中心点（取所有细胞的平均值）
    //         for (int i = 1; i < currentCells.size(); i++) {
    //             currentCenter.x += currentCells[i].x;
    //             currentCenter.y += currentCells[i].y;
    //         }
    //         for (int i = 1; i < cells.size(); i++) {
    //             ghostCenter.x += cells[i].x;
    //             ghostCenter.y += cells[i].y;
    //         }

    //         currentCenter.x /= currentCells.size();
    //         currentCenter.y /= currentCells.size();
    //         ghostCenter.x /= cells.size();
    //         ghostCenter.y /= cells.size();

    //         // 绘制虚线连接线
    //         QPen linePen(QColor(255, 255, 255, 80));
    //         linePen.setStyle(Qt::DotLine);
    //         linePen.setWidth(1);
    //         painter.setPen(linePen);

    //         painter.drawLine(
    //             currentCenter.x * cellSize + cellSize / 2,
    //             currentCenter.y * cellSize + cellSize / 2,
    //             ghostCenter.x * cellSize + cellSize / 2,
    //             ghostCenter.y * cellSize + cellSize / 2
    //         );
    //     }
    // }
}

void GameWidget::drawCurrentBlock(QPainter& painter)
{
    if (!m_engine || m_engine->getGameState() != GameEngine::STATE_RUNNING) return;

    const auto& currentBlock = m_engine->getCurrentBlock();
    auto cells = currentBlock.getOccupiedCells();
    QColor blockColor = currentBlock.getColor();

    // 获取下落进度
    float fallProgress = m_engine->getFallProgress();

    int cellSize = 30;

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 2));

    for (const auto& cell : cells) {
        // 计算实际绘制位置（包括下落进度）
        float actualY = cell.y + static_cast<int>(trunc(fallProgress));  // 对下落进度取整，防止出现在某一格内的情况
        int drawY = actualY * cellSize;

        // 只绘制场地内的部分
        if (actualY >= 0 && actualY < m_engine->getGameField().getHeight()) {
            painter.drawRect(cell.x * cellSize, drawY, cellSize, cellSize);

            // 绘制高光效果
            painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
            painter.drawLine(cell.x * cellSize, drawY, (cell.x + 1) * cellSize, drawY);
            painter.drawLine(cell.x * cellSize, drawY, cell.x * cellSize, drawY + cellSize);

            // 绘制阴影效果
            painter.setPen(QPen(QColor(0, 0, 0, 100), 2));
            painter.drawLine((cell.x + 1) * cellSize, drawY, (cell.x + 1) * cellSize, drawY + cellSize);
            painter.drawLine(cell.x * cellSize, drawY + cellSize, (cell.x + 1) * cellSize, drawY + cellSize);
        }
    }
}

// 下一个方块的预览实现
NextBlockWidget::NextBlockWidget(QWidget* parent)
    : QWidget(parent)
    , m_cellSize(20)  // 预览窗口的单元格较小
{
    setFixedSize(120, 120);  // 固定大小，适合显示方块
    setStyleSheet("background-color: rgba(30, 30, 30, 200); border: 2px solid gray;");
}

void NextBlockWidget::setNextBlock(const Block& block)
{
    m_nextBlock = block;
    update();  // 触发重绘
}

void NextBlockWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor(30, 30, 30, 200));

    // 绘制边框
    painter.setPen(QPen(Qt::gray, 2));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    // 绘制标题
    painter.setPen(Qt::white);
    painter.drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "下一个:");

    // 如果没有下一个方块，不绘制
    if (m_nextBlock.getType() == Block::TYPE_COUNT) {
        return;
    }

    // 获取方块的单元格
    auto cells = m_nextBlock.getOccupiedCells();
    QColor blockColor = m_nextBlock.getColor();

    // 计算居中位置
    QRect blockBounds = m_nextBlock.getBoundingBox();
    int blockWidth = blockBounds.width() * m_cellSize;
    int blockHeight = blockBounds.height() * m_cellSize;

    int startX = (width() - blockWidth) / 2;
    int startY = (height() - blockHeight) / 2 + 15;  // 向下偏移为标题留空间

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 1));

    for (const auto& cell : cells) {
        // 计算在预览窗口中的位置（相对于方块边界）
        int drawX = startX + (cell.x - blockBounds.x()) * m_cellSize;
        int drawY = startY + (cell.y - blockBounds.y()) * m_cellSize;

        painter.drawRect(drawX, drawY, m_cellSize, m_cellSize);

        // 添加简单的3D效果
        painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
        painter.drawLine(drawX, drawY, drawX + m_cellSize, drawY);
        painter.drawLine(drawX, drawY, drawX, drawY + m_cellSize);

        painter.setPen(QPen(QColor(0, 0, 0, 100), 1));
        painter.drawLine(drawX + m_cellSize, drawY, drawX + m_cellSize, drawY + m_cellSize);
        painter.drawLine(drawX, drawY + m_cellSize, drawX + m_cellSize, drawY + m_cellSize);
    }
}

// HoldBlockWidget实现
HoldBlockWidget::HoldBlockWidget(QWidget* parent)
    : QWidget(parent)
    , m_cellSize(20)  // 与下一个方块预览相同的单元格大小
{
    setFixedSize(120, 120);  // 固定大小
    setStyleSheet("background-color: rgba(30, 30, 30, 200); border: 2px solid gray;");
}

void HoldBlockWidget::setHoldBlock(const Block& block)
{
    m_holdBlock = block;
    update();  // 触发重绘
}

void HoldBlockWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor(30, 30, 30, 200));

    // 绘制边框
    QColor borderColor = Qt::gray;
    if (m_holdBlock.getType() != Block::TYPE_COUNT && m_holdBlock.isValid()) {
        borderColor = QColor(255, 215, 0);  // 金色边框表示有暂存方块
    }
    painter.setPen(QPen(borderColor, 2));
    painter.drawRect(rect().adjusted(1, 1, -1, -1));

    // 绘制标题
    painter.setPen(Qt::white);
    painter.drawText(rect().adjusted(5, 5, -5, -5), Qt::AlignTop | Qt::AlignLeft, "暂存:");

    // 如果没有Hold方块或Hold方块是空的，显示提示
    if (m_holdBlock.getType() == Block::TYPE_COUNT || !m_holdBlock.isValid()) {
        painter.setPen(QColor(100, 100, 100));
        painter.drawText(rect().center(), "空");

        // 添加提示文字
        painter.setPen(QColor(150, 150, 150));
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);
        painter.drawText(rect().adjusted(5, height() - 20, -5, -5),
                         Qt::AlignBottom | Qt::AlignLeft, "按 C 键暂存");
        return;
    }

    // 获取方块的单元格
    auto cells = m_holdBlock.getOccupiedCells();
    QColor blockColor = m_holdBlock.getColor();

    // 计算居中位置
    QRect blockBounds = m_holdBlock.getBoundingBox();
    int blockWidth = blockBounds.width() * m_cellSize;
    int blockHeight = blockBounds.height() * m_cellSize;

    int startX = (width() - blockWidth) / 2;
    int startY = (height() - blockHeight) / 2 + 15;  // 向下偏移为标题留空间

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 1));

    for (const auto& cell : cells) {
        // 计算在预览窗口中的位置（相对于方块边界）
        int drawX = startX + (cell.x - blockBounds.x()) * m_cellSize;
        int drawY = startY + (cell.y - blockBounds.y()) * m_cellSize;

        painter.drawRect(drawX, drawY, m_cellSize, m_cellSize);

        // 添加简单的3D效果
        painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
        painter.drawLine(drawX, drawY, drawX + m_cellSize, drawY);
        painter.drawLine(drawX, drawY, drawX, drawY + m_cellSize);

        painter.setPen(QPen(QColor(0, 0, 0, 100), 1));
        painter.drawLine(drawX + m_cellSize, drawY, drawX + m_cellSize, drawY + m_cellSize);
        painter.drawLine(drawX, drawY + m_cellSize, drawX + m_cellSize, drawY + m_cellSize);
    }

    // 添加状态提示
    painter.setPen(QColor(200, 200, 200));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    painter.drawText(rect().adjusted(5, height() - 20, -5, -5),
                     Qt::AlignBottom | Qt::AlignLeft, "已暂存 - 按 C 键交换");
}

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

void MainWindow::setupGameConnections()
{
    if (!m_gameEngine) {
        qDebug() << "CRITICAL ERROR: Game engine is still null in setupGameConnections!";
        // 尝试重新初始化
        initializeSystems();

        if (!m_gameEngine) {
            qDebug() << "Failed to initialize game engine, aborting game connections";
            return;
        }
    }

    // 连接游戏引擎信号
    connect(m_gameEngine.data(), &GameEngine::gameStateChanged,
                        this, &MainWindow::onGameStateChanged);
    connect(m_gameEngine.data(), &GameEngine::gameStatsUpdated,
                        this, &MainWindow::onGameStatsUpdated);
    // 连接下一个方块变化信号
    connect(m_gameEngine.data(), &GameEngine::nextBlockChanged,
                        this, &MainWindow::onNextBlockChanged);
    // 连接Hold方块变化信号
    connect(m_gameEngine.data(), &GameEngine::holdBlockChanged,
                        this, &MainWindow::onHoldBlockChanged);
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

void MainWindow::setupConnections()
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

    for (QPushButton* button : buttons) {
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
    for (QPushButton* button : pauseButtons) {
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

    for (QPushButton* button : buttons) {
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
    for (QPushButton* button : pauseButtons) {
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

// 新增：处理Hold方块变化的槽函数
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
    // 使用静态变量确保只处理一次
    static bool handlingGameOver = false;
    if (handlingGameOver) {
        return;
    }

    handlingGameOver = true;

    if (!m_gameEngine) {
        qDebug() << "ERROR: Game engine is null in game over handler";
        handlingGameOver = false;
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

    // 重置处理标志
    QTimer::singleShot(100, [this]() {
        handlingGameOver = false;
    });
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

