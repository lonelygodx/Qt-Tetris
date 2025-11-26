#ifndef BLOCK_H
#define BLOCK_H
#include <QVector>
#include <QColor>
#include <QRect>
#include "Position.h"

class Block
{
public:
    //方块类型
    enum BlockType {
        TYPE_I, TYPE_O, TYPE_T, TYPE_S, TYPE_Z, TYPE_J, TYPE_L,
        TYPE_COUNT
    };
    //旋转角度
    enum RotationState {
        ROT_0,    // 0度
        ROT_90,   // 90度
        ROT_180,  // 180度
        ROT_270,  // 270度
        ROT_COUNT
    };
    //方块结构体
    struct BlockShape {
        QString name; // 方块名
        QVector<QVector<bool>> pattern; // 4x4 矩阵
        QColor color; // 方块颜色
        //生成时坐标
        int spawnOffsetX;
        int spawnOffsetY;
        //默认为 0,0
        BlockShape() : spawnOffsetX(0), spawnOffsetY(0) {}
    };

    Block();
    explicit Block(BlockType type, const BlockShape& shape);

    // 属性获取
    BlockType getType() const { return m_type; }
    const BlockShape& getShape() const { return m_shape; }
    Position getPosition() const { return m_position; }
    RotationState getRotation() const { return m_rotation; }
    QColor getColor() const { return m_shape.color; }
    QString getName() const { return m_shape.name; }

    // 验证方块类型是否有效
    bool isValid() const { return m_type >= 0 && m_type < TYPE_COUNT; }

    // 变换操作
    void setPosition(const Position& pos) { m_position = pos; }
    void setPosition(int x, int y) { m_position = Position{ x, y }; }
    void move(int dx, int dy);
    void rotateClockwise(); // 顺时针旋转
    void rotateCounterClockwise();  // 逆时针
    void resetRotation();

    // 几何信息
    QVector<Position> getOccupiedCells() const;  // 获取各个方块的当前位置坐标
    QRect getBoundingBox() const; // 获取图形边界坐标所在的矩形

private:
    BlockType m_type;           // 类型
    BlockShape m_shape;         // 方块
    Position m_position;        // 位置
    RotationState m_rotation;   // 角度

    QVector<QVector<bool>> getRotatedPattern() const;  // 方块的几何位置
};

#endif // BLOCK_H
