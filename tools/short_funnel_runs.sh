#!/bin/zsh
# Stage B7 science runs (plans/ballistic_short.md section 3). Each gd_bthz call
# is multi-threaded; stages run sequentially.
# usage: tools/short_funnel_runs.sh S1|S2|S3|S4|S5 [G1 G2 G3 G4]
set -e
cd "$(dirname "$0")/.."
B=./build/gd_bthz
R=results/bshort
mkdir -p $R
typeset -A SHAPE ELL FMAX
SHAPE[G1]="--scale 0.5";                                              ELL[G1]=87.5; FMAX[G1]=3e12
SHAPE[G2]="--Lc 250 --Ld 250";                                        ELL[G2]=175;  FMAX[G2]=1e13
SHAPE[G3]="--Wc 500 --Lc 100 --Wn 100 --Wd 500 --Ld 100";             ELL[G3]=100;  FMAX[G3]=3e13
SHAPE[G4]="--Wc 400 --Lc 169 --alpha 45 --Wn 75 --Wd 400 --Ld 169";   ELL[G4]=75;   FMAX[G4]=3e13
FREQS=(1e10 3e10 1e11 3e11 1e12 3e12 1e13 3e13)
stage=$1; shift
GS=($@); [[ ${#GS} == 0 ]] && GS=(G3 G4 G2 G1)
case $stage in
  S1)  # static curves, two-sided estimator
    for g in $GS; do
      rm -f $R/S1_$g.csv
      for fm in uniform local; do for lam in inf 700 100; do
        N=2000000; [[ $lam != inf ]] && N=4000000
        for V in 0.1 -0.1 0.05 -0.05; do
          $B --geo funnel ${=SHAPE[$g]} --label $g --field $fm --ellF $ELL[$g] --lambda $lam --f 0 --V $V --N $N --out $R/S1_$g.csv
        done
      done; done
    done ;;
  S2)  # frequency sweeps at Va = 0.1 V, up to f L / v_F of about 20
    for g in $GS; do
      rm -f $R/S2_$g.csv
      for fm in uniform local; do for lam in inf 700 100; do
        N=1000000; [[ $lam != inf ]] && N=4000000
        for f in $FREQS; do
          (( f > FMAX[$g] )) && continue
          $B --geo funnel ${=SHAPE[$g]} --label $g --field $fm --ellF $ELL[$g] --lambda $lam --f $f --V 0.1 --N $N --out $R/S2_$g.csv
        done
      done; done
    done ;;
  S3)  # Device D clean limit beyond f L / v_F = 6
    rm -f $R/S3.csv
    for fm in uniform local; do for f in 2e12 3e12 5e12; do
      $B --geo funnel --label G0 --field $fm --ellF 500 --lambda inf --f $f --V 0.1 --N 1000000 --out $R/S3.csv
    done; done ;;
  S4)  # mirror-symmetric controls
    rm -f $R/S4.csv
    for g in $GS; do for fm in uniform local; do for f in 1e11 1e12 1e13; do
      (( f > FMAX[$g] )) && continue
      $B --geo symmetric ${=SHAPE[$g]} --label ${g}sym --field $fm --ellF $ELL[$g] --lambda inf --f $f --V 0.1 --N 1000000 --out $R/S4.csv
    done; done; done ;;
  S5)  # decision device: half-diffuse walls, and the amplitude ladder at 1 THz (field from $2, default uniform)
    rm -f $R/S5.csv
    g=G3
    for fm in uniform local; do
      for V in 0.1 -0.1; do
        $B --geo funnel ${=SHAPE[$g]} --label ${g}p05 --field $fm --ellF $ELL[$g] --lambda 700 --p 0.5 --f 0 --V $V --N 4000000 --out $R/S5.csv
      done
      for f in 3e11 1e12 3e12; do
        $B --geo funnel ${=SHAPE[$g]} --label ${g}p05 --field $fm --ellF $ELL[$g] --lambda 700 --p 0.5 --f $f --V 0.1 --N 4000000 --out $R/S5.csv
      done
      for V in 0.05 0.2 0.3; do
        N=4000000; [[ $V == 0.05 ]] && N=16000000
        $B --geo funnel ${=SHAPE[$g]} --label ${g}amp --field $fm --ellF $ELL[$g] --lambda 700 --f 1e12 --V $V --N $N --out $R/S5.csv
      done
    done ;;
  *) echo "unknown stage $stage"; exit 2 ;;
esac
