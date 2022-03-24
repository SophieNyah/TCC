#ifndef RULE_H
#define RULE_H

#include<string>
#include<vector>
#include"Tree.hpp"
#include"types.hpp"

using namespace std;

typedef struct pattern{
    string name;
    vector<string> variables;
} Pattern;


class Rule {

    private:

        int replace{ -1 };
        string name;
        code_t cost{ "" };
        vector<Pattern> patterns;
        Tree tree_pattern;
    
    public: 

        Rule(string name, Tree root, vector<Pattern> patterns, int replace = -1, code_t cost = "");

};

#endif