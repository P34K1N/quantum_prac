#!/bin/bash

n=(24 25 26 27 28)
t=(32 64 128 256 512)
filename=""

# for it in {0..2}
# do
#     for u in {3..4}
#     do
#         mpisubmit.bg -n ${t[u]} -w 00:05:00 --stdout "/dev/null" ./main -- g 28 0.01 "res/time_28_0.01_${t[u]}_${it}" "/dev/null"
#     done
# done

for it in {59..59}
do
    for u in {0..4}
    do
        mpisubmit.bg -n 32 --stdout "/dev/null" ./main -- g ${n[u]} 0.01 "/dev/null" "res/acc_${n[u]}_0.01_32_${it}"
    done
    mpisubmit.bg -n 32 --stdout "/dev/null" ./main -- g 26 0.001 "/dev/null" "res/acc_26_0.001_32_${it}"
    mpisubmit.bg -n 32 --stdout "/dev/null" ./main -- g 26 0.1 "/dev/null" "res/acc_26_0.1_32_${it}"
done

