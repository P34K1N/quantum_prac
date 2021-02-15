#!/bin/bash

n=(25 26 27)
k=(1 13)
t=(1 2 4 8 16)
filename=""
filename2=""

for i in {0..2}
do
    for j in {0..2}
    do
        if [[ ${j} = 2 ]]
        then filename2="res_n.dat" 
        else filename2="res_${k[j]}.dat"
        fi

        if [[ ${j} = 2 ]] 
        then ord=${n[i]} 
        else ord=${k[j]}
        fi
    
        for s in {0..4}
        do
            if [[ ${j} = 2 ]]
            then filename="res/res_${n[i]}_n_${t[s]}.dat" 
            else filename="res/res_${n[i]}_${k[j]}_${t[s]}.dat"
            fi
            
            printf "${n[i]} ${t[s]} " >> ${filename2}
            cat ${filename} >> ${filename2}
        done
    done
done

