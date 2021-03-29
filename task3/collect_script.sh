#!/bin/bash

n=(24 25 26 27 28)
t=(32 64 128 256 512)
filename=""

# for u in {0..4}
# do
#     for it in {0..2}
#     do
#         cat "res/time_28_0.01_${t[u]}_${it}" >> "res/temp.txt"
#     done
#     printf "${t[u]} " >> "res/time.txt"
#     awk '{ total += $1; count++ } END  {print total / count}' "res/temp.txt" >> "res/time.txt"
#     rm "res/temp.txt"
# done

# for it in {0..59}
# do
#     for u in {0..4}
#     do
#         cat "res/acc_${n[u]}_0.01_32_${it}" >> "res/acc_${n[u]}_0.01.txt"
#     done
#     cat "res/acc_26_0.001_32_${it}" >> "res/acc_26_0.001.txt"
#     cat "res/acc_26_0.1_32_${it}" >> "res/acc_26_0.1.txt"
# done

for u in {0..4}
do
    printf "${n[u]} 0.01 " >> "res/acc.txt"
    awk '{ total += $1; count++ } END  {print total / count}' "res/acc_${n[u]}_0.01.txt" >> "res/acc.txt"
done
printf "26 0.1 " >> "res/acc.txt"
awk '{ total += $1; count++ } END  {print total / count}' "res/acc_26_0.1.txt" >> "res/acc.txt"
printf "26 0.001 " >> "res/acc.txt"
awk '{ total += $1; count++ } END  {print total / count}' "res/acc_26_0.001.txt" >> "res/acc.txt"

