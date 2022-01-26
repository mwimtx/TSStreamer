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

#include "qmlplot.h"
#include "qcustomplot.h"
#include <QDebug>
#include <QVector>

#include <complex>
#include "fftreal_vector.h"
#include "globaldata.h"

#include <limits>


/*
void CustomPlotItem::ManageCursor(QCustomPlot *customPlot, QCPCursor *cursor, QVector<double> qvecPoint, QPen pen)
{
    QVector<double> qvecX;
    QVector<double> qvecY;

    if (qvecPoint.size() >= 2)
    {
        pen.setColor(Qt::darkCyan);

        if(cursor->hLine) customPlot->removeItem(cursor->hLine);
        cursor->hLine = new QCPItemLine(customPlot);
        customPlot->addItem(cursor->hLine);
        cursor->hLine->setPen(pen);

        if (this->ePlotType == PlotTypeTS)
        {
            QString qstrTemp;
            qstrTemp  = QString::number(qvecPoint[1], 'E', 2) + "\n";
            qstrTemp += "[mV]";

            customPlot->graph(1)->setName(qstrTemp);
        }
        else
        {
            double dIndex = this->dSampleFreq / this->uiWindowSize;
            dIndex = (qvecPoint[0] / dIndex) + 1;
            qvecPoint [0] = round (dIndex) * (this->dSampleFreq / this->uiWindowSize);
            qvecPoint [1] = this->qvecDataSpectra.at(dIndex);


            QString qstrTemp;
            qstrTemp  = QString::number((round (dIndex) + 1) * (this->dSampleFreq / this->uiWindowSize), 'E', 2) + "Hz\n";
            qstrTemp += QString::number(qvecPoint[1], 'E', 2) + "\n";
            qstrTemp += "[mV/sqrt(Hz)]";

            customPlot->graph(1)->setName(qstrTemp);
        }


        cursor->hLine->start->setCoords(QCPRange::minRange, qvecPoint [1]);
        cursor->hLine->end->setCoords(QCPRange::maxRange, qvecPoint [1]);

        if(cursor->vLine) customPlot->removeItem(cursor->vLine);
        cursor->vLine = new QCPItemLine(customPlot);
        customPlot->addItem(cursor->vLine);
        cursor->vLine->setPen(pen);
        cursor->vLine->start->setCoords (qvecPoint [0], QCPRange::minRange);
        cursor->vLine->end->setCoords   (qvecPoint [0], QCPRange::maxRange);

        qvecX.clear();
        qvecX.push_back(qvecPoint [0]);
        qvecY.push_back(qvecPoint [1]);
        customPlot->graph(1)->clearData();
        customPlot->graph(1)->setData(qvecX, qvecY);

        customPlot->rescaleAxes();
        customPlot->replot();
        this->onCustomReplot();
    }
}
*/


CustomPlotItem::CustomPlotItem( QQuickItem* parent ) : QQuickPaintedItem( parent )
    , m_CustomPlot( nullptr ), m_timerId( 0 )
{
    setFlag( QQuickItem::ItemHasContents, true );
    setAcceptedMouseButtons( Qt::AllButtons );

    connect( this, &QQuickPaintedItem::widthChanged,  this, &CustomPlotItem::updateCustomPlotSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );

    ePlotType = PlotTypeTS;

    qvecColours.clear();
    qvecColours.push_back(Qt::blue);
    qvecColours.push_back(Qt::red);
    qvecColours.push_back(Qt::green);
    qvecColours.push_back(Qt::black);
    qvecColours.push_back(Qt::darkCyan);

    this->qCursor.bCursorValid = false;

    this->bTSPlotInitDone = false;
    this->bSpectraPlotInitDone = false;
}

CustomPlotItem::~CustomPlotItem()
{
    pclGlobalData->deregisterPlot( this->uiMyType, this->uiMyChannel);

    delete m_CustomPlot;
    m_CustomPlot = nullptr;

    if(m_timerId != 0) {
        killTimer(m_timerId);
    }
}

void CustomPlotItem::initCustomPlot(unsigned int uiType, unsigned int uiChannel)
{
    m_CustomPlot = new QCustomPlot();

    qDebug () << "CustomPlotID = " << uiType << uiChannel;

    pclGlobalData->registerPlot(uiType, uiChannel, this);
    this->uiMyType    = uiType;
    this->uiMyChannel = uiChannel;
    this->bShowAxis   = false;

    updateCustomPlotSize();
    this->m_CustomPlot->clearGraphs();
    this->m_CustomPlot->addGraph   ();
    this->m_CustomPlot->addGraph   ();
//    this->m_CustomPlot->graph(0)->clearData();
//    this->m_CustomPlot->graph(1)->clearData();
    this->m_CustomPlot->graph(0)->data().clear();
    this->m_CustomPlot->graph(1)->data().clear();

    #ifdef QCUSTOMPLOT_USE_OPENGL
        this->m_CustomPlot->setOpenGl(true, 16);
    #endif

    this->qCursor.bCursorValid = false;
}


void CustomPlotItem::paint( QPainter* painter )
{
    if (m_CustomPlot)
    {
        QImage picture (boundingRect().size().toSize(), QImage::Format_ARGB32);

        QCPPainter qcpPainter(&picture);

        this->setRenderTarget(QQuickPaintedItem::FramebufferObject);

        qcpPainter.setAntialiasing(true);
        qcpPainter.setMode(QCPPainter::pmVectorized,  true);
        qcpPainter.setMode(QCPPainter::pmNonCosmetic, true);
        qcpPainter.setMode(QCPPainter::pmNoCaching,   true);

        m_CustomPlot->toPainter (&qcpPainter,  boundingRect().width(), boundingRect().height());
        painter->drawImage(QPoint(), picture);
    }
}


void CustomPlotItem::mousePressEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}


void CustomPlotItem::mouseReleaseEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}


void CustomPlotItem::mouseMoveEvent( QMouseEvent* event )
{
    double dAxisPos;
    double dMaxValue;

    if ((event->pos().x() >=  this->m_CustomPlot->x()) &&
        (event->pos().x() <= (this->m_CustomPlot->x() + this->m_CustomPlot->width())) &&
        (event->pos().y() >=  this->m_CustomPlot->y()) &&
        (event->pos().y() <= (this->m_CustomPlot->y() + this->m_CustomPlot->height())))
    {
        // compute data index for x-axis buffer from coordinates
        dAxisPos  = this->m_CustomPlot->xAxis->pixelToCoord(event->pos().x());
        dAxisPos  = dAxisPos - this->m_CustomPlot->graph(0)->dataMainKey (0);
        dMaxValue = this->m_CustomPlot->graph(0)->dataMainKey(this->m_CustomPlot->graph(0)->dataCount() - 1) - this->m_CustomPlot->graph(0)->dataMainKey (0);
        dAxisPos  = dAxisPos / dMaxValue;
        dAxisPos  = dAxisPos * (this->m_CustomPlot->graph(0)->dataCount() - 1);
        dAxisPos  = round (dAxisPos);

        if (dAxisPos > (this->m_CustomPlot->graph(0)->dataCount() - 1))
        {
            dAxisPos = (this->m_CustomPlot->graph(0)->dataCount() - 1);
        }

        if (dAxisPos < 0.0)
        {
            dAxisPos = 0.0;
        }

        emit (sigUpdateCursor(dAxisPos, this->ePlotType, this->uiChannel));
    }
}


void CustomPlotItem::mouseDoubleClickEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}


void CustomPlotItem::wheelEvent( QWheelEvent *event)
{
    routeWheelEvents( event );
}


void CustomPlotItem::timerEvent(QTimerEvent *event __attribute__ ((unused)))
{
    static double t, U;
    U = ((double)rand() / RAND_MAX) * 5;
    m_CustomPlot->graph(0)->addData(t, U);
    qDebug() << Q_FUNC_INFO << QString("Adding dot t = %1, S = %2").arg(t).arg(U);
    t++;
    m_CustomPlot->replot();
}

void CustomPlotItem::graphClicked( QCPAbstractPlottable* plottable )
{
    qDebug() << Q_FUNC_INFO << QString( "Clicked on graph '%1 " ).arg( plottable->name() );
}


void CustomPlotItem::routeMouseEvents( QMouseEvent* event )
{
    if (m_CustomPlot)
    {
        QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        QCoreApplication::postEvent( m_CustomPlot, newEvent );
    }
}


void CustomPlotItem::routeWheelEvents( QWheelEvent* event )
{
    if (m_CustomPlot)
    {
        QWheelEvent* newEvent = new QWheelEvent( event->pos(), event->delta(), event->buttons(), event->modifiers(), event->orientation() );
        QCoreApplication::postEvent( m_CustomPlot, newEvent );
    }
}


void CustomPlotItem::updateCustomPlotSize()
{
    if (m_CustomPlot)
    {
        m_CustomPlot->setGeometry(0, 0, (int)width(), (int)height());
        m_CustomPlot->setViewport(QRect(0, 0, (int)width(), (int)height()));
    }
}


void CustomPlotItem::onCustomReplot()
{
    qDebug() << Q_FUNC_INFO;
    update();
}


bool CustomPlotItem::updateGuiTS (void)
{
    bool                    bRetValue = true;
    unsigned int            uiCounter;
    QVector<double>         qvecYAxis;
    QVector<double>         qvecXAxis;

    double                  dYMax = std::numeric_limits<double>::min ();
    double                  dYMin = std::numeric_limits<double>::max ();

    unsigned int            uiMaxWindowLength;

    qDebug () << "updating GUI ...";

    uiMaxWindowLength = this->uiWindowSize;

    if ((unsigned int)qvecXAxis.size() != uiMaxWindowLength)
    {
        qvecXAxis.resize(uiMaxWindowLength);
        qvecYAxis.resize(uiMaxWindowLength);
    }

    if (this->m_CustomPlot->graphCount() > 1)
    {
        this->m_CustomPlot->removeGraph(1);
    }

    qDebug () << "buffer start-time:" << QDateTime::fromSecsSinceEpoch(this->sBufferStartTime).toString("HH:mm:ss-zzz");

    if ((unsigned int)this->qvecDataTS.size() < uiMaxWindowLength)
    {
        for (uiCounter = 0; uiCounter < uiMaxWindowLength; uiCounter++)
        {
            qvecXAxis [uiCounter] = ((double) sBufferStartTime) + (((double)uiCounter) / dSampleFreq) + this->dBufferStartTimeMillis;

            if (uiCounter >= (unsigned int)this->qvecDataTS.size())
            {
                if (this->qvecDataTS.size() <= 0)
                {
                    qvecYAxis [uiCounter] = 0.0;
                }
                else
                {
                    qvecYAxis [uiCounter] = (double) this->qvecDataTS.back();
                }
            }
            else
            {
                qvecYAxis [uiCounter] =  (double) this->qvecDataTS [uiCounter];
            }
        }
    }
    else
    {
        for (uiCounter = 0; uiCounter < uiMaxWindowLength; uiCounter++)
        {
            qvecXAxis [uiCounter] = ((double) sBufferStartTime) + (((double)uiCounter) / dSampleFreq) + this->dBufferStartTimeMillis;
            qvecYAxis [uiCounter] =  (double) this->qvecDataTS [uiCounter];
        }
    }

    for (uiCounter = 0; uiCounter < uiMaxWindowLength; uiCounter++)
    {
        if (qvecYAxis [uiCounter] < dYMax)
        {
            dYMax = qvecYAxis [uiCounter];
        }

        if (qvecYAxis [uiCounter] > dYMin)
        {
            dYMin = qvecYAxis [uiCounter];
        }
    }

    if (this->bTSPlotInitDone == false)
    {
        this->bTSPlotInitDone = true;
        this->m_CustomPlot->clearGraphs();
        this->m_CustomPlot->addGraph   ();

        this->m_CustomPlot->graph (0)->setName ("CH" + QString::number(this->uiMyChannel));

        QPen clPen = QPen (Qt::blue);
        if ((unsigned int)this->qvecColours.size() > this->uiMyChannel)
        {
            clPen.setColor(this->qvecColours[this->uiMyChannel]);
        }
        clPen.setWidth    (1);
        this->m_CustomPlot->graph (0)->setPen (clPen);
        this->m_CustomPlot->xAxis->setScaleType         (QCPAxis::stLinear);
        QSharedPointer<QCPAxisTickerDateTime> pclTicker (new  QCPAxisTickerDateTime());
        pclTicker->setDateTimeFormat("HH:mm:ss-zzz\nyyyy-MM-dd");
        this->m_CustomPlot->xAxis->setTicker(pclTicker);

        this->m_CustomPlot->yAxis->setScaleType      (QCPAxis::stLinear);
        this->m_CustomPlot->yAxis->setNumberPrecision(3);
        this->m_CustomPlot->yAxis->setNumberFormat   ("e");
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->yAxis->setTickLabelFont  (clFont);
        this->m_CustomPlot->axisRect()->setupFullAxesBox();

        // last entry is always 0, remove it to get correct scaling of y-axis
        qvecXAxis.pop_back();
        qvecYAxis.pop_back();
        m_CustomPlot->graph(0)->setData(qvecXAxis, qvecYAxis);
    }
    else
    {
        // last entry is always 0, remove it to get correct scaling of y-axis
        qvecXAxis.pop_back();
        qvecYAxis.pop_back();
        m_CustomPlot->graph(0)->setData(qvecXAxis, qvecYAxis);
    }

    if (this->bShowAxis == true)
    {
        this->m_CustomPlot->xAxis->setTickLabels (true);
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->xAxis->setTickLabelFont (clFont);
    }
    else
    {
        this->m_CustomPlot->xAxis->setTickLabels (false);
    }

    dYMin = dYMin - (fabs (dYMax - dYMin) * 1.2);
    dYMax = dYMax + (fabs (dYMax - dYMin) * 1.2);

    m_CustomPlot->yAxis->setRange(dYMin, dYMax);
    m_CustomPlot->rescaleAxes();

    this->onCustomReplot();

    return (bRetValue);
}


bool CustomPlotItem::updateGuiSpectra (void)
{
    bool                    bRetValue = true;
    unsigned int            uiCounter;
    QVector<double>         qvecYAxis;
    QVector<double>         qvecXAxis;
    QVector<double>         qvecTemp;
    unsigned int            uiMaxWindowLength;

    qDebug () << "updating GUI Spectra ...";

    this->computeFFT(this->qvecDataTS, this->qvecDataSpectra, qvecTemp, qvecXAxis, this->dSampleFreq, this->uiWindowSize);

    uiMaxWindowLength = this->qvecDataSpectra.size();
    uiMaxWindowLength = uiMaxWindowLength - (uiMaxWindowLength * 0.1);

    if ((unsigned int)qvecXAxis.size() != uiMaxWindowLength)
    {
        qvecXAxis.resize(uiMaxWindowLength);
        qvecYAxis.resize(uiMaxWindowLength);
    }

    for (uiCounter = 1; uiCounter < uiMaxWindowLength; uiCounter++)
    {
        qvecYAxis [uiCounter] =  (double) this->qvecDataSpectra [uiCounter];
    }
    qvecXAxis.erase(qvecXAxis.begin());
    qvecYAxis.erase(qvecYAxis.begin());

    if (this->m_CustomPlot->graphCount() > 1)
    {
        this->m_CustomPlot->removeGraph(1);
    }

    if (this->bSpectraPlotInitDone == false)
    {
        this->bSpectraPlotInitDone = true;
        this->m_CustomPlot->clearGraphs();
        this->m_CustomPlot->addGraph   ();

        this->m_CustomPlot->graph (0)->setName ("CH" + QString::number(this->uiMyChannel));

        QPen clPen = QPen (Qt::blue);
        if ((unsigned int)this->qvecColours.size() > this->uiMyChannel)
        {
            clPen.setColor(this->qvecColours[this->uiMyChannel]);
        }
        clPen.setWidth    (1);
        this->m_CustomPlot->graph (0)->setPen (clPen);

        this->m_CustomPlot->xAxis->setScaleType  (QCPAxis::stLogarithmic);
        this->m_CustomPlot->xAxis->ticker().clear();
        this->m_CustomPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog> (new  QCPAxisTickerLog));
        this->m_CustomPlot->xAxis->ticker()->setTickCount(10);
        this->m_CustomPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

        this->m_CustomPlot->yAxis->setScaleType  (QCPAxis::stLogarithmic);
        this->m_CustomPlot->yAxis->ticker().clear();
        this->m_CustomPlot->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog> (new  QCPAxisTickerLog));
        this->m_CustomPlot->yAxis->ticker()->setTickCount(10);
        this->m_CustomPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

        this->m_CustomPlot->yAxis->setNumberPrecision(3);
        this->m_CustomPlot->yAxis->setNumberFormat   ("e");
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->yAxis->setTickLabelFont  (clFont);
        this->m_CustomPlot->axisRect()->setupFullAxesBox();
        m_CustomPlot->graph(0)->setData(qvecXAxis, qvecYAxis);
    }
    else
    {
        m_CustomPlot->graph(0)->setData(qvecXAxis, qvecYAxis);
    }

    if (this->bShowAxis == true)
    {
        this->m_CustomPlot->xAxis->setTickLabels (true);
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->xAxis->setTickLabelFont (clFont);
    }
    else
    {
        this->m_CustomPlot->xAxis->setTickLabels (false);
    }

    m_CustomPlot->rescaleAxes();
    this->onCustomReplot();

    return (bRetValue);
}


bool CustomPlotItem::updateGuiSTSpectra (void)
{
    bool bRetValue = true;

    qDebug () << "updating GUI Stacked Spectra ...";

    if (this->m_CustomPlot->graphCount() > 1)
    {
        this->m_CustomPlot->removeGraph(1);
    }

    if (this->bSpectraPlotInitDone == false)
    {
        this->bSpectraPlotInitDone = true;
        this->m_CustomPlot->clearGraphs();
        this->m_CustomPlot->addGraph   ();

        this->m_CustomPlot->graph (0)->setName ("CH" + QString::number(this->uiMyChannel));

        QPen clPen = QPen (Qt::blue);
        if ((unsigned int)this->qvecColours.size() > this->uiMyChannel)
        {
            clPen.setColor(this->qvecColours[this->uiMyChannel]);
        }
        clPen.setWidth    (1);
        this->m_CustomPlot->graph (0)->setPen (clPen);

        this->m_CustomPlot->xAxis->setScaleType  (QCPAxis::stLogarithmic);
        this->m_CustomPlot->xAxis->ticker().clear();
        this->m_CustomPlot->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog> (new  QCPAxisTickerLog));
        this->m_CustomPlot->xAxis->ticker()->setTickCount(10);
        this->m_CustomPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

        this->m_CustomPlot->yAxis->setScaleType  (QCPAxis::stLogarithmic);
        this->m_CustomPlot->yAxis->ticker().clear();
        this->m_CustomPlot->yAxis->setTicker(QSharedPointer<QCPAxisTickerLog> (new  QCPAxisTickerLog));
        this->m_CustomPlot->yAxis->ticker()->setTickCount(10);
        this->m_CustomPlot->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

        this->m_CustomPlot->yAxis->setNumberPrecision(3);
        this->m_CustomPlot->yAxis->setNumberFormat   ("e");
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->yAxis->setTickLabelFont  (clFont);
        this->m_CustomPlot->axisRect()->setupFullAxesBox();
        m_CustomPlot->graph(0)->setData(this->qvecSTSpectraXAxis, this->qvecSTSpectraYAxis);
    }
    else
    {
        m_CustomPlot->graph(0)->setData(this->qvecSTSpectraXAxis, this->qvecSTSpectraYAxis);
    }

    if (this->bShowAxis == true)
    {
        this->m_CustomPlot->xAxis->setTickLabels (true);
        QFont clFont ("Courier", 10);
        this->m_CustomPlot->xAxis->setTickLabelFont (clFont);
    }
    else
    {
        this->m_CustomPlot->xAxis->setTickLabels (false);
    }

    m_CustomPlot->rescaleAxes();
    this->onCustomReplot();

    return (bRetValue);
}


void CustomPlotItem::computeFFT(QVector<double> &qvecIn, QVector<double> &qvecOutAmpl, QVector<double> &qvecOutPhase,  QVector<double> &qvecOutFreq, const double dSampleFreq, const unsigned int uiActWindowSize)
{
    // declaration of variables
    int             iCounter;
    int             iOutCounter;
    size_t          uiZeroPadding;
    double          dReal;
    double          dImg;
    double          dAmplitude;
    unsigned int    uiOutDataCounter;
    unsigned int    uiStackCount = 0;

    std::vector<double> vecIn;
    std::vector<std::complex<double>> vecOutTemp;
    std::vector<std::complex<double>> vecOut;
    std::vector<double> vecStackedSpectraAmpl;

    fftreal_vector* pFFT = new fftreal_vector ();

    uiZeroPadding = pFFT->next_power_of_two(qvecIn.size());
    vecIn.clear();
    int iSampleCount = 0;

    if (uiZeroPadding > uiActWindowSize)
    {
        vecIn.resize(uiActWindowSize);
    }
    else
    {
        vecIn.resize(uiZeroPadding);
    }

    for (iCounter = 0; iCounter < (int)uiZeroPadding; iCounter++)
    {
        if (iCounter < qvecIn.size())
        {
           vecIn [iSampleCount] = qvecIn.at(iCounter);
        }
        else
        {
            vecIn [iSampleCount] = 0.0;
        }
        iSampleCount++;
        if (iSampleCount >= (int)vecIn.size())
        {
            // compute FFT - skip DC Offset and very upper part of spectra as that contains DC Offset + falling part
            // of spectra caused by anti alisaing filters
            pFFT->set_forward_buffer(vecIn, vecOutTemp, dSampleFreq, true, false, 0.0, 0.0);
            pFFT->fwd_process       (vecIn, vecOutTemp, fftreal_ctrl::hanning);

            if (uiStackCount == 0)
            {
                vecOut = vecOutTemp;

                vecStackedSpectraAmpl.resize (vecOutTemp.size (), 0.0);
            }
            else
            {
                for (uiOutDataCounter = 0; uiOutDataCounter < vecOut.size(); uiOutDataCounter++)
                {
                    vecOut [uiOutDataCounter] += vecOutTemp [uiOutDataCounter];
                }
            }

            for (iOutCounter = 0; iOutCounter < (int)vecOutTemp.size (); iOutCounter++)
            {
                dReal      = vecOutTemp [iOutCounter].real ();
                dImg       = vecOutTemp [iOutCounter].imag ();
                dAmplitude = (double) (sqrt (pow (dReal, 2.0) + pow (dImg, 2.0)));
                vecStackedSpectraAmpl [iOutCounter] += dAmplitude;
            }

            iSampleCount = 0;
            uiStackCount++;
        }
    }

    for (iOutCounter = 0; iOutCounter < (int)vecOutTemp.size (); iOutCounter++)
    {
        vecStackedSpectraAmpl [iOutCounter] /= (double) uiStackCount;
    }

    delete (pFFT);

    for (uiOutDataCounter = 0; uiOutDataCounter < vecOut.size(); uiOutDataCounter++)
    {
        vecOut [uiOutDataCounter] /= uiStackCount;
    }

    qvecOutAmpl.clear ();
    qvecOutPhase.clear();
    qvecOutFreq.clear ();
    for (iCounter = 1; iCounter < (vecOut.size() - (vecOut.size() * 0.08)); iCounter++)
    {
        qvecOutAmpl.push_back   (vecStackedSpectraAmpl [iCounter]);
        qvecOutPhase.push_back  ((std::arg (vecOut[iCounter]) * 360.0) / (2 * M_PI));
        qvecOutFreq.push_back   ((double) (((double)iCounter) * (dSampleFreq / (uiActWindowSize))));
    }

    return;
}

bool CustomPlotItem::showNewDataTS (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, double dNewBufferStartTimeMillis, unsigned int uiNewChannel, const bool bShowXAxis)
{
    int iCounter;
    this->qvecDataTS.resize(qvecNewData.size());
    for (iCounter = 0; iCounter < qvecNewData.size(); iCounter++)
    {
        this->qvecDataTS [iCounter] = qvecNewData.at(iCounter);
    }
    this->dSampleFreq            = dNewSampleFreq;
    this->sBufferStartTime       = sNewBufferStartTime;
    this->dBufferStartTimeMillis = dNewBufferStartTimeMillis;
    this->uiChannel              = uiNewChannel;
    this->ePlotType              = PlotTypeTS;
    this->bShowAxis              = bShowXAxis;

    this->m_CustomPlot->yAxis->setLabelPadding(250);

    this->updateGuiTS();

    return (true);
}

bool CustomPlotItem::showNewDataSpectra (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, unsigned int uiNewChannel, const bool bShowXAxis)
{
    this->qvecDataTS        = qvecNewData;
    this->dSampleFreq       = dNewSampleFreq;
    this->sBufferStartTime  = sNewBufferStartTime;
    this->uiChannel         = uiNewChannel;
    this->ePlotType         = PlotTypeSpectra;
    this->bShowAxis         = bShowXAxis;

    this->updateGuiSpectra();

    return (true);
}


bool CustomPlotItem::showNewDataSTSpectra (QVector<double>& qvecNewData, double dNewSampleFreq, time_t sNewBufferStartTime, unsigned int uiNewChannel, const bool bShowXAxis, const eStackedSpectraMode_t eSTBufferMode)
{
    bool                    bRetValue = true;
    unsigned int            uiCounter;
    QVector<double>         qvecYAxis;
    QVector<double>         qvecXAxis;
    QVector<double>         qvecTemp;

    this->dSampleFreq       = dNewSampleFreq;
    this->sBufferStartTime  = sNewBufferStartTime;
    this->uiChannel         = uiNewChannel;
    this->ePlotType         = PlotTypeSpectra;
    this->bShowAxis         = bShowXAxis;

    // first step, compute Spectra from actual TS data.
    this->computeFFT(qvecNewData, qvecYAxis, qvecTemp, qvecXAxis, this->dSampleFreq, this->uiWindowSize);

    qvecXAxis.erase(qvecXAxis.begin());
    qvecYAxis.erase(qvecYAxis.begin());

    // now check buffer state
    switch (eSTBufferMode)
    {
        case STSpectraFirstBuffer:
            {
                this->uiSTSpectraNumStacks = 1;
                this->qvecSTSpectraXAxis   = qvecXAxis;
                this->qvecSTSpectraYAxis   = qvecYAxis;
                break;
            }

        case STSpectraContBuffer:
            {
                this->uiSTSpectraNumStacks++;
                for (uiCounter = 0; uiCounter < (unsigned int) this->qvecSTSpectraYAxis.size(); uiCounter++)
                {
                    this->qvecSTSpectraYAxis [uiCounter] += qvecYAxis [uiCounter];
                }
                break;
            }

        case STSpectraLastBuffer:
            {
                this->uiSTSpectraNumStacks++;
                for (uiCounter = 0; uiCounter < (unsigned int) this->qvecSTSpectraYAxis.size(); uiCounter++)
                {
                    this->qvecSTSpectraYAxis [uiCounter] = (this->qvecSTSpectraYAxis [uiCounter] + qvecYAxis [uiCounter]) / ((double) this->uiSTSpectraNumStacks);
                }
                this->updateGuiSTSpectra();
                break;
            }

        default:
            {
                bRetValue = false;
                break;
            }
    }

    return (bRetValue);
}


bool CustomPlotItem::hidePlot (void)
{
    if (this->isVisible() == true)
    {
        this->setVisible(false);
        this->onCustomReplot();
    }

    return (true);
}


bool CustomPlotItem::showPlot (const bool bShowXAxis)
{
    this->bShowAxis = bShowXAxis;
    this->showTickLabels();
    this->setVisible(true);
    this->onCustomReplot();

    return (true);
}


void CustomPlotItem::showTickLabels (void)
{
    if (this->bShowAxis == true)
    {
        if (this->ePlotType == PlotTypeTS)
        {
            this->m_CustomPlot->xAxis->setTickLabels (true);
        }
        else
        {
            this->m_CustomPlot->xAxis->setTickLabels (true);
        }
    }
    else
    {
        this->m_CustomPlot->xAxis->setTickLabels (false);
    }
}


void CustomPlotItem::slotNewWindowSize (unsigned int uiNewWindowSize)
{
    this->uiWindowSize = uiNewWindowSize;

    if (this->ePlotType == PlotTypeTS)
    {
        this->updateGuiTS();
    }
    else
    {
        this->updateGuiSpectra ();
    }
}


void CustomPlotItem::slotGetValue (const unsigned int uiBufferIndex, double& dXValue, double& dYValue)
{
    QVector<double> qvecXData;
    QVector<double> qvecYData;

    if (this->m_CustomPlot->graphCount() > 0)
    {
        if ((int)uiBufferIndex < (this->m_CustomPlot->graph(0)->dataCount() - 1))
        {
            dXValue = this->m_CustomPlot->graph(0)->dataMainKey  (uiBufferIndex);
            dYValue = this->m_CustomPlot->graph(0)->dataMainValue(uiBufferIndex);

            qvecXData.push_back(dXValue);
            qvecYData.push_back(dYValue);

            if (this->ePlotType == PlotTypeSpectra)
            {
                dXValue = this->m_CustomPlot->graph(0)->dataMainKey  (uiBufferIndex + 1);
            }

            if (this->m_CustomPlot->graphCount() < 2)
            {
                QPen clPen = QPen (Qt::black);
                clPen.setWidth(1);
                this->m_CustomPlot->addGraph();
                this->m_CustomPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssCrossSquare);
                this->m_CustomPlot->graph(1)->setVisible(true);
                this->m_CustomPlot->graph(1)->setPen(clPen);
            }

            this->m_CustomPlot->graph(1)->setData(qvecXData, qvecYData);

        }

        this->onCustomReplot();
    }
    else
    {
        dXValue = 0;
        dYValue = 0;
    }
}


unsigned int CustomPlotItem::slotGetValueMaxIndex (const unsigned int uiStartIndex)
{
    double          dTemp;
    double          dMinFreq;
    double          dMaxFreq;
    double          dActFreq;
    double          dValueAtStartIndex;
    unsigned int    uiMaxSearchWindowSize;
    unsigned int    uiMaxIndex;
    unsigned int    uiCounter;

    uiMaxIndex = uiStartIndex;

    if (this->m_CustomPlot->graphCount() > 0)
    {
        if ((int)uiStartIndex < (this->m_CustomPlot->graph(0)->dataCount() - 1))
        {
            // in spectra plot mode search for maxima near by
            if (this->ePlotType == PlotTypeSpectra)
            {
                dValueAtStartIndex = this->m_CustomPlot->graph(0)->dataMainValue(uiStartIndex);

                dMinFreq = this->m_CustomPlot->graph(0)->dataMainKey (0);
                dMaxFreq = this->m_CustomPlot->graph(0)->dataMainKey (this->m_CustomPlot->graph(0)->dataCount() - 1);
                dActFreq = this->m_CustomPlot->graph(0)->dataMainKey (uiStartIndex);
                dTemp    = (this->m_CustomPlot->graph(0)->dataCount() * 0.06) * (dActFreq / (dMaxFreq - dMinFreq));

                uiMaxSearchWindowSize = (unsigned int) dTemp;

                qDebug () << "search window size:" << uiMaxSearchWindowSize << "samples";

                if ((int)(uiStartIndex + uiMaxSearchWindowSize) < (this->m_CustomPlot->graph(0)->dataCount() - 1))
                {
                    for (uiCounter = 0; uiCounter < uiMaxSearchWindowSize; uiCounter++)
                    {
                        if ((this->m_CustomPlot->graph(0)->dataMainValue(uiStartIndex + uiCounter) > (dValueAtStartIndex * 6)))
                        {
                            uiMaxIndex = uiStartIndex + uiCounter;
                        }
                    }
                }

                int iTemp = uiStartIndex;
                iTemp -= uiMaxSearchWindowSize;
                if (iTemp >= 0)
                {
                    for (uiCounter = 0; uiCounter < uiMaxSearchWindowSize; uiCounter++)
                    {
                        if ((this->m_CustomPlot->graph(0)->dataMainValue(uiStartIndex - uiCounter) > (dValueAtStartIndex * 6)))
                        {
                            uiMaxIndex = uiStartIndex - uiCounter;
                        }
                    }
                }
            }
        }
    }

    return (uiMaxIndex);
}
