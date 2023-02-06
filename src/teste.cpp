#include<iostream>
#include"RegAlloc.hpp"

int main() {
    RegAlloc::_newReg("$s0");
    RegAlloc::_newReg("$s1");
    RegAlloc::_newReg("$s2");
    RegAlloc::_newReg("$s3");
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v1"}, {"v9"}, {"v1"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v9"}, {"v1"}});
    RegAlloc::newInstruction({"i"},            {});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v9"}, {"v9"}, {"v9"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v4"}, {"v9"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v3"}, {"v5"}, {"v4"}});
    RegAlloc::newInstruction({"i %o"},         {{"v9"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v5"}, {"v6"}, {"v7"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v9"}, {"v8"}, {"v2"}});
    RegAlloc::newInstruction({"i %o"},         {{"v4"}, {"v6"}});
    RegAlloc::newInstruction({"i %o, %o, %o"}, {{"v9"}, {"v3"}, {"v9"}});
    RegAlloc::newInstruction({"i %o"},         {{"v4"}, {"v10"}});
    RegAlloc::newInstruction({"i %o, %o"},     {{"v9"}, {"v7"}});
    RegAlloc::allocate();
}