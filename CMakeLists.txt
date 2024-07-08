cmake_minimum_required(VERSION 3.28)
project(h-wallpaper-win)

########
# 模块 #
########

include(cmake/libs.cmake)
include(cmake/msvc.cmake)

########
# 变量 #
########

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(LIBS ${CMAKE_SOURCE_DIR}/libs)
set(OUT_DIR ${CMAKE_BINARY_DIR}/out)
set(APP_NAME h-wallpaper)
set(APP_VERSION "1.0")
set(APP_DIR_NAME "${APP_NAME} ${APP_VERSION}")

# 子项目

add_subdirectory(app)

# 安装

set(CMAKE_INSTALL_PREFIX ${APP_DIR_NAME})