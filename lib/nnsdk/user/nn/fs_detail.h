#pragma once

namespace nn::fs::detail {
    void LogResultErrorMessage(nn::Result, char const*);
    void LogErrorMessage(nn::Result, char const*, ...);
    bool IsEnabledAccessLog();
    void OutputAccessLogToOnlySdCard(char const*, ...);
}
