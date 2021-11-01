//
// Created by Qing Xu on 2021/10/16.
//

#ifndef __joyeer_compiler_typedef_h__
#define __joyeer_compiler_typedef_h__

#include "joyeer/compiler/opcode.h"

#include <string>

enum class JrTypeKind : uint8_t {
    Void = 0x00,
    Nil,
    Boolean,
    Integer,
    Long,
    Any,

    FileModule,
    Class,
    Block,
    Function,
    Variable
};

enum JrAccessFlag : uint32_t {
    Public = 0x0001,
    Private = 0x0002,
    Protected = 0x0004,
    Static = 0x0008
};

std::string debugAccessFlag(JrAccessFlag flag);

// typedef is generated by compiler, which represent a **Type**
struct JrTypeDef {
    using Ptr = std::shared_ptr<JrTypeDef>;

    const std::string name;
    JrTypeKind kind;
    int32_t address;

protected:
    JrTypeDef(std::string  name, JrTypeKind kind):
            name(std::move(name)),
            kind(kind),
            address(-1) {
    }
};

// represent Nil kind
struct JrNilTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrNilTypeDef>;
    JrNilTypeDef();
};

// represent Void kind
struct JrVoidTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrVoidTypeDef>;
    JrVoidTypeDef();
};

// represent Any kind
struct JrAnyTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrAnyTypeDef>;
    JrAnyTypeDef(): JrTypeDef("Any", JrTypeKind::Any) {}
};

// Variable for Field/LocalVariable declarations
// will be used in ClassDef/FileModuleDef/FuncDef
struct JrVariableTypeDef: JrTypeDef {
    using Ptr = std::shared_ptr<JrVariableTypeDef>;

    int position = -1;
    int parent = -1;
    JrAccessFlag accessFlags = JrAccessFlag::Public;

    void markAsStatic() {
        accessFlags = static_cast<JrAccessFlag>( accessFlags | JrAccessFlag::Static);
    }

    // is a static variable, field only
    [[nodiscard]] bool isStatic() const {
        return (accessFlags & JrAccessFlag::Static) == JrAccessFlag::Static;
    }

    explicit JrVariableTypeDef(std::string  name):
            JrTypeDef(name, JrTypeKind::Variable) {}
};

// Represent Int kind
struct JrIntTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrIntTypeDef>;
    JrIntTypeDef();
};

// Represent Bool kind
struct JrBoolTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrBoolTypeDef>;
    JrBoolTypeDef();
};

// Represent Statement Block Type
struct JrBlockTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrBlockTypeDef>;

    int32_t base = -1; // re-located address for block variables
    std::vector<JrVariableTypeDef::Ptr> localVars; // local-variables

    JrBlockTypeDef();
};

enum JrFuncTypeKind : uint8_t {
    C_Func,
    VM_Func
};

// Represent Function kind (include class's function kind)
struct JrFuncTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrFuncTypeDef>;

    JrFuncTypeKind funcKind;
    JrBlockTypeDef::Ptr block;
    std::vector<JrTypeDef::Ptr> paramTypes;
    JrTypeDef::Ptr returnType;
    std::vector<Instruction> instructions;

    explicit JrFuncTypeDef(const std::string& name);
};

struct JrClassTypeDef : JrTypeDef {
    using Ptr = std::shared_ptr<JrClassTypeDef>;

    explicit JrClassTypeDef(const std::string& name);

    static JrClassTypeDef::Ptr create(const std::string& name) {
        return std::make_shared<JrClassTypeDef>(name);
    }
};

struct JrFileModuleTypeDef : JrClassTypeDef {
    using Ptr = std::shared_ptr<JrFileModuleTypeDef>;

    // include ClassDef/FuncDef
    JrBlockTypeDef::Ptr block;

    // File initialize instructions
    std::vector<Instruction> instructions;

    explicit JrFileModuleTypeDef(const std::string& name);
};

namespace BuildIn::TypeDef {
    // call CompilerService to register the following TypeDef
    [[maybe_unused]] static const JrVoidTypeDef::Ptr Void = std::make_shared<JrVoidTypeDef>();
    [[maybe_unused]] static const JrAnyTypeDef::Ptr Any = std::make_shared<JrAnyTypeDef>();
    [[maybe_unused]] static const JrNilTypeDef::Ptr Nil = std::make_shared<JrNilTypeDef>();
    [[maybe_unused]] static const JrIntTypeDef::Ptr Int = std::make_shared<JrIntTypeDef>();
    [[maybe_unused]] static const JrBoolTypeDef::Ptr Bool = std::make_shared<JrBoolTypeDef>();

    static const JrClassTypeDef::Ptr String = std::make_shared<JrClassTypeDef>("String");

    static const JrFuncTypeDef::Ptr print = std::make_shared<JrFuncTypeDef>("print(message:)");
};

#endif //__joyeer_compiler_typedef_h__
