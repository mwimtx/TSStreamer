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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QObject>

#include "qcustomplot.h"
#include "qmlplot.h"



class GlobalData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(QString targetIP   READ targetIP   WRITE setTargetIP   NOTIFY targetIPChanged)

    private:

        unsigned int uiWindowSize;

        QString qstrTargetIP;

        QCPMarginGroup* pclMarginGroup;

        QVector<bool> qvecPlotVisible;

    public slots:
        Q_INVOKABLE void slotWindowSizeChanged (const QString& qstrNewWindowSize)
        {
            this->uiWindowSize = qstrNewWindowSize.toUInt();

            qDebug () << "new window size:" << this->uiWindowSize;

            emit (sigWindowSizeChanged (this->uiWindowSize));
        }


        Q_INVOKABLE void setPlotVisible (const unsigned int uiIndex, const bool bVisible);


    signals:
        void sigWindowSizeChanged (unsigned int);
        void windowSizeChanged    (void);
        void targetIPChanged      (QString);
        void sigTargetIPChanged   (QString);

    public:
        std::vector<std::vector<CustomPlotItem*>> vecPlots;

        GlobalData();

        bool registerPlot   (const unsigned int uiType, const unsigned int uiChannel, CustomPlotItem* pclPlot);

        bool deregisterPlot (const unsigned int uiType, const unsigned int uiChannel);

        QString windowSize (void) const
        {
            qDebug () << "get window size:" << this->uiWindowSize;
            return (QString::number (this->uiWindowSize));
        }

        QString targetIP (void) const
        {
            return (this->qstrTargetIP);
        }

        void setWindowSize (const QString& qstrNewWindowSize)
        {
            this->uiWindowSize = qstrNewWindowSize.toUInt();

            emit (sigWindowSizeChanged (this->uiWindowSize));
        }

        void setTargetIP (const QString& qstrNewTargetIP)
        {
            this->qstrTargetIP = qstrNewTargetIP;

            emit (sigTargetIPChanged (this->qstrTargetIP));
        }

        bool isPlotVisibleTs1 ()
        {
            return (this->isPlotVisible(0));
        }

        bool isPlotVisibleTs2 ()
        {
            return (this->isPlotVisible(1));
        }

        bool isPlotVisibleTs3 ()
        {
            return (this->isPlotVisible(2));
        }

        bool isPlotVisibleTs4 ()
        {
            return (this->isPlotVisible(3));
        }

        bool isPlotVisibleTs5 ()
        {
            return (this->isPlotVisible(4));
        }

        bool isPlotVisible (const unsigned int uiPlotNum);
};


extern GlobalData* pclGlobalData;

#endif // GLOBALDATA_H
