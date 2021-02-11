#!/bin/bash

cd res

n=(20 24 28 30)
k=(1 13)
t=(1 2 4 8 16 32 64 128 160)
filename=""

for i in {3..3}
do
    for j in {2..2}
    do
        if [[ ${j} = 2 ]]
        then filename="res_n.dat" 
        else filename="res_${k[j]}.dat"
        fi

        if [[ ${j} = 2 ]] 
        then ord=${n[i]} 
        else ord=${k[j]}
        fi
    
        for s in {8..8}
        do
            printf "${n[i]} ${t[s]} " >> ${filename} 
            ../main ${n[i]} ${ord} ${t[s]} >> ${filename}
        done
    done
done

