#!/bin/bash

ROOT_DIR="fighter"

if [ $# -ne 1 ]; then
    echo "Usage: $0 <DATA_DIR>"
    exit 1
fi

DATA_DIR=$1

NUM_TESTS=$(ls $ROOT_DIR/$DATA_DIR/testcase*.in | wc -l)

DEMO_OUT="demo_out"
GAMER_OUT="gamer_out"
CHEKER_OUT="checker_out"

mkdir -p $ROOT_DIR/$DATA_DIR/$CHEKER_OUT

echo "Running checker on demo ..."
for id in $(seq 1 $NUM_TESTS); do 
    testcase="testcase$id.in"
    demoout="demo$id.out"
    chekerout="demo$id.log"
    echo "Running test $id..."
    echo "Demo:"
    ./fighter/checker/checker $ROOT_DIR/$DATA_DIR/$testcase $ROOT_DIR/$DATA_DIR/$DEMO_OUT/$demoout > $ROOT_DIR/$DATA_DIR/$CHEKER_OUT/$chekerout
    tail -n 1 $ROOT_DIR/$DATA_DIR/$CHEKER_OUT/$chekerout

    gamerout="gamer$id.out"
    chekerout="gamer$id.log"
    echo "gamer:"
    ./fighter/checker/checker $ROOT_DIR/$DATA_DIR/$testcase $ROOT_DIR/$DATA_DIR/$GAMER_OUT/$gamerout > $ROOT_DIR/$DATA_DIR/$CHEKER_OUT/$chekerout
    tail -n 1 $ROOT_DIR/$DATA_DIR/$CHEKER_OUT/$chekerout
    echo
done