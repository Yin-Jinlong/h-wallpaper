set(FFMPEG_HOME "")
set(FFMPEG_INCLUDE_DIR "")
set(FFMPEG_BIN_DIR "")
set(FFMPEG_LIB_DIR "")

set_var_dir(FFMPEG_HOME ${LIBS_DIR}/ffmpeg)
set_var_dir(FFMPEG_INCLUDE_DIR ${LIBS_DIR}/ffmpeg/include)
set_var_dir(FFMPEG_BIN_DIR ${LIBS_DIR}/ffmpeg/bin)
set_var_dir(FFMPEG_LIB_DIR ${LIBS_DIR}/ffmpeg/lib)

include_directories(${FFMPEG_INCLUDE_DIR})
link_directories(${FFMPEG_LIB_DIR})
