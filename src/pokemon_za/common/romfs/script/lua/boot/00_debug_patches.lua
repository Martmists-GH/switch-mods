_hx_hidden = {
  __id__ = true,
  _hx__closures = true,
  super = true,
  prototype = true,
  __fields__ = true,
  __ifields__ = true,
  __class__ = true,
  __properties__ = true,
}

function _hx_tostring(obj, depth)
  if depth == nil then
    depth = 0
  elseif depth > 5 then
    return "<...>"
  end

  local tstr = type(obj)
  if tstr == "string" then return obj
  elseif tstr == "nil" then return "null"
  elseif tstr == "number" then
    if obj == math.POSITIVE_INFINITY then return "Infinity"
    elseif obj == math.NEGATIVE_INFINITY then return "-Infinity"
    elseif obj == 0 then return "0"
    elseif obj ~= obj then return "NaN"
    else return tostring(obj)
    end
  elseif tstr == "boolean" then return tostring(obj)
  elseif tstr == "userdata" then
    local mt = getmetatable(obj)
    if mt then
      if mt.__index and type(mt.__index) == "table" then
        local keys = {}
        for key in pairs(mt.__index) do
          table.insert(keys, key)
        end
        table.sort(keys, function(a, b) return tostring(a) < tostring(b) end)
        if keys[1] == "__gc" then
          return string.format("<userdata: %p, __gc: %s, metatable keys: { %s }>", obj, _hx_tostring(mt.__index["__gc"]), table.concat(keys, ", "))
        else
          return string.format("<userdata: %p, metatable keys: { %s }>", obj, table.concat(keys, ", "))
        end
      elseif mt.__tostring then
        return string.format("<userdata: %p, __tostring: %s>", obj, tostring(obj))
      end
    end
    return string.format("<userdata: %p>", obj)
  elseif tstr == "function" then
    local ptr = _FuncPtr(obj)
    if ptr == 0 then
      return "<function [Lua]>"
    else
      return string.format("<function [Native: %x]>", ptr)
    end
  elseif tstr == "thread" then return "<thread>"
  elseif tstr == "table" then
    if obj.__enum__ ~= nil then
      return _hx_print_enum(obj, depth)
    elseif obj.toString ~= nil and not _hx_is_array(obj) then return obj:toString()
    elseif _hx_is_array(obj) then
      if obj.length > 5 then
        return "[...]"
      else
        local str = ""
        for i=0, (obj.length-1) do
          if i == 0 then
            str = str .. _hx_tostring(obj[i], depth+1)
          else
            str = str .. "," .. _hx_tostring(obj[i], depth+1)
          end
        end
        return "[" .. str .. "]"
      end
    elseif obj.__class__ ~= nil then
      if depth == 0 then
        local new = {}

        if obj.__name__ ~= nil then
          new.__name__ = _d(obj.__name__)
        end

        for k, v in pairs(obj) do
          new[_d(k)] = v
        end

        return _hx_print_class(new, depth)
      else
        return _hx_print_class(obj, depth)
      end
    else
      local buffer = {}
      local ref = obj
      if obj.__fields__ ~= nil then
        ref = obj.__fields__
      end
      for k,v in pairs(ref) do
        if _hx_hidden[k] == nil then
          _G.table.insert(buffer, _hx_tostring(k, depth+1) .. ' : ' .. _hx_tostring(obj[k], depth+1))
        end
      end

      return "{ " .. table.concat(buffer, ", ") .. " }"
    end
  else
    _G.error("Unknown Lua type", 0)
    return ""
  end
end

API = {}
_G["API"] = API
