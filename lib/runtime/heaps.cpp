//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/runtime/heaps.h"

intptr_t Page::allocate(size_t size) {
    if(used + size > kPageSize) {
        assert(false);
        return kInvalid;
    }

    intptr_t r = used;
    used += static_cast<intptr_t>(size);
    memset(&data[r], 0, size);
    return reinterpret_cast<intptr_t>(&data[r]);
}
