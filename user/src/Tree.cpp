//
// Created by sophie on 06/03/23.
//

#include <iostream>
#include "Tree.h"
#include "../output/grammar.hpp"

struct Tree::Private{

    static Tree commandIf(msa::if_else_t *command){
        Tree ifTree{"IF", Yamg::User_Symbols::IF, Node_type::operacao};
        ifTree.insertChild(commandExpression(command->exp));
        ifTree.insertChild(insertCommands(command->then_com));
        if(command->else_com != NULL) ifTree.insertChild(insertCommands(command->else_com));
        return ifTree;
    }
    static Tree commandWhile(msa::while_t *command){
        Tree whileTree{"WHILE", Yamg::User_Symbols::WHILE, Node_type::operacao};
        whileTree.insertChild(commandExpression(command->exp));
        whileTree.insertChild(insertCommands(command->commands));
        return whileTree;
    }
    static Tree commandFor(msa::for_t *command){
        Tree forTree{"FOR", Yamg::User_Symbols::IF, Node_type::operacao};
        forTree.insertChild(commandExpression(command->exp_init));
        forTree.insertChild(commandExpression(command->exp_update));
        forTree.insertChild(commandExpression(command->exp_check));
        forTree.insertChild(insertCommands(command->commands));
        return forTree;
    }
    static Tree commandReturn(msa::expression *command){
        Tree returnTree{"RETURN", Yamg::User_Symbols::IF, Node_type::operacao};
        return returnTree;
    }
    static Tree commandExpression(msa::expression *command){
        Tree expTree{"EXPRESSION", Yamg::User_Symbols::IF, Node_type::operacao};
        return expTree;
    }

    static Tree insertCommands(msa::command_list *commands) {
        Tree t{};
        Tree *currNode = &t;
        for(msa::command_list *aux = commands; aux; aux = aux->next) {
            currNode->name = "COMMAND";
            currNode->user_symbol = (int)Yamg::User_Symbols::COMMAND;
            currNode->type = Node_type::registrador;

            switch (aux->com_type) {
                case msa::COM_IF:     currNode->insertChild(commandIf(aux->com.if_com));          break;
                case msa::COM_FOR:    currNode->insertChild(commandFor(aux->com.for_com));        break;
                case msa::COM_EXP:    currNode->insertChild(commandExpression(aux->com.exp_com)); break;
                case msa::COM_WHILE:  currNode->insertChild(commandWhile(aux->com.while_com));    break;
                case msa::COM_BLOCK: *currNode = insertCommands(aux->com.block);                  break;
                case msa::COM_RETURN: currNode->insertChild(commandReturn(aux->com.return_com));  break;
                default: break;
            }

            if(aux->next) {
                Tree child{};
                currNode->insertChild(child);
                currNode = &currNode->getChildReference(currNode->children_size - 1);
            }
        }
        return t;
    }

    static std::string toString(const Tree &t) {
        std::vector<Tree> children{ t.children };
        std::string str{ t.name + '(' };
        for(size_t i=0; i<children.size(); i++) {
            str.append(toString(children[i]));
            if(i+1 != children.size()) str.append(", ");
        }
        str.append(")");
        return str;
    }

};


void Tree::readTree(AstSymbols::Funcao funcao) {
    msa::command_list comandos = funcao.getComandos();

    Tree root{ Tree::Private::insertCommands(&comandos) };
    std::cout << "\n" << Tree::Private::toString(root);

    *this = root;
}

Node_type Tree::readNodeType(std::string type) {
    return Node_type::operacao;
}

Yamg::User_Symbols Tree::readUserSymbol(std::string type) {
    Yamg::User_Symbols result;
    return result;
}

Tree::Tree() {}
Tree::Tree(const std::string name, const Yamg::User_Symbols symbol, const Node_type type)
    : YamgTree<Tree, AstSymbols::Funcao> { name, symbol, type }
    {}
