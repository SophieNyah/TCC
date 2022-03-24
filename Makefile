FLEX=flex
BISON=bison
CC=g++
FLAGS=-std=c++17

PROGRAMA = generator
LEXICO = lexer.l
SINTATICO = parser.y

$(PROGRAMA): $(LEXICO) $(SINTATICO) 
	$(BISON) -d $(SINTATICO)
	$(FLEX) $(LEXICO)
	$(CC) -c *.cpp -I. $(FLAGS)
	$(CC) *.o -o $(PROGRAMA) $(FLAGS)

clean:
	rm -f parser.cpp
	rm -f parser.hpp
	rm -f scanner.cpp
	rm -f scanner.hpp
	rm -f location.hh
	rm -f *.o
	rm -f *.exe
	rm -f generator
