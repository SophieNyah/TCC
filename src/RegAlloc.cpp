#include<algorithm>
#include"RegAlloc.hpp"

using namespace std;

/**************************
 *                        *
 *   Classe Instruction   *
 *                        *
 **************************/

Instruction::Instruction(string template_instruction, vector<OperandType> operands, vector<string> constants):
    template_instruction{ template_instruction },
    operands{ operands },
    registers{},
    constants{ constants }
    {}
#include<iostream>

string replaceAllTokens(const string &delimiter, const string &str, const vector<string> &values) {
    string replaced{};
    string tokenize{ str };
    string token{};
    size_t pos{ 0 };
    int iteration{ 0 };

    while( (pos = tokenize.find(delimiter)) != string::npos ){
        token = tokenize.substr(0, pos);
        replaced.append(token);
        if(values.size() >= iteration) replaced.append(values[iteration]);
        tokenize.erase(0, pos + delimiter.length());
        iteration++;
    }
    if(tokenize.size() > 0) replaced.append(tokenize);

    return replaced;
}
string Instruction::printInstruction(bool use_registers) {
    if(this->registers.empty() && this->operands.empty() && this->constants.empty()){
        if(this->registers.size() == 0) return this->template_instruction;
    }

    string instruction{};
    vector<string> operands{};

    if(use_registers) {
        operands = this->registers;
    } else {
        for(auto op: this->operands) { operands.emplace_back(op.name); }
    }

    instruction = replaceAllTokens("%o", this->template_instruction, operands);
    instruction = replaceAllTokens("%c", instruction, this->constants);

    return instruction;
}

/*************************
 *                       *
 *    Classe RegAlloc    *
 *                       *
 *************************/

    //*** Atributos ***
bool RegAlloc::use_allocator{ false };
optional<Instruction> RegAlloc::read_instruction{std::nullopt};
optional<Instruction> RegAlloc::write_instruction{std::nullopt};
map<string, RegAlloc::variable> RegAlloc::variables{};
vector<Instruction> RegAlloc::instructions{};
vector<string>      RegAlloc::registers{};
vector<string>      RegAlloc::spill_registers{};
constexpr size_t sizeOfVariable = 4;
const std::string clearStackValue    = "# clear stack";
const std::string storeStackValue    = "# store stack";
const std::string retrieveStackValue = "# retrieve stack";

    //*** Funções privadas ***
struct RegAlloc::Private {
    using reg_name = string;
    using variable = RegAlloc::variable;
    using name_variable = std::pair<reg_name, RegAlloc::variable>;

    /**
     * Manipulação de memória
     */

    static vector<Instruction>::iterator _clearStack(const vector<Instruction>::iterator &iterator, const vector<variable> &memory) {
        if(memory.empty()) {
//            iterator = RegAlloc::instructions.erase(iterator);
            return iterator;
        }
        size_t stack_size{ memory.size() };
        iterator.base()->template_instruction = "addi $sp, $sp, " + to_string(stack_size * sizeOfVariable) + "  #clear stack";
        return iterator;
    }
    static vector<Instruction>::iterator _storeStack(vector<Instruction>& instructions, vector<Instruction>::iterator& iterator, vector<vector<name_variable>>::iterator& actives_it) {
        vector<name_variable> actives_array{ *actives_it.base() };
        if(actives_array.empty()) {
//            iterator = instructions.erase(iterator);
            return iterator;
        }
        size_t used_registers{ actives_array.size() };
        iterator.base()->template_instruction = "subi $sp, $sp, " + to_string(used_registers * sizeOfVariable) + "  #store stack";
        for(int i=0; i<used_registers; i++){
            Instruction instruction{ {"sw " + actives_array[i].first + ", " + to_string(i * sizeOfVariable) + "($sp)  #store stack"}, {} };
            iterator = instructions.insert(iterator+1, instruction);
        }
        return iterator;
    }
    static vector<Instruction>::iterator _retrieveStack(vector<Instruction>& instructions, vector<Instruction>::iterator& iterator, vector<vector<name_variable>>::iterator& actives_it) {
        vector<name_variable> actives_array{ *actives_it.base() };
        if(actives_array.empty()) {
//            iterator = instructions.erase(iterator);
            return iterator;
        }
        size_t used_registers{ actives_array.size() };
        for(int i=used_registers - 1; i>=0; i--){
            std::string template_instruction{"lw " + actives_array[i].first + ", " + to_string(i * sizeOfVariable) + "($sp)  #retrieve stack"};
            if(i == used_registers-1) {
                iterator.base()->template_instruction = template_instruction;
            } else {
                iterator = instructions.insert(iterator + 1, Instruction{ template_instruction, {} });
            }
        }
        instructions.insert(iterator+1, { {"addi $sp, $sp, " + to_string(used_registers * sizeOfVariable) + "  #retrieve stack"}, {} });
        return iterator;
    }

    static optional<int> findPositionInMemory(vector<variable> &memory, string var_name) {
        auto it = std::find_if(memory.begin(), memory.end(), [var_name](variable var)->bool { return var_name == var.name; });
        if(it == memory.end()) {
            return nullopt;
        } else {
            return it - memory.begin();
        }
    }
    static void storeInMemory(vector<Instruction>::iterator &it, vector<variable> &memory, variable var) {
        RegAlloc::instructions.emplace(it, Instruction{"subi $sp, $sp, 4", {}});
        memory.emplace_back(var);
    }

    /**
     * Emissão de instruções 
     */

    static optional<vector<Instruction>::iterator> emitRead(vector<Instruction> &instructions, vector<Instruction>::iterator &i, vector<variable> &memory, Instruction::OperandType &var, string &reg_name) {
        if(!RegAlloc::read_instruction.has_value()) return nullopt;
        optional<int> read_variable_position = findPositionInMemory(memory, var.name);
        if(!read_variable_position.has_value()) return nullopt;
        Instruction new_i = RegAlloc::read_instruction.value();
        new_i.registers.emplace_back(reg_name);
        new_i.registers.emplace_back(std::to_string(read_variable_position.value()*sizeOfVariable));
        return instructions.insert(i, new_i) + 1;
    }
    static optional<vector<Instruction>::iterator> emitWrite(vector<Instruction> &instructions, vector<Instruction>::iterator &i, vector<variable> &memory, Instruction::OperandType &var, string &reg_name) {
        if(!RegAlloc::write_instruction.has_value()) return nullopt;
        if(!var.write_operand) return nullopt;
        optional<int> write_variable_position = findPositionInMemory(memory, var.name);
        if(!write_variable_position.has_value()) return nullopt;
        Instruction new_i = RegAlloc::write_instruction.value();
        new_i.registers.emplace_back(reg_name);
        new_i.registers.emplace_back(std::to_string(write_variable_position.value()*sizeOfVariable));
        return instructions.insert(i+1, new_i) + 1;
    }

    static void convertInstructions(vector<vector<name_variable>> &actives_array, vector<variable> &memory) {
        int instruction_counter{ 1 };
        auto actives_iterator{ actives_array.begin() };

        for(auto instruction_it = RegAlloc::instructions.begin(); instruction_it!=RegAlloc::instructions.end(); instruction_it++) {
            Instruction &instruction = *instruction_it.base();
            if(instruction.template_instruction == clearStackValue) {
                instruction_it = _clearStack(instruction_it, memory);
                if(instruction_it == RegAlloc::instructions.end()) break;
                continue;
            } else if(instruction.template_instruction == storeStackValue) {
                instruction_it = _storeStack(RegAlloc::instructions, instruction_it, actives_iterator);
                continue;
            } else if(instruction.template_instruction == retrieveStackValue) {
                instruction_it = _retrieveStack(RegAlloc::instructions, instruction_it, actives_iterator);
                continue;
            }

            vector<Instruction::OperandType> vars_in_memory{};
            int in_memory{ 0 };

            if(actives_iterator+1 != actives_array.end()){
                int greatest_next_birth =
                    max_element( (actives_iterator+1).base()->begin(), (actives_iterator+1).base()->end(), [](name_variable a, name_variable b)-> bool {
                        return a.second.range.birth < b.second.range.birth;
                    })
                    .base()->second.range.birth;
                if(instruction_counter >= greatest_next_birth) {
                    actives_iterator++;
                }
            }
            vector<name_variable> curr_actives{ *actives_iterator.base() };

            for(Instruction::OperandType operand: instruction.operands) {
                auto register_iterator = std::find_if(curr_actives.begin(), curr_actives.end(), [operand](name_variable var)->bool { return operand.name == var.second.name; });
                if(register_iterator != curr_actives.end()) {
                    instruction.registers.emplace_back(register_iterator->first);
                } else {
                    instruction.registers.emplace_back(RegAlloc::spill_registers[in_memory]);
                    vars_in_memory.emplace_back(operand);
                    in_memory++;
                }
            }

            for(int i=0; i<in_memory; i++) {
                if(auto iterator = emitRead(instructions, instruction_it, memory, vars_in_memory[i], RegAlloc::spill_registers[i]); iterator.has_value())
                    instruction_it = iterator.value();
            }
            for(int i=0; i<in_memory; i++) {
                if(auto iterator = emitWrite(instructions, instruction_it, memory, vars_in_memory[i], RegAlloc::spill_registers[i]); iterator.has_value())
                    instruction_it = iterator.value();
                if(instruction_it == instructions.end()) return;
            }

            instruction_counter++;
        }
    }

    /**
     * Manipulação ativos
     */

    static void insertActives(vector<name_variable> &actives, name_variable var) {
        auto position = std::find_if(actives.begin(), actives.end(), [var](name_variable v)->bool { return v.second.range.death > var.second.range.death; });
        actives.insert(position, var);
    }
    static void removeActives(vector<name_variable> &actives, name_variable var) {
        auto eraser_function = [var](name_variable v)-> bool { return v.first == var.first; };
        actives.erase(std::remove_if(actives.begin(), actives.end(), eraser_function), actives.end());
    }

    /**
     * Análise e operações em intervalos
     */

    static void liveRangeAnalysis() {
        int instructionNo{ 1 };
        for(auto instruction: RegAlloc::instructions) {
            for(auto operand: instruction.operands) {
                if(RegAlloc::variables.find(operand.name) == RegAlloc::variables.end()){
                    RegAlloc::variables.emplace(operand.name, variable{ operand.name, { instructionNo, instructionNo } });
                } else {
                    variable& var = RegAlloc::variables.at(operand.name);
                    var.range.death = instructionNo;
                }
            }
            instructionNo++;
        }
    }

    static void killInterval(vector<name_variable> &actives, vector<string> &avaialble_registers, vector<string> &used_registers, variable &var) {
        for(name_variable curr_var: actives) {
            if(curr_var.second.range.death >= var.range.birth){
                return;
            } else {
                removeActives(actives, curr_var);
                used_registers.erase(std::remove(used_registers.begin(), used_registers.end(), curr_var.first), used_registers.end());
                avaialble_registers.emplace_back(curr_var.first);
                return;
            }
        }
    }

    static void spill(vector<name_variable> &actives, vector<variable> &memory_registers, variable &var) {
        name_variable spilled{ actives.back() };
        vector<Instruction>::iterator curr_instruction = RegAlloc::instructions.begin() + var.range.birth - 1;
        if(spilled.second.range.death > var.range.death) {
            storeInMemory(curr_instruction, memory_registers, spilled.second);
            actives.pop_back();
            insertActives(actives, std::make_pair(spilled.first, var));
        } else {
            storeInMemory(curr_instruction, memory_registers, var);
        }
    }


    static void linearScan() {
        vector<name_variable> actives{};
        vector<variable> intervals{};
        vector<string> available_registers{ RegAlloc::registers };
        vector<string> used_registers{};
        vector<variable> memory_registers{};

        vector<vector<name_variable>> actives_array{};
        int last_birth_date{ 0 };

        for(auto var: RegAlloc::variables) { intervals.push_back(var.second); }
        auto comparator_function = [](variable& i1, variable& i2) -> bool { return i1.range.birth < i2.range.birth; };
        std::sort(intervals.begin(), intervals.end(), comparator_function);

        for(auto i: intervals) {
            killInterval(actives, available_registers, used_registers, i);
            if(actives.size() == RegAlloc::registers.size()){
                spill(actives, memory_registers, i);
            } else {
                insertActives(actives, std::make_pair(available_registers.back(), i));
                available_registers.pop_back();
            }

            if(last_birth_date == i.range.birth) {
                actives_array.pop_back();
                actives_array.emplace_back(actives);
            } else {
                actives_array.emplace_back(actives);
            }
            last_birth_date = i.range.birth;
        }

        convertInstructions(actives_array, memory_registers);
    }
};

    //*** Membros públicos ***
void RegAlloc::allocate() {
    if(RegAlloc::instructions.empty()) return;
    bool no_operands{true};
    for(const Instruction &i: RegAlloc::instructions){
        if(!i.operands.empty()){
            no_operands = false;
            break;
        }
    }
    if(no_operands) return;
    RegAlloc::Private::liveRangeAnalysis();
    RegAlloc::Private::linearScan();
}

void RegAlloc::printCode(ofstream &out_file, bool use_registers) {
    for(Instruction i: RegAlloc::instructions) {
        out_file << i.printInstruction(use_registers) << '\n';
    }
}

void RegAlloc::_setAllocator() { RegAlloc::use_allocator = true; }
bool RegAlloc::_isAllocatorSet() { return RegAlloc::use_allocator; }

void RegAlloc::_newReg(string name) { RegAlloc::registers.push_back(name); }
const vector<string> RegAlloc::_getRegs() { return RegAlloc::registers; }

void RegAlloc::_newSpillReg(string name) { RegAlloc::spill_registers.push_back(name); }
const vector<string> RegAlloc::_getSpillRegs() { return RegAlloc::spill_registers; }

void RegAlloc::_setReadInstruction(Instruction read) { RegAlloc::read_instruction = read; }
const optional<Instruction> RegAlloc::_getReadInstruction() { return RegAlloc::read_instruction; }

void RegAlloc::_setWriteInstruction(Instruction write) { RegAlloc::write_instruction = write; }
const optional<Instruction> RegAlloc::_getWriteInstruction() { return RegAlloc::write_instruction; }

void RegAlloc::newInstruction(Instruction i) { RegAlloc::instructions.push_back(i); }
void RegAlloc::newInstruction(string template_string, vector<Instruction::OperandType> operands, vector<string> constants) {
    RegAlloc::instructions.push_back(Instruction{template_string, operands, constants});
}

void RegAlloc::storeStack()    { RegAlloc::newInstruction(storeStackValue, {}); }
void RegAlloc::retrieveStack() { RegAlloc::newInstruction(retrieveStackValue, {}); }
void RegAlloc::clearStack()    { RegAlloc::newInstruction(clearStackValue, {}); }

void RegAlloc::clearInstructions() { RegAlloc::instructions.clear(); }
