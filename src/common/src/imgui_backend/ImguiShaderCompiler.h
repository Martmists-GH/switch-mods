#pragma once

#include <memory>

#include "nn/types.h"
#include "nvn/nvn_Cpp.h"

struct CompiledData {
    std::shared_ptr<u8> ptr;
    ulong size;
};

struct BinaryHeader {

    BinaryHeader(u32* header) {
        mFragmentControlOffset = header[0];
        mVertexControlOffset = header[1];
        mFragmentDataOffset = header[2];
        mVertexDataOffset = header[3];
    }

    u32 mVertexControlOffset;
    u32 mVertexDataOffset;
    u32 mFragmentControlOffset;
    u32 mFragmentDataOffset;

};
