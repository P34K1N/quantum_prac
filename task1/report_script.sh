#!/bin/bash

g++ -o command_file_gen command_file_gen.cpp

cd res

n=(20 24 28 30)
k=(1 13)
t=(1 2 4 8 16 32 64 128 160)
filename=""

for i in {1..3}
do
    for j in {0..2}
    do
        if [[ ${j} = 2 ]] 
        then ord=${n[i]} 
        else ord=${k[j]}
        fi
    
        for s in {0..8}
        do
            for u in {0..2}
            do
                if [[ ${j} = 2 ]]
                then filename="res_n_${n[i]}_${t[s]}_${u}.dat" 
                else filename="res_${k[j]}_${n[i]}_${t[s]}_${u}.dat"
                fi
                
                ../command_file_gen ${n[i]} ${ord} ${t[s]} ${filename}
                bsub < bsub_comm.lsf
            done
        done
    done
done

