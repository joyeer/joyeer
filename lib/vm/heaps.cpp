//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/vm/heaps.h"

intptr_t Page::allocate(size_t size) {
    if(used + size > kPageSize) {
        return kInvalid;
    }

    intptr_t r = used;
    used += static_cast<intptr_t>(size);
    return reinterpret_cast<intptr_t>(&data[r]);
}
