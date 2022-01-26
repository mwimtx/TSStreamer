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

#include "globaldata.h"

GlobalData::GlobalData() : QObject ()
{
    std::vector<CustomPlotItem*> vecTemp;
    vecTemp.resize(5, NULL);

    this->vecPlots.push_back(vecTemp);
    this->vecPlots.push_back(vecTemp);

    this->uiWindowSize = 4096;
    emit (sigWindowSizeChanged (this->uiWindowSize));

    qvecPlotVisible.clear();
    for (unsigned int uiCounter = 0; uiCounter < 5; uiCounter++)
    {
        qvecPlotVisible.push_back(true);
    }

    pclMarginGroup = NULL;
}

bool GlobalData::registerPlot (const unsigned int uiType, const unsigned int uiChannel, CustomPlotItem* pclPlot)
{
    bool bRetValue = true;

    if (uiType < this->vecPlots.size())
    {
        if (uiChannel < this->vecPlots[uiType].size ())
        {
            this->vecPlots[uiType][uiChannel] = pclPlot;
        }
        else
        {
            bRetValue = false;
        }

        if (pclMarginGroup == NULL)
        {
            pclMarginGroup = new QCPMarginGroup (pclPlot->m_CustomPlot);
        }
        pclPlot->m_CustomPlot->axisRect(0)->setMarginGroup(QCP::msLeft|QCP::msRight, pclMarginGroup);
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}

bool GlobalData::deregisterPlot (const unsigned int uiType, const unsigned int uiChannel)
{
    bool bRetValue = true;

    if (uiType < this->vecPlots.size())
    {
        if (uiChannel < this->vecPlots[uiType].size ())
        {
            this->vecPlots[uiType][uiChannel] = NULL;
        }
        else
        {
            bRetValue = false;
        }
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}


bool GlobalData::isPlotVisible (const unsigned int uiPlotNum)
{
    bool bRetValue = false;

    if (uiPlotNum < this->vecPlots[0].size())
    {
        if (this->vecPlots[0] [uiPlotNum] != NULL)
        {
            if (qvecPlotVisible [uiPlotNum] == true)
            {
                bRetValue = true;
            }
        }
    }

    return (bRetValue);
}


void GlobalData::setPlotVisible (const unsigned int uiIndex, const bool bVisible)
{
    unsigned int uiLastVisiblePlotIndex = 0xFFFFFFFF;
    unsigned int uiCounter;

    qDebug () << "set plot visible:" << uiIndex << "-" << bVisible;
    if (uiIndex < this->vecPlots[0].size())
    {
        if (this->vecPlots[0] [uiIndex] != NULL)
        {
            qvecPlotVisible [uiIndex] = bVisible;
        }
        else
        {
            qvecPlotVisible [uiIndex] = bVisible;
        }
    }

    for (uiCounter = 0; uiCounter < (unsigned int)this->qvecPlotVisible.size(); uiCounter++)
    {
        if (this->qvecPlotVisible.at(uiCounter) == true)
        {
            this->vecPlots[0][uiCounter]->showPlot (false);
            this->vecPlots[1][uiCounter]->showPlot (false);
            uiLastVisiblePlotIndex = uiCounter;
        }
        else
        {
            this->vecPlots[0][uiCounter]->hidePlot ();
            this->vecPlots[1][uiCounter]->hidePlot ();
        }
    }

    if (uiLastVisiblePlotIndex != 0xFFFFFFFF)
    {
        this->vecPlots[0][uiLastVisiblePlotIndex]->showPlot (true);
        this->vecPlots[1][uiLastVisiblePlotIndex]->showPlot (true);
    }
}
