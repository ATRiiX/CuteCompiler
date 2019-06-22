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

    std::map <string, std::vector<TypeNamePair>> _structMembers;

    std::map<string, llvm::StructType *> _structTypes;

    std::map<Type *, std::map < Type * , CastInst::CastOps>> _castTable;

    void addCast(Type *from, Type *to, CastInst::CastOps op);

public:
    Type *floatTy = Type::getFloatTy(llvmContext);
    Type *intTy = Type::getInt64Ty(llvmContext);
    Type *charTy = Type::getInt8Ty(llvmContext);
    Type *doubleTy = Type::getDoubleTy(llvmContext);
    Type *stringTy = Type::getInt8PtrTy(llvmContext);
    Type *voidTy = Type::getVoidTy(llvmContext);
    Type *boolTy = Type::getInt1Ty(llvmContext);



    TypeSystem(LLVMContext &context);

    void addStructType(string structName, llvm::StructType *);

    void addStructMember(string structName, string memType, string memName);

    long getStructMemberIndex(string structName, string memberName);

    Type *getVarType(const NIdentifier &type);

    Type *getVarType(string typeStr);

    Value *getDefaultValue(string typeStr, LLVMContext &context);

    Value *cast(Value *value, Type *type, BasicBlock *block);

    bool isStruct(string typeStr) const;

    static string llvmTypeToStr(Value *value);

    static string llvmTypeToStr(Type *type);
};


#endif 
