#pragma once
#include <nn/fs/fs_types.h>

// NS assumed from nn::diag::detail::AbortImpl("m_file","CopyToBuffer","C:/jenkins/workspace/sudachi/nuget/nuget_trinity_build_s2/trinity_titan/lib/gflib3/src/fs/archive_data.cpp",0x67);
namespace gfl::fs {
    struct Result {
        u64 unk1;
        u64 unk2;
    };

    struct FlatBufferReadInfo {
        long mSize;
        long mPosition;
        nn::fs::FileHandle mHandle;
    };

    struct FlatBufferLoaderInfo {
        char gap_0[56];
        const char *mPath;
    };

    struct FlatBufferLoader {
        void *__vftable;
        void *field_8;
        char gap_10[48];
        FlatBufferLoaderInfo *mFileInfo;
        char gap_48[88];
        int field_A0;
        int field_A4;
        bool field_A8;
        FlatBufferReadInfo mReadInfo;
        char gap_C8[128];
        void *field_148;
        void *field_150;
        void *field_158;
        void *field_160;
        void *mAllocator1;
        char gap_170[16];
        int field_180;
        void *struct_188;
        void *field_190;
        char gap_188[64];
        long mBufferSize;
        long mReadPosition;
        void *mFlatBuffer;
        void *mAllocator2;
        void *field_1F8;
        void *field_200;
    };

    Result ReadFileToBuffer(FlatBufferReadInfo *readInfo, void *buffer, ulong bufferSize);
    Result Read(FlatBufferLoader *thisPtr, void *buffer, ulong bufferSize);
}
