//
// Created by sophie on 02/03/23.
//

#include <cstdio>
#include <iostream>
#include "AstSymbols.h"
#include "lib/yamg/RegAlloc.hpp"

namespace AstSymbols {

    std::map<std::string, msa::Symbol> hashTableToMap(msa::HashTable table) {
        std::map<std::string, msa::Symbol> map{};
        if(!table) return map;
        char** c_keys =  msa::getAllKeys(table);

        for(int i=0; c_keys[i]; i++) {
            std::string key{ c_keys[i] };
            msa::Symbol sym = *( (msa::Symbol*)msa::getPrimeiroRegistro(table, c_keys[i]) );
            map.insert({key, sym});
        }

        return map;
    }

    /**********************
     *                    *
     *   Classe: Funcao   *
     *                    *
     **********************/
    Funcao::Funcao(std::string nome, msa::HashTable var_locais, msa::Var_type retorno, msa::function_prototype prototipo, msa::command_list comandos):
        nome{ nome },
        tabelaLocal{ hashTableToMap(var_locais) },
        tipo_retorno{ retorno },
        prototipo{ prototipo },
        comandos{ comandos }
        {}

    const std::string Funcao::getNome() {
        return this->nome;
    }

    const std::optional<msa::Symbol> Funcao::getLocalVar(const std::string nome) {
        try {
            msa::Symbol sym = this->tabelaLocal.at(nome);
            return sym;
        } catch (...) {
            return std::nullopt;
        }
    }

    const msa::Var_type Funcao::getRetorno() {
        return this->tipo_retorno;
    }

    const msa::function_prototype Funcao::getPrototipo() {
        return this->prototipo;
    }

    const msa::command_list Funcao::getComandos() {
        return this->comandos;
    }

    std::map<std::string, msa::Symbol> Funcao::getLocalTable() {
        return this->tabelaLocal;
    }

    void Funcao::generateHeader(std::ofstream& out_file) {
        if(this->nome == "main") {
            out_file << "\n\nmain:\n";
        } else {
            out_file << "\n\n" << this->nome << ":\n"
                                      "subi $sp, $sp, 4\n"
                                      "sw $ra, 0($sp)\n";
        }
        for(std::pair<std::string, msa::Symbol> variavel: this->tabelaLocal) {
            std::string nome_var{ variavel.first };
            msa::Symbol simbolo = variavel.second;
            if(simbolo.var.v.array) {
                int arraySize = simbolo.var.v.array->length;
                RegAlloc::newInstruction({"subi $sp, $sp, " + std::to_string(arraySize*4)}, {});
                RegAlloc::newInstruction({"move %o, $sp"}, std::vector<Instruction::OperandType>{ {nome_var} });
            }
        }
    }
    void Funcao::generateFooter(std::ofstream& out_file) {
        if(this->nome == "main"){
            out_file << "li $v0, 10\n"
                        "syscall\n\n";
        } else {
            out_file << "lw $ra, 0($sp)\n"
                        "addi $sp, $sp, 4\n"
                        "jr $ra\n\n";
        }
    }

    /************************
     *                      *
     *   Classe: Programa   *
     *                      *
     ************************/
    Programa Programa::programa{nullptr, {}};
    std::map<std::string, std::string> Programa::string_literals{};
    Programa::Programa(msa::HashTable var_globais, std::vector<Funcao> funcoes):
        variaveis_globais{ hashTableToMap(var_globais) },
        funcoes{ funcoes }
        {}

    Programa Programa::leArquivoC() {
        static bool lido{ false };
        if(lido) return Programa::programa;

        msa::setup(stdin);
        msa::yyparse();
        std::vector<Funcao> funcoes{};
        for(auto aux = msa::Program_Table.head; aux; aux=aux->next){
            auto func = aux->function;
            std::string funcName{func.name};
            Funcao funcao{funcName, func.Local_Symbol_Table, func.return_type, func.f, *func.commands_head};
            if(funcName == "main") {
                funcoes.insert(funcoes.begin(), funcao);
            } else {
                funcoes.emplace_back(funcao);
            }
        }

        lido = true;
        Programa::programa = Programa{msa::Program_Table.Global_Symbol_Table, funcoes};
        return Programa::programa;
    }

    std::optional<msa::Symbol> Programa::getGlobalVar(const std::string nome) {
        try {
            msa::Symbol sym = this->variaveis_globais.at(nome);
            return sym;
        } catch (...) {
            return std::nullopt;
        }
    }

    std::vector<Funcao> Programa::getFuncoes() {
        return funcoes;
    }

    bool Programa::insertStringLiteral(const std::string name, const std::string str) {
        auto p = Programa::string_literals.insert(std::pair<std::string, std::string>(str, name));
        return p.second;
    }

    std::optional<std::string> Programa::getLiteralVar(const std::string &literal) {
        try {
            return Programa::string_literals.at(literal);
        } catch(std::out_of_range const&) {
            return std::nullopt;
        }
    }

    void Programa::generateProgramHeader(std::ofstream &out_file) {
        out_file << ".data:\n";
        for(std::pair<std::string, msa::Symbol> var: variaveis_globais) {
            msa::Symbol symbol{ var.second };
            if(symbol.symbol_type == msa::tipos::DECLARACAO_FUNCAO) continue;
            switch(symbol.type.type) {
                case msa::tipos::TIPOS_INT:
                    out_file << symbol.id << ": .word " << symbol.var.v.value.i << "\n";
                    break;
                case msa::tipos::TIPOS_CHAR:
                    out_file << symbol.id << ": .byte: \'" << symbol.var.v.value.c << " \'\n";
                    break;
            }
        }
        for(std::pair<std::string, std::string> var: string_literals) {
            std::string var_name{ var.second };
            std::string literal{ var.first };
            out_file << var_name << ": .asciiz " << literal << "\n";
        }
        out_file << "\n.text:\n\n";
    }
}
