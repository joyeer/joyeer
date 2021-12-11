//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/loader.h"
#include "joyeer/vm/res+table.h"
#include "joyeer/vm/gc.h"
#include "joyeer/vm/types.h"

struct IsolateVM {

    StringTable* stringTable { new StringTable() };
    ClassResTable* classTable { new ClassResTable() };
    MethodResTable* methodTable { new MethodResTable() };

    explicit IsolateVM() ;
    ~IsolateVM();

    void run(const ModuleType::Ptr& module, CompilerService* compilerService);
    void run(const ModuleClass* moduleClass);
    void register_(const ModuleType::Ptr& moduleType, Class *klass);

    // import ModuleClass, prepare the memory for module class
    void import(ModuleClass* moduleClass);

    // query AST ClassType's Class definition in VM
    const Class* query(const ClassType::Ptr& classType);

    GC* gc;


    std::vector<const Class*> classes {};
    std::unordered_map<int, int> mapOfTypeAndClass {};


};

#endif //__joyeer_vm_isolate_h__
