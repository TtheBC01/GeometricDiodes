#!/bin/sh
# Stage H6, check T33 (plans/hydro_momentum.md, section 4): with --closure legacy the three
# fluid apps must give the numbers stored before the momentum equation was corrected.
# Outputs results/thz2/T33_*.csv and the comparison results/thz2/ladder_T33.txt.
set -e
cd "$(dirname "$0")/.."
O=results/thz2
F3=$(.venv/bin/python -c "print(','.join(repr(1e10 * 300.0 ** (i / 24)) for i in (12, 14, 19)))")
./build/gd_thz_sweep --geo chan --set 100K --term ds --hw 50 --hm 50 --closure legacy --out $O/T33_chan_100K_ds.csv > $O/T33_chan_100K_ds.txt 2>&1 &
./build/gd_thz_sweep --geo funnel_w1000 --set 100K --term dss --hw 80 --hm 480 --closure legacy --f $F3 --out $O/T33_funnel_w1000_100K_dss.csv > $O/T33_funnel_w1000_100K_dss.txt 2>&1 &
./build/gd_thz_sweep --geo funnel_w1000 --set 100K --term sym --hw 80 --hm 480 --closure legacy --f $F3 --out $O/T33_funnel_w1000_100K_sym.csv > $O/T33_funnel_w1000_100K_sym.txt 2>&1 &
./build/gd_thz_power --geo funnel_w1000 --set 100K --term dss --f 3e11 --Va 0.01,0.3 --closure legacy --out $O/T33_power_100K_dss_f3e11.csv > $O/T33_power_100K_dss_f3e11.txt 2>&1 &
rm -f $O/T33_dcgate.csv
./build/gd_dc_gate --geo funnel --set 300K --slip inf --hw 20 --hm 120 --v 0.01,0.1 --closure legacy --out $O/T33_dcgate.csv > $O/T33_dcgate.txt 2>&1 &
wait
.venv/bin/python tools/momentum_regress.py | tee $O/ladder_T33.txt
