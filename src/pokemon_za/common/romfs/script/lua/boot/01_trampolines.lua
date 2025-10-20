function _trampoline(owner, name, cb)
  local old = owner[name]
  owner[name] = function(...) return cb(old, ...) end
end

function _trampolineOnce(owner, name, cb)
  local old = owner[name]
  owner[name] = function(...)
    owner[name] = old
    return cb(old, ...)
  end
end
