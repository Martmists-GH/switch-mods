set(TARGET_TRIPLE "aarch64-none-elf")
set(MARCH "armv8-a")
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION "NX/Clang")

# FIXME: Apparently everything breaks on clang 21 onwards?

find_program(CLANG_PATH NAMES clang-20 clang)
cmake_path(SET ${CLANG_PATH} ${CLANG_PATH})
cmake_path(GET ${CLANG_PATH} PARENT_PATH CLANG_DIR)

message("Found clang: ${CLANG_PATH}/clang")

set(CMAKE_ASM_COMPILER "${CLANG_DIR}/clang")
set(CMAKE_C_COMPILER "${CLANG_DIR}/clang")
set(CMAKE_CXX_COMPILER "${CLANG_DIR}/clang++")
set(CMAKE_LINKER "ld.lld")
set(CMAKE_EXECUTABLE_SUFFIX ".nss")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_definitions(
    -D_POSIX_C_SOURCE=200809L
    -D_LIBUNWIND_IS_BAREMETAL
    -D_LIBCPP_HAS_THREAD_API_PTHREAD
    -D_GNU_SOURCE
    -D__SWITCH__=1
    -DNNSDK
)

set(LIBSTD_PATH "${CMAKE_SOURCE_DIR}/lib/std")

include_directories(
    SYSTEM
    ${LIBSTD_PATH}/llvm-project/build/include/c++/v1
    ${LIBSTD_PATH}/llvm-project/libunwind/include
    ${LIBSTD_PATH}/llvm-project/libc/include
#    ${LIBSTD_PATH}/llvm-project/libcxx/include
    ${LIBSTD_PATH}/musl/include
    ${LIBSTD_PATH}/musl/obj/include
    ${LIBSTD_PATH}/musl/arch/generic
    ${LIBSTD_PATH}/musl/arch/aarch64
)

set(DEFAULTLIBS
    ${LIBSTD_PATH}/libc.a
    ${LIBSTD_PATH}/libc++.a
    ${LIBSTD_PATH}/libc++abi.a
    ${LIBSTD_PATH}/libm.a
    ${LIBSTD_PATH}/libunwind.a
    ${LIBSTD_PATH}/libclang_rt.builtins-aarch64.a
)

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL Debug)
    add_definitions(-DHK_RELEASE)
    if (${CMAKE_BUILD_TYPE} STREQUAL RelWithDebInfo)
        add_definitions(-DHK_RELEASE_DEBINFO)
    endif()
    set(DEBUG FALSE)

    set(ARCH "${ARCH} -O3 -ffast-math -flto")
else()
    add_definitions(-DHK_DEBUG)
    set(DEBUG TRUE)

    set(ARCH "${ARCH} -O2 -gdwarf-4")
endif()

set(ARCH "--target=${TARGET_TRIPLE} -march=${MARCH} -mtune=cortex-a57 -fPIC -nodefaultlibs ${ARCH}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH} -Werror=return-type -Wno-invalid-offsetof")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH} ${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -fno-exceptions -frtti")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${ARCH}")
list(JOIN DEFAULTLIBS " " DEFAULTLIBS_STR)

set(CMAKE_C_COMPILER_WORKS true)
set(CMAKE_CXX_COMPILER_WORKS true)
set(CMAKE_ASM_COMPILER_WORKS true)

# Cmake compile bullshit
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DEFAULTLIBS_STR}")
