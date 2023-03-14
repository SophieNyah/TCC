//
// Created by sophie on 02/03/23.
//

#include<iostream>
#include"AstSymbols.h"
#include "Tree.h"

int main() {
    AstSymbols::Programa programa = AstSymbols::Programa::leArquivoC();
    Tree arvorePrograma{};
    arvorePrograma.readTree(programa.getFuncoes()[0]);
    Yamg::matchMaximalMunch(arvorePrograma);
//    Yamg::matchMinimalMunch(arvorePrograma);
    /*
     * Para cada função:
     *      emitir header (nome func, store $ra, etc)
     *      emitir instrucoes
     *      emitir finalizacao (load $ra, jump $ra, etc)
     */
    return 0;
}
