#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/Optional.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/IR/LegacyPassManager.h>

#include "CodeGen.h"
#include "ObjGenerate.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace llvm;

void ObjGenerate(CodeGenContext &context)
{
    const string file = "output.o";
    ObjGenerate(context, file);
}

void syscall(string inputfile, string outputfile)
{

    string clang = "clang++ -O3 -o";
    //   string inputfile = "output.o";
    //   string outputfile = "test";
    string tab = "  ";
    string cmd = clang + tab + outputfile + tab + inputfile;
    cout << "run cmd:  " << cmd << endl;
    system(cmd.c_str());
}
void ObjGenerate(CodeGenContext &context, string file)
{

    outs() << "Object code start generate "
           << "\n";
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    auto targetTriple = sys::getDefaultTargetTriple();
    context.myModule->setTargetTriple(targetTriple);

    std::string error;
    auto Target = TargetRegistry::lookupTarget(targetTriple, error);

    if (!Target)
    {
        throw std::invalid_argument(error);
    }

    auto CPU = "generic";
    auto features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto theTargetMachine = Target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    context.myModule->setDataLayout(theTargetMachine->createDataLayout());
    context.myModule->setTargetTriple(targetTriple);

    std::error_code EC;
    raw_fd_ostream dest(file.c_str(), EC, sys::fs::F_None);

    legacy::PassManager pass;
    auto fileType = TargetMachine::CGFT_ObjectFile;

    if (theTargetMachine->addPassesToEmitFile(pass, dest, fileType))
        throw std::invalid_argument("theTargetMachine can't emit a file of this type");

    pass.run(*context.myModule.get());
    dest.flush();

    outs() << "Object code wrote to " << file.c_str() << "\n";

    llvm_shutdown();

    syscall(file, "test");

    return;
}
