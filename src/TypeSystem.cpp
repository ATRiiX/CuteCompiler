#include "TypeSystem.h"
#include "CodeGen.h"

string TypeSystem::llvmTypeToStr(Type *value)
{

    if (value == nullptr)
    {
        throw std::invalid_argument("Value is nullptr");
    }

    //   value->print()
    //   cout << "typeID is     " << typeID << endl;

    switch (value->getTypeID())
    {
    case 0: ///<  0: type with no size
        return "VoidTyID";
    case 1: ///<  1: 16-bit floating point type
        return "HalfTyID";
    case 3: ///<  3: 64-bit floating point type
        return "DoubleTyID";
    case 11: ///< 11: Arbitrary bit width integers
        return "IntegerTyID";
    case 12: ///< 12: Functions
        return "FunctionTyID";
    case 14: ///< 14: Arrays
        return "ArrayTyID";
    case 15: ///< 15: Pointers
        return "PointerTyID";
    case 16: ///< 16: SIMD 'packed' format, or other vector type
        return "VectorTyID";
    default:
        throw std::invalid_argument("Unknown typeID in func llvmTypeToStr");
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
}

Type *TypeSystem::getVar(const NIdentifier &type)
{
    if (type.isType == false)
        throw std::invalid_argument("type.isType is false");
    if (type.isArray)
        return PointerType::get(getVar(type.name), 0);

    return getVar(type.name);

    return 0;
}

Value *TypeSystem::getValue(string typeStr, LLVMContext &context)
{
    Type *type = this->getVar(typeStr);
    if (type == this->intTy)
        return ConstantInt::get(type, 0, true);
    else if (type == this->doubleTy || type == this->doubleTy)
        return ConstantFP::get(type, 0);
    return nullptr;
}

void TypeSystem::addCast(Type *from, Type *to, CastInst::CastOps op)
{
    if (_castTable.find(from) == _castTable.end())
        _castTable[from] = std::map<Type *, CastInst::CastOps>();
    _castTable[from][to] = op;
}

Value *TypeSystem::cast(Value *value, Type *type, BasicBlock *block)
{
    Type *from = value->getType();
    if (from == type)
        return value;
    if (_castTable.find(from) == _castTable.end())
        throw std::logic_error("Type has no cast");
    if (_castTable[from].find(type) == _castTable[from].end())
    {
        string error = "Unable to cast from ";
        error += llvmTypeToStr(from) + " to " + llvmTypeToStr(type);
        throw std::logic_error(error.c_str());
    }

    return CastInst::Create(_castTable[from][type], value, type, "cast", block);
}
constexpr unsigned int stringToInt(const char *str, int h = 0)
{
    return !str[h] ? 5381 : (stringToInt(str, h + 1) * 33) ^ str[h];
}

Type *TypeSystem::getVar(string typeStr)
{

    switch (stringToInt(typeStr.c_str()))
    {
    case stringToInt("void"):
        return this->voidTy;
    case stringToInt("string"):
        return this->stringTy;
    case stringToInt("int"):
        return this->intTy;
    case stringToInt("double"):
        return this->doubleTy;
    case stringToInt("bool"):
        return this->boolTy;
    case stringToInt("char"):
        return this->charTy;

    default:
        return nullptr;
    }
}
