#!/bin/bash

# Script to run all Kyber test programs
# This runs the test_kyber programs which verify key exchange functionality

echo "================================================"
echo "Running Kyber Test Programs (with verification)"
echo "================================================"

cd "$(dirname "$0")/test"

echo ""
echo "========================================"
echo "Running Kyber512 Test..."
echo "========================================"
./test_kyber512
if [ $? -eq 0 ]; then
    echo "✓ Kyber512 test passed"
else
    echo "✗ Kyber512 test failed"
    exit 1
fi

echo ""
echo "========================================"
echo "Running Kyber768 Test..."
echo "========================================"
./test_kyber768
if [ $? -eq 0 ]; then
    echo "✓ Kyber768 test passed"
else
    echo "✗ Kyber768 test failed"
    exit 1
fi

echo ""
echo "========================================"
echo "Running Kyber1024 Test..."
echo "========================================"
./test_kyber1024
if [ $? -eq 0 ]; then
    echo "✓ Kyber1024 test passed"
else
    echo "✗ Kyber1024 test failed"
    exit 1
fi

echo ""
echo "================================================"
echo "✓ All Kyber tests passed successfully!"
echo "================================================"
