#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "ASTNodes2.h"
#include "CodeGen.h"
#include "ObjGen.h"
#include "json.hpp"

extern shared_ptr<NBlock> programBlock;

extern int yyparse();

using namespace std;
using json = nlohmann::json;

int main(int argc, char **argv)
{
    try
    {
        yyparse();

        cout << "AST JSON Generate start!!" << endl;
        json j = programBlock->AST_JSON_Generate();
        std::cout << j.dump(4) << std::endl;

        CodeGenContext context;
        context.generateCode(*programBlock);

        ObjGen(context);

        string jsonFile = "AST.json";
        std::ofstream outFile(jsonFile);
        outFile << std::setw(4) << j << std::endl;
        cout << "AST json write to " << jsonFile << endl;

        return 0;
    }
    catch (const std::logic_error &e)
    {
         cout << "logic_error error catched!" << endl;
        std::cout << e.what() << std::endl;
        return 1;
    }
    catch (std::exception &e)
    {
        cout << "unexpected error catched!" << endl;
        std::cout << e.what() << std::endl;
        return -1;
    }catch(...) { 
        cout << "other error catched!"; 
         return -2;
    } 
}