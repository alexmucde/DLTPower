#ifndef DLT_RELAIS_H
#define DLT_RELAIS_H

#include <QObject>
/**
 * @licence app begin@
 * Copyright (C) 2021 Alexander Wenzel
 *
 * This file is part of the DLT Relais project.
 *
 * \copyright
 * See LICENSE file.
 *
 * \author Alexander Wenzel <alexander.wenzel@eli2.de> 2021
 *
 * \file dltrelais.h
 * @licence end@
 */

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSerialPort>

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

    QString getRelaisName(int num) { if(num>0 && num<4) return relaisName[num-1]; else return QString(); }
    void setRelaisName(int num,const QString &name) { if(num>0 && num<4) this->relaisName[num-1] = name; }

    void clearSettings();
    void writeSettings(QXmlStreamWriter &xml);
    void readSettings(const QString &filename);

signals:

    void status(QString text);

private slots:

    void readyRead();

private:

    QSerialPort serialPort;

    QString interface;
    QString relaisName[3];
};

#endif // DLT_RELAIS_H
