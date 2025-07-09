#include "ThreadUtil.h"

#include <nn/os.h>

#include <utility>

void ThreadUtil::ThreadHandle::threadImpl(ThreadHandle* handle) {
    handle->callback(handle->data);
    handle->isRunning = false;
}

std::shared_ptr<ThreadUtil::ThreadHandle> ThreadUtil::createThread(void* threadData, std::function<void(void*)> callback, const char* name, int stackSize) {
    void* threadStack = aligned_alloc(0x1000, stackSize);
    auto ptr = std::shared_ptr<void>(threadStack, FreeDeleter());
    auto handle = std::make_shared<ThreadHandle>();
    handle->threadStack = std::move(ptr);
    handle->callback = std::move(callback);
    handle->data = threadData;
    nn::os::CreateThread(&handle->handle, reinterpret_cast<void(*)(void*)>(&ThreadHandle::threadImpl), handle.get(), threadStack, stackSize, 16, 0);

    if (strlen(name) > 0) {
        nn::os::SetThreadName(&handle->handle, name);
    }

    return handle;
}

ThreadUtil::ThreadHandle::~ThreadHandle() {
    stop();
}

void ThreadUtil::ThreadHandle::start() {
    nn::os::StartThread(&handle);
}

void ThreadUtil::ThreadHandle::join() {
    nn::os::WaitThread(&handle);
}

void ThreadUtil::ThreadHandle::stop() {
    nn::os::DestroyThread(&handle);
}


