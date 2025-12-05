#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <string>
#include <memory>
#include "SimpleIni.h"  // SimpleIni 头文件

class GameConfig
{
    // 默认配置数据
    struct configData {
        // 版本信息
        std::string version = "0.0.4";
        // 方块
        std::string randomizerType = "7-bag"; // 随机模式
        // 界面
        int width = 10;                    // 场地宽度
        int height = 20;                   // 场地高度
        int cellSize = 30;                 // 格子大小
        int widgetCellSize = 20;           // 预览窗格格子大小
        // 游戏
        bool ghostEnabled = true;          // 是否开启幽灵方块
        bool canHold = true;               // 是否开启暂存
        int gameTimerInterval = 16;        // 游戏更新间隔（刷新率）
        // 控制
        int autoRepeatDelay = 100;         // 最短自动重复延迟(ms)
        int addRepeatDelay = 200;          // 自动重复延迟变化量(ms)
        int autoRepeatInterval = 50;       // 自动重复间隔(ms)
        // 分数
        int maxHighScores = 5;             // 高分榜显示个数
        // UI
        int MainWindowFixedSizeW = 700;    // 主窗口宽度
        int MainWindowFixedSizeH = 700;    // 主窗口高度
        int GameWidgetFixedSizeW = 300;    // 游戏窗口宽度
        int GameWidgetFixedSizeH = 600;    // 游戏窗口高度
        int BlockWidgetFixedSizeW = 130;   // 辅助窗口宽度
        int BlockWidgetFixedSizeH = 130;   // 辅助窗口高度
        int InfoPanelWidgetWidth = 220;    // 信息面板宽度
    };

public:
    // 删除拷贝构造函数和赋值操作符，确保单例
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;

    // 获取单例实例
    static GameConfig& getInstance();

    // 初始化配置文件
    bool initialize(const std::string& configFilePath = "config.ini");

    // 读取配置值
    std::string getStringValue(const std::string& section,
                               const std::string& key,
                               const std::string& defaultValue = "");

    int getIntValue(const std::string& section,
                    const std::string& key,
                    int defaultValue = 0);

    double getDoubleValue(const std::string& section,
                          const std::string& key,
                          double defaultValue = 0.0);

    bool getBoolValue(const std::string& section,
                      const std::string& key,
                      bool defaultValue = false);

    // 写入配置值
    bool setStringValue(const std::string& section,
                        const std::string& key,
                        const std::string& value);

    bool setIntValue(const std::string& section,
                     const std::string& key,
                     int value);

    bool setDoubleValue(const std::string& section,
                        const std::string& key,
                        double value);

    bool setBoolValue(const std::string& section,
                      const std::string& key,
                      bool value);

    // 保存配置到文件
    bool saveConfig();

    // 重新加载配置文件
    bool reloadConfig();

    // 将配置数据加载到配置对象中
    void updateIniData();

    // 将配置对象加载到配置数据中
    void updateConfigData();

    // 获取配置文件路径
    std::string getConfigFilePath() const;

    // 获取配置文件数据
    configData getConfigData() const;

private:
    // 加载默认配置
    void setDefaultConfig();
    // 私有构造析构函数，禁止访问
    GameConfig() = default;
    ~GameConfig() = default;

private:
    std::unique_ptr<CSimpleIniA> m_ini;
    std::string m_configFilePath;
    configData m_configData;
    bool m_initialized = false;
};

// 宏定义提供快捷访问
#define GAME_CONFIG             GameConfig::getInstance()
#define GAME_CONFIG_DATA        GAME_CONFIG.getConfigData()
#define GAME_VERSION            GAME_CONFIG_DATA.version
#define RANDOMIZER_TYPE         GAME_CONFIG_DATA.randomizerType
#define FIELD_WIDTH             GAME_CONFIG_DATA.width
#define FIELD_HEIGHT            GAME_CONFIG_DATA.height
#define FIELD_CELL_SIZE         GAME_CONFIG_DATA.cellSize
#define WIDGET_CELL_SIZE        GAME_CONFIG_DATA.widgetCellSize
#define GHOST_BLOCK_ENABLED     GAME_CONFIG_DATA.ghostEnabled
#define BLOCK_CANHOLD           GAME_CONFIG_DATA.canHold
#define GAME_TIMER_INTERVAL     GAME_CONFIG_DATA.gameTimerInterval
#define AUTO_REPEAT_DELAY       GAME_CONFIG_DATA.autoRepeatDelay
#define ADD_REPEAT_DELAY        GAME_CONFIG_DATA.addRepeatDelay
#define AUTO_REPEAT_INTERVAL    GAME_CONFIG_DATA.autoRepeatInterval
#define MAX_HIGH_SCORES         GAME_CONFIG_DATA.maxHighScores
#define MAINWINDOW_FIXED_SIZEW  GAME_CONFIG_DATA.MainWindowFixedSizeW
#define MAINWINDOW_FIXED_SIZEH  GAME_CONFIG_DATA.MainWindowFixedSizeH
#define GAMEWIDGET_FIXED_SIZEW  GAME_CONFIG_DATA.GameWidgetFixedSizeW
#define GAMEWIDGET_FIXED_SIZEH  GAME_CONFIG_DATA.GameWidgetFixedSizeH
#define BLOCKWIDGET_FIXED_SIZEW GAME_CONFIG_DATA.BlockWidgetFixedSizeW
#define BLOCKWIDGET_FIXED_SIZEH GAME_CONFIG_DATA.BlockWidgetFixedSizeH
#define INFOPANELWIDGETWIDTH    GAME_CONFIG_DATA.InfoPanelWidgetWidth

#endif // GAMECONFIG_H
