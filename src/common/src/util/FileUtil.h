//
// Created by mart on 7/6/25.
//

#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vapours/results/results_common.hpp>

namespace FileUtil {
    struct FileHandle {
        std::shared_ptr<void> buffer;
        size_t size;
    };

    nn::Result createDirectory(const std::string& path);

    // Checks for files only!
    bool exists(const std::string& path);
    nn::Result deleteFile(const std::string& path);

    nn::Result writeFile(const std::string& path, const std::string& content);
    nn::Result writeFile(const std::string& path, const void* content, size_t size);

    std::optional<FileHandle> readFile(const std::string& path, int alignment = 1);
};
