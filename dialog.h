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

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:

    void on_checkBoxRelais1_clicked(bool checked);
    void on_pushButtonRelais1Trigger_clicked();

    void on_checkBoxRelais2_clicked(bool checked);
    void on_pushButtonRelais2Trigger_clicked();

    void on_checkBoxRelais3_clicked(bool checked);
    void on_pushButtonRelais3Trigger_clicked();

    void statusRelais(QString text);
    void statusDlt(QString text);

    void on_pushButtonDefaultSettings_clicked();

    void on_pushButtonLoadSettings_clicked();

    void on_pushButtonSaveSettings_clicked();

    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

    void on_pushButtonSettings_clicked();

private:
    Ui::Dialog *ui;

    DLTRelais dltRelais;
    DLTMiniServer dltMiniServer;

    void restoreSettings();
    void updateSettings();
};
#endif // DIALOG_H
