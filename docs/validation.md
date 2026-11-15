# Validation and remaining work

The project was tested in stages: bare-metal register access, Linux boot, kernel MMIO, character-device access, and built-in driver registration. The lab reports and screenshots document those stages.

## Recorded results

- Lab 3 UART output shows operand/product examples and the application reaching the end of its loop.
- Lab 4 shows Linux booting to a login prompt.
- Lab 5 shows the kernel module writing 7 and 2 and reading 14 from the product register.
- Lab 6 shows module registration and part of the interactive character-device test.
- Lab 7 shows driver registration among kernel boot messages and part of the application test.

The screenshots do not show a complete run of the current automated test. Its expected success message is `289 cases: 289 passed, 0 failed`; a new board run is still needed to establish that result.

## Software and source checks

The userspace test compiles with GCC using `-std=c11 -Wall -Wextra -Werror -O2`. Its missing-device path reports an error and returns a nonzero status.

Run `python tools/check_sources.py` from the repository root to check IP file references, the block design's device/version and IP identity, the `0x43C00000`/64 KiB address mapping, and consistency between the loadable and built-in driver sources. This is a source consistency check, not a hardware simulation.

## Image-size comparison

The Lab 7 screenshot lists image.ub sizes of 17,616,680 and 16,514,804 bytes. The difference is 1,101,876 bytes, about 1.05 MiB. The report's approximately 1 KB statement is incorrect. These are complete image sizes, so the comparison does not isolate the size of kernel code or the effect of each configuration option.

## Checks still needed

1. Build the block design and project script in Vivado 2023.1.
2. Complete the PetaLinux project configuration and device tree, and identify the exact target kernel revision.
3. Build and boot the supplied Lab 7 integration against that kernel.
4. Run the automated 289-pair test and record its output and exit status.
5. Test all operand byte lanes, result truncation, and pipeline timing.
6. Add transaction ownership or locking before supporting multiple clients.

The hardware has byte write strobes, but the complete processor/interconnect/driver path still needs board testing. The driver performs byte-wise reads and writes, ignores file offsets, and does not reserve the MMIO region. Avoid competing drivers and clients.

For each new hardware test, record the source revision, kernel version, image/bitstream identity, serial output, and test exit status. No Vivado/PetaLinux build or new board test was performed as part of the documentation updates.
