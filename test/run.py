# run.py wobble config time

import subprocess
from sys import argv, exit
import time
from pathlib import Path

cfg = Path(argv[2])
tolerance = 0.05

for i in range(1, 5):
    (cfg / f"job{i}.out").unlink(missing_ok=True)

proc = subprocess.Popen([argv[1], str(cfg / "config.json")])
time.sleep(float(argv[3]))
proc.terminate()

# compare results
for i in range(1, 5):
    with open(cfg / f"job{i}.out") as f:
        cur = int(f.read().strip())
    with open(cfg / f"job{i}.exp") as f:
        exp = int(f.read().strip())
    if abs(exp - cur) / exp > tolerance:
        print(f"job{i}.out: {cur}, job{i}.exp: {exp}")
        exit(1)
