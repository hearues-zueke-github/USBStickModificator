#! /bin/bash

#gcc -Wall -o $1 $1.c

if [ $# -eq 1 ] ; then
	echo "BEGIN with Compile:"
	arg1=($1)
	size=${#arg1}
	program_name=${arg1:0:$size-2}
	source_name=$arg1
	gcc -march=x86-64 -Wall -o $program_name $source_name
	echo "END of Compile"
elif [ $# -eq 2 ] ; then
	echo "BEGIN with Compile:"
	arg1=($1)
	arg2=($2)
	size=${#arg1}
	program_name=${arg1:0:$size-2}
	source_name=$arg1
	gcc $arg2 -Wall -o $program_name $source_name
	echo "END of Compile"
fi
