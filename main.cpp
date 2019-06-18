#include <iostream>
#include <fstream>
#include <iomanip>
#include "ASTNodes2.h"
#include "CodeGen.h"
#include "ObjGen.h"
#include "json.hpp"

extern shared_ptr <NBlock> programBlock;

extern int yyparse();
// extern void yyparse_init(const char* filename);
// extern void yyparse_cleanup();
//
//void createCoreFunctions(CodeGenContext& context);

//#include <iostream>
//#include "CodeGen.h"
//#include "ASTNodes2.h"

using namespace std;
using json = nlohmann::json;
//extern int yyparse();
//extern NBlock* programBlock;

int main(int argc, char **argv) {
    yyparse();

    // std::cout << programBlock << std::endl;
   // programBlock->print("--");

    json j = programBlock->AST_JSON_Generate();

    std::cout << j.dump(4) << std::endl;

    

//    cout << root;

//    cout << root << endl;
    CodeGenContext context;
//    createCoreFunctions(context);
    context.generateCode(*programBlock);
    ObjGen(context);

    
    string jsonFile = "AST.json";
    std::ofstream outFile(jsonFile);
    outFile << std::setw(4) << j << std::endl;
    cout << "AST json write to " << jsonFile << endl;
    

    
    
    return 0;
}