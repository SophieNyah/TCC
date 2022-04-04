#include<algorithm>
#include<iostream>
#include"Helper.hpp"

using namespace std;

    /* Definição dos atributos */
bool          Helper::hadError{false};
array<string> Helper::terminals{};
array<string> Helper::non_terminals{};
array<Rule>   Helper::rules{};
code_t        Helper::user_code{};



    /* Funções de suporte */
void insertElement(array<string> &arr, const string &str){ arr.push_back(str); }
void insertElement(array<Rule>& arr, const Rule& r){ arr.push_back(r); }

int findElement(const array<string> &arr, const string &str){
    auto it = find(arr.begin(), arr.end(), str);
    return  it != arr.end()  ?  it-arr.begin()  :  -1;
}



    /* Definição dos métodos */
bool Helper::isTerm(const string &str)   { return findElement(Helper::terminals, str) >= 0 ? true : false; }
bool Helper::isNonTerm(const string &str){ return findElement(Helper::non_terminals, str) >= 0 ? true : false; }

void Helper::newTerm(const string &str){ insertElement(Helper::terminals, str); }
void Helper::newNonTerm(const string &str){ insertElement(Helper::non_terminals, str); }
void Helper::newRule(const Rule& r){ insertElement(Helper::rules, r); }


    /* Getters e Setters */
array<Rule>& Helper::getRules(){ return Helper::rules; }

void Helper::setCode(code_t& code){ Helper::user_code = code; }
code_t& Helper::getCode(){ return Helper::user_code; }


array<string>& Helper::getTerms(){ return Helper::terminals; }
array<string>& Helper::getNonTerms(){ return Helper::non_terminals; }

bool Helper::getError(){ return Helper::hadError; }
void Helper::setError(){ Helper::hadError = true; }
void Helper::semanticError(const string &str){
    cerr << "Error: " << str << '\n';
    Helper::setError();
}
