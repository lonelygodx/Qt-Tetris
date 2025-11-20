cmake_minimum_required(VERSION 3.16)

project(Tetris LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RESOURCE_FILES resources.qrc)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui Core Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Core Widgets Sql)

add_executable(Tetris WIN32
  main.cpp
  Position.h
  Block.h Block.cpp
  BlockFactory.h BlockFactory.cpp
  GameEngine.h GameEngine.cpp
  GameStats.h
  GameField.h GameField.cpp
  InputHandler.h InputHandler.cpp
  ScoreManager.h ScoreManager.cpp
  TetrisGame.h TetrisGame.cpp
  ${RESOURCE_FILES}  # 包含资源文件
)

target_link_libraries(Tetris PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

include(GNUInstallDirs)
install(TARGETS Tetris
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
