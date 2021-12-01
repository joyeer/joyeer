//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_res_table_h__
#define __joyeer_vm_res_table_h__

#include "joyeer/vm/types.h"
#include <string>

// all static strings resources
class StringTable {
public:
    explicit StringTable() = default;

    void import(std::vector<std::string>& string);

    const std::string& operator[] (int index);
private:
    std::vector<std::string> strings {};
};

// Store all Class in Class resource table
struct ClassResTable {
public:
    explicit ClassResTable() = default;

    std::vector<Class*> classes {};
};

class MethodResTable {

};


#endif //__joyeer_vm_res_table_h__
