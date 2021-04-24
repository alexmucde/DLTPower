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
 * \file dialog.cpp
 * @licence end@
 */

#include <QSerialPortInfo>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "dialog.h"
#include "ui_dialog.h"
#include "settingsdialog.h"
#include "version.h"

Dialog::Dialog(bool autostart,QString configuration,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , dltRelais(this)
    , dltMiniServer(this)
{
    ui->setupUi(this);

    // clear settings
    on_pushButtonDefaultSettings_clicked();

    // set window title with version information
    setWindowTitle(QString("DLTPower %1").arg(DLT_POWER_VERSION));
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);

    // disable stop button at startup
    ui->pushButtonStop->setDisabled(true);

    // connect status slots
    connect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    connect(&dltMultimeter, SIGNAL(status(QString)), this, SLOT(statusMultimeter(QString)));
    connect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    //connect value slots from Multimeter
    connect(&dltMultimeter, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter(QString,QString)));

    //  load global settings from registry
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    bool autoload = settings.value("autoload/checked").toBool();
    bool autostartGlobal = settings.value("autostart/checked").toBool();

    // autoload settings, when activated in global settings
    if(autoload)
    {
        dltRelais.readSettings(filename);
        dltMultimeter.readSettings(filename);
        dltMiniServer.readSettings(filename);
        restoreSettings();
    }

    // autoload settings, when provided by command line
    if(!configuration.isEmpty())
    {
        dltRelais.readSettings(configuration);
        dltMultimeter.readSettings(configuration);
        dltMiniServer.readSettings(configuration);
        restoreSettings();
    }

    // autostart, when activated in global settings or by command line
    if(autostartGlobal || autostart)
    {
        on_pushButtonStart_clicked();
    }
}

Dialog::~Dialog()
{

    // disconnect all slots
    disconnect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    disconnect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    disconnect(&dltMultimeter, SIGNAL(status(QString)), this, SLOT(statusMultimeter(QString)));
    disconnect(&dltMultimeter, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter(QString,QString)));

    delete ui;
}

void Dialog::restoreSettings()
{
    // update names of Relais
    ui->lineEditRelaisName1->setText(dltRelais.getRelaisName(1));
    ui->lineEditRelaisName2->setText(dltRelais.getRelaisName(2));
    ui->lineEditRelaisName3->setText(dltRelais.getRelaisName(3));
    ui->lineEditRelaisName4->setText(dltRelais.getRelaisName(4));
    ui->lineEditRelaisName5->setText(dltRelais.getRelaisName(5));

    // DLTMultimeter
    ui->lineEditPowerName->setText(dltMultimeter.getPowerName());

}

void Dialog::updateSettings()
{
}

void Dialog::on_pushButtonStart_clicked()
{
    // start communication
    updateSettings();

    // start Relais and DLT communication
    dltRelais.start();
    dltMultimeter.start();
    dltMiniServer.start();

    // disable settings and start button
    // enable stop button
    ui->pushButtonStart->setDisabled(true);
    ui->pushButtonStop->setDisabled(false);
    ui->pushButtonDefaultSettings->setDisabled(true);
    ui->pushButtonLoadSettings->setDisabled(true);
    ui->pushButtonSettings->setDisabled(true);

    ui->checkBoxPower->setChecked(false);
    dltMultimeter.off();

    ui->checkBoxRelais1->setChecked(false);
    dltRelais.off(1);
    ui->checkBoxRelais2->setChecked(false);
    dltRelais.off(2);
    ui->checkBoxRelais3->setChecked(false);
    dltRelais.off(3);
    ui->checkBoxRelais4->setChecked(false);
    dltRelais.off(4);
    ui->checkBoxRelais5->setChecked(false);
    dltRelais.off(5);

}

void Dialog::on_pushButtonStop_clicked()
{
    // stop communication

    // stop Relais and DLT communication
    dltRelais.stop();
    dltMultimeter.stop();
    dltMiniServer.stop();

    // enable settings and start button
    // disable stop button
    ui->pushButtonStart->setDisabled(false);
    ui->pushButtonStop->setDisabled(true);
    ui->pushButtonDefaultSettings->setDisabled(false);
    ui->pushButtonLoadSettings->setDisabled(false);
    ui->pushButtonSettings->setDisabled(false);
}

void Dialog::statusRelais(QString text)
{
    // status from Relais

    // Relais status changed on Arduino board
    if(text=="R10\r\n")
    {
        ui->checkBoxRelais1->setChecked(false);
        return;
    }
    else if(text=="R11\r\n")
    {
        ui->checkBoxRelais1->setChecked(true);
        return;
    }
    else if(text=="R1T\r\n")
    {
        return;
    }
    else if(text=="R20\r\n")
    {
        ui->checkBoxRelais2->setChecked(false);
        return;
    }
    else if(text=="R21\r\n")
    {
        ui->checkBoxRelais2->setChecked(true);
        return;
    }
    else if(text=="R30\r\n")
    {
        ui->checkBoxRelais3->setChecked(false);
        return;
    }
    else if(text=="R31\r\n")
    {
        ui->checkBoxRelais3->setChecked(true);
        return;
    }
    else if(text=="R40\r\n")
    {
        ui->checkBoxRelais4->setChecked(false);
        return;
    }
    else if(text=="R41\r\n")
    {
        ui->checkBoxRelais4->setChecked(true);
        return;
    }
    else if(text=="R50\r\n")
    {
        ui->checkBoxRelais5->setChecked(false);
        return;
    }
    else if(text=="R51\r\n")
    {
        ui->checkBoxRelais5->setChecked(true);
        return;
    }

    // status of Relais communication changed
    else if(text == "" || text == "stopped")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatus->setPalette(palette);
        ui->lineEditStatus->setText(text);
    }
    else if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatus->setPalette(palette);
        ui->lineEditStatus->setText(text);
    }
    else if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatus->setPalette(palette);
        ui->lineEditStatus->setText(text);
    }
    else if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatus->setPalette(palette);
        ui->lineEditStatus->setText(text);
    }
}

void Dialog::statusMultimeter(QString text)
{
    ui->lineEditStatusMultimeter->setText(text);

    if(text == "" || text == "stopped")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
    if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
    if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
    if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
}

void Dialog::statusDlt(QString text)
{
    // status from DLT Mini Server
    ui->lineEditStatusDLT->setText(text);

    // status of DLT communication changed
    if(text == "" || text == "stopped")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    else if(text == "listening")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    else if(text == "connected")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    else if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusDLT->setPalette(palette);
    }
}

// The following functions are called, when the Relais Checkboxes are triggered or the Trigger
// button is pressed

void Dialog::on_checkBoxRelais1_clicked(bool checked)
{
    if(checked)
    {
        dltRelais.on(1);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(1),"On");
    }
    else
    {
        dltRelais.off(1);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(1),"Off");
    }
}

void Dialog::on_pushButtonRelais1Trigger_clicked()
{
    dltRelais.trigger(1);
    ui->checkBoxRelais1->setChecked(false);
    dltMiniServer.sendValue2(dltRelais.getRelaisName(1),"Trigger");
}

void Dialog::on_checkBoxRelais2_clicked(bool checked)
{
    if(checked)
    {
        dltRelais.on(2);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(2),"On");
    }
    else
    {
        dltRelais.off(2);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(2),"Off");
    }
}

void Dialog::on_pushButtonRelais2Trigger_clicked()
{
    dltRelais.trigger(2);
    ui->checkBoxRelais2->setChecked(false);
    dltMiniServer.sendValue2(dltRelais.getRelaisName(2),"Trigger");
}

void Dialog::on_checkBoxRelais3_clicked(bool checked)
{
    if(checked)
    {
        dltRelais.on(3);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(3),"On");
    }
    else
    {
        dltRelais.off(3);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(3),"Off");
    }
}

void Dialog::on_pushButtonRelais3Trigger_clicked()
{
    dltRelais.trigger(3);
    ui->checkBoxRelais3->setChecked(false);
    dltMiniServer.sendValue2(dltRelais.getRelaisName(3),"Trigger");
}

void Dialog::on_pushButtonRelais4Trigger_clicked()
{
    dltRelais.trigger(4);
    ui->checkBoxRelais4->setChecked(false);
    dltMiniServer.sendValue2(dltRelais.getRelaisName(4),"Trigger");
}

void Dialog::on_pushButtonRelais5Trigger_clicked()
{
    dltRelais.trigger(5);
    ui->checkBoxRelais5->setChecked(false);
    dltMiniServer.sendValue2(dltRelais.getRelaisName(5),"Trigger");
}

void Dialog::on_checkBoxRelais4_clicked(bool checked)
{
    if(checked)
    {
        dltRelais.on(4);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(4),"On");
    }
    else
    {
        dltRelais.off(4);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(4),"Off");
    }
}

void Dialog::on_checkBoxRelais5_clicked(bool checked)
{
    if(checked)
    {
        dltRelais.on(5);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(5),"On");
    }
    else
    {
        dltRelais.off(5);
        dltMiniServer.sendValue2(dltRelais.getRelaisName(5),"Off");
    }
}

void Dialog::on_pushButtonDefaultSettings_clicked()
{
    // Reset settings to default
    dltRelais.clearSettings();
    dltMiniServer.clearSettings();
    dltMultimeter.clearSettings();

    restoreSettings();
}

void Dialog::on_pushButtonLoadSettings_clicked()
{
    // Load settings from XML file

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings"), "", tr("DLTPower Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        // No file was selected or cancel was pressed
        return;
    }

    // read the settings from XML file
    dltRelais.readSettings(fileName);
    dltMultimeter.readSettings(fileName);
    dltMiniServer.readSettings(fileName);

    restoreSettings();
}

void Dialog::on_pushButtonSaveSettings_clicked()
{
    // Save settings into XML file

    updateSettings();

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Settings"), "", tr("DLTRelais Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        // No file was selected or cancel was pressed
        return;
    }

    // read the settings from XML file
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        // Cannot open the file for writing
        return;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    // FIXME: Cannot read data from XML file, which contains a start document
    // So currently do not call StartDocument
    //xml.writeStartDocument();

    xml.writeStartElement("DLTRelaisSettings");
        dltRelais.writeSettings(xml);
        dltMultimeter.writeSettings(xml);
        dltMiniServer.writeSettings(xml);
    xml.writeEndElement(); // DLTRelaisSettings

    // FIXME: Cannot read data from XML file, which contains a end document
    // So currently do not call EndDocument
    //xml.writeEndDocument();
    file.close();

}

void Dialog::on_pushButtonSettings_clicked()
{
    // Open settings dialog
    SettingsDialog dlg(this);

    dlg.restoreSettings(&dltRelais, &dltMultimeter, &dltMiniServer);
    if(dlg.exec()==QDialog::Accepted)
    {
        dlg.backupSettings(&dltRelais, &dltMultimeter, &dltMiniServer);
        restoreSettings();
    }
}

void Dialog::on_pushButtonInfo_clicked()
{
    // Open information window
    QMessageBox msgBox(this);

    msgBox.setWindowTitle("Info DLTPower");
    msgBox.setTextFormat(Qt::RichText);

    QString text;
    text += QString("Version: %1<br>").arg(DLT_POWER_VERSION);
    text += "<br>";
    text += "Information and Documentation can be found here:<br>";
    text += "<br>";
    text += "<a href='https://github.com/alexmucde/DLTRelais'>Github DLTRelais</a><br>";
    text += "<br>";
    text += "This SW is licensed under GPLv3.<br>";
    text += "<br>";
    text += "(C) 2021 Alexander Wenzel <alex@eli2.de>";

    msgBox.setText(text);

    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}

void Dialog::valueMultimeter(QString value,QString unit)
{
    ui->lineEditUnit->setText(unit);
    ui->lineEditValue->setText(value);

    dltMiniServer.sendValue3(dltMultimeter.getPowerName(),value,unit);
}

void Dialog::on_checkBoxPower_clicked(bool checked)
{
    if(checked)
    {
        dltMultimeter.on();
        dltMiniServer.sendValue2(dltMultimeter.getPowerName(),"On");
    }
    else
    {
        dltMultimeter.off();
        dltMiniServer.sendValue2(dltMultimeter.getPowerName(),"Off");
    }
}
