/*
 * Velocity-Next - Xbox 360 File Manager
 * 
 * Original Velocity Copyright (C) 2007-2015 Velocity Contributors
 * Velocity-Next Copyright (C) 2025-present Velocity-Next Contributors
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 * For information about the original Velocity project, see NOTICE file.
 */

#include <QtWidgets/QApplication>
#include <QStringList>
#include <QNetworkInformation>
#include "mainwindow.h"
#include "thememanager.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#error This project requires Qt 5.0.0 or higher.
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.addLibraryPath(":/plugins/imageformats");

    // It's good practice to register custom types in main
    qRegisterMetaType<FatxFileEntry*>("FatxFileEntry*");
    qRegisterMetaType<FatxDrive*>("FatxDrive*");
    qRegisterMetaType<Partition*>("Partition*");
    qRegisterMetaType<TitleEntry>("TitleEntry");
    qRegisterMetaType<StfsPackage*>("StfsPackage*");
    qRegisterMetaType<StfsFileEntry*>("StfsFileEntry*");
    qRegisterMetaType<AvatarAwardData*>("AvatarAwardData*");

    // Initialize theme manager (supports Auto/Dark/Light modes)
    ThemeManager::instance().initialize();

    QList<QUrl> args;
    for (int i = 1; i < argc; i++)
        args.append(QUrl("file:///" + QString::fromLatin1(argv[i]).replace("\\", "/")));

    QNetworkInformation::loadDefaultBackend();

    MainWindow w(args);
    w.show();

    return a.exec();
}


