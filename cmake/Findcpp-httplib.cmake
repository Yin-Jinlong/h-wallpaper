set(CPP_HTTPLIB_HOME "")
set(CPP_HTTPLIB_INCLUDE_DIR "")

set_var_dir(CPP_HTTPLIB_HOME ${LIBS_DIR}/cpp-httplib)
set_var_dir(CPP_HTTPLIB_INCLUDE_DIR ${LIBS_DIR}/cpp-httplib/include)

include_directories(${CPP_HTTPLIB_INCLUDE_DIR})
