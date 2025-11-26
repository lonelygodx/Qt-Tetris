#include "BlockFactory.h"
#include <qdebug.h>

BlockFactory::BlockFactory(QObject* parent)
    : QObject(parent)
    , m_randomizerType("7-bag")
    , m_randomEngine(std::random_device{}())
{
    initializeStandardBlocks();
    resetBag();
}

void BlockFactory::initializeStandardBlocks()
{
    m_standardBlocks.clear();

    // 定义所有标准方块
    m_standardBlocks[Block::TYPE_I] = createStandardShape(Block::TYPE_I);
    m_standardBlocks[Block::TYPE_O] = createStandardShape(Block::TYPE_O);
    m_standardBlocks[Block::TYPE_T] = createStandardShape(Block::TYPE_T);
    m_standardBlocks[Block::TYPE_S] = createStandardShape(Block::TYPE_S);
    m_standardBlocks[Block::TYPE_Z] = createStandardShape(Block::TYPE_Z);
    m_standardBlocks[Block::TYPE_J] = createStandardShape(Block::TYPE_J);
    m_standardBlocks[Block::TYPE_L] = createStandardShape(Block::TYPE_L);

    // 设置可用方块类型
    m_availableTypes = {
        Block::TYPE_I, Block::TYPE_O, Block::TYPE_T,
        Block::TYPE_S, Block::TYPE_Z, Block::TYPE_J, Block::TYPE_L
    };
}

Block::BlockShape BlockFactory::createStandardShape(Block::BlockType type)
{
    // 定义好每个方块的形状和其他属性
    Block::BlockShape shape;
    switch (type) {
    case Block::TYPE_I:
        shape.name = "I";
        shape.pattern = {
            {false, false, false, false},
            {true,  true,  true,  true},
            {false, false, false, false},
            {false, false, false, false}
        };
        shape.color = QColor(0, 255, 255); // 青色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_O:
        shape.name = "O";
        shape.pattern = {
            {true, true},
            {true, true}
        };
        shape.color = QColor(255, 255, 0); // 黄色
        shape.spawnOffsetX = 4;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_T:
        shape.name = "T";
        shape.pattern = {
            {false, true, false},
            {true,  true, true},
            {false, false, false}
        };
        shape.color = QColor(128, 0, 128); // 紫色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_S:
        shape.name = "S";
        shape.pattern = {
            {false, true, true},
            {true,  true, false},
            {false, false, false}
        };
        shape.color = QColor(0, 255, 0); // 绿色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_Z:
        shape.name = "Z";
        shape.pattern = {
            {true,  true, false},
            {false, true, true},
            {false, false, false}
        };
        shape.color = QColor(255, 0, 0); // 红色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_J:
        shape.name = "J";
        shape.pattern = {
            {true,  false, false},
            {true,  true,  true},
            {false, false, false}
        };
        shape.color = QColor(0, 0, 255); // 蓝色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    case Block::TYPE_L:
        shape.name = "L";
        shape.pattern = {
            {false, false, true},
            {true,  true,  true},
            {false, false, false}
        };
        shape.color = QColor(255, 165, 0); // 橙色
        shape.spawnOffsetX = 3;
        shape.spawnOffsetY = 0;
        break;

    default:
        break;
    }

    return shape;
}

Block BlockFactory::createRandomBlock()
{
    Block::BlockType type;

    if (m_randomizerType == "7-bag") {
        type = getNextFrom7Bag();
    }
    else {
        type = getNextRandom();
    }

    // 验证生成的类型是否有效
    if (type < 0 || type >= Block::TYPE_COUNT) {
        qDebug() << "ERROR: Generated invalid block type:" << type << ", falling back to TYPE_I";
        type = Block::TYPE_I; // 回退到I方块
    }

    Block block = createBlock(type);

    // 验证创建的方块是否有效
    if (!block.isValid()) {
        qDebug() << "ERROR: Created invalid block, recreating...";
        block = createBlock(Block::TYPE_I); // 回退到I方块
    }

    return block;
}

Block BlockFactory::createBlock(Block::BlockType type)
{
    // 验证请求的类型是否有效
    if (type < 0 || type >= Block::TYPE_COUNT) {
        qDebug() << "ERROR: Requested invalid block type:" << type;
        return Block(); // 返回空方块
    }

    if (m_standardBlocks.contains(type)) {
        Block block(type, m_standardBlocks[type]);
        return block;
    }

    qDebug() << "WARNING: Block type not found in standard blocks:" << type;
    return Block();
}

Block::BlockType BlockFactory::getNextFrom7Bag()
{
    if (m_bag.isEmpty()) {
        resetBag();
    }

    Block::BlockType type = m_bag.takeFirst();
    return type;
}

Block::BlockType BlockFactory::getNextRandom()
{
    std::uniform_int_distribution<int> dist(0, m_availableTypes.size() - 1);
    int index = dist(m_randomEngine);
    return m_availableTypes[index];
}

void BlockFactory::resetBag()
{
    m_bag = m_availableTypes;
    // 随机打乱袋子
    std::shuffle(m_bag.begin(), m_bag.end(), m_randomEngine);
}
