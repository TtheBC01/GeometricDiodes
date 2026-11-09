#!/bin/sh
# Stage H4 science runs (plans/hydro_density.md, sec. 5) on the 18601-node
# production mesh of S2/S5 (hw 80, hm 480). Outputs results/thz/D*_*.{csv,txt}.
#   thz_density_runs.sh D1          # D1 (grid), D1m (matched antenna), D1t (tau ~ sqrt n), ten at a time
#   thz_density_runs.sh D2 n d_nm   # contact resistance 100/300/1000 ohm um at (n, d), 75 ohm and matched
#   thz_density_runs.sh D3          # time-domain saturation ladder at (5e11, 10 nm), 0.3 THz, both sets
set -e
cd "$(dirname "$0")/.."
B=./build/gd_thz_sweep; O=results/thz; MESH="--geo funnel_w1000 --term dss --hw 80 --hm 480"; F=1e11,3e11,1e12
P=${JOBS:-10}
case "$1" in
D1)
  for set in 100K 300K; do for n in 1e12 5e11 2e11 1e11 5e10; do for d in 30 10 5; do
    tag=${set}_n${n}_d${d}
    echo "$B $MESH --set $set --n $n --d ${d}e-9 --f $F --out $O/D1_$tag.csv > $O/D1_$tag.txt 2>&1"
    echo "$B $MESH --set $set --n $n --d ${d}e-9 --f $F --match 1 --out $O/D1m_$tag.csv > $O/D1m_$tag.txt 2>&1"
    [ $set = 100K ] && echo "$B $MESH --set $set --n $n --d ${d}e-9 --f $F --tauscale sqrtn --out $O/D1t_$tag.csv > $O/D1t_$tag.txt 2>&1"
  done; done; done | xargs -P $P -I{} sh -c '{}'
  echo "D1, D1m, D1t finished" ;;
D2)
  n=$2; d=$3
  for set in 100K 300K; do for Rc in 100 300 1000; do
    tag=${set}_n${n}_d${d}_Rc${Rc}
    echo "$B $MESH --set $set --n $n --d ${d}e-9 --f $F --Rc $Rc --out $O/D2_$tag.csv > $O/D2_$tag.txt 2>&1"
    echo "$B $MESH --set $set --n $n --d ${d}e-9 --f $F --Rc $Rc --match 1 --out $O/D2m_$tag.csv > $O/D2m_$tag.txt 2>&1"
  done; done | xargs -P $P -I{} sh -c '{}'
  echo "D2 at ($n, $d nm) finished" ;;
D3)
  for set in 100K 300K; do
    nohup ./build/gd_thz_power --geo funnel_w1000 --set $set --term dss --f 3e11 --Va 0.1,0.3,0.5,0.7 --n 5e11 --d 10e-9 \
      --out $O/D3_funnel_w1000_${set}_dss_n5e11_d10_f3e11.csv > $O/D3_funnel_w1000_${set}_dss_n5e11_d10_f3e11.txt 2>&1 &
  done
  wait; echo "D3 finished" ;;
*) echo "usage: $0 D1 | D2 n d_nm | D3"; exit 1 ;;
esac
