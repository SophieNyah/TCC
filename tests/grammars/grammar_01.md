
%term ADD
%term SUB
%term MEM
%nonterm mais

{
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

}

%nonterm menos
%nonterm div
%nonterm mul
%nonterm mem

%%

$! addConst  $%
    [ADD $[regA, regB, regC$]] 3 <-
        mais(r1, c1)
$% { $cost[0] + 3 } $!

$! addReg $% 
    [ADD $[regA, regB, conA$]] 2 <-
        mais(r1, r2) 
$% { 2 } $!

$! load $%
    [ADD $[ s1, s2 $]; MEM $[ s3, s2 $]] <-
        mem( mais( r1, div(r1, r2) ), menos(c1, c2) )
$% { $cost[1] + 2 } $!

%%

{
int main(){
    Tree t{};
    t = t.readTree();
    std::cout << t.getChild(0).value().coisa1;
}
}
