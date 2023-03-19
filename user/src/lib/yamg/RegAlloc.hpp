#ifndef REGISTER_ALLOCATOR_HPP
#define REGISTER_ALLOCATOR_HPP

#include<string>
#include<map>
#include<vector>
#include<optional>
#include<fstream>

constexpr bool YAMG_WRITEABLE_OPERAND = true;

class Instruction {

    public:
        struct OperandType {
            std::string name;
            bool write_operand = false;
        };

        Instruction(std::string template_instruction, std::vector<OperandType> operands, std::vector<std::string> constants={});
        std::string printInstruction(bool use_registers = false);

        /**
         * O template de uma instrução, com operadores incompletos, por exemplo "lw %o, 4(%o)", onde "%o" serão substituídos pelos operandos especificados
         * Nota: a std::string "%o" (porcentagem, letra 'o' minúscula) será substituída pelos registradores alocados,
         *       e a std::string "%c" será substituída pelas constantes.
         **/
        std::string template_instruction;
        /**
         * Uma lista de todos os operandos que irão para a instrução template.
         * Podem ser registradores virtuais que serão alocados, acessos à memória, dentre outros.
         * Ex.: <"%reg1", "%reg3">
         **/
        std::vector<OperandType> operands;
        /**
         * Uma lista de todas as constantes que irão para a instrução template.
         */
        std::vector<std::string> constants;

        /**
         * Essa é uma lista utilizada internamente pelo alocador.
         * Ela contém os registradores que foram alocados à instrução.
         */
        std::vector<std::string> registers;

};


class RegAlloc {
    
    public:
        struct live_range{
            int birth;
            int death;
        };
        struct variable{
            std::string name;
            live_range range;
        };

        RegAlloc() = delete;

        static void _setAllocator();
        static bool _isAllocatorSet();

        static void _newReg(std::string name);
        static const std::vector<std::string> _getRegs();

        static void _newSpillReg(std::string name);
        static const std::vector<std::string> _getSpillRegs();

        static void _setReadInstruction(Instruction read);
        static const std::optional<Instruction> _getReadInstruction();

        static void _setWriteInstruction(Instruction write);
        static const std::optional<Instruction> _getWriteInstruction();

        static void allocate();
        static void clearStack();
        static void printCode(std::ofstream &out_file, bool use_registers=true);
        static void newInstruction(Instruction);
        static void newInstruction(std::string template_instruction, std::vector<Instruction::OperandType> operands, std::vector<std::string> constants={});

        static void clearInstructions();

    private:
        static bool use_allocator;

        static std::map<std::string, variable> variables;
        static std::vector<Instruction> instructions;
        static std::vector<std::string> registers;
        static std::vector<std::string> spill_registers;

        static std::optional<Instruction> read_instruction;
        static std::optional<Instruction> write_instruction;

        struct Private;

};

#endif
