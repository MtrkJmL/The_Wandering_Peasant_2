cmake_minimum_required(VERSION 3.10)
project(Wandering_Peasant_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wandering_peasant_2
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Boss.cpp
    src/Item.cpp
    src/Potion.cpp
    src/AttackType.cpp
    src/ChapterMap.cpp
    src/RunState.cpp
    src/Quest.cpp
    src/Node.cpp
    src/Discoveries.cpp
    src/Achievement.cpp
)

target_include_directories(wandering_peasant_2 PRIVATE include)

add_executable(test_map src/test_map_main.cpp src/ChapterMap.cpp src/Node.cpp)
target_include_directories(test_map PRIVATE include)
