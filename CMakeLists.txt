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
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${PROJECT_VERSION_TWEAK})

set(APP_NAME h-wallpaper)
set(APP_VERSION "\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\"")
set(APP_DIR_NAME "${APP_NAME} ${APP_VERSION}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(DEBUG ON)
else ()
    set(DEBUG OFF)
endif ()

file(WRITE ${PROJECT_BINARY_DIR}/VERSION ${PROJECT_VERSION})

add_definitions(
        -DUNICODE
        -DAPP_NAME=L"h-wallpaper"
        -DVERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        -DVERSION_MINOR=${PROJECT_VERSION_MINOR}
        -DVERSION_PATCH=${PROJECT_VERSION_PATCH}
        -DVERSION_TWEAK=${PROJECT_VERSION_TWEAK}
        -DAPP_VERSION=${APP_VERSION}
        -DBUILD_DATE="${BUILD_DATE}"
)

# 子项目

add_subdirectory(app)
#add_subdirectory(ui)

# 安装

set(CMAKE_INSTALL_PREFIX ${APP_DIR_NAME})
install(FILES
        ${PROJECT_BINARY_DIR}/VERSION
        DESTINATION .
)