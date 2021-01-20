#!/bin/bash

mkdir 1.b.files.out
FILES=$(ls 1.b.files/)
FILES=(${FILES[@]})
touch 1.b.out.txt
for FILE in ${FILES[@]}
do	
	det=$(sort -nr "1.b.files/$FILE")
	touch "1.b.files.out/$FILE"
	echo $det > "1.b.files.out/$FILE"
	echo $det >> 1.b.out.txt
done        
sort -nr 1.b.out.txt > /dev/null
