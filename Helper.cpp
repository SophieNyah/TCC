#include<algorithm>
#include<iostream>
#include"Helper.hpp"

using namespace std;

bool Helper::hadError{false};
list<string> Helper::terminals{};
list<string> Helper::non_terminals{};

void Helper::insertIntoList(list<string> &list, const string &str){
    list.push_back(str);
}

bool Helper::findInList(const list<string> &list, const string &str){
    auto it = find(list.begin(), list.end(), str);
    return  it != list.end()  ?  true  :  false;
}

Helper::Helper(){}

bool Helper::error(){ return Helper::hadError; }

bool Helper::isTerm(const string &str)   { return findInList(Helper::terminals, str); }
bool Helper::isNonTerm(const string &str){ return findInList(Helper::non_terminals, str); }

void Helper::newTerm(const string &str){ return insertIntoList(Helper::terminals, str); }
void Helper::newNonTerm(const string &str){ return insertIntoList(Helper::non_terminals, str); }

void Helper::semanticError(const string &str){
    cerr << "Error: " << str << '\n';
    Helper::hadError = true;
}
