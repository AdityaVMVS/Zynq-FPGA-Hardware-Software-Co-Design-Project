# Custom AXI4-Lite Peripheral: FPGA-to-Linux Driver Co-Design

This project connects a custom multiplier built inside an FPGA to software running on an ARM processor. It follows the same hardware through three ways of using it: a small C program without an operating system, a Linux kernel module, and a Linux character device that an ordinary application can open.

The target is the **Digilent ZYBO Z7-10**, a development board containing a **Zynq-7000** chip. The chip combines a processor and programmable hardware. The project demonstrates how those two parts communicate, and how a hardware function becomes accessible through Linux.

**Repository status:** the original multiplier hardware, packaged IP driver, Vivado block design, and application sources are available. Historical screenshots show multiplication, Linux boot, and driver registration. The complete PetaLinux project and original Lab 7 kernel changes are missing. The Lab 7 integration files here are explicitly reconstructed. The updated automated test compiles on the host; it has not been rerun on the board.

## Contents

- [Start with the basic ideas](#start-with-the-basic-ideas)
- [Follow one multiplication](#follow-one-multiplication)
- [Understand the hardware interface](#understand-the-hardware-interface)
- [Explore the repository](#explore-the-repository)
- [Follow the development stages](#follow-the-development-stages)
- [Build and run](#build-and-run)
- [Understand the Linux driver](#understand-the-linux-driver)
- [Review validation and limitations](#review-validation-and-limitations)
- [Troubleshooting](#troubleshooting)
- [License and attribution](#license-and-attribution)

## Start with the basic ideas

A processor executes software instructions. An **FPGA** is programmable hardware: a design describes logic circuits and registers that the development tools implement in the chip. Here, the custom circuit multiplies two numbers.

The Zynq chip has two main parts:

| Term | Meaning in this project |
| --- | --- |
| Processing System (PS) | The ARM Cortex-A9 processor and its supporting hardware; it runs the C application or Linux. |
| Programmable Logic (PL) | The FPGA area containing the custom multiplier. |
| RTL | Register-transfer-level source, written here in Verilog, describing the hardware's registers and behavior. |
| IP core | A reusable hardware component. The multiplier is packaged with metadata so Vivado can add it to a design. |
| AXI4-Lite | A bus interface used by the processor to read and write the multiplier's control/data registers. |
| Register | A small hardware storage location. Software writes the input numbers to registers and reads the answer from another register. |
| Memory-mapped I/O (MMIO) | Accessing hardware registers through assigned addresses. These addresses refer to hardware, rather than ordinary RAM. |
| Driver | Kernel software that handles access to the hardware on behalf of applications. |
| Character device | A Linux interface accessed through file operations such as open, read, write, and close. Here it is `/dev/multiplier`. |
| Bare metal | Running a program directly on the processor without Linux. |
| Device tree | A description of hardware provided to Linux. The original project-specific device-tree source is missing. |

The goal is to demonstrate the complete hardware/software interface. This repository does not establish that multiplying through system calls is faster than using a processor instruction.

## Follow one multiplication

For example, to calculate **7 × 2 = 14** under Linux:

1. The application opens `/dev/multiplier`.
2. It sends eight bytes containing two 32-bit numbers: 7 and 2.
3. The character driver copies those bytes from the application and writes them to the multiplier's registers.
4. The FPGA logic computes the product.
5. The application reads twelve bytes: the two input numbers followed by the product.
6. It decodes the bytes and compares the answer with the expected value.

```text
Application: devtest.c
        |  open / write / read / close
        v
Linux character device: /dev/multiplier
        |
        v
Kernel driver: multiply.c
        |  memory-mapped AXI4-Lite transactions
        v
FPGA multiplier
        |-- register 0: input A
        |-- register 1: input B
        |-- register 2: product
        `-- register 3: scratch storage
```

The bare-metal application accesses the same kind of registers directly through the packaged IP access macros. The Lab 5 module performs a fixed example in kernel space. The Lab 6 character driver adds the application-facing file interface. Lab 7 describes linking that driver into the kernel so initialization happens during boot.

## Understand the hardware interface

The recovered block design assigns the peripheral a physical address window from **`0x43C00000` through `0x43C0FFFF`**, a total of **64 KiB**. The supplied software header and character-driver constants agree with this mapping.

An **offset** is a distance from the base address. For example, the product register is at base plus 8: `0x43C00008`.

| Offset | Register | Access | Meaning |
| --- | --- | --- | --- |
| `0x00` | `slv_reg0` | Read/write | Unsigned 32-bit operand A |
| `0x04` | `slv_reg1` | Read/write | Unsigned 32-bit operand B |
| `0x08` | `slv_reg2` | Read-only result | Low 32 bits of A × B; writes have no effect |
| `0x0c` | `slv_reg3` | Read/write | Independent scratch register; not the upper product |

Although the address window is 64 KiB, the RTL selects registers using only address bits [3:2]. The 16-byte register bank therefore repeats within that window. Software should use the defined offsets.

Multiplying two 32-bit numbers can require a 64-bit answer. This design keeps only the low 32 bits, so large products are truncated. Its result also passes through two registered stages after an operand update. There is no ready/done register. Byte writes are supported through AXI write strobes, which identify the bytes to update. Processor/interconnect byte-lane behavior and timing still need end-to-end validation on the board.

See [hardware implementation notes](hardware/README.md) for the recovered sources and the packaged self-test limitation.

## Explore the repository

```text
hardware/
  ip/multiply_1_0/          Original RTL, AXI slave, component.xml, and IP drivers
  vivado/                  Original Lab 4 block design and new import helper
  board/                   ZYBO processing-system preset
  constraints/             Earlier GPIO lab pin constraints
software/
  baremetal/multiply_test/ Original standalone Vitis application
  linux/mmio/              Lab 5 hello and 32-bit MMIO modules
  linux/chardev/           Lab 6 byte-wise character driver
  linux/userspace/         Revised automated operand/product test
  linux/builtin/           Labeled reconstruction of Lab 7 kernel integration
petalinux/                 Original recipe draft and recovery notes
examples/gpio/             Earlier MicroBlaze GPIO applications
docs/                     Reports, screenshots, source audit, and check records
tools/                    Source-package consistency checker
```

The folders distinguish original files from newly authored helpers and reconstructed integration. [The source audit](docs/source-audit.md) records provenance, evidence limits, and missing material. Generated builds, compiled modules, complete BSP trees, and boot binaries are excluded. Rebuild those products using a matching toolchain and hardware platform.

## Follow the development stages

| Stage | What it teaches | Report |
| --- | --- | --- |
| Lab 1 | Introductory FPGA logic and pin assignments | [Lab 1](docs/reports/lab1-report.pdf) |
| Lab 2 | Processor-controlled GPIO using MicroBlaze | [Lab 2](docs/reports/lab2-report.pdf) |
| Lab 3 | Custom AXI multiplier and bare-metal register access | [Lab 3](docs/reports/lab3-report.pdf) |
| Lab 4 | Hardware export, PetaLinux image creation, and Linux boot | [Lab 4](docs/reports/lab4-report.pdf) |
| Lab 5 | Loadable Linux modules and register access from the kernel | [Lab 5](docs/reports/lab5-report.pdf) |
| Lab 6 | Character device and application-to-driver communication | [Lab 6](docs/reports/lab6-report.pdf) |
| Lab 7 | Built-in kernel integration and image-size comparison | [Lab 7](docs/reports/lab7-report.pdf) |

MicroBlaze belongs to the introductory GPIO work. The multiplier/Linux stages use the Zynq ARM processor. Earlier GPIO constraints are historical examples; they do not replace the multiplier block design.

## Build and run

### Required tools and equipment

The hardware target is ZYBO Z7-10, device `xc7z010clg400-1`. The recovered block design records **Vivado 2023.1**. A historical Linux login screenshot identifies **PetaLinux 2023.1+release-S05010539**. The exact kernel revision and complete build configuration have not been recovered.

Use Vivado for FPGA design, Vitis and its board support package (BSP) for the standalone application, and a matching PetaLinux/ARM toolchain for Linux software. Board testing requires the board, appropriate power and USB connections, an SD card for the documented boot flow, and a serial terminal at 115200 baud. FPGA hardware and Linux kernel builds are not provided by a normal desktop C compiler.

### 1. Check the source package on a computer

From the repository root, run with Python 3:

```sh
python tools/check_sources.py
```

This checks the IP metadata's file references, its identity against the block design, device/tool metadata, address mapping, and the reconstructed driver's consistency with the original. It does not simulate Verilog or communicate with a board.

You can compile the userspace test on a host with GCC:

```sh
gcc -std=c11 -Wall -Wextra -Werror -O2 software/linux/userspace/devtest.c -o devtest
```

A host build checks C compilation. Running it on an ordinary computer will fail to open the device unless the matching driver and hardware are actually available there.

### 2. Prepare the FPGA hardware

The original packaged IP is in `hardware/ip/multiply_1_0/`. The original block design is in `hardware/vivado/lab4_block_design.bd`.

The new convenience script creates a Vivado project, registers the IP directory, opens and validates the BD, and generates its wrapper:

```sh
vivado -mode batch -source hardware/vivado/recreate.tcl
```

It writes generated files under `build/vivado/`. It has not been executed in Vivado during this recovery. With the required IP installed, review validation results, then run synthesis, implementation, and bitstream generation in Vivado. Export the matching hardware platform as an XSA for Vitis/PetaLinux. The helper does not perform these final build/export steps.

### 3. Run the bare-metal application

In Vitis, create a standalone Cortex-A9 application using the matching exported hardware platform and BSP. Include the recovered multiply IP driver, then import the files in `software/baremetal/multiply_test/`.

The application uses `MULTIPLY_mWriteReg` to write offsets 0 and 4 and `MULTIPLY_mReadReg` to read offset 8. It iterates over operands 0 through 16 and prints the values over UART. The original program prints results; it does not automatically assert that each product is correct.

The wizard-generated `MULTIPLY_Reg_SelfTest` expects arbitrary writes to every register to read back unchanged. That expectation is incompatible with the product register. It must not be used as proof that this multiplier works.

### 4. Prepare Linux

The historical flow imported the hardware into PetaLinux, built Linux, and booted from an SD card containing `BOOT.BIN`, `image.ub`, and `boot.scr`. These are generated boot products and are intentionally excluded.

The complete PetaLinux project, device-tree modifications, and original kernel configuration are missing, so a reliable one-command Linux rebuild cannot be provided. [PetaLinux notes](petalinux/README.md) explain the recovered recipe draft, including its filename and dependency problems. Recover or recreate and validate that configuration before treating the build as reproducible.

### 5. Build a loadable module

A kernel module must be compiled against the configured kernel build tree matching the board's running kernel. On a suitable Linux development environment:

```sh
make -C software/linux/chardev \
  KDIR=/path/to/matching/kernel/build \
  ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

Replace the path and compiler prefix with those from your target SDK. This produces `multiply.ko`. The Lab 5 modules can be built the same way by changing the directory to `software/linux/mmio`.

The MMIO example and character driver both access the same hardware and use the module name `multiply`. Use one at a time. A built-in version must likewise not run alongside a second driver owning the same registers.

### 6. Create the device node and run the application

On the board, as root, load the character module and inspect its registration:

```sh
insmod multiply.ko
dmesg | tail
cat /proc/devices
```

The driver asks Linux for a **dynamic major number**, which identifies the character driver. Read the current number from the log or `/proc/devices`; do not copy the historical screenshot's number. Create the node using that number:

```sh
# Replace MAJOR with the number reported by this boot.
mknod /dev/multiplier c MAJOR 0
```

The driver does not automatically create `/dev/multiplier`. Compile the test for the target, replacing the compiler prefix if necessary:

```sh
arm-linux-gnueabihf-gcc -std=c11 -Wall -Wextra -O2 \
  software/linux/userspace/devtest.c -o devtest
```

Copy the executable to the board, then run:

```sh
./devtest
# An optional argument selects another device path:
./devtest /dev/multiplier
```

The revised program tests all **289 operand pairs** from 0–16 × 0–16. It checks transfer lengths, both operand readbacks, and the product, and returns a nonzero exit status for errors. A successful run would print `289 cases: 289 passed, 0 failed`. That is the expected output, not a result obtained during this repository update.

For the loadable character module, `rmmod multiply` removes it after applications have closed the device. A built-in driver remains part of the kernel.

### 7. Understand the built-in alternative

A loadable module is inserted after Linux starts. A built-in driver is linked into the kernel image and initialized during boot. Lab 7 describes the second approach.

[Built-in integration instructions](software/linux/builtin/README.md) provide the reconstructed driver directory, Kconfig entry, Makefile entry, and parent kernel edits. Select `CONFIG_MULTIPLIER_DRIVER=y` for built-in linkage. These files were reconstructed from the report narrative; the original edited kernel tree and `.config` were not supplied, and this reconstruction has not been target-built.

## Understand the Linux driver

The Lab 5 module uses 32-bit `iowrite32` / `ioread32` operations. The Lab 6 character driver uses byte-wise `iowrite8` / `ioread8` operations, with `get_user` and `put_user` to transfer bytes between application memory and kernel space.

| Application operation | Original character-driver behavior |
| --- | --- |
| `open` / `close` | Logs device activity |
| `write` | Writes up to 8 bytes starting at the peripheral base |
| `read` | Reads up to 12 bytes starting at the peripheral base |
| File position | Ignored; each call starts at the same register base |

Each number occupies four bytes in **little-endian** order: the least significant byte comes first. Bytes 0–3 contain operand A, bytes 4–7 contain operand B, and read bytes 8–11 contain the product. For example, 7 is encoded as `07 00 00 00`.

Use **one client at a time**. The original driver has no transaction lock or exclusive-open rule. Another process can change the inputs between a write and its following read. Locking individual system calls would still leave that gap. The reads also occur one byte at a time, so the interface does not guarantee an atomic snapshot while values change. Byte writes expose intermediate operand values, and the hardware result has pipeline latency.

The driver uses a fixed address rather than obtaining a resource from a device-tree platform device, and it does not reserve the MMIO region. Those are limitations of the preserved educational implementation. They are reasons to review ownership, synchronization, and platform integration before extending it to multiple applications.

## Review validation and limitations

| Evidence or check | What it establishes | Limit |
| --- | --- | --- |
| [Lab 3 UART capture](docs/evidence/lab3/report-image2.png) | Visible operand/product examples and end of the original application run | Does not show every pair or automatically assert correctness |
| [Lab 4 Linux capture](docs/evidence/lab4/report-image1.png) | Historical Linux login and PetaLinux release banner | Does not reproduce the build configuration |
| [Lab 5 MMIO capture](docs/evidence/lab5/report-image1.png) | Kernel module writes 7 and 2 and reads product 14 | One historical example |
| [Lab 6 captures](docs/evidence/lab6/) | Loadable registration, source screenshots, partial interactive tests | Not a complete automated test transcript |
| [Lab 7 boot capture](docs/evidence/lab7/report-image1.png) | Registration messages appear during kernel initialization | Original linked kernel source/configuration remains missing |
| Host GCC compilation | Revised test compiles with warnings treated as errors | No hardware access verified |
| Source checker | Metadata references and hardware/software mapping agree | No HDL simulation, synthesis, or kernel build |

The Lab 7 [image-size screenshot](docs/evidence/lab7/report-image2.png) shows `image.ub` shrinking from 17,616,680 to 16,514,804 bytes: **1,101,876 bytes, about 1.05 MiB**. This corrects the report's approximately 1 KB statement. These are sizes of complete boot images; the capture does not isolate kernel code size or prove which option caused each saved byte.

No Vivado/PetaLinux build, target kernel build, or board test was rerun during source recovery. A complete 289-case hardware pass for the revised test remains pending. A full reproduction still needs the complete PetaLinux configuration/device tree, exact kernel revision, and original Lab 7 edits or a validated replacement.

[The source audit](docs/source-audit.md) and [local check records](docs/evidence/) provide the detailed provenance and validation boundaries. Future validation should record source revision, kernel version, hardware/image identity, complete test output, and exit status. The small operand sweep does not cover overflow, all byte lanes, or concurrent access.

## Troubleshooting

| Symptom | What to check |
| --- | --- |
| `multiply.h` is missing in Vitis | Include the recovered IP driver in the matching BSP/platform. |
| Test cannot open `/dev/multiplier` | Check driver registration, node spelling, current major number, and permissions. |
| Module reports an invalid format | Rebuild against the board's exact kernel configuration/version and target architecture. |
| Wrong operand or product values | Confirm the programmed hardware matches the address map; check byte lanes, result timing, and competing clients. |
| Module name already exists | Check whether another `multiply` module or built-in driver is active. |
| Vivado cannot find the custom IP | Register `hardware/ip/` as an IP repository and refresh the catalog; use the recorded Vivado version. |
| SD-card mount fails | Verify the actual block-device name on the board. Historical screenshots contain failed mount commands and should not be copied as instructions. |

## License and attribution

See [LICENSE](LICENSE) for the repository license. Individual files retain their original notices and terms, including Xilinx application/platform code and GPL-declared Linux modules. The top-level license does not override those notices. Original source files and reports are preserved; newly authored helpers and reconstructed files are identified in their documentation.
