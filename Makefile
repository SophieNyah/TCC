FLEX=flex
BISON=bison
CC=g++
FLAGS=-std=c++17

PROGRAMA = generator

LEXICO = lexer.l
LEXICO_FLAGS = --outfile=src/scanner.cpp --header-file=src/scanner.hpp

SINTATICO = parser.y
SINTATICO_FLAGS = --output=src/parser.cpp -d

OBJECTS = Helper.o Rule.o Tree.o CodeGen.o parser.o scanner.o RegAlloc.o
OUT_PATH=./output/src
SRC_PATH=./src


generator: parser objs
	$(CC) $(OUT_PATH)/*.o -o $(OUT_PATH)/$(PROGRAMA) $(FLAGS)


objs: $(OBJECTS)

$(OBJECTS): %.o: $(SRC_PATH)/%.cpp
	$(CC) $(FLAGS) -c $< -o $(OUT_PATH)/$@


parser: $(SRC_PATH)/$(LEXICO) $(SRC_PATH)/$(SINTATICO)
	$(BISON) $(SINTATICO_FLAGS) $(SRC_PATH)/$(SINTATICO)
	$(FLEX) $(LEXICO_FLAGS) $(SRC_PATH)/$(LEXICO) 
	

clean:
	rm -f src/parser.cpp
	rm -f src/parser.hpp
	rm -f src/scanner.cpp
	rm -f src/scanner.hpp
	rm -f src/location.hh
	rm -f output/src/*.o
	rm -f output/src/generator

clean_tests:
	rm -f output/grammars/*.hpp
	rm -f output/grammars/*.cpp
	rm -f output/grammars/*.o

clean_all: clean clean_tests
