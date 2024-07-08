include(FetchContent)

function(fetch_lib NAME URL)
    FetchContent_Declare(${NAME}
            URL ${URL}
            SOURCE_DIR ${LIBS}/${NAME}
            BINARY_DIR ${LIBS}/${NAME}-${CMAKE_BUILD_TYPE}
    )

    FetchContent_MakeAvailable(${NAME})
    FetchContent_GetProperties(${NAME})

    if (NOT ${NAME}_POPULATED)
        message(STATUS "Fetching ${NAME}...")
        FetchContent_Populate(${NAME})
        message(STATUS "${NAME} source dir: ${${NAME}_SOURCE_DIR}")
        message(STATUS "${NAME} binary dir: ${${NAME}_BINARY_DIR}")
    endif ()
endfunction()

function(fetch_git_lib NAME URL TAG)

    FetchContent_Declare(${NAME}
            GIT_REPOSITORY ${URL}
            GIT_TAG ${TAG}
            GIT_SHALLOW TRUE
            SOURCE_DIR ${LIBS}/${NAME}
            BINARY_DIR ${LIBS}/${NAME}-${CMAKE_BUILD_TYPE}
    )

    FetchContent_MakeAvailable(${NAME})
    FetchContent_GetProperties(${NAME})

    if (NOT ${NAME}_POPULATED)
        message(STATUS "Fetching ${NAME}...")
        FetchContent_Populate(${NAME})
        message(STATUS "${NAME} source dir: ${${NAME}_SOURCE_DIR}")
        message(STATUS "${NAME} binary dir: ${${NAME}_BINARY_DIR}")
    endif ()
endfunction()