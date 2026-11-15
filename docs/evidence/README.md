# Evidence guide

The PNG files are unchanged images extracted from the author's report documents. They document historical work; none represents a new board test during repository recovery.

| Folder | Contents |
| --- | --- |
| [Lab 3](lab3/) | Bare-metal application source screenshot and UART output |
| [Lab 4](lab4/) | Linux login and PetaLinux release banner |
| [Lab 5](lab5/) | Module insertion, MMIO operands 7 and 2, result 14 |
| [Lab 6](lab6/) | Interactive testing and original driver/application source screenshots |
| [Lab 7](lab7/) | Boot registration, image-size comparison, partial interactive testing |

Lab 7 includes a manually transcribed boot-registration excerpt and image-size CSV. The screenshot is the primary evidence; the excerpt is incomplete and is labeled accordingly.

`local-checks.txt` records host compilation/error-path checks from the initial recovery. `recovery-checks.txt` records source-package checks after locating the parent hardware files. These checks do not establish hardware behavior.

`workspace-manifest.csv` and `archive-entries.csv` describe the original, narrower `ecen_749_lab` search. `parent-recovery-manifest.csv` hashes selected original sources and reports found after expanding to `AXI-Project`. Manifest paths describe the original workspace, not the final repository layout. Reports were subsequently consolidated and renamed `lab1-report.pdf` through `lab7-report.pdf` in [reports](../reports/), without editing their content.

Duplicate Lab 6 screenshots were removed after verifying that their hashes matched the retained report images. Introductory Lab 1/2 screenshot copies were omitted because their reports already preserve that history. The complete packaged multiplier IP, including its original example designs, remains intact so its metadata, notices, and supplied references are preserved.

See the [source audit](../source-audit.md) for what each capture establishes and what remains missing.
