#!/bin/bash
#First you can use grep (-n) to find the number of lines of string.
#Then you can use awk to separate the answer.

src=$1
str=$2
dst=$3

grep -n $str < $src | sed -n -e 's/\:.*//gp' > $dst
