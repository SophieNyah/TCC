#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<optional>
#include"types.hpp"

using namespace std;

enum class Node_type{ operacao, registrador, constante, especifico };
enum class Non_terminals: int;

template<typename T>
class TemplateTree{

    protected:

        vector<T> children;
        Node_type type;
        string name;
        int non_term;
        code_t action;

    public:

            /* Construtores */
        TemplateTree(){}
        TemplateTree(const string& name, const int non_term, const Node_type& type)
            : name{ name }, non_term{ non_term }, type{ type }{}
        TemplateTree(const string& name, const Non_terminals non_term, const Node_type& type)
            : name{ name }, non_term{ (int)non_term }, type{ type }{}
        TemplateTree(const string& name, const int non_term, const Node_type& type, const code_t& action)
            : name{ name }, non_term{ non_term }, type{ type }, action{ action }{}


            /* Getters/Setters */
        string getName(){ return this->name; }
        Node_type getType(){ return this->type; }
        int getNonTerm(){ return this->non_term; }

            /* Métodos */
        void insertChild(const T& c){ this->children.push_back(c); }
        
        optional<T> getChild(int index){
            try{
                return this->children.at(index);
            }
             catch (const out_of_range& e) {
                return std::nullopt;
            }
        }
        
        vector<T>& getChildren(){ return this->children; }

};


struct Cost_expression{
    vector<int> cost_directives;
    int integer_part;
    Cost_expression(){}
    Cost_expression(vector<int> c, int i=0):cost_directives{ c }, integer_part{ i }{}
};


class BasicTree: public TemplateTree<BasicTree>{
    public:
        BasicTree();
        BasicTree(const string& name, const int non_term, const Node_type& type);
        BasicTree(const string& name, const Non_terminals non_term, const Node_type& type);
        BasicTree(const string& name, const int non_term, const Node_type& type, const code_t& action);  

        Cost_expression cost;  
};


template<typename U>
class VirtualTree: public TemplateTree<U>{

    protected:

        VirtualTree(){}
        VirtualTree(const string& name, const Non_terminals non_term, const Node_type& type)
            : TemplateTree<U>{ name, non_term, type }
            {}
            
    public:

        virtual U readTree() = 0;

        virtual ~VirtualTree() = default;

};

#endif