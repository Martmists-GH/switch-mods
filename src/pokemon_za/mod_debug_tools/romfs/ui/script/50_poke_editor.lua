---@param id int
function API:getMonsName(id)
    local fmt
    if (id < 1000) then
        fmt = "MONSNAME_%03d"
    else
        fmt = "MONSNAME_%04d"
    end

    return GetMessage("monsname", fmt:format(id)) or "[Invalid Pokemon]"
end

---@param id int
function API:getFormName(mon, id)
    if (ffi.int(ffi._ZN3pml8personal14PersonalSystem14CheckPokeExistEjj(mon, id)) == 0) then
         return "[Invalid Form]"
    end

    local fmt
    if (mon < 1000) then
        fmt = "ZKN_FORM_%03d_%03d"
    else
        fmt = "ZKN_FORM_%04d_%03d"
    end

    return GetMessage("zkn_form", fmt:format(mon, id)) or "[Invalid Form]"
end

---@param details json
function API:validateFormNo(details)
    if details["allowInvalid"] == true then return end

    if (ffi.int(ffi._ZN3pml8personal14PersonalSystem14CheckPokeExistEjj(details["species"] or 1, details["species"] or 0)) == 0) then
        details["form"] = 0
    end
end
