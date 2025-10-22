#include <cstddef>
#include <game_constants.h>
#include <externals/gfl/string.h>
#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_files.h>
#include <nn/fs/fs_types.h>
#include <util/FileUtil.h>
#include "externals/gfl/hash.h"

namespace gfl::fs {
    struct Result {
        u64 unk1;
        u64 unk2;
    };

    struct Path {
        gfl::String::instance* m_string;
        gfl::StringHolder m_stringHolder;
        uint8_t m_type;
    };

    struct FlatBufferFileInfo {
        char unk[0x16];
        char* mPath;
    };

    struct FlatBufferReadInfo {
        long mSize;
        long mPosition;
        nn::fs::FileHandle mHandle;

        gfl::fs::Result ReadToBuffer(void* buffer, long bufferSize);
    };

    struct FlatBufferLoader {
        char unk[0x40];
        gfl::fs::Path path;
        gfl::fs::Path originalPath;
        char unk2[0x30];
        FlatBufferReadInfo readInfo;
        char unk3[0x8c];
        char mode;
        char unk4[0x30];
        ulong mSize;
        ulong mPosition;

        gfl::fs::Result Open();
        gfl::fs::Result Read(void* buffer, long bufferSize);
    };

    static_assert(offsetof(FlatBufferLoader, path) == 0x40);
    static_assert(offsetof(FlatBufferLoader, readInfo) == 0xd0);
    static_assert(offsetof(FlatBufferLoader, mode) == 0x174);
    static_assert(offsetof(FlatBufferLoader, mSize) == 0x1a8);
}

HkTrampoline<gfl::fs::Result, gfl::fs::FlatBufferLoader *> FlatBufferOpenHook = hk::hook::trampoline([](gfl::fs::FlatBufferLoader *thisPtr) {
    auto res = FlatBufferOpenHook.orig(thisPtr);

    std::string path = std::string(ROM_MOUNT) + ":/" + thisPtr->path.m_stringHolder.m_content.m_string;
    if (FileUtil::exists(path)) {
        auto newSize = FileUtil::getFileSize(path);
        thisPtr->mSize = newSize;
    }

    return res;
});

HkTrampoline<gfl::fs::Result, gfl::fs::FlatBufferLoader*, void*, long> FlatBufferReadHook = hk::hook::trampoline([](gfl::fs::FlatBufferLoader* p1, void* p2, long p3) {
    if (p1->mode == 3) {
        std::string path = std::string(ROM_MOUNT) + ":/" + p1->path.m_stringHolder.m_content.m_string;
        if (FileUtil::exists(path)) {
            Logger::log("Redirecting file access for %s\n", p1->path.m_stringHolder.m_content.m_string);
            gfl::fs::FlatBufferReadInfo info {};
            auto res = nn::fs::OpenFile(&info.mHandle, path.c_str(), nn::fs::OpenMode_Read);
            if (res.IsSuccess()) {
                auto result = info.ReadToBuffer(p2, p3);
                nn::fs::CloseFile(info.mHandle);
                p1->mPosition = info.mPosition;
                return result;
            }
        }
    }
    return FlatBufferReadHook.orig(p1, p2, p3);
});

void romfs_hooks() {
    FlatBufferOpenHook.installAtPtr(pun<void*>(&gfl::fs::FlatBufferLoader::Open));
    FlatBufferReadHook.installAtPtr(pun<void*>(&gfl::fs::FlatBufferLoader::Read));
}
