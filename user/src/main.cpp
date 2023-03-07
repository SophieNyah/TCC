//
// Created by sophie on 02/03/23.
//

#include<iostream>
#include"AstSymbols.h"

int main() {
    AstSymbols::Programa programa = AstSymbols::Programa::leArquivoC();
    std::cout << AstSymbols::TiposNoNome
        .at(
                programa
                .getGlobalVar("fat")
                .value()
                .symbol_type
            );
    /*
     * Para cada função:
     *      emitir header (nome func, store $ra, etc)
     *      emitir instrucoes
     *      emitir finalizacao (load $ra, jump $ra, etc)
     */
    return 0;
}
