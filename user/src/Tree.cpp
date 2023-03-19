//
// Created by sophie on 06/03/23.
//

#include <iostream>
#include "Tree.h"
#include "AstSymbols.h"
#include "../output/mips.hpp"

using variablesMap = std::map<std::string, std::string>;

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

int labelIndex(bool newIndex) {
    static int currLabel{ 0 };
    if (newIndex) { currLabel++; }
    return currLabel;
}

struct Tree::Private{

    static Tree createExpressionNode(msa::expression *exp, const variablesMap &parameterVariables) {
        switch(exp->node_type) {
            case msa::yytokentype::ADD:           return Tree{newTempRegister(), Yamg::User_Symbols::ADD, Node_type::operacao};
            case msa::yytokentype::SUB:           return Tree{newTempRegister(), Yamg::User_Symbols::SUB, Node_type::operacao};
            case msa::yytokentype::MUL:           return Tree{newTempRegister(), Yamg::User_Symbols::MUL, Node_type::operacao};

            case msa::yytokentype::EQUALS:        return Tree{newTempRegister(), Yamg::User_Symbols::EQUALS, Node_type::operacao};
            case msa::yytokentype::ASSIGN:        return Tree{"ASSIGN", Yamg::User_Symbols::ASSIGN, Node_type::operacao};

            case msa::yytokentype::FUNCTION_CALL: return Tree{exp->node_value.sym->id, Yamg::User_Symbols::FUNCTION_CALL, Node_type::operacao};
            case msa::yytokentype::IDENTIFIER: {
                try {
                    std::string parameter = parameterVariables.at(exp->node_value.sym->id);
                    return Tree{parameter, Yamg::User_Symbols::VARIABLE, Node_type::registrador};
                } catch(std::out_of_range) {
                    return Tree{exp->node_value.sym->id, Yamg::User_Symbols::VARIABLE, Node_type::registrador};
                }
            }
            case msa::yytokentype::NUM_INT:       return Tree{std::to_string(exp->node_value.num), Yamg::User_Symbols::CONST, Node_type::constante};
            case msa::yytokentype::STRING:
            case msa::yytokentype::CHARACTER:
                AstSymbols::Programa::insertStringLiteral(newStringLiteralName(), exp->node_value.str);
                return Tree{exp->node_value.str, Yamg::User_Symbols::STRING, Node_type::constante};
        }
        return Tree{};
    }

    static Tree commandIf(msa::if_else_t *command, const variablesMap &parameterVariables){
        Tree ifTree{"IF", Yamg::User_Symbols::IF, Node_type::operacao};
        ifTree.insertChild(commandExpression(command->exp, parameterVariables));
        ifTree.insertChild(insertCommands(command->then_com, parameterVariables));
        if(command->else_com != nullptr) ifTree.insertChild(insertCommands(command->else_com, parameterVariables));
        return ifTree;
    }
    static Tree commandWhile(msa::while_t *command, const variablesMap &parameterVariables){
        Tree whileTree{"WHILE", Yamg::User_Symbols::WHILE, Node_type::operacao};
        whileTree.insertChild(commandExpression(command->exp, parameterVariables));
        whileTree.insertChild(insertCommands(command->commands, parameterVariables));
        return whileTree;
    }
    static Tree commandFor(msa::for_t *command, const variablesMap &parameterVariables){
        Tree forTree{"FOR", Yamg::User_Symbols::FOR, Node_type::operacao};
        forTree.insertChild(commandExpression(command->exp_init, parameterVariables));
        forTree.insertChild(commandExpression(command->exp_update, parameterVariables));
        forTree.insertChild(commandExpression(command->exp_check, parameterVariables));
        forTree.insertChild(insertCommands(command->commands, parameterVariables));
        return forTree;
    }
    static Tree commandReturn(msa::expression *command, const variablesMap &parameterVariables){
        Tree returnTree{"RETURN", Yamg::User_Symbols::RETURN, Node_type::operacao};
        returnTree.insertChild(commandExpression(command, parameterVariables));
        return returnTree;
    }
    static Tree _commandExpression(msa::expression *expression, const variablesMap &parameterVariables){
        Tree expTree{ createExpressionNode(expression, parameterVariables) };
        if(expression->left)  expTree.insertChild(commandExpression(expression->left, parameterVariables));
        if(expression->right) expTree.insertChild(commandExpression(expression->right, parameterVariables));
        return expTree;
    }
    static Tree commandExpression(msa::expression *expression, const variablesMap &parameterVariables) {
        newTempRegister(true);
        return _commandExpression(expression, parameterVariables);
    }

    static Tree insertCommands(msa::command_list *commands, const variablesMap &parameterVariables) {
        Tree t{};
        Tree *currNode = &t;
        for(msa::command_list *aux = commands; aux; aux = aux->next) {
            currNode->name = "COMMAND";
            currNode->user_symbol = (int)Yamg::User_Symbols::COMMAND;
            currNode->type = Node_type::registrador;

            switch (aux->com_type) {
                case msa::COM_IF:     currNode->insertChild(commandIf(aux->com.if_com, parameterVariables));          break;
                case msa::COM_FOR:    currNode->insertChild(commandFor(aux->com.for_com, parameterVariables));        break;
                case msa::COM_EXP:    currNode->insertChild(commandExpression(aux->com.exp_com, parameterVariables)); break;
                case msa::COM_WHILE:  currNode->insertChild(commandWhile(aux->com.while_com, parameterVariables));    break;
                case msa::COM_BLOCK: *currNode = insertCommands(aux->com.block, parameterVariables);                  break;
                case msa::COM_RETURN: currNode->insertChild(commandReturn(aux->com.return_com, parameterVariables));  break;
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
    auto *parametros = funcao.getPrototipo().parameters;
    variablesMap parameterVariables{};
    int currArgument{0};
    while(parametros) {
        parameterVariables.insert( std::make_pair(parametros->symbol->id, "$a" + std::to_string(currArgument)) );
        currArgument++;
        parametros = parametros->next;
    }

    Tree root{ Tree::Private::insertCommands(&comandos, parameterVariables) };
//    std::cout << "\n" << Tree::Private::toString(root);

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
