#include "InputHandler.h"

InputHandler::InputHandler(QObject* parent)
    : QObject(parent)
    , m_currentRepeatingAction(ACTION_COUNT)
    , m_isRepeating(false)
    , add_autoRepeatDelay(200)
{
    m_autoRepeatTimer = new QTimer(this);
    connect(m_autoRepeatTimer, &QTimer::timeout, this, &InputHandler::onAutoRepeat);

    initializeDefaultMapping();
}

void InputHandler::initializeDefaultMapping()
{
    m_config.keyMapping.clear();

    // 默认按键映射，wasd也可以控制上下左右
    m_config.keyMapping[Qt::Key_Left] = ACTION_MOVE_LEFT;
    m_config.keyMapping[Qt::Key_A] = ACTION_MOVE_LEFT;
    m_config.keyMapping[Qt::Key_Right] = ACTION_MOVE_RIGHT;
    m_config.keyMapping[Qt::Key_D] = ACTION_MOVE_RIGHT;
    m_config.keyMapping[Qt::Key_Down] = ACTION_SOFT_DROP;
    m_config.keyMapping[Qt::Key_S] = ACTION_SOFT_DROP;
    m_config.keyMapping[Qt::Key_Space] = ACTION_HARD_DROP;
    m_config.keyMapping[Qt::Key_Up] = ACTION_ROTATE_CW;
    m_config.keyMapping[Qt::Key_W] = ACTION_ROTATE_CW;
    m_config.keyMapping[Qt::Key_Z] = ACTION_ROTATE_CCW;
    m_config.keyMapping[Qt::Key_C] = ACTION_HOLD;
    m_config.keyMapping[Qt::Key_P] = ACTION_PAUSE;
    m_config.keyMapping[Qt::Key_R] = ACTION_RESTART;
}

void InputHandler::setInputConfig(const InputConfig& config)
{
    m_autoRepeatTimer->setInterval(m_config.autoRepeatInterval);
    m_config = config;
}

bool InputHandler::processKeyEvent(QKeyEvent* event)
{
    Qt::Key key = static_cast<Qt::Key>(event->key());

    if (!m_config.keyMapping.contains(key)) {
        return false;
    }

    GameAction action = m_config.keyMapping[key];

    if (event->type() == QEvent::KeyPress) {
        // 处理按键按下
        if (!m_isRepeating) {
            emit actionTriggered(action);

            // 对于移动操作，启动自动重复
            if (action == ACTION_MOVE_LEFT || action == ACTION_MOVE_RIGHT) {
                startAutoRepeat(action);
            }
            // 软下落不启动自动重复，而是持续状态
            else if (action == ACTION_SOFT_DROP) {
                m_currentRepeatingAction = action;
                m_isRepeating = true;
            }
        }
        else if (action != ACTION_MOVE_LEFT && action != ACTION_MOVE_RIGHT) {
            // 自动重复状态下依然可以响应其他按键
            emit actionTriggered(action);
        }
        return true;
    }
    else if (event->type() == QEvent::KeyRelease) {
        // 处理按键释放
        if (m_isRepeating && action == m_currentRepeatingAction) {
            // 停止自动重复
            stopAutoRepeat();

            // 如果是软下落释放，发送停止信号
            if (action == ACTION_SOFT_DROP) {
                emit actionReleased(action);
            }
        } else {
            // 即使不在重复状态，也要发送释放信号（针对软下落）
            if (action == ACTION_SOFT_DROP) {
                emit actionReleased(action);
            }
        }
        return true;
    }

    return false;
}

void InputHandler::startAutoRepeat(GameAction action)
{
    m_currentRepeatingAction = action;
    m_autoRepeatTimer->setSingleShot(true);
    m_autoRepeatTimer->start(m_config.autoRepeatDelay + add_autoRepeatDelay);
    m_isRepeating = true;
}

void InputHandler::stopAutoRepeat()
{
    m_autoRepeatTimer->stop();
    m_currentRepeatingAction = ACTION_COUNT;
    m_isRepeating = false;
}

void InputHandler::onAutoRepeat()
{
    if (m_isRepeating && m_currentRepeatingAction != ACTION_COUNT) {
        emit actionTriggered(m_currentRepeatingAction);
        m_autoRepeatTimer->setSingleShot(false);
        m_autoRepeatTimer->start(m_config.autoRepeatInterval);
    }
}

void InputHandler::updateAutoRepeatDelay(int level){
    // 根据游戏等级缩短自动重复等待延迟
    add_autoRepeatDelay = (200 - 20 * level) > 0 ? (200 - 20 * level) : 0;
}
