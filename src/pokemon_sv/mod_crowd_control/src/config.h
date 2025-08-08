#pragma once

struct Config {
    int pollInterval;
    int pollDuration;
    int pollOptions;
    int effectDuration;
};

Config& getConfig();
