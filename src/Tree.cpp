#include<stdexcept>
#include<iostream>
#include"Tree.hpp"

int Tree::max_children{2};

    /* Construtores */
Tree::Tree(){};
Tree::Tree(const string& name, const Node_type& type)
    : name{ name }, type{ type }
    {}
Tree::Tree(const string& name, const Node_type& type, const code_t& action)
    : name{ name }, type{ type }, action{ action }
    {}


    /* Getters/Setters */
string Tree::getName(){ return this->name; }

    /* Métodos */
void Tree::insertChild(Tree& t){
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

bool Tree::matchTree(Tree& root){
    if( this->type == root.type ){
        if( (this->type == operacao || this->type == especifico) 
        && this->name != root.name ){
            return false;
        }

        for( int i=0; i<this->getChildren().size(); i++ ){
            Tree c1 = this->getChild(i).value();
            optional<Tree> c2 = root.getChild(i);

            if( !c2.has_value() ){ return false; }

            if( !c1.matchTree(c2.value()) ){ return false; }
        }

    } else {
        return false;
    }

    return true;
}


    /* Overloads de operadores */
ostream& operator<<(ostream& out, Tree& tree){
    out << tree.name << " (" << tree.type << ")";
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
