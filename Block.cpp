#include "Block.h"
#include <algorithm>
#include <qdebug.h>

Block::Block()
    : m_type(TYPE_COUNT), m_position(0, 0), m_rotation(ROT_0)
{
    // 创建一个明确的空方块形状
    m_shape.name = "Empty";
    m_shape.pattern = QVector<QVector<bool>>(4, QVector<bool>(4, false));
    m_shape.color = Qt::black;
    m_shape.spawnOffsetX = 0;
    m_shape.spawnOffsetY = 0;
}

Block::Block(BlockType type, const BlockShape& shape)
    : m_type(type), m_shape(shape), m_position(0, 0), m_rotation(ROT_0)
{
}

void Block::move(int dx, int dy)
{
    m_position.x += dx;
    m_position.y += dy;
}

void Block::rotateClockwise()
{
    m_rotation = static_cast<RotationState>((m_rotation + 1) % ROT_COUNT);
}

void Block::rotateCounterClockwise()
{
    m_rotation = static_cast<RotationState>((m_rotation + ROT_COUNT - 1) % ROT_COUNT);
}

void Block::resetRotation()
{
    m_rotation = ROT_0;
}

QVector<Position> Block::getOccupiedCells() const
{
    QVector<Position> cells;
    QVector<QVector<bool>> pattern = getRotatedPattern();

    for (int y = 0; y < pattern.size(); ++y) {
        for (int x = 0; x < pattern[y].size(); ++x) {
            if (pattern[y][x]) {
                cells.append(Position(m_position.x + x, m_position.y + y));
            }
        }
    }

    return cells;
}

QRect Block::getBoundingBox() const
{
    QVector<Position> cells = getOccupiedCells();
    if (cells.isEmpty()) {
        return QRect();
    }

    int minX = cells[0].x;
    int maxX = cells[0].x;
    int minY = cells[0].y;
    int maxY = cells[0].y;

    for (const Position& cell : cells) {
        minX = std::min(minX, cell.x);
        maxX = std::max(maxX, cell.x);
        minY = std::min(minY, cell.y);
        maxY = std::max(maxY, cell.y);
    }

    return QRect(minX, minY, maxX - minX + 1, maxY - minY + 1);
}

QVector<QVector<bool>> Block::getRotatedPattern() const
{
    QVector<QVector<bool>> rotated = m_shape.pattern;
    int size = rotated.size();

    // 根据旋转状态进行矩阵旋转
    for (int i = 0; i < m_rotation; ++i) {
        QVector<QVector<bool>> temp(size, QVector<bool>(size, false));
        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                temp[x][size - 1 - y] = rotated[y][x];
            }
        }
        rotated = temp;
    }

    return rotated;
}
