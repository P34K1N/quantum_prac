#!/bin/bash

t=(1 2 4 8 16)

for n in {1..5}
do
    for k in {1..16}
    do
        for m in {0..4}
        do
            mpirun -n ${t[m]} ./main f ${k} test/test${n} test/res${n}_${t[m]}
            if [[ ${m} = 0 ]]
            then m=m
            else ./test/vec_comp test/res${n}_1 test/res${n}_${t[m]}
            fi
        done 
        rm test/res*
    done
done

