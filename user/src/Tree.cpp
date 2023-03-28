//
// Created by sophie on 06/03/23.
//

#include <iostream>
#include <cstring>
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

            case msa::yytokentype::INC:           return Tree{newTempRegister(), Yamg::User_Symbols::INC, Node_type::operacao};
            case msa::yytokentype::SUBSCRIPT:     return Tree{newTempRegister(), Yamg::User_Symbols::SUBSCRIPT, Node_type::operacao};

            case msa::yytokentype::ASSIGN:        return Tree{"ASSIGN", Yamg::User_Symbols::ASSIGN, Node_type::operacao};
            case msa::yytokentype::EQUALS:        return Tree{newTempRegister(), Yamg::User_Symbols::EQUALS, Node_type::operacao};
            case msa::yytokentype::LESS:          return Tree{newTempRegister(), Yamg::User_Symbols::LESS, Node_type::operacao};
            case msa::yytokentype::LEQ:           return Tree{newTempRegister(), Yamg::User_Symbols::LEQ, Node_type::operacao};

            case msa::yytokentype::FUNCTION_CALL: {
                Tree functionTree{exp->node_value.sym->id, Yamg::User_Symbols::FUNCTION_CALL, Node_type::operacao};
                msa::expression *root = exp->left;
                if(root){
                    for(; root->node_type == msa::yytokentype::COMMA_SEPARATOR; root = root->right ) {
                        switch(root->node_type){
                            case msa::yytokentype::STRING    : functionTree.insertChild(Tree{root->left->node_value.str, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                            case msa::yytokentype::NUM_INT   : functionTree.insertChild(Tree{std::to_string(root->left->node_value.num), Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                            case msa::yytokentype::CHARACTER : functionTree.insertChild(Tree{root->left->node_value.chr, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                            case msa::yytokentype::IDENTIFIER: functionTree.insertChild(Tree{root->left->node_value.sym->id, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                        }
                    }
                    switch(root->node_type){
                        case msa::yytokentype::STRING    : functionTree.insertChild(Tree{root->node_value.str, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                        case msa::yytokentype::NUM_INT   : functionTree.insertChild(Tree{std::to_string(root->node_value.num), Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                        case msa::yytokentype::CHARACTER : functionTree.insertChild(Tree{root->node_value.chr, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                        case msa::yytokentype::IDENTIFIER: functionTree.insertChild(Tree{root->node_value.sym->id, Yamg::User_Symbols::VARIABLE, Node_type::registrador}); break;
                    }
                }
                exp->left = nullptr;
                return functionTree;
            }
            case msa::yytokentype::IDENTIFIER:    {
                try {
                    std::string parameter = parameterVariables.at(exp->node_value.sym->id);
                    return Tree{parameter, Yamg::User_Symbols::VARIABLE, Node_type::registrador};
                } catch(std::out_of_range const&) {
                    return Tree{exp->node_value.sym->id, Yamg::User_Symbols::VARIABLE, Node_type::registrador};
                }
            }
            case msa::yytokentype::NUM_INT:       return Tree{std::to_string(exp->node_value.num), Yamg::User_Symbols::CONST, Node_type::constante};
            case msa::yytokentype::STRING:
            case msa::yytokentype::CHARACTER:     {
                AstSymbols::Programa::insertStringLiteral(newStringLiteralName(), exp->node_value.str);
                return Tree{exp->node_value.str, Yamg::User_Symbols::STRING, Node_type::constante};
            }
        }
        return Tree{};
    }

    static Tree commandIf(msa::if_else_t *if_t, const variablesMap &parameterVariables){
        Tree ifTree{"IF", Yamg::User_Symbols::IF, Node_type::operacao};
        ifTree.insertChild(commandExpression(if_t->exp, parameterVariables));
        ifTree.insertChild(insertCommands(if_t->then_com, parameterVariables));
        if(if_t->else_com != nullptr) ifTree.insertChild(insertCommands(if_t->else_com, parameterVariables));
        return ifTree;
    }
    static Tree commandWhile(msa::while_t *while_t, const variablesMap &parameterVariables){
        Tree whileTree{"WHILE", Yamg::User_Symbols::WHILE, Node_type::operacao};
        whileTree.insertChild(commandExpression(while_t->exp, parameterVariables));
        whileTree.insertChild(insertCommands(while_t->commands, parameterVariables));
        return whileTree;
    }
    static Tree commandFor(msa::for_t *for_t, const variablesMap &parameterVariables){
        Tree forTree{"FOR", Yamg::User_Symbols::FOR, Node_type::operacao};
        forTree.insertChild(commandExpression(for_t->exp_init, parameterVariables));
        forTree.insertChild(commandExpression(for_t->exp_check, parameterVariables));
        forTree.insertChild(commandExpression(for_t->exp_update, parameterVariables));
        forTree.insertChild(insertCommands(for_t->commands, parameterVariables));
        return forTree;
    }

    static Tree commandReturn(msa::expression *expression, const variablesMap &parameterVariables){
        Tree returnTree{"RETURN", Yamg::User_Symbols::RETURN, Node_type::operacao};
        returnTree.insertChild(commandExpression(expression, parameterVariables));
        return returnTree;
    }
    static Tree commandPrintf(msa::printf_t *printf_t, const variablesMap& parameterVariables) {
        Tree printTree{"PRINTF", Yamg::User_Symbols::PRINTF, Node_type::operacao};
        printTree.insertChild({printf_t->string, Yamg::User_Symbols::STRING, Node_type::constante});
        if(printf_t->exp) printTree.insertChild(commandExpression(printf_t->exp, parameterVariables));
        if(strcmp(printf_t->string, "\"%d\"") != 0) AstSymbols::Programa::insertStringLiteral(newStringLiteralName(), printf_t->string);
        return printTree;
    }
    static Tree commandScanf(msa::scanf_t *scanf_t, const variablesMap& parameterVariables) {
        Tree scanTree{"SCANF", Yamg::User_Symbols::SCANF, Node_type::operacao};
//        scanTree.insertChild(commandExpression(scanf_t, parameterVariables));
        scanTree.insertChild({scanf_t->var, Yamg::User_Symbols::VARIABLE, Node_type::registrador});
//        AstSymbols::Programa::insertStringLiteral(newStringLiteralName(), scanf_t->string);
        return scanTree;
    }
    static Tree commandExit(msa::expression *exit_t, const variablesMap& parameterVariables) {
        Tree exitTree{"EXIT", Yamg::User_Symbols::EXIT, Node_type::operacao};
        exitTree.insertChild(commandExpression(exit_t, parameterVariables));
        return exitTree;
    }

    static Tree _commandExpression(msa::expression *expression, const variablesMap &parameterVariables){
        Tree expTree{ createExpressionNode(expression, parameterVariables) };
        if(expression->left)  expTree.insertChild(commandExpression(expression->left, parameterVariables));
        if(expression->right) expTree.insertChild(commandExpression(expression->right, parameterVariables));
        return expTree;
    }
    static Tree commandExpression(msa::expression *expression, const variablesMap &parameterVariables) {
        newTempRegister();
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
                case msa::COM_EXIT:   currNode->insertChild(commandExit(aux->com.exit_com, parameterVariables));      break;
                case msa::COM_SCANF:  currNode->insertChild(commandScanf(aux->com.scanf_com, parameterVariables));    break;
                case msa::COM_WHILE:  currNode->insertChild(commandWhile(aux->com.while_com, parameterVariables));    break;
                case msa::COM_BLOCK: *currNode = insertCommands(aux->com.block, parameterVariables);                  break;
                case msa::COM_RETURN: currNode->insertChild(commandReturn(aux->com.return_com, parameterVariables));  break;
                case msa::COM_PRINTF: currNode->insertChild(commandPrintf(aux->com.printf_com, parameterVariables));  break;
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

    if(funcao.getNome() == "main") {
        root.map( [](TemplateTree<Tree> *t) -> void {
            if(t->getSymbol() == (int)Yamg::User_Symbols::RETURN){
                t->setSymbol(Yamg::User_Symbols::EXIT);
            }
        });
    }
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

Tree::Tree() {}

Tree::Tree(const std::string &name, const Yamg::User_Symbols symbol, const Node_type type)
    : YamgTree<Tree, AstSymbols::Funcao> { name, symbol, type }
    {}


std::ofstream Logger::logger_file{};

void Logger::setLoggerStream(std::ofstream& stream) {
    Logger::logger_file = std::move(stream);
}

void Logger::Log(const std::string& str, const std::string& expression) {
    Logger::logger_file
        << str
        << (!expression.empty() ? " (" + expression + ")" : "")
        << "\n";
}

