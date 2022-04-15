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
#include <QStringList>

#include "dialog.h"
#include "ui_dialog.h"
#include "settingsdialog.h"
#include "version.h"

Dialog::Dialog(bool autostart,QString configuration,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , dltRelais1(this)
    , dltRelais2(this)
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
    connect(&dltRelais1, SIGNAL(status(QString)), this, SLOT(statusRelais1(QString)));
    connect(&dltRelais2, SIGNAL(status(QString)), this, SLOT(statusRelais2(QString)));
    connect(&dltMultimeter1, SIGNAL(status(QString)), this, SLOT(statusMultimeter1(QString)));
    connect(&dltMultimeter2, SIGNAL(status(QString)), this, SLOT(statusMultimeter2(QString)));
    connect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    connect(&dltMiniServer, SIGNAL(injection(QString)), this, SLOT(injection(QString)));

    //connect value slots from Multimeter
    connect(&dltMultimeter1, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter1(QString,QString)));
    connect(&dltMultimeter2, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter2(QString,QString)));

    //  load global settings from registry
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    bool autoload = settings.value("autoload/checked").toBool();
    bool autostartGlobal = settings.value("autostart/checked").toBool();

    // autoload settings, when activated in global settings
    if(autoload)
    {
        dltRelais1.readSettings(filename,1);
        dltRelais2.readSettings(filename,2);
        dltMultimeter1.readSettings(filename,1);
        dltMultimeter2.readSettings(filename,2);
        dltMiniServer.readSettings(filename);
        restoreSettings();
    }

    // autoload settings, when provided by command line
    if(!configuration.isEmpty())
    {
        dltRelais1.readSettings(configuration,1);
        dltRelais2.readSettings(configuration,2);
        dltMultimeter1.readSettings(configuration,1);
        dltMultimeter2.readSettings(configuration,2);
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
    disconnect(&dltRelais1, SIGNAL(status(QString)), this, SLOT(statusRelais1(QString)));
    disconnect(&dltRelais2, SIGNAL(status(QString)), this, SLOT(statusRelais2(QString)));
    disconnect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    disconnect(&dltMiniServer, SIGNAL(injection(QString)), this, SLOT(injection(QString)));

    disconnect(&dltMultimeter1, SIGNAL(status(QString)), this, SLOT(statusMultimeter1(QString)));
    disconnect(&dltMultimeter1, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter1(QString,QString)));
    disconnect(&dltMultimeter2, SIGNAL(status(QString)), this, SLOT(statusMultimeter2(QString)));
    disconnect(&dltMultimeter2, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter2(QString,QString)));

    delete ui;
}

void Dialog::restoreSettings()
{
    // update names of Relais
    ui->lineEditRelaisName1->setText(dltRelais1.getRelaisName(1));
    ui->lineEditRelaisName2->setText(dltRelais1.getRelaisName(2));
    ui->lineEditRelaisName3->setText(dltRelais1.getRelaisName(3));
    ui->lineEditRelaisName4->setText(dltRelais2.getRelaisName(1));
    ui->lineEditRelaisName5->setText(dltRelais2.getRelaisName(2));
    ui->lineEditRelaisName6->setText(dltRelais2.getRelaisName(3));
    ui->lineEditRelaisName7->setText(dltRelais1.getRelaisName(4));
    ui->lineEditRelaisName8->setText(dltRelais2.getRelaisName(4));

    // DLTMultimeter
    ui->lineEditPower1Name->setText(dltMultimeter1.getPowerName());
    ui->lineEditPower2Name->setText(dltMultimeter2.getPowerName());

}

void Dialog::updateSettings()
{
}

void Dialog::on_pushButtonStart_clicked()
{
    // start communication
    updateSettings();

    // start Relais and DLT communication
    dltRelais1.start();
    dltRelais2.start();
    dltMultimeter1.start();
    dltMultimeter2.start();
    dltMiniServer.start();

    // disable settings and start button
    // enable stop button
    ui->pushButtonStart->setDisabled(true);
    ui->pushButtonStop->setDisabled(false);
    ui->pushButtonDefaultSettings->setDisabled(true);
    ui->pushButtonLoadSettings->setDisabled(true);
    ui->pushButtonSettings->setDisabled(true);

    ui->checkBoxPower1->setChecked(false);
    dltMultimeter1.off();
    ui->checkBoxPower2->setChecked(false);
    dltMultimeter2.off();

    ui->checkBoxRelais1->setChecked(false);
    dltRelais1.off(1);
    ui->checkBoxRelais2->setChecked(false);
    dltRelais1.off(2);
    ui->checkBoxRelais3->setChecked(false);
    dltRelais1.off(3);
    ui->checkBoxRelais7->setChecked(false);
    dltRelais1.off(4);
    ui->checkBoxRelais4->setChecked(false);
    dltRelais2.off(1);
    ui->checkBoxRelais5->setChecked(false);
    dltRelais2.off(2);
    ui->checkBoxRelais6->setChecked(false);
    dltRelais2.off(3);
    ui->checkBoxRelais8->setChecked(false);
    dltRelais2.off(4);

}

void Dialog::on_pushButtonStop_clicked()
{
    // stop communication

    // stop Relais and DLT communication
    dltRelais1.stop();
    dltRelais2.stop();
    dltMultimeter1.stop();
    dltMultimeter2.stop();
    dltMiniServer.stop();

    // enable settings and start button
    // disable stop button
    ui->pushButtonStart->setDisabled(false);
    ui->pushButtonStop->setDisabled(true);
    ui->pushButtonDefaultSettings->setDisabled(false);
    ui->pushButtonLoadSettings->setDisabled(false);
    ui->pushButtonSettings->setDisabled(false);
}

void Dialog::statusRelais1(QString text)
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
        ui->checkBoxRelais7->setChecked(false);
        return;
    }
    else if(text=="R41\r\n")
    {
        ui->checkBoxRelais7->setChecked(true);
        return;
        return;
    }
    else if(text=="R50\r\n")
    {
        return;
    }
    else if(text=="R51\r\n")
    {
        return;
    }

    // status of Relais communication changed
    else if(text == "" || text == "stopped" || text == "not active")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatus1->setPalette(palette);
        ui->lineEditStatus1->setText(text);
    }
    else if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatus1->setPalette(palette);
        ui->lineEditStatus1->setText(text);
    }
    else if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatus1->setPalette(palette);
        ui->lineEditStatus1->setText(text);
    }
    else if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatus1->setPalette(palette);
        ui->lineEditStatus1->setText(text);
    }
}

void Dialog::statusRelais2(QString text)
{
    // status from Relais

    // Relais status changed on Arduino board
    if(text=="R10\r\n")
    {
        ui->checkBoxRelais4->setChecked(false);
        return;
    }
    else if(text=="R11\r\n")
    {
        ui->checkBoxRelais4->setChecked(true);
        return;
    }
    else if(text=="R1T\r\n")
    {
        return;
    }
    else if(text=="R20\r\n")
    {
        ui->checkBoxRelais5->setChecked(false);
        return;
    }
    else if(text=="R21\r\n")
    {
        ui->checkBoxRelais5->setChecked(true);
        return;
    }
    else if(text=="R30\r\n")
    {
        ui->checkBoxRelais6->setChecked(false);
        return;
    }
    else if(text=="R31\r\n")
    {
        ui->checkBoxRelais6->setChecked(true);
        return;
    }
    else if(text=="R40\r\n")
    {
        ui->checkBoxRelais8->setChecked(false);
        return;
    }
    else if(text=="R41\r\n")
    {
        ui->checkBoxRelais8->setChecked(true);
        return;
    }
    else if(text=="R50\r\n")
    {
        return;
    }
    else if(text=="R51\r\n")
    {
        return;
    }

    // status of Relais communication changed
    else if(text == "" || text == "stopped" || text == "not active")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatus2->setPalette(palette);
        ui->lineEditStatus2->setText(text);
    }
    else if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatus2->setPalette(palette);
        ui->lineEditStatus2->setText(text);
    }
    else if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatus2->setPalette(palette);
        ui->lineEditStatus2->setText(text);
    }
    else if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatus2->setPalette(palette);
        ui->lineEditStatus2->setText(text);
    }
}

void Dialog::statusMultimeter1(QString text)
{
    ui->lineEditStatusMultimeter1->setText(text);

    if(text == "" || text == "stopped" || text == "not active")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusMultimeter1->setPalette(palette);
    }
    if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatusMultimeter1->setPalette(palette);
    }
    if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusMultimeter1->setPalette(palette);
    }
    if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusMultimeter1->setPalette(palette);
    }
}

void Dialog::statusMultimeter2(QString text)
{
    ui->lineEditStatusMultimeter2->setText(text);

    if(text == "" || text == "stopped" || text == "not active")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusMultimeter2->setPalette(palette);
    }
    if(text == "reconnect")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatusMultimeter2->setPalette(palette);
    }
    if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusMultimeter2->setPalette(palette);
    }
    if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusMultimeter2->setPalette(palette);
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
        dltRelais1.on(1);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(1),"On");
    }
    else
    {
        dltRelais1.off(1);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(1),"Off");
    }
}

void Dialog::on_pushButtonRelais1Trigger_clicked()
{
    relais1Trigger(500);
}

void Dialog::relais1Trigger(unsigned int duration)
{
    dltRelais1.trigger(1,duration);
    ui->checkBoxRelais1->setChecked(false);
    dltMiniServer.sendValue3(dltRelais1.getRelaisName(1),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_checkBoxRelais2_clicked(bool checked)
{
    if(checked)
    {
        dltRelais1.on(2);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(2),"On");
    }
    else
    {
        dltRelais1.off(2);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(2),"Off");
    }
}

void Dialog::on_pushButtonRelais2Trigger_clicked()
{
    relais2Trigger(500);
}

void Dialog::relais2Trigger(unsigned int duration)
{
    dltRelais1.trigger(2,duration);
    ui->checkBoxRelais2->setChecked(false);
    dltMiniServer.sendValue3(dltRelais1.getRelaisName(2),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_checkBoxRelais3_clicked(bool checked)
{
    if(checked)
    {
        dltRelais1.on(3);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(3),"On");
    }
    else
    {
        dltRelais1.off(3);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(3),"Off");
    }
}

void Dialog::on_pushButtonRelais3Trigger_clicked()
{
    relais3Trigger(500);
}

void Dialog::relais3Trigger(unsigned int duration)
{
    dltRelais1.trigger(3,duration);
    ui->checkBoxRelais3->setChecked(false);
    dltMiniServer.sendValue3(dltRelais1.getRelaisName(3),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_pushButtonRelais4Trigger_clicked()
{
    relais4Trigger(500);
}

void Dialog::relais4Trigger(unsigned int duration)
{
    dltRelais2.trigger(1,duration);
    ui->checkBoxRelais4->setChecked(false);
    dltMiniServer.sendValue3(dltRelais2.getRelaisName(1),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_pushButtonRelais5Trigger_clicked()
{
    relais5Trigger(500);
}

void Dialog::relais5Trigger(unsigned int duration)
{
    dltRelais2.trigger(2,duration);
    ui->checkBoxRelais5->setChecked(false);
    dltMiniServer.sendValue3(dltRelais2.getRelaisName(2),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_pushButtonRelais6Trigger_clicked()
{
    relais6Trigger(500);
}

void Dialog::relais6Trigger(unsigned int duration)
{
    dltRelais2.trigger(3,duration);
    ui->checkBoxRelais5->setChecked(false);
    dltMiniServer.sendValue3(dltRelais2.getRelaisName(3),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_pushButtonRelais7Trigger_clicked()
{
    relais7Trigger(500);
}

void Dialog::relais7Trigger(unsigned int duration)
{
    dltRelais1.trigger(4,duration);
    ui->checkBoxRelais7->setChecked(false);
    dltMiniServer.sendValue3(dltRelais1.getRelaisName(4),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_pushButtonRelais8Trigger_clicked()
{
    relais8Trigger(500);
}

void Dialog::relais8Trigger(unsigned int duration)
{
    dltRelais2.trigger(4,duration);
    ui->checkBoxRelais8->setChecked(false);
    dltMiniServer.sendValue3(dltRelais2.getRelaisName(4),"Trigger",QString("%1").arg(duration));
}

void Dialog::on_checkBoxRelais4_clicked(bool checked)
{
    if(checked)
    {
        dltRelais2.on(1);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(1),"On");
    }
    else
    {
        dltRelais2.off(1);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(1),"Off");
    }
}

void Dialog::on_checkBoxRelais5_clicked(bool checked)
{
    if(checked)
    {
        dltRelais2.on(2);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(2),"On");
    }
    else
    {
        dltRelais2.off(2);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(2),"Off");
    }
}


void Dialog::on_checkBoxRelais6_clicked(bool checked)
{
    if(checked)
    {
        dltRelais2.on(3);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(3),"On");
    }
    else
    {
        dltRelais2.off(3);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(3),"Off");
    }
}

void Dialog::on_checkBoxRelais7_clicked(bool checked)
{
    if(checked)
    {
        dltRelais1.on(4);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(4),"On");
    }
    else
    {
        dltRelais1.off(4);
        dltMiniServer.sendValue2(dltRelais1.getRelaisName(4),"Off");
    }
}

void Dialog::on_checkBoxRelais8_clicked(bool checked)
{
    if(checked)
    {
        dltRelais2.on(4);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(4),"On");
    }
    else
    {
        dltRelais2.off(4);
        dltMiniServer.sendValue2(dltRelais2.getRelaisName(4),"Off");
    }
}

void Dialog::on_pushButtonDefaultSettings_clicked()
{
    // Reset settings to default
    dltRelais1.clearSettings();
    dltRelais2.clearSettings();
    dltMiniServer.clearSettings();
    dltMultimeter1.clearSettings();
    dltMultimeter2.clearSettings();

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
    dltRelais1.readSettings(fileName,1);
    dltRelais2.readSettings(fileName,2);
    dltMultimeter1.readSettings(fileName,1);
    dltMultimeter2.readSettings(fileName,2);
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
        dltRelais1.writeSettings(xml,1);
        dltRelais2.writeSettings(xml,2);
        dltMultimeter1.writeSettings(xml,1);
        dltMultimeter2.writeSettings(xml,2);
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

    dlg.restoreSettings(&dltRelais1, &dltRelais2, &dltMultimeter1, &dltMultimeter2, &dltMiniServer);
    if(dlg.exec()==QDialog::Accepted)
    {
        dlg.backupSettings(&dltRelais1, &dltRelais2, &dltMultimeter1, &dltMultimeter2, &dltMiniServer);
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

void Dialog::valueMultimeter1(QString value,QString unit)
{
    if(unit=="A" || unit=="mA")
    {
        ui->lineEditUnitA1->setText(unit);
        ui->lineEditValueA1->setText(value);
    }
    if(unit=="V" || unit=="mV")
    {
        ui->lineEditUnitV1->setText(unit);
        ui->lineEditValueV1->setText(value);
    }

    if(unit=="A")
        dltMiniServer.sendValue2("POWR","AMP",dltMultimeter1.getPowerName(),value);
    else if(unit=="V")
        dltMiniServer.sendValue2("POWR","VOLT",dltMultimeter1.getPowerName(),value);
    else if(unit=="mA")
        dltMiniServer.sendValue2("POWR","MAMP",dltMultimeter1.getPowerName(),value);
    else if(unit=="mV")
        dltMiniServer.sendValue2("POWR","MVOL",dltMultimeter1.getPowerName(),value);
}

void Dialog::valueMultimeter2(QString value,QString unit)
{
    if(unit=="A" || unit=="mA")
    {
        ui->lineEditUnitA2->setText(unit);
        ui->lineEditValueA2->setText(value);
    }
    if(unit=="V" || unit=="mV")
    {
        ui->lineEditUnitV2->setText(unit);
        ui->lineEditValueV2->setText(value);
    }

    if(unit=="A")
        dltMiniServer.sendValue2("POWR","AMP",dltMultimeter2.getPowerName(),value);
    else if(unit=="V")
        dltMiniServer.sendValue2("POWR","VOLT",dltMultimeter2.getPowerName(),value);
    else if(unit=="mA")
        dltMiniServer.sendValue2("POWR","MAMP",dltMultimeter2.getPowerName(),value);
    else if(unit=="mV")
        dltMiniServer.sendValue2("POWR","MVOL",dltMultimeter2.getPowerName(),value);
}

void Dialog::on_checkBoxPower1_clicked(bool checked)
{
    if(checked)
    {
        dltMultimeter1.on();
        dltMiniServer.sendValue2(dltMultimeter1.getPowerName(),"On");
    }
    else
    {
        dltMultimeter1.off();
        dltMiniServer.sendValue2(dltMultimeter1.getPowerName(),"Off");
    }
}

void Dialog::on_checkBoxPower2_clicked(bool checked)
{
    if(checked)
    {
        dltMultimeter2.on();
        dltMiniServer.sendValue2(dltMultimeter2.getPowerName(),"On");
    }
    else
    {
        dltMultimeter2.off();
        dltMiniServer.sendValue2(dltMultimeter2.getPowerName(),"Off");
    }
}

void Dialog::injection(QString text)
{
    QStringList list = text.split(' ');

    qDebug() << "Injection received: " << text;

    if(list[0]=="VOLTAGE1")
    {
        float value = list[1].toFloat();
        dltMultimeter1.setVoltage(value);
    }
    else if(list[0]=="VOLTAGE2")
    {
        float value = list[1].toFloat();
        dltMultimeter2.setVoltage(value);
    }
    else if(dltRelais1.getRelaisName(1) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais1->setChecked(true);
            on_checkBoxRelais1_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais1->setChecked(false);
            on_checkBoxRelais1_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais1Trigger(list[2].toUInt());
            }
            else
            {
                relais1Trigger(500);
            }
        }
    }
    else if(dltRelais1.getRelaisName(2) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais2->setChecked(true);
            on_checkBoxRelais2_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais2->setChecked(false);
            on_checkBoxRelais2_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais2Trigger(list[2].toUInt());
            }
            else
            {
                relais2Trigger(500);
            }
        }
    }
    else if(dltRelais1.getRelaisName(3) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais3->setChecked(true);
            on_checkBoxRelais3_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais3->setChecked(false);
            on_checkBoxRelais3_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais3Trigger(list[2].toUInt());
            }
            else
            {
                relais3Trigger(500);
            }
        }
    }
    else if(dltRelais1.getRelaisName(4) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais7->setChecked(true);
            on_checkBoxRelais7_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais7->setChecked(false);
            on_checkBoxRelais7_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais7Trigger(list[2].toUInt());
            }
            else
            {
                relais7Trigger(500);
            }
        }
    }
    else if(dltRelais2.getRelaisName(1) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais4->setChecked(true);
            on_checkBoxRelais4_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais4->setChecked(false);
            on_checkBoxRelais4_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais4Trigger(list[2].toUInt());
            }
            else
            {
                relais4Trigger(500);
            }
        }
    }
    else if(dltRelais2.getRelaisName(2) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais5->setChecked(true);
            on_checkBoxRelais5_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais5->setChecked(false);
            on_checkBoxRelais5_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais5Trigger(list[2].toUInt());
            }
            else
            {
                relais5Trigger(500);
            }
        }
    }
    else if(dltRelais2.getRelaisName(3) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais6->setChecked(true);
            on_checkBoxRelais6_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais6->setChecked(false);
            on_checkBoxRelais6_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais6Trigger(list[2].toUInt());
            }
            else
            {
                relais6Trigger(500);
            }
        }
    }
    else if(dltRelais2.getRelaisName(4) == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxRelais8->setChecked(true);
            on_checkBoxRelais8_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxRelais8->setChecked(false);
            on_checkBoxRelais8_clicked(false);
        }
        else if(list[1]=="trigger")
        {
            if(list.length()>2)
            {
                relais8Trigger(list[2].toUInt());
            }
            else
            {
                relais8Trigger(500);
            }
        }
    }

    if(dltMultimeter1.getPowerName() == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxPower1->setChecked(true);
            on_checkBoxPower1_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxPower1->setChecked(false);
            on_checkBoxPower1_clicked(false);
        }
    }
    else if(dltMultimeter2.getPowerName() == list[0])
    {
        if(list[1]=="on")
        {
            ui->checkBoxPower2->setChecked(true);
            on_checkBoxPower2_clicked(true);
        }
        else if(list[1]=="off")
        {
            ui->checkBoxPower2->setChecked(false);
            on_checkBoxPower2_clicked(false);
        }
    }

}


