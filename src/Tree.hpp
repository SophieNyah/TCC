#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<limits>
#include<optional>
#include"types.hpp"

using namespace std;

template<typename T>
class TemplateTree{

    protected:

        int user_symbol;
        string name;
        vector<T> children;
        code_t action;
        Node_type type;

    public:

            /* Construtores */
        TemplateTree(){}
        TemplateTree(const string& name, const int user_symbol, const Node_type& type)
            : name{ name }, user_symbol{ user_symbol }, type{ type }{}
        TemplateTree(const string& name, const User_Symbols user_symbol, const Node_type& type)
            : name{ name }, user_symbol{ (int)user_symbol }, type{ type }{}
        TemplateTree(const string& name, const int user_symbol, const Node_type& type, const code_t& action)
            : name{ name }, user_symbol{ user_symbol }, type{ type }, action{ action }{}


            /* Getters/Setters */
        string getName(){ return this->name; }
        Node_type getType(){ return this->type; }
        int getSymbol(){ return this->user_symbol; }
        code_t getAction(){ return this->action; }

            /* Atributos */
        int children_size{0};

            /* Métodos */
        void insertChild(const T& c){
            this->children.push_back(c);
            this->children_size++;
        }
        
        optional<T> getChild(int index){
            try{
                return this->children.at(index);
            }
             catch (const out_of_range& e) {
                return std::nullopt;
            }
        }

        T& getChildReference(int index){
            return this->children.at(index);
        }
        
        vector<T>& getChildren(){ return this->children; }

};


class BasicTree: public TemplateTree<BasicTree>{
    public:
        BasicTree();
        BasicTree(const string& name, const int non_term, const Node_type& type);
        BasicTree(const string& name, const User_Symbols non_term, const Node_type& type);
        BasicTree(const string& name, const int non_term, const Node_type& type, const code_t& action);  

        Cost_expression cost;  
};


template<typename U>
class YamgTree: public TemplateTree<U>{

    private:

        int& _getChildCost(int& child){
            if(child == 0) return this->matched_rules.at(0).second;

            for( U& c: this->getChildren() ){
                child--;
                int& value = c._getChildCost(child);
                if(child==0) return value;
            }
        }

    protected:

        YamgTree(){}
        YamgTree(const string& name, const User_Symbols non_term, const Node_type& type)
            : TemplateTree<U>{ name, non_term, type }
            {}
            
    public:

        virtual U readTree(U& tree) = 0;

        virtual ~YamgTree() = default;

        MyArray<int> non_terminal{};
        MyArray<pair<Rules, cost_t>> matched_rules{ pair<Rules, cost_t>{static_cast<Rules>(-1), numeric_limits<cost_t>::max()} };
        cost_t cost{ 0 };
        Cost_expression cost_expression{};

        int& getChildCost(int child){
            return this->_getChildCost(child);
        }
};

#endif