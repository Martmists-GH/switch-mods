#pragma once

#include "util.h"

template <typename T>
struct IlStruct {

};

template <typename T>
struct IlClass {
    struct instance : T {

    };
};
