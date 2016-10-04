#!/usr/bin/env bash


for i in {1..12}; do
    ./rbftest$i
    if [ $? -ne 0 ]; then
        exit $?
    fi
done

./rbftest8b
