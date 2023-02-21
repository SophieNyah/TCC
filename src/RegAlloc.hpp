#ifndef REGISTER_ALLOCATOR_HPP
#define REGISTER_ALLOCATOR_HPP

#include<string>
#include<map>
#include<vector>
#include<optional>

using namespace std;

constexpr bool YAMG_WRITEABLE_OPERAND = true;

class Instruction {

    private:
        /**
         * O template de uma instrução, com operadores incompletos, por exemplo "lw %o, 4(%o)", onde "%o" serão substituídos pelos operandos especificados
         * Nota: apenas a string "%o" (porcentagem, letra 'o' minúscula) será reconhecida para substituição
         **/
        string template_instruction;

    public:
        struct OperandType {
            string name;
            bool write_operand = false;
        };

        Instruction(string template_instruction, vector<OperandType> operands);
        string printInstruction(bool use_registers);

        /**
         * Uma lista de todos os operandos que irão para a instrução template
         * Podem ser constantes, registradores virtuais (começando com '%') que serão alocados, acessos à memória, dentre outros
         * Ex.: <"%reg1", "%reg3">
         **/
        vector<OperandType> operands;
        vector<string> registers;

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

        static void _setAllocator();
        static bool _isAllocatorSet();
        static void _newReg(string name);
        static void _newSpillReg(string name);
        static void _setReadInstruction(Instruction read);
        static void _setWriteInstruction(Instruction write);
        
        static void allocate();
        static void printCode(bool use_registers=true);
        static void newInstruction(Instruction);
        static void newInstruction(string template_instruction, vector<Instruction::OperandType> operands);

    private:
        static bool use_allocator;

        static map<string, variable> variables;
        static vector<Instruction> instructions;
        static vector<string> registers;
        static vector<string> spill_registers;

        static optional<Instruction> read_instruction;
        static optional<Instruction> write_instruction;

        struct Private;
        // static void linearScan();
        // static void liveRangeAnalysis();

};

#endif
