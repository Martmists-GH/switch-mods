#pragma once
#include "nn/os.h"

namespace nn::os {
    void DefaultUserExceptionHandler(UserExceptionInfo*);
    void EnableUserExceptionHandlerOnDebugging(bool);
}
