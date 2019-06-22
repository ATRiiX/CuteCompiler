#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
/*
rm test system_test
g++ -o system_test system_test.cpp
./system_test
./test
*/
using namespace std;
using string=std::string;
int main(int argc, char **argv) {
    string cmd = "clang++ -O3 -o";
    string inputfile = "output.o";
    string outputfile = "test";
    string tab="    ";
    system((cmd+tab+outputfile+tab+inputfile).c_str());
   // system("clang++ -O3  output.o -o test ");
    return 0;
}