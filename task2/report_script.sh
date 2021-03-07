#!/bin/bash

n=(25 26 27)
k=(1 13)
t=(1 2 4 8 16)
filename=""

for i in {0..2}
do
    for j in {0..2}
    do
        if [[ ${j} = 2 ]] 
        then ord=${n[i]} 
        else ord=${k[j]}
        fi
    
        for s in {0..4}
        do
            for u in {0..2}
            do
                if [[ ${j} = 2 ]]
                then filename="res/res_${n[i]}_n_${t[s]}_${u}.dat" 
                else filename="res/res_${n[i]}_${k[j]}_${t[s]}_${u}.dat"
                fi
                
                mpisubmit.pl -p ${t[s]} ./main -- g ${n[i]} ${ord} ${filename}
            done
        done
    done
done

