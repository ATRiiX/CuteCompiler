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
    string command = "clang++ -O3 -o";
    string inputfilename = "output.o";
    string outputfilename = "test";
    system((command+outputfilename+inputfilename).c_str());
   // system("clang++ -O3  output.o -o test ");
    return 0;
}