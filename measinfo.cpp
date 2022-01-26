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

#include <QDateTime>

#include "measinfo.h"

MeasInfo::MeasInfo(const int iIndexID)
{
    this->iMeasID = iIndexID;
}


QString MeasInfo::getStartDateString (void)
{
    QString qstrRetValue;

    if (this->qvecATSHeaders.size() > 0)
    {
        QDateTime clStartDate;
        clStartDate.setTimeSpec(Qt::UTC);
        clStartDate  = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clStartDate  = clStartDate.toLocalTime();
        qstrRetValue = clStartDate.toString("yyyy-MM-dd HH:mm:ss");
    }
    else
    {
        qstrRetValue = "inv. Startdate";
    }

    return (qstrRetValue);
}


unsigned int MeasInfo::getStartDate (void)
{
    if (this->qvecATSHeaders.size() > 0)
    {
        QDateTime clStartDate;
        clStartDate.setTimeSpec(Qt::UTC);
        return (QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime).toTime_t());
    }
    else
    {
        return (0);
    }
}


QString MeasInfo::getDurationString (void)
{
    QString         qstrRetValue;
    QString         qstrTemp;
    unsigned int    uiDays;
    unsigned int    uiHours;
    unsigned int    uiMinutes;
    unsigned int    uiSeconds;
    unsigned int    uiTemp;

    if (this->qvecATSHeaders.size() > 0)
    {
        uiTemp = this->qvecATSHeaders.at(0).iSamples / this->qvecATSHeaders.at(0).rSampleFreq;

        uiDays = (uiTemp / 86400);
        uiTemp = uiTemp - (uiDays * 86400);

        uiHours = (uiTemp / 3600);
        uiTemp  = uiTemp - (uiHours * 3600);

        uiMinutes = (uiTemp / 60);
        uiTemp    = uiTemp - (uiMinutes * 60);

        uiSeconds = uiTemp;

        qstrTemp = QString::number(uiDays);
        while (qstrTemp.size() < 3)
        {
            qstrTemp.prepend("0");
        }
        qstrRetValue = qstrTemp + "d ";

        qstrTemp = QString::number(uiHours);
        while (qstrTemp.size() < 2)
        {
            qstrTemp.prepend("0");
        }
        qstrRetValue += qstrTemp + ":";

        qstrTemp = QString::number(uiMinutes);
        while (qstrTemp.size() < 2)
        {
            qstrTemp.prepend("0");
        }
        qstrRetValue += qstrTemp + ":";

        qstrTemp = QString::number(uiSeconds);
        while (qstrTemp.size() < 2)
        {
            qstrTemp.prepend("0");
        }
        qstrRetValue += qstrTemp;
    }
    else
    {
        qstrTemp = "inv. Duration";
    }

    return (qstrRetValue);
}


QString MeasInfo::getSampleFreqString (void)
{
    QString qstrRetValue;

    if (this->qvecATSHeaders.size() > 0)
    {
        if (this->qvecATSHeaders.at(0).rSampleFreq < 0.0)
        {
            qstrRetValue  = QString::number((1.0 / this->qvecATSHeaders.at(0).rSampleFreq), 'f', 2);
            qstrRetValue += "s";
        }
        else
        {
            qstrRetValue  = QString::number(this->qvecATSHeaders.at(0).rSampleFreq, 'f', 2);
            qstrRetValue += "Hz";
        }
    }
    else
    {
        qstrRetValue = "inv. Sample Freq.";
    }

    return (qstrRetValue);
}


int MeasInfo::getMeasID (void)
{
    return (this->iMeasID);
}


bool MeasInfo::operator< (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() < clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


bool MeasInfo::operator> (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() > clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


bool MeasInfo::operator<= (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() <= clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


bool MeasInfo::operator>= (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() >= clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


bool MeasInfo::operator== (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() == clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


bool MeasInfo::operator!= (const MeasInfo& rhs)
{
    QDateTime clLhsStartDate = QDateTime::fromSecsSinceEpoch(0);
    QDateTime clRhsStartDate = QDateTime::fromSecsSinceEpoch(0);

    if (this->qvecATSHeaders.size() > 0)
    {
        clLhsStartDate.setTimeSpec(Qt::UTC);
        clLhsStartDate = QDateTime::fromSecsSinceEpoch(this->qvecATSHeaders.at(0).iStartDateTime);
        clLhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (rhs.qvecATSHeaders.size() > 0)
    {
        clRhsStartDate.setTimeSpec(Qt::UTC);
        clRhsStartDate = QDateTime::fromSecsSinceEpoch(rhs.qvecATSHeaders.at(0).iStartDateTime);
        clRhsStartDate = clLhsStartDate.toLocalTime();
    }

    if (clLhsStartDate.toTime_t() != clRhsStartDate.toTime_t())
    {
        return (true);
    }
    else
    {
        return (false);
    }
}


MeasInfo& MeasInfo::operator=(const MeasInfo & clInstanceToCopy)
{
    if(this == &clInstanceToCopy)
    {
       return (*this);
    }

    this->qstrRootPath      = clInstanceToCopy.qstrRootPath;

    this->qstrMeasDocXML    = clInstanceToCopy.qstrMeasDocXML;

    this->qvecATSFiles      = clInstanceToCopy.qvecATSFiles;

    this->qvecATSHeaders    = clInstanceToCopy.qvecATSHeaders;

    this->iMeasID           = clInstanceToCopy.iMeasID;


    return (*this);
}


MeasInfo::MeasInfo(const MeasInfo& clInit) : QObject ()
{
    this->qstrRootPath      = clInit.qstrRootPath;

    this->qstrMeasDocXML    = clInit.qstrMeasDocXML;

    this->qvecATSFiles      = clInit.qvecATSFiles;

    this->qvecATSHeaders    = clInit.qvecATSHeaders;

    this->iMeasID           = clInit.iMeasID;
}

