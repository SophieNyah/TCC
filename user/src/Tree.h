//
// Created by sophie on 06/03/23.
//

#ifndef USERPROJECT_TREE_H
#define USERPROJECT_TREE_H

#include "../../src/Tree.hpp"
#include "../output/grammar.hpp"
#include "../src/AstSymbols.h"

class Tree: public YamgTree<Tree, AstSymbols::Funcao> {
    public:
        Tree();
        void readTree(AstSymbols::Funcao) override;

        Node_type readNodeType(std::string) override;

        Yamg::User_Symbols readUserSymbol(std::string) override;

        int dolmes;

    private:
        Tree(const std::string name, const Yamg::User_Symbols, const Node_type);
        struct Private;

};


#endif //USERPROJECT_TREE_H
