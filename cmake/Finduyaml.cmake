set(UYAML_HOME "")
set(UYAML_SRC_DIR "")
set(UYAML_INCLUDE_DIR "")
set(UYAML_LIB_DIR "")

set_var_dir(UYAML_SRC_DIR ${DOWNLOADS_DIR}/uyaml)
set_var_dir(UYAML_HOME ${LIBS_DIR}/uyaml-${CMAKE_BUILD_TYPE})
set_var_dir(UYAML_INCLUDE_DIR ${LIBS_DIR}/uyaml-${CMAKE_BUILD_TYPE}/include)
set_var_dir(UYAML_LIB_DIR ${LIBS_DIR}/uyaml-${CMAKE_BUILD_TYPE}/lib)

include_directories(${UYAML_INCLUDE_DIR})
link_directories(${UYAML_LIB_DIR})
