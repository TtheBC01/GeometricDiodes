#!/bin/sh
# Stage H8 runs (plans/hydro_referee.md): three checks the referee read of the ninth draft asked for.
# Outputs in results/thz2/h8.
#   referee_runs.sh mesh  # create every mesh of runs A and C, one at a time, and print its size
#   referee_runs.sh ac    # run A (the 1 um funnel with leads of 1, 3 and 6 um) and run C (the 1 um valve)
#   referee_runs.sh b     # run B: time-domain ladders at five thin-gate grid points, 80 nm mesh
#   referee_runs.sh bss   # after run B: the small-signal solver with a constant viscosity at its three 100 K points
# This machine has 16 GB. Jobs on meshes above 50 000 nodes go two at a time, the others five at a time,
# so at most seven jobs run at once. The binaries are built apart, in build/h8, so that a later build
# does not replace the binary of a running job.
set -e
cd "$(dirname "$0")/.."
S=./build/h8/gd_thz_sweep; W=./build/h8/gd_thz_power; M=./build/h8/gd_thz_mesh; O=results/thz2/h8
F3=1e11,3e11,1e12
mkdir -p $O
line() { name=$1; shift; echo "$S $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
run() { name=$1; shift; echo "$W $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
VL=tesla_classic_w1000; VS=tesla_classic_w1000_L1000
case "$1" in
mesh)
  for L in 1000 3000 6000; do
    $M --geo funnel_w1000_L$L --hw 20 --hm 120 | tail -1; $M --geo chan_w4000_L$L --hw 20 --hm 120 | tail -1
  done
  $M --geo funnel_w1000_L1000 --hw 40 --hm 240 | tail -1
  for g in $VL $VS; do
    $M --geo $g --hw 114 --hm 686 | tail -1; $M --geo $g --hw 80 --hm 480 | tail -1; $M --geo $g --hw 57 --hm 343 | tail -1
  done ;;
ac)
  # ---- meshes above 50 000 nodes: the 6 um leads (56 119 and 52 751), the long valve at 80 nm (53 433)
  # and both valves at 57 nm (78 483 and 56 995)
  {
    for set in 300K 100K; do
      for term in dss sym; do line A_funnel_w1000_L6000_${set}_$term --geo funnel_w1000_L6000 --set $set --term $term --hw 20 --hm 120; done
      line A_chan_w4000_L6000_${set}_dss --geo chan_w4000_L6000 --set $set --term dss --hw 20 --hm 120
    done
    for set in 300K 100K; do for term in dss sym; do
      line C_${VL}_${set}_$term --geo $VL --set $set --term $term --hw 80 --hm 480
    done; done
    for set in 300K 100K; do for term in dss sym; do
      line C_${VL}_${set}_${term}_f3_hw80 --geo $VL --set $set --term $term --hw 80 --hm 480 --f $F3
      line C_${VL}_${set}_${term}_f3_match --geo $VL --set $set --term $term --hw 80 --hm 480 --f $F3 --match 1
      line C_${VL}_${set}_${term}_f3_hw57 --geo $VL --set $set --term $term --hw 57 --hm 343 --f $F3
      line C_${VS}_${set}_${term}_f3_hw57 --geo $VS --set $set --term $term --hw 57 --hm 343 --f $F3
    done; done
  } | xargs -S 4096 -P 2 -I{} sh -c '{}' &
  # ---- the smaller meshes
  {
    for L in 1000 3000; do for set in 300K 100K; do
      for term in dss sym; do line A_funnel_w1000_L${L}_${set}_$term --geo funnel_w1000_L$L --set $set --term $term --hw 20 --hm 120; done
      line A_chan_w4000_L${L}_${set}_dss --geo chan_w4000_L$L --set $set --term dss --hw 20 --hm 120
    done; done
    for term in dss sym; do line A_funnel_w1000_L1000_300K_${term}_hw40 --geo funnel_w1000_L1000 --set 300K --term $term --hw 40 --hm 240; done
    for set in 300K 100K; do for term in dss sym; do
      line C_${VS}_${set}_$term --geo $VS --set $set --term $term --hw 80 --hm 480
    done; done
    for set in 300K 100K; do for term in dss sym; do
      line C_${VS}_${set}_${term}_f3_hw80 --geo $VS --set $set --term $term --hw 80 --hm 480 --f $F3
      line C_${VS}_${set}_${term}_f3_match --geo $VS --set $set --term $term --hw 80 --hm 480 --f $F3 --match 1
      line C_${VL}_${set}_${term}_f3_hw114 --geo $VL --set $set --term $term --hw 114 --hm 686 --f $F3
      line C_${VS}_${set}_${term}_f3_hw114 --geo $VS --set $set --term $term --hw 114 --hm 686 --f $F3
    done; done
  } | xargs -S 4096 -P 5 -I{} sh -c '{}' &
  wait
  echo "runs A and C finished" ;;
b)
  # amplitudes in units of V_unit: 0.25, 0.5, ... times the amplitude at which the small-signal solver
  # puts the first-order drift at 0.3 v_F on the 20 nm mesh (column Va_at_0p3vF of results/thz2/fine/D1_*)
  G="--geo funnel_w1000 --term dss"
  {
    run B_100K_n5e11_d5_f3e11  $G --set 100K --n 5e11 --d 5e-9  --f 3e11 --Va 0.172,0.344,0.516,0.688,0.859,1.031,1.203
    run B_100K_n2e11_d10_f3e11 $G --set 100K --n 2e11 --d 10e-9 --f 3e11 --Va 0.106,0.213,0.319,0.425,0.531,0.638,0.744
    run B_100K_n1e12_d5_f3e11  $G --set 100K --n 1e12 --d 5e-9  --f 3e11 --Va 0.165,0.329,0.494,0.659,0.823,0.988,1.153
    run B_300K_n5e11_d5_f1e11  $G --set 300K --n 5e11 --d 5e-9  --f 1e11 --Va 0.181,0.361,0.542,0.723,0.903
    run B_300K_n1e12_d5_f3e11  $G --set 300K --n 1e12 --d 5e-9  --f 3e11 --Va 0.171,0.341,0.512,0.683,0.854
    run B_100K_n5e11_d5_f3e11_hw40 $G --set 100K --n 5e11 --d 5e-9 --f 3e11 --Va 0.688,1.031 --hw 40 --hm 240
  } | xargs -S 4096 -P 6 -I{} sh -c '{}'
  echo "run B finished" ;;
bss)
  # the two solvers differ at 100 K by the viscoelastic term only if these agree with the square law of the ladders
  for hw in 80 20; do
    P=3; [ $hw = 20 ] && P=2
    for p in "5e11 5" "2e11 10" "1e12 5"; do
      n=${p% *}; d=${p#* }
      line B_ss_const_100K_n${n}_d${d}_hw$hw --geo funnel_w1000 --term dss --set 100K --n $n --d ${d}e-9 --f 3e11 --tauee 0 --hw $hw --hm $((hw * 6))
    done | xargs -S 4096 -P $P -I{} sh -c '{}'
  done
  echo "constant-viscosity solves finished" ;;
*) echo "usage: $0 mesh | ac | b | bss"; exit 1 ;;
esac
