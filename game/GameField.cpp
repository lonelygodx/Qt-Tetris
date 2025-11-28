#include "GameField.h"
#include <algorithm>
#include <qdebug.h>

GameField::GameField(int width, int height)
    : m_width(width), m_height(height)
{
    initializeGrid();
}

void GameField::initializeGrid()
{
    m_grid.resize(m_height);
    for (int y = 0; y < m_height; ++y) {
        m_grid[y].resize(m_width);
        for (int x = 0; x < m_width; ++x) {
            m_grid[y][x] = Cell();
        }
    }
}

bool GameField::isCellEmpty(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return false; // 边界外视为非空
    }
    return !m_grid[y][x].occupied;
}

QColor GameField::getCellColor(int x, int y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
        return Qt::black;
    }
    return m_grid[y][x].color;
}

void GameField::setCell(int x, int y, const QColor& color)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_grid[y][x].occupied = true;
        m_grid[y][x].color = color;
    }
}

void GameField::clearCell(int x, int y)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_grid[y][x].occupied = false;
        m_grid[y][x].color = Qt::black;
    }
}

void GameField::clearField()
{
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            clearCell(x, y);
        }
    }
}

bool GameField::isLineComplete(int y) const
{
    if (y < 0 || y >= m_height) return false;

    for (int x = 0; x < m_width; ++x) {
        if (!m_grid[y][x].occupied) {
            return false;
        }
    }
    return true;
}

QVector<int> GameField::findCompleteLines() const
{
    QVector<int> completeLines;
    for (int y = 0; y < m_height; ++y) {
        if (isLineComplete(y)) {
            completeLines.append(y);
        }
    }
    return completeLines;
}

void GameField::removeLine(int y)
{
    if (y < 0 || y >= m_height) {
        qDebug() << "Invalid line to remove:" << y;
        return;
    }

    // 将y行以上的所有行下移一行
    for (int row = y; row > 0; --row) {
        for (int x = 0; x < m_width; ++x) {
            m_grid[row][x] = m_grid[row - 1][x];
        }
    }

    // 清空最顶行
    for (int x = 0; x < m_width; ++x) {
        m_grid[0][x] = Cell();
    }
}

void GameField::removeLines(const QVector<int>& lines)
{
    if (lines.isEmpty()) return;

    // 按从小到大排序，这样从上往下移除不会影响下面的行号
    QVector<int> sortedLines = lines;
    std::sort(sortedLines.begin(), sortedLines.end());

    // 移除重复
    auto last = std::unique(sortedLines.begin(), sortedLines.end());
    sortedLines.erase(last, sortedLines.end());

    // 一次性移除所有完整行
    for (int line : sortedLines) {
        removeLine(line);
    }
}

int GameField::removeAllCompleteLines()
{
    QVector<int> completeLines = findCompleteLines();

    if (!completeLines.isEmpty()) {
        removeLines(completeLines);
    }

    return completeLines.size();
}

void GameField::shiftLinesDown(int startY, int count)
{
    // 将startY行以上的所有行下移count行
    for (int row = m_height - 1; row >= startY + count; --row) {
        for (int x = 0; x < m_width; ++x) {
            m_grid[row][x] = m_grid[row - count][x];
        }
    }

    // 清空顶部的count行
    for (int row = startY; row < startY + count; ++row) {
        for (int x = 0; x < m_width; ++x) {
            clearCell(x, row);
        }
    }
}

void GameField::debugPrintField() const
{
    qDebug() << "=== Game Field State ===";
    for (int y = 0; y < m_height; ++y) {
        QString line;
        for (int x = 0; x < m_width; ++x) {
            line.append(m_grid[y][x].occupied ? "X" : ".");
        }
        qDebug() << "Line" << y << ":" << line << (isLineComplete(y) ? " [COMPLETE]" : "");
    }
    qDebug() << "========================";
}
