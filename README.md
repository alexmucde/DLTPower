# DLTRelais

DLTRelais is used to control Relais for Test Automation.
It is developed for an Arduino SW running on a Wemos Mini D1 HW and several shields.
It provides an interface to DLT for Logging and Test Automation.

![Image of DLTRelais](https://github.com/alexmucde/DLTRelais/blob/main/doc/images/DLTRelais.jpg)

For further information about DLT visit the DLT Viewer project:

https://github.com/GENIVI/dlt-viewer

## Wemos Mini D1 Hardware

The following parts are needed to setup a Relais board:

* Wemos D1 Mini [Amazon Germany](https://amzn.to/3thvzYd) [AliExpress](https://s.click.aliexpress.com/e/_AXoYOK)
* 1-3 Wemos Relais Shields [Amazon Germany](https://amzn.to/3csEJdE) [AliExpress](https://s.click.aliexpress.com/e/_9JeBua)
* Triple Base or Dual Base [Amazon Germany](https://amzn.to/3eyI9Ov) [AliExpress Triple Base](https://s.click.aliexpress.com/e/_AXI4VC) [AliExpress Dual Base](https://s.click.aliexpress.com/e/_9In2Z0)
* Optional: 1-Button Shield [AliExpress](https://best.aliexpress.com) [AliExpress](https://s.click.aliexpress.com/e/_9INwTG)
* Optional: Buzzer Shield [AliExpress](https://best.aliexpress.com) [AliExpress](https://s.click.aliexpress.com/e/_A7sRs2)

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

* "D11\n", "D21\n"  or "D61\n"  Activate Relais connected to pin D1, D2 or D6
* "D10\n", "D20\n"  or "D60\n" Deactivate Relais connected to pin D1, D2 or D6
* "D1T\n", "D2T\n"  or "D6T\n" Trigger Relais connected to pin D1, D2 or D6 for 500ms

## Installation

To build this SW the Qt Toolchain must be used.

## Usage

DLTRelais.exe

## Contributing

Contibutions are always welcome! Please provide a Pull Request on Github.

https://github.com/alexmucde/DLTRelais

## Donation

If you find this SW useful and you want to donate my work please select one of the following donations:

Paypal Donation:

[![Donations](https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=YBWSNXYWJJP2Q)

Github Sponsors:

[:heart: Sponsor](https://github.com/sponsors/alexmucde)

## Copyright

Alexander Wenzel <alex@eli2.de>

This code is licensed under GPLv3.
