#!/bin/bash
# Verify drill solutions

if [ "$1" = "" ]; then
    drill="all"
else
    drill="$1"
fi

passed=0
failed=0
skipped=0

test_drill() {
    local file="$1"
    local expected="$2"
    local input="$3"
    
    local drillName="${file%.cpp}"
    
    if [ ! -f "$file" ]; then
        echo "  ⊘ $drillName - SKIPPED (file not found)"
        return 2
    fi
    
    # Compile
    local exeName="$drillName.exe"
    if ! g++ -g "$file" -o "$exeName" 2>&1; then
        echo "  ✗ $drillName - COMPILE ERROR"
        return 1
    fi
    
    # Run
    if [ -n "$input" ]; then
        output=$(echo "$input" | "./$exeName" 2>&1)
    else
        output=$("./$exeName" 2>&1)
    fi
    
    # Trim whitespace
    output=$(echo "$output" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
    expected=$(echo "$expected" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')
    
    if [ "$output" = "$expected" ]; then
        echo "  ✓ $drillName - PASSED"
        return 0
    else
        echo "  ✗ $drillName - FAILED"
        echo "    Expected: $expected"
        echo "    Got:      $output"
        return 1
    fi
}

echo ""
echo "=== C++ Drill Verification ==="
echo ""

if [ "$drill" = "all" ]; then
    # Test all drills
    test_drill "drill01.cpp" "Hello World!" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill02.cpp" "My name is Alice and I am 25 years old." ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill03.cpp" "What is your name? Hello, Bob!" "Bob"
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill04.cpp" "Sum: 13
Difference: 7
Product: 30
Quotient: 3" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill05.cpp" "Pass" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill06.cpp" "1
2
3
4
5" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill07.cpp" "5
4
3
2
1
Blast off!" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill08.cpp" "2
4
6
8
10" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill09.cpp" "42" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
    
    test_drill "drill10.cpp" "Hello, World!" ""
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
else
    # Test single drill
    drillFile=$(printf "drill%02d.cpp" "$drill")
    
    case "$drill" in
        1|01) test_drill "$drillFile" "Hello World!" "" ;;
        2|02) test_drill "$drillFile" "My name is Alice and I am 25 years old." "" ;;
        3|03) test_drill "$drillFile" "What is your name? Hello, Bob!" "Bob" ;;
        4|04) test_drill "$drillFile" "Sum: 13
Difference: 7
Product: 30
Quotient: 3" "" ;;
        5|05) test_drill "$drillFile" "Pass" "" ;;
        6|06) test_drill "$drillFile" "1
2
3
4
5" "" ;;
        7|07) test_drill "$drillFile" "5
4
3
2
1
Blast off!" "" ;;
        8|08) test_drill "$drillFile" "2
4
6
8
10" "" ;;
        9|09) test_drill "$drillFile" "42" "" ;;
        10) test_drill "$drillFile" "Hello, World!" "" ;;
        *) echo "Drill not found: $drillFile"; exit 1 ;;
    esac
    
    [ $? -eq 0 ] && ((passed++)) || { [ $? -eq 2 ] && ((skipped++)) || ((failed++)); }
fi

# Summary
echo ""
echo "=== Summary ==="
echo "  Passed:  $passed"
echo "  Failed:  $failed"
echo "  Skipped: $skipped"
echo ""

if [ $failed -eq 0 ] && [ $passed -gt 0 ]; then
    echo "🎉 All drills passed!"
fi
