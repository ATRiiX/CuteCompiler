#include <iostream>
#include <fstream>
#include <iomanip>
#include "ASTNodes2.h"
#include "CodeGen.h"
#include "ObjGen.h"
#include "json.hpp"

extern shared_ptr <NBlock> programBlock;

extern int yyparse();
//void createCoreFunctions(CodeGenContext& context);



using namespace std;
using json = nlohmann::json;
//extern int yyparse();
//extern NBlock* programBlock;

int main(int argc, char **argv) {
    yyparse();

    cout<<"AST JSON Generate start!!"<<endl;
    json j = programBlock->AST_JSON_Generate();
    std::cout << j.dump(4) << std::endl;



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