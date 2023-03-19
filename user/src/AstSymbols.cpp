//
// Created by sophie on 02/03/23.
//

#include <cstdio>
#include "AstSymbols.h"

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
            out_file << "main:\n";
        } else {
            out_file << this->nome << ":\n"
                                      "subi $sp, $sp, 4\n"
                                      "sw $ra, 0($sp)\n";
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
            if(funcName == "main"){
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

    void Programa::generateProgramHeader(std::ofstream &out_file) {
        out_file << ".data:\n\n";
        // TODO: implementar variáveis globais e literais de string
        out_file << ".text:\n\n";
    }
}
