#ifndef RULE_H
#define RULE_H

#include<string>
#include<vector>
#include<iostream>
#include"Tree.hpp"
#include"types.hpp"

using namespace std;

struct Pattern{
    string name;
    vector<string> variables;

    public:
        Pattern(string name, vector<string> variables);
};


class Rule {

    private:

        static int rules_quantity;

        int replace{ -1 };
        int rule_no;
        string user_symbol;
        string name;
        code_t cost{ "" };
        code_t action{ "" };
        vector<Pattern> patterns;
        BasicTree tree_pattern;
    
    public: 

        Rule();
        Rule(string name, vector<Pattern> patterns, BasicTree root, string non_term, code_t action, int replace = -1, code_t cost = "");

        int getRuleNumber();
        string getName();
        string getSymbol();
        code_t getCost();
        void   setAction(code_t);
        code_t getAction();
        BasicTree getPattern();

};

#endif