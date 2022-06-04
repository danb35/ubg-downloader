# Hardware design
This device is built on a custom printed-circuit board; design files for KiCAD 6.0, schematics, a bill of materials (BOM), and Gerber files are available in the Hardware directory.  You can order PCBs from any PCB fab you like; the most economical are likely to be oshpark.com (US based, three beautiful purple PCBs for $5/square inch) or jlcpcb.com (China-based, good quality, cheap, but shipping can be expensive).  I have no affiliation with either of these vendors.

This PCB uses surface-mount components in order to reduce its size.  If you're careful, you should be able to hand-solder them without difficulty.  JLCPCB (linked above) does offer SMT assembly service, but I haven't verified that all the components I use on this board are available through their service.

The PCB also includes connections for devices that aren't used in the current version of the software.  For example, SW1 and SW2 are intended for pushbutton switches that could be used to, e.g., set certain parameters on the device.  The software doesn't currently use them, though.  Similarly, there's a network of vias below the RTC battery; those are intended for the user to add any desired components, and you'd naturally need to code support for them.

# Components
As noted above, the BOM is in the Hardware directory, but major components of this device include:

* A [Teensy 2.0](https://www.pjrc.com/store/teensy.html) microcontroller board
* A [micro SD Card Adapter](https://www.pjrc.com/store/sd_adaptor.html)
* A [16x2 LCD Display Module](https://www.dfrobot.com/index.php?route=product/product&path=53&product_id=135)

# Background
This circuit and PCB were originally designed in CadSoft EAGLE in 2012.  At that time, that software was available at no charge for certain noncommercial uses.  Since then, licensing has changed and there's no longer a no-cost version of this software.  I've accordingly imported the schematic and PCB layout into KiCAD, which is free and open-source software.