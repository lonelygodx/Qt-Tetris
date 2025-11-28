#include "GameEngine.h"
#include <QDebug>
#include <QDateTime>

GameEngine::GameEngine(QObject* parent)
    : QObject(parent)
    , m_gameState(STATE_STOPPED)
    , m_canHold(true)
    , m_fallProgress(0.0f)
    , m_fastDrop(false)
    , m_fallSpeed(1000)
    , m_fastFallSpeed(50)
    , m_lastUpdateTime(0)
{
    // 创建方块工厂
    m_blockFactory.reset(new BlockFactory(this));

    // 创建游戏计时器
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameEngine::updateGame);
    m_gameTimer->setInterval(16); // 约60FPS
}

GameEngine::~GameEngine()
{
    if (m_gameTimer) {
        m_gameTimer->stop();
    }
}

bool GameEngine::initialize(int width, int height)
{
    m_gameField = GameField(width, height);
    resetGameStats();

    // 初始化Hold方块为一个有效的空方块
    m_holdBlock = Block(); // 这会创建一个TYPE_COUNT的方块，表示"无Hold方块"

    // 初始化预览方块
    m_nextBlock = m_blockFactory->createRandomBlock();
    emit nextBlockChanged();

    return true;
}

void GameEngine::startGame()
{
    if (m_gameState == STATE_RUNNING) {
        qDebug() << "Game is already running, ignoring start request";
        return;
    }

    m_gameState = STATE_RUNNING;

    resetGameStats();
    m_gameField.clearField();
    m_canHold = true;
    m_fallProgress = 0.0f;
    m_fallSpeed = 1000;
    m_fastDrop = false;
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();

    // 清除holdblock
    m_holdBlock = Block();

    // 生成第一个方块
    spawnNewBlock();

    // 启动游戏计时器
    if (m_gameTimer) {
        m_gameTimer->start();
    } else {
        qDebug() << "ERROR: Game timer is null!";
    }

    emit holdBlockChanged();   // 更新holdblock状态
    emit gameStateChanged(m_gameState);
    emit gameFieldChanged();
}

void GameEngine::pauseGame()
{
    if (m_gameState != STATE_RUNNING) return;

    m_gameState = STATE_PAUSED;
    m_gameTimer->stop();

    emit gameStateChanged(m_gameState);
}

void GameEngine::resumeGame()
{
    if (m_gameState != STATE_PAUSED) return;

    m_gameState = STATE_RUNNING;
    m_lastUpdateTime = QDateTime::currentMSecsSinceEpoch();
    m_gameTimer->start();

    emit gameStateChanged(m_gameState);
}

void GameEngine::restartGame()
{
    endGame();
    startGame();
}

void GameEngine::endGame()
{
    if (m_gameState == STATE_STOPPED) return;

    m_gameState = STATE_STOPPED;
    m_gameTimer->stop();

    emit gameStateChanged(m_gameState);
}

void GameEngine::updateGame()
{
    if (m_gameState != STATE_RUNNING) return;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 deltaTime = currentTime - m_lastUpdateTime;
    m_lastUpdateTime = currentTime;

    // 更新游戏时间
    if (m_gameStats.startTime.isNull()) {
        m_gameStats.startTime = QDateTime::currentDateTime();
    } else {
        m_gameStats.gameDuration = m_gameStats.startTime.secsTo(QDateTime::currentDateTime());
    }

    // 更新下落进度
    int currentFallSpeed = m_fastDrop ? m_fastFallSpeed : m_fallSpeed;
    float progressIncrement = static_cast<float>(deltaTime) / currentFallSpeed;

    // 更新进度
    m_fallProgress += progressIncrement;

    // 如果进度达到1，达到下落时机
    if (m_fallProgress >= 1.0f) {
        // 检查是否可以继续下落
        if (isValidPosition(m_currentBlock, 0, 1)) {
            // 执行实际下落
            m_currentBlock.move(0, 1);
            m_fallProgress = 0.0f;
            emit currentBlockChanged();
        } else {
            // 不能下落，立即锁定方块
            m_fallProgress = 0.0f; // 重置进度
            lockCurrentBlock();
        }
    }
}

bool GameEngine::moveLeft()
{
    if (m_gameState != STATE_RUNNING) return false;

    if (isValidPosition(m_currentBlock, -1, 0)) {
        m_currentBlock.move(-1, 0);
        emit currentBlockChanged(); // 这会触发重绘，包括幽灵方块
        return true;
    }
    return false;
}

bool GameEngine::moveRight()
{
    if (m_gameState != STATE_RUNNING) return false;

    if (isValidPosition(m_currentBlock, 1, 0)) {
        m_currentBlock.move(1, 0);
        emit currentBlockChanged(); // 这会触发重绘，包括幽灵方块
        return true;
    }
    return false;
}

bool GameEngine::rotateClockwise()
{
    if (m_gameState != STATE_RUNNING) return false;

    Block testBlock = m_currentBlock;
    testBlock.rotateClockwise();

    if (isValidPosition(testBlock)) {
        m_currentBlock.rotateClockwise();
        emit currentBlockChanged(); // 这会触发重绘，包括幽灵方块
        return true;
    }
    return false;
}

bool GameEngine::rotateCounterClockwise()
{
    if (m_gameState != STATE_RUNNING) return false;

    Block testBlock = m_currentBlock;
    testBlock.rotateCounterClockwise();

    if (isValidPosition(testBlock)) {
        m_currentBlock.rotateCounterClockwise();
        emit currentBlockChanged(); // 这会触发重绘，包括幽灵方块
        return true;
    }
    return false;
}

void GameEngine::softDrop()
{
    if (m_gameState != STATE_RUNNING) return;

    m_fastDrop = true;
}

void GameEngine::stopSoftDrop()
{
    if (m_gameState != STATE_RUNNING) return;

    m_fastDrop = false;
}

void GameEngine::hardDrop()
{
    if (m_gameState != STATE_RUNNING) return;

    // 计算可以下落的最大距离
    int dropDistance = 0;
    while (isValidPosition(m_currentBlock, 0, dropDistance + 1)) {
        dropDistance++;
    }

    if (dropDistance > 0) {
        m_currentBlock.move(0, dropDistance);
        m_gameStats.score += dropDistance * 2;
        m_fallProgress = 0.0f;
        emit currentBlockChanged();
        emit gameStatsUpdated(m_gameStats);
    }

    // 立即锁定，不等待下一次更新
    lockCurrentBlock();
}

void GameEngine::holdBlock()
{
    // 验证游戏状态
    if (m_gameState != STATE_RUNNING) {
        qDebug() << "Cannot hold: game not running";
        return;
    }

    // 验证hold状态
    if (!m_canHold) {
        qDebug() << "Cannot hold: already used hold in this turn";
        return;
    }

    // 验证当前方块类型
    if (!m_currentBlock.isValid()) {
        qDebug() << "ERROR: Current block has invalid type!" << m_currentBlock.getType();
        return;
    }

    // 验证Hold方块类型（如果是第一次使用，它应该是TYPE_COUNT，这是允许的）
    if (m_holdBlock.getType() != Block::TYPE_COUNT && !m_holdBlock.isValid()) {
        qDebug() << "ERROR: Hold block has invalid type!" << m_holdBlock.getType();
        return;
    }

    // 检查Hold方块是否为空（TYPE_COUNT表示空）
    if (m_holdBlock.getType() == Block::TYPE_COUNT) {
        // 第一次使用Hold - 存储当前方块并生成新方块

        m_holdBlock = m_currentBlock;
        m_holdBlock.resetRotation();

        // 生成新方块
        spawnNewBlock();
    } else {
        // 交换当前方块和Hold方块

        // 保存当前方块到临时变量
        Block temp = m_currentBlock;

        // 将Hold方块设置为当前方块
        m_currentBlock = m_holdBlock;
        m_currentBlock.resetRotation();

        // 将临时方块设置为Hold方块
        m_holdBlock = temp;
        m_holdBlock.resetRotation();

        // 设置当前方块的位置（从顶部重新开始下落）
        int spawnX = m_gameField.getWidth() / 2 - 2;
        m_currentBlock.setPosition(spawnX, 0);

        // 验证交换后的方块位置是否有效
        if (!isValidPosition(m_currentBlock)) {
            qDebug() << "WARNING: Swapped block is in invalid position!";
            qDebug() << "Block position: (" << m_currentBlock.getPosition().x
                     << ", " << m_currentBlock.getPosition().y << ")";

            // 尝试调整位置
            bool foundValidPosition = false;
            for (int offset = -2; offset <= 2; offset++) {
                m_currentBlock.setPosition(spawnX + offset, 0);
                if (isValidPosition(m_currentBlock)) {
                    qDebug() << "Adjusted position to: (" << (spawnX + offset) << ", 0)";
                    foundValidPosition = true;
                    break;
                }
            }

            // 如果还是无效，结束游戏
            if (!foundValidPosition) {
                qDebug() << "CRITICAL: Cannot find valid position for swapped block!";
                m_gameState = STATE_GAME_OVER;
                emit gameStateChanged(m_gameState);
                return;
            }
        }
    }

    m_canHold = false;
    m_fallProgress = 0.0f;

    emit currentBlockChanged();
    emit holdBlockChanged();
}

void GameEngine::spawnNewBlock()
{
    m_currentBlock = m_nextBlock;
    m_nextBlock = m_blockFactory->createRandomBlock();

    // 设置初始位置（场地中央顶部）
    int spawnX = m_gameField.getWidth() / 2 - 2;
    m_currentBlock.setPosition(spawnX, 0);

    // 检查游戏结束条件
    bool canSpawn = true;
    QVector<Position> cells = m_currentBlock.getOccupiedCells();

    for (const Position& cell : std::as_const(cells)) {
        // 检查新方块是否会与已有方块重叠
        if (cell.y >= 0 && cell.y < m_gameField.getHeight() &&
            cell.x >= 0 && cell.x < m_gameField.getWidth()) {
            if (!m_gameField.isCellEmpty(cell.x, cell.y)) {
                canSpawn = false;
                break;
            }
        }
    }

    if (!canSpawn) {
        // 游戏结束
        m_gameState = STATE_GAME_OVER;
        emit gameStateChanged(m_gameState);
        return;
    }

    m_canHold = true;
    m_fallProgress = 0.0f;
    m_fastDrop = false;

    m_gameStats.totalPieces++;

    emit currentBlockChanged();
    emit nextBlockChanged();  // 确保发出下一个方块变化信号
    emit gameStatsUpdated(m_gameStats);
}

void GameEngine::placeCurrentBlock()
{
    QVector<Position> cells = m_currentBlock.getOccupiedCells();
    QColor blockColor = m_currentBlock.getColor();

    for (const Position& cell : std::as_const(cells)) {
        if (cell.x >= 0 && cell.x < m_gameField.getWidth() &&
            cell.y >= 0 && cell.y < m_gameField.getHeight()) {
            m_gameField.setCell(cell.x, cell.y, blockColor);
        }
    }
}

void GameEngine::lockCurrentBlock()
{
    if (m_gameState != STATE_RUNNING) return;

    // 将当前方块放置到场地上
    placeCurrentBlock();

    // 清除完整的行
    clearCompletedLines();

    // 生成新方块
    spawnNewBlock();

    // 重置下落状态
    m_fallProgress = 0.0f;
    m_fastDrop = false;
}

int GameEngine::clearCompletedLines()
{
    // m_gameField.debugPrintField(); // 打印消除前的场地状态

    QVector<int> completeLines = m_gameField.findCompleteLines();

    int linesCleared = completeLines.size();
    if (linesCleared > 0) {
        m_gameField.removeLines(completeLines);
        // m_gameField.debugPrintField(); // 打印消除后的场地状态

        updateGameStats(linesCleared);
        emit gameFieldChanged();
    }

    return linesCleared;
}

void GameEngine::updateGameStats(int linesCleared)
{
    m_gameStats.linesCleared += linesCleared;
    calculateScore(linesCleared);
    updateLevel();

    emit gameStatsUpdated(m_gameStats);
}

void GameEngine::calculateScore(int linesCleared)
{
    int baseScore = 0;

    switch (linesCleared) {
    case 1: baseScore = 100; break;
    case 2: baseScore = 300; break;
    case 3: baseScore = 500; break;
    case 4: baseScore = 800; break;
    }

    baseScore *= m_gameStats.level;
    m_gameStats.score += baseScore;
}

void GameEngine::updateLevel()
{
    int newLevel = m_gameStats.linesCleared / 10 + 1;
    if (newLevel > m_gameStats.level) {
        m_gameStats.level = newLevel;

        // 提高下落速度
        m_fallSpeed = qMax(100, 1000 - (m_gameStats.level - 1) * 100);

        emit updateNewLevel(newLevel);
    }
}

bool GameEngine::isValidPosition(const Block& block, int dx, int dy) const
{
    QVector<Position> cells = block.getOccupiedCells();

    for (const Position& cell : std::as_const(cells)) {
        int testX = cell.x + dx;
        int testY = cell.y + dy;

        // 检查左右边界
        if (testX < 0 || testX >= m_gameField.getWidth()) {
            return false;
        }

        // 检查底部边界
        if (testY >= m_gameField.getHeight()) {
            return false;
        }

        // 检查与其他方块的碰撞（只检查场地内的位置）
        if (testY >= 0 && !m_gameField.isCellEmpty(testX, testY)) {
            return false;
        }
    }

    return true;
}

void GameEngine::resetGameStats()
{
    m_gameStats.reset();
    emit gameStatsUpdated(m_gameStats);
}

// 添加幽灵方块计算方法
Position GameEngine::calculateGhostPosition() const
{
    if (m_gameState != STATE_RUNNING) {
        return m_currentBlock.getPosition();
    }

    // 计算方块可以下落的最大距离
    int dropDistance = 0;
    while (isValidPosition(m_currentBlock, 0, dropDistance + 1)) {
        dropDistance++;
    }

    // 返回幽灵方块的位置
    Position ghostPos = m_currentBlock.getPosition();
    ghostPos.y += dropDistance;

    // 确保不会超出底部
    if (ghostPos.y >= m_gameField.getHeight()) {
        ghostPos.y = m_gameField.getHeight() - 1;
    }

    return ghostPos;
}

Block GameEngine::getGhostBlock() const
{
    if (m_gameState != STATE_RUNNING) {
        return Block(); // 返回空方块
    }

    // 创建幽灵方块（当前方块的副本，但位置在预测位置）
    Block ghostBlock = m_currentBlock;
    ghostBlock.setPosition(calculateGhostPosition());
    return ghostBlock;
}
