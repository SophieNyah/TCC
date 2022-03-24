#include"Rule.hpp"

Rule::Rule(string name, Tree root, vector<Pattern> patterns, int replace, code_t cost):
    name{ name },
    tree_pattern{ root },
    patterns{ patterns },
    replace{ replace },
    cost{ cost }
    {}
