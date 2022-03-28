#include<stdexcept>
#include<iostream>
#include"Tree.hpp"

int Tree::max_children{2};

    /* Construtores */
Tree::Tree(){};
Tree::Tree(const string& op): op{ op }{}
Tree::Tree(const string& op, const code_t& action): op{ op }, action{ action }{}


    /* Métodos */
void Tree::insertChild(Tree t){
    this->children.push_back(t);
}

optional<Tree> Tree::getChild(int index){
    try{
        return this->children.at(index);
    } catch (const out_of_range& e) {
        return std::nullopt;
    }
}

vector<Tree>& Tree::getChildren(){
    return this->children;
}

ostream& operator<<(ostream& out, Tree& tree){
    out << tree.op;
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
