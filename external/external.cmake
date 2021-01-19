cmake_minimum_required(VERSION 3.14)

include(FetchContent)

function(log_found library)
    message(STATUS "Local installation of dependency ${library} found.")
endfunction()

function(log_dir_found library)
    message(STATUS "Source directory for dependency ${library} found.")
endfunction()

function(log_fetch library)
    message(STATUS "Fetching dependency ${library}...")
endfunction()

if (TORRENTOOLS_BUILD_TESTS)
    include(${CMAKE_CURRENT_LIST_DIR}/Catch2.cmake)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/gsl-lite.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/CLI11.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/expected-lite.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/nlohmann_json.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/re2.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/yaml-cpp.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/bencode.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/termcontrol.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cliprogress.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dottorrent.cmake)

