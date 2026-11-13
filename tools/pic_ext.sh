#!/bin/zsh
# Stage B8 extension runs (plans/ballistic_selfconsistent.md section 5, P5
# and P6). G3 is the 0.55 um funnel; shape-only bias and drive throughout.
# Numerical settings as in the core set (tools/pic_runs.sh); the 1 THz runs
# average over 20 periods instead of 10, because the currents are smaller.
# usage: tools/pic_ext.sh P5|P6|P6c ...     (results/bpic/P5.csv, P6.csv, P6c.csv, ext.log)
set -e
cd "$(dirname "$0")/.."
B=./build/gd_bpic; R=results/bpic; mkdir -p $R
st() {   # st <csv> <V> <d> [extra flags]: static point, as P1
  local out=$1 V=$2 d=$3; shift 3
  $B --f 0 --V $V --d $d --mode sym --N 4000000 --settle 3000 --nper 10 --label G3 --out $R/$out "$@" | tee -a $R/ext.log
}
ac() {   # ac <csv> <V> <d> <periods> [extra flags]: 1 THz
  local out=$1 V=$2 d=$3 n=$4; shift 4
  $B --f 1e12 --V $V --d $d --mode sym --N 8000000 --settle 3000 --nper $n --label G3 --seed 5 --out $R/$out "$@" | tee -a $R/ext.log
}
for stage in "$@"; do
case $stage in
  P5)  # no scattering and a 100 nm mean free path: static +-0.1 V and 1 THz
    rm -f $R/P5.csv
    for lam in 100 inf; do for d in 300 30; do
      st P5.csv 0.1 $d --lambda $lam
      st P5.csv -0.1 $d --lambda $lam
      ac P5.csv 0.1 $d 20 --lambda $lam
    done; done ;;
  P6)  # 700 nm: half-diffuse walls (1 THz, and static +-0.1 V, which the
       # plan did not list); drive amplitudes 0.05 and 0.2 V at 1 THz
    rm -f $R/P6.csv
    for d in 300 30; do
      ac P6.csv 0.1 $d 20 --lambda 700 --p 0.5
      st P6.csv 0.1 $d --lambda 700 --p 0.5
      st P6.csv -0.1 $d --lambda 700 --p 0.5
    done
    for d in 300 30; do
      ac P6.csv 0.05 $d 20 --lambda 700
      ac P6.csv 0.2 $d 10 --lambda 700
    done ;;
  P6c) # checks on the half-diffuse result: the mirror-symmetric control with the
       # same walls (must be zero) and the funnel with a second seed
    rm -f $R/P6c.csv
    for d in 300 30; do
      $B --f 1e12 --V 0.1 --d $d --mode sym --N 8000000 --settle 3000 --nper 20 --label control --geo symmetric --lambda 700 --p 0.5 --out $R/P6c.csv | tee -a $R/ext.log
      $B --f 1e12 --V 0.1 --d $d --mode sym --N 8000000 --settle 3000 --nper 20 --label G3 --seed 11 --lambda 700 --p 0.5 --out $R/P6c.csv | tee -a $R/ext.log
    done ;;
esac
done
echo "done $*" >> $R/ext.log
