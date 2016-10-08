#!/usr/bin/env bash


for i in {1..12}; do
    ./rbftest$i 2> /dev/null
    if [ $? -ne 0 ]; then
        exit $?
    fi

    if [[ $i -eq 8 ]]; then
        ./rbftest${i}b 2> /dev/null
    fi

done
