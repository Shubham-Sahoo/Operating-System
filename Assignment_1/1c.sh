#!/bin/bash

FILE = $1
NUM = $2
touch 1c_output_$2_column.freq.txt
awk 'BEGIN{
	while IFS= read -r line; do
	COUNT=0
    for word in line
    do
    	if [[ COUNT == $2 ]]; then
    		echo $word | tr '[:upper:]' '[:lower:]'
    		((COUNT++))
    	fi
done < "$1"

}'

