cmake_minimum_required(VERSION 3.28)
project(h-wallpaper)

########
# 模块 #
########

include(cmake/libs.cmake)
include(cmake/msvc.cmake)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(LIBS ${CMAKE_SOURCE_DIR}/libs)

set(FFMPEG_LIBS
        avcodec
        # avdevice
        # avfilter
        avformat
        avutil
        swresample
        swscale
)

#########
# 编译器 #
#########

set_msvc_utf_8()

###########
# 外部依赖 #
###########

find_package(ffmpeg)

#########
# 头文件 #
#########

include_directories(
        "include"
        ${FFMPEG_INCLUDE_DIR}
)

###########
# 链接目录 #
###########

link_directories(
        ${FFMPEG_LIB_DIR}
)

aux_source_directory("app" SRCS)

add_executable(${PROJECT_NAME} WIN32
        "resources/resources.rc"
        ${SRCS}
)
target_link_libraries(${PROJECT_NAME}
        ${FFMPEG_LIBS}
)

#########
# 动态库 #
#########

file(GLOB DLLS
        "${FFMPEG_HOME}/bin/avcodec-61.dll"
        #        "${FFMPEG_HOME}/bin/avdevice-61.dll"
        #        "${FFMPEG_HOME}/bin/avfilter-10.dll"
        "${FFMPEG_HOME}/bin/avformat-61.dll"
        "${FFMPEG_HOME}/bin/avutil-59.dll"
        "${FFMPEG_HOME}/bin/swresample-5.dll"
        "${FFMPEG_HOME}/bin/swscale-8.dll"
)

foreach (DLL ${DLLS})
    # 复制dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${CMAKE_BINARY_DIR}"
    )
endforeach ()

########
# 安装 #
########

set(CMAKE_INSTALL_PREFIX "${PROJECT_NAME} 1.0")
install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${DLLS} DESTINATION .)
