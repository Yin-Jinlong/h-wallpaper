
set_var_dir(YAML_CPP_HOME ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE} "yaml-cpp 主目录")
set_var_dir(YAML_CPP_INCLUDE_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/include "yaml-cpp include 目录")
set_var_dir(YAML_CPP_BIN_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/bin "yaml-cpp bin 目录")
set_var_dir(YAML_CPP_LIB_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/lib "yaml-cpp lib 目录")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(YAML_CPP_LIBS yaml-cppd)
else ()
    set(YAML_CPP_LIBS yaml-cpp)
endif ()
