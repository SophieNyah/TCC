#include<iostream>
#include"RegAlloc.hpp"

int main() {
    RegAlloc::_newReg("$s0");
    RegAlloc::_newReg("$s1");
    RegAlloc::_newReg("$s2");
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v1"}, {"v2"}, {"v1"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v2"}, {"v3"}});
    RegAlloc::newInstruction({"i"},            {});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v1"}, {"v2"}, {"v3"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v4"}, {"v1"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v5"}, {"v3"}, {"v4"}});
    RegAlloc::newInstruction({"i %o"},         {{"v2"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v3"}, {"v6"}, {"v7"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v1"}, {"v8"}, {"v9"}});
    RegAlloc::newInstruction({"i %o"},         {{"v4"}, {"v6"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v2"}, {"v5"}, {"v6"}});
    RegAlloc::newInstruction({"i %o"},         {{"v4"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v6"}, {"v7"}});
    RegAlloc::allocate();
}