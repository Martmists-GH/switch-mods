---Show login popup on main menu
---main.ui.title.Title:Setup
cc_init_done = false
_trampoline(C11AAB7F72B54DA24.prototype, "F7C68FEDB79AB6396", function(old, self, ctx)
  old(self, ctx)
  if not cc_init_done then
    cc_init_done = true
    cc_Init()
  end
end)

--- If starter not yet obtained:
---main_dynamic.event.scenario.main.Common_0080:postEnd
_trampoline(CDC6D71168F8C306B.prototype, "F3E13E4094D09A594", function(old, self)
  old(self)
  cc_Start()
  API:pushNotice("CrowdControl active\nMod by Martmists")
end)

--- Else:
cc_started = false
---main.playables.free_move.ctrl.ActorControlBehavior:PostUpdate
_trampoline(C51C789C764D66E76.prototype, "F20A40E2F8B95D5F6", function(old, self, ctxt)
  old(self, ctxt)

  if self[22] == 1 then
    ---main.savedata.accessor.FlagWorkSaveAccessor.FlagWorkCache.s_workCache.h
    local pkmn = C10578806AC30DCA3.S971699EB064C6350.h["WEVT_COMMON_0080_FIRST_POKEMON"]
    if pkmn ~= nil and pkmn ~= -1 and not cc_started then
      cc_started = true
      cc_Start()
      API:pushNotice("CrowdControl active\nMod by Martmists")
    end
  end
end)

--- Hooks for events ---

---main.playables.free_move.ctrl.RideAbilityUtil.GetRideAbility
_trampoline(C9B18926651A71FDA, "S57669E023D07F721", function(old, control, abilityNo)
  if API:CC_ABILITY_DISABLED() then
    return false
  end
  return old(control, abilityNo)
end)

---main.encount.EncountManager:SpawnPokeEncount
_trampoline(C4EE52E49562F8277.prototype, "F316E31E09886ADC9", function(old, self, monsNo,formNo,sexType,minLevel,maxLevel,pos,bandType,bandMonsNo,bandFormno,bandSexType,fakeSp,pokeData,pokeCollision,rareTryUp,option)
  if API:CC_RANDOM_SPAWNS_ENABLED() then
    monsNo, formNo, sexType = API:randomPokemonSpecies()
  end

  if API:CC_SHINY_BOOST_ENABLED() then
    rareTryUp = (rareTryUp or 0) + 512
  end

  return old(self,monsNo,formNo,sexType,minLevel,maxLevel,pos,bandType,bandMonsNo,bandFormno,bandSexType,fakeSp,pokeData,pokeCollision,rareTryUp,option)
end)

---main.encount.EncountManager:GetSpawnLimit
_trampoline(C4EE52E49562F8277.prototype, "FDEBEF6A53436DB51", function(old, self, normalOnly)
  local res = old(self, normalOnly)
  if API:CC_SPAWN_BOOST_ENABLED() then
    res = res * 10
  end
  return res
end)

print("Finished setting up Crowd Control hooks!")
