
#include <checks.hpp>
#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/types.h>
#include <nn/fs/fs_files.h>
#include <util/FileUtil.h>
#include <util/MessageUtil.h>

#include "externals/gfl/fs.h"

HkTrampoline<gfl::fs::Result, gfl::fs::FlatBufferLoader *, void *, ulong> FlatBufferReadHook = hk::hook::trampoline([](gfl::fs::FlatBufferLoader *thisPtr, void *buffer, ulong bufferSize) {
    if (thisPtr->mFileInfo && thisPtr->field_148 && thisPtr->field_A8) {
        if (FileUtil::exists(thisPtr->mFileInfo->mPath)) {
            Logger::log("Redirecting file access for %s\n", thisPtr->mFileInfo->mPath);

            gfl::fs::FlatBufferReadInfo readInfo{};
            readInfo.mPosition = 0;

            nn::Result openResult = nn::fs::OpenFile(&readInfo.mHandle, thisPtr->mFileInfo->mPath,
                                                     nn::fs::OpenMode::OpenMode_Read);
            if (openResult.IsSuccess()) {
                auto result = gfl::fs::ReadFileToBuffer(&readInfo, buffer, bufferSize);

                nn::fs::CloseFile(readInfo.mHandle);

                thisPtr->field_180 = 1;
                thisPtr->mReadPosition = readInfo.mPosition;
                return result;
            }
        }
    }

    return FlatBufferReadHook.orig(thisPtr, buffer, bufferSize);
});

void NOINLINE fileRedirectionFunc(gfl::fs::FlatBufferLoader *loader) {
    if (loader->mFileInfo && loader->field_148 && loader->field_A8) {
        if (FileUtil::exists(loader->mFileInfo->mPath)) {
            auto fileSize = FileUtil::getFileSize(loader->mFileInfo->mPath);
            loader->mBufferSize = fileSize;
        }
    }
}

#define ReadInvokeHook(n) HkTrampoline<gfl::fs::Result, gfl::fs::FlatBufferLoader *> FlatBufferReadInvokeHook##n = hk::hook::trampoline([](gfl::fs::FlatBufferLoader* thisPtr) { fileRedirectionFunc(thisPtr); return FlatBufferReadInvokeHook##n.orig(thisPtr); })

ReadInvokeHook(0);
ReadInvokeHook(1);
ReadInvokeHook(2);

void romfs_hooks() {
    FlatBufferReadHook.installAtPtr(gfl::fs::Read);

    if (is_version("3.0.1")) {
        FlatBufferReadInvokeHook0.installAtMainOffset(0x00cc8138);
        FlatBufferReadInvokeHook1.installAtMainOffset(0x0203da2c);
        FlatBufferReadInvokeHook2.installAtMainOffset(0x0208a980);
    } else {
        HK_ABORT("ReadInvokeHook Not implemented for version!", nullptr);
    }
}
