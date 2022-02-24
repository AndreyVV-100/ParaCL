#!/bin/bash

BASEDIR=$(dirname $(realpath "$0"))

for file in Examples\ with\ errors/*
do
    answer=$($BASEDIR/../build/ParaCL $BASEDIR/"$file")
    right_answer=$(cat $BASEDIR/Answers/"${file:21}")

    if [ "$answer" = "$right_answer" ]
    then
        echo "Test ${file:21} Passed."
    else
        echo "Test ${file:21} Failed."
    fi
done

# BASEDIR=$(dirname $(realpath "$0"))
# NUM_TESTS=$((17))

# for ((i_test=1; i_test <= NUM_TESTS; i_test++))
# do
#     answer=$($BASEDIR/../a.out < $BASEDIR/Tasks/$i_test.task)
#     right_answer=$(cat $BASEDIR/Answers/$i_test.answer)
#     if [ "$answer" = "$right_answer" ]
#     then
#         echo "Test $i_test Passed."
#     else
#         echo "Test $i_test Failed."
#     fi
# done
