cmake_minimum_required(VERSION 3.16)

project(Tetris LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
  main.cpp
)

set(HEADERS
  # 添加 .h 文件
)

set(GAME_SOURCES
  game/Block.cpp
  game/BlockFactory.cpp
  game/GameEngine.cpp
  game/GameField.cpp
  game/InputHandler.cpp
  game/ScoreManager.cpp
)

set(GAME_HEADERS
  game/Block.h
  game/BlockFactory.h
  game/GameEngine.h
  game/GameField.h
  game/InputHandler.h
  game/ScoreManager.h
)

set(UI_SOURCES
  ui/GameWidget.cpp
  ui/MainWindow.cpp
)

set(UI_HEADERS
  ui/GameWidget.h
  ui/MainWindow.h
)

set(CONFIG_SOURCES
  config/GameConfig.cpp
  config/GameConfig.cpp
)

set(CONFIG_HEADERS
  config/GameConfig.h
  config/GameConfig.h
)

set(DATA_HEADERS
  data/GameStats.h
  data/Position.h
)

set(RESOURCE_FILES
  resources.qrc
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Core Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Core Widgets Sql)

add_executable(Tetris WIN32
  ${SOURCES}
  ${HEADERS}
  ${GAME_SOURCES}
  ${GAME_HEADERS}
  ${UI_SOURCES}
  ${UI_HEADERS}
  ${CONFIG_SOURCES}
  ${CONFIG_HEADERS}
  ${DATA_HEADERS}
  ${RESOURCE_FILES}
)

# 添加头文件包含路径
target_include_directories(Tetris PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/game
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
  ${CMAKE_CURRENT_SOURCE_DIR}/data
  ${CMAKE_CURRENT_SOURCE_DIR}/simpleini
  ${CMAKE_CURRENT_SOURCE_DIR}/config
)

target_link_libraries(Tetris PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

include(GNUInstallDirs)
install(TARGETS Tetris
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
