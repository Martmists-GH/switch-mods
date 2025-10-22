#pragma once

namespace ik::event {
    struct IkkakuEventScriptCommand {
        static uint32_t GetCurrentMoney();
        static void AddAndShowMonyeUI(uint32_t money);
        static void AddItem(uint32_t item, uint32_t amount);
    };
}
