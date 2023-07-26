#!/bin/bash

filesdir=$1
searchstr=$2
if [ $# != 2 ]
then
	echo "FAILURE: TWO PARAMETERS NOT PASSED"
	exit 1
fi

if [ ! -d "${filesdir}" ]
then
	echo "FAILURE: DIRECTORY DOES NOT EXIST"
	exit 1
fi

#if [ -z "${filesdir}" ]
#then
#	echo "INVALID INPUT PARAMETER"
#	exit 1
#fi

#if [ -z "${searchstr}" ]
#then
#	echo "INVALID INPUT PARAMETER"
#	exit 1
#fi


#numFiles=$(grep -r $filesdir | wc -l)
#numFiles=$(find "${filesdire}" -type f | wc -l)
numFiles=$(ls ${filesdir} | wc -l)
linesFound=$(grep -r "${searchstr}" "${filesdir}" | wc -l)

echo "# of Files: ${numFiles} | # of Matching Lines: ${linesFound}"
exit 0
