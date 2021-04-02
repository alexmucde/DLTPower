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
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.setPortName(interface);

    if(serialPort.open(QIODevice::ReadWrite)==true)
    {
        serialPort.setDataTerminalReady(false);
        status(QString("started"));
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
        qDebug() << "DLTRelais: started" << interface;
    }
    else
    {
        qDebug() << "DLTRelais: Failed to open interface" << interface;
        status(QString("error"));
    }

    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(3000);
    watchDogCounter = 0;
    watchDogCounterLast = 0;
}

void DLTRelais::stop()
{

    if(serialPort.isOpen())
    {
        serialPort.close();
        disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }

    timer.stop();
    disconnect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));

    status(QString("stopped"));
    qDebug() << "DLTRelais: stopped" << interface;
}

void DLTRelais::readyRead()
{
    while(serialPort.bytesAvailable())
    {
        char data[256];
        qint64 size = serialPort.readLine(data,sizeof(data));

        if(size>0)
            qDebug() << "DLTRelais: readLine" << data;


        if(QString(data) == "WD\r\n")
        {
            watchDogCounter++;
        }
        else
        {
            status(QString(data));
        }
    }
}

void DLTRelais::timeout()
{
    if(watchDogCounter==watchDogCounterLast)
    {
        qDebug() << "DLTRelais: Watchdog expired try reconnect" ;

        if(serialPort.isOpen())
        {
            serialPort.close();
            disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
        }

        if(serialPort.open(QIODevice::ReadWrite)==true)
        {
            serialPort.setDataTerminalReady(false);
            connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
            status(QString("started"));
            qDebug() << "DLTRelais: started" << interface;
        }
        else
        {
            qDebug() << "DLTRelais: Failed to open interface" << interface;
            status(QString("error"));
        }
    }
    watchDogCounterLast = watchDogCounter;
}

void DLTRelais::clearSettings()
{
    for(int num=0;num<5;num++)
        relaisName[num] = QString("Relais%1").arg(num+1);
}

void DLTRelais::writeSettings(QXmlStreamWriter &xml)
{
    /* Write project settings */
    xml.writeStartElement("DLTRelais");
        xml.writeTextElement("relaisName1",relaisName[0]);
        xml.writeTextElement("relaisName2",relaisName[1]);
        xml.writeTextElement("relaisName3",relaisName[2]);
        xml.writeTextElement("relaisName4",relaisName[3]);
        xml.writeTextElement("relaisName5",relaisName[4]);
        xml.writeTextElement("interface",interface);
    xml.writeEndElement(); // DLTRelais
}

void DLTRelais::readSettings(const QString &filename)
{
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
                  if(xml.name() == QString("relaisName4"))
                  {
                      relaisName[3] = xml.readElementText();
                  }
                  if(xml.name() == QString("relaisName5"))
                  {
                      relaisName[4] = xml.readElementText();
                  }
                  if(xml.name() == QString("interface"))
                  {
                      interface = xml.readElementText();
                  }
              }
              else if(xml.name() == QString("DLTRelais"))
              {
                    isDLTRelais = true;
              }
          }
          else if(xml.isEndElement())
          {
              /* Connection, plugin and filter */
              if(xml.name() == QString("DLTRelais"))
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
    if(num==1)
        serialPort.write("R1T\n");
    else if(num==2)
        serialPort.write("R2T\n");
    else if(num==3)
        serialPort.write("R3T\n");
    else if(num==4)
        serialPort.write("R4T\n");
    else if(num==5)
        serialPort.write("R5T\n");

    qDebug() << "DLTRelais: trigger" << num;
}

void DLTRelais::on(int num)
{
    if(num==1)
        serialPort.write("R11\n");
    else if(num==2)
        serialPort.write("R21\n");
    else if(num==3)
        serialPort.write("R31\n");
    else if(num==4)
        serialPort.write("R41\n");
    else if(num==5)
        serialPort.write("R51\n");

    qDebug() << "DLTRelais: on" << num;
}

void DLTRelais::off(int num)
{
    if(num==1)
        serialPort.write("R10\n");
    else if(num==2)
        serialPort.write("R20\n");
    else if(num==3)
        serialPort.write("R30\n");
    else if(num==4)
        serialPort.write("R40\n");
    else if(num==5)
        serialPort.write("R50\n");

    qDebug() << "DLTRelais: off" << num;
}
