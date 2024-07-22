set(LIBYAML_SRC_DIR "")
set(LIBYAML_HOME "")
set(LIBYAML_INCLUDE_DIR "")
set(LIBYAML_LIB_DIR "")

set_var_dir(LIBYAML_SRC_DIR ${DOWNLOADS_DIR}/libyaml)
set_var_dir(LIBYAML_HOME ${LIBS_DIR}/libyaml-${CMAKE_BUILD_TYPE})
set_var_dir(LIBYAML_INCLUDE_DIR ${LIBS_DIR}/libyaml-${CMAKE_BUILD_TYPE}/include)
set_var_dir(LIBYAML_LIB_DIR ${LIBS_DIR}/libyaml-${CMAKE_BUILD_TYPE}/lib)

include_directories(${LIBYAML_INCLUDE_DIR})
link_directories(${LIBYAML_LIB_DIR})
