# 260926ESPAcornAtomESP32VGA
 This is my interpretation of the Acorn Atom running on the ESP32VGA 1.2 board. This board is readily available from Chinese online stores and provides a VGA output, as well as connections for a PS/2 keyboard and an SD card. While a PS/2 mouse can also be connected to the hardware, mouse support has not yet been implemented in this emulator.

It behaves like a standard Acorn Atom equipped with an MMC interface and a ROM switching board. The most essential ROMs are included on board. SHIFT F12 does the autoboot, F12 only, is the break;

The keyboard mapping matches the physical (US) keyboard layout directly. For instance, pressing SHIFT + 2 produces an '@'.
The TAB key functions as the COPY key.

Still open:
- Exact timing of the 6847 (chess mode/scan) Not tested
- AGD (not tested)
- Sound 
