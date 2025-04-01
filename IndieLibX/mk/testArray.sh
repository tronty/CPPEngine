#!/bin/bash

arr=("something to search for" "a string" "test2000")
value="a string"

if [[ " ${arr[@]} " =~ " ${value} " ]]; then
    echo "# whatever you want to do when arr contains value"
else
    echo "# whatever you want to do when arr doesn't contain value"
fi

