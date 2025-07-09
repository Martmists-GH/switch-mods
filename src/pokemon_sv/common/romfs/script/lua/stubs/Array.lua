---@class Array<T>
---@field length number
Array = {}

---@return T
function Array:shift() return nil end

---@param arg T
function Array:push(arg) end

---@generic T
---@param t table
---@param s number
---@return Array<T>
function _hx_tab_array(t, s) return nil end
