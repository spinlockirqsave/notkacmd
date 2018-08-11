/*
 * main.cpp
 *
 * Copyright(C) 2017, Piotr Gregor <piotr@dataandsignal.com>
 *
 * Notka Online Clipboard
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include "../inc/notka.h"
#include <QApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "../inc/globals.h"
#include <QDebug>


namespace Database {
        QMutex mutex;
}

static void usage(char *argv[])
{
        qDebug() << __func__ << QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss")
                << "Usage: \n\n" << argv[0] << " SSLcert SSLkey SSLcacert\n\n";
}

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);

        if (argc < 4) {

                usage(argv);
                return EXIT_FAILURE;
        }

        Notka notka(argv[1], argv[2], argv[3]);

        return a.exec();
}
