set(OPENSSL_HOME "")
set(OPENSSL_INCLUDE_DIR "")
set(OPENSSL_LIB_HOME_DIR "")

find_program(OPENSSL_EXE openssl)
if (NOT OPENSSL_EXE)
    message(FATAL_ERROR "openssl not found")
endif ()

set_var_dir(OPENSSL_HOME "${OPENSSL_EXE}/../..")
set_var_dir(OPENSSL_INCLUDE_DIR "${OPENSSL_HOME}/include")
set_var_dir(OPENSSL_LIB_HOME_DIR "${OPENSSL_HOME}/lib")

include_directories(${OPENSSL_INCLUDE_DIR})
