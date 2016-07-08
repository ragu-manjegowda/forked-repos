#!/bin/bash

for i in $(seq 1 4); do
    echo $i
    echo ====
    echo test_vec_read $i
    time ./test_vec_read $i
    echo test_cow_read $i
    time ./test_cow_read $i
    echo test_copy_read $i
    time ./test_copy_read $i

    echo test_vec_write $i
    time ./test_vec_write $i
    echo test_cow_write $i
    time ./test_cow_write $i
done

