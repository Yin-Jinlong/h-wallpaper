include(FetchContent)

function(fetch_lib NAME URL HASH)
    FetchContent_Declare(${NAME}
            URL ${URL}
            URL_HASH SHA256=${HASH}
            SOURCE_DIR ${LIBS}/${NAME}
            BINARY_DIR ${LIBS}/${NAME}-build
    )

    FetchContent_GetProperties(${NAME})

    if (NOT ${NAME}_POPULATED)
        message(STATUS "Fetching ${NAME}...")
        FetchContent_Populate(${NAME})
        message(STATUS "${NAME} source dir: ${${NAME}_SOURCE_DIR}")
        message(STATUS "${NAME} binary dir: ${${NAME}_BINARY_DIR}")
    endif ()
endfunction()