#ifndef TYPESYSTEM_H
#define TYPESYSTEM_H

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>

#include "ASTNodes2.h"

using std::string;
using namespace llvm;



using TypeNamePair = std::pair<std::string, std::string>;

class TypeSystem {
private:

    LLVMContext &llvmContext;

    std::map<Type *, std::map < Type * , CastInst::CastOps>> _castTable;

    void addCast(Type *from, Type *to, CastInst::CastOps op);

public:

    Type *intTy = Type::getInt64Ty(llvmContext);
    Type *charTy = Type::getInt8Ty(llvmContext);
    Type *doubleTy = Type::getDoubleTy(llvmContext);
    Type *stringTy = Type::getInt8PtrTy(llvmContext);
    Type *voidTy = Type::getVoidTy(llvmContext);
    Type *boolTy = Type::getInt1Ty(llvmContext);



    TypeSystem(LLVMContext &context);



    Type *getVarType(const NIdentifier &type);

    Type *getVarType(string typeStr);

    Value *getDefaultValue(string typeStr, LLVMContext &context);

    Value *cast(Value *value, Type *type, BasicBlock *block);


    static string llvmTypeToStr(Value *value);

    static string llvmTypeToStr(Type *type);
};


#endif 
