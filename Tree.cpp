#include<stdexcept>
#include<iostream>
#include"Tree.hpp"

int Tree::max_children{2};

    /* Construtores */
Tree::Tree(){};
Tree::Tree(const string op): op{ op }{}


    /* Métodos */
bool Tree::insertChild(Tree t){
    if( this->children.size() < Tree::max_children ){
        this->children.push_back(t);
        return true;
    } else {
        return false;
    }
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
