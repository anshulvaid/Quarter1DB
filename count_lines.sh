#!/usr/bin/env bash

files=(rbf/pfm.cc rbf/pfm.h rbf/rbfm.cc rbf/rbfm.h)
wc -l ${files[*]} | tail -n1 | awk '{print $1}'
