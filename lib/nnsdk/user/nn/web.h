#pragma once

#include "externals/util.h"

namespace nn::web {
    enum WebDisplayUrlKind
    {
        FullPath,
        Domain,
    };

    struct ShowWebPageArg {
        u8 unk[8192];

        explicit ShowWebPageArg(const char* url) {
            memset(unk, 0, sizeof(unk));
            external<void>(0x0343f7f0, this, url);
        }

        void SetDisplayUrlKind(const WebDisplayUrlKind displayUrlKind);
        void SetCallbackUrl(const char* pCallbackUrl);
    };

    struct WebPageReturnValue {
        u8 unk[8192];

        WebPageReturnValue() {
            external<void>(0x0343f820);
        }
    };

    nn::Result ShowWebPage(WebPageReturnValue* out, ShowWebPageArg const& arg);
}
