# Custom AXI4-Lite Peripheral: FPGA-to-Linux Driver Co-Design

Hardware/software co-design on the **Digilent ZYBO Z7-10**, connecting a custom FPGA multiplier to the **Zynq-7000 ARM Cortex-A9** through AXI4-Lite.

The project progresses from Vivado RTL and IP integration to bare-metal C validation, PetaLinux boot, and Linux kernel-space register access.

## Architecture

```text
ARM Cortex-A9 (Zynq Processing System)
    |
    | memory-mapped AXI4-Lite reads / writes
    v
Custom multiplier (Programmable Logic)
    +-- operand A    offset 0x00
    +-- operand B    offset 0x04
    +-- product      offset 0x08
```

The Linux module maps the peripheral with `ioremap()`, writes operands using `iowrite32()`, reads the result using `ioread32()`, and releases the mapping using `iounmap()`. The physical base address must come from the matching hardware export; it is not specified by these offsets.

## Project stages and evidence

| Stage | Work documented | Report |
| --- | --- | --- |
| FPGA foundations | Verilog switch/LED logic, counter, and jackpot game | [Lab 1](Medha-Venkata-Subrahmanya-Aditya-Vellampalli_Lab1_749_Sec605.pdf) |
| Processor-controlled I/O | MicroBlaze, AXI GPIO, and Vitis C applications | [Lab 2](Medha-Venkata-Subrahmanya-Aditya-Vellampalli_Lab2_749_Sec605.pdf) |
| Custom AXI4-Lite IP | Three-register multiplier integrated with the Zynq PS; bare-metal register validation over UART | [Lab 3](Medha-Venkata-Subrahmanya-Aditya-Vellampalli_Lab3_749_Sec605.pdf) |
| Embedded Linux bring-up | Vivado XSA export, PetaLinux image generation, and SD-card boot | [Lab 4](Medha-Venkata-Subrahmanya-Aditya-Vellampalli_Lab4_749_Sec605.pdf) |
| Kernel-space MMIO | Loadable hello and multiply modules; register access checked through kernel logs | [Lab 5](Medha-Venkata-Subrahmanya-Aditya-Vellampalli_Lab5_749_Sec605.pdf) |

Lab 2 uses MicroBlaze as an introductory stage. The custom multiplier and Linux stages use the Zynq Processing System.

## Register interface

| Offset | Register | Purpose |
| --- | --- | --- |
| `0x00` | `slv_reg0` | 32-bit operand A |
| `0x04` | `slv_reg1` | 32-bit operand B |
| `0x08` | `slv_reg2` | 32-bit product result |

A full product of two 32-bit operands may require 64 bits. The documented three-register interface cannot expose the entire product when it exceeds 32 bits. Lab 3 discusses an additional upper-result register as a possible extension; it is not part of the documented interface.

## Development flow

1. Build and package the multiplier IP in Vivado, connect it to the Zynq Processing System, validate the block design, and generate the bitstream.
2. Export the hardware platform as an XSA and use the matching platform in Vitis.
3. Write operands and read the result with the packaged IP register-access macros; inspect UART output.
4. Configure PetaLinux from the hardware export and build the Linux image.
5. Package the boot artifacts and boot the board from SD. Lab 4 documents `BOOT.BIN`, `image.ub`, and `boot.scr`, with the serial console at 115200 baud.
6. Build the kernel modules for the target kernel, load them on the board, and inspect `dmesg` and `lsmod`.

These steps describe the reported workflow. A reproducible build requires the original RTL, IP metadata, hardware configuration, software sources, and tool versions, which are not yet included in this repository.

## Validation and repository status

The existing reports document bare-metal UART validation, successful Linux boot, and kernel-module register access. Lab 5 records an operand example of 7 and 2.

The author's resume additionally describes:

- A Linux character driver using `get_user()` / `put_user()`.
- A complete 289-case operand sweep.
- Built-in kernel integration using Kconfig and Makefile entries, with registration at boot.

The imported Lab 6 character driver uses `get_user()` / `put_user()`, and its userspace test covers the 289 operand pairs. The repository test now checks operand readback, products, transfer lengths, and returns failure on errors. Hardware execution of this updated test remains pending. LAB7 supplied only `image.ub`; built-in integration source and boot-registration evidence remain pending. No hardware tests or Vivado/PetaLinux builds have been rerun as part of this documentation update.

## Tools and platform

- Digilent ZYBO Z7-10; Zynq-7000 `xc7z010clg400-1`
- Verilog, C, AXI4-Lite, memory-mapped I/O
- AMD/Xilinx Vivado, Vitis, and PetaLinux
- Linux kernel modules and UART serial debugging with picocom

## License

See [LICENSE](LICENSE).

## Source layout and usage

- `software/baremetal/multiply_test/`: original Vitis multiplier application and platform helpers. Create a standalone Cortex-A9 application using the matching XSA and BSP, including the custom multiply IP driver, then import these sources.
- `software/linux/mmio/`: original Lab 5 hello and multiplier modules with supplied hardware headers.
- `software/linux/chardev/`: original Lab 6 character driver with an added external-module Makefile.
- `software/linux/userspace/devtest.c`: revised noninteractive 289-case test, based on the supplied Lab 6 test.
- `hardware/constraints/`: introductory GPIO design constraints; these do not replace a multiplier block design.
- `hardware/board/`: supplied ZYBO processing-system preset, not a complete hardware recreation script.
- `examples/gpio/`: original MicroBlaze GPIO applications.

Build either Linux module directory against the matching configured target kernel with `make KDIR=/path/to/kernel/build ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-`. Use the cross compiler supplied by your PetaLinux SDK when its prefix differs. Do not load the MMIO and character-driver multiplier modules simultaneously.

The character driver assumes physical base `0x43C00000` and span `0x10000`; verify these against the deployed hardware before loading. After `insmod multiply.ko`, obtain the dynamic major number from `dmesg` or `/proc/devices`, then create `/dev/multiplier` using `mknod /dev/multiplier c <major> 0` as root. The driver does not create the device node automatically.

Compile the test for the target with `arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -O2 software/linux/userspace/devtest.c -o devtest`, copy it to the board, and run `./devtest`. An optional first argument overrides the device path. The API writes eight little-endian operand bytes and reads twelve bytes containing both operands and the product. It uses snapshot semantics, ignoring file offsets. Run with one client: the original driver has no transaction locking across write/read calls. Its byte-wise MMIO behavior still requires validation against the actual AXI slave.

Still needed for a reproducible full build: custom multiplier RTL and IP metadata, the Vivado block design or exported recreation Tcl, PetaLinux project configuration/recipes, and the actual Lab 7 Kconfig/Makefile/kernel source changes. Supplied boot images and compiled modules are excluded from source control. Original source copyright notices are preserved; consult them for third-party file terms.
