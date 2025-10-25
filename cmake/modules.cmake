add_custom_target(all_modules)
add_custom_target(all_modules_zips)

function(create_mod name folder)
    cmake_parse_arguments(MODULE_ARGS "WITH_VARIANTS;WITH_OLD_SDK;WITH_NVN;WITH_DEBUGRENDERER;WITH_IMGUI;WITH_HEAPSOURCE_BSS;WITH_HEAPSOURCE_DYNAMIC;WITH_EXPHEAP;SDK_PAST_1900" "HOOK_POOL_SIZE;BSS_HEAP_SIZE;TITLE_ID;GAME_TITLE" "INCLUDE;SOURCE;SOURCE_SHALLOW;LIBRARIES" ${ARGN})

    set(ALL_INCLUDE)
    set(ALL_SOURCE)
    set(ALL_DEFINES)

    set(EXTRA_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/common/src" "${CMAKE_CURRENT_SOURCE_DIR}/${folder}/src" "${PROJECT_SOURCE_DIR}/src/common/src")

    # Hakkun
    list(APPEND ALL_INCLUDE "${HAKKUN_LIB_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/common/include" "${CMAKE_CURRENT_SOURCE_DIR}/${folder}/include")
    list(APPEND ALL_SOURCE
        "${HAKKUN_LIB_DIR}/src/hk/diag/diag.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/diag/ipclogger.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/diag/ResultName.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/hook/MapUtil.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/hook/Trampoline.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/init/mod0.S"
        "${HAKKUN_LIB_DIR}/src/hk/init/moduleEntry.S"
        "${HAKKUN_LIB_DIR}/src/hk/init/module.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/os/Libcxx.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/os/Thread.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/ro/RoUtil.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/ro/RoModule.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/sail/detail.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/sail/init.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/util/Context.cpp"
        "${HAKKUN_LIB_DIR}/src/rtld/RoModule.cpp"
        "${HAKKUN_LIB_DIR}/src/hk/svc/api.aarch64.S"
    )

    # Hakkun addons
    if (${MODULE_ARGS_WITH_IMGUI})
        list(APPEND ALL_INCLUDE
            ${PROJECT_SOURCE_DIR}/lib/imgui
        )
        list(APPEND ALL_SOURCE
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui.cpp"
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui_draw.cpp"
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui_tables.cpp"
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp"
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui_demo.cpp"
            "${PROJECT_SOURCE_DIR}/lib/imgui/imgui.cpp"

            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/nvn/nvn_CppFuncPtrImpl.cpp"
        )
        list(APPEND ALL_DEFINES IMGUI_ENABLED IMGUI_DISABLE_DEFAULT_ALLOCATORS IMGUI_USER_CONFIG="${CMAKE_SOURCE_DIR}/src/common/src/imgui_backend/nvn_imgui_config.h")
    endif ()

    if (${MODULE_ARGS_WITH_NVN})
        list(APPEND ALL_INCLUDE
            "${HAKKUN_ADDON_DIR}/Nvn/include"
            "${HAKKUN_ADDON_DIR}/Nvn/include/hk"
        )
        list(APPEND ALL_SOURCE
            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/nvn/nvn_CppFuncPtrImpl.cpp"
            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/gfx/ShaderImpl.cpp"
            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/gfx/TextureImpl.cpp"
            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/gfx/UboImpl.cpp"
            "${HAKKUN_ADDON_DIR}/Nvn/src/hk/gfx/NvnBootstrapOverride.cpp"
        )
        list(APPEND ALL_DEFINES HK_ADDON_Nvn)
    endif ()

    if (${MODULE_ARGS_WITH_DEBUGRENDERER})
        error("WITH_DEBUGRENDERER not implemented!")
    endif ()

    if (${MODULE_ARGS_WITH_OLD_SDK})
        error("WITH_OLD_SDK not implemented!")
    endif ()

    if (${MODULE_ARGS_WITH_HEAPSOURCE_BSS})
        set(MODULE_ARGS_WITH_EXPHEAP TRUE)
        list(APPEND ALL_INCLUDE
            "${HAKKUN_ADDON_DIR}/HeapSourceBss/include"
        )
        list(APPEND ALL_SOURCE
            "${HAKKUN_ADDON_DIR}/HeapSourceBss/src/hk/mem/BssHeap.cpp"
            "${HAKKUN_ADDON_DIR}/HeapSourceBss/src/hk/mem/BssHeapWrappers.cpp"
        )
        list(APPEND ALL_DEFINES HK_ADDON_HeapSourceBss)
    endif ()

    if (${MODULE_ARGS_WITH_HEAPSOURCE_DYNAMIC})
        if (${MODULE_ARGS_WITH_HEAPSOURCE_BSS})
            error("WITH_HEAPSOURCE_BSS and WITH_HEAPSOURCE_DYNAMIC are not compatible!")
        endif ()
        list(APPEND ALL_SOURCE
            "${HAKKUN_ADDON_DIR}/HeapSourceDynamic/src/hk/mem/DynamicHeapWrappers.cpp"
        )
        list(APPEND ALL_DEFINES HK_ADDON_HeapSourceDynamic)
    endif ()

    if (${MODULE_ARGS_WITH_EXPHEAP})
        list(APPEND ALL_INCLUDE
            "${HAKKUN_ADDON_DIR}/ExpHeap/include"
            "${HAKKUN_ADDON_DIR}/ExpHeap/src"
        )
        list(APPEND ALL_SOURCE
            "${HAKKUN_ADDON_DIR}/ExpHeap/src/ams/lmem/lmem_common.cpp"
            "${HAKKUN_ADDON_DIR}/ExpHeap/src/ams/lmem/lmem_exp_heap.cpp"
            "${HAKKUN_ADDON_DIR}/ExpHeap/src/ams/lmem/impl/lmem_impl_common_heap.cpp"
            "${HAKKUN_ADDON_DIR}/ExpHeap/src/ams/lmem/impl/lmem_impl_exp_heap.cpp"
            "${HAKKUN_ADDON_DIR}/ExpHeap/src/hk/mem/ExpHeap.cpp"
        )
        list(APPEND ALL_DEFINES HK_ADDON_HeapSourceBss)
    endif ()

    if (${MODULE_ARGS_SDK_PAST_1900})
        list(APPEND ALL_DEFINES __RTLD_PAST_19XX__)
    endif ()

    # Defaults
    if (NOT DEFINED MODULE_ARGS_HOOK_POOL_SIZE)
        set(MODULE_ARGS_HOOK_POOL_SIZE 0x40)
    endif ()
    if (NOT DEFINED MODULE_ARGS_BSS_HEAP_SIZE)
        set(MODULE_ARGS_BSS_HEAP_SIZE 0x4000000)
    endif ()
    list(APPEND ALL_DEFINES HK_HOOK_TRAMPOLINE_POOL_SIZE=${MODULE_ARGS_HOOK_POOL_SIZE} HAKKUN_BSS_HEAP_SIZE=${MODULE_ARGS_BSS_HEAP_SIZE})

    # Handle paths
    foreach(include ${EXTRA_PATHS} ${MODULE_ARGS_INCLUDE})
        list(APPEND ALL_INCLUDE ${include})
    endforeach()
    foreach(source ${EXTRA_PATHS} ${MODULE_ARGS_SOURCE})
        file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${source}/*.c ${source}/*.cpp ${source}/*.s)
        list(APPEND ALL_INCLUDE ${source})
        list(APPEND ALL_SOURCE ${SOURCE_FILES})
    endforeach()
    foreach(source ${MODULE_ARGS_SOURCE_SHALLOW})
        file(GLOB SOURCE_FILES CONFIGURE_DEPENDS ${source}/*.c ${source}/*.cpp ${source}/*.s)
        list(APPEND ALL_INCLUDE ${source})
        list(APPEND ALL_SOURCE ${SOURCE_FILES})
    endforeach()

    # FIXME: This won't always regenerate files???
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}/lang/strings.cpp"
            COMMAND ${python} "${CMAKE_SOURCE_DIR}/tools/lang_generate_strings.py" "${CMAKE_CURRENT_BINARY_DIR}/${name}/lang" "${CMAKE_CURRENT_SOURCE_DIR}/common/lang" "${CMAKE_CURRENT_SOURCE_DIR}/${folder}/lang" "${PROJECT_SOURCE_DIR}/src/common/lang"
            #        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/common/lang" "${CMAKE_CURRENT_SOURCE_DIR}/${PARENT_FOLDER}/lang" "${PROJECT_SOURCE_DIR}/src/common/lang"
    )
    list(APPEND ALL_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}/${name}/lang")
    list(APPEND ALL_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${name}/lang/strings.cpp")

    # Set up targets
    add_custom_target(${name})
    add_dependencies(all_modules ${name})

    set_property(
            TARGET ${name}
            PROPERTY MODULE_INCLUDE ${ALL_INCLUDE}
    )
    set_property(
            TARGET ${name}
            PROPERTY MODULE_SOURCE ${ALL_SOURCE}
    )
    set_property(
            TARGET ${name}
            PROPERTY MAIN_DIRECTORY ${folder}
    )
    set_property(
            TARGET ${name}
            PROPERTY DEFINES ${ALL_DEFINES}
    )
    set_property(
            TARGET ${name}
            PROPERTY FOLDER ${folder}
    )

    create_releases_main(${name})

    if (NOT ${MODULE_ARGS_WITH_VARIANTS})
        create_mod_variant(${name} "main" "${MODULE_ARGS_TITLE_ID}" "${MODULE_ARGS_GAME_TITLE}")
    endif ()
endfunction()


function(create_mod_variant module variant title_id game)
    cmake_parse_arguments(VARIANT_ARGS "" "NPDM_TEMPLATE" "INCLUDE;SOURCE" ${ARGN})

    if (NOT DEFINED VARIANT_ARGS_NPDM_TEMPLATE)
        if ((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
            set(VARIANT_ARGS_NPDM_TEMPLATE ${PROJECT_SOURCE_DIR}/build-files/npdm-debug.json.template)
        else ()
            set(VARIANT_ARGS_NPDM_TEMPLATE ${PROJECT_SOURCE_DIR}/build-files/npdm.json.template)
        endif ()
    endif()

    if (NOT TARGET ${module})
        message(FATAL_ERROR "Mod ${module} not found")
    endif()

    set(ALL_INCLUDE)
    foreach(include ${VARIANT_ARGS_INCLUDE})
        list(APPEND ALL_INCLUDE ${include})
    endforeach()

    set(ALL_SOURCE)
    foreach(source ${VARIANT_ARGS_SOURCE})
        file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ${source}/*.c ${source}/*.cpp ${source}/*.asm)
        list(APPEND ALL_SOURCE ${SOURCE_FILES})
    endforeach()

    if ("${variant}" STREQUAL "")
        set(variant ${module})
    else ()
        set(variant ${module}_${variant})
    endif ()

    get_target_property(PARENT_INCLUDE ${module} MODULE_INCLUDE)
    get_target_property(PARENT_SOURCE ${module} MODULE_SOURCE)
    get_target_property(PARENT_POOL_SIZE ${module} HOOK_POOL_SIZE)
    get_target_property(PARENT_DEFINES ${module} DEFINES)
    get_target_property(PARENT_FOLDER ${module} FOLDER)

    add_executable(${variant} ${ALL_SOURCE} ${PARENT_SOURCE})
    target_include_directories(${variant} PUBLIC ${ALL_INCLUDE} ${PARENT_INCLUDE})

    string(LENGTH ${module} MODULE_NAME_LEN)
    target_compile_definitions(
        ${variant}
        PUBLIC
        MODULE_NAME=${module}
        MODULE_NAME_LEN=${MODULE_NAME_LEN}
        HK_TITLE_ID=0x${title_id}
        LOGGER_IP="${LOGGER_IP}"
        LOGGER_PORT=${LOGGER_PORT}
        ${PARENT_DEFINES}
    )
#    add_dependencies(${variant} ${variant}_strings)

    string(TOLOWER "0x${title_id}" CONFIG_TITLE_ID)

    configure_file(
        ${VARIANT_ARGS_NPDM_TEMPLATE} ${CMAKE_CURRENT_BINARY_DIR}/${variant}.npdm.json
    )

    if (NOT DEFINED ${variant}_npdm)
        create_npdm(${variant} ${title_id})
    endif()

    add_custom_target(
        ${variant}_sail_prepare
        COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/sail_syms_${variant}
        COMMAND bash -c "cp -r '${PROJECT_SOURCE_DIR}/src/common/sym' '${CMAKE_CURRENT_BINARY_DIR}/sail_syms_${variant}'"
        COMMAND bash -c "cp -r '${CMAKE_CURRENT_SOURCE_DIR}/common/sym' '${CMAKE_CURRENT_BINARY_DIR}/sail_syms_${variant}'"
    )
    create_sail(${variant} "${CMAKE_CURRENT_BINARY_DIR}/sail_syms_${variant}/sym")

    target_link_libraries(
        ${variant} PUBLIC

        "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/datablocks.o"
        "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/fakesymbols.so"
        "${CMAKE_CURRENT_BINARY_DIR}/sail_output_${variant}/symboldb.o"
        nnsdk
        nlohmann_json
        MbedTLS::mbedtls

        -nodefaultlibs
        -nostartfiles
        -Wno-unused-command-line-argument

        -Wl,--no-demangle
        -Wl,--gc-sections
        "-T${PROJECT_SOURCE_DIR}/lib/hakkun/data/link.aarch64.ld"
        "-T${PROJECT_SOURCE_DIR}/lib/hakkun/data/misc.ld"
        -Wl,-init=__module_entry__
        -Wl,--pie
        -Wl,--export-dynamic-symbol=_ZN2nn2ro6detail15g_pAutoLoadListE
        -Wl,--unresolved-symbols=report-all
    )

    create_nso(${variant} "subsdk9")

    add_custom_target(${variant}_all ALL DEPENDS ${variant}_npdm ${variant}_sail ${variant}_nso)
    add_dependencies(${module} ${variant}_all)

    create_releases(${module} ${game} ${variant} ${title_id})
endfunction()


function(create_releases_main module)
    add_custom_target(${module}_release_all)
    add_custom_target(${module}_release_variants)
    add_custom_target(${module}_zip_all)
    add_dependencies(all_modules_zips ${module}_zip_all)

    function(create_release target with_ftp copy_path)
        add_custom_target(${module}_release_${target} DEPENDS ${module}_release_variants)
        add_custom_command(
            TARGET ${module}_release_${target} POST_BUILD
            COMMAND mkdir -p "${CMAKE_BINARY_DIR}/${module}_releases/${target}/"
            COMMAND bash -c 'cp -r ${CMAKE_BINARY_DIR}/${module}_*_releases/${target}/* ${CMAKE_BINARY_DIR}/${module}_releases/${target}/'
        )

        add_dependencies(${module}_release_all ${module}_release_${target})

        add_custom_target(${module}_zip_${target} DEPENDS ${module}_release_${target})
        add_custom_command(
            TARGET ${module}_zip_${target} POST_BUILD
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/zips/
            COMMAND zip -r "${CMAKE_BINARY_DIR}/zips/${module}_${target}.zip" *
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${module}_releases/${target}/"
        )

        add_dependencies(${module}_zip_all ${module}_zip_${target})

        if (${with_ftp})
            add_custom_target(
                ${module}_ftp_${target}
                DEPENDS ${module}_release_${target}
                COMMAND ${python} "${PROJECT_SOURCE_DIR}/tools/send_patch.py" ${FTP_IP} ${FTP_PORT} ${FTP_USER} ${FTP_PASS} "${CMAKE_BINARY_DIR}/${module}_releases/${target}/"
                USES_TERMINAL
            )
        endif()

        if (NOT "${copy_path}" STREQUAL "")
            add_custom_target(
                ${module}_copy_${target}
                DEPENDS ${module}_release_${target}
                COMMAND cp -r "${CMAKE_BINARY_DIR}/${module}_releases/${target}/*" "${copy_path}"
            )
        endif()
    endfunction()

    create_release(atmosphere TRUE "")
    create_release(modmanager TRUE "")
    create_release(ryujinx FALSE "$ENV{HOME}/.config/Ryujinx/")
    create_release(yuzu FALSE "$ENV{HOME}/.local/share/suyu/")
endfunction()

function(create_releases module game_name variant title_id)
    add_custom_target(${variant}_release_all)
    add_dependencies(${module}_release_variants ${variant}_release_all)

    function(create_release target folder)
        add_custom_target(${variant}_release_${target} DEPENDS ${variant}_all)
        add_custom_command(
            TARGET ${variant}_release_${target} POST_BUILD
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/exefs
            COMMAND bash -c \"cp -r ${CMAKE_CURRENT_BINARY_DIR}/${variant}_out/* ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/exefs\"
        )

        get_target_property(PARENT_FOLDER ${module} MAIN_DIRECTORY)
        add_custom_command(
            TARGET ${variant}_release_${target} POST_BUILD
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs
            # Copy common/game/mod romfs into one folder
            COMMAND bash -c \"shopt -s dotglob && cp -r -t ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs ${CMAKE_SOURCE_DIR}/src/common/romfs/* \"
            COMMAND bash -c \"shopt -s dotglob && cp -r -t ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs ${CMAKE_CURRENT_SOURCE_DIR}/common/romfs/* \"
            COMMAND bash -c \"shopt -s dotglob && cp -r -t ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs ${CMAKE_CURRENT_SOURCE_DIR}/${PARENT_FOLDER}/romfs/* \"
            # Clear up .gitkeep files, if any. Touch in case none exist to prevent rm from failing this task.
            COMMAND bash -c \"touch ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs/.gitkeep && rm ${CMAKE_BINARY_DIR}/${variant}_releases/${target}/${folder}/romfs/.gitkeep \"
        )

        add_dependencies(${variant}_release_all ${variant}_release_${target})
    endfunction()

    create_release(atmosphere "atmosphere/contents/${title_id}/")
    create_release(modmanager "mods/${game_name}/${module}/contents/${title_id}/")
    create_release(ryujinx "mods/contents/${title_id}/${module}/")
    create_release(yuzu "load/${title_id}/${module}/")
endfunction()
