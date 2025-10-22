#pragma once

#include <utility>
#include <vector>
#include <string>
#include <functional>
#include "ui/base/element.h"

namespace ui {
    ELEMENT(FunctionElement) {
        std::function<void()> callback;

        bool isValid()
        override{
                return callback != nullptr;
        }

        void draw()
        override{
                callback();
        }

        FunctionElement() = default;
        explicit FunctionElement(std::function<void()> callback) : callback(std::move(callback)) {};
    };
}
