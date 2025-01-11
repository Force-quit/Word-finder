cmake_minimum_required(VERSION 3.26)
project(Word-finder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(ENV{PATH} "C:/Qt/6.8.1/msvc2022_64/bin;$ENV{PATH}")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/msvc2022_64")
    add_compile_options(/EHsc)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

include(FetchContent)
FetchContent_Declare(
    EQUtilities
    URL https://github.com/Force-quit/EQUtilities/archive/refs/tags/v1.1.0.zip
)
FetchContent_MakeAvailable(EQUtilities)

set(SOURCE
    src/QWordFinder.hpp
    src/QWordFinder.cpp
    src/QWordFinderWorker.hpp
    src/QWordFinderWorker.cpp
    src/main.cpp
)

qt_add_executable(Word-finder ${SOURCE})
target_link_libraries(Word-finder PRIVATE Qt6::Widgets EQUtilities)

if(WIN32)
    set_target_properties(Word-finder PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

add_subdirectory(resources)