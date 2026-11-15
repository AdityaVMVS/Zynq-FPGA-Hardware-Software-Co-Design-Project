# Built-in multiplier driver

A built-in driver is linked into the Linux kernel and initialized during boot. This avoids loading a separate `.ko` file after startup. Lab 7 uses this approach for the multiplier character device.

`multiplier_driver/` contains the driver source, its Kconfig option, and its kernel Makefile. The integration files follow the Lab 7 report; the exact lab kernel tree and configuration are not available. This version has not been target-built.

## Add the driver to a kernel tree

Copy `multiplier_driver/` into the kernel's `drivers/` directory. Add this line to `drivers/Makefile`:

```make
obj-$(CONFIG_MULTIPLIER_DRIVER) += multiplier_driver/
```

Add the following inside the existing menu in `drivers/Kconfig`:

```kconfig
source "drivers/multiplier_driver/Kconfig"
```

Enable `CONFIG_MULTIPLIER_DRIVER=y` in the target kernel configuration. The option depends on ARM. Choosing `y` links the driver into the kernel; choosing `m` builds a loadable module. Build the kernel using the matching PetaLinux environment.

The driver's `module_init(multiplier_init)` becomes an initialization call during kernel boot when built in. The initialization maps the peripheral, registers a character device, and prints its major number. Create `/dev/multiplier` using the number from the current boot. The driver does not create the node automatically.

## Test the interface

The [boot screenshot](../../../docs/evidence/lab7/report-image1.png) shows registration during kernel initialization. The [application screenshot](../../../docs/evidence/lab7/report-image3.png) shows device-node creation and part of the interactive test.

Use the userspace test in `software/linux/userspace/` for operand/readback checks. Run one client at a time because the driver does not lock a complete write/read transaction. Do not load another multiplier module while the built-in driver is active. A built-in driver cannot be removed with `rmmod`; changes require rebuilding and booting the kernel.
