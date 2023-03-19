FLEX=flex
BISON=bison
CC=g++
FLAGS=-std=c++17 -g

PROGRAMA = generator

LEXICO = lexer.l
LEXICO_FLAGS = --outfile=src/scanner.cpp --header-file=src/scanner.hpp

SINTATICO = parser.y
SINTATICO_FLAGS = --output=src/parser.cpp -d

OBJECTS = Helper.o Rule.o Tree.o CodeGen.o parser.o scanner.o RegAlloc.o
OUT_PATH=./output/src
LIB_PATH=./output/lib
SRC_PATH=./src


generator: parser objs
	$(CC) $(OUT_PATH)/*.o -o $(OUT_PATH)/$(PROGRAMA) $(FLAGS)


objs: $(OBJECTS)

$(OBJECTS): %.o: $(SRC_PATH)/%.cpp
	$(CC) $(FLAGS) -c $< -o $(OUT_PATH)/$@


parser: $(SRC_PATH)/$(LEXICO) $(SRC_PATH)/$(SINTATICO)
	$(BISON) $(SINTATICO_FLAGS) $(SRC_PATH)/$(SINTATICO)
	$(FLEX) $(LEXICO_FLAGS) $(SRC_PATH)/$(LEXICO) 
	

lib:
	ar rcs $(LIB_PATH)/libyamg.a $(OUT_PATH)/CodeGen.o $(OUT_PATH)/Helper.o $(OUT_PATH)/parser.o $(OUT_PATH)/RegAlloc.o $(OUT_PATH)/Rule.o $(OUT_PATH)/scanner.o $(OUT_PATH)/Tree.o
	cp $(SRC_PATH)/RegAlloc.hpp $(LIB_PATH)/

lib_user:
	ar rcs ./user/src/lib/yamg/libyamg.a $(OUT_PATH)/CodeGen.o $(OUT_PATH)/Helper.o $(OUT_PATH)/parser.o $(OUT_PATH)/RegAlloc.o $(OUT_PATH)/Rule.o $(OUT_PATH)/scanner.o $(OUT_PATH)/Tree.o
	cp $(SRC_PATH)/RegAlloc.hpp ./user/src/lib/yamg/

clean:
	rm -f src/parser.cpp
	rm -f src/parser.hpp
	rm -f src/scanner.cpp
	rm -f src/scanner.hpp
	rm -f src/location.hh
	rm -f output/src/*.o
	rm -f output/src/generator
	rm -f output/lib/libyamg.a
	rm -f output/lib/*.hpp

clean_tests:
	rm -f output/grammars/*.hpp
	rm -f output/grammars/*.cpp
	rm -f output/src/*.hpp
	rm -f output/src/*.cpp
	rm -f output/grammars/*.o

clean_all: clean clean_tests
