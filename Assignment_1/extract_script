#!/bin/bash

file=$1
if [ "${file: -4}" == ".tar" ] 
then
	tar -xvf $file
elif [ "${file: -7}" == ".tar.gz" -o "${file: -4}" == ".tgz" ]
then
	tar -zxvf $file
elif [ "${file: -8}" == ".tar.bz2" -o "${file: -5}" == ".tbz2" ]
then
	tar -jxvf $file
elif [ "${file: -3}" == ".gz" -o "${file: -2}" == ".z" ]
then
	gzip -dk $file
elif [ "${file: -4}" == ".bz2" ]
then
	bzip2 -dk $file
elif [ "${file: -4}" == ".zip" ]
then
	unzip $file
elif [ "${file: -4}" == ".rar" ]
then
	rar x $file
elif [ "${file: -2}" == ".Z" ]
then
	uncompress -dk $file
elif [ "${file: -3}" == ".7z" ]
then
	7z x $file
else
	echo "Unknown file format : cannot extract"
fi

