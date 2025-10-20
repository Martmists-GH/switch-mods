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
    bool exists(const char* path);
    bool exists(const std::string& path);
    size_t getFileSize(const std::string& path);
    nn::Result deleteFile(const std::string& path);

    nn::Result writeFile(const std::string& path, const std::string& content);
    nn::Result writeFile(const std::string& path, const void* content, size_t size);
    template <typename T>
    nn::Result writeFile(const std::string& path, const T& content) {
        return writeFile(path, reinterpret_cast<const void*>(&content), sizeof(content));
    }

    std::optional<FileHandle> readFile(const std::string& path, int alignment = 1);
    template <typename T>
    std::optional<T> readFile(const std::string& path) {
        T out;
        auto handle = readFile(path);
        if (handle.has_value()) {
            auto& fh = handle.value();
            memcpy(&out, fh.buffer.get(), sizeof(out));
            return out;
        } else {
            return std::nullopt;
        }
    }
};
