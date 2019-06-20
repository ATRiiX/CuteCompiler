#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
rm test
g++ -o system_test system_test.cpp
./system_test
./test
*/
using namespace std;
int main(int argc, char **argv) {
    string command = "clang++ -O3 -o";
    string inputfilename = "output.o";
    string outputfilename = "test";
    system((command+outputfilename+inputfilename).c_str());
   // system("clang++ -O3  output.o -o test ");
    return 0;
}