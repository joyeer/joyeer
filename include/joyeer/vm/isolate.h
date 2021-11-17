//
// Created by Qing Xu on 2021/11/8.
//

#ifndef __joyeer_vm_isolate_h__
#define __joyeer_vm_isolate_h__

#include "joyeer/vm/compiler.h"
#include "joyeer/vm/stringtable.h"
#include "joyeer/vm/types.h"


class IsolateVM {
    StringTable* stringTable;

    void run(FileModuleDecl::Ptr fileModule);
};

#endif //__joyeer_vm_isolate_h__
