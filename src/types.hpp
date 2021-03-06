#ifndef TYPES_H
#define TYPES_H

#include<string>
#include<vector>

using code_t = std::string;

template <typename T>
using MyArray = std::vector<T>;

enum class Node_type{ operacao, registrador, constante, especifico };

struct Cost_expression{
    std::vector<int> cost_directives;
    int integer_part;
    Cost_expression(){}
    Cost_expression(std::vector<int> c, int i=0):cost_directives{ c }, integer_part{ i }{}
};

    /* Tipos que estarão no código gerado */
enum class User_Symbols: int;
enum class Rules: int;

using cost_t = int;

#endif