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

    explicit IsolateVM() ;
    ~IsolateVM();

    void run(const ModuleType::Ptr& module, CompilerService* compilerService);

private:
    friend class ClassLoader;

    void register_(const ModuleType::Ptr& moduleType, const ModuleClass* moduleClass);

    // import ModuleClass, prepare the memory for module class
    void import(ModuleClass* moduleClass);


    ModuleClass* query(const ModuleType::Ptr& module);
    Class* query(const ClassType::Ptr& classType);

    GC* gc;

    StringTable* stringTable { new StringTable() };
    ClassResTable* classTable { new ClassResTable() };
    MethodResTable* funcTable { new MethodResTable() };

    std::vector<ModuleClass*> modules;
    std::unordered_map<int, int> mapOfModuleTypeAndModuleClass;
};

#endif //__joyeer_vm_isolate_h__
