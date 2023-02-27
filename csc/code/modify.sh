#!/bin/bash

src=$1
str=$2
rep=$3

sed -i "s/$str/$rep/g" $src
