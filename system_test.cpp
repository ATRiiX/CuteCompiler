#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
rm test
g++ -o system_test system_test.cpp
./system_test

*/
using namespace std;
int main(int argc, char **argv) {
    system("clang++ -O3  output.o -o test ");
    return 0;
}