#!/bin/bash
# Simple test script for the compiler

COMPILER="build/bin/compiler"
TESTS_DIR="tests"
OUTPUT_DIR="build/test_outputs"

echo "=== Educational Compiler Test Suite ==="
echo "Compiler: $COMPILER"
echo "Tests directory: $TESTS_DIR"
echo ""

# Create output directory
mkdir -p "$OUTPUT_DIR"

TESTS_PASSED=0
TESTS_FAILED=0
FAILED_TESTS=()

# Test all .prog files
for test_file in "$TESTS_DIR"/*.prog; do
    if [ -f "$test_file" ]; then
        test_name=$(basename "$test_file")
        output_file="$OUTPUT_DIR/${test_name}.s"
        
        echo -n "Running test: $test_name ... "
        
        if $COMPILER "$test_file" "$output_file" >/dev/null 2>&1; then
            if [ -f "$output_file" ]; then
                echo "✅ PASS"
                ((TESTS_PASSED++))
            else
                echo "❌ FAIL (no output)"
                ((TESTS_FAILED++))
                FAILED_TESTS+=("$test_name")
            fi
        else
            echo "❌ FAIL (compiler error)"
            ((TESTS_FAILED++))
            FAILED_TESTS+=("$test_name")
        fi
    fi
done

echo ""
echo "=== Test Results ==="
echo "Tests passed: $TESTS_PASSED"
echo "Tests failed: $TESTS_FAILED"
echo "Total tests: $((TESTS_PASSED + TESTS_FAILED))"

if [ $TESTS_FAILED -gt 0 ]; then
    echo ""
    echo "Failed tests:"
    for failed_test in "${FAILED_TESTS[@]}"; do
        echo "  • $failed_test"
    done
    exit 1
else
    echo ""
    echo "🎉 All tests passed!"
fi
