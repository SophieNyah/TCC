#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<optional>
#include"types.hpp"

using namespace std;

enum Node_type{ registrador, constante, operacao, especifico };

class Tree{

    private:

        Node_type type;
        string name;
        static int max_children;
        vector<Tree> children;
        code_t action;  /* Ação a ser executada quando o nó for visitado */

    public:

            /* Construtores */
        Tree();
        Tree(const string& op, const Node_type& type);
        Tree(const string& op, const Node_type& type, const code_t& action);


            /* Métodos */
        void insertChild(Tree&);
        
        optional<Tree> getChild(int index);
        vector<Tree>& getChildren();

        bool matchTree(Tree&);

        friend ostream& operator<<(ostream&, Tree&);

};

#endif