#include "externals/gfl/object.h"

namespace gfl {
    AllocatedObject::instance* AllocatedObject::copy() {
        auto impl = this->impl()->fields;
        auto clone = static_cast<gfl::AllocatedObject::instance*>(impl.m_resource->impl()->vtable->allocate(impl.m_resource, impl.m_size, impl.m_align));
        memcpy(clone, this, impl.m_size);
        clone->fields.m_ptr = clone;
        return clone;
    }
}
