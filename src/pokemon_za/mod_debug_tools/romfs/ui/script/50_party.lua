---@param ctx table<string, any>
function API:loadPartyData(ctx)
    if ctx == nil then return end
    if ctx["forceChange"] ~= true then return end
    ctx["poke"] = ffi.json(ffi.hook_loadPartyData(ctx.selectedIndex))
end

---@param ctx table<string, any>
function API:savePartyData(ctx)
    if ctx == nil then return end
    ffi.hook_savePartyData(ctx.selectedIndex, ctx["poke"])
end

---@param ctx table<string, any>
function API:fixPartyData(ctx)
    if ctx == nil then return end
    ffi.hook_fixPartyData(ctx.selectedIndex)
end
