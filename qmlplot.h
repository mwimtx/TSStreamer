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

#ifndef QMLPLOT_H
#define QMLPLOT_H

#include <QtQuick>
#include <QtGlobal>
#include "qcustomplot.h"

class QCustomPlot;
class QCPAbstractPlottable;

enum ePlotType_t
{
    PlotTypeSpectra = 0,
    PlotTypeTS      = 1
};


enum eStackedSpectraMode_t
{
    STSpectraFirstBuffer = 0,
    STSpectraContBuffer  = 1,
    STSpectraLastBuffer  = 2
};


typedef struct
{
    QCPItemLine *hLine;
    QCPItemLine *vLine;

    QVector<double> qvecPoint;

    double dXPixel;

    bool bCursorValid;

} QCPCursor;


class CustomPlotItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    CustomPlotItem( QQuickItem* parent = 0 );
    virtual ~CustomPlotItem();

    QCustomPlot*         m_CustomPlot;

    void paint( QPainter* painter );

    Q_INVOKABLE void initCustomPlot(unsigned int uiType, unsigned int uiChannel);

    bool showNewDataTS      (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, double dNewBufferStartTimeMillis, unsigned int uiNewChannel, const bool bShowXAxis);
    bool showNewDataSpectra (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, unsigned int uiNewChannel, const bool bShowXAxis);

    bool showNewDataSTSpectra (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, unsigned int uiNewChannel, const bool bShowXAxis, const eStackedSpectraMode_t eSTBufferMode);

    bool hidePlot (void);
    bool showPlot (const bool bShowXAxis = false);

protected:
    void routeMouseEvents( QMouseEvent* event );
    void routeWheelEvents( QWheelEvent* event );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent *event );

    virtual void timerEvent(QTimerEvent *event);

private:
    int                  m_timerId;
    unsigned int         uiMyType;
    unsigned int         uiMyChannel;
    unsigned int         uiWindowSize;

    QVector<double>      qvecDataTS;
    QVector<double>      qvecDataSpectra;
    double               dSampleFreq;
    time_t               sBufferStartTime;
    unsigned int         uiChannel;
    bool                 bShowAxis;

    bool                 bTSPlotInitDone;
    bool                 bSpectraPlotInitDone;

    double              dBufferStartTimeMillis;

    QVector<Qt::GlobalColor> qvecColours;

    QVector<double>      qvecSTSpectraXAxis;
    QVector<double>      qvecSTSpectraYAxis;
    unsigned int         uiSTSpectraNumStacks;

    ePlotType_t ePlotType;

    bool updateGuiTS      (void);
    bool updateGuiSpectra (void);
    bool updateGuiSTSpectra (void);

    QCPCursor qCursor;

    void computeFFT (QVector<double>& qvecIn, QVector<double>& qvecOutAmpl, QVector<double>& qvecOutPhase, QVector<double>& qvecOutSpectra, const double dSampleFreq, const unsigned int uiActWindowSize);

    void ManageCursor(QCustomPlot *customPlot, QCPCursor *cursor, QVector<double> qvecPoint, QPen pen);

    void showTickLabels (void);

private slots:
    void graphClicked( QCPAbstractPlottable* plottable );
    void onCustomReplot();
    void updateCustomPlotSize();

public slots:
    void slotNewWindowSize (unsigned int uiNewWindowSize);

    void slotGetValue (const unsigned int uiBufferIndex, double& dXValue, double &dYValue);

    unsigned int slotGetValueMaxIndex (const unsigned int uiStartIndex);

signals:
    void sigUpdateCursor (const double, const enum ePlotType_t, const unsigned int uiPlotNum);

};

#endif // QMLPLOT_H
