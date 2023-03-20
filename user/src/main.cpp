//
// Created by sophie on 02/03/23.
//

#include <iostream>
#include "AstSymbols.h"
#include "Tree.h"
#include "./lib/yamg/RegAlloc.hpp"

int main() {
    msa::setup(stdin);
    msa::yyparse();
    auto programa = msa::Program_Table;

//    AstSymbols::Programa programa = AstSymbols::Programa::leArquivoC();
/*
    std::ofstream file{"../output/out.md", std::ios::out | std::ios::trunc};
    programa.generateProgramHeader(file);

    for(AstSymbols::Funcao funcao: programa.getFuncoes()){
//        if(funcao.getNome() != "main") continue;
        Tree arvorePrograma{};
        arvorePrograma.readTree(funcao);

        Yamg::matchMaximalMunch(arvorePrograma);
//        Yamg::matchMinimalMunch(arvorePrograma);
//        Yamg::matchDynamicProgramming(arvorePrograma)

        if(funcao.getNome() != "main") RegAlloc::clearStack();
        RegAlloc::allocate();

        funcao.generateHeader(file);
        RegAlloc::printCode(file);
        funcao.generateFooter(file);

        RegAlloc::clearInstructions();
    }

    /*
     * Para cada função:
     *      emitir header (nome func, store $ra, etc)
     *      emitir instrucoes
     *      emitir finalizacao (load $ra, jump $ra, etc)
     */
    return 0;
}
