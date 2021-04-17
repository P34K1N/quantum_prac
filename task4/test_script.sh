#!/bin/bash

t=(1 2 4 8 16)
e=(0 0.001 0.01 0.1 1)
q=(1 7 16 1)

cd test

g++ -o check_H check_H.cpp -O2
g++ -o check_NOT check_NOT.cpp -O2
g++ -o check_ROT check_ROT.cpp -O2
g++ -o check_Hn check_Hn.cpp -O2
g++ -o check_CNOT check_CNOT.cpp -O2
g++ -o check_CROT check_CROT.cpp -O2

mpic++ -o sample_H sample_H.cpp -O2
mpic++ -o sample_NOT sample_NOT.cpp -O2
mpic++ -o sample_ROT sample_ROT.cpp -O2
mpic++ -o sample_Hn sample_Hn.cpp -O2
mpic++ -o sample_CNOT sample_CNOT.cpp -O2
mpic++ -o sample_CROT sample_CROT.cpp -O2

# for n in {1..5}
# do
#     for k in {0..2}
#     do
#         ./check_NOT "test${n}" ${q[k]} "ground_truth"
#         for m in {0..4}
#         do
#             mpirun -n ${t[m]} ./sample_NOT "test${n}" ${q[k]} "result"
#             ./vec_comp "ground_truth" "result"
#         done 
#     done
# done

# for n in {1..5}
# do
#     for k in {0..2}
#     do
#         ./check_H "test${n}" ${q[k]} "ground_truth"
#         for m in {0..4}
#         do
#             mpirun -n ${t[m]} ./sample_H "test${n}" ${q[k]} "result"
#             ./vec_comp "ground_truth" "result"
#         done 
#     done
# done

# for n in {1..5}
# do
#     for k in {0..2}
#     do
#         ./check_ROT "test${n}" ${q[k]} "ground_truth"
#         for m in {0..4}
#         do
#             mpirun -n ${t[m]} ./sample_ROT "test${n}" ${q[k]} "result"
#             ./vec_comp "ground_truth" "result"
#         done 
#     done
# done

# for n in {1..5}
# do
#     ./check_Hn "test${n}" "ground_truth"
#     for m in {0..4}
#     do
#         mpirun -n ${t[m]} ./sample_Hn "test${n}" "result"
#         ./vec_comp "ground_truth" "result"
#     done 
# done

# for n in {1..5}
# do
#     for k in {0..2}
#     do
#         ./check_CNOT "test${n}" ${q[k]} ${q[k+1]} "ground_truth"
#         for m in {0..4}
#         do
#             mpirun -n ${t[m]} ./sample_CNOT "test${n}" ${q[k]} ${q[k+1]} "result"
#             ./vec_comp "ground_truth" "result"
#         done 
#     done
# done

for n in {1..5}
do
    for k in {0..2}
    do
        ./check_CROT "test${n}" ${q[k]} ${q[k+1]} "ground_truth"
        for m in {0..4}
        do
            mpirun -n ${t[m]} ./sample_CROT "test${n}" ${q[k]} ${q[k+1]} "result"
            ./vec_comp "ground_truth" "result"
        done 
    done
done

rm "ground_truth"
rm "result"

rm check_H
rm check_CNOT
rm check_NOT
rm check_ROT
rm check_CROT
rm check_Hn

rm sample_H
rm sample_CNOT
rm sample_NOT
rm sample_ROT
rm sample_CROT
rm sample_Hn
