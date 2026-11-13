#!/bin/zsh
# Stage H5 science runs (plans/hydro_dcgate.md section 3).
# usage: tools/dc_gate_runs.sh D1|D2|D3|D4
set -e
cd "$(dirname "$0")/.."
B=./build/gd_dc_gate
R=results/dcgate
mkdir -p $R
VS=0.001,0.003,0.01,0.03,0.1,0.2,0.3
stage=$1
case $stage in
  D1)  # controls: straight channel and mirror-symmetric constriction
    rm -f $R/D1.csv
    for set in 300K 100K; do
      $B --geo chan --set $set --hw 50 --hm 50 --v $VS --out $R/D1.csv
      $B --geo symcon --set $set --hw 20 --hm 120 --v $VS --out $R/D1.csv
    done ;;
  D2)  # funnels, both wall conditions
    rm -f $R/D2.csv
    for set in 300K 100K; do for slip in inf 0; do
      $B --geo funnel --set $set --slip $slip --hw 20 --hm 120 --v $VS --out $R/D2.csv
      $B --geo funnel_w1000 --set $set --slip $slip --hw 80 --hm 480 --v $VS --out $R/D2.csv
    done; done ;;
  D3)  # classical Tesla valve, slip walls (175 nm channel and its 1 um copy)
    rm -f $R/D3.csv
    for set in 300K 100K; do
      $B --geo tesla_classic --set $set --hw 20 --hm 120 --v $VS --out $R/D3.csv
      $B --geo tesla_classic_w1000 --set $set --hw 114 --hm 686 --v $VS --out $R/D3.csv
    done ;;
  D4)  # mesh convergence of the headline points
    rm -f $R/D4.csv
    for m in "28 168" "20 120" "14 84"; do set -- ${=m}
      $B --geo funnel --set 300K --hw $1 --hm $2 --v 0.01,0.03 --out $R/D4.csv
    done
    for m in "20 120" "14 84" "10 60"; do set -- ${=m}
      $B --geo tesla_classic --set 300K --hw $1 --hm $2 --v 0.01,0.03 --out $R/D4.csv
    done
    for m in "20 120" "14 84"; do set -- ${=m}
      $B --geo symcon --set 300K --hw $1 --hm $2 --v 0.03 --out $R/D4.csv
    done ;;
  *) echo "unknown stage $stage"; exit 2 ;;
esac
