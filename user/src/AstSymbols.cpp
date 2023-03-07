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

    /************************
     *                      *
     *   Classe: Programa   *
     *                      *
     ************************/
    Programa Programa::programa{nullptr, {}};
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
            funcoes.emplace_back(Funcao{func.name, func.Local_Symbol_Table, func.return_type, func.f, *func.commands_head});
        }

        lido = true;
        Programa programa{msa::Program_Table.Global_Symbol_Table, funcoes};
        Programa::programa = programa;
        return programa;
    }

    const std::optional<msa::Symbol> Programa::getGlobalVar(const std::string nome) {
        try {
            msa::Symbol sym = this->variaveis_globais.at(nome);
            return sym;
        } catch (...) {
            return std::nullopt;
        }
    }

    const std::vector<Funcao> Programa::getFuncoes() {
        return funcoes;
    }

}
