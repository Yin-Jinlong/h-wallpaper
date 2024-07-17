project(h-wallpaper)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 23)
set(YAML_BUILD_SHARED_LIBS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})

add_definitions(
        -DUNICODE
        -DAPP_NAME=L"h-wallpaper"
        -DAPP_VERSION=1.0
)

#########
# 编译器 #
#########

set_msvc_utf_8()

###########
# 外部依赖 #
###########

find_package(ffmpeg)
find_package(skia)
find_package(yaml-cpp)

#########
# 头文件 #
#########

include_directories(
        "../include"
        "include"
)

###########
# 链接目录 #
###########

#link_directories()

##########
# 主程序 #
##########

aux_source_directory("src" SRCS)

add_executable(${PROJECT_NAME} WIN32
        "resources/resources.rc"
        ${SRCS}
)

target_link_libraries(${PROJECT_NAME}
        # ffmpeg
        avcodec
        # avdevice
        # avfilter
        avformat
        avutil
        swresample
        swscale

        # skia
        skia.dll.lib

        # yaml-cp
        ${YAML_CPP_LIBS}
)

#########
# 动态库 #
#########

file(GLOB DLLS
        "${FFMPEG_BIN_DIR}/avcodec-61.dll"
        # "${FFMPEG_BIN_DIR}/avdevice-61.dll"
        # "${FFMPEG_BIN_DIR}/avfilter-10.dll"
        "${FFMPEG_BIN_DIR}/avformat-61.dll"
        "${FFMPEG_BIN_DIR}/avutil-59.dll"
        "${FFMPEG_BIN_DIR}/swresample-5.dll"
        "${FFMPEG_BIN_DIR}/swscale-8.dll"
        # skia
        "${SKIA_BIN_DIR}/skia.dll"
        # yaml-cpp
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
