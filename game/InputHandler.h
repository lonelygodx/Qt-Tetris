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

    explicit InputHandler(QObject* parent = nullptr);

    // 输入处理
    bool processKeyEvent(QKeyEvent* event);

signals:
    void actionTriggered(InputHandler::GameAction action);
    void actionReleased(InputHandler::GameAction action);

public slots:
    void updateAutoRepeatDelay(int level);

private slots:
    void onAutoRepeat();

private:
    void initializeDefaultMapping();
    void startAutoRepeat(GameAction action);
    void stopAutoRepeat();

    // 输入配置
    QHash<Qt::Key, GameAction> keyMapping; // 按键映射
    int add_autoRepeatDelay;

    // 自动重复状态
    QTimer* m_autoRepeatTimer;
    GameAction m_currentRepeatingAction;
    bool m_isRepeating;
};

#endif // INPUTHANDLER_H
