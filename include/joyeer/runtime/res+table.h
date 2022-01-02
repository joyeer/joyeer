//
// Created by Qing Xu on 2022/1/2.
//

#ifndef __joyeer_runtime_res_table_h__
#define __joyeer_runtime_res_table_h__

#include <string>
#include <vector>

#include "joyeer/runtime/types.h"

struct StringResTable {

    // import string
    int import(const std::string& string);

    const std::string& operator[] (int index);
    std::vector<std::string> strings {};
};


struct TypeResTable {
    Type* operator[] (int index);
    std::vector<Type*> types {};
};

#endif //__joyeer_runtime_res_table_h__
