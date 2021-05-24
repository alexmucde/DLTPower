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
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    /* update serial ports list */
    QList<QSerialPortInfo> 	availablePorts  = QSerialPortInfo::availablePorts();
    ui->comboBoxSerialPort1->clear();
    ui->comboBoxSerialPort2->clear();
    ui->comboBoxSerialPortMultimeter1->clear();
    ui->comboBoxSerialPortMultimeter2->clear();
    qDebug() << "portName" << "description" << "manufacturer" << "serialNumber" << "productIdentifier" << "vendorIdentifier" << "systemLocation";
    for(int num = 0; num<availablePorts.length();num++)
    {
        qDebug() << availablePorts[num].portName() << availablePorts[num].description() << availablePorts[num].manufacturer() << availablePorts[num].serialNumber() << availablePorts[num].productIdentifier() << availablePorts[num].vendorIdentifier() << availablePorts[num].systemLocation();
        ui->comboBoxSerialPort1->addItem(availablePorts[num].portName());
        ui->comboBoxSerialPort2->addItem(availablePorts[num].portName());
        ui->comboBoxSerialPortMultimeter1->addItem(availablePorts[num].portName());
        ui->comboBoxSerialPortMultimeter2->addItem(availablePorts[num].portName());
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

void SettingsDialog::restoreSettings(DLTRelais *dltRelais1, DLTRelais *dltRelais2, DLTMultimeter *dltMultimeter1, DLTMultimeter *dltMultimeter2, DLTMiniServer *dltMiniServer)
{
    /* DLTRelais */
    ui->lineEditRelaisName1->setText(dltRelais1->getRelaisName(1));
    ui->lineEditRelaisName2->setText(dltRelais1->getRelaisName(2));
    ui->lineEditRelaisName3->setText(dltRelais1->getRelaisName(3));
    ui->lineEditRelaisName4->setText(dltRelais2->getRelaisName(1));
    ui->lineEditRelaisName5->setText(dltRelais2->getRelaisName(2));
    ui->lineEditRelaisName6->setText(dltRelais2->getRelaisName(3));
    ui->lineEditRelaisName7->setText(dltRelais1->getRelaisName(4));
    ui->lineEditRelaisName8->setText(dltRelais2->getRelaisName(4));
    ui->comboBoxSerialPort1->setCurrentText(dltRelais1->getInterface());
    ui->comboBoxSerialPort2->setCurrentText(dltRelais2->getInterface());
    ui->checkBoxRelais1Active->setChecked(dltRelais1->getActive());
    ui->checkBoxRelais2Active->setChecked(dltRelais2->getActive());

    /* DLTMultimeter */
    ui->comboBoxSerialPortMultimeter1->setCurrentText(dltMultimeter1->getInterface());
    ui->comboBoxSerialPortMultimeter2->setCurrentText(dltMultimeter2->getInterface());
    ui->comboBoxType1->setCurrentIndex(dltMultimeter1->getType());
    ui->comboBoxType2->setCurrentIndex(dltMultimeter2->getType());
    ui->lineEditPower1Name->setText(dltMultimeter1->getPowerName());
    ui->lineEditPower2Name->setText(dltMultimeter2->getPowerName());
    ui->checkBoxMultimeter1Active->setChecked(dltMultimeter1->getActive());
    ui->checkBoxMultimeter2Active->setChecked(dltMultimeter2->getActive());

    /* DLTMiniServer */
    ui->lineEditPort->setText(QString("%1").arg(dltMiniServer->getPort()));
    ui->lineEditApplicationId->setText(dltMiniServer->getApplicationId());
    ui->lineEditContextId->setText(dltMiniServer->getContextId());


}

void SettingsDialog::backupSettings(DLTRelais *dltRelais1, DLTRelais *dltRelais2, DLTMultimeter *dltMultimeter1, DLTMultimeter *dltMultimeter2, DLTMiniServer *dltMiniServer)
{
    /* DLTRelais */
    dltRelais1->setInterface(ui->comboBoxSerialPort1->currentText());
    dltRelais2->setInterface(ui->comboBoxSerialPort2->currentText());
    dltRelais1->setRelaisName(1,ui->lineEditRelaisName1->text());
    dltRelais1->setRelaisName(2,ui->lineEditRelaisName2->text());
    dltRelais1->setRelaisName(3,ui->lineEditRelaisName3->text());
    dltRelais2->setRelaisName(1,ui->lineEditRelaisName4->text());
    dltRelais2->setRelaisName(2,ui->lineEditRelaisName5->text());
    dltRelais2->setRelaisName(3,ui->lineEditRelaisName6->text());
    dltRelais1->setRelaisName(4,ui->lineEditRelaisName7->text());
    dltRelais2->setRelaisName(4,ui->lineEditRelaisName8->text());
    dltRelais1->setActive(ui->checkBoxRelais1Active->isChecked());
    dltRelais2->setActive(ui->checkBoxRelais2Active->isChecked());

    /* DLTMultimeter */
    dltMultimeter1->setInterface(ui->comboBoxSerialPortMultimeter1->currentText());
    dltMultimeter1->setType(ui->comboBoxType1->currentIndex());
    dltMultimeter1->setPowerName(ui->lineEditPower1Name->text());
    dltMultimeter2->setInterface(ui->comboBoxSerialPortMultimeter2->currentText());
    dltMultimeter2->setType(ui->comboBoxType2->currentIndex());
    dltMultimeter2->setPowerName(ui->lineEditPower2Name->text());
    dltMultimeter1->setActive(ui->checkBoxMultimeter1Active->isChecked());
    dltMultimeter2->setActive(ui->checkBoxMultimeter2Active->isChecked());

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
