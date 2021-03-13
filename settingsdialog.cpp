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
 * \file dsettingsdialog.h
 * @licence end@
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QSerialPortInfo>
#include <QSettings>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    QList<QSerialPortInfo> 	availablePorts  = QSerialPortInfo::availablePorts();

    ui->comboBoxSerialPort->clear();

    for(int num = 0; num<availablePorts.length();num++)
    {
        ui->comboBoxSerialPort->addItem(availablePorts[num].portName());
    }

    /*  load global settings */
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    ui->lineEditAutoload->setText(filename);
    bool autoload = settings.value("autoload/checked").toBool();
    ui->groupBoxAutoload->setChecked(autoload);
    bool autostart = settings.value("autostart/checked").toBool();
    ui->checkBoxAutostart->setChecked(autostart);

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButtonAutoload_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings"), "", tr("DLTRelais Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        return;
    }

    ui->lineEditAutoload->setText(fileName);

    QSettings settings;
    settings.setValue("autoload/filename",fileName);
}

void SettingsDialog::restoreSettings(DLTRelais *dltRelais, DLTMiniServer *dltMiniServer)
{
    /* DLTRelais */
    ui->lineEditRelaisName1->setText(dltRelais->getRelaisName(1));
    ui->lineEditRelaisName2->setText(dltRelais->getRelaisName(2));
    ui->lineEditRelaisName3->setText(dltRelais->getRelaisName(3));
    ui->comboBoxSerialPort->setCurrentText(dltRelais->getInterface());

    /* DLTMiniServer */
    ui->lineEditPort->setText(QString("%1").arg(dltMiniServer->getPort()));
    ui->lineEditApplicationId->setText(dltMiniServer->getApplicationId());
    ui->lineEditContextId->setText(dltMiniServer->getContextId());
}

void SettingsDialog::backupSettings(DLTRelais *dltRelais, DLTMiniServer *dltMiniServer)
{
    /* DLTRelais */
    dltRelais->setInterface(ui->comboBoxSerialPort->currentText());
    dltRelais->setRelaisName(1,ui->lineEditRelaisName1->text());
    dltRelais->setRelaisName(2,ui->lineEditRelaisName2->text());
    dltRelais->setRelaisName(3,ui->lineEditRelaisName3->text());

    /* DLTMiniServer */
    dltMiniServer->setPort(ui->lineEditPort->text().toUShort());
    dltMiniServer->setApplicationId(ui->lineEditApplicationId->text());
    dltMiniServer->setContextId(ui->lineEditContextId->text());
}

void SettingsDialog::on_checkBoxAutostart_clicked(bool checked)
{
    QSettings settings;
    settings.setValue("autostart/checked",checked);
}

void SettingsDialog::on_groupBoxAutoload_clicked(bool checked)
{
    QSettings settings;
    settings.setValue("autoload/checked",checked);
}
