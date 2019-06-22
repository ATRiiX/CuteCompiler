#include "TypeSystem.h"
#include "CodeGen.h"

string TypeSystem::llvmTypeToStr(Type *value)
{

    Type::TypeID typeID;
    if (value != nullptr)
        typeID = value->getTypeID();
    else
        throw std::invalid_argument("Value is nullptr");
    //   value->print()
    switch (typeID)
    {
    case Type::VoidTyID:
        return "VoidTyID";
    case Type::HalfTyID:
        return "HalfTyID";
    case Type::DoubleTyID:
        return "DoubleTyID";
    case Type::IntegerTyID:
        return "IntegerTyID";
    case Type::FunctionTyID:
        return "FunctionTyID";
    case Type::StructTyID:
        return "StructTyID";
    case Type::ArrayTyID:
        return "ArrayTyID";
    case Type::PointerTyID:
        return "PointerTyID";
    case Type::VectorTyID:
        return "VectorTyID";
    default:
        throw std::invalid_argument("Unknown error in func llvmTypeToStr");
    }
}

string TypeSystem::llvmTypeToStr(Value *value)
{
    if (value)
        return llvmTypeToStr(value->getType());
    else
        throw std::invalid_argument("Value is nullptr");
}

TypeSystem::TypeSystem(LLVMContext &context) : llvmContext(context)
{
    addCast(intTy, doubleTy, llvm::CastInst::SIToFP);
    addCast(boolTy, doubleTy, llvm::CastInst::SIToFP);
    addCast(doubleTy, intTy, llvm::CastInst::FPToSI);
    addCast(intTy, intTy, llvm::CastInst::SExt);
}

void TypeSystem::addStructMember(string structName, string memType, string memName)
{
    if (this->_structTypes.find(structName) == this->_structTypes.end())
    {
        throw std::logic_error("Unknown struct name");
    }
    this->_structMembers[structName].push_back(std::make_pair(memType, memName));
}

void TypeSystem::addStructType(string name, llvm::StructType *type)
{
    this->_structTypes[name] = type;
    this->_structMembers[name] = std::vector<TypeNamePair>();
}

Type *TypeSystem::getVarType(const NIdentifier &type)
{
    assert(type.isType);
    if (type.isArray)
    {
        return PointerType::get(getVarType(type.name), 0);
    }

    return getVarType(type.name);

    return 0;
}

Value *TypeSystem::getDefaultValue(string typeStr, LLVMContext &context)
{
    Type *type = this->getVarType(typeStr);
    if (type == this->intTy)
    {
        return ConstantInt::get(type, 0, true);
    }
    else if (type == this->doubleTy || type == this->doubleTy)
    {
        return ConstantFP::get(type, 0);
    }
    return nullptr;
}

void TypeSystem::addCast(Type *from, Type *to, CastInst::CastOps op)
{
    if (_castTable.find(from) == _castTable.end())
    {
        _castTable[from] = std::map<Type *, CastInst::CastOps>();
    }
    _castTable[from][to] = op;
}

Value *TypeSystem::cast(Value *value, Type *type, BasicBlock *block)
{
    Type *from = value->getType();
    if (from == type)
        return value;
    if (_castTable.find(from) == _castTable.end())
    {
        throw std::logic_error("Type has no cast");
        return value;
    }
    if (_castTable[from].find(type) == _castTable[from].end())
    {
        string error = "Unable to cast from ";
        error += llvmTypeToStr(from) + " to " + llvmTypeToStr(type);
        throw std::logic_error(error.c_str());
        return value;
    }

    return CastInst::Create(_castTable[from][type], value, type, "cast", block);
}

bool TypeSystem::isStruct(string typeStr) const
{
    return this->_structTypes.find(typeStr) != this->_structTypes.end();
}

long TypeSystem::getStructMemberIndex(string structName, string memberName)
{
    if (this->_structTypes.find(structName) == this->_structTypes.end())
    {
        throw std::logic_error("Unknown struct name");
        return 0;
    }
    auto &members = this->_structMembers[structName];
    for (auto it = members.begin(); it != members.end(); it++)
    {
        if (it->second == memberName)
        {
            return std::distance(members.begin(), it);
        }
    }

    throw std::logic_error("Unknown struct member");

}

Type *TypeSystem::getVarType(string typeStr)
{

    if (typeStr.compare("int") == 0)
    {
        return this->intTy;
    }
    if (typeStr.compare("double") == 0)
    {
        return this->doubleTy;
    }
    if (typeStr.compare("bool") == 0)
    {
        return this->boolTy;
    }
    if (typeStr.compare("char") == 0)
    {
        return this->charTy;
    }
    if (typeStr.compare("void") == 0)
    {
        return this->voidTy;
    }
    if (typeStr.compare("string") == 0)
    {
        return this->stringTy;
    }

    if (this->_structTypes.find(typeStr) != this->_structTypes.end())
        return this->_structTypes[typeStr];

    return nullptr;
}
