//
// Created by sophie on 06/03/23.
//

#include <iostream>
#include "Tree.h"
#include "AstSymbols.h"
#include "../output/mips.hpp"

namespace {
    std::string newStringLiteralName() {
        static int iteration = 0;
        std::string name{"__literal_" + std::to_string(iteration)};
        iteration++;
        return name;
    }
}

std::string newTempRegister(bool reset) {
    static int currReg{ 0 };
    if(reset) {
        currReg = 3;
    } else {
        currReg++;
        currReg = (currReg%5) + 3;
    }
    std::string reg{ "$t" + std::to_string(currReg) };
    return reg;
}

struct Tree::Private{

    static Tree createExpressionNode(msa::expression *exp) {
        switch(exp->node_type) {
            case msa::yytokentype::ADD:           return Tree{newTempRegister(), Yamg::User_Symbols::ADD, Node_type::operacao};
            case msa::yytokentype::SUB:           return Tree{newTempRegister(), Yamg::User_Symbols::SUB, Node_type::operacao};
            case msa::yytokentype::MUL:           return Tree{newTempRegister(), Yamg::User_Symbols::MUL, Node_type::operacao};

            case msa::yytokentype::EQUALS:        return Tree{newTempRegister(), Yamg::User_Symbols::EQUALS, Node_type::operacao};
            case msa::yytokentype::ASSIGN:        return Tree{"ASSIGN", Yamg::User_Symbols::ASSIGN, Node_type::operacao};

            case msa::yytokentype::FUNCTION_CALL: return Tree{exp->node_value.sym->id, Yamg::User_Symbols::FUNCTION_CALL, Node_type::operacao};
            case msa::yytokentype::IDENTIFIER:    return Tree{exp->node_value.sym->id, Yamg::User_Symbols::VARIABLE, Node_type::registrador};
            case msa::yytokentype::NUM_INT:       return Tree{std::to_string(exp->node_value.num), Yamg::User_Symbols::CONST, Node_type::constante};
            case msa::yytokentype::STRING:
            case msa::yytokentype::CHARACTER:
                AstSymbols::Programa::insertStringLiteral(newStringLiteralName(), exp->node_value.str);
                return Tree{exp->node_value.str, Yamg::User_Symbols::STRING, Node_type::constante};
        }
        return Tree{};
    }

    static Tree commandIf(msa::if_else_t *command){
        Tree ifTree{"IF", Yamg::User_Symbols::IF, Node_type::operacao};
        ifTree.insertChild(commandExpression(command->exp));
        ifTree.insertChild(insertCommands(command->then_com));
        if(command->else_com != nullptr) ifTree.insertChild(insertCommands(command->else_com));
        return ifTree;
    }
    static Tree commandWhile(msa::while_t *command){
        Tree whileTree{"WHILE", Yamg::User_Symbols::WHILE, Node_type::operacao};
        whileTree.insertChild(commandExpression(command->exp));
        whileTree.insertChild(insertCommands(command->commands));
        return whileTree;
    }
    static Tree commandFor(msa::for_t *command){
        Tree forTree{"FOR", Yamg::User_Symbols::FOR, Node_type::operacao};
        forTree.insertChild(commandExpression(command->exp_init));
        forTree.insertChild(commandExpression(command->exp_update));
        forTree.insertChild(commandExpression(command->exp_check));
        forTree.insertChild(insertCommands(command->commands));
        return forTree;
    }
    static Tree commandReturn(msa::expression *command){
        Tree returnTree{"RETURN", Yamg::User_Symbols::RETURN, Node_type::operacao};
        returnTree.insertChild(commandExpression(command));
        return returnTree;
    }
    static Tree _commandExpression(msa::expression *expression){
        Tree expTree{ createExpressionNode(expression) };
        if(expression->left)  expTree.insertChild(commandExpression(expression->left));
        if(expression->right) expTree.insertChild(commandExpression(expression->right));
        return expTree;
    }
    static Tree commandExpression(msa::expression *expression) {
        newTempRegister(true);
        return _commandExpression(expression);
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
        std::string str{ t.name };
        if(t.children.empty()) return str;
        str.append("(");
        for(size_t i=0; i<t.children.size(); i++) {
            str.append(toString(t.children[i]));
            if(i+1 != t.children.size()) str.append(", ");
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

void Tree::setName(const std::string& name){
    this->name = name;
}

Tree::Tree() {}

Tree::Tree(const std::string &name, const Yamg::User_Symbols symbol, const Node_type type)
    : YamgTree<Tree, AstSymbols::Funcao> { name, symbol, type }
    {}
