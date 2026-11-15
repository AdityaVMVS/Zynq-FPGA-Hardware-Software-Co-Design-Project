# Multiplier hardware

This folder contains the FPGA design for the AXI4-Lite multiplier. The ARM processor writes two operands through the AXI bus and reads the product from a register.

| Folder | Purpose |
| --- | --- |
| `ip/multiply_1_0/hdl/` | Verilog multiplier and AXI4-Lite register interface |
| `ip/multiply_1_0/component.xml` | IP metadata used by the Vivado IP catalog |
| `ip/multiply_1_0/drivers/` | Vitis register-access macros and driver support files |
| `ip/multiply_1_0/xgui/` and `bd/` | Vivado customization and block-design support |
| `ip/multiply_1_0/example_designs/` | Example simulation and hardware-test projects |
| `vivado/` | Zynq block design and project setup script |
| `board/` | ZYBO processing-system preset |
| `constraints/` | Pin assignments for the earlier GPIO exercises |

## Register operation

The block design targets `xc7z010clg400-1` and assigns the multiplier base address `0x43C00000` with a 64 KiB window.

| Offset | Function |
| --- | --- |
| `0x00` | Read/write operand A |
| `0x04` | Read/write operand B |
| `0x08` | Read-only low 32 bits of the product |
| `0x0c` | Read/write scratch register |

The AXI interface uses address bits [3:2] to select a register. Each write strobe enables one byte lane, allowing software to update operands one byte at a time. Reads return the selected word. The register bank repeats within the larger address window, so applications should use the offsets above.

The multiplier stores its result in `tmp_reg` and then transfers it to `slv_reg2` on the next clock. This adds two registered stages after an operand update. There is no done flag, and products larger than 32 bits are truncated. The scratch register does not store the upper half of the product.

## Open the design

Use Vivado 2023.1 and run from the repository root:

```sh
vivado -mode batch -source hardware/vivado/recreate.tcl
```

The script creates a project under `build/vivado/`, adds the custom IP, opens the block design, and generates its wrapper. Synthesis, implementation, bitstream generation, and hardware export are separate steps. The script still needs validation in Vivado.

For the standalone application, include the multiply IP driver in the matching Vitis platform/BSP. Its access macros are `MULTIPLY_mWriteReg` and `MULTIPLY_mReadReg`.

The packaged `MULTIPLY_Reg_SelfTest` assumes every register can store an arbitrary written value. That assumption is wrong for the product register. Use operand/product checks to test the multiplier; the example test projects have not been validated here.
