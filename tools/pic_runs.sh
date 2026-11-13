#!/bin/zsh
# Stage B8 production runs, core set (plans/ballistic_selfconsistent.md
# section 5). G3 is the 0.55 um funnel (the default shape of gd_bpic); mean
# free path 700 nm; V_a = 0.1 V. Each run is multi-threaded; runs are sequential.
# Settings from the checkpoint: 8 million particles (4 million for static runs
# and at 0.1 THz), 5 nm grid, low-pass 1.5 cells. The averaging window is at
# least 10 ps and at least three periods.
# usage: tools/pic_runs.sh P1|P2|P3|P4 ...     (results/bpic/P*.csv, runs.log)
set -e
cd "$(dirname "$0")/.."
B=./build/gd_bpic; R=results/bpic; mkdir -p $R
# frequency (THz) -> "periods settle_fs N"
typeset -A PLAN
PLAN[0.1]="6 10000 4000000"
PLAN[0.3]="6 6667 8000000"
PLAN[0.6]="10 3334 8000000"
PLAN[1]="10 3000 8000000"
PLAN[1.8]="18 3000 8000000"
PLAN[3]="30 3000 8000000"
PLAN[6]="60 3000 8000000"
ac() {   # ac <csv> <label> <f_THz> <d> <mode> [extra flags]
  local out=$1 lab=$2 f=$3 d=$4 mode=$5; shift 5
  local a=(${=PLAN[$f]})
  $B --lambda 700 --f ${f}e12 --V 0.1 --d $d --mode $mode --nper $a[1] --settle $a[2] --N $a[3] --label $lab --out $R/$out "$@" | tee -a $R/runs.log
}
for stage in "$@"; do
case $stage in
  P1)  # static, shape-only bias, with potential maps at +-0.1 V
    rm -f $R/P1.csv
    for d in 300 30; do for V in 0.1 -0.1 0.05 -0.05; do
      m=(); [[ $V == 0.1 || $V == -0.1 ]] && m=(--map $R/P1_d${d}_V$V)
      $B --lambda 700 --f 0 --V $V --d $d --mode sym --N 4000000 --settle 3000 --nper 10 --label G3 --out $R/P1.csv $m | tee -a $R/runs.log
    done; done ;;
  P2)  # G3 at 0.3, 1 and 3 THz, both gates, shape-only drive and drain tied to the gate
    rm -f $R/P2.csv
    for f in 1 0.3 3; do for d in 300 30; do for mode in sym gated; do
      ac P2.csv G3 $f $d $mode --seed 5
    done; done; done ;;
  P3)  # mirror-symmetric control, shape-only drive (must be zero); strip with the drain tied to the gate
    rm -f $R/P3.csv
    for f in 1 0.3 3; do for d in 300 30; do
      ac P3.csv control $f $d sym --geo symmetric
      ac P3.csv strip $f $d gated --geo strip --L 546.41 --W 500
    done; done ;;
  P4)  # G3, shape-only drive: more frequencies
    rm -f $R/P4.csv
    for f in 0.6 1.8 6 0.1; do for d in 300 30; do
      ac P4.csv G3 $f $d sym --seed 5
    done; done ;;
esac
done
echo "done $*" >> $R/runs.log
