#!/bin/bash

make clean
make

max_args=3;
test_number=1
test_passed=0
test_failed=0
failed_tests=()
combinations=()

one="-"
double="--"

valid_file="Makefile"
directory="../grep/"
not_exist="non.txt"
restricted_file="restricted_file"

short_flags=("-n" "-b" "-s" "-t" "-e" "-v" "-E" "-T")
long_flags=("--number" "--number-nonblank" "--squeeze-blank")
illegal_flags=("-f" "--flag")


all_arguments=("$one" "$double" "$valid_file" "$directory"  "$not_exist"  "$restricted_file" "${short_flags[@]}" "${long_flags[@]}" "${illegal_flags[@]}" )

generate_combinations() {
for arg1 in "${all_arguments[@]}"; do
    combinations+=("$arg1")
    for arg2 in "${all_arguments[@]}"; do
        if (( max_args == 2 )); then
            combinations+=("$arg1 $arg2")
        else
            for arg3 in "${all_arguments[@]}"; do
                if (( max_args == 3 )); then
                    combinations+=("$arg1 $arg2 $arg3")
                else
                    for arg4 in "${all_arguments[@]}"; do
                        combinations+=("$arg1 $arg2 $arg3 $arg4")
                    done
                fi
            done
        fi
    done
done
}

run_test(){
    local output="$1"        
    local expected="$2"      
    local test_number="$3"   
    local test_name="$4"     

    if [ "$output" == "$expected" ]; then
        ((test_passed++))
        echo $test_number $test_name "PASSED"
        
    else
        ((test_failed++))
        failed_tests+=("$test_number: $test_name")   
    fi
}

test_cat(){
generate_combinations
for arg in "${combinations[@]}"; do
    output=$(echo "Hello" | ./s21_cat $arg 2>/dev/null)
    expected=$(echo "Hello" | cat $arg 2>/dev/null)

    description="Argument: '$arg'"

    run_test "$output" "$expected" $test_number "$description"
    test_number=$((test_number + 1))
done

}

test_cat

echo "================ TEST RESULTS ================"
echo "Total tests: $((test_number - 1))"
echo "Passed tests: $test_passed"
echo "Failed tests: $test_failed"

if ((test_failed > 0)); then
    echo "Failed test details:"
    for test in "${failed_tests[@]}"; do
        echo "  $test"
    done
fi

make clean
