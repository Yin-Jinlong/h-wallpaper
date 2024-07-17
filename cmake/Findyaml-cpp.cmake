set(YAML_CPP_HOME "")
set(YAML_CPP_INCLUDE_DIR "")
set(YAML_CPP_BIN_DIR "")
set(YAML_CPP_LIB_DIR "")

set_var_dir(YAML_CPP_HOME ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE})
set_var_dir(YAML_CPP_INCLUDE_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/include)
set_var_dir(YAML_CPP_BIN_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/bin)
set_var_dir(YAML_CPP_LIB_DIR ${LIBS_DIR}/yaml-cpp-${CMAKE_BUILD_TYPE}/lib)

include_directories(${YAML_CPP_INCLUDE_DIR})
link_directories(${YAML_CPP_LIB_DIR})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(YAML_CPP_LIBS yaml-cppd)
else ()
    set(YAML_CPP_LIBS yaml-cpp)
endif ()
