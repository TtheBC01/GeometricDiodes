#!/bin/zsh
# Stage B8, test T30: convergence of the decision number (G3, mean free path
# 700 nm, 1 THz, 0.1 V, shape-only drive) in grid, time step, particle number,
# low-pass filter, contact representation and seed.
# usage: tools/pic_convergence.sh        (results/bpic/T30.csv, results/bpic/T30.log)
B=./build/gd_bpic; R=results/bpic; mkdir -p $R
OUT=$R/T30.csv
run() { $B --geo funnel --lambda 700 --f 1e12 --V 0.1 --mode sym --settle 3000 --nper 10 --out $OUT "$@" | tee -a $R/T30.log }
for d in 300 30; do
  run --label base_d$d    --d $d --N 4000000
  run --label seed2_d$d   --d $d --N 4000000 --seed 2
  run --label N1e6_d$d    --d $d --N 1000000
  run --label h2.5_d$d    --d $d --N 4000000 --h 2.5
  run --label dt_d$d      --d $d --N 4000000 --dt 1.25
  run --label filt0.75_d$d --d $d --N 4000000 --filter 0.75
  run --label filt3_d$d   --d $d --N 4000000 --filter 3
  run --label clen100_d$d --d $d --N 4000000 --clen 100
  run --label clen400_d$d --d $d --N 4000000 --clen 400
  run --label cblock4_d$d --d $d --N 4000000 --cblock 4
done
