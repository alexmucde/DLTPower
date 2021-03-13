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
    if(serialPort.isOpen())
        return;

    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setPortName(interface);

    if(serialPort.open(QIODevice::ReadWrite)==true)
    {
        connect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
    else
    {
        qDebug() << "DLTRelais: Failed to open interface" << interface;
        status(QString("interface error"));
    }

    status(QString("started"));
    qDebug() << "DLTRelais: started" << interface;
}

void DLTRelais::stop()
{
    if(!serialPort.isOpen())
        return;

    serialPort.close();
    disconnect(&serialPort, SIGNAL(readyRead()), this, SLOT(readyRead()));

    status(QString("stopped"));
    qDebug() << "DLTRelais: stopped" << interface;
}

void DLTRelais::readyRead()
{

}

void DLTRelais::clearSettings()
{
    for(int num=0;num<3;num++)
        relaisName[num] = QString("Relais%1").arg(num+1);
}

void DLTRelais::writeSettings(QXmlStreamWriter &xml)
{
    /* Write project settings */
    xml.writeStartElement("DLTRelais");
        xml.writeTextElement("relaisName1",relaisName[0]);
        xml.writeTextElement("relaisName2",relaisName[1]);
        xml.writeTextElement("relaisName3",relaisName[2]);
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
        serialPort.write("D1T\n");
    else if(num==2)
        serialPort.write("D2T\n");
    else if(num==3)
        serialPort.write("D6T\n");

    qDebug() << "DLTRelais: trigger" << num;
}

void DLTRelais::on(int num)
{
    if(num==1)
        serialPort.write("D11\n");
    else if(num==2)
        serialPort.write("D21\n");
    else if(num==3)
        serialPort.write("D61\n");

    qDebug() << "DLTRelais: on" << num;
}

void DLTRelais::off(int num)
{
    if(num==1)
        serialPort.write("D10\n");
    else if(num==2)
        serialPort.write("D20\n");
    else if(num==3)
        serialPort.write("D60\n");

    qDebug() << "DLTRelais: off" << num;
}
