/**
 * @licence app begin@
 * Copyright (C) 2021 Alexander Wenzel
 *
 * This file is part of the DLT Relais project.
 *
 * \copyright This code is licensed under GPLv3.
 *
 * \author Alexander Wenzel <alex@eli2.de>
 *
 * \file dltrelais.h
 * @licence end@
 */

#include "dltrelais.h"

#include <QDebug>
#include <QFile>

DLTRelais::DLTRelais(QObject *parent) : QObject(parent)
{
    clearSettings();
}

DLTRelais::~DLTRelais()
{
    stop();
}

void DLTRelais::start()
{
    if(!active)
    {
        status(QString("not active"));
        return;
    }

    // start communication

    // set serial port parameters
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.setPortName(interface);

    // open serial port
    if(serialPort.open(QIODevice::ReadWrite)==true)
    {
        // open with success

        // prevent flash mode of Wemos D1 mini
        serialPort.setDataTerminalReady(false);

        // connect slot to receive data from serial port
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

        status(QString("started"));
        qDebug() << "DLTRelais: started" << interface;
    }
    else
    {
        // open failed

        qDebug() << "DLTRelais: Failed to open interface" << interface;
        status(QString("error"));
    }

    // connect slot watchdog timer and start watchdog timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(5000);
    watchDogCounter = 0;
    watchDogCounterLast = 0;
}

void DLTRelais::stop()
{
    if(!active)
    {
        return;
    }

    // stop communication
    status(QString("stopped"));
    qDebug() << "DLTRelais: stopped" << interface;

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

void DLTRelais::readyRead()
{
    // data on serial port was received

    // loop as long as data is available
    while(serialPort.bytesAvailable())
    {
        char data[256];

        // read one line form serial port
        qint64 size = serialPort.readLine(data,sizeof(data));

        if(size>0)
        {
            // line is not empty
            qDebug() << "DLTRelais: readLine" << data;

            if(QString(data) == "WD\r\n")
            {
                // watchdog message received
                watchDogCounter++;
            }
            else
            {
                // all other messages forward to status signal
                status(QString(data));
            }
        }
    }
}

void DLTRelais::timeout()
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
        qDebug() << "DLTRelais: Watchdog expired try to reconnect" ;

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

            // prevent flash mode of Wemos D1 mini
            serialPort.setDataTerminalReady(false);

            // connect slot to receive data from serial port
            connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

            status(QString("reconnect"));
            qDebug() << "DLTRelais: reconnect" << interface;
        }
        else
        {
            // retry failed

            qDebug() << "DLTRelais: Failed to open interface" << interface;
            status(QString("error"));
        }
    }
}

void DLTRelais::clearSettings()
{
    // clear settings
    for(int num=0;num<3;num++)
        relaisName[num] = QString("Relais%1").arg(num+1);

    active = 0;
}

void DLTRelais::writeSettings(QXmlStreamWriter &xml,int num)
{
    // Write project settings to XML file
    xml.writeStartElement(QString("DLTRelais%1").arg(num));
        xml.writeTextElement("relaisName1",relaisName[0]);
        xml.writeTextElement("relaisName2",relaisName[1]);
        xml.writeTextElement("relaisName3",relaisName[2]);
        xml.writeTextElement("interface",interface);
        xml.writeTextElement("active",QString("%1").arg(active));
    xml.writeEndElement(); // DLTRelais
}

void DLTRelais::readSettings(const QString &filename,int num)
{
    // read settings from XML file
    bool isDLTRelais = false;

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text))
             return;

    QXmlStreamReader xml(&file);

    while (!xml.atEnd())
    {
          xml.readNext();

          if(xml.isStartElement())
          {
              if(isDLTRelais)
              {
                  /* Project settings */
                  if(xml.name() == QString("relaisName1"))
                  {
                      relaisName[0] = xml.readElementText();
                  }
                  if(xml.name() == QString("relaisName2"))
                  {
                      relaisName[1] = xml.readElementText();
                  }
                  if(xml.name() == QString("relaisName3"))
                  {
                      relaisName[2] = xml.readElementText();
                  }
                  if(xml.name() == QString("interface"))
                  {
                      interface = xml.readElementText();
                  }
                  if(xml.name() == QString("active"))
                  {
                      active = xml.readElementText().toInt();
                  }
              }
              else if(xml.name() == QString("DLTRelais%1").arg(num))
              {
                    isDLTRelais = true;
              }
          }
          else if(xml.isEndElement())
          {
              /* Connection, plugin and filter */
              if(xml.name() == QString("DLTRelais%1").arg(num))
              {
                    isDLTRelais = false;
              }
          }
    }
    if (xml.hasError())
    {
         qDebug() << "Error in processing filter file" << filename << xml.errorString();
    }

    file.close();
}

void DLTRelais::trigger(int num)
{
    // trigger a Relais for 500ms
    qDebug() << "DLTRelais: trigger" << num;

    if(num==1)
        serialPort.write("R1T\n");
    else if(num==2)
        serialPort.write("R2T\n");
    else if(num==3)
        serialPort.write("R3T\n");
}

void DLTRelais::on(int num)
{
    // set Relais to on
    qDebug() << "DLTRelais: on" << num;

    if(num==1)
        serialPort.write("R11\n");
    else if(num==2)
        serialPort.write("R21\n");
    else if(num==3)
        serialPort.write("R31\n");
}

void DLTRelais::off(int num)
{
    // set Relais to off
    qDebug() << "DLTRelais: off" << num;

    if(num==1)
        serialPort.write("R10\n");
    else if(num==2)
        serialPort.write("R20\n");
    else if(num==3)
        serialPort.write("R30\n");
}
