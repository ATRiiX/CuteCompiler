#ifndef __CODEGEN_H__
#define __CODEGEN_H__


#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <json/json.h>

#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "ASTNodes2.h"
#include "grammar.hpp"
#include "TypeSystem.h"

using namespace llvm;
using std::unique_ptr;
using std::string;

using SymTable = std::map<string, Value *>;

class CodeGenBlock {
public:
    BasicBlock *block;
    Value *returnValue;
    std::map<string, Value *> locals;
    std::map <string, shared_ptr<NIdentifier>> types;     // type name string of vars
    std::map<string, bool> isFuncArg;
    std::map <string, std::vector<int64_t>> arraySizes;
};

class CodeGenContext {
private:
    std::vector<CodeGenBlock *> blockStack;

public:
    LLVMContext llvmContext;
    IRBuilder<> builder;
    unique_ptr <Module> myModule;
    SymTable globalVars;
    TypeSystem typeSystem;
    NBlock root;
    CodeGenContext() : builder(llvmContext), typeSystem(llvmContext) {
        myModule = unique_ptr<Module>(new Module("main", this->llvmContext));
    }

    Value *getSymbolValue(string name) const {
        for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++) {
            if ((*it)->locals.find(name) != (*it)->locals.end()) {
                return (*it)->locals[name];
            }
        }
        return nullptr;
    }

    shared_ptr <NIdentifier> getSymbolType(string name) const {
        for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++) {
            if ((*it)->types.find(name) != (*it)->types.end()) {
                return (*it)->types[name];
            }
        }
        return nullptr;
    }

    bool isFuncArg(string name) const {

        for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++) {
            if ((*it)->isFuncArg.find(name) != (*it)->isFuncArg.end()) {
                return (*it)->isFuncArg[name];
            }
        }
        return false;
    }

    void setSymbolValue(string name, Value *value) {
        blockStack.back()->locals[name] = value;
    }

    void setSymbolType(string name, shared_ptr <NIdentifier> value) {
        blockStack.back()->types[name] = value;
    }

    void setFuncArg(string name, bool value) {
        cout << "Set " << name << " as func arg" << endl;
        blockStack.back()->isFuncArg[name] = value;
    }

    BasicBlock *currentBlock() const {
        return blockStack.back()->block;
    }

    void pushBlock(BasicBlock *block) {
        CodeGenBlock *codeGenBlock = new CodeGenBlock();
        codeGenBlock->block = block;
        codeGenBlock->returnValue = nullptr;
        blockStack.push_back(codeGenBlock);
    }

    void popBlock() {
        CodeGenBlock *codeGenBlock = blockStack.back();
        blockStack.pop_back();
        delete codeGenBlock;
    }

    void setCurrentReturnValue(Value *value) {
        blockStack.back()->returnValue = value;
    }

    Value *getCurrentReturnValue() {
        return blockStack.back()->returnValue;
    }

    void setArraySize(string name, std::vector <int64_t> value) {
        cout << "setArraySize: " << name << ": " << value.size() << endl;
        blockStack.back()->arraySizes[name] = value;
    }

    std::vector <int64_t> getArraySize(string name) {
        for (auto it = blockStack.rbegin(); it != blockStack.rend(); it++) {
            if ((*it)->arraySizes.find(name) != (*it)->arraySizes.end()) {
                return (*it)->arraySizes[name];
            }
        }
        return blockStack.back()->arraySizes[name];
    }


    void generateCode(NBlock &);
};



#endif