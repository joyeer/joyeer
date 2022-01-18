//
// Created by Qing Xu on 2022/1/2.
//

#ifndef __joyeer_isolate_vm_h__
#define __joyeer_isolate_vm_h__

#include "joyeer/runtime/res+table.h"
#include "joyeer/runtime/gc.h"
#include "joyeer/runtime/sys.h"

struct IsolateVM {

    IsolateVM();
    virtual ~IsolateVM();

    void bootstrap();

    StringResTable* strings { new StringResTable() };
    TypeResTable* types { new TypeResTable() };

    GC* gc;

    ArrayClass* arrayClass = nullptr;
    DictClass* dictClass = nullptr;
    DictEntry* dictEntryClass = nullptr;
    Optional* optionalClass = nullptr;
    StringClass* stringClass = nullptr;
};


#endif //__joyeer_isolate_vm_h__
