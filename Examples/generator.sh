#!/bin/bash

# WARNING: Don't start it if you isn't confident!!!

BASEDIR=$(dirname $(realpath "$0"))

for file in Examples\ with\ errors/*
do
    $BASEDIR/../build/ParaCL $BASEDIR/"$file" > Answers/"${file:21}";
done
