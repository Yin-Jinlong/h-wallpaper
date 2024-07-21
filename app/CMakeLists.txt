project(h-wallpaper)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUT_DIR})

#########
# 编译器 #
#########

set_msvc_utf_8()

#########
# 头文件 #
#########

include_directories(
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

if (${DEBUG})
    set(SKIA_LIB_NAMES
            skia.dll.lib
    )
    set(SKIA_DLLS
            "${SKIA_BIN_DIR}/skia.dll"
    )
else ()
    set_property(TARGET ${PROJECT_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(SKIA_LIB_NAMES
            skia
            opengl32
    )
    set(SKIA_DLLS)
endif ()

target_link_libraries(${PROJECT_NAME}
        $<TARGET_LINKER_FILE:utils>

        # ffmpeg
        avcodec
        # avdevice
        # avfilter
        avformat
        avutil
        swresample
        swscale

        # skia
        ${SKIA_LIB_NAMES}

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
        ${SKIA_DLLS}
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
