#pragma once
#include <functional>
#include <nn/os/os_ThreadTypes.h>

#include "common_utils.h"

class ThreadUtil {
    class ThreadHandle {
        std::shared_ptr<void> threadStack;
        nn::os::ThreadType handle = {};
        bool isRunning = false;
        void* data;
        std::function<void(void*)> callback;

        static void threadImpl(ThreadHandle* handle);
        friend ThreadUtil;

    public:
        ~ThreadHandle();
        void start();
        void join();
        void stop();
    };

    static std::shared_ptr<ThreadHandle> createThread(void* threadData, std::function<void(void*)> callback , const char* name = "", int stackSize = 0x10000);
public:
    template<typename T>
    static std::shared_ptr<ThreadHandle> createThread(T* threadData, std::function<void(T*)> callback, const char* name = "", int stackSize = 0x10000) {
        return createThread(threadData, callback, name, stackSize);
    }

    static std::shared_ptr<ThreadHandle> createThread(const std::function<void(void*)>& callback, const char* name = "", int stackSize = 0x10000) {
        return createThread(nullptr, callback, name, stackSize);
    }

    static std::shared_ptr<ThreadHandle> createThread(const std::function<void()>& callback, const char* name = "", int stackSize = 0x10000) {
        return createThread(nullptr, std::move([callback](void*){ callback(); }), name, stackSize);
    }
};
