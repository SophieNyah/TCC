//
// Created by sophie on 02/03/23.
//

#ifndef TCC_ASTSYMBOLS_H
#define TCC_ASTSYMBOLS_H

namespace msa {
    extern "C" {

#include "./lib_msa/ast_symbols.h"
#include "./lib_msa/semantico.tab.h"

    }
}
#include <array>
#include <string>
#include <variant>
#include <map>
#include <vector>
#include <optional>

namespace AstSymbols {
    class Expressao {
        Expressao *dir;
        Expressao *esq;
        msa::tipos tipo_node;
        msa::Var_type tipo_expressao;
        std::variant<int, std::string, std::string, msa::Symbol> valor_node;
        Expressao(msa::tipos, msa::Var_type, std::variant<int, std::string, std::string, msa::Symbol>, Expressao* = nullptr, Expressao* = nullptr);

        public:

    };

    class Funcao {
        std::string nome;
        std::map<std::string, msa::Symbol> tabelaLocal;
        msa::Var_type tipo_retorno;
        msa::function_prototype prototipo;
        msa::command_list comandos;

        public:
            Funcao(std::string, msa::HashTable, msa::Var_type, msa::function_prototype, msa::command_list);

            const std::string getNome();
            const std::optional<msa::Symbol> getLocalVar(const std::string);
            const msa::Var_type getRetorno();
            const msa::function_prototype getPrototipo();
            const msa::command_list getComandos();

    };

    class Programa {
        std::map<std::string, msa::Symbol> variaveis_globais;
        std::vector<Funcao> funcoes;
        Programa(msa::HashTable, std::vector<Funcao>);
        static Programa programa;

        public:
            static Programa leArquivoC();

            const std::optional<msa::Symbol> getGlobalVar(const std::string);
            const std::vector<Funcao> getFuncoes();

    };

    const std::array<std::string, 11> TiposNome = {
            "TIPOS_INT",
            "TIPOS_CHAR",
            "TIPOS_VOID",
            "DECLARACAO_VARIAVEL",
            "DECLARACAO_FUNCAO",
            "COM_IF",
            "COM_WHILE",
            "COM_FOR",
            "COM_RETURN",
            "COM_EXP",
            "COM_BLOCK",
    };
    // Esses valores foram obtidos a partir de ./lib_msa/semantico.tab.h
    const std::map<int, std::string> TiposNoNome {
            {258, "VOID_T"},
            {259, "INT_T"},
            {260, "CHAR_T"},
            {261, "RETURN_T"},
            {262, "DO_T"},
            {263, "WHILE_T"},
            {264, "FOR_T"},
            {265, "IF_T"},
            {266, "ELSE_T"},
            {267, "ADD"},
            {268, "SUB"},
            {269, "ASTERISK"},
            {270, "DIV"},
            {271, "MOD"},
            {272, "INC"},
            {273, "DEC"},
            {274, "AMPERSAND"},
            {275, "BIT_OR"},
            {276, "BIT_NOT"},
            {277, "BIT_XOR"},
            {278, "LOG_AND"},
            {279, "LOG_OR"},
            {280, "LOG_NOT"},
            {281, "EQUALS"},
            {282, "NOT_EQUALS"},
            {283, "LESS"},
            {284, "GREAT"},
            {285, "LEQ"},
            {286, "GEQ"},
            {287, "RSHIFT"},
            {288, "LSHIFT"},
            {289, "ASSIGN"},
            {290, "ADD_ASSIGN"},
            {291, "SUB_ASSIGN"},
            {292, "SEMICOLON"},
            {293, "COMMA"},
            {294, "COLON"},
            {295, "LPAREN"},
            {296, "RPAREN"},
            {297, "LCBRACK"},
            {298, "RCBRACK"},
            {299, "LBRACK"},
            {300, "RBRACK"},
            {301, "QUEST_MARK"},
            {302, "NUMBER_SIGN"},
            {303, "DEFINE"},
            {304, "PRINTF_T"},
            {305, "SCANF_T"},
            {306, "EXIT_T"},
            {307, "NUM_HEXA"},
            {308, "NUM_OCTA"},
            {309, "NUM_INT"},
            {310, "CHARACTER"},
            {311, "STRING"},
            {312, "IDENTIFIER"},
            {313, "LINE_COMMENT"},
            {314, "BLOCK_COMMENT"},
            {315, "UNTERMINATED_COMMENT"},
            {316, "UNKNOWN_CHAR"},
            {317, "END_OF_FILE"},
            {318, "ADDRESS"},
            {319, "MUL"},
            {320, "BIT_AND"},
            {321, "POINTER_DEFERENCE"},
            {322, "UNR_PLUS"},
            {323, "UNR_MINUS"},
            {324, "CONDITIONAL_EXP"},
            {325, "CONDITIONAL_EXP_THENELSE"},
            {326, "SUBSCRIPT"},
            {327, "CAST"},
            {328, "FUNCTION_CALL"},
            {329, "COMMA_SEPARATOR"},
            {330, "BREAK_T"},
            {331, "SWITCH_T"},
            {332, "CASE_T"},
            {333, "DEFAULT_T"},
            {334, "TYPEDEF_T"},
            {335, "STRUCT_T"},
    };

}

#endif //TCC_ASTSYMBOLS_H
