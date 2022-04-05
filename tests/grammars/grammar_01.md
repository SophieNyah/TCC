
%term ADD
%term SUB
%term MEM
%nonterm mais

{
#include<iostream>
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
    Tree mais{"mais", Non_terminals::mais, operacao}, r1{"r1", 0, registrador}, c1{"c1", 0, registrador};
    mais.insertChild(r1);
    mais.insertChild(c1);
}
}
