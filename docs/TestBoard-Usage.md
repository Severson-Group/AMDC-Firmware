# AMDC TestBoard Usage

This document describes the AMDC TestBoard and the things for which it can be used. The necessary hardware setup is outlined and experimental waveforms are shown. Several practical use cases are outlined.

Note that the TestBoard, and therefore this entire document, is only applicable to / supported by AMDC REV D hardware.

## Hardware Setup

To use the TestBoard, you will need to plug it into the AMDC. This requires four analog cables (like Ethernet cables) and one power stack cable (high density DB15 cable).

If you need to buy cables, see some recommended options [here](https://github.com/Severson-Group/AMDC-Hardware/tree/develop/Accessories/TestBoard/REV20200624A#recommended-cables).

#### AMDC Analog Port Connections

The AMDC has a 2x2 RJ45 jack which the analog input voltages are read from. The TestBoard has the same jack. Use the four ethernet cables to connection these two jacks together.

When looking at the jacks from the front, the locations match on the AMDC and TestBoard. For example, the top left port on the AMDC connects to the top left port on the TestBoard.

#### AMDC PowerStack Port Connection

The AMDC has eight PowerStack ports on the front, arranged in four stacks of two DB15 connectors. The TestBoard has a single DB15 connector. Plug in the DB15 cable into one of the eight AMDC PowerStack ports. Plug the other end into the TestBoard.

#### TestBoard Configuration

The TestBoard has a few jumpers which need to be configured.

Ensure the DIP switches are in the OFF position (i.e. the switches are open).

Ensure only one jumper (or no jumper) is installed across JP1 to JP6. This jumper connects the `VOUT` analog voltage to the BNC jack on the TestBoard. If you are not using the BNC jack, this jumper is optional.
