{
#include<iostream>

class Tree: public YamgTree<Tree>{
    public:

        Tree(){}
        Tree(const string& name, const User_Symbols user_symbol, const Node_type& type)
            : YamgTree{ name, user_symbol, type }
            {}

        User_Symbols readUserSymbol() override {
            string s = this->name;
            if(s == "ADD")  return User_Symbols::ADD;
            if(s == "SUB")  return User_Symbols::SUB;
            if(s == "MUL")  return User_Symbols::MUL;
            if(s == "MEM")  return User_Symbols::MEM;
            if(s == "CONST")return User_Symbols::CONST;
            if(s == "MOVE") return User_Symbols::MOVE;
            if(s == "FP")   return User_Symbols::FP;
            if(s == "VAR")  return User_Symbols::VAR;
            if(s == "reg")  return User_Symbols::reg;
                            return User_Symbols::stmt;
        }
        Node_type readNodeType() override {
            string s = this->name;
            if(s == "c") return Node_type::constante;
                         return Node_type::constante;
        }

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
                            t.user_symbol = (int)t.readUserSymbol();
                            t.type = t.readNodeType();
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

        void readTree() override {
            readTreeRecursion(*this).first;
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
%term MEM
%term CONST
%term MOVE
%term FP
%term VAR
%nonterm reg
%nonterm stmt

%%

addConstRight <- reg: ADD(reg, CONST) { return $cost[1] + 1; } = {
    std::cout << "addi $ri, $rj, c\n";
};

addConstLeft <- reg: ADD(CONST, reg) { return $cost[2] + 1; } = {
    std::cout << "addi $ri, $rj, c\n";
};

addReg <- reg: ADD(reg, reg) { return $cost[1] + $cost[2] + 1; } = {
    std::cout << "add $ri, $rj, $rk\n";
};



mul <- reg: MUL( reg, reg ) { return $cost[1] + $cost[2] + 1; } = {
    std::cout << "mul $ri, $rj, $rk\n";
};



sub <- reg: SUB( reg, reg ) { return $cost[1] + $cost[2] + 1; } = {
    std::cout << "sub $ri, $rj, $rk\n";
};

subConst <- reg: SUB( reg, CONST ) { return $cost[1] + 1; } = {
    std::cout << "sub $ri, $rj, c\n";
};



loadAddConstRight <- reg: MEM( ADD( reg, CONST ) ) { return $cost[2] + 1; } = {
    std::cout << "lw $ri, c($rj)\n";
};

loadAddConstLeft <- reg: MEM( ADD( CONST, reg ) ) { return $cost[3] + 1; } = { 
    std::cout << "lw $ri, c($rj)\n";
};

loadConst <- reg: MEM( CONST ) { return 1; } = {
    std::cout << "lw $ri, c(0)\n";
};

loadReg <- reg: MEM( reg ) { return $cost[1] + 1; } = {
    std::cout << "lw, $ri, 0($rj)\n";
};



storeMemAddConstRight <- stmt: MOVE( MEM( ADD(reg,CONST) ), reg ) { return $cost[3] + $cost[5] + 2; } = {
    std::cout << "sw, $ri, c($rj)\n";
};

storeMemAddConstLeft <- stmt: MOVE( MEM( ADD(CONST,reg) ), reg ) { return $cost[4] + $cost[5] + 2; } = {
    std::cout << "sw, $ri, c($rj)\n";
};

storeMemConst <- stmt: MOVE( MEM( CONST ), reg ) { return $cost[3] + 2; } = {
    std::cout << "sw, $ri, c(0)\n";
};

storeMemReg <- stmt: MOVE( MEM( reg ), reg ) { return $cost[3] + $cost[2] + 2; } = {
    std::cout << "sw, $ri, $rj\n";
};

moveMemMem <- stmt: MOVE( MEM( reg ), MEM( reg ) ) { return $cost[2] + $cost[4] + 3; } = {
    std::cout << "move, $ri, $rj\n";
};


statement <- stmt: reg { return $cost[0]; } = {
    std::cout << "Statement\n";
};

framePointer <- reg: FP { return 0; } = { ; };

variable <- reg: VAR { return 0; } = { ; };

constant <- reg: CONST { return 1; } = {
    std::cout << "addi $ri, $zero, c\n";
};

%%

{

using namespace Code_Generator;

int main(){
    Tree t{};
    t.readTree();
    // label(t);
    // reduce(t);
}

}
