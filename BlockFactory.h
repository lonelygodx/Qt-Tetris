#ifndef BLOCKFACTORY_H
#define BLOCKFACTORY_H
#include <QObject>
#include <QVector>
#include <QHash>
#include <random>
#include "Block.h"

class BlockFactory : public QObject
{
    Q_OBJECT

public:
    explicit BlockFactory(QObject* parent = nullptr);

    // 方块创建
    Block createRandomBlock();
    Block createBlock(Block::BlockType type);

    // 配置
    void setRandomizerType(const QString& type) { m_randomizerType = type; resetBag(); }

private:
    // 预定义方块
    void initializeStandardBlocks();
    Block::BlockShape createStandardShape(Block::BlockType type);

    // 随机化算法
    Block::BlockType getNextFrom7Bag();
    Block::BlockType getNextRandom();
    void resetBag();

    // 成员变量
    QHash<Block::BlockType, Block::BlockShape> m_standardBlocks;  // 标准方块集合
    QVector<Block::BlockType> m_availableTypes;                   // 合法方块类型集合

    // 随机化状态
    QString m_randomizerType;           // 生成方块算法
    QVector<Block::BlockType> m_bag;    // 7-bags模式方块队列

    // 随机数生成
    std::mt19937 m_randomEngine;
};

#endif // BLOCKFACTORY_H
