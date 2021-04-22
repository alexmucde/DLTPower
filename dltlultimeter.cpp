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

DLTMultimeter::DLTMultimeter(QObject *parent) : QObject(parent)
{
    clearSettings();

    value = 0;
    lastValue = -1;
}

DLTMultimeter::~DLTMultimeter()
{
    stop();
}

void DLTMultimeter::start()
{
    value = 0;
    lastValue = -1;

    // set serial port parameters
    serialPort.setBaudRate(QSerialPort::Baud2400);
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

    // connect slot watchdog timer and start watchdog timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(5000);
    watchDogCounter = 0;
    watchDogCounterLast = 0;
}

void DLTMultimeter::stop()
{
    // stop communication
    status(QString("stopped"));
    qDebug() << "DLTMultimeter: stopped" << interface;

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
                if(lastValue!=value)
                {
                    valueMultimeter(QString("%1").arg(value),unit);
                    lastValue = value;
                }
                watchDogCounter++;
                qDebug() << "DLTMultimeter: Value received" << interface << value << unit;
                break;
            default:
                rawData+=data[num];
                break;
        }
    }
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
}

void DLTMultimeter::writeSettings(QXmlStreamWriter &xml)
{
    /* Write project settings */
    xml.writeStartElement("DLTMultimeter");
        xml.writeTextElement("interface",interface);
    xml.writeEndElement(); // DLTMultimeter
}

void DLTMultimeter::readSettings(const QString &filename)
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
              }
              else if(xml.name() == QString("DLTMultimeter"))
              {
                    isDLTMultimeter = true;
              }
          }
          else if(xml.isEndElement())
          {
              /* Connection, plugin and filter */
              if(xml.name() == QString("DLTMultimeter"))
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

}
