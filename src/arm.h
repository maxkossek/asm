#ifndef ARM_H
#define ARM_H

/* APSR Flags: r[16]: [N, Z, C, V]. */
#define NFLAG ~(~0u >> 1)
#define ZFLAG ~(~0u >> 2)
#define CFLAG ~(~0u >> 4)
#define VFLAG ~(~0u >> 8)
#define UFLAG ~(~0u >> 16)
#define ADDRSPACE_SIZE 1000

/* Special registers. */
#define SP 13
#define LR 14
#define PC 15
#define APSR 16
#define REGCOUNT 17

#endif
