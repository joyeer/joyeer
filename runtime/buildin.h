#ifndef __joyeer_runtime_buildin_h__
#define __joyeer_runtime_buildin_h__


#include <string>
#include <unordered_map>
#include "function.h"

struct Global {
    const static std::vector<JrFunction::Pointer> funcTable;
};

#endif
