# Paper

Manuscript draft for the results of this repository. Outline and decisions:
`outline.md`.

| file | content |
|---|---|
| `main.tex`, `sections/*.tex` | the article (REVTeX 4.2, Physical Review Applied style) |
| `supplement.tex` | supplemental material: acceptance ladders, convergence checks, run parameters |
| `refs.bib`, `refs_verification.md` | bibliography and the record of how each entry and claim was checked against its source |
| `refs_detectors.bib` | records for the detector papers, fetched from the Crossref API |
| `literature_detectors.md`, `literature_detectors.csv` | measured graphene THz detectors used in Table III and Fig. 8(b) |
| `make_figures.py`, `figstyle.py` | all eight figures, drawn from `results/` |
| `figs/` | figure output (PDF for the manuscript, PNG for viewing) |
| `check_tex.py` | structural check of the sources (braces, environments, labels, figures, citation keys) |

Build:

    .venv/bin/python paper/make_figures.py          # all figures; or: make_figures.py 4 7
    python3 paper/check_tex.py                       # structural check, no TeX needed
    cd paper && latexmk -pdf main.tex && latexmk -pdf supplement.tex

The draft was written on a machine without a TeX installation, so it has
been checked structurally but **not compiled**. `brew install tectonic` (then
`tectonic main.tex`) or MacTeX provides a toolchain.

Every number in the text comes from `results/*.md`; the closing checks added
for the paper are in `results/checks/` and `results/bthz/B6.csv`.
