#ifndef DLT_RELAIS_H
#define DLT_RELAIS_H

#include <QObject>
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

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSerialPort>
#include <QTimer>

class DLTRelais : public QObject
{
    Q_OBJECT
public:
    explicit DLTRelais(QObject *parent = nullptr);
    ~DLTRelais();

    // Start and stop connection
    void checkPortName();
    void start();
    void stop();

    // Trigger Relais
    void trigger(int num,unsigned int duration);
    void on(int num);
    void off(int num);

    // Interface name
    QString getInterface() { return interface; }
    void setInterface(QString interface) { this->interface = interface; }

    // Active
    bool getActive() { return active; }
    void setActive(bool active) { this->active = active; }

    // Relais names
    QString getRelaisName(int num) { if(num>0 && num<5) return relaisName[num-1]; else return QString(); }
    void setRelaisName(int num,const QString &name) { if(num>0 && num<5) this->relaisName[num-1] = name; }

    // Settings
    void clearSettings();
    void writeSettings(QXmlStreamWriter &xml,int num);
    void readSettings(const QString &filename,int num);

signals:

    // Called when status changed
    void status(QString text);

private slots:

    // Serial data available
    void readyRead();

    // Watchdog Timeout
    void timeout();

private:

    // Temporary variables
    QSerialPort serialPort;
    QTimer timer;
    unsigned int watchDogCounter,watchDogCounterLast;

    // Settings
    QString interface;
    QString interfaceSerialNumber;
    ushort interfaceProductIdentifier;
    ushort interfaceVendorIdentifier;
    QString relaisName[4];
    bool active;
};

#endif // DLT_RELAIS_H
