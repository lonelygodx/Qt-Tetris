#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include <QObject>
#include <QHash>
#include <QTimer>
#include <QKeyEvent>

class InputHandler : public QObject
{
    Q_OBJECT

public:
    enum GameAction {
        ACTION_MOVE_LEFT,
        ACTION_MOVE_RIGHT,
        ACTION_SOFT_DROP,
        ACTION_HARD_DROP,
        ACTION_ROTATE_CW,
        ACTION_ROTATE_CCW,
        ACTION_HOLD,
        ACTION_PAUSE,
        ACTION_RESTART,
        ACTION_COUNT
    };

    struct InputConfig {
        QHash<Qt::Key, GameAction> keyMapping; // 按键映射
        int autoRepeatDelay;    // 自动重复延迟(ms)
        int autoRepeatInterval; // 自动重复间隔(ms)

        InputConfig() : autoRepeatDelay(150), autoRepeatInterval(50) {}
    };

    explicit InputHandler(QObject* parent = nullptr);

    // 配置管理
    void setInputConfig(const InputConfig& config);
    InputConfig getInputConfig() const { return m_config; }

    // 输入处理
    bool processKeyEvent(QKeyEvent* event);

signals:
    void actionTriggered(InputHandler::GameAction action);
    void actionReleased(InputHandler::GameAction action);

private:
    void initializeDefaultMapping();

    InputConfig m_config;
};

#endif // INPUTHANDLER_H
