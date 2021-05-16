# DLTPower

DLTPower is used to control Power Supplies, Multimeter and Relais for Test Automation.
It is developed for an Arduino SW running on a Wemos Mini D1 HW, several shields.
Differenr Multimeters and Power supplies are supported.
It provides an interface to DLT for Logging and Test Automation.

![Image of DLTPower](https://github.com/alexmucde/DLTPower/blob/main/doc/images/DLTPower.jpg)

![Image of Wemos Relais Board](https://github.com/alexmucde/DLTPower/blob/main/doc/images/WemosRelaisBoard.jpg)

For further information about DLT visit the DLT Viewer project:

https://github.com/GENIVI/dlt-viewer

## Supported Hardware

The following Hardware is currently supported:

* Relais Boards: Arduino Wemos Mini D1 + Relais Shields
* Multimeter: HoldPeak HP-90EPC [Amazon Germany](https://amzn.to/3bIsPwT) [AliExpress](https://s.click.aliexpress.com/e/_AM1Vsi)
  * Interface: USB Serial
  * Protocol: https://sigrok.org/wiki/Multimeter_ICs#Fortune_Semiconductor_FS9721_LP3
  * USB Driver: https://www.holdpeak.de/download/
* Power supply: Manson HCS-3302 or compatible (e.g. Voltcraft) [Amazon Germany](https://amzn.to/2QUWQ4D)
  * Interface: USB Serial
  * Protocol: See user manual

## Wemos Mini D1 Hardware

The following parts are needed to setup a Relais board:

* Wemos D1 Mini [Amazon Germany](https://amzn.to/3thvzYd) [AliExpress](https://s.click.aliexpress.com/e/_AXoYOK)
* 1-3 Wemos Relais Shields [Amazon Germany](https://amzn.to/3csEJdE) [AliExpress](https://s.click.aliexpress.com/e/_9JeBua)
* Triple Base or Dual Base [Amazon Germany](https://amzn.to/3eyI9Ov) [AliExpress Triple Base](https://s.click.aliexpress.com/e/_AXI4VC) [AliExpress Dual Base](https://s.click.aliexpress.com/e/_9In2Z0)
* Optional: 1-Button Shield [AliExpress](https://s.click.aliexpress.com/e/_9INwTG)
* Optional: Buzzer Shield [AliExpress](https://s.click.aliexpress.com/e/_A7sRs2)

If more than one Relais Shields are used, the pin D1 must be removed and connected to D2 (second Relais) or D6 (third Relais).

## Wemos Mini D1 Arduino SW

The following Arduino SW is needed [DLTRelais.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/DLTRelais/DLTRelais.ino)

The Arduino SW is based on the Wemos Library:

[Wemos Library](https://github.com/alexmucde/WemosLibrary)

Compile, upload and run the SW with the [Arduino IDE](https://www.arduino.cc/en/software).

Clone or copy the Wemos Library into the Arduino Libraries folder before compiling the [DLTRelais.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/DLTRelais/DLTRelais.ino) sketch.

### Features

The Arduino SW provides the following Features:

* Activate and deactivate three Relais
* Trigger the Relais for short time (500ms)
* If a button shield is connected, a short press triggers the Relais connected to pin D1
* If a button shield is connected, a long ppress toggles the Relais connected to pin D2

### Protocol

The Wemos D1 Mini is connected by a virtual serial device on USB. The serial port settings are 115.200 Baud with 8N1 and no handshake.

A USB driver is needed which can be found here:

https://www.wemos.cc/en/latest/ch340_driver.html

The Arduino SW understands the following commands:

* "R11\n", "R21\n", "R31\n", "R41\n", "R51\n" Activates Relais connected to pin D1, D2, D6, D7 and D8
* "R10\n", "R20\n", "R30\n", "R40\n", "R50\n" Deactivates Relais connected to pin D1, D2, D6, D7 and D8
* "R1T\n", "R2T\n", "R3T\n", "R4T\n", "R5T\n" Trigger Relais connected to pin D1, D2, D6, D7 and D8 for 500ms

The Arduino board sends the status of Relais, when it was triggered with the button with the same command.

The Arduino board sends every second a watchdog signal "WD\n".

## DLT injection commands

The commands are interpreted independent of the service id.

* <Relais name> on/off/trigger
* <Power name> on/off

## Installation

To build this SW the Qt Toolchain must be used.

## Usage

* DLTPower.exe [options] configuration

* Options:
*  -?, -h, --help          Help
*  -v, --version           Version
*  -a                      Autostart Communication

* Arguments:
*  configuration           Configuration file

## Contributing

Contibutions are always welcome! Please provide a Pull Request on Github.

https://github.com/alexmucde/DLTPower

## Donation

If you find this SW useful and you want to donate my work please select one of the following donations:

Paypal Donation:

[![Donations](https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=YBWSNXYWJJP2Q)

Github Sponsors:

[:heart: Sponsor](https://github.com/sponsors/alexmucde)

## Changes

v0.0.6:

* added DLT Injection feature to control Relais and Power

v0.0.5:

* support also voltage in Multimeter
* Autodetect COM port change
* Reduce to support only 3 Relais per board

v0.0.4:

* Rename from DLTRelais to DLTPower
* Integrate DLTMultimeter
* Support of Power Supply
* Support two Relais Boards and two Multimeter/PowerSupplies
* Add active flag for each device

v0.0.3:

* Support up to 5 Relais
* Changed communication protocol with Arduino board
* Added Watchdog
* Fixed startup problem with Arduino Flash Mode
* Added command line options autoload and configuration file

## Copyright

Alexander Wenzel <alex@eli2.de>

This code is licensed under GPLv3.
