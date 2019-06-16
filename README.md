# CuteCompiler



```js
sudo apt-get install libjsoncpp-dev build-essential zlib1g-dev libssl-dev libsqlite3-dev libbz2-dev libreadline-dev           
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json          
sudo apt-get install libncurses-dev build-essential linux-headers-`uname -r` flex bison llvm-3.9          

git clone https://github.com/stardust95/TinyCompiler.git          
cd TinyCompiler           
make          

make test          
clang++  output.o -o test          
./test          

cat tests/testArray.input | ./compiler          
clang++  output.o -o test          
./test          


cat tests/testBasic.input | ./compiler          
clang++  output.o -o test          
./test          



yacc -d grammar.y   #����   y.tab.c              
bison -d -o grammar.cpp grammar.y          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o grammar.o grammar.cpp          
flex -o token.cpp token.l          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o token.o token.cpp          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o CodeGen.o CodeGen.cpp          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o utils.o utils.cpp          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o main.o main.cpp          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o ObjGen.o ObjGen.cpp          
g++ -c `llvm-config-3.9 --cppflags` -std=c++11 -o TypeSystem.o TypeSystem.cpp          
g++ `llvm-config-3.9 --cppflags` -std=c++11 -o compiler grammar.o token.o CodeGen.o utils.o main.o ObjGen.o TypeSystem.o  `llvm-config-3.9 --libs` `llvm-config-3.9 --ldflags` -lpthread -ldl -lz -lncurses -rdynamic -L/usr/local/lib -ljsoncpp          

```





