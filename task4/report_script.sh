#!/bin/bash

cd report

n=(25 26 27)
k=(1 13)
t=(32 64 128 256 512)
filename=""

for i in {0..2}
do
    for j in {2..2}
    do
        if [[ ${j} = 2 ]] 
        then 
            ord1=${n[i]} 
            ord2=1
        elif [[ ${j} = 1 ]]
        then
            ord1=13
            ord2=${n[i]}
        else 
            ord1=1
            ord2=13
        fi
    
        for s in {0..4}
        do
            for u in {0..2}
            do
                if [[ ${j} = 2 ]]
                then filename="cnot_${n[i]}_n_1_${t[s]}_${u}.dat" 
                elif [[ ${j} = 1 ]] 
                then filename="cnot_${n[i]}_13_n_${t[s]}_${u}.dat"
                else filename="cnot_${n[i]}_1_13_${t[s]}_${u}.dat"
                fi
                
                mpisubmit.bg -n ${t[s]} -w 00:05:00 --stdout "/dev/null" ./sample_CNOT -- ${n[i]} ${ord1} ${ord2} ${filename}
            done
        done
    done
done

# for i in {0..2}
# do
#     for s in {0..4}
#     do
#         for u in {0..2}
#         do
#             filename="hn_${n[i]}_${t[s]}_${u}.dat"

#             mpisubmit.bg -n ${t[s]} -w 00:05:00 --stdout "/dev/null" ./sample_Hn -- ${n[i]} ${filename}
#         done
#     done
# done

