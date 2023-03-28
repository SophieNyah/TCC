//
// Created by sophie on 06/03/23.
//

#ifndef USERPROJECT_TREE_H
#define USERPROJECT_TREE_H

#include "../../src/Tree.hpp"
#include "../output/mips.hpp"
#include "../src/AstSymbols.h"

std::string newTempRegister(bool reset=false);
int labelIndex(bool newIndex = false);

class Tree: public YamgTree<Tree, AstSymbols::Funcao> {
    public:
        Tree();
        void readTree(AstSymbols::Funcao) override;
        Node_type readNodeType(std::string) override;
        Yamg::User_Symbols readUserSymbol(std::string) override;

    private:
        Tree(const std::string &name, const Yamg::User_Symbols, const Node_type);
        struct Private;
};

class Logger {
    public:
        Logger() = delete;
        static void setLoggerStream(std::ofstream&);
        static void Log(const std::string& message, const std::string& expression="");

    private:
        static std::ofstream logger_file;
};


#endif //USERPROJECT_TREE_H
