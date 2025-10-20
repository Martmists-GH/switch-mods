# Hakkun
set(HAKKUN_DIR "${PROJECT_SOURCE_DIR}/lib/hakkun")
set(HAKKUN_LIB_DIR "${HAKKUN_DIR}/hakkun")
set(HAKKUN_ADDON_DIR "${HAKKUN_DIR}/addons")

# Switch Tools
file(TO_CMAKE_PATH "$ENV{SWITCHTOOLS}" SWITCHTOOLS)
file(TO_CMAKE_PATH "$ENV{DEVKITPRO}" DEVKITPRO)

if(NOT IS_DIRECTORY ${SWITCHTOOLS})
    if(NOT IS_DIRECTORY ${DEVKITPRO})
        message(FATAL_ERROR "Please install devkitA64 or set SWITCHTOOLS in your environment.")
    else()
        set(SWITCHTOOLS ${DEVKITPRO}/tools/bin)
    endif()
endif()

# Python
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/.python_env")
    message("Setting up Python 3 venv")
    execute_process(
            COMMAND python3.12 -m venv .python_env
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
    message("Installing required packages")
    execute_process(
            COMMAND .python_env/bin/python -m pip install lz4 jsonschema jinja2 websockets
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif ()
set(python "${PROJECT_SOURCE_DIR}/.python_env/bin/python")

add_custom_target(
    "logger"
    COMMAND ${python} "${PROJECT_SOURCE_DIR}/tools/logger_server.py"
    USES_TERMINAL
)

# C++ STL
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/lib/std")
    message("Compiling STL")
    execute_process(
            COMMAND ${python} tools/setup_libcxx.py
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
endif ()

# Sail
if (NOT EXISTS "${PROJECT_SOURCE_DIR}/tools/sail")
    message ("Building sail")
    execute_process(
        COMMAND ${python} ${PROJECT_SOURCE_DIR}/tools/setup_sail.py
    )
endif ()
set(sail ${PROJECT_SOURCE_DIR}/tools/sail)
function(create_sail variant module_folder)
    file(GLOB SAIL_FILES CONFIGURE_DEPENDS ${module_folder}/*.sym)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/fakesymbols.so"
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/symboldb.o"
        COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}"
        COMMAND ${sail} ${module_folder}/ModuleList.sym ${module_folder}/VersionList.sym "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}" ${CMAKE_C_COMPILER} 0 E "${module_folder}/game"
        DEPENDS ${SAIL_FILES} ${variant}_sail_prepare
    )
    add_custom_target(
        ${variant}_sail
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/fakesymbols.so" "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/symboldb.o"
    )
    add_dependencies(${variant} ${variant}_sail)
endfunction()

# npdmtool
find_program(npdmtool npdmtool ${SWITCHTOOLS})
function(create_npdm variant title_id)
    add_custom_target(
        ${variant}_npdm
        COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out"
        COMMAND ${npdmtool} "${CMAKE_CURRENT_BINARY_DIR}/${variant}.npdm.json" "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/main.npdm"
        DEPENDS ${variant}
        VERBATIM
    )
endfunction()

# elf2nso
find_program(elf2nso elf2nso ${SWITCHTOOLS})
function(create_nso variant subsdk)
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/${variant}_out/${subsdk}
        COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out"
        COMMAND ${elf2nso} $<TARGET_FILE:${variant}> "${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/${subsdk}"
        DEPENDS ${variant}
        VERBATIM
    )

    # Add the respective NSO target and set the required linker flags for the original target.
    add_custom_target(${variant}_nso SOURCES ${CMAKE_BINARY_DIR}/${variant}_out/${subsdk})
endfunction()

# bin2s
find_program(bin2s bin2s ${SWITCHTOOLS})
function(embed_file project filepath filename)
    set(SPATH ${CMAKE_CURRENT_BINARY_DIR}/${filename}_shader.S)

    add_custom_command(
        OUTPUT ${SPATH}
        COMMAND ${bin2s} ${filepath} --header ${CMAKE_CURRENT_BINARY_DIR}/embed_${filename}.h -a 0x1000 > ${SPATH}
        DEPENDS ${filepath}
    )
    set_source_files_properties(${SPATH} PROPERTIES GENERATED TRUE)
    target_sources(${project} PRIVATE ${SPATH})
    target_include_directories(${project} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
