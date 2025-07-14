#include "KeyboardUtil.h"

namespace KeyboardUtil {
    std::string openKeyboard(const std::string& headerText, const std::string& subText, nn::swkbd::Preset preset, const std::function<void(nn::swkbd::KeyboardConfig&)>& configure) {
        nn::swkbd::ShowKeyboardArg arg;
        nn::swkbd::MakePreset(&arg.keyboardConfig, preset);
        nn::swkbd::SetHeaderTextUtf8(&arg.keyboardConfig, headerText.c_str());
        nn::swkbd::SetSubTextUtf8(&arg.keyboardConfig, subText.c_str());

        configure(arg.keyboardConfig);

        auto workSize = nn::swkbd::GetRequiredWorkBufferSize(false);
        char work[workSize];
        char textCheckWork[0x400];
        char customizeDicWork[0x400];

        arg.workBuf = work;
        arg.workBufSize = workSize;
        arg.textCheckWorkBuf = textCheckWork;
        arg.textCheckWorkBufSize = sizeof(textCheckWork);
        arg._customizeDicBuf = customizeDicWork;
        arg._customizeDicBufSize = sizeof(customizeDicWork);

        nn::swkbd::String res(nn::swkbd::GetRequiredStringBufferSize());
        if (nn::swkbd::ShowKeyboard(&res, arg) == 671) {
            return "";
        }
        return std::string(res.cstr(), strlen(res.cstr()));
    }
}
