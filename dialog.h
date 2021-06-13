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
 * \file dialog.h
 * @licence end@
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>

#include "dltrelais.h"
#include "dltminiserver.h"
#include "dltmultimeter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(bool autostart,QString configuration,QWidget *parent = nullptr);
    ~Dialog();

private slots:

    // Relais control
    void on_checkBoxRelais1_clicked(bool checked);
    void on_pushButtonRelais1Trigger_clicked();
    void on_checkBoxRelais2_clicked(bool checked);
    void on_pushButtonRelais2Trigger_clicked();
    void on_checkBoxRelais3_clicked(bool checked);
    void on_pushButtonRelais3Trigger_clicked();
    void on_checkBoxRelais4_clicked(bool checked);
    void on_pushButtonRelais4Trigger_clicked();
    void on_checkBoxRelais5_clicked(bool checked);
    void on_pushButtonRelais5Trigger_clicked();
    void on_pushButtonRelais6Trigger_clicked();
    void on_checkBoxRelais6_clicked(bool checked);

    // Status of Relais and DLT connection
    void statusRelais1(QString text);
    void statusRelais2(QString text);
    void statusMultimeter1(QString text);
    void statusMultimeter2(QString text);
    void statusDlt(QString text);

    void injection(QString text);

    // value from Multimeter or Power Supply
    void valueMultimeter1(QString value,QString unit);
    void valueMultimeter2(QString value,QString unit);

    // Settings and Info
    void on_pushButtonSettings_clicked();
    void on_pushButtonDefaultSettings_clicked();
    void on_pushButtonLoadSettings_clicked();
    void on_pushButtonSaveSettings_clicked();
    void on_pushButtonInfo_clicked();

    // Start and stop communication
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

    void on_checkBoxPower1_clicked(bool checked);
    void on_checkBoxPower2_clicked(bool checked);

    void on_checkBoxRelais7_clicked(bool checked);

    void on_pushButtonRelais7Trigger_clicked();

    void on_checkBoxRelais8_clicked(bool checked);


    void on_pushButtonRelais8Trigger_clicked();

private:
    Ui::Dialog *ui;

    DLTRelais dltRelais1,dltRelais2;
    DLTMultimeter dltMultimeter1;
    DLTMultimeter dltMultimeter2;
    DLTMiniServer dltMiniServer;

    // Settings
    void restoreSettings();
    void updateSettings();

    void relais1Trigger(unsigned int duration);
    void relais2Trigger(unsigned int duration);
    void relais3Trigger(unsigned int duration);
    void relais4Trigger(unsigned int duration);
    void relais5Trigger(unsigned int duration);
    void relais6Trigger(unsigned int duration);
    void relais7Trigger(unsigned int duration);
    void relais8Trigger(unsigned int duration);

};
#endif // DIALOG_H
