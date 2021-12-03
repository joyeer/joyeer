//
// Created by Qing Xu on 2021/11/7.
//

#ifndef __joyeer_vm_res_table_h__
#define __joyeer_vm_res_table_h__

#include <string>
#include <unordered_map>
#include "joyeer/vm/types.h"
#include "joyeer/compiler/compiler+service.h"

class CompilerService;

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

// Store all Method into a ResTable
class MethodResTable {
public:

    //import functions from compiler service
    void import(CompilerService* compilerService);

    Method* query(const FuncType::Ptr& funcType);
private:
    std::vector<Method*> methods {};
    std::unordered_map<int, int> mapOfTypeAndMethod {};
};


#endif //__joyeer_vm_res_table_h__
