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
 * \file dltmultimeter.h
 * @licence end@
 */

#ifndef DLT_MULTIMETER_H
#define DLT_MULTIMETER_H

#include <QObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSerialPort>
#include <QTimer>

class DLTMultimeter : public QObject
{
    Q_OBJECT
public:
    explicit DLTMultimeter(QObject *parent = nullptr);
    ~DLTMultimeter();

    void start();
    void stop();

    QString getInterface() { return interface; }
    void setInterface(QString interface) { this->interface = interface; }

    void clearSettings();
    void writeSettings(QXmlStreamWriter &xml);
    void readSettings(const QString &filename);

signals:

    void status(QString text);
    void valueMultimeter(QString value,QString unit);

private slots:

    void readyRead();

    // Watchdog Timeout
    void timeout();

private:

    QSerialPort serialPort;
    QTimer timer;
    unsigned int watchDogCounter,watchDogCounterLast;

    QString interface;

    QByteArray rawData;
    float value,lastValue;
    QString unit;

    void calculateValue();
    int calculateNumber(unsigned char a,unsigned char b);

};

#endif // DLT_MULTIMETER_H
