function(set_var_dir VAR DIR)
    if (NOT EXISTS ${DIR})
        message(FATAL_ERROR "目录 ${DIR} 不存在")
    endif ()
    set(${VAR} ${DIR} PARENT_SCOPE)
endfunction()
