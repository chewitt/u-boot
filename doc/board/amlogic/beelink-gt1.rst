.. SPDX-License-Identifier: GPL-2.0+

U-Boot for Beelink GT1/GT1-Ultimate
===================================

The Shenzen AZW (Beelink) GT1 and GT1-Ultimate are based on the Amlogic
Q200 reference board with an S912 chip.

- 2GB or 3GB DDR3 RAM
- 32GB eMMC
- HDMI 2.1 video
- S/PDIF optical output
- 10/100/1000 Ethernet
- AP6356S Wireless (802.11 a/b/g/n/ac, BT 4.2)
- 3x USB 2.0 ports
- IR receiver
- 1x micro SD card slot
- 1x Power LED (white)
- 1x Reset button (internal)

Beelink do not provide public schematics, but have been willing
to share them with known distro developers on request.

U-Boot compilation
------------------

.. code-block:: bash

    $ export CROSS_COMPILE=aarch64-none-elf-
    $ make beelink-gt1_defconfig
    $ make

Image creation
--------------

Amlogic does not provide sources for the firmware and for tools needed
to create the bootloader image. Beelink have provided the Amlogic SDK
u-boot sources to known distro developers on-request, allowing FIPs to
be generated. The GT1 (2GB) and GT1-Ultimate (3GB) models differ in 
RAM size and original Beelink devices use the same spec RAM chips and
can share the same boot FIPs:

https://github.com/LibreELEC/amlogic-boot-fip/tree/master/beelink-gt1

For simplified usage, pleaser refer to :doc:`pre-generated-fip` with codename `beelink-gt1`

.. code-block:: bash

    $ wget https://github.com/LibreELEC/amlogic-boot-fip/archive/master.zip
    $ unzip master.zip
    $ export FIPDIR=$PWD/amlogic-boot-fip/beelink-gt1

Go back to the mainline U-Boot source tree then:

.. code-block:: bash

    $ mkdir fip
    $ cp $FIPDIR/* fip/
    $ cp u-boot.bin fip/bl33.bin

    $ sh fip/blx_fix.sh \
    	 fip/bl30.bin \
    	 fip/zero_tmp \
    	 fip/bl30_zero.bin \
    	 fip/bl301.bin \
    	 fip/bl301_zero.bin \
    	 fip/bl30_new.bin \
    	 bl30

    $ python fip/acs_tool.pyc fip/bl2.bin fip/bl2_acs.bin fip/acs.bin 0

    $ sh fip/blx_fix.sh \
         fip/bl2_acs.bin \
         fip/zero_tmp \
         fip/bl2_zero.bin \
         fip/bl21.bin \
         fip/bl21_zero.bin \
         fip/bl2_new.bin \
         bl2

    $ fip/gxl/aml_encrypt_gxl --bl3enc --input fip/bl30_new.bin
    $ fip/gxl/aml_encrypt_gxl --bl3enc --input fip/bl31.img
    $ fip/gxl/aml_encrypt_gxl --bl3enc --input fip/bl33.bin
    $ fip/gxl/aml_encrypt_gxl --bl2sig --input fip/bl2_new.bin --output fip/bl2.n.bin.sig
    $ fip/gxl/aml_encrypt_gxl --bootmk \
                              --output fip/u-boot.bin \
                              --bl2 fip/bl2.n.bin.sig \
                              --bl30 fip/bl30_new.bin.enc \
                              --bl31 fip/bl31.img.enc \
                              --bl33 fip/bl33.bin.enc

And then write the image to SD or eMMC with:

.. code-block:: bash

    $ DEV=/dev/your_sd_device
    $ dd if=fip/u-boot.bin.sd.bin of=$DEV conv=fsync,notrunc bs=512 skip=1 seek=1
    $ dd if=fip/u-boot.bin.sd.bin of=$DEV conv=fsync,notrunc bs=1 count=440
