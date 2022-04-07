#include<stdexcept>
#include<iostream>
#include"Tree.hpp"

    /* Construtores */
BasicTree::BasicTree(){};
BasicTree::BasicTree(const string& name, const int non_term, const Node_type& type)
    : name{ name }, non_term{ non_term }, type{ type }
    {}
BasicTree::BasicTree(const string& name, const Non_terminals non_term, const Node_type& type)
    : name{ name }, non_term{ (int)non_term }, type{ type }
    {}
BasicTree::BasicTree(const string& name, const int non_term, const Node_type& type, const code_t& action)
    : name{ name }, non_term{ non_term }, type{ type }, action{ action }
    {}


    /* Getters/Setters */
string BasicTree::getName(){ return this->name; }
Node_type BasicTree::getType(){ return this->type; }
int BasicTree::getNonTerm(){ return this->non_term; }

    /* Métodos */
void BasicTree::insertChild(const BasicTree& t){
    this->children.push_back(t);
}

optional<BasicTree> BasicTree::getChild(int index){
    try{
        return this->children.at(index);
    } catch (const out_of_range& e) {
        return std::nullopt;
    }
}

vector<BasicTree>& BasicTree::getChildren(){
    return this->children;
}

bool BasicTree::matchTree(BasicTree& root){
    if( this->type == root.type ){
        if( (this->type == Node_type::operacao || this->type == Node_type::especifico) 
        && this->name != root.name ){
            return false;
        }

        for( int i=0; i<this->getChildren().size(); i++ ){
            BasicTree c1 = this->getChild(i).value();
            optional<BasicTree> c2 = root.getChild(i);

            if( !c2.has_value() ){ return false; }

            if( !c1.matchTree(c2.value()) ){ return false; }
        }

    } else {
        return false;
    }

    return true;
}


    /* Overloads de operadores */
ostream& operator<<(ostream& out, BasicTree& tree){
    out << tree.name << " (" << (int)tree.type << ")";
    int children = tree.getChildren().size();
    
    if( children > 0 ){ out << "(" << tree.action << " "; }

    for( int i=0; i<children; i++){
        BasicTree t = tree.getChild(i).value();
        out << t;
        if(i < children - 1){
            out << ", ";
        }
    }

    if( children > 0 ){ out << ")"; }
    
    return out;
}
