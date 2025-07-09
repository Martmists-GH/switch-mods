#include <stddef.h>

extern "C" {
    void* hk_malloc(size_t size);
    void hk_free(void* ptr);

    void* __libc_malloc(size_t size) {
        return hk_malloc(size);
    }

    void __libc_free(void* ptr) {
        hk_free(ptr);
    }
}
