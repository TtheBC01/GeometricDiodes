#!/bin/zsh
# Stage B8 follow-up checks on the core set: the two 3 THz runs under the 30 nm gate with the drain
# tied to the gate, where the two plane estimators of the direct current differed by more than the
# batch error after a 3 ps settle. Repeated with a 10 ps settle. -> results/bpic/checks.csv
set -e
cd "$(dirname "$0")/.."
B=./build/gd_bpic; R=results/bpic
C="--lambda 700 --f 3e12 --V 0.1 --d 30 --mode gated --nper 30 --settle 10000 --N 8000000 --out $R/checks.csv"
$B ${=C} --label strip --geo strip --L 546.41 --W 500 | tee -a $R/checks.log
$B ${=C} --label G3 --seed 5 | tee -a $R/checks.log
echo "done" >> $R/checks.log
