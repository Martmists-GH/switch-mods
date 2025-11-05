#pragma once

#include <vector>
#include <string>
#include <functional>
#include <hk/diag/diag.h>
#include <util/MessageUtil.h>
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

#include "imgui.h"
#include "logger/logger.h"

namespace {
    std::string demangle(const char* name) {

        int status = -4; // some arbitrary value to eliminate the compiler warning

        // enable c++11 by passing the flag -std=c++11 to g++
        std::unique_ptr<char, void(*)(void*)> res {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
        };

        return (status==0) ? res.get() : name ;
    }
}

namespace ui {
    class Drawable {
    protected:
        std::vector<Drawable *> mChildren;

        virtual bool beginDraw() {
            return true;
        }

        virtual void draw() {
            for (auto child: mChildren) {
                child->render();
            }
        }

        virtual void endDraw() {}

    public:
        bool disabled = false;
        bool canDisable = false;

        virtual ~Drawable() = default;

        virtual void render() {
            if (canDisable) {
                ImGui::BeginDisabled(this->disabled);
            }
            if (this->beginDraw()) {
                this->draw();
                this->endDraw();
            }
            if (canDisable) {
                ImGui::EndDisabled();
            }
        }

        void addChild(Drawable *child) {
            mChildren.push_back(child);
        }

        virtual void clearChildren() {
            mChildren.clear();
        }
    };

#define ELEMENT(TName, ...) struct TName : public ui::Factory<TName> __VA_OPT__(,) __VA_ARGS__
#define ELEMENT_SUPPORTS_CHILD(TName)                             \
template<std::invocable<TName&> T>                                \
struct TName* TName(T&& initializer) {                            \
    struct TName* instance = TName::create(initializer);          \
    mChildren.push_back(instance);                                \
    mOwnedChildren.push_back(instance);                           \
    return instance;                                              \
}                                                                 \
template <typename ...Args>                                       \
struct TName* TName(Args... args) {                               \
    struct TName* instance = TName::createArgs<Args...>(args...); \
    mChildren.push_back(instance);                                \
    mOwnedChildren.push_back(instance);                           \
    return instance;                                              \
}

#define COMMON_ELEMENTS()                       \
ELEMENT_SUPPORTS_CHILD(Button)                  \
ELEMENT_SUPPORTS_CHILD(Checkbox)                \
ELEMENT_SUPPORTS_CHILD(Combo)                   \
ELEMENT_SUPPORTS_CHILD(ComboSimple)             \
ELEMENT_SUPPORTS_CHILD(FunctionElement)         \
ELEMENT_SUPPORTS_CHILD(FunctionWrappingElement) \
ELEMENT_SUPPORTS_CHILD(InputInt)                \
ELEMENT_SUPPORTS_CHILD(MenuBar)                 \
ELEMENT_SUPPORTS_CHILD(SliderFloat)             \
ELEMENT_SUPPORTS_CHILD(SliderInt)               \
ELEMENT_SUPPORTS_CHILD(Spacing)                 \
ELEMENT_SUPPORTS_CHILD(Separator)               \
ELEMENT_SUPPORTS_CHILD(Text)                    \
ELEMENT_SUPPORTS_CHILD(TextUnformatted)         \
ELEMENT_SUPPORTS_CHILD(TextSeparator)           \
ELEMENT_SUPPORTS_CHILD(StringView)

    template<typename T>
    class Factory : public virtual ui::Drawable {
    protected:
        std::vector<Drawable *> mOwnedChildren;

        virtual bool isValid() {
            return true;
        }

        ~Factory() override {
            for (auto owned: mOwnedChildren) {
                delete owned;
            }
        }

    public:
        void clearChildren() override {
            mChildren.clear();
            for (auto owned: mOwnedChildren) {
                delete owned;
            }
            mOwnedChildren.clear();
        }

        template<std::invocable<T&> F>
        static T *create(F&& initializer) {
            T *instance = IM_NEW(T);
            initializer(*instance);
            if (!instance->isValid()) {
                MessageUtil::abort("UI Element reported invalid after initializing!", "Element type: %s", demangle(typeid(T).name()).c_str());
            }
            return instance;
        }

        template <typename ...Args>
        static T *createArgs(Args... args) {
            T *instance = IM_NEW(T)(args...);
            if (!instance->isValid()) {
                MessageUtil::abort("UI Element reported invalid after initializing!", "Element type: %s", demangle(typeid(T).name()).c_str());
            }
            return instance;
        }

        template<std::invocable<T&> F>
        static T single(F&& initializer) {
            T instance{};
            initializer(instance);
            if (!instance.isValid()) {
                MessageUtil::abort("UI Element reported invalid after initializing!", "Element type: %s", demangle(typeid(T).name()).c_str());
            }
            return std::move(instance);
        }

        template <typename ...Args>
        static T single(Args... args) {
            T instance(args...);
            if (!instance.isValid()) {
                MessageUtil::abort("UI Element reported invalid after initializing!", "Element type: %s", demangle(typeid(T).name()).c_str());
            }
            return std::move(instance);
        }
    };
}
