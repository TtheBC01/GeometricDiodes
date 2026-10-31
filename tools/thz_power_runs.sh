#!/bin/sh
# Stage H3 science runs (plans/hydro_largesignal.md, sec. 5): one background
# process per (set, f), nine in all. Outputs results/thz/P{1,2,3,4}_*.{csv,txt}.
set -e
cd "$(dirname "$0")/.."
B=./build/gd_thz_power; O=results/thz
VA=0.01,0.02,0.05,0.1,0.2,0.3,0.5,0.7
run() { name=$1; shift; nohup $B "$@" --out $O/$name.csv > $O/$name.txt 2>&1 & }
for f in 1e11 3e11 1e12; do
  run P1_funnel_w1000_100K_dss_f$f --geo funnel_w1000 --set 100K --term dss --f $f --Va $VA
  run P2_funnel_w1000_300K_dss_f$f --geo funnel_w1000 --set 300K --term dss --f $f --Va $VA
done
run P3_funnel_w1000_100K_ant_f3e11 --geo funnel_w1000 --set 100K --term ant --f 3e11 --Va $VA
for f in 3e11 1e12; do
  run P4_funnel_w1000_100K_sym_f$f --geo funnel_w1000 --set 100K --term sym --f $f --Va 0.05,0.1,0.2,0.3,0.5
done
wait
echo "all Stage H3 runs finished"
