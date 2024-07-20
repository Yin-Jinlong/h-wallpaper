function(install_licenses NAME FILES)
    install(FILES
            ${FILES}
            DESTINATION licenses/${NAME}
    )
endfunction()
