.. SPDX-License-Identifier: GPL-2.0+

Amlogic SoC Boot Flow
=====================

The Amlogic SoCs has a pre-defined boot sequence in the SoC ROM code.

Here is the possible boot sources of different SoC families supported by U-Boot:

GX* & AXG family
----------------

+----------+--------------------+-------+-------+---------------+---------------+
|          |   1                | 2     | 3     |    4          |     5         |
+==========+====================+=======+=======+===============+===============+
| S905     | POC=0: SPI NOR     | eMMC  | NAND  | SD Card       | USB Device    |
| S905X    |                    |       |       |               |               |
| S905L    |                    |       |       |               |               |
| S905W    |                    |       |       |               |               |
| S912     |                    |       |       |               |               |
+----------+--------------------+-------+-------+---------------+---------------+
| S805X    | POC=0: SPI NOR     | eMMC  | NAND  | USB Device    | -             |
| A113D    |                    |       |       |               |               |
| A113X    |                    |       |       |               |               |
+----------+--------------------+-------+-------+---------------+---------------+

POC pin: `NAND_CLE`

Usually boards provides a button to force USB BOOT which disables eMMC clock signal to
bypass eMMC.

Most of the GX SBCs have removable eMMC modules, in this case removing the eMMC and SDCard
will boot over USB.

An exception is the lafrite board (aml-s805x-xx) which doesn't have an SDCard and boots
over SPI. The only ways to boot over USB are:

 - erase first sectors of SPI NOR flash
 - insert an HDMI boot plug forcing boot over USB

The VIM1 and initial VIM2 boards provides a test point on the eMMC signals to block the
storage from answering and continue to the next boot step.

The USB Device boot uses the first USB interface, on some boards this port is only
available on an USB-A type connector, and needs an special Type-A to Type-A cable
to communicate with the BootROM.

G12* & SM1 family
-----------------

+-------+-------+-------+---------------+---------------+---------------+---------------+
| POC0  | POC1  | POC2  | 1             | 2             | 3             | 4             |
+=======+=======+=======+===============+===============+===============+===============+
| 0     | 0     | 0     | USB Device    | SPI NOR       | NAND/eMMC     | SDCard        |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 0     | 0     | 1     | USB Device    | NAND/eMMC     | SDCard        | -             |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 0     | 1     | 0     | SPI NOR       | NAND/eMMC     | SDCard        | USB Device    |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 0     | 1     | 1     | SPI NAND      | NAND/eMMC     | USB Device    | -             |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 1     | 0     | 0     | USB Device    | SPI NOR       | NAND/eMMC     | SDCard        |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 1     | 0     | 1     | USB Device    | NAND/eMMC     | SDCard        | -             |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 1     | 1     | 0     | SPI NOR       | NAND/eMMC     | SDCard        | USB Device    |
+-------+-------+-------+---------------+---------------+---------------+---------------+
| 1     | 1     | 1     | NAND/eMMC     | SDCard        | USB Device    | -             |
+-------+-------+-------+---------------+---------------+---------------+---------------+

The last is the normal default boot on production devices.

 * POC0 pin: `BOOT_4` (0 and all other 1 means SPI NAND boot first)
 * POC1 pin: `BOOT_5` (0 and all other 1 means USB Device boot first
 * POC2 pin: `BOOT_6` (0 and all other 1 means SPI NOR boot first)

Usually boards provides a button to force USB BOOT which lowers `BOOT_5` to 0.

Some boards provides a test point on the eMMC or SPI NOR clock signals to block the
storage from answering and continue to the next boot step.

The Khadas VIM3 boards embeds a microcontroller which sets POC signals depending
on it's configuration or a specific key press sequence to either boot from SPI NOR
or eMMC then SDCard, or boot as USB Device.

The Odroid-N2(+) has a switch to select SPI NOR or eMMC boot.

Boot Modes
----------

 * SDCard

The BootROM fetches the first SDCard sectors in one sequence, then checks the content
of the data.

The BootROM expects finding the FIP binary are sector 1, 512 bytes offset from the start.

 * eMMC

The BootROM fetches the first sectors in one sequence, first on the main partition,
and then on the Boot0 followed by Boot1 HW partitions.

After each read, the BootROM checks the data and looks the next partition if it fails.

The BootROM expects finding the FIP binary are sector 1, 512 bytes offset from the start.

 * SPI NOR

The BootROM fetches the first SPI NOR sectors in one sequence, then checks the content
of the data.

The BootROM expects finding the FIP binary are sector 1, 512 bytes offset from the start.

 * NAND & SPI NAND

Those modes are not widely used in open platforms, thus no details are available.

 * USB Device

The BootROM setups the USB Gadget interface to serve a custom USB protocols with the
USB ID 1b8e:c003.

This protocol is also implemented in the Amlogic Vendor U-Boot.

The `update` utility provided by Amlogic is designed to use this protocol.

The https://github.com/superna9999/pyamlboot open-source utility also implements this
protocol and can load U-Boot in memory in order to start the SoC without any attached
storage or to recover from a failed storage/flash tentative.

HDMI Recovery
-------------

The BootROM also briefly reads 8 bytes at address I2C 0x52 offset 0xf8 (248) on the
HDMI DDC bus.

If the content is `boot@USB` it will force USB boot mode, if the content is `boot@SDC`
it will force SDCard boot mode.

If USB Device doesn't enumerate or SD Card boot step doesn't work, it will continue the
boot steps.

Special boot dongles can be built by connecting a 256bytes EEPROM set to answer on
address 0x52, and program `boot@USB` or `boot@SDC` at offset 0xf8 (248).

Note: if the SoC was booted with USB Device forced at first step, it will keep the boot
order on a warm reboot, only a cold reboot (remove power) will reset the boot order.
