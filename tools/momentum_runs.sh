#!/bin/sh
# Stage H6 production runs (plans/hydro_momentum.md, section 5): every fluid run of Stages H2
# to H5 repeated with the corrected momentum equation. Outputs go to results/thz2 and
# results/dcgate2; the files of the earlier stages are not touched.
#   momentum_runs.sh small     # 3a: S1 to S3, the controls C1, C2 and the finite-temperature variant
#   momentum_runs.sh density   # 3a: D1, D1m, D1t, D2, D2m (the operating-point grid and contact resistance)
#   momentum_runs.sh steady    # 3b: D1 to D4 of Stage H5
#   momentum_runs.sh large     # 3c: P1 to P4 and D3
set -e
cd "$(dirname "$0")/.."
# This machine has 16 GB of memory: more than eight solver jobs at a time make it swap.
S=./build/gd_thz_sweep; W=./build/gd_thz_power; O=results/thz2; P=${JOBS:-8}
mkdir -p $O results/dcgate2
# isothermal finite-temperature coefficients at 1e12 cm^-2 (tools/momentum_step0.py, part 4)
FT100="--cdiv -0.4745 --cgrad -0.2372"; FT300="--cdiv -0.3417 --cgrad -0.1709"
# The 175 nm funnel and the Tesla valve use the 14 nm wall mesh of Stages H2 to H4. The first Stage H6
# runs of these two devices left the mesh size out and ran at the default 20 nm; those files are kept in
# results/thz2/hw20 as a mesh comparison.
M14="--hw 14 --hm 84"
line() { name=$1; shift; echo "$S $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
case "$1" in
small)
  {
    # S1: uniform channel, ideal source, open drain
    line S1_chan_100K_ds --geo chan --set 100K --term ds --hw 50 --hm 50
    line S1_chan_300K_ds --geo chan --set 300K --term ds --hw 50 --hm 50
    line S1_chan_100K_inviscid_ds --geo chan --set 100K --term ds --hw 50 --hm 50 --nu 0 --tauee 0
    line S1_chan_100K_sw_ds --geo chan --set 100K --term ds --hw 50 --hm 50 --closure sw --nu 0 --tauee 0
    line S1tp_chan_100K_ds --geo chan --set 100K --term ds --hw 50 --hm 50 --closure tp
    line S1tp_chan_300K_ds --geo chan --set 300K --term ds --hw 50 --hm 50 --closure tp
    for set in 100K 300K; do
      [ $set = 100K ] && FT=$FT100 || FT=$FT300
      line S1ft_chan_${set}_ds --geo chan --set $set --term ds --hw 50 --hm 50 $FT
      line S1c2_chan_${set}_ds --geo chan --set $set --term ds --hw 50 --hm 50 --contact_drift 1
      # S2: the three funnels
      for term in dss sym dsd; do line S2_funnel_${set}_$term --geo funnel --set $set --term $term $M14; done
      for term in dss sym; do
        line S2_funnel_short_${set}_$term --geo funnel_short --set $set --term $term --hw 14 --hm 84
        line S2_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term --hw 80 --hm 480
        # C1: plain channel with the lead width and the full length of the 1 um funnel
        line C1_chan_w4000_${set}_$term --geo chan_w4000 --set $set --term $term --hw 80 --hm 480
        # C2: the contact condition with the drift term; finite-temperature coefficients
        line C2_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term --hw 80 --hm 480 --contact_drift 1
        line FT_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term --hw 80 --hm 480 $FT
        line FT_funnel_${set}_$term --geo funnel --set $set --term $term $M14 $FT
        # S3: classical Tesla valve
        line S3_tesla_classic_${set}_$term --geo tesla_classic --set $set --term $term $M14
      done
    done
    for term in dss sym; do line S2_funnel_short_100K_${term}_fine --geo funnel_short --set 100K --term $term --hw 14 --hm 84 --fmin 1.2e12 --fmax 2.6e12 --nf 15; done
  } | xargs -P $P -I{} sh -c '{}'
  echo "small-signal runs finished" ;;
density)
  MESH="--geo funnel_w1000 --term dss --hw 80 --hm 480"; F=1e11,3e11,1e12
  {
    for set in 100K 300K; do for n in 1e12 5e11 2e11 1e11 5e10; do for d in 30 10 5; do
      tag=${set}_n${n}_d${d}
      line D1_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F
      line D1m_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F --match 1
      [ $set = 100K ] && line D1t_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F --tauscale sqrtn
    done; done; done
    for nd in "1e12 30" "5e11 10" "5e10 30"; do set -- $nd; n=$1; d=$2
      for st in 100K 300K; do for Rc in 100 300 1000; do
        tag=${st}_n${n}_d${d}_Rc${Rc}
        line D2_$tag $MESH --set $st --n $n --d ${d}e-9 --f $F --Rc $Rc
        line D2m_$tag $MESH --set $st --n $n --d ${d}e-9 --f $F --Rc $Rc --match 1
      done; done
    done
  } | xargs -P $P -I{} sh -c '{}'
  echo "density and contact-resistance runs finished" ;;
steady)
  B=./build/gd_dc_gate; R=results/dcgate2; VS=0.001,0.003,0.01,0.03,0.1,0.2,0.3
  rm -f $R/D1.csv $R/D2.csv $R/D3.csv $R/D4.csv
  (for set in 300K 100K; do
     $B --geo chan --set $set --hw 50 --hm 50 --v $VS --out $R/D1.csv
     $B --geo symcon --set $set --hw 20 --hm 120 --v $VS --out $R/D1.csv
   done > $R/D1.log 2>&1) &
  (for set in 300K 100K; do for slip in inf 0; do
     $B --geo funnel --set $set --slip $slip --hw 20 --hm 120 --v $VS --out $R/D2.csv
     $B --geo funnel_w1000 --set $set --slip $slip --hw 80 --hm 480 --v $VS --out $R/D2.csv
   done; done > $R/D2.log 2>&1) &
  (for set in 300K 100K; do
     $B --geo tesla_classic --set $set --hw 20 --hm 120 --v $VS --out $R/D3.csv
     $B --geo tesla_classic_w1000 --set $set --hw 114 --hm 686 --v $VS --out $R/D3.csv
   done > $R/D3.log 2>&1) &
  ({ for m in "28 168" "20 120" "14 84" "10 60"; do set -- $m
     $B --geo funnel --set 300K --hw $1 --hm $2 --v 0.01,0.03 --out $R/D4.csv
   done
   for m in "20 120" "14 84" "10 60"; do set -- $m
     $B --geo tesla_classic --set 300K --hw $1 --hm $2 --v 0.01,0.03 --out $R/D4.csv
   done
   for m in "20 120" "14 84"; do set -- $m
     $B --geo symcon --set 300K --hw $1 --hm $2 --v 0.03 --out $R/D4.csv
   done; } > $R/D4.log 2>&1) &
  wait
  echo "steady gated runs finished" ;;
large)
  VA=0.01,0.02,0.05,0.1,0.15,0.2,0.25,0.3,0.4,0.5,0.7
  run() { name=$1; shift; echo "$W $* --out $O/$name.csv > $O/$name.txt 2>&1"; }
  {
    for f in 1e11 3e11 1e12; do
      # as run, the 1 THz, 100 K ladder was 0.01, 0.02, then 0.05, 0.1, 0.2, 0.3 (27 minutes per amplitude)
      run P1_funnel_w1000_100K_dss_f$f --geo funnel_w1000 --set 100K --term dss --f $f --Va $VA
      run P2_funnel_w1000_300K_dss_f$f --geo funnel_w1000 --set 300K --term dss --f $f --Va $VA
    done
    run P3_funnel_w1000_100K_ant_f3e11 --geo funnel_w1000 --set 100K --term ant --f 3e11 --Va $VA
    for f in 3e11 1e12; do
      run P4_funnel_w1000_100K_sym_f$f --geo funnel_w1000 --set 100K --term sym --f $f --Va 0.02,0.05,0.1,0.15,0.2,0.3,0.5
    done
    for set in 100K 300K; do
      run D3_funnel_w1000_${set}_dss_n5e11_d10_f3e11 --geo funnel_w1000 --set $set --term dss --f 3e11 --Va 0.05,0.1,0.2,0.3,0.5,0.7 --n 5e11 --d 10e-9
    done
    # the second-order form in the time solver, for comparison with the exact one
    run P1d2_funnel_w1000_100K_dss_f3e11 --geo funnel_w1000 --set 100K --term dss --f 3e11 --Va 0.05,0.1,0.2,0.3 --closure dirac2
  } | xargs -P $P -I{} sh -c '{}'
  echo "large-signal runs finished" ;;
contact)
  # C2 with the opposite sign: the contact fixes Phi(n) - u^2 / 4 (the Fermi energy of the drifting
  # gas averaged over directions). With --contact_drift 1 (in "small") it brackets the default.
  {
    for set in 100K 300K; do
      line S1c2m_chan_${set}_ds --geo chan --set $set --term ds --hw 50 --hm 50 --contact_drift -1
      for term in dss sym; do
        line C2m_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term --hw 80 --hm 480 --contact_drift -1
        line C2_funnel_${set}_$term --geo funnel --set $set --term $term $M14 --contact_drift 1
        line C2m_funnel_${set}_$term --geo funnel --set $set --term $term $M14 --contact_drift -1
      done
    done
  } | xargs -P $P -I{} sh -c '{}'
  echo "contact-variant runs finished" ;;
mesh14)
  # the 22 sweeps of the 175 nm funnel and the Tesla valve alone (see M14 above)
  {
    for set in 100K 300K; do
      [ $set = 100K ] && FT=$FT100 || FT=$FT300
      for term in dss sym dsd; do line S2_funnel_${set}_$term --geo funnel --set $set --term $term $M14; done
      for term in dss sym; do
        line S3_tesla_classic_${set}_$term --geo tesla_classic --set $set --term $term $M14
        line FT_funnel_${set}_$term --geo funnel --set $set --term $term $M14 $FT
        line C2_funnel_${set}_$term --geo funnel --set $set --term $term $M14 --contact_drift 1
        line C2m_funnel_${set}_$term --geo funnel --set $set --term $term $M14 --contact_drift -1
      done
    done
  } | xargs -P $P -I{} sh -c '{}'
  echo "14 nm runs finished" ;;
fine)
  # the four headline sweeps of the 1 um funnel on the 20 nm wall mesh (98 963 nodes; the production
  # mesh is 80 nm, 18 601 nodes), after the corner check of tools/momentum_corner.sh showed the 80 nm
  # mesh low by up to 12 % at 100 K. 3.6 GB per job: two at a time.
  mkdir -p $O/fine
  {
    for set in 100K 300K; do for term in dss sym; do
      line fine/S2_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term --hw 20 --hm 120
    done; done
  } | xargs -S 4096 -P ${JOBS:-2} -I{} sh -c '{}'
  # the same on the 40 nm mesh (41 509 nodes), for an error estimate at every frequency; three checks of
  # the interior element size (80 nm walls with 120 nm interior elements, which separates the two errors);
  # and the grid point with the slowest plasma wave on the 20 nm mesh
  {
    for set in 100K 300K; do for term in dss sym; do
      line fine/S2_funnel_w1000_${set}_${term}_hw40 --geo funnel_w1000 --set $set --term $term --hw 40 --hm 240
    done; done
    line fine/check_hw80_hm120_300K_dss --geo funnel_w1000 --set 300K --term dss --hw 80 --hm 120 --f 9.142e11,1.8651e12,3e12
    line fine/check_hw80_hm120_100K_dss --geo funnel_w1000 --set 100K --term dss --hw 80 --hm 120 --f 7.208e11,1.8651e12,3e12
    line fine/check_hw80_hm60_300K_dss --geo funnel_w1000 --set 300K --term dss --hw 80 --hm 60 --f 1.8651e12,3e12
  } | xargs -S 4096 -P 4 -I{} sh -c '{}'
  {
    for set in 100K 300K; do
      line fine/D1_${set}_n5e10_d5_hw20 --geo funnel_w1000 --term dss --hw 20 --hm 120 --set $set --n 5e10 --d 5e-9 --f 1e11,3e11,1e12
    done
  } | xargs -S 4096 -P 2 -I{} sh -c '{}'
  echo "fine runs finished" ;;
fine2)
  # every other small-signal run of the 1 um funnel on the 20 nm wall mesh, with the names of the 80 nm
  # runs, in results/thz2/fine: contact variants, finite-temperature coefficients, the plain-channel
  # control, the density and gate grid, contact resistance. About 2.5 to 3.6 GB per job: two at a time.
  mkdir -p $O/fine
  M20="--hw 20 --hm 120"; MESH="--geo funnel_w1000 --term dss $M20"; F=1e11,3e11,1e12
  # make the channel mesh before the parallel runs
  [ -f meshes/h_chan_w4000_hw20_hm120.mesh ] || $S --geo chan_w4000 --set 300K --term dss $M20 --f 1e11 > $O/fine/mesh_chan_w4000.txt 2>&1 || true
  {
    for set in 100K 300K; do
      [ $set = 100K ] && FT=$FT100 || FT=$FT300
      for term in dss sym; do
        line fine/C1_chan_w4000_${set}_$term --geo chan_w4000 --set $set --term $term $M20
        line fine/C2_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term $M20 --contact_drift 1
        line fine/C2m_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term $M20 --contact_drift -1
        line fine/FT_funnel_w1000_${set}_$term --geo funnel_w1000 --set $set --term $term $M20 $FT
      done
    done
    for set in 100K 300K; do for n in 1e12 5e11 2e11 1e11 5e10; do for d in 30 10 5; do
      tag=${set}_n${n}_d${d}
      line fine/D1_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F
      line fine/D1m_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F --match 1
      [ $set = 100K ] && line fine/D1t_$tag $MESH --set $set --n $n --d ${d}e-9 --f $F --tauscale sqrtn
    done; done; done
    for nd in "1e12 30" "5e11 10" "5e10 30"; do set -- $nd; n=$1; d=$2
      for st in 100K 300K; do for Rc in 100 300 1000; do
        tag=${st}_n${n}_d${d}_Rc${Rc}
        line fine/D2_$tag $MESH --set $st --n $n --d ${d}e-9 --f $F --Rc $Rc
        line fine/D2m_$tag $MESH --set $st --n $n --d ${d}e-9 --f $F --Rc $Rc --match 1
      done; done
    done
  } | xargs -S 4096 -P ${JOBS:-2} -I{} sh -c '{}'
  echo "fine2 runs finished" ;;
legacy20)
  # the 1 um funnel with the ordinary convective term (the equation of Stages H2 to H5) on the 20 nm
  # wall mesh, so that its comparison with the Dirac gas is made on one mesh. Two jobs at a time.
  mkdir -p $O/fine
  {
    for set in 100K 300K; do
      line fine/L_funnel_w1000_${set}_dss --geo funnel_w1000 --set $set --term dss --hw 20 --hm 120 --closure legacy
    done
  } | xargs -S 4096 -P ${JOBS:-2} -I{} sh -c '{}'
  echo "legacy20 runs finished" ;;
*) echo "usage: $0 small | density | steady | large | contact | mesh14 | fine | fine2 | legacy20"; exit 1 ;;
esac
