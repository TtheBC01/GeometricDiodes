#!/bin/zsh
# Stage B2 science runs (plans/ballistic_thz.md section 4). Each gd_bthz call
# is multi-threaded over 64 batches; stages run sequentially.
# usage: tools/ballistic_thz_runs.sh B1|B2u|B2l|B3|B0|B4|B5 [f_Hz for B4] [field for B4] [ellF for B4]
set -e
cd "$(dirname "$0")/.."
B=./build/gd_bthz
R=results/bthz
mkdir -p $R
XN=3580.5
FREQS=(3e8 1e9 3e9 1e10 3e10 1e11 3e11 1e12)
stage=$1
case $stage in
  B1)  # static I(V) curves, both estimators, four field models, two mean free paths
    rm -f $R/B1.csv
    for field in uniform local175 local500 local1600; do
      fm=${field%%[0-9]*}; ell=${field#local}; [[ $fm == uniform ]] && ell=500
      for lam in inf 700; do
        N=1000000; [[ $lam == 700 ]] && N=2000000
        for V in 0.025 0.05 0.1 0.2; do
          for sgn in 1 -1; do
            VV=$(python3 -c "print($sgn*$V)")
            th=""; [[ $V == 0.1 && $sgn == 1 ]] && th="--thist $R/B1_thist_${field}_lam${lam}.csv"
            $B --geo funnel_nguyenD --field $fm --ellF $ell --lambda $lam --f 0 --V $VV --N $N --out $R/B1.csv ${=th}
          done
        done
      done
    done ;;
  B2u|B2l)  # frequency sweeps at Va 0.1 (and 0.05 at two frequencies)
    fm=uniform; ell=500; [[ $stage == B2l ]] && fm=local
    rm -f $R/$stage.csv
    for lam in inf 700; do
      N=1000000; [[ $lam == 700 ]] && N=4000000
      for f in $FREQS; do
        hist=""; [[ $f == 1e12 || $f == 3e11 ]] && hist="--hist $R/${stage}_Ehist_lam${lam}_f${f}.csv --thist $R/${stage}_thist_lam${lam}_f${f}.csv"
        $B --geo funnel_nguyenD --field $fm --ellF $ell --lambda $lam --f $f --V 0.1 --N $N --out $R/$stage.csv ${=hist}
      done
      for f in 3e8 3e11; do
        $B --geo funnel_nguyenD --field $fm --ellF $ell --lambda $lam --f $f --V 0.05 --N $((4*N)) --out $R/$stage.csv
      done
    done ;;
  B3)  # where the drop lives: ell_F at the two decision frequencies (clean limit)
    rm -f $R/B3.csv
    for ell in 175 500 1600; do
      for f in 3e11 1e12; do
        $B --geo funnel_nguyenD --field local --ellF $ell --lambda inf --f $f --V 0.1 --N 1000000 --out $R/B3.csv
      done
    done ;;
  B0)  # straight control at every frequency, both field models
    rm -f $R/B0.csv
    for fm in uniform local; do
      for f in $FREQS; do
        $B --geo straight_control --field $fm --ellF 500 --lambda inf --f $f --V 0.1 --N 1000000 --out $R/B0.csv
      done
    done ;;
  B4)  # amplitude ladder at one frequency: B4 f field ellF
    f=$2; fm=$3; ell=$4
    rm -f $R/B4.csv
    for V in 0.05 0.1 0.2 0.3; do
      N=1000000; [[ $V == 0.05 ]] && N=4000000
      $B --geo funnel_nguyenD --field $fm --ellF $ell --lambda inf --f $f --V $V --N $N --out $R/B4.csv --hist $R/B4_Ehist_V${V}.csv
    done ;;
  B5)  # Dirac-cut sensitivity: the same points with the classical turning point instead of the cut
    rm -f $R/B5.csv
    for args in "local 500 inf 1e12 1000000" "local 500 inf 3e11 1000000" "uniform 500 inf 1e10 1000000" "local 500 700 1e12 4000000" "local 500 700 3e11 4000000"; do
      set -- ${=args}
      for rf in "" "--reflect"; do
        $B --geo funnel_nguyenD --field $1 --ellF $2 --lambda $3 --f $4 --V 0.1 --N $5 --out $R/B5.csv ${=rf}
      done
    done ;;
  *) echo "unknown stage $stage"; exit 2 ;;
esac
