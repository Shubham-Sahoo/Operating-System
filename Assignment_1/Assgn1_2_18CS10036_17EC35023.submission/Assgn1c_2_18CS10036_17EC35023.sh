#!/bin/bash

FILE=$1
NUM=$2
out=1c_output_$2_column.freq
touch $out
awk -v out="$out" -v NUM="$NUM" '
{
arr[tolower($	NUM)]++
}
END{
for (i in arr) {
      print i,arr[i] > out
   }
}
' $FILE
sort -k 2nr -o $out $out
