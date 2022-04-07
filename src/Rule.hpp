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
        friend ostream& operator<<(ostream& out, Pattern& s);
};


class Rule {

    private:

        static int rules_quantity;

        int replace{ -1 };
        int rule_no;
        string name;
        code_t cost{ "" };
        vector<Pattern> patterns;
        BasicTree tree_pattern;
    
    public: 

        Rule();
        Rule(string name, vector<Pattern> patterns, BasicTree root, int replace = -1, code_t cost = "");

        string getName();
        int getRuleNumber();
        code_t getCost();
        BasicTree getPattern();

        friend ostream& operator<<(ostream&, Rule&);

};

#endif