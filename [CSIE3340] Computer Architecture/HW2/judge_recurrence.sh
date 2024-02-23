#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"
current_dir="$(pwd)"
ans=("0" "1" "2" "5" "12" "29" "70" "169" "408" "985" "2378" "5741" "13860" "33461" "80782" "195025")

if [ ! -f "$current_dir/$filename" ]; then
    echo "File not found: $filename"
    exit 1
fi

for i in {0..15}; do
    echo $i | jupiter "$current_dir/$filename" > "/home/student/10/b10902138/Public/CAjudge/HW2/output.txt"
    read -r output < "/home/student/10/b10902138/Public/CAjudge/HW2/output.txt"
    if [ "$output" = "${ans[i]}" ]; then
        echo -e "\e[30mInput = $i \e[32mPassed\e[0m"
    else
        echo -e "\e[30mInput = $i \e[31mShould be ${ans[i]} but got $output\e[0m"
    fi
done