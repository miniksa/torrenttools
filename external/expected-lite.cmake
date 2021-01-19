
find_package(expected-lite QUIET)
if (expected-lite_FOUND)
    log_found(expected-lite)
else()
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/expected-lite)
        message(STATUS "expected-lite source directory found")
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/expected-lite)
        set(expected-lite_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/expected-lite)
    else()
        log_fetch(expected-lite)
        FetchContent_Declare(
                expected-lite
                GIT_REPOSITORY https://github.com/martinmoene/expected-lite.git
                GIT_TAG        master
        )
        FetchContent_MakeAvailable(expected-lite)
    endif()
endif()

if(IS_DIRECTORY "${expected-lite_SOURCE_DIR}")
    set_property(DIRECTORY ${expected-lite_SOURCE_DIR} PROPERTY EXCLUDE_FROM_ALL YES)
endif()