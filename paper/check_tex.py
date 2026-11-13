"""Structural check of the manuscript source when no TeX toolchain is
available: balanced braces and environments, math delimiters, resolved
labels, figure files present, citation keys present in refs.bib.
usage: python3 paper/check_tex.py"""
import re, os, sys
here = os.path.dirname(os.path.abspath(__file__))

def expand(path):
    s = open(path).read()
    s = re.sub(r"(?m)(?<!\\)%.*$", "", s)
    return re.sub(r"\\input\{([^}]+)\}", lambda m: expand(os.path.join(here, m.group(1) + ".tex")), s)

bad = 0
bib = "".join(open(os.path.join(here, b)).read() for b in ("refs.bib", "refs_detectors.bib", "refs_prior.bib") if os.path.exists(os.path.join(here, b)))
bibkeys = set(re.findall(r"@\w+\{([^,\s]+),", bib))
for name in ("main.tex", "supplement.tex"):
    s = expand(os.path.join(here, name))
    depth = 0
    for i, ch in enumerate(s):
        if ch == "{" and (i == 0 or s[i - 1] != "\\"): depth += 1
        if ch == "}" and (i == 0 or s[i - 1] != "\\"): depth -= 1
        if depth < 0: print(name, "unbalanced } near:", s[max(0, i - 60):i + 10].replace("\n", " ")); bad += 1; break
    if depth != 0: print(name, "brace depth at end:", depth); bad += 1
    stack = []
    for m in re.finditer(r"\\(begin|end)\{([^}]+)\}", s):
        if m.group(1) == "begin": stack.append(m.group(2))
        elif not stack or stack.pop() != m.group(2): print(name, "environment mismatch at", m.group(0)); bad += 1
    if stack: print(name, "unclosed environments:", stack); bad += 1
    body = re.sub(r"\\\$", "", s)
    for para in re.split(r"\n\s*\n", body):
        if para.count("$") % 2: print(name, "odd number of $ in paragraph:", para[:90].replace("\n", " ")); bad += 1
    labels = set(re.findall(r"\\label\{([^}]+)\}", s)); refs = set(re.findall(r"\\(?:eq)?ref\{([^}]+)\}", s))
    for r in sorted(refs - labels): print(name, "undefined ref:", r); bad += 1
    for l in sorted(labels - refs): print(name, "label never referenced:", l)
    for g in re.findall(r"\\includegraphics(?:\[[^\]]*\])?\{([^}]+)\}", s):
        g = g.replace("\\figpath", "figs")
        if not os.path.exists(os.path.join(here, g)): print(name, "missing figure:", g); bad += 1
    cites = set(k.strip() for c in re.findall(r"\\cite\{([^}]+)\}", s) for k in c.split(","))
    missing = sorted(cites - bibkeys)
    print(f"{name}: {len(s.split())} words, {len(labels)} labels, {len(cites)} cite keys, {len(missing)} not in refs.bib")
    if missing: print("   missing:", " ".join(missing)); bad += bool(bibkeys) * len(missing)
    for tok in ("%%", "TODO", "LADDER_", "RESULTS_SECTION"):
        if tok in open(os.path.join(here, name)).read() or any(tok in open(os.path.join(here, "sections", f)).read() for f in os.listdir(os.path.join(here, "sections"))):
            print(name, "placeholder token present:", tok)
print("problems:", bad)
sys.exit(1 if bad else 0)
