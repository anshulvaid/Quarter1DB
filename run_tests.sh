#!/usr/bin/env bash


for i in {1..12}; do
    rbf/rbftest$i || exit $?

    if [[ $i -eq 8 ]]; then
        rbf/rbftest${i}b || exit $?
    fi

done


# Run private tests
rbf/rbftest_p0 || exit $?
rbf/rbftest_p1 || exit $?
rbf/rbftest_p1b || exit $?
rbf/rbftest_p1c || exit $?
rbf/rbftest_p2 || exit $?
rbf/rbftest_p2b || exit $?
rbf/rbftest_p3 || exit $?
rbf/rbftest_p4 || exit $?