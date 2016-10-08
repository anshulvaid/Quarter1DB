#!/usr/bin/env bash


for i in {1..12}; do
    rbf/rbftest$i 2> /dev/null
    if [ $? -ne 0 ]; then
        exit $?
    fi

    if [[ $i -eq 8 ]]; then
        rbf/rbftest${i}b 2> /dev/null
        if [ $? -ne 0 ]; then
            exit $?
        fi
    fi

done
