# DLTPower

DLTPower is used to control Power Supplies, Multimeter and Relais for Test Automation.
It is developed for an Arduino SW running on a Wemos Mini D1 HW or Wemos D1 HW and several shields.
Different Multimeters and Power supplies are supported.
It provides an interface to DLT for Logging and Test Automation.

![Image of DLTPower](https://github.com/alexmucde/DLTPower/blob/main/doc/images/DLTPower.jpg)

![Image of Wemos D1 Mini Relais Board](https://github.com/alexmucde/DLTPower/blob/main/doc/images/WemosD1MiniRelaisBoard.jpg)

![Image of Wemos D1 Relais Board](https://github.com/alexmucde/DLTPower/blob/main/doc/images/WemosD1RelaisBoard.jpg)

For further information about DLT visit the DLT Viewer project:

https://github.com/GENIVI/dlt-viewer

## Supported Hardware

The following Hardware is currently supported:

* Relais Board: Arduino Wemos Mini D1 + Relais Shields
* Relais Board: Arduino Wemos D1 + Arduino 4 Relais Shields HW-68
* Multimeter: HoldPeak HP-90EPC [Amazon Germany](https://amzn.to/3bIsPwT) [AliExpress](https://s.click.aliexpress.com/e/_AM1Vsi)
  * Interface: USB Serial
  * Protocol: https://sigrok.org/wiki/Multimeter_ICs#Fortune_Semiconductor_FS9721_LP3
  * USB Driver: https://www.holdpeak.de/download/
* Power supply: Manson HCS-3302 or Voltcraft PPS-11810 or compatible [Amazon Germany Manson](https://amzn.to/2QUWQ4D) [Amazon Germany Voltcraft](https://amzn.to/3e33EWJ)
  * Interface: USB Serial
  * Protocol: See user manual

## Wemos Mini D1 Hardware

The following parts are needed to setup a Relais board:

* Arduino Wemos Mini D1
  * Wemos D1 Mini [Amazon Germany](https://amzn.to/3thvzYd) [AliExpress](https://s.click.aliexpress.com/e/_AXoYOK)
  * 1-3 Wemos Relais Shields [Amazon Germany](https://amzn.to/3csEJdE) [AliExpress](https://s.click.aliexpress.com/e/_9JeBua)
  * If more than one Relais Shields are used, the pin D1 must be removed and connected to D2 (second Relais) or D6 (third Relais).
  * Triple Base or Dual Base [Amazon Germany](https://amzn.to/3eyI9Ov) [AliExpress Triple Base](https://s.click.aliexpress.com/e/_AXI4VC) [AliExpress Dual Base](https://s.click.aliexpress.com/e/_9In2Z0)
  * Optional: 1-Button Shield [AliExpress](https://s.click.aliexpress.com/e/_9INwTG)

* Arduino Wemos D1
  * Wemos D1 [Amazon Germany](https://amzn.to/3fdFrOu) [AliExpress](https://s.click.aliexpress.com/e/_AMlgFl)
  * Arduino 4 Relais Shield [Amazon Germany](https://amzn.to/3fBYlgY) [AliExpress](https://s.click.aliexpress.com/e/_A4QDGb)

## Arduino Wemos Mini D1 SW

The following Arduino SW is needed [WemosD1MiniRelais3Button.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/WemosD1MiniRelais3Button/WemosD1MiniRelais3Button.ino)

The Arduino SW is based on the Wemos Library v0.0.1:

[Wemos Library](https://github.com/alexmucde/WemosLibrary)

Compile, upload and run the SW with the [Arduino IDE](https://www.arduino.cc/en/software).

Clone or copy the Wemos Library into the Arduino Libraries folder before compiling the sketch.

## Arduino Wemos D1 SW 4 Relais Shield

The following Arduino SW is needed [WemosD1Relais4Shield.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/WemosD1Relais4Shield/WemosD1Relais4Shield.ino)

The Arduino SW is based on the Wemos Library v0.0.1:

[Wemos Library](https://github.com/alexmucde/WemosLibrary)

Compile, upload and run the SW with the [Arduino IDE](https://www.arduino.cc/en/software).

Clone or copy the Wemos Library into the Arduino Libraries folder before compiling the sketch.

### Features

The Arduino SW provides the following Features:

* Activate and deactivate up to four Relais
* Trigger the Relais for short time (500ms or custom value)
* If a button shield is connected, a short press triggers the Relais 1 connected to pin D1 (only Wemos D1 Mini)
* If a button shield is connected, a long press toggles the Relais 2 connected to pin D2 (only Wemos D1 Mini)

### Protocol

The Wemos D1 Mini and Wemos D1 is connected by a virtual serial device on USB. The serial port settings are 115.200 Baud with 8N1 and no handshake.

A USB driver is needed which can be found here:

https://www.wemos.cc/en/latest/ch340_driver.html

The Arduino SW understands the following commands:

* "R11\n", "R21\n", "R31\n", "R41\n" Activates Relais connected to pin D1, D2, D6 and D7 (Wemos D1 Mini) or D7, D6, D5, D4 (Wemos D1)
* "R10\n", "R20\n", "R30\n", "R40\n" Deactivates Relais connected to pin D1, D2, D6 and D7 (Wemos D1 Mini) or D7, D6, D5, D4 (Wemos D1)
* "R1T\n", "R2T\n", "R3T\n", "R4T\n" Trigger Relais connected to pin D1, D2, D6 and D7 (Wemos D1 Mini) or D7, D6, D5, D4 (Wemos D1) for 500ms
* "R1T<time in ms>\n", "R2T<time in ms>\n", "R3T<time in ms>\n", "R4T<time in ms>\n" Trigger Relais connected to pin D1, D2, D6 and D7 (Wemos D1 Mini) or D7, D6, D5, D4 (Wemos D1) for the defined time in ms

The Arduino board sends the status of Relais, when it was triggered with the button with the same command.

The Arduino board sends every second a watchdog signal "WD\n".

## DLT injection commands

The commands are interpreted independent of the service id.

* \<Relais name\> on/off/trigger
* \<Relais name\> trigger \<time in ms\>
* \<Power name\> on/off
* VOLTAGE1 \<value\>
* VOLTAGE2 \<value\>

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

v0.1.2:

* Fix PS/MM setVoltage to match the 3-digit syntax 
* Added feature to substract current

v0.1.1:

* Fix Relais Board 2 activating wrong Relais
* Update build script to Qt 5.15.2 and MSVC 2019

v0.1.0:

* Fixed set voltage problems
* Remove automatic port name detection
* Fix relais pins
* Configurable trigger time by injection

v0.0.9:

* configurable trigger time by injection
* user configurable trigger time in Arduino sketch

v0.0.8:

* added DLT injection for setting voltage

v0.0.7:

* support new Relais Board Wemos D1 + 4 Relais shield
* support up to 4 Relais per board

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
