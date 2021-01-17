#!/bin/bash

gcd () { 
if [ $1 -eq 0 ]
then 
	return $2
fi 
gcd `expr $2 % $1` $1
next=$?
return $next
}
IFS=','
val=($@)

if [ ${#val[@]} -eq 1 ]
then
	echo "$1"
	exit 0
elif [ ${#val[@]} -eq 0 ]
then
	exit 0
fi

if [ ${#val[@]} -lt 10 ]
then
	if [ ${val[0]} -ge 0 ]
	then
		res=${val[0]}
	else
		res=`expr ${val[0]} \* -1`	
	fi
	i=1
	while [ $i -lt ${#val[@]} ]
	do
		if [ ${val[$i]} -ge 0 ]
		then
			mul=${val[$i]}
		else
			mul=`expr ${val[$i]} \* -1`	
		fi
		gcd $res $mul
		res=$?
		i=`expr $i + 1`
	done
fi
echo "$res"
