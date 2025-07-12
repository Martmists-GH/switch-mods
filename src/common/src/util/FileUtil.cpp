#include "FileUtil.h"

#include <cstdlib>
#include <logger/logger.h>
#include <nn/primitives.h>
#include <nn/fs/fs_directories.h>
#include <nn/fs/fs_files.h>

#include "common_utils.h"

namespace FileUtil {
    nn::Result createDirectory(const std::string& path) {
        return nn::fs::CreateDirectory(path.c_str());
    }

    bool exists(const std::string& path) {
        nn::fs::DirectoryEntryType type;
        nn::Result result = nn::fs::GetEntryType(&type, path.c_str());
        return result.IsSuccess() && type == nn::fs::DirectoryEntryType::DirectoryEntryType_File;
    }

    size_t getFileSize(const std::string& path) {
        if (!exists(path)) return -1;

        nn::fs::FileHandle handle{};
        long result = -1;

        nn::Result openResult = nn::fs::OpenFile(&handle, path.c_str(), nn::fs::OpenMode::OpenMode_Read);

        if (openResult.IsSuccess()) {
            nn::fs::GetFileSize(&result, handle);
            nn::fs::CloseFile(handle);
        }

        return result;
    }

    nn::Result deleteFile(const std::string& path) {
        return nn::fs::DeleteFile(path.c_str());
    }

    nn::Result writeFile(const std::string& path, const std::string& content) {
        return writeFile(path, content.c_str(), content.size());
    }

    nn::Result writeFile(const std::string& path, const void* content, size_t size) {
        if (!exists(path)) {
            auto r = nn::fs::CreateFile(path.c_str(), size);
            if (r.IsFailure()) {
                return r;
            }
        }

        nn::fs::FileHandle file{};
        auto r = nn::fs::OpenFile(&file, path.c_str(), nn::fs::OpenMode_Write);
        if (r.IsFailure()) return r;
        r = nn::fs::WriteFile(file, 0, content, size, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        if (r.IsFailure()) {
            nn::fs::CloseFile(file);
            return r;
        }
        nn::fs::CloseFile(file);
        return nn::ResultSuccess();
    }

    std::optional<FileHandle> readFile(const std::string& path, int alignment) {
        if (!exists(path)) return std::nullopt;

        nn::fs::FileHandle handle{};
        if (nn::fs::OpenFile(&handle, path.c_str(), nn::fs::OpenMode_Read).IsFailure()) return std::nullopt;
        long size = 0;
        nn::fs::GetFileSize(&size, handle);

        size_t alignedSize = ALIGN_UP(size, alignment);
        Logger::log("Reading file %s with aligned size %d (from %d)\n", path.c_str(), alignedSize, size);
        const auto ptr = std::malloc(alignedSize);  // FIXME: This seems to align in *some* games at least, but aligned_alloc causes errors in others. Needs a better fix!

        if (nn::fs::ReadFile(handle, 0, ptr, size).IsFailure()) {
            nn::fs::CloseFile(handle);
            return std::nullopt;
        }

        nn::fs::CloseFile(handle);
        return FileHandle {
            std::shared_ptr<void>(ptr, FreeDeleter()),
            alignedSize,
        };
    }
}
