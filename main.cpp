/***************************************************************************
**                                                                        **
**  TSStreamer Online Time Series Viewer                                  **
**  Copyright (C) 2019-2022 metronix GmbH                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
** Parts of the software are linked against the Open Source Version of Qt **
** Qt source code copy is available under https://www.qt.io               **
** Qt source code modifications are non-proprietary,no modifications made **
** https://www.qt.io/licensing/open-source-lgpl-obligations               **
****************************************************************************
**           Author: metronix geophysics                                  **
**  Website/Contact: www.metronix.de                                      **
**                                                                        **
**                                                                        **
****************************************************************************/

#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qcustomplot.h>

#include "qmlplot.h"
#include "globaldata.h"
#include "tsstreamer.h"

GlobalData* pclGlobalData;

int main(int argc, char *argv[])
{
    TSStreamer* pclTSStreamer;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    qmlRegisterType<CustomPlotItem> ("CustomPlot", 1, 0, "CustomPlotItem");

    pclGlobalData = new GlobalData ();

    if (argc > 1)
    {
        pclTSStreamer = new TSStreamer (argv[1]);
    }
    else
    {
        pclTSStreamer = new TSStreamer ("");
    }

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tsStreamer", pclTSStreamer);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
