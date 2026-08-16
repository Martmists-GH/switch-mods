#include "ffi.h"
#undef cast
#include "hk/ro/RoUtil.h"
#include <vector>

#include "hk/sail/detail.h"
#include "logger/logger.h"

static int ffi_int(lua_State* L) {
    if (lua_isnil(L, 1)) {
        lua_pushinteger(L, 0);
        return 1;
    }

    if (lua_islightuserdata(L, 1)) {
        auto raw = reinterpret_cast<uintptr_t>(lua_touserdata(L, 1));
        lua_pushinteger(L, static_cast<lua_Integer>(raw));
        return 1;
    }

    return luaL_error(L, "ffi.int: expected lightuserdata or nil");
}

static int ffi_float(lua_State* L) {
    if (lua_isnil(L, 1)) {
        lua_pushnumber(L, 0);
        return 1;
    }

    if (lua_islightuserdata(L, 1)) {
        auto raw = reinterpret_cast<uintptr_t>(lua_touserdata(L, 1));
        lua_pushnumber(L, static_cast<lua_Number>(raw));
        return 1;
    }

    return luaL_error(L, "ffi.float: expected lightuserdata or nil");
}

static int ffi_string(lua_State* L) {
    if (lua_islightuserdata(L, 1)) {
        auto raw = static_cast<char*>(lua_touserdata(L, 1));
        lua_pushstring(L, raw);
        return 1;
    }

    return luaL_error(L, "ffi.string: expected lightuserdata");
}

static int ffi_getptr(lua_State* L) {
    auto symbol = luaL_checkstring(L, 1);
    auto symbol_addr = hk::ro::lookupSymbol(symbol);
    if (symbol_addr == 0) {
        symbol_addr = hk::sail::lookupSymbolFromDb<false, char>(symbol, false);

        if (symbol_addr == 0) {
            return luaL_error(L, "Could not resolve symbol: %s", symbol);
        }
    }
    lua_pushinteger(L, symbol_addr);
    return 1;
}

static int ffi_readptr(lua_State* L) {
    if (lua_islightuserdata(L, 1)) {
        auto raw = static_cast<void**>(lua_touserdata(L, 1));
        lua_pushlightuserdata(L, *raw);
        return 1;
    }

    if (lua_isinteger(L, 1)) {
        auto raw = reinterpret_cast<void**>(lua_tointeger(L, 1));
        lua_pushlightuserdata(L, *raw);
        return 1;
    }

    return luaL_error(L, "ffi.readptr: expected lightuserdata or integer");
}

static luaL_Reg ffi_funcs[] = {
    {"int", ffi_int},
    {"float", ffi_float},
    {"string", ffi_string},
    {"getptr", ffi_getptr},
    {"readptr", ffi_readptr},
    {nullptr},
};

CallResult invoke_wrapper(void* funcPtr, const CallFrame* frame) {
    CallResult result;
    auto func = reinterpret_cast<uint64_t>(funcPtr);
    auto i_args = frame->int_args;
    auto f_args = frame->fp_args;
    auto s_args = frame->stack_args;
    auto s_count = frame->stack_count;

    __asm__ volatile(R"(
        stp x29, x30, [sp, #-32]!
        mov x29, sp
        str %[func], [x29, #16]
        str %[res], [x29, #24]
        mov x9, %[s_count]
        cbz x9, 3f
        lsl x10, x9, #3
        tst x9, #1
        beq 1f
        add x10, x10, #8
    1:
        sub sp, sp, x10
        mov x11, %[s_args]
        mov x12, sp
    2:
        ldr x13, [x11], #8
        str x13, [x12], #8
        subs x9, x9, #1
        b.ne 2b
    3:
        mov x9, %[f_args]
        ldp d0, d1, [x9, #0]
        ldp d2, d3, [x9, #16]
        ldp d4, d5, [x9, #32]
        ldp d6, d7, [x9, #48]
        mov x9, %[i_args]
        ldp x0, x1, [x9, #0]
        ldp x2, x3, [x9, #16]
        ldp x4, x5, [x9, #32]
        ldp x6, x7, [x9, #48]
        ldr x9, [x29, #16]
        blr x9
        ldr x9, [x29, #24]
        str x0, [x9, #0]
        str d0, [x9, #8]
        mov sp, x29
        ldp x29, x30, [sp], #32
    )" ::
        [func] "r"(func),
        [i_args] "r"(i_args),
        [f_args] "r"(f_args),
        [s_args] "r"(s_args),
        [s_count] "r"(s_count),
        [res] "r"(&result)
        :
        "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7",
        "x9", "x10", "x11", "x12", "x13",
        "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
        "cc", "memory"
    );
    return result;
}

static int ffi_invoke(lua_State* L) {
    auto func_ptr = lua_touserdata(L, lua_upvalueindex(1));
    if (!func_ptr) {
        return luaL_error(L, "Attempted to invoke null symbol pointer.");
    }

    int argc = lua_gettop(L);

    CallFrame frame {};
    size_t int_reg_idx = 0;
    size_t fp_reg_idx = 0;
    std::vector<uint64_t> stack_overflow_args;

    for (int i = 1; i <= argc; ++i) {
        int type = lua_type(L, i);

        if (type == LUA_TNIL) {
            if (int_reg_idx < 8) {
                frame.int_args[int_reg_idx++] = 0;
            } else {
                stack_overflow_args.push_back(0);
            }
        } else if (type == LUA_TLIGHTUSERDATA || type == LUA_TUSERDATA) {
            auto val = static_cast<uint64_t*>(lua_touserdata(L, i));
            if (int_reg_idx < 8) {
                frame.int_args[int_reg_idx++] = *val;
            } else {
                stack_overflow_args.push_back(*val);
            }
        } else if (type == LUA_TNUMBER) {
            if (lua_isinteger(L, i)) {
                auto val = static_cast<uint64_t>(lua_tointeger(L, i));
                if (int_reg_idx < 8) {
                    frame.int_args[int_reg_idx++] = val;
                } else {
                    stack_overflow_args.push_back(val);
                }
            } else {
                double val = lua_tonumber(L, i);
                if (fp_reg_idx < 8) {
                    frame.fp_args[fp_reg_idx++] = val;
                } else {
                    uint64_t raw_bits;
                    std::memcpy(&raw_bits, &val, sizeof(double));
                    stack_overflow_args.push_back(raw_bits);
                }
            }
        } else if (type == LUA_TSTRING) {
            auto val = reinterpret_cast<uint64_t>(lua_tostring(L, i));
            if (int_reg_idx < 8) {
                frame.int_args[int_reg_idx++] = val;
            } else {
                stack_overflow_args.push_back(val);
            }
        } else if (type == LUA_TBOOLEAN) {
            auto val = lua_toboolean(L, i) ? 1 : 0;
            if (int_reg_idx < 8) {
                frame.int_args[int_reg_idx++] = val;
            } else {
                stack_overflow_args.push_back(val);
            }
        } else {
            return luaL_error(L, "Unsupported argument type at index %d", i);
        }
    }

    frame.stack_args = stack_overflow_args.empty() ? nullptr : stack_overflow_args.data();
    frame.stack_count = stack_overflow_args.size();

    auto res = invoke_wrapper(func_ptr, &frame);

    auto raw_return = res.int_res;
    if (raw_return == 0) {
        uint64_t fp_bits;
        std::memcpy(&fp_bits, &res.fp_res, sizeof(double));
        if (fp_bits != 0) {
            raw_return = fp_bits;
        }
    }

    if (raw_return == 0) {
        lua_pushnil(L);
    } else {
        lua_pushlightuserdata(L, reinterpret_cast<void*>(raw_return));
    }

    return 1;
}

static int ffi_index(lua_State* L) {
    auto symbol = luaL_checkstring(L, 2);

    auto symbol_addr = hk::ro::lookupSymbol(symbol);
    if (symbol_addr == 0) {
        symbol_addr = hk::sail::lookupSymbolFromDb<false, char>(symbol, false);

        if (symbol_addr == 0) {
            return luaL_error(L, "Could not resolve symbol: %s", symbol);
        }
    }

    lua_pushlightuserdata(L, (void*)symbol_addr);
    lua_pushcclosure(L, ffi_invoke, 1);

    lua_pushvalue(L, 2);
    lua_pushvalue(L, -2);
    lua_rawset(L, 1);

    return 1;
}

void luaopen_ffi(lua_State* L) {
    lua_newtable(L);

    luaL_setfuncs(L, ffi_funcs, 0);

    lua_newtable(L);
    lua_pushcfunction(L, ffi_index);
    lua_setfield(L, -2, "__index");

    lua_setmetatable(L, -2);
    lua_setglobal(L, "ffi");
}
