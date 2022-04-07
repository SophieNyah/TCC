#include<stdexcept>
#include<iostream>
#include"Tree.hpp"

    /**********************
    *                     *
    *     TemplateTree    *
    *                     *
    **********************/

    /* Construtores */

// template<typename T>    
// TemplateTree<T>::TemplateTree(){};

// template<typename T>
// TemplateTree<T>::TemplateTree(const string& name, const int non_term, const Node_type& type)
//     : name{ name }, non_term{ non_term }, type{ type }{}

// template<typename T>    
// TemplateTree<T>::TemplateTree(const string& name, const Non_terminals non_term, const Node_type& type)
//     : name{ name }, non_term{ (int)non_term }, type{ type }{}

// template<typename T>    
// TemplateTree<T>::TemplateTree(const string& name, const int non_term, const Node_type& type, const code_t& action)
//     : name{ name }, non_term{ non_term }, type{ type }, action{ action }{}


    /* Getters/Setters */

// template<typename T>    
// string TemplateTree<T>::getName(){ return this->name; }

// template<typename T>
// Node_type TemplateTree<T>::getType(){ return this->type; }

// template<typename T>
// int TemplateTree<T>::getNonTerm(){ return this->non_term; }



    /********************
    *                   *
    *     BasicTree     *
    *                   *
    ********************/

BasicTree::BasicTree()
    : TemplateTree{}{}

BasicTree::BasicTree(const string& name, const int non_term, const Node_type& type)
    : TemplateTree{ name, non_term, type }{}

BasicTree::BasicTree(const string& name, const Non_terminals non_term, const Node_type& type)
    : TemplateTree{ name, non_term, type }{}

BasicTree::BasicTree(const string& name, const int non_term, const Node_type& type, const code_t& action)
    : TemplateTree{ name, non_term, type, action }{}
