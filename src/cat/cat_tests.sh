#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0

test_files=("test_files/1.txt" "test_files/2.txt" "test_files/3.txt" "test_files/4.txt" "test_files/5.txt")

flags=("b" "e" "n" "s" "t")

# 1 file 1 flag
for file in "${test_files[@]}"
do
    for flag1 in "${flags[@]}"
    do
        echo "$flag1 $file1:"
        ./my_cat "-$flag1" "$file" > my_cat_output.txt
        cat "-$flag1" "$file" > cat_output.txt
        if diff -q my_cat_output.txt cat_output.txt > /dev/null 2>&1;
        then
            let "COUNTER++"
            let "SUCCESS++"
            echo "$COUNTER - Success"
        else
            let "COUNTER++"
            let "FAIL++"
            echo "$COUNTER - Fail"
        fi
        rm my_cat_output.txt cat_output.txt
    done
done

# 2 file 2 flags
for file1 in "${test_files[@]}"
do
    for file2 in "${test_files[@]}"
    do
        for flag1 in "${flags[@]}"
        do
            for flag2 in "${flags[@]}"
            do
                echo "$flag1 $flag2 $file1:"
                ./my_cat "-$flag1" "-$flag2" "$file1" "$file2" > my_cat_output.txt
                cat "-$flag1" "-$flag2" "$file1" "$file2" > cat_output.txt
                if diff -q my_cat_output.txt cat_output.txt > /dev/null 2>&1;
                then
                    let "COUNTER++"
                    let "SUCCESS++"
                    echo "$COUNTER - Success"
                else
                    let "COUNTER++"
                    let "FAIL++"
                    echo "$COUNTER - Fail"
                fi
                rm my_cat_output.txt cat_output.txt
            done
        done
    done
done

# 2 files 3 flags
for file1 in "${test_files[@]}"
do
    for file2 in "${test_files[@]}"
    do
        for flag1 in "${flags[@]}"
        do
            for flag2 in "${flags[@]}"
            do
                for flag3 in "${flags[@]}"
                do
                    echo "$flag1 $flag2 $flag3 $file1:"
                    ./my_cat "-$flag1" "-$flag2" "-$flag3" "$file1" "$file2" > my_cat_output.txt
                    cat "-$flag1" "-$flag2" "-$flag3" "$file1" "$file2" > cat_output.txt
                    if diff -q my_cat_output.txt cat_output.txt > /dev/null 2>&1;
                    then
                        let "COUNTER++"
                        let "SUCCESS++"
                        echo "$COUNTER - Success"
                    else
                        let "COUNTER++"
                        let "FAIL++"
                        echo "$COUNTER - Fail"
                    fi
                    rm my_cat_output.txt cat_output.txt
                done
            done
        done
    done
done

echo "RESULTS:"
echo "Success: $SUCCESS/$COUNTER"
echo "Fails: $FAIL/$COUNTER"