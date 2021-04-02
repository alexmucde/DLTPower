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

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , dltRelais(this)
    , dltMiniServer(this)
{
    ui->setupUi(this);

    // clear settings
    on_pushButtonDefaultSettings_clicked();

    // set window title with version information
    setWindowTitle(QString("DLTRelais %1").arg(DLT_RELAIS_VERSION));

    // disable stop button at startup
    ui->pushButtonStop->setDisabled(true);

    // connect status slots
    connect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    connect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    //  load global settings from registry
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    bool autoload = settings.value("autoload/checked").toBool();
    bool autostart = settings.value("autostart/checked").toBool();

    // autoload settings, when activated in global settings
    if(autoload)
    {
        dltRelais.readSettings(filename);
        dltMiniServer.readSettings(filename);
        restoreSettings();
    }

    // autostart, when activated in global settings
    if(autostart)
    {
        on_pushButtonStart_clicked();
    }
}

Dialog::~Dialog()
{

    // disconnect status slots
    disconnect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    disconnect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

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
    dltMiniServer.start();

    // disable settings and start button
    // enable stop button
    ui->pushButtonStart->setDisabled(true);
    ui->pushButtonStop->setDisabled(false);
    ui->pushButtonDefaultSettings->setDisabled(true);
    ui->pushButtonLoadSettings->setDisabled(true);
    ui->pushButtonSettings->setDisabled(true);
}

void Dialog::on_pushButtonStop_clicked()
{
    // stop communication

    // stop Relais and DLT communication
    dltRelais.stop();
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
    else if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatus->setPalette(palette);
        ui->lineEditStatus->setText(text);
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

    restoreSettings();
}

void Dialog::on_pushButtonLoadSettings_clicked()
{
    // Load settings from XML file

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings"), "", tr("DLTRelais Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        // No file was selected or cancel was pressed
        return;
    }

    // read the settings from XML file
    dltRelais.readSettings(fileName);
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

    dlg.restoreSettings(&dltRelais, &dltMiniServer);
    if(dlg.exec()==QDialog::Accepted)
    {
        dlg.backupSettings(&dltRelais, &dltMiniServer);
        restoreSettings();
    }
}

void Dialog::on_pushButtonInfo_clicked()
{
    // Open information window
    QMessageBox msgBox(this);

    msgBox.setWindowTitle("Info DLTRelais");
    msgBox.setTextFormat(Qt::RichText);

    QString text;
    text += QString("Version: %1<br>").arg(DLT_RELAIS_VERSION);
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

