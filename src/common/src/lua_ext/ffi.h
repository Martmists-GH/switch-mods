#pragma once
#include <lua.hpp>
#include <cstdint>

struct CallFrame {
    uint64_t int_args[8];
    double fp_args[8];
    uint64_t* stack_args;
    size_t stack_count;
};

struct CallResult {
    uint64_t int_res;
    double fp_res;
};

CallResult invoke_wrapper(void* funcPtr, const CallFrame* frame);

void luaopen_ffi(lua_State* state);
