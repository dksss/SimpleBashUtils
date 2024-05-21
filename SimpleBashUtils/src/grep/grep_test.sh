#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0

test_files=("test_files/1.txt" "test_files/2.txt" "test_files/3.txt" "test_files/4.txt" "test_files/5.txt" "test_files/empty.txt")

templates=("abc" "123" "banana" "verter" "rat")

pat_file="test_files/pat.txt"

# 1 flag
for file in "${test_files[@]}"
do
    for flag in "e" "i" "v" "c" "l" "n" "h" "s" "o"
    do
        for tem in "${templates[@]}"
        do
            echo "$flag $tem $file:"
            ./my_grep "-$flag" "$tem" "$file" > my_grep_output.txt
            grep "-$flag" "$tem" "$file" > grep_output.txt
            if diff -q my_grep_output.txt grep_output.txt >/dev/null 2>&1;
            then
                let "COUNTER++"
                let "SUCCESS++"
                echo "$COUNTER - Success"
            else 
                let "COUNTER++"
                let "FAIL++"
                echo "$COUNTER - Fail"
            fi
            rm my_grep_output.txt grep_output.txt
        done
    done
done

# 2 flags
for flag1 in "i" "v" "c" "l" "n" "h" "s"
do
    for flag2 in "e" "i" "v" "c" "l" "n" "h" "s"
    do
        if [[ "$flag1" != "$flag2" ]]
        then
            for tem in "${templates[@]}"
            do
                echo "-$flag1 -$flag2 $tem ${test_files[0]} ${test_files[1]} ${test_files[2]}:"
                ./my_grep "-$flag1" "-$flag2" "$tem" "${test_files[0]}" "${test_files[1]}" "${test_files[2]}" > my_grep_output.txt
                grep "-$flag1" "-$flag2" "$tem" "${test_files[0]}" "${test_files[1]}" "${test_files[2]}" > grep_output.txt
                if diff -q my_grep_output.txt grep_output.txt >/dev/null 2>&1;
                then
                    let "COUNTER++"
                    let "SUCCESS++"
                    echo "$COUNTER - Success"
                else 
                    let "COUNTER++"
                    let "FAIL++"
                    echo "$COUNTER - Fail"
                    echo "Your output:\n"
                    cat my_grep_output.txt
                    echo "\n\nGrep output:\n"
                    cat grep_output.txt
                    echo "\n"
                fi
                rm my_grep_output.txt grep_output.txt
            done
        fi
    done
done

echo "###########################"
echo "RESULTS:"
echo "Success: $SUCCESS/$COUNTER"
echo "Fails: $FAIL/$COUNTER"
echo "###########################"