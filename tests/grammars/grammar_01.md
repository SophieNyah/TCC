{
#include<iostream>

User_Symbols getSymbolFromStr(string s){
    if(s == "ADD")  return User_Symbols::ADD;
    if(s == "MUL")  return User_Symbols::MUL;
    if(s == "DIV")  return User_Symbols::DIV;
    if(s == "MEM")  return User_Symbols::MEM;
    if(s == "CONST")return User_Symbols::CONST;
    if(s == "reg")  return User_Symbols::reg;
                    return User_Symbols::stmt;
}
Node_type getNodeTypeFromStr(string s){
    if(s == "c")    return Node_type::constante;
    if(s == "r")    return Node_type::registrador;
    if(s[0] == '#') return Node_type::especifico;
                    return Node_type::operacao;
}

class Tree: public YamgTree<Tree>{
    public:

        Tree(){}
        Tree(const string& name, const User_Symbols user_symbol, const Node_type& type)
            : YamgTree{ name, user_symbol, type }
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
%term MUL
%term DIV
%term MEM
%term CONST
%nonterm reg
%nonterm stmt

%%

regist <- reg: CONST { return 1; } = {
    std::cout << "addi $r, $zero, c\n";
    RegAlloc::newInstruction({"addi %o, $zero, c"}, { {$[0], YAMG_WRITEABLE_OPERAND}, {$[1]} });
};

statement <- stmt: reg { return $cost[0]; } = { ; };

addConst <- reg: ADD(reg,CONST) { return $cost[1] + 1; } = {
    std::cout << "addi $ri, $rj, c\n";
};

addReg <- reg: ADD(reg,reg) { return $cost[1] + $cost[2] + 1; } = {
    std::cout << "add $ri, $rj, $rk\n";
};

loadAdd <- reg: MEM(ADD(reg,CONST)) { return $cost[2] + 2; } = {
    std::cout << "lw $ri, c($rj)\n";
};

loadReg <- reg: MEM(reg) { return $cost[1] + 1; } = {
    std::cout << "lw, $ri, 0($rj)\n";
};

mul <- reg: MUL(reg,reg) { return $cost[1] + $cost[2] + 2; } = {
    std::cout << "mul $ri, $rj, $rk\n";
};

%%

%register $s0
%register $s1
%register $s2
%register $s3
%register $s4
%register $s5
%register $s6
%register $s7

%spill_register $t0
%spill_register $t1
%spill_register $t2

%set_read "lw %o, %o($sp)"
%set_write "sw %o, %o($sp)"

%%

{

using namespace Code_Generator;

int main(){
    Tree t{};
    t = t.readTree(t);
    std::cout << t;
    // label(t);
    // reduce(t);
    // RegAlloc::printCode(false);
}

}
