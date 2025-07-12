#include "config.h"

#include <util/FileUtil.h>
#include <nlohmann/json.hpp>

Config getConfigImpl() {
    if (!FileUtil::exists("sd:/mod_data/sv_crowdcontrol.json5")) {
        FileUtil::createDirectory("sd:/mod_data");
        std::string defaultConfig = R"({
    // The interval between crowd control polls in seconds. Default: 300 (5 minutes)
    "pollInterval": 300,
    // The duration of a poll in seconds. Must be between 15 and 1800. Default: 120 (2 minutes)
    // NOTE: pollDuration MUST be shorter than pollInterval, or the mod WILL break!
    "pollDuration": 120,
    // The number of options viewers can vote for. Must be between 2 and 5. Default: 3
    "pollOptions": 3,
    // The duration of effects from crowd control. If longer than pollInterval+pollDuration, effects may overlap. Default: 420 (7 minutes)
    "effectDuration": 420
})";
        FileUtil::writeFile("sd:/mod_data/sv_crowdcontrol.json5", defaultConfig);
    }

    auto res = FileUtil::readFile("sd:/mod_data/sv_crowdcontrol.json5");
    auto parsed = nlohmann::json::parse(static_cast<const char*>(res.value().buffer.get()), nullptr, false, true);

    return Config {
        parsed["pollInterval"],
        parsed["pollDuration"],
        parsed["pollOptions"],
        parsed["effectDuration"],
    };
}

Config& getConfig() {
    static auto config = getConfigImpl();
    return config;
}
