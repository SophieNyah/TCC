#ifndef REGISTER_ALLOCATOR_HPP
#define REGISTER_ALLOCATOR_HPP

#include<string>
#include<map>
#include<vector>

using namespace std;
using regId = int;

class Instruction {

    private:
        /**
         * O template de uma instrução, com operadores incompletos, por exemplo "lw %o, 4(%o)", onde "%o" serão substituídos pelos operandos especificados
         * Nota: apenas a string "%o" (porcentagem, letra 'o' minúscula) será reconhecida para substituição
         **/
        string template_instruction;

    public:
        Instruction(string template_instruction, vector<string> operands);
        string printInstruction();
        
        /**
         * Uma lista de todos os operandos que irão para a instrução template
         * Podem ser constantes, registradores virtuais (começando com '%') que serão alocados, acessos à memória, dentre outros
         * Ex.: <"%reg1", "%reg3">
         **/
        vector<string> operands;

};


class RegAlloc {
    
    public:
        struct live_range{
            int birth;
            int death;
        };
        struct variable{
            string name;
            live_range range;
        };

        RegAlloc() = delete;

        static void allocate();
        static void emitCode();
        static void _newReg(string name);
        static void newInstruction(Instruction);
        static void newInstruction(string template_instruction, vector<string> operands);

    private:
        static map<string, variable> variables;
        static vector<Instruction> instructions;
        static vector<string> registers;
        // static vector<regId> actives;
        // static vector<regId> availables;

        struct Private;
        // static void linearScan();
        // static void liveRangeAnalysis();

};

#endif
