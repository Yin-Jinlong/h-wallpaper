project(h-wallpaper)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 23)
set(YAML_BUILD_SHARED_LIBS ON)

set(FFMPEG_LIBS
        avcodec
        # avdevice
        # avfilter
        avformat
        avutil
        swresample
        swscale
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})

add_definitions(
        -DAPP_NAME="h-wallpaper"
        -DAPP_VERSION="1.0"
)

#########
# 编译器 #
#########

set_msvc_utf_8()

###########
# 外部依赖 #
###########

find_package(ffmpeg)
find_package(yaml-cpp)

#########
# 头文件 #
#########

include_directories(
        "../include"
        "include"
        ${FFMPEG_INCLUDE_DIR}
        ${YAML_CPP_INCLUDE_DIR}
)

###########
# 链接目录 #
###########

link_directories(
        ${FFMPEG_LIB_DIR}
        ${YAML_CPP_LIB_DIR}
)

##########
# 主程序 #
##########

aux_source_directory("src" SRCS)

add_executable(${PROJECT_NAME} WIN32
        "resources/resources.rc"
        ${SRCS}
)

target_link_libraries(${PROJECT_NAME}
        ${FFMPEG_LIBS}
        ${YAML_CPP_LIBS}
)

#########
# 动态库 #
#########

file(GLOB DLLS
        "${FFMPEG_HOME}/bin/avcodec-61.dll"
        # "${FFMPEG_HOME}/bin/avdevice-61.dll"
        # "${FFMPEG_HOME}/bin/avfilter-10.dll"
        "${FFMPEG_HOME}/bin/avformat-61.dll"
        "${FFMPEG_HOME}/bin/avutil-59.dll"
        "${FFMPEG_HOME}/bin/swresample-5.dll"
        "${FFMPEG_HOME}/bin/swscale-8.dll"
        "${YAML_CPP_BIN_DIR}/*.dll"
)

foreach (DLL ${DLLS})
    # 复制dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${OUT_DIR}"
    )
endforeach ()

########
# 安装 #
########

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${DLLS} DESTINATION .)
#install(TARGETS yaml-cpp DESTINATION .)
