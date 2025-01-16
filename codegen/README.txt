This directory contains code-generation python scripts which will generate the
z80.h, m6502.h and mc6800.h headers.

In a bash compatible shell run:

```sh
./z80_gen.sh
./m6502_gen.sh
./mc6800_gen.py
```

This will run Python3 inside a virtual environment and read/write the `chips/z80.h`, 
`chips/m6502.h` and `chips/mc6800.h` headers.
