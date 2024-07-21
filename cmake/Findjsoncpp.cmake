set(JSONCPP_SRC_DIR "")
set(JSONCPP_HOME "")
set(JSONCPP_INCLUDE_DIR "")
set(JSONCPP_BIN_DIR "")
set(JSONCPP_LIB_DIR "")

set_var_dir(JSONCPP_SRC_DIR ${DOWNLOADS_DIR}/jsoncpp)
set_var_dir(JSONCPP_HOME ${LIBS_DIR}/jsoncpp-${CMAKE_BUILD_TYPE})
set_var_dir(JSONCPP_INCLUDE_DIR ${LIBS_DIR}/jsoncpp-${CMAKE_BUILD_TYPE}/include)
set_var_dir(JSONCPP_BIN_DIR ${LIBS_DIR}/jsoncpp-${CMAKE_BUILD_TYPE}/bin)
set_var_dir(JSONCPP_LIB_DIR ${LIBS_DIR}/jsoncpp-${CMAKE_BUILD_TYPE}/lib)

include_directories(${JSONCPP_INCLUDE_DIR})
link_directories(${JSONCPP_LIB_DIR})
