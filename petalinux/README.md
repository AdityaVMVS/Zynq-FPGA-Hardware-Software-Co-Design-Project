# Recovered PetaLinux material

`reference/multiply.bb.txt` is an unchanged copy of the supplied Lab 6 draft, not a usable project layer or a validated recipe.

It inherits `module` and requests `file://multiplier.c`, whereas the supplied driver is named `multiply.c`. It supplies no Makefile in `SRC_URI`; the repository's external-module Makefile is separate. The draft declares `LICENSE = "CLOSED"` while the driver declares `MODULE_LICENSE("GPL")`; resolve the actual distribution license and required recipe metadata before enabling it. The repository LICENSE does not replace third-party notices.

The Lab 4 boot screenshot identifies PetaLinux 2023.1+release-S05010539. No layer configuration, project configuration, device-tree changes, or original built-in kernel recipe/patch was supplied. Preserve this draft as provenance rather than silently treating a reconstructed recipe as the original integration. See [the source audit](../docs/source-audit.md) for missing artifacts and validation limits.
