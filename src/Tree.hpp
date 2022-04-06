#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<optional>
#include"types.hpp"

using namespace std;

enum class Node_type{ operacao, registrador, constante, especifico };

class Tree{

    private:

        Node_type type;
        string name;
        int non_term;
        static int max_children;
        vector<Tree> children;
        code_t action;  /* Ação a ser executada quando o nó for visitado */

    public:

            /* Construtores */
        Tree();
        Tree(const string& op, const int non_term, const Node_type& type);
        Tree(const string& op, const int non_term, const Node_type& type, const code_t& action);


            /* Getters/Setters */
        string getName();
        Node_type getType();
        int getNonTerm();

            /* Métodos */
        void insertChild(Tree&);
        
        optional<Tree> getChild(int index);
        vector<Tree>& getChildren();

        bool matchTree(Tree&);

            /* Overloads de operadores */
        friend ostream& operator<<(ostream&, Tree&);

};

class BaseUserTree: public Tree{

    protected:

        BaseUserTree(const string& name, const int non_term, const Node_type& type)
            : Tree{ name, non_term, type }
            {}
            
    public:

        virtual Tree readTree() = 0;

        virtual ~BaseUserTree() = default;

};

#endif