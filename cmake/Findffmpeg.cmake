
fetch_lib(ffmpeg
        https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n7.0-latest-win64-lgpl-shared-7.0.zip
)

set(FFMPEG_HOME ${LIBS}/ffmpeg CACHE INTERNAL "ffmpeg 主目录")
set(FFMPEG_INCLUDE_DIR ${LIBS}/ffmpeg/include CACHE INTERNAL "ffmpeg include 目录")
set(FFMPEG_LIB_DIR ${LIBS}/ffmpeg/lib CACHE INTERNAL "ffmpeg lib 目录")
