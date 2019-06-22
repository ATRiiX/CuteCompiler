#include "CodeGen.h"
#include "ASTNodes2.h"
#include <assert.h>
#include "llvm/IR/CallSite.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/IPO.h"
#include <llvm/IR/Value.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/raw_ostream.h>
#include "llvm/ADT/iterator_range.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbolFlags.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"
#include <stdexcept>
#include <llvm/IR/LLVMContext.h>
using legacy::PassManager;

static Type *
TypeOf(const NIdentifier &type, CodeGenContext &context)
{
    return context.typeSystem.getVarType(type);
}

static Value *CastToBoolean(CodeGenContext &context, Value *temp_Value)
{
    auto id = temp_Value->getType()->getTypeID();
    switch (id)
    {
    case (Type::IntegerTyID):
    {
        temp_Value = context.builder.CreateIntCast(temp_Value, Type::getInt1Ty(context.llvmContext), true);
        return context.builder.CreateICmpNE(temp_Value, ConstantInt::get(Type::getInt1Ty(context.llvmContext), 0, true));
    }
    case (Type::DoubleTyID):
    {
        return context.builder.CreateFCmpONE(temp_Value, ConstantFP::get(context.llvmContext, APFloat(0.0)));
    }
    default:
        return temp_Value;
    }

}

static llvm::Value *calcArrayIndex(shared_ptr<NArrayIndex> index, CodeGenContext &context)
{
    auto sizeVec = context.getArraySize(index->arrayName->name);
    if (!(sizeVec.size() > 0 && sizeVec.size() == index->expressions->size()))
    {
        throw std::invalid_argument("!(sizeVec.size() > 0 && sizeVec.size() == index->expressions->size())");
    }

    auto expression = *(index->expressions->rbegin());

    for (unsigned int i = sizeVec.size() - 1; i >= 1; i--)
    {
        auto temp = make_shared<NBinaryOperator>(make_shared<NInteger>(sizeVec[i]), TMUL,
                                                 index->expressions->at(i - 1));

        expression = make_shared<NBinaryOperator>(temp, TPLUS, expression);
    }

    return expression->codeGen(context);
}

void CodeGenContext::generateCode(NBlock &root)
{
    //this->llvmContext;
    //  root.llvmContext=CodeGenContext::llvmContext;

    using namespace llvm;
    cout << "Generating IR code" << endl;

    std::vector<Type *> sysArgs;
    FunctionType *mainFuncType = FunctionType::get(Type::getVoidTy(this->llvmContext), makeArrayRef(sysArgs), false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
    BasicBlock *block = BasicBlock::Create(this->llvmContext, "entry");

    pushBlock(block);
    Value *retValue = root.codeGen(*this);
    popBlock();

    cout << "Code generate success" << endl;
    cout << "IR Code  is :" << endl;

    //  llvm::legacy::PassManager passManager;
    //  passManager.add(createPrintModulePass(outs()));
    //  passManager.run(*(this->myModule.get()));

    llvm::legacy::PassManager *pm = new llvm::legacy::PassManager();
    int optLevel = 3;  //优化级别 0-3 3最高
    int sizeLevel = 0; //大小优化 0是无 2最大
    PassManagerBuilder builder;
    builder.OptLevel = optLevel;
    builder.SizeLevel = sizeLevel;
    builder.Inliner = createFunctionInliningPass(optLevel, sizeLevel);
    builder.DisableUnitAtATime = false;
    builder.DisableUnrollLoops = false; //If True, disable loop unrolling.
    builder.LoopVectorize = true;       //allow vectorizing loops
    builder.SLPVectorize = true;        //If True, enable the SLP vectorizer, which uses a different algorithm than the loop vectorizer. Both may be enabled at the same time.
    builder.populateModulePassManager(*pm);
    //  builder.DisableTailCalls
    (*pm).add(createPrintModulePass(outs()));
    (*pm).run(*(this->myModule.get()));

    return;
}

llvm::Value *NAssignment::codeGen(CodeGenContext &context)
{

    Value *dst = context.getSymbolValue(this->lhs->name);
    auto dstType = context.getSymbolType(this->lhs->name);
    string dstTypeStr = dstType->name;
    if (!dst)
    {
        throw std::logic_error("Undeclared variable");
    }
    Value *exp = exp = this->rhs->codeGen(context);

    exp = context.typeSystem.cast(exp, context.typeSystem.getVarType(dstTypeStr), context.currentBlock());
    context.builder.CreateStore(exp, dst);
    return dst;
}

llvm::Value *NBinaryOperator::codeGen(CodeGenContext &context)
{

    auto *L = this->lhs->codeGen(context);
    auto *R = this->rhs->codeGen(context);
    bool fp = false;

    if ((L->getType()->getTypeID() == Type::DoubleTyID) ||
        (R->getType()->getTypeID() == Type::DoubleTyID))
    { // type upgrade cast
        fp = true;
    }

    if (!L || !R)
    {
        return nullptr;
    }

    switch (this->op)
    {
    case TPLUS:
        return fp ? context.builder.CreateFAdd(L, R, "addftmp")
                  : context.builder.CreateAdd(L, R, "addtmp");
    case TMINUS:
        return fp ? context.builder.CreateFSub(L, R, "subftmp")
                  : context.builder.CreateSub(L, R, "subtmp");
    case TMUL:
        return fp ? context.builder.CreateFMul(L, R, "mulftmp")
                  : context.builder.CreateMul(L, R, "multmp");
    case TDIV:
        return fp ? context.builder.CreateFDiv(L, R, "divftmp")
                  : context.builder.CreateSDiv(L, R, "divtmp");
    case TAND:
        return fp ? throw std::logic_error("Double type has no AND operation")
                  : context.builder.CreateAnd(L, R, "andtmp");
    case TOR:
        return fp ? throw std::logic_error("Double type has no OR operation")
                  : context.builder.CreateOr(L, R, "or tmp");
    case TXOR:
        return fp ? throw std::logic_error("Double type has no XOR operation")
                  : context.builder.CreateXor(L, R, "xor tmp");
    case TCLT:
        return fp ? context.builder.CreateFCmpULT(L, R, "cmpftmp")
                  : context.builder.CreateICmpULT(L, R, "cmptmp");
    case TCLE:
        return fp ? context.builder.CreateFCmpOLE(L, R, "cmpftmp")
                  : context.builder.CreateICmpSLE(L, R, "cmptmp");
    case TCGE:
        return fp ? context.builder.CreateFCmpOGE(L, R, "cmpftmp")
                  : context.builder.CreateICmpSGE(L, R, "cmptmp");
    case TCGT:
        return fp ? context.builder.CreateFCmpOGT(L, R, "cmpftmp")
                  : context.builder.CreateICmpSGT(L, R, "cmptmp");
    case TCEQ:
        return fp ? context.builder.CreateFCmpOEQ(L, R, "cmpftmp")
                  : context.builder.CreateICmpEQ(L, R, "cmptmp");
    case TCNE:
        return fp ? context.builder.CreateFCmpONE(L, R, "cmpftmp")
                  : context.builder.CreateICmpNE(L, R, "cmptmp");
    default:
        throw std::logic_error("Unknown binary operator");
    }
}

llvm::Value *NBlock::codeGen(CodeGenContext &context)
{
    cout << "Generating block" << endl;
    Value *last = nullptr;
    for (auto it = this->statements->begin(); it != this->statements->end(); it++)
    {
        last = (*it)->codeGen(context);
    }
    return last;
}

llvm::Value *NInteger::codeGen(CodeGenContext &context)
{
    cout << "Generating Integer: " << this->value << endl;
    return ConstantInt::get(Type::getInt64Ty(context.llvmContext), this->value, true);
}

llvm::Value *NDouble::codeGen(CodeGenContext &context)
{
    cout << "Generating Double: " << this->value << endl;
    return ConstantFP::get(Type::getDoubleTy(context.llvmContext), this->value);
}

llvm::Value *NIdentifier::codeGen(CodeGenContext &context)
{
    cout << "Generating identifier " << this->name << endl;
    Value *value = context.getSymbolValue(this->name);
    if (!value)
    {
        throw std::logic_error("Unknown variable name " + this->name);
    }
    if (value->getType()->isPointerTy())
    {
        auto arrayPtr = context.builder.CreateLoad(value, "arrayPtr");
        if (arrayPtr->getType()->isArrayTy())
        {

            std::vector<Value *> indices;
            indices.push_back(ConstantInt::get(context.typeSystem.intTy, 0, false));
            auto ptr = context.builder.CreateInBoundsGEP(value, indices, "array Ptr");
            return ptr;
        }
    }
    return context.builder.CreateLoad(value, false, "");
}

llvm::Value *NExpressionStatement::codeGen(CodeGenContext &context)
{
    return this->expression->codeGen(context);
}

llvm::Value *NFunctionDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Generating function declaration of " << this->id->name << endl;
    std::vector<Type *> argTypes;

    for (auto &arg : *this->arguments)
    {
        if (arg->type->isArray)
        {
            argTypes.push_back(PointerType::get(context.typeSystem.getVarType(arg->type->name), 0));
        }
        else
        {
            argTypes.push_back(TypeOf(*arg->type, context));
        }
    }
    Type *retType = nullptr;
    if (this->type->isArray)
        retType = PointerType::get(context.typeSystem.getVarType(this->type->name), 0);
    else
        retType = TypeOf(*this->type, context);

    FunctionType *functionType = FunctionType::get(retType, argTypes, false);
    Function *function = Function::Create(functionType, GlobalValue::ExternalLinkage, this->id->name.c_str(),
                                          context.myModule.get());

    if (!this->isExternal)
    {
        BasicBlock *basicBlock = BasicBlock::Create(context.llvmContext, "entry", function, nullptr);

        context.builder.SetInsertPoint(basicBlock);
        context.pushBlock(basicBlock);

        // declare function params
        auto origin_arg = this->arguments->begin();

        for (auto &ir_arg_it : function->args())
        {
            ir_arg_it.setName((*origin_arg)->id->name);
            Value *argAlloc;
            if ((*origin_arg)->type->isArray)
                argAlloc = context.builder.CreateAlloca(
                    PointerType::get(context.typeSystem.getVarType((*origin_arg)->type->name), 0));
            else
                argAlloc = (*origin_arg)->codeGen(context);

            context.builder.CreateStore(&ir_arg_it, argAlloc, false);
            context.setSymbolValue((*origin_arg)->id->name, argAlloc);
            context.setSymbolType((*origin_arg)->id->name, (*origin_arg)->type);
            context.setFuncArg((*origin_arg)->id->name, true);
            origin_arg++;
        }

        this->block->codeGen(context);
        if (context.getCurrentReturnValue())
        {
            context.builder.CreateRet(context.getCurrentReturnValue());
        }
        else
        {
            throw std::logic_error("Function block return value not founded");
        }
        context.popBlock();
    }

    return function;
}

llvm::Value *NMethodCall::codeGen(CodeGenContext &context)
{
    cout << "Generating method call of " << this->id->name << endl;
    Function *calleeF = context.myModule->getFunction(this->id->name);
    if (calleeF == nullptr)
    {
        throw std::logic_error("Function name not found");
    }

    if (calleeF->arg_size() != this->arguments->size() && this->id->name != "printf")
    {
        throw std::logic_error("Function arguments size not match, calleeF=" + std::to_string(calleeF->size()) + ", this->arguments=" +
                               std::to_string(this->arguments->size()));
    }

    std::vector<Value *> argsv;
    for (auto it = this->arguments->begin(); it != this->arguments->end(); it++)
    {
        argsv.push_back((*it)->codeGen(context));
        if (!argsv.back())
        { // if any argument codegen fail
            return nullptr;
        }
    }
    return context.builder.CreateCall(calleeF, argsv, "calltmp");
}

llvm::Value *NVariableDeclaration::codeGen(CodeGenContext &context)
{
    cout << "Generating variable declaration of " << this->type->name << " " << this->id->name << endl;
    Type *type = TypeOf(*this->type, context);
    Value *initial = nullptr;

    Value *inst = nullptr;

    if (this->type->isArray)
    {
        int64_t arraySize = 1;
        std::vector<int64_t> arraySizes;
        for (auto it = this->type->arraySize->begin(); it != this->type->arraySize->end(); it++)
        {
            NInteger *integer = dynamic_cast<NInteger *>(it->get());
            arraySize *= integer->value;
            arraySizes.push_back(integer->value);
        }

        context.setArraySize(this->id->name, arraySizes);
        Value *arraySizeValue = NInteger(arraySize).codeGen(context);
        auto arrayType = ArrayType::get(context.typeSystem.getVarType(this->type->name), arraySize);
        inst = context.builder.CreateAlloca(arrayType, arraySizeValue, "arraytmp");
    }
    else
    {
        inst = context.builder.CreateAlloca(type);
    }

    context.setSymbolType(this->id->name, this->type);
    context.setSymbolValue(this->id->name, inst);

    //  context.PrintSymTable();

    if (this->assignmentExpr != nullptr)
    {
        NAssignment assignment(this->id, this->assignmentExpr);
        assignment.codeGen(context);
    }
    return inst;
}

llvm::Value *NReturnStatement::codeGen(CodeGenContext &context)
{
    cout << "Generating return statement" << endl;
    Value *returnValue = this->expression->codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

llvm::Value *NIfStatement::codeGen(CodeGenContext &context)
{
    cout << "Generating if statement" << endl;
    Value *temp_Value = this->condition->codeGen(context);
    if (!temp_Value)
        return nullptr;

    temp_Value = CastToBoolean(context, temp_Value);

    Function *theFunction = context.builder.GetInsertBlock()->getParent(); // the function where if statement is in

    BasicBlock *thenBB = BasicBlock::Create(context.llvmContext, "then", theFunction);
    BasicBlock *falseBB = BasicBlock::Create(context.llvmContext, "else");
    BasicBlock *mergeBB = BasicBlock::Create(context.llvmContext, "ifcont");

    if (this->falseBlock)
    {
        context.builder.CreateCondBr(temp_Value, thenBB, falseBB);
    }
    else
    {
        context.builder.CreateCondBr(temp_Value, thenBB, mergeBB);
    }

    context.builder.SetInsertPoint(thenBB);

    context.pushBlock(thenBB);

    this->trueBlock->codeGen(context);

    context.popBlock();

    thenBB = context.builder.GetInsertBlock();

    if (thenBB->getTerminator() == nullptr)
    {
        context.builder.CreateBr(mergeBB);
    }

    if (this->falseBlock)
    {
        theFunction->getBasicBlockList().push_back(falseBB);
        context.builder.SetInsertPoint(falseBB);

        context.pushBlock(thenBB);

        this->falseBlock->codeGen(context);

        context.popBlock();

        context.builder.CreateBr(mergeBB);
    }

    theFunction->getBasicBlockList().push_back(mergeBB);
    context.builder.SetInsertPoint(mergeBB);

    return nullptr;
}

llvm::Value *NForStatement::codeGen(CodeGenContext &context)
{

    Function *theFunction = context.builder.GetInsertBlock()->getParent();

    BasicBlock *block = BasicBlock::Create(context.llvmContext, "forloop", theFunction);
    BasicBlock *after = BasicBlock::Create(context.llvmContext, "forcont");

    // execute the initial
    if (this->initial)
        this->initial->codeGen(context);

    Value *temp_Value = this->condition->codeGen(context);
    if (!temp_Value)
        return nullptr;

    temp_Value = CastToBoolean(context, temp_Value);

    context.builder.CreateCondBr(temp_Value, block, after);

    context.builder.SetInsertPoint(block);

    context.pushBlock(block);

    this->block->codeGen(context);

    context.popBlock();

    if (this->increment)
    {
        this->increment->codeGen(context);
    }

    temp_Value = this->condition->codeGen(context);
    temp_Value = CastToBoolean(context, temp_Value);
    context.builder.CreateCondBr(temp_Value, block, after);

    theFunction->getBasicBlockList().push_back(after);
    context.builder.SetInsertPoint(after);

    return nullptr;
}

llvm::Value *NArrayIndex::codeGen(CodeGenContext &context)
{
    cout << "Generating array index expression of " << this->arrayName->name << endl;
    auto varPtr = context.getSymbolValue(this->arrayName->name);
    auto type = context.getSymbolType(this->arrayName->name);
    string typeStr = type->name;

    if (type->isArray == false)
    {
        throw std::invalid_argument("type.isArray is false");
    }

    auto value = calcArrayIndex(make_shared<NArrayIndex>(*this), context);
    ArrayRef<Value *> indices;
    if (context.isFuncArg(this->arrayName->name))
    {
        cout << "isFuncArg" << endl;
        varPtr = context.builder.CreateLoad(varPtr, "actualArrayPtr");
        indices = {value};
    }
    else if (varPtr->getType()->isPointerTy())
    {
        cout << this->arrayName->name << "Not isFuncArg" << endl;
        indices = {ConstantInt::get(Type::getInt64Ty(context.llvmContext), 0), value};
    }
    else
    {
        throw std::logic_error("The variable is not array");
    }
    auto ptr = context.builder.CreateInBoundsGEP(varPtr, indices, "elementPtr");

    return context.builder.CreateAlignedLoad(ptr, 4);
}

llvm::Value *NArrayAssignment::codeGen(CodeGenContext &context)
{
    auto varPtr = context.getSymbolValue(this->arrayIndex->arrayName->name);

    if (varPtr == nullptr)
    {
        throw std::logic_error("Unknown variable name");
    }

    auto arrayPtr = context.builder.CreateLoad(varPtr, "arrayPtr");

    if (!arrayPtr->getType()->isArrayTy() && !arrayPtr->getType()->isPointerTy())
    {
        throw std::logic_error("The variable is not array");
    }

    auto index = calcArrayIndex(arrayIndex, context);
    ArrayRef<Value *> gep2_array{ConstantInt::get(Type::getInt64Ty(context.llvmContext), 0), index};
    auto ptr = context.builder.CreateInBoundsGEP(varPtr, gep2_array, "elementPtr");

    return context.builder.CreateAlignedStore(this->expression->codeGen(context), ptr, 4);
}

llvm::Value *NArrayInitialization::codeGen(CodeGenContext &context)
{

    auto arrayPtr = this->declaration->codeGen(context);
    auto sizeVec = context.getArraySize(this->declaration->id->name);

    if (sizeVec.size() != 1)
    {
        throw std::invalid_argument("sizeVec.size is not 1");
    }

    for (int index = 0; index < this->expressionList->size(); index++)
    {
        shared_ptr<NInteger> indexValue = make_shared<NInteger>(index);
        shared_ptr<NArrayIndex> arrayIndex = make_shared<NArrayIndex>(this->declaration->id, indexValue);
        NArrayAssignment assignment(arrayIndex, this->expressionList->at(index));
        assignment.codeGen(context);
    }
    return nullptr;
}

llvm::Value *NLiteral::codeGen(CodeGenContext &context)
{
    return context.builder.CreateGlobalString(this->value, "string");
}
