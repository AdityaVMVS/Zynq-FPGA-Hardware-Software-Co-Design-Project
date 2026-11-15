# Lab 7 built-in kernel integration reconstruction

The Lab 7 report describes a `drivers/multiplier_driver/` directory, ARM-dependent tristate `MULTIPLIER_DRIVER`, and `obj-$(CONFIG_MULTIPLIER_DRIVER) += multiplier.o`. It does not supply the edited kernel tree or screenshots of Kconfig/Makefile. The files here reconstruct that described integration; they are not recovered historical kernel edits. `multiplier.c` is a copy of the repository's original Lab 6 driver, with its limitations intact. The more heavily commented parent-folder source is preserved separately as `../chardev/multiple.report-original.c.txt`; source screenshots match that variant.

Copy `multiplier_driver/` into the matching kernel's `drivers/` directory. Add these entries to the kernel's existing files (do not replace them):

```make
# drivers/Makefile
obj-$(CONFIG_MULTIPLIER_DRIVER) += multiplier_driver/
```

```kconfig
# drivers/Kconfig, inside the existing driver menu
source "drivers/multiplier_driver/Kconfig"
```

Enable `CONFIG_MULTIPLIER_DRIVER=y` in the actual target kernel configuration and build using the project's matching PetaLinux/kernel sources. Do not also enable the loadable multiplier recipe. `module_init(multiplier_init)` becomes a boot initcall when linked built-in; it does not by itself prove how an existing image was linked. The report describes using external kernel sources under `components/ext_sources`, but that tree, exact kernel revision and complete project configuration remain missing. These reconstructed files have not been target-built.

The [boot screenshot](../../../docs/evidence/lab7/report-image1.png) shows `Registered device with major number 244` and `mknod /dev/multiplier c 244 0` between FPGA manager and network initialization messages, supporting historical registration during kernel boot. The [test screenshot](../../../docs/evidence/lab7/report-image3.png) shows manual device-node creation and a partial interactive operand sweep. Neither is a fresh test of this reconstruction. Use the dynamic major from the current boot rather than hard-coding 244.
