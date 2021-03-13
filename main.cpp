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
 * \file main.h
 * @licence end@
 */

#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("alexmucde");
    QCoreApplication::setOrganizationDomain("github.com");
    QCoreApplication::setApplicationName("DLTRelais");

    Dialog w;
    w.show();
    return a.exec();
}
