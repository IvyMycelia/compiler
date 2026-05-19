#!/bin/bash
#set -e

WORKING="./bin/Flower"
BACKUP="./bin/Flower_backup"
NEW_C="./bin/Flower_new.c"
NEW_BIN="./bin/Flower_new_bin"
TEST_C="./bin/Flower_test.c"

echo "=== Building new version ==="
$WORKING ./src/main.flo ./bin/Flower_new

echo "=== Compiling new binary ==="
gcc $NEW_C -o $NEW_BIN -fsanitize=address -fno-omit-frame-pointer

echo "=== Testing new compiler ==="
$NEW_BIN ./src/main.flo ./bin/Flower_test

if [ $? -ne 0 ]; then
    echo "ERROR: New compiler failed self-test!"
    rm -f $NEW_BIN
    exit 1
fi

echo "=== Idempotency check ==="
if ! diff $NEW_C $TEST_C > /dev/null 2>&1; then
    echo "ERROR: Generated code differs between runs!"
    echo "This indicates non-deterministic compilation."
    rm -f $NEW_BIN
    exit 1
fi

echo "=== Accepting new version ==="
cp $WORKING $BACKUP
cp $NEW_BIN $WORKING

echo "Bootstrap successful"
echo "Previous version backed up to: $BACKUP"

# Cleanup
cp $NEW_C ./bin/Flower.c
rm -f $NEW_C $NEW_BIN $TEST_C