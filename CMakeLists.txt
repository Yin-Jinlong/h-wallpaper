cmake_minimum_required(VERSION 3.28)
project(h-wallpaper)

set(CMAKE_CXX_STANDARD 26)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(FFMPEG_HOME $ENV{FFMPEG_HOME})

function(check_dir DIR VAR NAME)
    if (NOT EXISTS "${DIR}")
        message(FATAL_ERROR "${VAR} not found")
    else ()
        message("found ${NAME} at ${FFMPEG_HOME}")
    endif ()
endfunction()

check_dir(${FFMPEG_HOME} FFMPEG_HOME ffmpeg)

include_directories(
        "include"
        "${FFMPEG_HOME}/include"
)

link_directories(
        "${FFMPEG_HOME}/lib"
)

aux_source_directory("app" SRCS)

add_executable(${PROJECT_NAME} WIN32
        "resources/resources.rc"
        ${SRCS}
)
target_link_libraries(${PROJECT_NAME}
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
)

file(GLOB DLLS
        "${FFMPEG_HOME}/bin/avcodec-61.dll"
        "${FFMPEG_HOME}/bin/avdevice-61.dll"
        "${FFMPEG_HOME}/bin/avfilter-10.dll"
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

set(CMAKE_INSTALL_PREFIX "${PROJECT_NAME} 1.0")

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${DLLS} DESTINATION .)
