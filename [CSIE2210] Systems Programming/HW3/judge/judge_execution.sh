#!/bin/sh

sample_1() {
    $PWD/main_judge 3 6 40 -1 -1 -1 -1 -1
}

sample_2() {
    $PWD/main_judge 3 5 3 -1 -1 -1 -1 -1 &
    child=$!
    sleep 1.5
    kill -TSTP $child
    sleep 5
    kill -TSTP $child
    wait $child
}

sample_3() {
    $PWD/main_judge 3 4 -1 2 -1 -1 -1 -1 &
    child=$!
    sleep 0.5
    exec 3>2_max_subarray
    kill -TSTP $child
    sleep 1
    echo '  -1' >&3
    sleep 5
    echo '  99' >&3
    wait $child
}

sample_4() {
    $PWD/main_judge 1 7 -1 -1 -1 -1 -1 -1
}

sample_5() {
    $PWD/main_judge 2 9 45 -1 -1 -1 -1 -1
}

sample_6() {
    $PWD/main_judge 3 6 20 -1 -1 -1 -1 -1
}

sample_7() {
    $PWD/main_judge 2 8 21 -1 -1 -1 -1 -1 &
    child=$!
    sleep 1.5
    kill -TSTP $child
    kill -TSTP $child
    kill -TSTP $child
    kill -TSTP $child
    kill -TSTP $child
    sleep 2
    kill -TSTP $child
    kill -TSTP $child
    kill -TSTP $child
    wait $child
}

sample_8() {
    $PWD/main_judge 2 7 20 6 -1 -1 -1 -1 &
    child=$!
    sleep 0.5
    exec 3>2_max_subarray
    sleep 4
    echo '   1' >&3
    sleep 3
    echo '  -2' >&3
    sleep 3
    echo '   7' >&3
    sleep 3
    echo '  -4' >&3
    sleep 3
    echo '   6' >&3
    sleep 3
    echo '  -3' >&3
    wait $child
}

sample_9() {
    $PWD/main_judge 2 8 35 6 -1 -1 -1 -1 &
    child=$!
    sleep 0.5
    exec 3>2_max_subarray
    sleep 2
    echo '   1' >&3
    sleep 2
    echo '  -2' >&3
    sleep 4
    echo '   7' >&3
    sleep 1
    echo '  -4' >&3
    sleep 1
    echo '   6' >&3
    sleep 1
    echo '  -3' >&3
    wait $child
}

sample_10() {
    $PWD/main_judge 1 12 96 5 -1 -1 -1 -1 &
    child=$!
    sleep 0.5
    exec 3>2_max_subarray
    sleep 1
    echo '  -8' >&3
    sleep 2
    echo '  -5' >&3
    sleep 1
    echo '  -9' >&3
    sleep 1
    echo '  -4' >&3
    sleep 1
    echo '  -2' >&3
    wait $child
}

sample_11() {
    $PWD/main_judge 3 8 20 2 10 24 2 2 &
    child=$!
    sleep 0.5
    exec 8>2_max_subarray
    exec 9>5_max_subarray
    exec 10>6_max_subarray
    sleep 1
    echo '  42' >&8
    echo '  -1' >&9
    sleep 2
    echo ' -99' >&10
    sleep 1
    echo ' 325' >&9
    sleep 1
    echo '  99' >&10
    sleep 1
    echo '  -2' >&8
    wait $child
}

sample_12() {
    $PWD/main_judge 1 5 17 3 2 11 2 1 &
    child=$!
    sleep 0.5
    exec 8>2_max_subarray
    exec 9>5_max_subarray
    exec 10>6_max_subarray
    sleep 1.5
    echo ' 999' >&8
    echo '   0' >&9
    sleep 1.5
    echo ' 999' >&8
    sleep 1
    echo '  -1' >&10
    sleep 2
    echo ' 999' >&8
    echo '   0' >&9
    sleep 1
    wait $child
}

sample_13() {
    $PWD/main_judge 1 4 23 7 4 5 7 7 &
    child=$!
    sleep 0.5
    exec 8>2_max_subarray
    exec 9>5_max_subarray
    exec 10>6_max_subarray
    sleep 1.5
    echo '   1' >&8
    echo '  99' >&10
    echo '   2' >&9
    sleep 1.5
    echo '  -1' >&9
    echo ' -32' >&10
    echo '   3' >&9
    echo '   3' >&8
    sleep 1.5
    echo '  -2' >&9
    sleep 2
    echo '   5' >&10
    echo '  12' >&10
    echo '   2' >&8
    echo '   4' >&9
    sleep 1.5
    echo '   4' >&8
    echo '  -5' >&9
    sleep 0.5
    echo ' -33' >&10
    sleep 1
    echo '   5' >&8
    echo '  -1' >&10
    sleep 1.5
    echo '   6' >&8
    echo '  99' >&8
    sleep 2.5
    echo '  14' >&10
    sleep 1
    echo '   2' >&9
    wait $child
}

print_help() {
    echo "usage: $0 [subtask]"
}

main() {

    case "$1" in
        1)
            sample_1
            ;;
        2)
            sample_2
            ;;
        3)
            sample_3
            ;;
        4)
            sample_4
            ;;
        5)
            sample_5
            ;;
        6)
            sample_6
            ;;
        7)
            sample_7
            ;;
        8)
            sample_8
            ;;
        9)
            sample_9
            ;;
        10)
            sample_10
            ;;
        11)
            sample_11
            ;;
        12)
            sample_12
            ;;
        13)
            sample_13
            ;;
        *)
            print_help
            ;;
    esac
}

cp /home/student/10/b10902138/spjudge/main_judge.c .
gcc -o main_judge main_judge.c scheduler.c threads.c >& error.log
if [ $? -ne 0 ]
then
    rm main_judge.c 
    rm error.log
    exit 1
fi

main "$1"
rm main_judge