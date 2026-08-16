---@return number
function API:getMoney()
    local money = ffi._ZN2ik5event24IkkakuEventScriptCommand15GetCurrentMoneyEv()
    return ffi.int(money)
end

---@param amount number
function API:addMoney(amount)
    ffi._ZN2ik5event24IkkakuEventScriptCommand17AddAndShowMonyeUIEj(amount)
end

---@return number
function API:getMedals()
    local medals = ffi._ZN2ik20ZARoyaleSaveAccessor11GetMedalNumEv()
    return ffi.int(medals)
end

---@param amount number
function API:addMedals(amount)
    local current = API:getMedals() + amount

    if current < 0 then
        current = 0
    elseif current > 9999 then
        current = 9999
    end

    ffi._ZN2ik20ZARoyaleSaveAccessor11SetMedalNumEj(current)
end

---@return number
function API:getTickets()
    local mgr = ffi.getptr("_ZN3gfl9SingletonIN2ik18TrainerRankManagerEE10s_instanceE")
    local value = ffi.readptr(mgr + 28);
    return ffi.int(value) & 0xFFFFFFFF
end

---@param amount number
function API:addTickets(amount)
    local mgr = ffi.getptr("_ZN3gfl9SingletonIN2ik18TrainerRankManagerEE10s_instanceE")
    ffi._ZN2ik18TrainerRankManager8AddPointEib(mgr, amount);
end
