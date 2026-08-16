#pragma once
#include "game_constants.h"
#include "nn/fs/fs_directories.h"
#include "nn/fs/fs_files.h"
#include "nn/fs/fs_types.h"
#include "util/FileUtil.h"

static inline void executeScriptsInDirectory(lua_State* L, std::string folder) {
    nn::fs::DirectoryHandle handle{};

    nn::fs::DirectoryEntryType type;
    nn::Result result = nn::fs::GetEntryType(&type, folder.c_str());
    if (result.IsFailure() || type != nn::fs::DirectoryEntryType::DirectoryEntryType_Directory) return;

    nn::fs::OpenDirectory(&handle, folder.c_str(), nn::fs::OpenDirectoryMode_File);
    int64_t entryCount;
    nn::fs::GetDirectoryEntryCount(&entryCount, handle);
    if (entryCount > 0) {
        auto entries = new nn::fs::DirectoryEntry[entryCount];
        nn::fs::ReadDirectory(&entryCount, entries, handle, entryCount);
        nn::fs::CloseDirectory(handle);

        std::vector<nn::fs::DirectoryEntry *> sortedFiles;
        for (int i = 0; i < entryCount; i++) {
            sortedFiles.push_back(&entries[i]);
        }
        std::ranges::sort(sortedFiles,
                          [](nn::fs::DirectoryEntry *a, nn::fs::DirectoryEntry *b) {
                              return std::strcmp(a->mName, b->mName) <= 0;
                          });

        char buffer[0x320];
        for (auto item: sortedFiles) {
            Logger::log("Executing: %s\n", item->mName);
            auto file = folder + "/%s";
            snprintf(buffer, sizeof(buffer), file.c_str(), item->mName);
            nn::fs::FileHandle fileHandle{};
            nn::fs::OpenFile(&fileHandle, buffer, nn::fs::OpenMode_Read);
            auto strBuf = new char[item->mFileSize + 1];
            nn::fs::ReadFile(fileHandle, 0, strBuf, item->mFileSize);
            nn::fs::CloseFile(fileHandle);
            strBuf[item->mFileSize] = '\0';
            if (luaL_dostring(L, strBuf) != LUA_OK) {
                Logger::log("An error occurred in %s: %s\n", item->mName, lua_tostring(L, -1));
            }
            delete[] strBuf;
        }

        delete[] entries;
    }
}
