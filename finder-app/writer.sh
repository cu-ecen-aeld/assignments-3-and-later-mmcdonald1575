#!/bin/bash

writefile="$1"
writestr="$2"

if [ $# != 2 ]
then
	echo "FAILURE: TWO PARAMETERS NOT PASSED"
	exit 1
fi

# Check if writefile argument is provided
#if [ -z "$writefile" ]; then
#  echo "Error: File path not specified."
#  exit 1
#fi

# Check if writestr argument is provided
#if [ -z "$writestr" ]; then
#  echo "Error: Text to write not specified."
#  exit 1
#fi

# Create the directory path if it doesn't exist
dirpath=$(dirname "$writefile")
mkdir -p "$dirpath"

# Attempt to write to the file
echo "$writestr" > "$writefile"

# Check if the file was created successfully
if [ $? -ne 0 ]; then
  echo "Error: Could not create the file '$writefile'."
  exit 1
fi

echo "File '$writefile' created successfully with content:"
echo "$writestr"
