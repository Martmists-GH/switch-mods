#pragma once
#include <functional>
#include <nn/swkbd/swkbd.h>
#include <string>

namespace KeyboardUtil {
    std::string openKeyboard(const std::string& headerText,
                             const std::string& subText,
                             nn::swkbd::Preset preset = nn::swkbd::Preset::Default,
                             const std::function<void(nn::swkbd::KeyboardConfig&)>& configure = [](nn::swkbd::KeyboardConfig& cfg){});
}
