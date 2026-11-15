# Recovered multiplier hardware

`ip/multiply_1_0/` is copied unchanged from the parent workspace's `LAB4/ip_repo/multiply_1_0/`, including component.xml, RTL, software driver, GUI/BD Tcl, and generated example designs. All supplied notices remain in place. `vivado/lab4_block_design.bd` is the original Lab 4 BD, recorded as Vivado 2023.1 targeting `xc7z010clg400-1`. Its address segment maps the custom IP at `0x43C00000` for 64 KiB. The PS, AXI interconnect, clock, and reset connections are in the BD.

`vivado/recreate.tcl` is a newly authored import/wrapper-generation helper, not a recovered export or a verified build. Run it from Vivado 2023.1 with the required Xilinx IP installed. Generated products go to ignored `build/vivado/`. No Vivado installation capable of executing this flow was available for this update.

The 32-bit AXI slave decodes address bits [3:2]. Operand writes honor each WSTRB byte lane; reads return the selected full word, leaving byte-lane selection to the AXI interconnect/processor. This supports the intended byte-wise Lab 6 interface by source inspection; physical integration still needs a board test. Only the low four address bits enter this IP, so the 16-byte register bank aliases through the larger assigned window.

| Offset | Recovered RTL behavior |
| --- | --- |
| 0x00 | Operand A, read/write, byte strobes honored |
| 0x04 | Operand B, read/write, byte strobes honored |
| 0x08 | Read-only low 32 bits of product; AXI writes have no effect |
| 0x0c | Independent read/write scratch register, not upper product bits |

`tmp_reg <= slv_reg0 * slv_reg1; slv_reg2 <= tmp_reg;` forms two registered stages after operand update, with truncation to 32 bits and no ready/done bit. Intermediate products occur during byte-wise operand writes. The driver reads operands before product, but this is not a general synchronization contract for a future faster client or changed hardware.

The packaged `MULTIPLY_Reg_SelfTest` is an unmodified wizard test that expects all four registers to echo arbitrary writes. It is incompatible with the read-only product register and must not be presented as a passing multiplier test. Use the application operand/product test instead. Supplied BFM examples also require review and Vivado simulation support; they have not been run.
