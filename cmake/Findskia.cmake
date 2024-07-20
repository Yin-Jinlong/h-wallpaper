set(SKIA_HOME "")
set(SKIA_INCLUDE_DIR "")
set(SKIA_BIN_DIR "")
set(SKIA_LIB_DIR "")

set(SKAI_NAME "skia")

if (NOT ${DEBUG})
    set(SKAI_NAME "skia-static")
endif ()

set_var_dir(SKIA_HOME ${LIBS_DIR}/${SKAI_NAME})
set_var_dir(SKIA_INCLUDE_DIR ${LIBS_DIR}/${SKAI_NAME})
set_var_dir(SKIA_BIN_DIR ${LIBS_DIR}/${SKAI_NAME}/bin)
set_var_dir(SKIA_LIB_DIR ${LIBS_DIR}/${SKAI_NAME}/lib)

include_directories(${SKIA_INCLUDE_DIR})
link_directories(${SKIA_LIB_DIR})
