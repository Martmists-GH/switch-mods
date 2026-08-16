---@class event<T>
---@field type string
---@field before T
---@field after T

---@class ffi : { [string]: fun(...):lightuserdata }
---@field int fun(arg: lightuserdata): number
---@field float fun(arg: lightuserdata): number
---@field string fun(arg: lightuserdata): string
---@field getptr fun(arg: string): lightuserdata
---@field readptr fun(arg: lightuserdata): lightuserdata

---@class json : userdata
---@field [string] int|number|string|json|(int|number|string|json)[]
