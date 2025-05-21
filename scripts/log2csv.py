#!/usr/bin/env python3
"""
run.log  ->  run.csv
CSV: sec,floor,apt,step
Strips ANSI color codes before parsing.
"""

import re
import sys
import csv

# -------- Regex'ler --------
ANSI_RE = re.compile(r'\x1b\[[0-9;]*m')          # renk kodu

LOG_RE = re.compile(
    r'\[(\d\d):(\d\d)] '
    r'Kat-(\d+) Daire-(\d+) ➜ (.+?)(?: \(|$)'
)


def parse_line(raw):
    line = ANSI_RE.sub('', raw).strip()          # renkleri at
    m = LOG_RE.match(line)
    if not m:
        return None
    mm, ss, fl, ap, step = m.groups()
    return int(mm)*60+int(ss), int(fl), int(ap), step.strip()


def convert(inp_path, out_path):
    with open(inp_path) as fin, open(out_path, 'w', newline='') as fout:
        w = csv.writer(fout)
        w.writerow(['sec', 'floor', 'apt', 'step'])
        for ln in fin:
            row = parse_line(ln)
            if row:
                w.writerow(row)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: log2csv.py run.log run.csv")
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
    print("✓ CSV written:", sys.argv[2])
