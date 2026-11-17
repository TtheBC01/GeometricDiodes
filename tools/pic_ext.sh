#!/bin/zsh
# Stage B8 extension runs (plans/ballistic_selfconsistent.md section 5, P5
# and P6). G3 is the 0.55 um funnel; shape-only bias and drive throughout.
# Numerical settings as in the core set (tools/pic_runs.sh); the 1 THz runs
# average over 20 periods instead of 10, because the currents are smaller.
# P9 and P9c are Stage B9 (plans/ballistic_edges.md): walls of a given rms
# roughness, whose specularity depends on the angle of the hit.
# P10* and P7* are Stage B10 (plans/ballistic_contacts.md): contacts that
# reflect part of the electrons, and the 6 um Device D with a computed field.
# usage: tools/pic_ext.sh P5|P6|P6c|P8|P9c|P9|P9s|P10c|P10t|P10k|P10l|P10n|P10s|P10r|P7p|P7|P7n|P7x ...
#        (results/bpic/<stage>.csv, ext.log)
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
# Device D of Nguyen et al. (the shape of the first stage), static, on a 7.5 nm grid
dev() {   # dev <csv> <V> <N> [extra flags]
  local out=$1 V=$2 N=$3; shift 3
  $B --Wc 2000 --Lc 2000 --alpha 30 --Wn 175 --Wd 2000 --Ld 2420 --f 0 --V $V --d 300 --mode sym --h 7.5 --N $N --settle 15000 --period 1000 --nper 15 --label D --seed 5 \
     --out $R/$out "$@" | tee -a $R/ext.log
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
  P8)  # wall specularity between 0.5 and 1 (after the plan): 1 THz, 700 nm
    rm -f $R/P8.csv
    for p in 0.9 0.8 0.7; do for d in 300 30; do
      ac P8.csv 0.1 $d 20 --lambda 700 --p $p
    done; done ;;
  P9c) # controls of Stage B9, run first: the mirror-symmetric funnel with rough
       # walls (must be zero), the funnel with rough walls and no drive (must be
       # zero), and one point of P8 repeated with the Stage B9 binary
    rm -f $R/P9c.csv
    for d in 300 30; do
      ac P9c.csv 0.1 $d 20 --lambda 700 --rough 2.4 --geo symmetric --label control
    done
    ac P9c.csv 0 30 20 --lambda 700 --rough 2.4 --label nodrive
    ac P9c.csv 0.1 300 20 --lambda 700 --p 0.9 --label repeat ;;
  P9)  # rms edge roughness: head-on specularity 0.75, 0.5, 0.35, 0.2 at the
       # Fermi wave vector (the plan), then 0.8 and 0.05 (added after the reading)
    rm -f $R/P9.csv
    for hr in 1.3 2.0 2.4 3.0 1.1 4.1; do for d in 300 30; do
      ac P9.csv 0.1 $d 20 --lambda 700 --rough $hr
    done; done ;;
  P9s) # second seed for the roughness that fell on the 0.15 uA line of the
       # plan's decision table (2.4 nm, head-on specularity 0.36)
    rm -f $R/P9s.csv
    for d in 300 30; do ac P9s.csv 0.1 $d 20 --lambda 700 --rough 2.4 --seed 11; done ;;
  P10c) # controls of Stage B10, run first: the mirror-symmetric funnel between
        # contacts of transparency 0.25 (must be zero) and the funnel with no
        # drive behind the 38 nm junction and behind contacts of transparency
        # 0.25 (must be zero; the second was added after test T32d)
    rm -f $R/P10c.csv
    for d in 300 30; do
      ac P10c.csv 0.1 $d 20 --lambda 700 --ct 0.25 --settle 10000 --geo symmetric --label control
    done
    ac P10c.csv 0 30 20 --lambda 700 --cjunction 38 --settle 10000 --label nodrive
    ac P10c.csv 0 30 20 --lambda 700 --ct 0.25 --settle 10000 --label nodrive ;;
  P10t) # contact transparency (a contact resistance of 81, 145, 338 ohm um)
    rm -f $R/P10t.csv
    for T in 0.75 0.5 0.25; do for d in 300 30; do
      s=3000; [[ $T == 0.25 ]] && s=10000
      ac P10t.csv 0.1 $d 20 --lambda 700 --ct $T --settle $s
    done; done ;;
  P10k) # collimating contacts with the strip conductance of transparency 0.5 and 0.25
    rm -f $R/P10k.csv
    for dj in 7 38; do for d in 300 30; do
      s=3000; [[ $dj == 38 ]] && s=10000
      ac P10k.csv 0.1 $d 20 --lambda 700 --cjunction $dj --settle $s
    done; done ;;
  P10l) # ideal contacts moved out: leads of 300 nm on both sides
    rm -f $R/P10l.csv
    for d in 300 30; do
      ac P10l.csv 0.1 $d 20 --lambda 700 --Lc 300 --Ld 300 --N 16000000 --label G3L
    done ;;
  P10n) # particle-number check where the electrons stay longest
    rm -f $R/P10n.csv
    ac P10n.csv 0.1 300 20 --lambda 700 --ct 0.25 --settle 10000 --N 16000000 ;;
  P10r) # second seed where the current came out above the series-resistor value
       # (plan, section 7, outcome 4); transparency 0.5 also with the longer settling time
    rm -f $R/P10r.csv
    for T in 0.5 0.25; do for d in 300 30; do
      ac P10r.csv 0.1 $d 20 --lambda 700 --ct $T --settle 10000 --seed 11
    done; done ;;
  P10s) # static +-0.1 V behind reflecting contacts, with potential maps
    rm -f $R/P10s.csv
    for T in 0.5 0.25; do for V in 0.1 -0.1; do
      st P10s.csv $V 300 --lambda 700 --ct $T --settle 8000 --map $R/P10s_T${T}_V$V
    done; done ;;
  P7p)  # pilot: 2 ps of Device D, for the time per step and the memory
    rm -f $R/P7p.csv
    dev P7p.csv 0.1 50000000 --lambda 700 --settle 1000 --nper 1 ;;
  P7)   # Device D, 700 nm, +-0.1 V, with potential maps
    rm -f $R/P7.csv
    for V in 0.1 -0.1; do dev P7.csv $V 50000000 --lambda 700 --map $R/P7_V$V; done ;;
  P7n)  # the same with half the particles
    rm -f $R/P7n.csv
    for V in 0.1 -0.1; do dev P7n.csv $V 25000000 --lambda 700; done ;;
  P7x)  # conditional: no scattering
    rm -f $R/P7x.csv
    for V in 0.1 -0.1; do dev P7x.csv $V 50000000 --lambda inf; done ;;
esac
done
echo "done $*" >> $R/ext.log
