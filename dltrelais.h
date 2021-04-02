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

    void start();
    void stop();

    void trigger(int num);
    void on(int num);
    void off(int num);

    QString getInterface() { return interface; }
    void setInterface(QString interface) { this->interface = interface; }

    QString getRelaisName(int num) { if(num>0 && num<6) return relaisName[num-1]; else return QString(); }
    void setRelaisName(int num,const QString &name) { if(num>0 && num<6) this->relaisName[num-1] = name; }

    void clearSettings();
    void writeSettings(QXmlStreamWriter &xml);
    void readSettings(const QString &filename);

signals:

    void status(QString text);

private slots:

    void readyRead();

    void timeout();

private:

    QSerialPort serialPort;

    QString interface;
    QString relaisName[5];

    unsigned int watchDogCounter,watchDogCounterLast;

    QTimer timer;
};

#endif // DLT_RELAIS_H
