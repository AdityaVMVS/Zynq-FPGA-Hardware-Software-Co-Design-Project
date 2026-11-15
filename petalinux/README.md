# PetaLinux configuration

PetaLinux builds the Linux software that runs on the Zynq processor. It uses the hardware platform exported from Vivado to prepare the bootloader, device tree, kernel, and root filesystem.

The lab boot screenshot shows PetaLinux `2023.1+release-S05010539`. The complete project configuration and device-tree source are not included, so this folder is not a ready-to-build PetaLinux project.

## Module recipe

`reference/multiply.bb.txt` is a recipe draft for building a kernel module. A BitBake recipe lists the sources, build steps, and package information. The `inherit module` line selects the kernel-module build support.

Before using this draft:

1. Make the source name in `SRC_URI` agree with the driver filename. The draft requests `multiplier.c`, while the loadable module source is `multiply.c`.
2. Include the module Makefile in the recipe's source files.
3. Check the license metadata: the draft says `CLOSED`, while the driver declares GPL. Keep the correct source license and recipe checksums.
4. Build against the same kernel configuration used on the board.

The loadable module can also be built using the Makefile in `software/linux/chardev/` and a matching configured kernel tree. See the [main instructions](../README.md#build-and-run).

## Boot files

The SD-card flow uses `BOOT.BIN`, `image.ub`, and `boot.scr`. These files are generated during the hardware/Linux build and are excluded from this repository. Keep the bitstream, hardware export, kernel, and device tree consistent when rebuilding them.
