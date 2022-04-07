#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<optional>
#include"types.hpp"

using namespace std;

enum class Node_type{ operacao, registrador, constante, especifico };
enum class Non_terminals: int;

class BasicTree{

    private:

        Node_type type;
        string name;
        int non_term;
        vector<BasicTree> children;
        code_t action;  /* Ação a ser executada quando o nó for visitado */

    public:

            /* Construtores */
        BasicTree();
        BasicTree(const string& op, const int non_term, const Node_type& type);
        BasicTree(const string& op, const Non_terminals non_term, const Node_type& type);
        BasicTree(const string& op, const int non_term, const Node_type& type, const code_t& action);


            /* Getters/Setters */
        string getName();
        Node_type getType();
        int getNonTerm();

            /* Métodos */
        void insertChild(const BasicTree&);
        
        optional<BasicTree> getChild(int index);
        vector<BasicTree>& getChildren();

        bool matchTree(BasicTree&);

            /* Overloads de operadores */
        friend ostream& operator<<(ostream&, BasicTree&);

};

class Tree;

struct Cost_expression{
    vector<int> cost_directives;
    int integer_part;
    Cost_expression(){}
    Cost_expression(vector<int> c, int i=0):cost_directives{ c }, integer_part{ i }{}
};
class BaseTree: public BasicTree{

    protected:

        BaseTree(){}
        BaseTree(const string& name, const Non_terminals non_term, const Node_type& type)
            : BasicTree{ name, non_term, type }
            {}
            
    public:

        virtual Tree* readTree() = 0;

        Cost_expression cost;

        virtual ~BaseTree() = default;

};

#endif