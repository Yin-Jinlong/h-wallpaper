project(h-wallpaper-utils)

########
# 变量 #
########

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})

#########
# 编译器 #
#########

set_msvc_utf_8()

###########
# 外部依赖 #
###########


#########
# 头文件 #
#########

include_directories(
        "include"
)

###########
# 链接目录 #
###########


##########
# 主程序 #
##########

aux_source_directory("src" SRCS)

add_library(utils SHARED ${SRCS})

target_link_libraries(utils
        # win icu
        icuuc
        icuin
)

#########
# 动态库 #
#########

#file(GLOB DLLS
#)
#
#foreach (DLL ${DLLS})
#    # 复制dll
#    add_custom_command(TARGET utils POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" "${OUT_DIR}"
#    )
#endforeach ()

########
# 安装 #
########

install(TARGETS utils RUNTIME DESTINATION .)
install(FILES ${DLLS} DESTINATION .)

########
# 协议 #
########

