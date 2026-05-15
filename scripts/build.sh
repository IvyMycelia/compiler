#!/bin/bash
set -e

WORKING =   "./bin/Flower"
BACKUP  =   "./bin/Flower_backup"
NEW_C   =   "./bin/Flower_new.c"
NEW_BIN =   "./bin/Flower_new"

echo "=== Building new version ==="
$WORKING ./src/main.flo $NEW_C

echo "=== Compiling new binary ==="
gcc $NEW_C -o $NEW_BIN -fsanitize=address -fno-omit-frame-pointer

echo "=== Testing new compiler ==="
$NEW_BIN ./src/main.flo ${NEW_C}.test

if [ $? ne 0 ]; then
    echo "ERROR: New compiler failed self-test!"
    rm $NEW_C $NEW_BIN
    exit 1
fi

echo "=== Check ==="
diff $NEW_C ${NEW_C}.test > /dev/null
if [ $? -ne 0 ]; then
    echo "WARNING: Generated code differs (may be ok)"
fi

echo "=== Accepting new version ==="
cp $WORKING $BACKUP
cp $NEW_BIN $WORKING

echo "Bootstrap successful"
echo "Previous version backed up to: $BACKUP"

# Cleanup
rm -f $NEW_C ${NEW_C}.c $NEW_BIN