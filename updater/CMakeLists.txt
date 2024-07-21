project(h-wallpaper-updater)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})

#########
# 编译器 #
#########

set_msvc_utf_8()

#########
# 头文件 #
#########

include_directories(
        "include"
)

###########
# 链接目录 #
###########

if (DEBUG)
    link_directories(${OPENSSL_LIB_HOME_DIR}/VC/x64/MTd)
else ()
    link_directories(${OPENSSL_LIB_HOME_DIR}/VC/x64/MT)
endif ()

##########
# 主程序 #
##########

aux_source_directory("src" SRCS)

add_executable(${PROJECT_NAME}
        ${SRCS}
)

target_link_libraries(${PROJECT_NAME}
        $<TARGET_LINKER_FILE:utils>

        # sys
        runtimeobject

        # jsoncpp
        jsoncpp_static

        # openssl
        libssl
        libcrypto
)

#########
# 动态库 #
#########

#file(GLOB DLLS
#)
#
#foreach (DLL ${DLLS})
#    # 复制dll
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${OUT_DIR}"
#    )
#endforeach ()

########
# 安装 #
########

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(FILES ${DLLS} DESTINATION .)
