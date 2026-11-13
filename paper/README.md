# Paper

Manuscript draft for the results of this repository. Outline and decisions:
`outline.md`.

| file | content |
|---|---|
| `main.tex`, `sections/*.tex` | the article (REVTeX 4.2, Physical Review Applied style) |
| `supplement.tex` | supplemental material: acceptance ladders, convergence checks, run parameters |
| `refs.bib`, `refs_verification.md` | bibliography and the record of how each entry and claim was checked against its source |
| `refs_detectors.bib` | records for the detector papers, fetched from the Crossref API |
| `refs_prior.bib` | the 24 references added after the prior-work search, fetched from doi.org and the arXiv API |
| `prior_work.md` | the prior-work search of 2026-11-10 and what was done about it |
| `literature_detectors.md`, `literature_detectors.csv` | measured graphene THz detectors used in Table IV and Fig. 11(b) |
| `make_figures.py`, `figstyle.py` | all eleven figures, drawn from `results/`. File numbers are the order of creation: `fig9_gated_dc` is Fig. 4 of the paper, `fig10_ballistic_length` is Fig. 9, `fig11_selfconsistent` is Fig. 10 and `fig8_synthesis` is Fig. 11 |
| `figs/` | figure output (PDF for the manuscript, PNG for viewing) |
| `check_tex.py` | structural check of the sources (braces, environments, labels, figures, citation keys) |
| `main.pdf`, `supplement.pdf` | the compiled draft |

Build:

    .venv/bin/python paper/make_figures.py          # all figures; or: make_figures.py 4 7
    python3 paper/check_tex.py                       # structural check, no TeX needed
    cd paper && tectonic -o build main.tex && tectonic -o build supplement.tex

Both documents compile cleanly with Tectonic 0.17 (`brew install tectonic`):
no undefined references or citations, no overfull boxes; the main article is
26 pages in two-column REVTeX including references and appendices, the
supplement 6. `main.pdf` and `supplement.pdf` in this folder are copies of
the last build; `build/` is ignored by git. `latexmk -pdf` with TeX Live
works as well.

Every number in the text comes from `results/*.md`; the closing checks added
for the paper are in `results/checks/` and `results/bthz/B6.csv`. The second
draft (2026-11-11) adds Stages B7 and H5 (`results/BALLISTIC_SHORT.md`,
`results/HYDRO_GATE.md`) and the prior-work citations. The third draft
(2026-11-12) adds Stage B8 (`results/BALLISTIC_SELFCONSISTENT.md`): the
short funnel with the field computed, which replaces the imposed-field
figure of 0.70 A/W by 0.12 A/W.
