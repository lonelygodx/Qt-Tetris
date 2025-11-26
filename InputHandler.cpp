#include "InputHandler.h"

InputHandler::InputHandler(QObject* parent)
    : QObject(parent)
{
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
        // 发送按键信号
        emit actionTriggered(action);
        return true;
    }
    else if (event->type() == QEvent::KeyRelease) {
        // 发送释放信号
        emit actionReleased(action);
        return true;
    }

    return false;
}
