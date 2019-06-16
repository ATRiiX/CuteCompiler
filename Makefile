all: compiler

OBJS = grammar.o \
		token.o  \
		CodeGen.o \
		utils.o \
		main.o	 \
		ObjGen.o \
		TypeSystem.o \

LLVMCONFIG = llvm-config-3.9
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic -L/usr/local/lib -ljsoncpp
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf grammar.cpp grammar.hpp test compiler tokens.cpp *.output $(OBJS)

ObjGen.cpp: ObjGen.h

CodeGen.cpp: CodeGen.h ASTNodes.h

grammar.cpp: grammar.y
	bison -d -o $@ $<

grammar.hpp: grammar.cpp

token.cpp: token.l grammar.hpp
	flex -o $@ $<

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

test: compiler test.input
	cat test.input | ./compiler

testlink: output.o testmain.cpp
	clang output.o testmain.cpp -o test
	./test
