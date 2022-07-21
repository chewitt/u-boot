.. SPDX-License-Identifier: GPL-2.0+

U-Boot for WeTek Play2
======================

WeTek Play2 is an S905-H Set-Top Box with factory support for dual booting between Android
and Linux (LibreELEC) and options for multi-standard DVB tuners. The box has the following
specifications:

 - Amlogic S905 ARM Cortex-A53 quad-core SoC @ 1.5GHz
 - ARM Mali 450 GPU
 - 2GB DDR3 SDRAM
 - 8GB eMMC
 - HDMI 2.0 4K/60Hz display
 - S/PDIF Optical Audio
 - CVBS + Stereo Audio Jack
 - 10/100/1000 Base-T Ethernet
 - AP6330 SDIO Wifi (b/g/n) and BT 4.0
 - 3x USB 2.0 ports (1x OTG)
 - MicroSD
 - IR Receiver
 - UART connector (2.5mm Jack with DB9 cable)

Schematics have been shared with kernel and LibreELEC maintainers.

U-Boot Compilation
------------------

.. code-block:: bash

    $ export CROSS_COMPILE=aarch64-none-elf-
    $ make wetek-play2_defconfig
    $ make

Image Creation
--------------

For simplified usage, pleaser refer to :doc:`pre-generated-fip` with codename `wetek-play2`

WeTek shared private u-boot sources with kernel and LibreELEC maintainers allowing the FIPs
to be compiled and shared via the pre-built repository:

https://github.com/LibreELEC/amlogic-boot-fip/tree/master/wetek-play2

.. code-block:: bash

    $ git clone https://github.com/LibreELEC/amlogic-boot-fip 
    $ cd amlogic-boot-fip/wetek-play2
    $ export FIPDIR=$(pwd)

Go back to your mainline u-boot source tree then:

.. code-block:: bash

    $ mkdir fip
    $ cp $FIPDIR/* fip/
    $ cp u-boot.bin fip/bl33.bin

    $ fip/blx_fix.sh \
    	fip/bl30.bin \
        fip/zero_tmp \
        fip/bl30_zero.bin \
        fip/bl301.bin \
        fip/bl301_zero.bin \
        fip/bl30_new.bin \
        bl30

    $ sed -i 's/\x73\x02\x08\x91/\x1F\x20\x03\xD5/' fip/bl2.bin
    $ python fip/acs_tool.pyc fip/bl2.bin fip/bl2_acs.bin fip/acs.bin 0

    $ fip/blx_fix.sh \
        fip/bl2_acs.bin \
        fip/zero_tmp \
        fip/bl2_zero.bin \
        fip/bl21.bin \
        fip/bl21_zero.bin \
        fip/bl2_new.bin \
        bl2

    $ fip/fip_create --bl30 fip/bl30_new.bin --bl31 fip/bl31.img --bl33 fip/bl33.bin fip/fip.bin

    $ cat fip/bl2_new.bin fip/fip.bin > fip/boot_new.bin

    $ fip/aml_encrypt_gxb --bootsig --input fip/boot_new.bin --output fip/u-boot.bin

    $ dd if=fip/u-boot.bin of=fip/u-boot.bin.gxbb bs=512 conv=fsync
    $ dd if=fip/u-boot.bin of=fip/u-boot.bin.gxbb bs=512 seek=9 skip=8 count=87 conv=fsync,notrunc
    $ dd if=/dev/zero of=fip/u-boot.bin.gxbb bs=512 seek=8 count=1 conv=fsync,notrunc
    $ dd if=fip/bl1.bin.hardkernel of=fip/u-boot.bin.gxbb bs=512 seek=2 skip=2 count=1 conv=fsync,notrunc
    $ fip/aml_chksum fip/u-boot.bin.gxbb

Then write u-boot to SD or the intenal eMMC module with:

.. code-block:: bash

    $ DEV=/dev/mmcblkX
    $ dd if=fip/u-boot.bin.gxbb of=$DEV conv=fsync,notrunc bs=1 count=112
    $ dd if=fip/u-boot.bin.gxbb of=$DEV conv=fsync,notrunc bs=512 skip=1 seek=1

Note: bs/count values are intentionally different to other Amlogic 'fusing' recipes

Credit to Jonas Karlmann (@Kwiboo) for deconstructing the Odroid C2 sd/emmc build recipe
that allows the Amlogic signed boot firmware and MBR structures to coexist on eMMC, see:
https://github.com/Kwiboo/u-boot/commit/6d0a17632922077a2e64b13ae1a6bdf0024b718f
