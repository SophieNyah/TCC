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
%term SUB
%term MUL
%term DIV
%term MEM
%term CONST
%nonterm reg
%nonterm stmt

%%

regist { std::cout << "addi $r, $zero, c\n"; } <-
    reg: CONST,
    { return 1; };

statement { ; } <-
    stmt: reg,
    { return $cost[0]; };

addConst { std::cout << "addi $ri, $rj, c\n"; } <-
    reg: ADD(reg,CONST),
    { return $cost[1] + 1; };

addReg { std::cout << "add $ri, $rj, $rk\n"; } <-
    reg: ADD(reg,reg) ,
    { return $cost[1] + $cost[2] + 1; };

loadAdd { std::cout << "lw $ri, c($rj)\n"; } <-
    reg: MEM(ADD(reg,CONST)),
    { return $cost[2] + 2; };

loadReg { std::cout << "lw, $ri, 0($rj)\n"; } <-
    reg: MEM(reg),
    { return $cost[1] + 1; };

mul { std::cout << "mul $ri, $rj, $rk\n"; } <-
    reg: MUL(reg,reg),
    { return $cost[1] + $cost[2] + 2; };

%%

{

using namespace Code_Generator;

int main(){
    Tree t{};
    t = t.readTree(t);
    label(t);
    reduce(t);
}

}
