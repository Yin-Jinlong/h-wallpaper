set(SKIA_HOME "")
set(SKIA_INCLUDE_DIR "")
set(SKIA_BIN_DIR "")
set(SKIA_LIB_DIR "")

set_var_dir(SKIA_HOME ${LIBS_DIR}/skia)
set_var_dir(SKIA_INCLUDE_DIR ${LIBS_DIR}/skia)
set_var_dir(SKIA_BIN_DIR ${LIBS_DIR}/skia/bin)
set_var_dir(SKIA_LIB_DIR ${LIBS_DIR}/skia/lib)

include_directories(${SKIA_INCLUDE_DIR})
link_directories(${SKIA_LIB_DIR})
