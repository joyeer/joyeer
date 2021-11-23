//
// Created by Qing Xu on 2021/11/15.
//

#ifndef __joyeer_vm_heaps_h__
#define __joyeer_vm_heaps_h__

#include "joyeer/vm/types.h"

constexpr size_t kPageSize = 256 * 1024;
constexpr size_t kMaxPageNumberInSingleSpace = 1024 * 1024; // the max number of page in one single space
constexpr intptr_t kInvalid = -1;


// MemoryAddress to locate correct data in Heap
// Each single Space contains max memory - 64Gb
struct alignas(uintptr_t) MemoryAddress {
    uintptr_t space: 4 = 0;         // max 16 space per heap
    uintptr_t page: 18 = 0;         // max 256k page per space
    uintptr_t position: 18 = 0;     // max 256k byte per page
    uintptr_t reserved: 24 = 0;
};

struct alignas(uintptr_t) SpaceAddress {
    intptr_t page: 16 = 0;
    intptr_t position: 16 = 0;
};

// the default page size : 256k
struct Page {

    char8_t data[kPageSize]{};
    intptr_t used = 0;

    [[nodiscard]] intptr_t allocate(size_t size) {
        if(used + size > kPageSize) {
            return kInvalid;
        }

        intptr_t r = used;
        used += static_cast<intptr_t>(size);
        return reinterpret_cast<intptr_t>(&data[r]);
    }
};

// Some memory allocation is larger than 16k
struct LargeChunk {

};

// the space contains multi-pages
struct Space {
    std::vector<Page*> pages { new Page() };
    size_t cur = 0;

    intptr_t allocate(size_t size) {
        return pages[cur]->allocate(size);
    }
};

constexpr static int kStaticSpace = 0;
constexpr static int kNewSpace = 1;
constexpr static int kOldSpace = 2;
constexpr static int kTotalSpaceCount = 3;

struct Heap {

#define DECLARE_SPACE(space) new Space(),
    Space* spaces[kTotalSpaceCount] {
            DECLARE_SPACE(kStaticSpace)
            DECLARE_SPACE(kNewSpace)
            DECLARE_SPACE(kOldSpace)
    };
#define SPACE(space) spaces[space]
#undef DECLARE_SPACE

    // allocate a memory for a given size
    MemoryAddress allocate(size_t size) {
        auto address = SPACE(kNewSpace)->allocate(size);
        return {
            .space = 1
        };
    }
};



#endif //__joyeer_vm_heaps_h__
