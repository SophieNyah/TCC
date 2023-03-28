//
// Created by sophie on 02/03/23.
//

#include <iostream>
#include "AstSymbols.h"
#include "Tree.h"
#include "./lib/yamg/RegAlloc.hpp"

int main(int argc, char* argv[]) {
    std::string out_file_name{ "fatorial" };
    if(argc > 1) {
        out_file_name = argv[1];
    }
    AstSymbols::Programa programa = AstSymbols::Programa::leArquivoC();

    std::ofstream file{"../output/" + out_file_name + ".asm", std::ios::out | std::ios::trunc};
    std::ofstream logging{"../output/" + out_file_name + ".log", std::ios::out | std::ios::trunc};
    Logger::setLoggerStream(logging);

    std::vector<std::pair<AstSymbols::Funcao, Tree>> funcoesArvore{};
    for (AstSymbols::Funcao f: programa.getFuncoes()) {
        Tree arvore{};
        arvore.readTree(f);
        funcoesArvore.emplace_back(f, arvore);
    }
    programa.generateProgramHeader(file);

    for (std::pair<AstSymbols::Funcao, Tree> funcaoArvore: funcoesArvore) {
        Logger::Log(funcaoArvore.first.getNome() + ":");
        funcaoArvore.first.generateHeader(file);
        Yamg::matchMaximalMunch(funcaoArvore.second);

        RegAlloc::allocate();

        RegAlloc::printCode(file);

        RegAlloc::clearInstructions();
        Logger::Log("");
    }

    /*
    for(AstSymbols::Funcao funcao: programa.getFuncoes()){
        Tree arvorePrograma{};
        arvorePrograma.readTree(funcao);

        Yamg::matchMaximalMunch(arvorePrograma);
//        Yamg::matchMinimalMunch(arvorePrograma);
//        Yamg::matchDynamicProgramming(arvorePrograma)

//        if(funcao.getNome() == "main") RegAlloc::clearStack();
        RegAlloc::allocate();

        funcao.generateHeader(file);
        RegAlloc::printCode(file);
//        if(funcao.getNome() == "main") funcao.generateFooter(file);

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
