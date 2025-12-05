#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H
#include <QWidget>
#include "GameEngine.h"

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
    void drawGameField(QPainter& painter);   // 绘制游戏场地
    void drawGhostBlock(QPainter& painter);  // 绘制幽灵方块
    void drawCurrentBlock(QPainter& painter);// 绘制当前方块
};

// 下一个方块预览界面
class NextBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NextBlockWidget(QWidget* parent = nullptr);
    void setNextBlock(const Block& block);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Block m_nextBlock;
};

// 暂存方块预览界面
class HoldBlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HoldBlockWidget(QWidget* parent = nullptr);
    void setHoldBlock(const Block& block);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Block m_holdBlock;
};

#endif // GAMEWIDGET_H
