//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_stringtable_h__
#define __joyeer_vm_stringtable_h__

#include <vector>
#include "joyeer/vm/types.h"

// all static strings resources
class StringTable {
public:

};


class FuncTable {
public:
    std::vector<JrFunc> functions;
};

#endif //__joyeer_vm_stringtable_h__
