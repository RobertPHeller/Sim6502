# Sim6502

This is a simple basic 6502 simulator, coded in C++.

Sim6502 is a simple basic 6502 simulator, written in C++. It compiles and
runs under Linux on a Raspberry Pi 5, but should compile on any Linux
system and will probably compile on any operating system.
 
The program will execute the supplied program contimiously until interupted
with a ^C (SIGINT) at which point if the -d flag was specified, it will dump
first 1024 bytes of ram and exit. 
 
Sim6502 [-m ramsize] [-r romsize] [-d] hexfile

## Options:

- -m ramsize Set the size of the ram to allocate.  Default is 2048.
- -r romsize Set the size of the rom to allocate.  Default is 2048.
- -d Dump the first 1024 bytes of ram on program executation.
 
The hexfile parameter is a file containing a 6502 machine program in Intel Hex
record format.
