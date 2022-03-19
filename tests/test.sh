#!/bin/bash

base_dir=`dirname "$0"`

input_dir="$base_dir/grammars"
grammars=`ls $input_dir`

generator="$base_dir/../generator"
output_dir="$base_dir/grammar_outputs"

for grammar in $grammars;
do
    `./$generator < $input_dir/$grammar > $output_dir/$grammar`
done
