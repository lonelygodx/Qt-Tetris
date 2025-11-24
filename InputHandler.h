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
        QHash<Qt::Key, GameAction> keyMapping;
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
    void setEnabled(bool enabled) { m_enabled = enabled; }

signals:
    void actionTriggered(GameAction action);
    void actionRepeated(GameAction action);
    void actionReleased(GameAction action);

private slots:
    void onAutoRepeat();

private:
    void initializeDefaultMapping();
    void startAutoRepeat(GameAction action);
    void stopAutoRepeat();

    InputConfig m_config;
    bool m_enabled;

    // 自动重复状态
    QTimer* m_autoRepeatTimer;
    GameAction m_currentRepeatingAction;
    bool m_isRepeating;
};

#endif // INPUTHANDLER_H
