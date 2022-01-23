//
// Created by Qing Xu on 2022/1/4.
//

#ifndef __joyeer_runtime_loader_h__
#define __joyeer_runtime_loader_h__

#include "joyeer/runtime/isolate+vm.h"

class ClassLoader {
public:
    explicit ClassLoader(IsolateVM* vm);

    void load(Class* klass);
private:
    IsolateVM* vm;
};

#endif //__joyeer_runtime_loader_h__
