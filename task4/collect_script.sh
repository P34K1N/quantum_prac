#!/bin/bash

cd report

n=(25 26 27)
k=(1 13)
t=(32 64 128 256 512)
filename=""
filename2=""

for i in {0..2}
do
    for j in {0..2}
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

        if [[ ${j} = 2 ]]
        then filename2="cnot_n_1.dat" 
        elif [[ ${j} = 1 ]] 
        then filename2="cnot_13_n.dat"
        else filename2="cnot_1_13.dat"
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
                
                cat ${filename} >> "temp.dat"
            done

            printf "${n[i]} ${t[s]} " >> ${filename2}
            awk '{ total += $1; count++ } END  {print total / count}' "temp.dat" >> ${filename2}

            rm temp.dat
        done
    done
done

filename2="hn.dat"

for i in {0..2}
do
    for s in {0..4}
    do
        for u in {0..2}
        do
            filename="hn_${n[i]}_${t[s]}_${u}.dat"
            cat ${filename} >> "temp.dat"
        done

        printf "${n[i]} ${t[s]} " >> ${filename2}
        awk '{ total += $1; count++ } END  {print total / count}' "temp.dat" >> ${filename2}

        rm temp.dat
    done
done

