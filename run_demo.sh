#!/bin/bash

ROOT_DIR="fighter"

if [ $# -ne 1 ]; then
    echo "Usage: $0 <DATA_DIR>"
    exit 1
fi

DATA_DIR=$1

NUM_TESTS=$(ls $ROOT_DIR/$DATA_DIR/testcase*.in | wc -l)

OUT_DIR="demo_out"

mkdir -p $ROOT_DIR/$DATA_DIR/$OUT_DIR

for id in $(seq 1 $NUM_TESTS); do 
    testcase="testcase$id.in"
    demoout="demo$id.out"
    ./fighter/demo/demo < $ROOT_DIR/$DATA_DIR/$testcase > $ROOT_DIR/$DATA_DIR/$OUT_DIR/$demoout
done

