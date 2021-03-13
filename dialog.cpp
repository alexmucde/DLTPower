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

#include "dialog.h"
#include "ui_dialog.h"
#include "settingsdialog.h"
#include "version.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , dltRelais(this)
{
    ui->setupUi(this);

    on_pushButtonDefaultSettings_clicked();

    setWindowTitle(QString("DLTRelais %1").arg(DLT_RELAIS_VERSION));

    ui->pushButtonStop->setDisabled(true);

    connect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    connect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    /*  load global settings */
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    bool autoload = settings.value("autoload/checked").toBool();
    bool autostart = settings.value("autostart/checked").toBool();

    /* autoload settings */
    if(autoload)
    {
        dltRelais.readSettings(filename);
        dltMiniServer.readSettings(filename);
        restoreSettings();
    }

    /* autostart */
    if(autostart)
    {
        on_pushButtonStart_clicked();
    }
}

Dialog::~Dialog()
{
    disconnect(&dltRelais, SIGNAL(status(QString)), this, SLOT(statusRelais(QString)));
    disconnect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    delete ui;
}

void Dialog::restoreSettings()
{
    /* DLTRelais */
    ui->lineEditRelaisName1->setText(dltRelais.getRelaisName(1));
    ui->lineEditRelaisName2->setText(dltRelais.getRelaisName(2));
    ui->lineEditRelaisName3->setText(dltRelais.getRelaisName(3));

}

void Dialog::updateSettings()
{
}

void Dialog::on_pushButtonStart_clicked()
{
    updateSettings();

    dltRelais.start();
    dltMiniServer.start();

    ui->pushButtonStart->setDisabled(true);
    ui->pushButtonStop->setDisabled(false);
    ui->pushButtonDefaultSettings->setDisabled(true);
    ui->pushButtonLoadSettings->setDisabled(true);
    ui->pushButtonSettings->setDisabled(true);
}

void Dialog::on_pushButtonStop_clicked()
{
    dltRelais.stop();
    dltMiniServer.stop();

    ui->pushButtonStart->setDisabled(false);
    ui->pushButtonStop->setDisabled(true);
    ui->pushButtonDefaultSettings->setDisabled(false);
    ui->pushButtonLoadSettings->setDisabled(false);
    ui->pushButtonSettings->setDisabled(false);
}

void Dialog::statusRelais(QString text)
{
    ui->lineEditStatus->setText(text);
}

void Dialog::statusDlt(QString text)
{
    ui->lineEditStatusDLT->setText(text);
}

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

void Dialog::on_pushButtonDefaultSettings_clicked()
{
    dltRelais.clearSettings();
    dltMiniServer.clearSettings();

    restoreSettings();
}

void Dialog::on_pushButtonLoadSettings_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings"), "", tr("DLTRelais Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        return;
    }

    dltRelais.readSettings(fileName);
    dltMiniServer.readSettings(fileName);

    restoreSettings();
}

void Dialog::on_pushButtonSaveSettings_clicked()
{
    updateSettings();

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Settings"), "", tr("DLTRelais Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
             return;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    //xml.writeStartDocument();

    xml.writeStartElement("DLTRelaisSettings");
        dltRelais.writeSettings(xml);
        dltMiniServer.writeSettings(xml);
    xml.writeEndElement(); // DLTRelaisSettings

    //xml.writeEndDocument();
    file.close();

}

void Dialog::on_pushButtonSettings_clicked()
{
    SettingsDialog dlg(this);

    dlg.restoreSettings(&dltRelais, &dltMiniServer);

    dlg.exec();

    dlg.backupSettings(&dltRelais, &dltMiniServer);

    restoreSettings();
}

void Dialog::on_pushButtonInfo_clicked()
{
    QMessageBox msgBox(this);

    msgBox.setWindowTitle("Info DLTRelais");
    msgBox.setTextFormat(Qt::RichText);

    QString text;
    text += "Information and Documentation can be found here:<br>";
    text += "<br>";
    text += "<a href='https://github.com/alexmucde/DLTRelais'>https://github.com/alexmucde/DLTRelais</a><br>";
    text += "<br>";
    text += "This SW is licensed under GPLv3.<br>";
    text += "<br>";
    text += "(C) 2021 Alexander Wenzel <alex@eli2.de>";

    msgBox.setText(text);

    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}
