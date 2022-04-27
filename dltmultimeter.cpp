/**
 * @licence app begin@
 * Copyright (C) 2021 Alexander Wenzel
 *
 * This file is part of the DLT Multimeter project.
 *
 * \copyright This code is licensed under GPLv3.
 *
 * \author Alexander Wenzel <alex@eli2.de>
 *
 * \file dltmultimeter.cpp
 * @licence end@
 */

#include "dltmultimeter.h"

#include <QDebug>
#include <QFile>
#include <QSerialPortInfo>

DLTMultimeter::DLTMultimeter(QObject *parent) : QObject(parent)
{
    clearSettings();

    value = 0;
    type = 0;
    powerName = "Power";
    readVoltageOngoing = false;
}

DLTMultimeter::~DLTMultimeter()
{
    stop();
}

void DLTMultimeter::checkPortName()
{
    /* check if information is stored, if not do not check */
    if(interfaceSerialNumber.isEmpty() && interfaceProductIdentifier==0 && interfaceVendorIdentifier==0)
        return;

    /* check if port information still matches port name */
    if((QSerialPortInfo(interface).serialNumber()!=interfaceSerialNumber ||
       QSerialPortInfo(interface).productIdentifier()!=interfaceProductIdentifier ||
       QSerialPortInfo(interface).vendorIdentifier()!=interfaceVendorIdentifier))
    {
        qDebug() << "Port" << interface << "not found anymore";

        /* port name has changed, try to find new port name */
        QList<QSerialPortInfo> 	availablePorts  = QSerialPortInfo::availablePorts();
        for(int num = 0; num<availablePorts.length();num++)
        {
            if(availablePorts[num].serialNumber()==interfaceSerialNumber &&
               availablePorts[num].productIdentifier()==interfaceProductIdentifier &&
               availablePorts[num].vendorIdentifier()==interfaceVendorIdentifier)
            {
                // The following is not working, if several interfaces have the same name
                // Must be improved in future, e.g. with Serial Id of each device
                // qDebug() << "Port name has changed from" << interface << "to" << availablePorts[num].portName();
                // interface = availablePorts[num].portName();
            }
        }
    }
}

void DLTMultimeter::start()
{
    if(!active)
    {
        status(QString("not active"));
        return;
    }

    // start communication
    // checkPortName();

    value = 0;

    // set serial port parameters
    if(type==0) // Holdpeak HP-90EPC
        serialPort.setBaudRate(QSerialPort::Baud2400);
    else if(type==1) // Mason HCS-3302 USB
        serialPort.setBaudRate(QSerialPort::Baud9600);
    else if(type==2) // WemosD1MiniPower
        serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setPortName(interface);

    // open serial port
    if(serialPort.open(QIODevice::ReadWrite)==true)
    {
        // open with success

        // connect slot to receive data from serial port
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

        status(QString("started"));
        qDebug() << "DLTMultimeter: started" << interface;
     }
    else
    {
        // open failed

        qDebug() << "DLTMultimeter: Failed to open interface" << interface;
        status(QString("error"));
    }

    if(type==1) // Mason HCS-3302 USB
    {
        timerRequest.start(1000);
        connect(&timerRequest, SIGNAL(timeout()), this, SLOT(timeoutRequest()));
    }

    serialData.clear();

    // connect slot watchdog timer and start watchdog timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(5000);
    watchDogCounter = 0;
    watchDogCounterLast = 0;
    readVoltageOngoing = false;
}

void DLTMultimeter::stop()
{
    if(!active)
    {
        return;
    }

    // stop communication
    status(QString("stopped"));
    qDebug() << "DLTMultimeter: stopped" << interface;

    if(type==1) // Mason HCS-3302 USB
    {
        timerRequest.stop();
        disconnect(&timerRequest, SIGNAL(timeout()), this, SLOT(timeoutRequest()));
    }

    // close serial port, if it is open
    if(serialPort.isOpen())
    {
        serialPort.close();

        // disconnect slot to receive data from serial port
        disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }

    // stop watchdog timer
    timer.stop();
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

void DLTMultimeter::readyRead()
{
    if(type==0) // Holdpeak HP-90EPC
    {
        QByteArray data = serialPort.readAll();
        for(int num=0;num<data.length();num++)
        {
            switch(data[num]&0xf0)
            {
                case 0x10:
                    rawData.clear();
                    rawData+=data[num];
                    break;
                case 0xe0:
                    data.clear();
                    rawData+=data[num];
                    qDebug() << "DLTMultimeter: Raw Data " << rawData.toHex();
                    calculateValue();
                    valueMultimeter(QString("%1").arg(value),unit);
                    watchDogCounter++;
                    qDebug() << "DLTMultimeter: Value received" << interface << value << unit;
                    break;
                default:
                    rawData+=data[num];
                    break;
            }
        }
    }
    else if(type==1) // Mason HCS-3302 USB
    {
        // loop as long as data is available
        while(serialPort.bytesAvailable())
        {
            // read one line form serial port
            serialData += serialPort.readAll();

            int pos;
            pos = serialData.indexOf('\r');
            while(pos!=-1)
            {
                QString line(serialData.mid(0,pos));

                qDebug() << "DLTMultimeter: readLine" << line;

                if(line=="OK")
                {
                    watchDogCounter++;
                }
                else if(line.length()==9)
                {
                    float current = (QString("%1%2.%3%4").arg(line[4]).arg(line[5]).arg(line[6]).arg(line[7])).toFloat() - substractCurrent;
                    valueMultimeter(QString("%1").arg(current),"A");
                    valueMultimeter(QString("%1%2.%3%4").arg(line[0]).arg(line[1]).arg(line[2]).arg(line[3]),"V");
                    readVoltageOngoing = false;
                    if(!voltageCmd.isEmpty())
                    {
                        serialPort.write(voltageCmd.toLatin1());
                        voltageCmd.clear();
                    }
                }

                serialData.remove(0,pos+1);
                pos = serialData.indexOf('\r');
            }
        }
    }
    else if(type==2) // WemosD1MiniPower
    {
        // loop as long as data is available
        while(serialPort.bytesAvailable())
        {
            char data[256];

            // read one line form serial port
            qint64 size = serialPort.readLine(data,sizeof(data));

            if(size>0)
            {
                // line is not empty
                qDebug() << "DLTMultimeter: readLine" << data;

                if(QString(data) == "WDMP\r\n")
                {
                    // watchdog message received
                    watchDogCounter++;
                }
                else if(QString(data) == "failed\r\n")
                {
                }
                else if(data[0]=='C')
                {
                    QString text = QString(data);
                    QStringList list = text.split(' ');
                    float current = list[1].toFloat();
                    valueMultimeter(QString("%1").arg(current/(float)1000),"A");
                }
                else if(data[0]=='V')
                {
                    QString text = QString(data);
                    QStringList list = text.split(' ');
                    float voltage = list[1].toFloat();
                    valueMultimeter(QString("%1").arg(voltage),"V");
                }
            }
        }
    }
}

void DLTMultimeter::timeoutRequest()
{
    serialPort.write("GETD\r");
    readVoltageOngoing = true;
}

float DLTMultimeter::getSubstractCurrent() const
{
    return substractCurrent;
}

void DLTMultimeter::setSubstractCurrent(float newSubstractCurrent)
{
    substractCurrent = newSubstractCurrent;
}

void DLTMultimeter::timeout()
{
    // watchdog timeout

    // check if watchdog was triggered between last call
    if(watchDogCounter!=watchDogCounterLast)
    {
        watchDogCounterLast = watchDogCounter;
        status(QString("started"));
    }
    else
    {
        // no watchdog was received
        qDebug() << "DLTMultimeter: Watchdog expired try to reconnect" ;

        // if serial port is open close serial port
        if(serialPort.isOpen())
        {
            serialPort.close();
            disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
        }

        // check if port name has changed
        // checkPortName();

        serialData.clear();

        // try to reopen serial port
        if(serialPort.open(QIODevice::ReadWrite)==true)
        {
            // retry was succesful

            // connect slot to receive data from serial port
            connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

            status(QString("reconnect"));
            qDebug() << "DLTMultimeter: reconnect" << interface;
        }
        else
        {
            // retry failed

            qDebug() << "DLTMultimeter: Failed to open interface" << interface;
            status(QString("error"));
        }
    }

}

void DLTMultimeter::clearSettings()
{
    type = 0;
    powerName = "Power";
    active = 0;
    substractCurrent = 0;

    interfaceSerialNumber = "";
    interfaceProductIdentifier = 0;
    interfaceVendorIdentifier = 0;
}

void DLTMultimeter::writeSettings(QXmlStreamWriter &xml,int num)
{
    /* Write project settings */
    xml.writeStartElement(QString("DLTMultimeter%1").arg(num));
        xml.writeTextElement("interface",interface);
        xml.writeTextElement("interfaceSerialNumber",QSerialPortInfo(interface).serialNumber());
        xml.writeTextElement("interfaceProductIdentifier",QString("%1").arg(QSerialPortInfo(interface).productIdentifier()));
        xml.writeTextElement("interfaceVendorIdentifier",QString("%1").arg(QSerialPortInfo(interface).vendorIdentifier()));
        xml.writeTextElement("type",QString("%1").arg(type));
        xml.writeTextElement("powerName",powerName);
        xml.writeTextElement("substractCurrent",QString("%1").arg(substractCurrent));
        xml.writeTextElement("active",QString("%1").arg(active));
    xml.writeEndElement(); // DLTMultimeter
}

void DLTMultimeter::readSettings(const QString &filename,int num)
{
    bool isDLTMultimeter = false;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
             return;

    QXmlStreamReader xml(&file);

    while (!xml.atEnd())
    {
          xml.readNext();

          if(xml.isStartElement())
          {
              if(isDLTMultimeter)
              {
                  /* Project settings */
                  if(xml.name() == QString("interface"))
                  {
                      interface = xml.readElementText();
                  }
                  else if(xml.name() == QString("interfaceSerialNumber"))
                  {
                      interfaceSerialNumber = xml.readElementText();
                  }
                  else if(xml.name() == QString("interfaceProductIdentifier"))
                  {
                      interfaceProductIdentifier = xml.readElementText().toUShort();
                  }
                  else if(xml.name() == QString("interfaceVendorIdentifier"))
                  {
                      interfaceVendorIdentifier = xml.readElementText().toUShort();
                  }
                  else if(xml.name() == QString("interfaceSerialNumber"))
                  {
                      interfaceSerialNumber = xml.readElementText();
                  }
                  else if(xml.name() == QString("interfaceProductIdentifier"))
                  {
                      interfaceProductIdentifier = xml.readElementText().toUShort();
                  }
                  else if(xml.name() == QString("interfaceVendorIdentifier"))
                  {
                      interfaceVendorIdentifier = xml.readElementText().toUShort();
                  }
                  else if(xml.name() == QString("type"))
                  {
                      type = xml.readElementText().toInt();
                  }
                  else if(xml.name() == QString("powerName"))
                  {
                      powerName = xml.readElementText();
                  }
                  else if(xml.name() == QString("substractCurrent"))
                  {
                      substractCurrent = xml.readElementText().toFloat();
                  }
                  else if(xml.name() == QString("active"))
                  {
                      active = xml.readElementText().toInt();
                  }
              }
              else if(xml.name() == QString("DLTMultimeter%1").arg(num))
              {
                    isDLTMultimeter = true;
              }
          }
          else if(xml.isEndElement())
          {
              /* Connection, plugin and filter */
              if(xml.name() == QString("DLTMultimeter%1").arg(num))
              {
                    isDLTMultimeter = false;
              }
          }
    }
    if (xml.hasError())
    {
         qDebug() << "Error in processing filter file" << filename << xml.errorString();
    }

    file.close();
}

void DLTMultimeter::calculateValue()
{
    if(rawData.length()!=14)
        return;

    value=calculateNumber(rawData[1]&0x7,rawData[2]&0xf);
    value=value*10+calculateNumber(rawData[3]&0x7,rawData[4]&0xf);
    value=value*10+calculateNumber(rawData[5]&0x7,rawData[6]&0xf);
    value=value*10+calculateNumber(rawData[7]&0x7,rawData[8]&0xf);

    if(rawData[1]&0x08) value*=-1;

    if(rawData[3]&0x08) value/=1000;
    if(rawData[5]&0x08) value/=100;
    if(rawData[7]&0x08) value/=10;

    if(rawData[10]&0x08) value/=1000; // milli
    if(rawData[9]&0x08) value/=1000; // mycro

    if(rawData[12]&0x08)
        unit = "A";
    else if(rawData[12]&0x04)
        unit = "V";
    else
        unit = "unknown";
}

int DLTMultimeter::calculateNumber(unsigned char a,unsigned char b)
{
    if(a==0x7 && b==0x0d)
        return 0;
    else if(a==0x0 && b==0x05)
        return 1;
    else if(a==0x5 && b==0x0b)
        return 2;
    else if(a==0x1 && b==0x0f)
        return 3;
    else if(a==0x2 && b==0x07)
        return 4;
    else if(a==0x3 && b==0x0e)
        return 5;
    else if(a==0x7 && b==0x0e)
        return 6;
    else if(a==0x1 && b==0x05)
        return 7;
    else if(a==0x7 && b==0x0f)
        return 8;
    else if(a==0x3 && b==0x0f)
        return 9;
    else
        return 0;

}

void DLTMultimeter::on()
{
    if(type==1) // Mason HCS-3302 USB
    {
        serialPort.write("SOUT0\r");

        qDebug() << "DLTMultimeter: power on";
    }
}

void DLTMultimeter::off()
{
    if(type==1) // Mason HCS-3302 USB
    {
        serialPort.write("SOUT1\r");

        qDebug() << "DLTMultimeter: power off";
    }
}

void DLTMultimeter::setVoltage(float value)
{
    if(type==1) // Mason HCS-3302 USB
    {
        QString text;
        
        // Voltage set cmd sintax: VOLT### (### is the voltage in dV)
        text = QString("VOLT%L1\r").arg((int)(value*10), 3, 10, QLatin1Char('0'));

        if(!readVoltageOngoing)
        {
            serialPort.write(text.toLatin1());
        }
        else
        {
            voltageCmd = text;
        }

        qDebug() << "DLTMultimeter: setVoltage" << text;
    }
}
