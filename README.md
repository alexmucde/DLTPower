# DLTRelais

DLTRelais is used to control Relais for Test Automation.
It is developed for an Arduino SW running on a Wemos Mini D1 HW and several shields.
It provides an interface to DLT for Logging and Test Automation.

![Image of DLTRelais](https://github.com/alexmucde/DLTRelais/blob/main/doc/images/DLTRelais.jpg)

For further information about DLT visit the DLT Viewer project:

https://github.com/GENIVI/dlt-viewer

## Wemos Mini D1 Hardware

The following parts are needed to setup a Relais board:

* Wemos D1 Mini [Amazon Germany](https://amzn.to/3thvzYd) [AliExpress](https://best.aliexpress.com)
* 1-3 Wemos Relais Shields [Amazon Germany](https://amzn.to/3csEJdE) [AliExpress](https://best.aliexpress.com)
* Tripler Base or Dual Base [Amazon Germany](https://amzn.to/3eyI9Ov) [AliExpress](https://best.aliexpress.com)
* Optional: 1-Button Shield [AliExpress](https://best.aliexpress.com)
* Optional: Buzzer Shield [AliExpress](https://best.aliexpress.com)

If more than one Relais Shields are used, the pin D1 must be removed and connected to D2 (second Relais) or D6 (third Relais).

## Wemos Mini D1 Arduino SW

The following Arduino SW is needed [DLTRelais.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/DLTRelais/DLTRelais.ino)

The Arduino SW is based on the Wemos Library:

[Wemos Library](https://github.com/alexmucde/WemosLibrary)

Compile, upload and run the SW with the [Arduino IDE](https://www.arduino.cc/en/software).

Copy the Wemos Library into the Arduino Libraries folder before compiling the [DLTRelais.ino](https://github.com/alexmucde/DLTRelais/blob/main/arduino/DLTRelais/DLTRelais.ino) sketch.

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

[![Donations](https://www.paypalobjects.com/en_US/DK/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate?hosted_button_id=YBWSNXYWJJP2Q)https://www.paypal.com/donate?hosted_button_id=YBWSNXYWJJP2Q

Github Sponsors:

[:heart: Sponsor](https://github.com/sponsors/alexmucde)

## Copyright

Alexander Wenzel <alex@eli2.de>

This code is licensed under GPLv3.
