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
    void setPreviewCount(int count) { m_previewCount = count; }
    int getPreviewCount() const { return m_previewCount; }

private:
    // 预定义方块
    void initializeStandardBlocks();
    Block::BlockShape createStandardShape(Block::BlockType type);

    // 随机化算法
    Block::BlockType getNextFrom7Bag();
    Block::BlockType getNextRandom();
    void resetBag();

    // 成员变量
    QHash<Block::BlockType, Block::BlockShape> m_standardBlocks;
    QVector<Block::BlockType> m_availableTypes;

    // 随机化状态
    QString m_randomizerType;
    QVector<Block::BlockType> m_bag;
    QVector<Block::BlockType> m_history;
    int m_previewCount;

    // 随机数生成
    std::mt19937 m_randomEngine;
};

#endif // BLOCKFACTORY_H
