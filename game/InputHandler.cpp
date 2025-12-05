#include "InputHandler.h"
#include "GameConfig.h"

InputHandler::InputHandler(QObject* parent)
    : QObject(parent)
    , m_currentRepeatingAction(ACTION_COUNT)
    , m_isRepeating(false)
    , add_autoRepeatDelay(ADD_REPEAT_DELAY)
{
    m_autoRepeatTimer = new QTimer(this);
    connect(m_autoRepeatTimer, &QTimer::timeout, this, &InputHandler::onAutoRepeat);
    m_autoRepeatTimer->setInterval(AUTO_REPEAT_INTERVAL);
    initializeDefaultMapping();
}

void InputHandler::initializeDefaultMapping()
{
    keyMapping.clear();

    // 默认按键映射，wasd也可以控制上下左右
    keyMapping[Qt::Key_Left] = ACTION_MOVE_LEFT;
    keyMapping[Qt::Key_A] = ACTION_MOVE_LEFT;
    keyMapping[Qt::Key_Right] = ACTION_MOVE_RIGHT;
    keyMapping[Qt::Key_D] = ACTION_MOVE_RIGHT;
    keyMapping[Qt::Key_Down] = ACTION_SOFT_DROP;
    keyMapping[Qt::Key_S] = ACTION_SOFT_DROP;
    keyMapping[Qt::Key_Space] = ACTION_HARD_DROP;
    keyMapping[Qt::Key_Up] = ACTION_ROTATE_CW;
    keyMapping[Qt::Key_W] = ACTION_ROTATE_CW;
    keyMapping[Qt::Key_Z] = ACTION_ROTATE_CCW;
    keyMapping[Qt::Key_C] = ACTION_HOLD;
    keyMapping[Qt::Key_P] = ACTION_PAUSE;
    keyMapping[Qt::Key_R] = ACTION_RESTART;
}

bool InputHandler::processKeyEvent(QKeyEvent* event)
{
    Qt::Key key = static_cast<Qt::Key>(event->key());

    if (!keyMapping.contains(key)) {
        return false;
    }

    GameAction action = keyMapping[key];

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
    m_autoRepeatTimer->start(AUTO_REPEAT_DELAY + add_autoRepeatDelay);
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
        m_autoRepeatTimer->start(AUTO_REPEAT_INTERVAL);
    }
}

void InputHandler::updateAutoRepeatDelay(int level){
    if (add_autoRepeatDelay <= 0)
        return;
    // 根据游戏等级缩短自动重复等待延迟
    add_autoRepeatDelay = (ADD_REPEAT_DELAY - 20 * level) > 0 ? (ADD_REPEAT_DELAY - 20 * level) : 0;
}
