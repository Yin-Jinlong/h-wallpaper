cmake_minimum_required(VERSION 3.28)
project(h-wallpaper-win)

########
# 模块 #
########

include(cmake/libs.cmake)
include(cmake/licenses.cmake)
include(cmake/msvc.cmake)

########
# 变量 #
########

string(TIMESTAMP BUILD_DATE %Y%m%d)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(DOWNLOADS_DIR ${CMAKE_SOURCE_DIR}/downloads)
set(LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)
set(OUT_DIR ${CMAKE_BINARY_DIR}/out)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_TWEAK ${BUILD_DATE})
set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(VERSION_TWEAK ${BUILD_DATE})
set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-${VERSION_TWEAK})

set(APP_NAME h-wallpaper)
set(APP_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(APP_DIR_NAME "${APP_NAME} ${APP_VERSION}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(DEBUG ON)
else ()
    set(DEBUG OFF)
endif ()

file(WRITE ${PROJECT_BINARY_DIR}/VERSION ${PROJECT_VERSION})
file(WRITE ${OUT_DIR}/VERSION ${PROJECT_VERSION})

configure_file(
        "${CMAKE_SOURCE_DIR}/pre-config.h.in"
        "${PROJECT_BINARY_DIR}/pre-config.h"
        @ONLY
)

add_definitions(
        -DUNICODE
)

# 公共引入

include_directories(
        "include"
        "utils/include"
        "${PROJECT_BINARY_DIR}"
)

# 外部库

find_package(ffmpeg)
find_package(skia)
find_package(yaml-cpp)


# 子项目

add_subdirectory(app)
#add_subdirectory(ui)
add_subdirectory(updater)
add_subdirectory(utils)

# 安装

set(CMAKE_INSTALL_PREFIX ${APP_DIR_NAME})
install(FILES
        ${PROJECT_BINARY_DIR}/VERSION
        DESTINATION .
)


########
# 协议 #
########

install(FILES
        ${CMAKE_SOURCE_DIR}/LICENSE.md
        DESTINATION .
)

# ffmpeg
install_licenses(ffmpeg
        "${FFMPEG_HOME}/LICENSE.txt"
)

# skia
install_licenses(skia
        ${SKIA_HOME}/LICENSE
)

# yamp-cpp
install_licenses(yaml-cpp
        ${YAML_CPP_SRC_DIR}/LICENSE
)
