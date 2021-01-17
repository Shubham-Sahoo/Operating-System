#!/bin/bash

mkdir 1.b.files.out

FILES = $(1.b.files/ls *.txt)
touch 1.b.out.txt
for FILE in $FILES
    do
        sort -r $FILE
        $FILE >> 1.b.out.txt
        touch "1.b.files.out/$FILE"
done        

sort -r 1.b.out.txt


