# iMX boot plugin #
This repository contains a C-only implementation of iMX boot ROM plugin.

This plugin is an alternative to DCD tables and uboot SPL to initialize the SDRAM memory.

The plugin was written for a board, that had different SDRAM configurations, but did not have other boot interfaces than eMMC and USB. The flash had to be programmed in-circuit. We did not want to have multiple images for every memory configuration deployed with fixed DCD tables. We had to boot up the board via USB and by using the plugin mechanism, all can be done using imx\_usb\_loader and a single image of plugin+uboot. If needed, a single bootloader update package can be deployed for all board variants. In later projects, it has been used for NAND boot as well.
This plugin is made completely transparent to uboot; it does not require any major modification of uboot image (other than removing SDRAM init DCD table).

The advantages over uboot SPL:
* supports all boot options, that the iMX boot ROM does. It just resumes to loading the uboot image that is concatenated after plugin itself.
* supports imx serial download. A single image of plugin + uboot can be booted with imx\_usb\_loader via usb. This is important when the board flash must be in-circuit programmed (instead of imaging SD cards etc).
* possible support of authenticated and encrypted boot (not tested)

Boot flow:
* iMX boot ROM loads the plugin image to SRAM and executes it
* plugin initializes SDRAM
* USB:
 * plugin returns to bootloader to let the PC upload the next bootloader
* others:
 * plugin calls the ROM API to load the next bootloader image written after itself to SDRAM
 * plugin returns to ROM with pointer to loaded image
 * ROM verifies and executes the loaded image
  
Usage:
* Set platform define in config.h
* compile plugin
* compile uboot without DCD SDRAM init table. For example, use: CONFIG\_SYS\_EXTRA\_OPTIONS="IMX\_CONFIG=arch/arm/imx-common/spl\_sd.cfg,MX6Q" (or just remove all DATA and CHECK\_BITS\_SET lines from cfg).
* concatenate plugin.imx and uboot.imx
* Deploy:
 * flash: prepend with required amount of whitespace. For SD/MMC, this is 1024 bytes. Write to flash.
 * usb: upload using imx\_usb\_loader

This plugin has been so far tested with following configurations:
* iMX6D, iMX6Q, iMX6DP: eMMC boot
* iMX7D: NAND boot

# Running memory calibration/test #
The plugin can be used with Freescale ddr\_stress\_tester to calibrate the DDR or to verify the configuration. This way we avoid the duplicate work of generating .inc files for the tool. To do that, you need to add imx header to the ddr\_stress\_tester. A header for ddr\_stress\_tester v2.52 is provided in this repository.
This is needed because the imx6 serial upload protocol can't directly jump to an address, the JUMP\_ADDRESS command needs to point to an imx header, where the real jump address is.

Usage:
 * cat plugin.imx memtest\_header.bin ddr-test-uboot-jtag-mx6dq.bin > memtest.imx
 * imx\_usb memtest.imx
