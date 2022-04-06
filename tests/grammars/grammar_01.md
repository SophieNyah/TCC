
%term ADD
%term SUB
%term MEM
%nonterm mais

{
#include<iostream>

class UserTree: public BaseUserTree{
    public:

        UserTree(const string& name, const int non_term, const Node_type& type)
            : BaseUserTree{ name, non_term, type }
            {}

        Tree readTree(){
            Tree t1{ "mais", mais, Node_type::operacao};
            t1.insertChild(Tree{ "r1", 0, Node_type::registrador });
            t1.insertChild(Tree{ "r2", 0, Node_type::registrador });
            return t1;
        }
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
    UserTree ut{}
}
}
