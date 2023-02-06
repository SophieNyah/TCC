#include<algorithm>
#include"RegAlloc.hpp"

/**************************
 *                        *
 *   Classe Instruction   *
 *                        *
 **************************/

Instruction::Instruction(string template_instruction, vector<string> operands):
    template_instruction{ template_instruction },
    operands{ operands }
    {}
#include<iostream>
string Instruction::printInstruction() {
    string instruction{};
    string tokenize{ this->template_instruction };
    string delimiter{ "%o" };
    string token{};
    size_t pos{ 0 };
    int iteration{ 0 };

    while((pos = tokenize.find(delimiter)) != string::npos){
        token = tokenize.substr(0, pos);
        instruction.append(token);
        if(this->operands.size() >= iteration)
            instruction.append(this->operands[iteration]);
        tokenize.erase(0, pos+delimiter.length());
        iteration++;
    }
    if(this->operands.size() >= iteration)
        instruction.append(this->operands[iteration]);
    instruction.append(tokenize);
    return instruction;
}

/*************************
 *                       *
 *    Classe RegAlloc    *
 *                       *
 *************************/

    //*** Atributos ***
map<string, RegAlloc::variable> RegAlloc::variables{};
vector<Instruction> RegAlloc::instructions{};
vector<string>      RegAlloc::registers{};
// vector<regId>       RegAlloc::actives{};
// vector<regId>       RegAlloc::availables{};

    //*** Funções privadas ***
struct RegAlloc::Private {
    static void liveRangeAnalysis() {
        int instructionNo{ 1 };
        for(auto instruction: RegAlloc::instructions) {
            for(auto operand: instruction.operands) {
                if(RegAlloc::variables.find(operand) == RegAlloc::variables.end()){
                    RegAlloc::variables.emplace(operand, RegAlloc::variable{ operand, { instructionNo, instructionNo } });
                } else {
                    RegAlloc::variable& var = RegAlloc::variables.at(operand);
                    var.range.death = instructionNo;
                }
            }
            instructionNo++;
        }
    }

    using reg_name = string;
    using name_variable = std::pair<reg_name, RegAlloc::variable>;

    static void insert_actives(vector<name_variable> &actives, name_variable var) {
        auto position = std::find_if(actives.begin(), actives.end(), [var](name_variable v)->bool { return v.second.range.death > var.second.range.death; });
        actives.insert(position, var);
    }

    static void remove_actives(vector<name_variable> &actives, name_variable var) {
        auto eraser_function = [var](name_variable v)-> bool { return v.first == var.first; };
        actives.erase(std::remove_if(actives.begin(), actives.end(), eraser_function), actives.end());
    }

    static void killInterval(vector<name_variable> &actives, vector<string> &avaialble_registers, vector<string> &used_registers, RegAlloc::variable &var) {
        for(name_variable curr_var: actives) {
            if(curr_var.second.range.death >= var.range.birth){
                return;
            } else {
                remove_actives(actives, curr_var);
                used_registers.erase(std::remove(used_registers.begin(), used_registers.end(), curr_var.first), used_registers.end());
                avaialble_registers.emplace_back(curr_var.first);
                return;
            }
        }
    }

    static void spill(vector<name_variable> &actives, vector<RegAlloc::variable> &memory_registers, RegAlloc::variable &var) {
        name_variable spilled{ actives.back() };
        if(spilled.second.range.death > var.range.death) {
            memory_registers.emplace_back(spilled.second);
            actives.pop_back();
            insert_actives(actives, std::make_pair(spilled.first, var));
        }
    }

    static void linearScan() {
        vector<name_variable> actives{};
        vector<RegAlloc::variable> intervals{};
        vector<string> available_registers{ RegAlloc::registers };
        vector<string> used_registers{};
        vector<RegAlloc::variable> memory_registers{};

        for(auto var: RegAlloc::variables) { intervals.push_back(var.second); }
        auto comparator_function = [](RegAlloc::variable& i1, RegAlloc::variable& i2) -> bool { return i1.range.birth < i2.range.birth; };
        std::sort(intervals.begin(), intervals.end(), comparator_function);

        for(auto i: intervals) {
            killInterval(actives, available_registers, used_registers, i);
            if(actives.size() == RegAlloc::registers.size()){
                spill(actives, memory_registers, i);
            } else {
                insert_actives(actives, std::make_pair(available_registers.back(), i));
                available_registers.pop_back();
            }
            for(auto a: actives){ std::cout << '[' << a.first << ':' << a.second.name << ' ' << a.second.range.birth << ',' << a.second.range.death << ']' << "  " ; } std::cout << "\n\n";
        }
    }
};

    //*** Membros públicos ***
void RegAlloc::allocate() {
    RegAlloc::Private::liveRangeAnalysis();
    RegAlloc::Private::linearScan();
}

void RegAlloc::emitCode() {}

void RegAlloc::_newReg(string name) { RegAlloc::registers.push_back(name); };
void RegAlloc::newInstruction(Instruction i) { RegAlloc::instructions.push_back(i); };
void RegAlloc::newInstruction(string template_string, vector<string> operands) { RegAlloc::instructions.push_back(Instruction{template_string, operands}); };
