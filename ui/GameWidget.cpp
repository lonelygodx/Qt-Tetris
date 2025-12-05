#include <QPainter>
#include "GameWidget.h"
#include "GameConfig.h"
#include "GameConfig.h"

// GameWidget 实现
GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent)
    , m_engine(nullptr)
{
    setFixedSize(GAMEWIDGET_FIXED_SIZEW, GAMEWIDGET_FIXED_SIZEH);
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

    // 绘制网格
    painter.setPen(QPen(QColor(40, 40, 40), 1));
    for (int x = 0; x <= field.getWidth(); ++x) {
        painter.drawLine(x * FIELD_CELL_SIZE, 0, x * FIELD_CELL_SIZE, field.getHeight() * FIELD_CELL_SIZE);
    }
    for (int y = 0; y <= field.getHeight(); ++y) {
        painter.drawLine(0, y * FIELD_CELL_SIZE, field.getWidth() * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE);
    }

    // 绘制已放置的方块
    drawGameField(painter);

    // 绘制幽灵方块
    if (m_engine->getGameState() == GameEngine::STATE_RUNNING && GHOST_BLOCK_ENABLED) {
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

    // 绘制已放置的方块
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            if (!field.isCellEmpty(x, y)) {
                QColor color = field.getCellColor(x, y);

                // 绘制方块主体
                painter.fillRect(x * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE, FIELD_CELL_SIZE, FIELD_CELL_SIZE, color);

                // 绘制高光效果
                painter.setPen(QPen(QColor(255, 255, 255, 150), 2));
                painter.drawLine(x * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE, (x + 1) * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE);
                painter.drawLine(x * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE, x * FIELD_CELL_SIZE, (y + 1) * FIELD_CELL_SIZE);

                // 绘制阴影效果
                painter.setPen(QPen(QColor(0, 0, 0, 100), 2));
                painter.drawLine((x + 1) * FIELD_CELL_SIZE, y * FIELD_CELL_SIZE, (x + 1) * FIELD_CELL_SIZE, (y + 1) * FIELD_CELL_SIZE);
                painter.drawLine(x * FIELD_CELL_SIZE, (y + 1) * FIELD_CELL_SIZE, (x + 1) * FIELD_CELL_SIZE, (y + 1) * FIELD_CELL_SIZE);

                // 绘制内部细节
                painter.setPen(QPen(QColor(255, 255, 255, 50), 1));
                painter.drawRect(x * FIELD_CELL_SIZE + 2, y * FIELD_CELL_SIZE + 2, FIELD_CELL_SIZE - 4, FIELD_CELL_SIZE - 4);
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

    // 绘制幽灵方块
    for (const auto& cell : std::as_const(cells)) {
        if (cell.y >= 0) {
            // 绘制虚影
            painter.fillRect(cell.x * FIELD_CELL_SIZE, cell.y * FIELD_CELL_SIZE, FIELD_CELL_SIZE, FIELD_CELL_SIZE, ghostColor);
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
    //             currentCenter.x * FIELD_CELL_SIZE + FIELD_CELL_SIZE / 2,
    //             currentCenter.y * FIELD_CELL_SIZE + FIELD_CELL_SIZE / 2,
    //             ghostCenter.x * FIELD_CELL_SIZE + FIELD_CELL_SIZE / 2,
    //             ghostCenter.y * FIELD_CELL_SIZE + FIELD_CELL_SIZE / 2
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

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 2));

    for (const auto& cell : std::as_const(cells)) {
        // 计算实际绘制位置（包括下落进度）
        float actualY = cell.y + static_cast<int>(trunc(fallProgress));  // 对下落进度取整，防止出现在某一格内的情况
        int drawY = actualY * FIELD_CELL_SIZE;

        // 只绘制场地内的部分
        if (actualY >= 0 && actualY < m_engine->getGameField().getHeight()) {
            painter.drawRect(cell.x * FIELD_CELL_SIZE, drawY, FIELD_CELL_SIZE, FIELD_CELL_SIZE);

            // 绘制高光效果
            painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
            painter.drawLine(cell.x * FIELD_CELL_SIZE, drawY, (cell.x + 1) * FIELD_CELL_SIZE, drawY);
            painter.drawLine(cell.x * FIELD_CELL_SIZE, drawY, cell.x * FIELD_CELL_SIZE, drawY + FIELD_CELL_SIZE);

            // 绘制阴影效果
            painter.setPen(QPen(QColor(0, 0, 0, 100), 2));
            painter.drawLine((cell.x + 1) * FIELD_CELL_SIZE, drawY, (cell.x + 1) * FIELD_CELL_SIZE, drawY + FIELD_CELL_SIZE);
            painter.drawLine(cell.x * FIELD_CELL_SIZE, drawY + FIELD_CELL_SIZE, (cell.x + 1) * FIELD_CELL_SIZE, drawY + FIELD_CELL_SIZE);
        }
    }
}

// 下一个方块的预览实现
NextBlockWidget::NextBlockWidget(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(BLOCKWIDGET_FIXED_SIZEW, BLOCKWIDGET_FIXED_SIZEH);  // 固定大小，适合显示方块
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
    int blockWidth = blockBounds.width() * WIDGET_CELL_SIZE;
    int blockHeight = blockBounds.height() * WIDGET_CELL_SIZE;

    int startX = (width() - blockWidth) / 2;
    int startY = (height() - blockHeight) / 2 + 15;  // 向下偏移为标题留空间

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 1));

    for (const auto& cell : std::as_const(cells)) {
        // 计算在预览窗口中的位置（相对于方块边界）
        int drawX = startX + (cell.x - blockBounds.x()) * WIDGET_CELL_SIZE;
        int drawY = startY + (cell.y - blockBounds.y()) * WIDGET_CELL_SIZE;

        painter.drawRect(drawX, drawY, WIDGET_CELL_SIZE, WIDGET_CELL_SIZE);

        // 添加简单的3D效果
        painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
        painter.drawLine(drawX, drawY, drawX + WIDGET_CELL_SIZE, drawY);
        painter.drawLine(drawX, drawY, drawX, drawY + WIDGET_CELL_SIZE);

        painter.setPen(QPen(QColor(0, 0, 0, 100), 1));
        painter.drawLine(drawX + WIDGET_CELL_SIZE, drawY, drawX + WIDGET_CELL_SIZE, drawY + WIDGET_CELL_SIZE);
        painter.drawLine(drawX, drawY + WIDGET_CELL_SIZE, drawX + WIDGET_CELL_SIZE, drawY + WIDGET_CELL_SIZE);
    }
}

// HoldBlockWidget实现
HoldBlockWidget::HoldBlockWidget(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(BLOCKWIDGET_FIXED_SIZEW, BLOCKWIDGET_FIXED_SIZEH);  // 固定大小
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
    int blockWidth = blockBounds.width() * WIDGET_CELL_SIZE;
    int blockHeight = blockBounds.height() * WIDGET_CELL_SIZE;

    int startX = (width() - blockWidth) / 2;
    int startY = (height() - blockHeight) / 2 + 15;  // 向下偏移为标题留空间

    painter.setBrush(blockColor);
    painter.setPen(QPen(Qt::white, 1));

    for (const auto& cell : std::as_const(cells)) {
        // 计算在预览窗口中的位置（相对于方块边界）
        int drawX = startX + (cell.x - blockBounds.x()) * WIDGET_CELL_SIZE;
        int drawY = startY + (cell.y - blockBounds.y()) * WIDGET_CELL_SIZE;

        painter.drawRect(drawX, drawY, WIDGET_CELL_SIZE, WIDGET_CELL_SIZE);

        // 添加简单的3D效果
        painter.setPen(QPen(QColor(255, 255, 255, 150), 1));
        painter.drawLine(drawX, drawY, drawX + WIDGET_CELL_SIZE, drawY);
        painter.drawLine(drawX, drawY, drawX, drawY + WIDGET_CELL_SIZE);

        painter.setPen(QPen(QColor(0, 0, 0, 100), 1));
        painter.drawLine(drawX + WIDGET_CELL_SIZE, drawY, drawX + WIDGET_CELL_SIZE, drawY + WIDGET_CELL_SIZE);
        painter.drawLine(drawX, drawY + WIDGET_CELL_SIZE, drawX + WIDGET_CELL_SIZE, drawY + WIDGET_CELL_SIZE);
    }

    // 添加状态提示
    painter.setPen(QColor(200, 200, 200));
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    painter.drawText(rect().adjusted(5, height() - 20, -5, -5),
                     Qt::AlignBottom | Qt::AlignLeft, "已暂存 - 按 C 键交换");
}
