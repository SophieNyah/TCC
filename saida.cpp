#include"./src/Tree.hpp"

enum class Rules {
    null = -1,
    addConst = 0,
    addReg = 1,
    load = 2,
};

enum class Non_terminals {
    null = -1,
    div = 2,
    mais = 0,
    mem = 4,
    menos = 1,
    mul = 3,
};

#include<iostream>

class Tree: public VirtualTree<Tree>{
    public:

        Tree(){}
        Tree(const string& name, const Non_terminals non_term, const Node_type& type)
            : VirtualTree{ name, non_term, type }
            {}

        Tree readTree() override {
            Tree t1{ string("mais"), Non_terminals::mais, Node_type::operacao};
            Tree t2{ string("r1"), Non_terminals::null, Node_type::registrador };
            t2.coisa1 = 3;
            t1.insertChild(t2);
            t1.insertChild(Tree{ string("r2"), Non_terminals::null, Node_type::registrador });
            return t1;
        }

        int coisa1;
        int coisa2;

};

namespace Code_Generator{

using cost_t = int;
using rule_number_t = Rules;
using MyPair = std::pair<rule_number_t, cost_t>;

int recognition_table[14][8]{
    {1, 0, 0, 0, 5, 0, 0, 0},
    {0, 0, 0, 0, 0, 2, 0, 0},
    {0, 0, 0, 0, 0, 4, 3, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {6, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 7, 0, 0},
    {0, 0, 8, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 9, 0, 0},
    {0, 0, 0, 0, 0, 10, 0, 0},
    {0, 11, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 12, 0},
    {0, 0, 0, 0, 0, 0, 13, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
};

MyPair isFinalState(int state){
    switch(state){
        case 3: return MyPair{ Rules::addConst, 3 };
        case 4: return MyPair{ Rules::addReg, 3 };
        case 13: return MyPair{ Rules::load, 3 };
        default: return MyPair{Rules::null, -1};
    }
}

}


int main(){
    Tree t{};
    t = t.readTree();
    std::cout << t.getChild(0).value().coisa1;
}
