This directory contains code-generation python scripts which will generate the
z80.h, m6502.h and mc6800.h headers.

First install pyyaml:

pip3 install pyyaml

Then run:

> python3 z80_gen.py
> python3 m6502_gen.py
> python3 mc6800_gen.py

To generate the respective decoder source files in the '../chips' directory.

