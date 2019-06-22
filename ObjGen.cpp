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
#include "ObjGen.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace llvm;

void ObjGen(CodeGenContext &context)
{
    const string filename = "output.o";
    ObjGen(context, filename);
}
/*
rm test system_test
g++ -o system_test system_test.cpp
./system_test
./test
*/
void system_call_clang(string inputfilename, string outputfilename)
{

    string clang = "clang++ -O3 -o";
    //   string inputfilename = "output.o";
    //   string outputfilename = "test";
    string tab = "  ";
    string command = clang + tab + outputfilename + tab + inputfilename;
    cout << "run command:  " << command << endl;
    system(command.c_str());
    // system("clang++ -O3  output.o -o test ");
}
void ObjGen(CodeGenContext &context, string filename)
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
        errs() << error;
        return;
    }

    auto CPU = "generic";
    auto features = "";

    TargetOptions opt;
    auto RM = Optional<Reloc::Model>();
    auto theTargetMachine = Target->createTargetMachine(targetTriple, CPU, features, opt, RM);

    context.myModule->setDataLayout(theTargetMachine->createDataLayout());
    context.myModule->setTargetTriple(targetTriple);

    std::error_code EC;
    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::F_None);
    //    raw_fd_ostream dest(filename.c_str(), EC, sys::fs::F_None);
    //    formatted_raw_ostream formattedRawOstream(dest);

    legacy::PassManager pass;
    auto fileType = TargetMachine::CGFT_ObjectFile;

    if (theTargetMachine->addPassesToEmitFile(pass, dest, fileType))
    {
        errs() << "theTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*context.myModule.get());
    dest.flush();

    outs() << "Object code wrote to " << filename.c_str() << "\n";

    llvm_shutdown();

    system_call_clang(filename, "test");

    return;
}
