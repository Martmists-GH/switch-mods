---@type Array<string>
local NOTICE_QUEUE = _hx_tab_array({length=0},0)

--- main.ui.hud_notice.HudNoticeTag.setUpTag
_trampoline(C2B3E52E6BEE978D5.prototype, "FE0CF254958206DA6",
    ---@param self main_ui_hud_notice_HudNoticeTag
    function(old, self)
  old(self)

  if NOTICE_QUEUE.length > 0 then
    local txt = NOTICE_QUEUE:shift()

    --- net_contents.stadium.CompetitionUtil.ConvertStringToStrBuf
    local tmp = cF18EF7C2.f09D1B75C(txt)

    --- pe.text.lua.Text.SetPaneString
    c8C3BF576.f316077B2(self[3], self[5] .. "/T_noticename_00", tmp)
  end
end)

---@param text string
---@param type? number
function API:pushNotice(text, type)
  local noticeType = type or 35

  if NOTICE_QUEUE.length ~= 0 then return end
  NOTICE_QUEUE:push(text)

  --- main.ui.hud_notice.SceneHudNoticeManager.PushNotice
  C5871932D02D179E6.S7BE6AD359F822AF9(noticeType, true)
end
