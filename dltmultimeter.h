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

    void checkPortName();
    void start();
    void stop();

    QString getInterface() { return interface; }
    void setInterface(QString interface) { this->interface = interface; }

    // Active
    bool getActive() { return active; }
    void setActive(bool active) { this->active = active; }

    int getType() { return type; }
    void setType(int type) { this->type = type; }

    void clearSettings();
    void writeSettings(QXmlStreamWriter &xml,int num);
    void readSettings(const QString &filename,int num);

    void on();
    void off();
    void setVoltage(float value);

    QString getPowerName() { return powerName; }
    void setPowerName(const QString &powerName) { this->powerName = powerName; }

    float getSubstractCurrent() const;
    void setSubstractCurrent(float newSubstractCurrent);

signals:

    void status(QString text);
    void valueMultimeter(QString value,QString unit);

private slots:

    void readyRead();

    // Watchdog Timeout
    void timeout();

    void timeoutRequest();

private:

    QSerialPort serialPort;
    QTimer timer;
    QTimer timerRequest;
    unsigned int watchDogCounter,watchDogCounterLast;

    int type;
    QString interface;
    QString interfaceSerialNumber;
    ushort interfaceProductIdentifier;
    ushort interfaceVendorIdentifier;
    QString powerName;
    float substractCurrent;
    bool active;

    QByteArray serialData;

    QByteArray rawData;
    float value;
    QString unit;

    void calculateValue();
    int calculateNumber(unsigned char a,unsigned char b);

    bool readVoltageOngoing;
    QString voltageCmd;

};

#endif // DLT_MULTIMETER_H
