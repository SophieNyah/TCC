#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<optional>
#include"types.hpp"

using namespace std;

class Tree{

    private:

        string op;  /* terminal ou não-terminal */
        static int max_children;
        vector<Tree> children;
        code_t action;  /* Ação a ser executada quando o nó for visitado */

    public:

            /* Construtores */
        Tree();
        Tree(const string& op);
        Tree(const string& op, const code_t& action);


            /* Métodos */
        void insertChild(Tree);
        
        optional<Tree> getChild(int index);
        vector<Tree>& getChildren();

        friend ostream& operator<<(ostream&, Tree&);

};

#endif