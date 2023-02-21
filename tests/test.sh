#!/bin/bash

print_to_terminal=false
while getopts ":f" opt;
do
    case $opt in
        f)
            print_to_terminal=true
        ;;
    esac
done

base_dir=`dirname "$0"`

input_dir="$base_dir/grammars"
grammars=`ls $input_dir`

generator="$base_dir/../output/src/generator"
grammar_output_dir="$base_dir/../output/grammars"
dependencies="$grammar_output_dir/../src/RegAlloc.o"

echo -e "$generator"
for grammar in $grammars;
do
    echo -e "Running $grammar:"
    
    name=${grammar%.*}
    `$generator $grammar_output_dir/$name < $input_dir/$grammar`
#    `./$generator $grammar_output_dir/$name < $input_dir/$grammar`
    `g++ --std=c++17 $grammar_output_dir/$name.cpp -o $grammar_output_dir/$name $dependencies`

    echo -e ""
done
