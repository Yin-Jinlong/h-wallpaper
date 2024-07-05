fetch_git_lib(yaml-cpp
        https://github.com/jbeder/yaml-cpp.git
        0.8.0
)

set(YAML_CPP_HOME ${LIBS}/yaml-cpp CACHE INTERNAL "yaml-cpp 主目录")
set(YAML_CPP_INCLUDE_DIR ${LIBS}/yaml-cpp/include CACHE INTERNAL "yaml-cpp include 目录")
set(YAML_CPP_LIB_DIR ${LIBS}/yaml-cpp-${CMAKE_BUILD_TYPE} CACHE INTERNAL "yaml-cpp lib 目录")
