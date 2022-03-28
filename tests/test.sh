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

generator="$base_dir/../output/generator"
output_dir="$base_dir/grammar_outputs"

for grammar in $grammars;
do
    echo -e "Running $grammar:"
    
    if [ $print_to_terminal = true ];
    then
        echo -e `./$generator < $input_dir/$grammar`
    else
        `./$generator < $input_dir/$grammar > $output_dir/$grammar`
    fi

    echo -e ""
done
