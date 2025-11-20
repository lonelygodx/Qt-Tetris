#include "TetrisGame.h"
#include <QApplication>
#include <QFile>

bool checkResourceAvailability()
{
    // 检查各种格式的图标是否可用
    QStringList iconPaths = {
        ":/resources/icons/games_tetris.ico",
        ":/resources/icons/games_tetris.png",
        ":/resources/icons/games_tetris.icns"
    };

    for (const QString &path : iconPaths) {
        if (QFile::exists(path)) {
            return true;
        }
    }

    qDebug() << "未找到任何图标文件";
    return false;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("俄罗斯方块");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("liguowei");

    // 检查资源
    if (!checkResourceAvailability()) {
        qWarning() << "图标资源检查失败，但继续运行...";
    }

    // 设置应用程序图标
    QIcon appIcon(":/resources/icons/games_tetris.png");
    if (!appIcon.isNull()) {
        app.setWindowIcon(appIcon);
    } else {
        qWarning() << "应用程序图标设置失败";
    }

    MainWindow window;
    window.show();

    return app.exec();
}
