cmake_minimum_required(VERSION 3.21)
project(SwitchMods CXX C ASM)

set(CMAKE_CXX_STANDARD 23)

set(NN_SDK_MAJOR 13)
set(NN_SDK_MINOR 3)
set(NN_SDK_PATCH 13)
set(NN_SDK_TYPE "Release")

include(cmake/tools.cmake)
include(cmake/modules.cmake)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
    configure_file(build-files/config.cmake.template ${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
endif ()
include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)

add_subdirectory(lib/json)
add_subdirectory(lib/mbedtls)
add_subdirectory(lib/nnsdk)

file(GLOB GAMES LIST_DIRECTORIES true CONFIGURE_DEPENDS RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/*")
foreach (GAME ${GAMES})
    if (EXISTS "${CMAKE_SOURCE_DIR}/src/${GAME}/CMakeLists.txt")
        add_subdirectory("src/${GAME}")
    endif ()
endforeach ()

function(generate_run_configurations dir)
    get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)

    foreach(tgt IN LISTS targets)
        get_target_property(src_dir "${tgt}" SOURCE_DIR)
        set(TARGET_NAME "${tgt}")
        file(RELATIVE_PATH TARGET_FOLDER ${CMAKE_SOURCE_DIR} ${src_dir})
        if ("${TARGET_FOLDER}" STREQUAL "")
            set(TARGET_FOLDER "root")
        endif ()
        if (NOT EXISTS "${CMAKE_SOURCE_DIR}/.run/${tgt}.run.xml")
            configure_file("${CMAKE_SOURCE_DIR}/build-files/run_configuration.xml.template" "${CMAKE_SOURCE_DIR}/.run/${tgt}.run.xml")
        endif ()
    endforeach()

    get_property(subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(subdir IN LISTS subdirs)
        generate_run_configurations("${subdir}")
    endforeach()
endfunction()

generate_run_configurations("${CMAKE_SOURCE_DIR}")
