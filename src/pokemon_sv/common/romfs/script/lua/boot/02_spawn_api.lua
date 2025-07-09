local idx = 0

---@param pokemon number
---@param form number
---@param level number
---@param x number
---@param y number
---@param z number
function API:spawn(pokemon, form, level, x, y, z)
  ---main.encount.EncountManager.s_controlObject:getScene
  local parent = C4EE52E49562F8277.SBF5E0EFFB821427F:f462C9B70()
  local objName = "custom_spawn_" .. tostring(idx)
  idx = idx + 1

  math.randomseed(cc_Tick())

  local param = API:randomPokemon(level, pokemon, form)

  ---generated.pokemon.PokemonObjectFactory.Create
  c05424CF6.fF104BCD4(parent, objName, param, 0, x, y, z, true)
end
