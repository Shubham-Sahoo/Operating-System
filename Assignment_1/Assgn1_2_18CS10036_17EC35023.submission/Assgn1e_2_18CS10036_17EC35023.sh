#!/bin/bash
if [ "$#" -eq "1" ] 
then lenc=$1 
else lenc=16 
fi
res=$(cat </dev/urandom | tr -dc 'a-zA-Z0-9_' | head -c $lenc ; echo)
echo $res
