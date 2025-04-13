BIN = bin

OBJS = parser.o  

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++14
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

###############################################

parser.o : parser_y parser_cpp 

parser_cpp: parser/parser.cpp 
	@clang++ -gfull -c $(CPPFLAGS) -o $(BIN)/parser.o $^

parser_y: parser/parser.y
	@bison -d -o parser/parser.cpp $^

###############################################

lexer.o: lexer_l lexer_cpp 

lexer_cpp: lexer/lexer.cpp 
	@clang++ -gfull -c $(CPPFLAGS) -o $(BIN)/lexer.o $^

lexer_l: lexer/lexer.l
	@flex -olexer/lexer.cpp $^

###############################################

%.o: %.cpp
	@clang++ -gfull -c $(CPPFLAGS) -o  $(BIN)/$@ $<

###############################################

parser: $(OBJS)
	clang++  -gfull -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

###############################################

test: toy example/example.toy
	cat example/example.txt | ./toy

###############################################

clean: 
	@rm -rf *.o
	@rm -rf parser/*.cpp
	@rm -rf parser/*.hpp
	@rm -rf lexer/*.hpp
