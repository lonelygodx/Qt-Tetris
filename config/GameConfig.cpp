#include "GameConfig.h"
#include "GameConfig.h"
#include <filesystem>
#include <QDebug>

// 获取单例实例
GameConfig& GameConfig::getInstance()
{
    static GameConfig instance;
    return instance;
}

// 初始化配置文件
bool GameConfig::initialize(const std::string& configFilePath)
{
    m_configFilePath = configFilePath;
    m_ini = std::make_unique<CSimpleIniA>();

    // 设置 Unicode 支持
    m_ini->SetUnicode();

    // 如果配置文件不存在，加载默认配置
    if (!std::filesystem::exists(configFilePath))
    {
        qDebug() << "加载默认配置";
        // 加载默认配置
        setDefaultConfig();

        // 保存配置文件
        SI_Error result = m_ini->SaveFile(configFilePath.c_str());
        if (result < 0)
        {
            return false;
        }
    }

    // 加载配置文件
    SI_Error result = m_ini->LoadFile(configFilePath.c_str());
    if (result < 0)
    {
        return false;
    }
    m_initialized = true;
    // 更新配置数据
    updateConfigData();
    return true;
}

// 读取字符串配置值
std::string GameConfig::getStringValue(const std::string& section,
                                          const std::string& key,
                                          const std::string& defaultValue)
{
    if (!m_initialized) return defaultValue;

    const char* value = m_ini->GetValue(section.c_str(), key.c_str(), defaultValue.c_str());
    return value ? std::string(value) : defaultValue;
}

// 读取整数配置值
int GameConfig::getIntValue(const std::string& section,
                               const std::string& key,
                               int defaultValue)
{
    if (!m_initialized) return defaultValue;

    return static_cast<int>(m_ini->GetLongValue(section.c_str(), key.c_str(), defaultValue));
}

// 读取浮点数配置值
double GameConfig::getDoubleValue(const std::string& section,
                                     const std::string& key,
                                     double defaultValue)
{
    if (!m_initialized) return defaultValue;

    return m_ini->GetDoubleValue(section.c_str(), key.c_str(), defaultValue);
}

// 读取布尔配置值
bool GameConfig::getBoolValue(const std::string& section,
                                 const std::string& key,
                                 bool defaultValue)
{
    if (!m_initialized) return defaultValue;

    return m_ini->GetBoolValue(section.c_str(), key.c_str(), defaultValue);
}

// 设置字符串配置值
bool GameConfig::setStringValue(const std::string& section,
                                   const std::string& key,
                                   const std::string& value)
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->SetValue(section.c_str(), key.c_str(), value.c_str());
    return (result == SI_OK);
}

// 设置整数配置值
bool GameConfig::setIntValue(const std::string& section,
                                const std::string& key,
                                int value)
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->SetLongValue(section.c_str(), key.c_str(), value);
    return (result == SI_OK);
}

// 设置浮点数配置值
bool GameConfig::setDoubleValue(const std::string& section,
                                   const std::string& key,
                                   double value)
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->SetDoubleValue(section.c_str(), key.c_str(), value);
    return (result == SI_OK);
}

// 设置布尔配置值
bool GameConfig::setBoolValue(const std::string& section,
                                 const std::string& key,
                                 bool value)
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->SetBoolValue(section.c_str(), key.c_str(), value);
    return (result == SI_OK);
}

// 保存配置到文件
bool GameConfig::saveConfig()
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->SaveFile(m_configFilePath.c_str());
    return (result == SI_OK);
}

// 重新加载配置文件
bool GameConfig::reloadConfig()
{
    if (!m_initialized) return false;

    SI_Error result = m_ini->LoadFile(m_configFilePath.c_str());
    return (result == SI_OK);
}

// 获取配置文件路径
std::string GameConfig::getConfigFilePath() const
{
    return m_configFilePath;
}

// 获取配置数据
GameConfig::configData GameConfig::getConfigData() const
{
    return m_configData;
}


//
void GameConfig::setDefaultConfig()
{
    configData default_configData;
    // 版本信息
    m_ini->SetValue("General", "version", default_configData.version.c_str());
    // 方块相关
    m_ini->SetValue("Block", "randomizerType", default_configData.randomizerType.c_str());
    // 游戏引擎相关
    m_ini->SetBoolValue("Engine", "ghostEnabled", default_configData.ghostEnabled);
    m_ini->SetBoolValue("Engine", "canHold", default_configData.canHold);
    m_ini->SetLongValue("Engine", "gameTimerInterval", default_configData.gameTimerInterval);
    // 游戏界面相关
    m_ini->SetLongValue("Field", "width", default_configData.width);
    m_ini->SetLongValue("Field", "height", default_configData.height);
    m_ini->SetLongValue("Field", "cellSize", default_configData.cellSize);
    m_ini->SetLongValue("Field", "widgetCellSize", default_configData.widgetCellSize);
    // 窗口相关1
    m_ini->SetLongValue("MainWindow", "MainWindowFixedSizeW", default_configData.MainWindowFixedSizeW);
    m_ini->SetLongValue("MainWindow", "MainWindowFixedSizeH", default_configData.MainWindowFixedSizeH);
    // 窗口相关2
    m_ini->SetLongValue("GameWidget", "GameWidgetFixedSizeW", default_configData.GameWidgetFixedSizeW);
    m_ini->SetLongValue("GameWidget", "GameWidgetFixedSizeH", default_configData.GameWidgetFixedSizeH);
    // 窗口相关3
    m_ini->SetLongValue("BlockWidget", "BlockWidgetFixedSizeW", default_configData.BlockWidgetFixedSizeW);
    m_ini->SetLongValue("BlockWidget", "BlockWidgetFixedSizeH", default_configData.BlockWidgetFixedSizeH);
    // 窗口相关4
    m_ini->SetLongValue("InfoPanel", "InfoPanelWidgetWidth", default_configData.InfoPanelWidgetWidth);
    // 控制相关
    m_ini->SetLongValue("Input", "autoRepeatDelay", default_configData.autoRepeatDelay);
    m_ini->SetLongValue("Input", "addRepeatDelay", default_configData.addRepeatDelay);
    m_ini->SetLongValue("Input", "autoRepeatInterval", default_configData.autoRepeatInterval);
    // 分数相关
    m_ini->SetLongValue("Score", "maxHighScores", default_configData.maxHighScores);
}

void GameConfig::updateIniData()
{
    // 版本信息
    m_ini->SetValue("General", "version", m_configData.version.c_str());
    // 方块相关
    m_ini->SetValue("Block", "randomizerType", m_configData.randomizerType.c_str());
    // 游戏引擎相关
    m_ini->SetBoolValue("Engine", "ghostEnabled", m_configData.ghostEnabled);
    m_ini->SetBoolValue("Engine", "canHold", m_configData.canHold);
    m_ini->SetLongValue("Engine", "gameTimerInterval", m_configData.gameTimerInterval);
    // 游戏界面相关
    m_ini->SetLongValue("Field", "width", m_configData.width);
    m_ini->SetLongValue("Field", "height", m_configData.height);
    m_ini->SetLongValue("Field", "cellSize", m_configData.cellSize);
    m_ini->SetLongValue("Field", "widgetCellSize", m_configData.widgetCellSize);
    // 窗口相关1
    m_ini->SetLongValue("MainWindow", "MainWindowFixedSizeW", m_configData.MainWindowFixedSizeW);
    m_ini->SetLongValue("MainWindow", "MainWindowFixedSizeH", m_configData.MainWindowFixedSizeH);
    // 窗口相关2
    m_ini->SetLongValue("GameWidget", "GameWidgetFixedSizeW", m_configData.GameWidgetFixedSizeW);
    m_ini->SetLongValue("GameWidget", "GameWidgetFixedSizeH", m_configData.GameWidgetFixedSizeH);
    // 窗口相关3
    m_ini->SetLongValue("BlockWidget", "BlockWidgetFixedSizeW", m_configData.BlockWidgetFixedSizeW);
    m_ini->SetLongValue("BlockWidget", "BlockWidgetFixedSizeH", m_configData.BlockWidgetFixedSizeH);
    // 窗口相关4
    m_ini->SetLongValue("InfoPanel", "InfoPanelWidgetWidth", m_configData.InfoPanelWidgetWidth);
    // 控制相关
    m_ini->SetLongValue("Input", "autoRepeatDelay", m_configData.autoRepeatDelay);
    m_ini->SetLongValue("Input", "addRepeatDelay", m_configData.addRepeatDelay);
    m_ini->SetLongValue("Input", "autoRepeatInterval", m_configData.autoRepeatInterval);
    // 分数相关
    m_ini->SetLongValue("Score", "maxHighScores", m_configData.maxHighScores);
}

void GameConfig::updateConfigData()
{
    // 如果没有值则取默认值
    m_configData.version = getStringValue("General", "version", m_configData.version);

    m_configData.randomizerType = getStringValue("Block", "randomizerType", m_configData.randomizerType);
    m_configData.ghostEnabled = getBoolValue("Engine", "ghostEnabled", m_configData.ghostEnabled);
    m_configData.canHold = getBoolValue("Engine", "canHold", m_configData.canHold);
    m_configData.gameTimerInterval = getIntValue("Engine", "gameTimerInterval", m_configData.gameTimerInterval);

    m_configData.width = getIntValue("Field", "width", m_configData.width);
    m_configData.height = getIntValue("Field", "height", m_configData.height);
    m_configData.cellSize = getIntValue("Field", "cellSize", m_configData.cellSize);
    m_configData.widgetCellSize = getIntValue("Field", "widgetCellSize", m_configData.widgetCellSize);

    m_configData.MainWindowFixedSizeW = getIntValue("MainWindow", "MainWindowFixedSizeW", m_configData.MainWindowFixedSizeW);
    m_configData.MainWindowFixedSizeH = getIntValue("MainWindow", "MainWindowFixedSizeH", m_configData.MainWindowFixedSizeH);
    m_configData.GameWidgetFixedSizeW = getIntValue("GameWidget", "GameWidgetFixedSizeW", m_configData.GameWidgetFixedSizeW);
    m_configData.GameWidgetFixedSizeH = getIntValue("GameWidget", "GameWidgetFixedSizeH", m_configData.GameWidgetFixedSizeH);
    m_configData.BlockWidgetFixedSizeW = getIntValue("BlockWidget", "BlockWidgetFixedSizeW", m_configData.BlockWidgetFixedSizeW);
    m_configData.BlockWidgetFixedSizeH = getIntValue("BlockWidget", "BlockWidgetFixedSizeH", m_configData.BlockWidgetFixedSizeH);
    m_configData.InfoPanelWidgetWidth = getIntValue("InfoPanel", "InfoPanelWidgetWidth", m_configData.InfoPanelWidgetWidth);

    m_configData.autoRepeatDelay = getIntValue("Input", "autoRepeatDelay", m_configData.autoRepeatDelay);
    m_configData.addRepeatDelay = getIntValue("Input", "addRepeatDelay", m_configData.addRepeatDelay);
    m_configData.autoRepeatInterval = getIntValue("Input", "autoRepeatInterval", m_configData.autoRepeatInterval);

    m_configData.maxHighScores = getIntValue("Score", "maxHighScores", m_configData.maxHighScores);
}
