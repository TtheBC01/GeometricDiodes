#!/bin/zsh
# Stage B8, test T30 (second part): dependence of the decision number (G3, mean
# free path 700 nm, 1 THz, 0.1 V, shape-only drive) on the number of sample
# particles, from 2 to 16 million; 1 and 4 million are in results/bpic/T30.csv.
# usage: tools/pic_nseries.sh        (results/bpic/T30N.csv, results/bpic/T30N.log)
B=./build/gd_bpic; R=results/bpic; mkdir -p $R
OUT=$R/T30N.csv
run() { $B --geo funnel --lambda 700 --f 1e12 --V 0.1 --mode sym --settle 3000 --nper 10 --out $OUT "$@" | tee -a $R/T30N.log }
run --label N16e6_d300       --d 300 --N 16000000
run --label N16e6_seed2_d300 --d 300 --N 16000000 --seed 2
run --label N16e6_d30        --d 30  --N 16000000
run --label N8e6_d300        --d 300 --N 8000000
run --label N8e6_d30         --d 30  --N 8000000
run --label N2e6_d300        --d 300 --N 2000000
run --label N2e6_seed2_d300  --d 300 --N 2000000 --seed 2
run --label N2e6_d30         --d 30  --N 2000000
run --label N2e6_seed2_d30   --d 30  --N 2000000 --seed 2
echo done >> $R/T30N.log
