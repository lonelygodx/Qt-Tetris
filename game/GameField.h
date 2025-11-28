#ifndef GAMEFIELD_H
#define GAMEFIELD_H
#include <QVector>
#include <QColor>
#include <QRect>

class GameField
{
public:
    struct Cell {
        bool occupied;
        QColor color;

        Cell() : occupied(false), color(Qt::black) {}
    };

    explicit GameField(int width = 10, int height = 20);

    // 基本操作
    bool isCellEmpty(int x, int y) const;
    QColor getCellColor(int x, int y) const;
    void setCell(int x, int y, const QColor& color);
    void clearCell(int x, int y);
    void clearField();

    // 场地信息
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    QRect getBounds() const { return QRect(0, 0, m_width, m_height); }

    // 行操作
    bool isLineComplete(int y) const;
    QVector<int> findCompleteLines() const;
    void removeLine(int y);
    void removeLines(const QVector<int>& lines);
    int removeAllCompleteLines();

    // 调试函数
    void debugPrintField() const;

private:
    int m_width;
    int m_height;
    QVector<QVector<Cell>> m_grid;

    void initializeGrid();
    void shiftLinesDown(int startY, int count);
};

#endif // GAMEFIELD_H
