#!/bin/bash

# Script to run all Kyber KAT tests (Known Answer Tests)
# This runs the NIST KAT test generation for Kyber512, Kyber768, and Kyber1024

echo "================================================"
echo "Running Kyber KAT Tests"
echo "================================================"

cd "$(dirname "$0")/nistkat"

echo ""
echo "========================================"
echo "Running Kyber512 KAT Test..."
echo "========================================"
./PQCgenKAT_kem512
if [ $? -eq 0 ]; then
    echo "✓ Kyber512 KAT test completed successfully"
    echo "  Generated: PQCkemKAT_1632.req and PQCkemKAT_1632.rsp"
else
    echo "✗ Kyber512 KAT test failed"
fi

echo ""
echo "========================================"
echo "Running Kyber768 KAT Test..."
echo "========================================"
./PQCgenKAT_kem768
if [ $? -eq 0 ]; then
    echo "✓ Kyber768 KAT test completed successfully"
    echo "  Generated: PQCkemKAT_2400.req and PQCkemKAT_2400.rsp"
else
    echo "✗ Kyber768 KAT test failed"
fi

echo ""
echo "========================================"
echo "Running Kyber1024 KAT Test..."
echo "========================================"
./PQCgenKAT_kem1024
if [ $? -eq 0 ]; then
    echo "✓ Kyber1024 KAT test completed successfully"
    echo "  Generated: PQCkemKAT_3168.req and PQCkemKAT_3168.rsp"
else
    echo "✗ Kyber1024 KAT test failed"
fi

echo ""
echo "================================================"
echo "All KAT tests completed!"
echo "================================================"
echo ""
echo "Generated files:"
ls -lh *.req *.rsp 2>/dev/null || echo "No KAT files found"

echo ""
echo "To view the generated test vectors, check the .rsp files:"
echo "  - PQCkemKAT_1632.rsp  (Kyber512)"
echo "  - PQCkemKAT_2400.rsp  (Kyber768)"
echo "  - PQCkemKAT_3168.rsp  (Kyber1024)"
