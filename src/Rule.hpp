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

        int replace{ -1 };
        string name;
        code_t cost{ "" };
        vector<Pattern> patterns;
        Tree tree_pattern;
    
    public: 

        Rule();
        Rule(string name, vector<Pattern> patterns, Tree root, int replace = -1, code_t cost = "");

        friend ostream& operator<<(ostream&, Rule&);

};

#endif