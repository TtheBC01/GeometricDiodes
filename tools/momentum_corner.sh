#!/bin/sh
# Corner and viscosity check for the 1 um funnel (paper/review_seventh_draft.md, section 3.3).
# Without viscosity the time-averaged equations of the Dirac gas are those of the classical gas with the
# dc current and voltage halved, in any geometry; the small-signal solver gives that for a channel and
# not for the funnel. This script varies the wall mesh (80, 40, 20 nm), the viscosity (production, a
# fifth of it, zero), the gas (Dirac, classical) and the treatment of wall corners (pinned, the default;
# or free to slide, --corner_deg 181) at 0.1, 0.3 and 1 THz. Outputs results/thz2/corner/K_*.csv.
# At most three jobs at a time by default: the 20 nm mesh has 100 000 nodes.
set -e
cd "$(dirname "$0")/.."
S=./build/gd_thz_sweep; O=results/thz2/corner; P=${JOBS:-3}; F=1e11,3e11,1e12
mkdir -p $O
# make the meshes one at a time before the parallel runs
for h in 80 40 20; do [ -f meshes/h_funnel_w1000_hw${h}_hm$((6 * h)).mesh ] && continue; $S --geo funnel_w1000 --set 300K --term dss --hw $h --hm $((6 * h)) --f 1e11 --Rc 0 > $O/mesh_hw$h.txt 2>&1 || true; [ "$1" = "meshes" ] && [ $h = 40 ] && exit 0; done
job() {  # set term closure hw nutag corner
  set_=$1; term=$2; clos=$3; h=$4; nut=$5; cor=$6
  case "$nut:$set_" in
    prod:*) NU="" ;;
    fifth:100K) NU="--nu 0.02" ;;
    fifth:300K) NU="--nu 0.006" ;;
    zero:*) NU="--nu 0 --tauee 0" ;;
  esac
  [ $cor = free ] && C="--corner_deg 181" || C=""
  name=K_${set_}_${term}_${clos}_hw${h}_${nut}_${cor}
  echo "$S --geo funnel_w1000 --set $set_ --term $term --closure $clos --hw $h --hm $((6 * h)) --f $F $NU $C --out $O/$name.csv > $O/$name.txt 2>&1"
}
{
  for h in 80 40 20; do
    for term in dss sym; do for cor in pin free; do
      for set_ in 100K 300K; do job $set_ $term graphene $h prod $cor; done          # A: production viscosity
      for clos in graphene sw; do job 100K $term $clos $h zero $cor; done            # D: no viscosity
      if [ $h != 20 ]; then
        job 100K $term sw $h prod $cor                                               # B: classical gas, production
        for clos in graphene sw; do job 100K $term $clos $h fifth $cor; done         # C: a fifth of the viscosity
        job 300K $term graphene $h fifth $cor
      fi
    done; done
  done
} | xargs -S 4096 -P $P -I{} sh -c '{}'     # -S: the BSD xargs default of 255 bytes is too short for these lines
echo "corner check finished"
