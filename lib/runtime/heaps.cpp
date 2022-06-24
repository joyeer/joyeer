//
// Created by Qing Xu on 2021/11/15.
//

#include "joyeer/runtime/heaps.h"
#include <cassert>

//--------------------------------------------------------

size_t Page::getTotalMemory() const {
    return kPageSize;
}

size_t Page::getUsedMemory() const {
    return used;
}

size_t Page::getFreeMemory() const {
    return getTotalMemory() - getUsedMemory();
}

AddressPtr Page::allocate(size_t size) {
    if(used + size > kPageSize) {
        assert(false);
        return kInvalid;
    }

    AddressPtr r = used;
    used += static_cast<AddressPtr>(size);
    for(int i = 0 ; i < size ; i ++) {
        assert(data[r + i] == 0);
    }
    memset(&data[r], 0, size);
    return reinterpret_cast<AddressPtr>(&data[r]);
}

//--------------------------------------------------------

size_t Space::getTotalMemory() const {
    size_t result = 0;
    for(auto& page: pages) {
       result += page->getTotalMemory();
    }
    return result;
}

size_t Space::getUsedMemory() const {
    size_t result = 0;
    for(auto& page: pages) {
        result += page->getUsedMemory();
    }
    return result;
}

size_t Space::getFreeMemory() const {
    size_t result = 0;
    for(auto& page: pages) {
        result += page->getFreeMemory();
    }
    return result;
}

//--------------------------------------------------------

size_t Heap::getTotalMemory() const {
    return spaces[kStaticSpace]->getTotalMemory() +
        spaces[kNewSpace]->getTotalMemory() +
        spaces[kOldSpace]->getTotalMemory();
}

size_t Heap::getUsedMemory() const {
    return spaces[kStaticSpace]->getUsedMemory() +
        spaces[kNewSpace]->getUsedMemory() +
        spaces[kOldSpace]->getUsedMemory();
}

size_t Heap::getFreeMemory() const {
    return spaces[kStaticSpace]->getFreeMemory() +
           spaces[kNewSpace]->getFreeMemory() +
           spaces[kOldSpace]->getFreeMemory();
}