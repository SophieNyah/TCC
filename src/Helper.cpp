#include<algorithm>
#include<iostream>
#include"Helper.hpp"

using namespace std;

    /* Definição dos atributos */
bool Helper::hadError{false};
array<string> Helper::terminals{};
array<string> Helper::non_terminals{};
array<Rule>   Helper::rules{};



    /* Funções de suporte */
void insertElement(array<string> &arr, const string &str){ arr.push_back(str); }
void insertElement(array<Rule>& arr, const Rule& r){ arr.push_back(r); }

bool findElement(const array<string> &arr, const string &str){
    auto it = find(arr.begin(), arr.end(), str);
    return  it != arr.end()  ?  true  :  false;
}



    /* Definição dos métodos */
bool Helper::isTerm(const string &str)   { return findElement(Helper::terminals, str); }
bool Helper::isNonTerm(const string &str){ return findElement(Helper::non_terminals, str); }

void Helper::newTerm(const string &str){ insertElement(Helper::terminals, str); }
void Helper::newNonTerm(const string &str){ insertElement(Helper::non_terminals, str); }
void Helper::newRule(const Rule& r){ insertElement(Helper::rules, r); }

array<Rule>& Helper::getRules(){ return Helper::rules; }

bool Helper::getError(){ return Helper::hadError; }
void Helper::setError(){ Helper::hadError = true; }
void Helper::semanticError(const string &str){
    cerr << "Error: " << str << '\n';
    Helper::setError();
}
