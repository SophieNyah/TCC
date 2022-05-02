{
#include<iostream>

User_Symbols getSymbolFromStr(string s){
    if(s == "ADD")  return User_Symbols::ADD;
    if(s == "SUB")  return User_Symbols::SUB;
    if(s == "MUL")  return User_Symbols::MUL;
    if(s == "DIV")  return User_Symbols::DIV;
    if(s == "MEM")  return User_Symbols::MEM;
    if(s == "CONST")  return User_Symbols::CONST;
    if(s == "reg")  return User_Symbols::reg;
    if(s == "stmt")  return User_Symbols::stmt;
}
Node_type getNodeTypeFromStr(string s){
    if(s == "c")    return Node_type::constante;
    if(s == "r")    return Node_type::registrador;
    if(s[0] == '#') return Node_type::especifico;
                    return Node_type::operacao;
}

class Tree: public VirtualTree<Tree>{
    public:

        Tree(){}
        Tree(const string& name, const User_Symbols user_symbol, const Node_type& type)
            : VirtualTree{ name, user_symbol, type }
            {}

        std::pair<Tree, bool> readTreeRecursion(Tree& t){
            char c;
            bool endOfCurrChildren{ false };
            while(std::cin>>c){
                if(isspace(c)) continue;

                if(isalnum(c)){
                    string s{ c };
                    while(std::cin>>c){

                        if(isalnum(c)){
                            s.push_back(c);
                        }else if( c=='(' || c==',' || c==')' ){
                            t.name = s;
                            t.user_symbol = (int)getSymbolFromStr(s);
                            t.type = getNodeTypeFromStr(s);
                            if( c=='(' ){
                                do{
                                    Tree t1{};
                                    auto pair = this->readTreeRecursion(t1);
                                    t1 = pair.first;
                                    endOfCurrChildren = pair.second;
                                    t.insertChild(t1);
                                }while(!endOfCurrChildren);
                            }else if( c==',' ){
                                return std::pair<Tree, bool>{ t, false };
                            }else if( c==')' ){
                                return std::pair<Tree, bool>{ t, true };
                            }
                        }
                    }

                }

            }
            return std::pair<Tree, bool>{ t, true };
        }

        Tree readTree(Tree& t) override {
            return readTreeRecursion(t).first;
        }

        int coisa1;
        int coisa2;

        friend ostream& operator<<(ostream&, Tree&);

};

ostream& operator<<(ostream& out, Tree& tree){
    out << tree.name;
    int children = tree.getChildren().size();
    
    if( children > 0 ){ out << "(" << tree.action << " "; }
    for( int i=0; i<children; i++){
        Tree t = tree.getChild(i).value();
        out << t;
        if(i < children - 1){
            out << ", ";
        }
    }
    if( children > 0 ){ out << ")"; }
    
    return out;
}
}

%term ADD
%term SUB
%term MUL
%term DIV
%term MEM
%term CONST
%nonterm reg
%nonterm stmt

%%

$! regist $% 
    [ADD $[regA, regB, conA$]] 1 { std::cout << "Registrador\n"; } <-
        reg: CONST
$% { return 1; } $!

$! statement $% 
    [ADD $[regA, regB, conA$]] 1 { std::cout << "Statement\n"; } <-
        stmt: reg
$% { return $cost[0]; } $!

$! addConst  $%
    [ADD $[regA, regB, regC$]] 3 { std::cout << "ADD constante\n"; } <-
        reg: ADD(reg, CONST)
$% { return $cost[1] + 1; } $!

$! addReg $% 
    [ADD $[regA, regB, conA$]] 2 { std::cout << "ADD registrador\n"; } <-
        reg: ADD(reg, reg) 
$% { return $cost[1] + $cost[2] + 1; } $!

$! loadAdd $%
    [ADD $[ s1, s2 $]; MEM $[ s3, s2 $]] { std::cout << "LOAD ADD\n"; } <-
        stmt: MEM( ADD( reg, CONST ), reg )
$% { return $cost[2] + $cost[4] + 2; } $!

$! loadReg $%
    [ADD $[ s1, s2 $]; MEM $[ s3, s2 $]] { std::cout << "LOAD registrador\n"; } <-
        stmt: MEM( reg, reg )
$% { return $cost[1] + $cost[2] + 2; } $!

$! mul $%
    [ADD $[ s1, s2 $]; MEM $[ s3, s2 $]] { std::cout << "MUL\n"; } <-
        reg: MUL( reg, reg )
$% { return $cost[1] + $cost[2] + 2; } $!

%%

{

using namespace Code_Generator;

void printLabels(Tree t){
    std::cout << "labels de " << t.getName() << "(";
    for( auto rule: t.matched_rules ) std::cout << (int)rule.first << ", ";
    std::cout << ")\n";

    for( Tree t: t.getChildren()) printLabels(t);
}

int main(){
    Tree t{};
    t = t.readTree(t);
    label(t);
    printLabels(t);
}

}
