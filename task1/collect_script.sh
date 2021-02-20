#!/bin/bash

rm -f res_*.dat

cd res

n=(20 24 28 30)
k=(1 13)
t=(1 2 4 8 16 32 64 128 160)
filename=""
filename2=""
filename3=""

for i in {0..3}
do
    for j in {0..2}
    do
        if [[ ${j} = 2 ]]
        then filename3="../res_n.dat" 
        else filename3="../res_${k[j]}.dat"
        fi

        if [[ ${j} = 2 ]] 
        then ord=${n[i]} 
        else ord=${k[j]}
        fi
    
        for s in {0..8}
        do
            if [[ ${j} = 2 ]]
            then filename2="res_n_${n[i]}_${t[s]}.dat" 
            else filename2="res_${k[j]}_${n[i]}_${t[s]}.dat"
            fi

            for u in {0..2}
            do
                if [[ ${j} = 2 ]]
                then filename="res_n_${n[i]}_${t[s]}_${u}.dat" 
                else filename="res_${k[j]}_${n[i]}_${t[s]}_${u}.dat"
                fi
                
                cat ${filename} >> ${filename2}
            done
            printf "${n[i]} ${t[s]} " >> ${filename3}
            awk '{ total += $1; count++ } END  {print total / count}' ${filename2} >> ${filename3}
        done
    done
done

