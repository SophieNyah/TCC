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

    static void killInterval() {

    }

    static void spill() {

    }

    static void linearScan() {
        vector<int> actives{};
        vector<RegAlloc::variable> intervals{};
        for(auto var: RegAlloc::variables) { intervals.push_back(var.second); }
        std::sort(intervals.begin(), intervals.end(), [](RegAlloc::variable& i1, RegAlloc::variable& i2) -> bool { return i1.range.birth < i2.range.birth; });

        for(auto i: intervals){ std::cout << i.name << '[' << i.range.birth << ',' << i.range.death << "]\n"; }
    }
};

    //*** Membros públicos ***
void RegAlloc::allocate() {
    RegAlloc::Private::liveRangeAnalysis();
    RegAlloc::Private::linearScan();
}

void RegAlloc::emitCode() {}

void RegAlloc::_newReg(string name) { RegAlloc::registers.push_back(name); }
void RegAlloc::newInstruction(Instruction i) { RegAlloc::instructions.push_back(i); };
void RegAlloc::newInstruction(string template_string, vector<string> operands) { RegAlloc::instructions.push_back(Instruction{template_string, operands}); };
