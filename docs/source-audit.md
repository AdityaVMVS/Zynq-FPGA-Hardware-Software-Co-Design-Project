# Source and evidence audit

Updated 2026-11-14 (America/Chicago), after expanding the search to the parent AXI-Project folder. The initial search covered only ecen_749_lab; its missing-source findings were incomplete and are superseded here. Parent and original workspace files remain untouched. No AGENTS.md was found. The checkout started clean at d26f5555f9a756b3affcb979094dddd297337a3e; a subsequent GitHub fetch confirmed the same origin/main. Existing edits were retained.

## Recovery and provenance

| Material | Origin relative to AXI-Project | Repository destination |
| --- | --- | --- |
| Original custom IP package | LAB4/ip_repo/multiply_1_0/ | hardware/ip/multiply_1_0/, unchanged RTL, AXI slave, component.xml, IP drivers, Tcl, and examples |
| Original block design | LAB4/LAB4.srcs/sources_1/bd/lab4_block_design/lab4_block_design.bd | hardware/vivado/lab4_block_design.bd; Vivado 2023.1, xc7z010clg400-1 |
| Import helper | Newly authored | hardware/vivado/recreate.tcl; not a recovered export or verified Vivado build |
| Lab 6/7 reports | LAB6/ and LAB7/ PDFs and DOCX | PDFs in docs/reports/; all embedded PNGs extracted unchanged to docs/evidence/lab6/ and lab7/ |
| Earlier report screenshots | LAB3/Lab3_report.docx, LAB4/LAB4_Report.docx, LAB5/LAB 5 REPORT.docx | docs/evidence/lab3/, lab4/, lab5/ |
| Commented original driver | LAB6/multiple.c | software/linux/chardev/multiple.report-original.c.txt; matches source screenshots |
| Recipe draft | ecen_749_lab/LAB6/multiply.bb.txt | petalinux/reference/multiply.bb.txt; unchanged, issues documented |
| Lab 7 integration | Report narrative | software/linux/builtin/; explicitly reconstructed Kconfig/Makefile and copied Lab 6 driver |

The parent workspace/ contains Vitis application/IP-driver copies and generated BSP/platform/build files. The packaged IP driver is retained as the primary driver source; duplicate generated BSP trees are omitted. Lab_manuals/ contains PDFs named as the author's Lab 3–7 reports, not an independently established project configuration.

[Initial workspace hashes](evidence/workspace-manifest.csv) and [ZIP entry inventory](evidence/archive-entries.csv) document the original narrower search. The separate parent recovery manifest records imported sources/reports. The inner LAB7.zip contains only an image; the parent LAB7 folder also contains the report and embedded evidence. Boot binaries, generated Vivado runs/cache, compiled modules, bitstreams/XSA, and generated Vitis platforms were not imported.

## Hardware/software contract

The recovered BD maps the multiplier at 0x43C00000 for 64K. The generated header gives high address 0x43C0FFFF. Both match the Lab 6 span 0x10000. This verifies source agreement, not the identity of a currently programmed board image.

The AXI RTL decodes address bits [3:2], honors each WSTRB operand byte lane, and returns a selected full word for reads. Processor/interconnect byte-lane selection is consistent with the byte-wise driver, but still needs end-to-end hardware testing. Product writes at 0x08 have no effect. Register 0x0c is independent read/write scratch, not the upper product. The product truncates to 32 bits and passes through tmp_reg then slv_reg2, two sequential stages after operand update, without a ready/done bit. See [hardware notes](../hardware/README.md).

The driver ignores file offsets, caps write/read lengths at 8/12 bytes, and has no exclusive-open rule or transaction lock. Another client can replace operands between calls; a mutex around individual calls would still leave that gap. Run one client. Byte writes expose intermediate operands/products. The driver maps without reserving the region; avoid simultaneous MMIO owners. The packaged wizard self-test expects every register to echo arbitrary writes, which is incompatible with the read-only product register; its presence is not passing multiplier validation.

## Screenshot evidence

- Lab 3 report-image1 shows application source; report-image2 shows UART output. The application prints values rather than automatically asserting every product.
- Lab 4 report-image1 records Linux boot/login. Lab 5 report-image1 records module use and MMIO logging.
- Lab 6 report-images 1–2 show registration after insmod, manual device-node creation, and partial interactive tests. Images 3–4 show the commented driver; images 5–6 show the interactive test. Original program files exist in the parent folder, so transcription is unnecessary. The revised automated test remains separate.
- Lab 7 [boot capture](evidence/lab7/report-image1.png) shows registration with major 244 and the mknod instruction amid kernel initialization, supporting historical built-in registration. It does not supply the exact linked kernel sources/configuration.
- Lab 7 [size capture](evidence/lab7/report-image2.png) shows 17,616,680 and 16,514,804 bytes: a reduction of 1,101,876 bytes (about 1.05 MiB), contradicting the report's approximately 1 KB. These are complete image.ub sizes, not isolated kernel text sizes; the capture cannot attribute the whole difference to individual configuration options.
- Lab 7 [test capture](evidence/lab7/report-image3.png) shows manual node creation and visible cases 0×0 through 0×16, then 1×0 through 1×4. It does not show a complete 289-case pass. Failed mount commands remain visible in the unedited capture.

Extracted PNGs preserve original image bytes. Historical report claims and screenshots are not fresh execution results. The screenshot major 244 must not be assumed for a later boot. No complete 289-case transcript or hardware test of the revised noninteractive program was recovered.

## Remaining gaps and checks

No PetaLinux project-spec configuration, layer configuration, device-tree source/overlay, complete recipe tree, kernel config fragment, or original Lab 7 external kernel source/patch was found in the expanded parent inventory. Lab 4 describes a cracklib branch edit (master to main), and Lab 5 describes recipe header dependencies, but exact patches are missing. No code screenshot supplies these edits. Lab 7 Kconfig wording and parent kernel edits are reconstructed from narrative, not verbatim transcription.

The Lab 4 boot capture identifies PetaLinux 2023.1+release-S05010539. A full reproduction still needs the exact kernel revision and complete PetaLinux setup, project configuration/device tree, original kernel changes or a validated replacement, and a verified Vivado 2023.1 hardware build. Capture a complete boot log and 289-case run with exit status, source revision, kernel version, and image/bitstream identity. Cover high byte lanes, truncation, pipeline timing and concurrent access separately.

Host GCC compiled the revised test with -std=c11 -Wall -Wextra -Werror -O2; a nonexistent device returns exit 1. Package XML/BD structure and referenced files are checked by tools/check_sources.py. No RTL simulation, target kernel build, Vivado/PetaLinux build or hardware run was performed. Available host tools cannot establish those results.
