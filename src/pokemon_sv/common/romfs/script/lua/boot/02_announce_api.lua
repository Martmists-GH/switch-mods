---@type Array<string>
local ANNOUNCE_QUEUE = _hx_tab_array({length=0},0)

--- main.ui.hud_announce.HudAnnounceTag.setUpTag
_trampoline(CD66781320C0AF981.prototype, "FE0CF254958206DA6",
    ---@param self main_ui_hud_announce_HudAnnounceTag
    function(old, self)
  old(self)

  if ANNOUNCE_QUEUE.length > 0 then
    local txt = ANNOUNCE_QUEUE:shift()

    --- net_contents.stadium.CompetitionUtil.ConvertStringToStrBuf
    local tmp = cF18EF7C2.f09D1B75C(txt)

    --- pe.text.lua.Text.SetPaneString
    c8C3BF576.f316077B2(self[2], "T_00", tmp)
    self[3]:fEAD9FB7D("switch_mode", 2)
    self[3]:fEAD9FB7D("switch_icon", 2)
  end
end)

---@param text string
function API:pushAnnounce(text)
  if ANNOUNCE_QUEUE.length ~= 0 then ANNOUNCE_QUEUE:shift() end

  ANNOUNCE_QUEUE:push(text)

  --- main.ui.hud_announce.SceneHudAnnounceManager.SetMissionTargetAnnounce
  C15A829DA3D306710.SC4225F3CD6983505("", true)
end
