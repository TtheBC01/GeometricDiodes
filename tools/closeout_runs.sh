#!/bin/sh
# Stage H7 runs (plans/hydro_closeout.md): the open points of the fluid model. Outputs in results/thz2/h7.
#   closeout_runs.sh r1   # 100 K sweeps of the 1 um funnel with a constant viscosity, 20 nm mesh
#   closeout_runs.sh r1x  # the same at 0.1, 0.3 and 1 THz exactly
#   closeout_runs.sh r2   # position and extent of the largest first-order drift, three meshes
#   closeout_runs.sh r3   # finite-temperature coefficients at the low-density grid points
#   closeout_runs.sh r4   # amplitudes on either side of the drift limit, time domain, 80 nm mesh
#   closeout_runs.sh r5   # time domain on the 40 nm mesh at 0.3 THz
# This machine has 16 GB: at most two jobs at a time on the 20 nm mesh (98 963 nodes), eight in all.
set -e
cd "$(dirname "$0")/.."
S=./build/gd_thz_sweep; W=./build/gd_thz_power; O=results/thz2/h7
# r2 needs the sweep app that writes where the largest drift is; it is built apart so that the
# binaries of running jobs are not replaced (see the header of tools/closeout_report.py)
S2=./build/h7/gd_thz_sweep
mkdir -p $O
line() { name=$1; shift; echo "$S $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
line2() { name=$1; shift; echo "$S2 $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
run() { name=$1; shift; echo "$W $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
case "$1" in
r1)
  {
    for term in dss sym; do
      line R1_funnel_w1000_100K_${term}_nuconst --geo funnel_w1000 --set 100K --term $term --hw 20 --hm 120 --tauee 0
    done
  } | xargs -S 4096 -P 2 -I{} sh -c '{}'
  echo "r1 finished" ;;
r1x)
  # the same at the three frequencies of the time-domain ladders, for a comparison of the two solvers point for point
  $S --geo funnel_w1000 --set 100K --term dss --hw 20 --hm 120 --tauee 0 --f 1e11,3e11,1e12 \
     --out $O/R1x_funnel_w1000_100K_dss_nuconst.csv > $O/R1x_funnel_w1000_100K_dss_nuconst.txt 2>&1
  echo "r1x finished" ;;
r2)
  F=1e11,3e11,1e12
  {
    for set in 100K 300K; do
      line2 R2_${set}_dss_hw80 --geo funnel_w1000 --set $set --term dss --hw 80 --hm 480 --f $F
      line2 R2_${set}_dss_hw40 --geo funnel_w1000 --set $set --term dss --hw 40 --hm 240 --f $F
      line2 R2_${set}_dss_hw20 --geo funnel_w1000 --set $set --term dss --hw 20 --hm 120 --f $F
      line2 R2_${set}_sym_hw20 --geo funnel_w1000 --set $set --term sym --hw 20 --hm 120 --f $F
    done
  } | xargs -S 4096 -P 2 -I{} sh -c '{}'
  echo "r2 finished" ;;
r3)
  # coefficients from tools/closeout_coeffs.py: lines "set n d c_div c_grad"
  [ -f $O/ft_coeffs.txt ] || { echo "run tools/closeout_coeffs.py first"; exit 1; }
  grep -v '^#' $O/ft_coeffs.txt | while read set n d cdiv cgrad; do
    line R3_FT_${set}_n${n}_d${d} --geo funnel_w1000 --term dss --hw 20 --hm 120 --set $set --n $n --d ${d}e-9 --f 1e11,3e11,1e12 --cdiv $cdiv --cgrad $cgrad
  done | xargs -S 4096 -P 2 -I{} sh -c '{}'
  echo "r3 finished" ;;
r4)
  # 4 % below and above the amplitude at which the ladders of results/thz2 cross 0.3 v_F (linear
  # interpolation between their steps); the 100 K, 0.3 THz ladder has a point at 0.296 v_F already
  {
    run R4_100K_f1e11 --geo funnel_w1000 --set 100K --term dss --f 1e11 --Va 0.0810,0.0878
    run R4_100K_f3e11 --geo funnel_w1000 --set 100K --term dss --f 3e11 --Va 0.1053
    run R4_100K_f1e12 --geo funnel_w1000 --set 100K --term dss --f 1e12 --Va 0.1610,0.1744
    run R4_300K_f1e11 --geo funnel_w1000 --set 300K --term dss --f 1e11 --Va 0.1827,0.1979
    run R4_300K_f3e11 --geo funnel_w1000 --set 300K --term dss --f 3e11 --Va 0.1644,0.1781
    run R4_300K_f1e12 --geo funnel_w1000 --set 300K --term dss --f 1e12 --Va 0.1599,0.1732
  } | xargs -S 4096 -P 6 -I{} sh -c '{}'
  echo "r4 finished" ;;
r5)
  # the amplitudes are those of the 80 nm ladders (0.05, 0.1; 0.1) and of r4 (0.1781)
  {
    run R5_100K_f3e11_hw40 --geo funnel_w1000 --set 100K --term dss --f 3e11 --Va 0.05,0.1 --hw 40 --hm 240
    run R5_300K_f3e11_hw40 --geo funnel_w1000 --set 300K --term dss --f 3e11 --Va 0.1,0.1781 --hw 40 --hm 240
  } | xargs -S 4096 -P 2 -I{} sh -c '{}'
  echo "r5 finished" ;;
*) echo "usage: $0 r1 | r1x | r2 | r3 | r4 | r5"; exit 1 ;;
esac
