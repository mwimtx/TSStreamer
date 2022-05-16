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

#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QFile>
#include <QTcpSocket>

#include "tsstreamer.h"
#include "globaldata.h"
#include "systeminfo.h"

#define C_TSSTREAMER_DEBUG_OUTPUT 1

TSStreamer::TSStreamer(const QString qstrTargetIP) :
    pclNetworkreply(Q_NULLPTR)
{
    this->qlSystemInfo.clear();
    this->qlMeasInfo.clear();

    qstrTargetIPAddress.clear();

    uiWindowSize = 512;

    this->bFirstTimeMeasSelected = true;

    this->atomHttpAccessActive.store(false);

    this->atomCancelShowSTSpectra.store(false);

    this->bConnecting = false;

    this->uiLastVisiblePlot = 4;

    if (qstrTargetIP.size() > 0)
    {
        this->qstrDirectTargetIP = qstrTargetIP;
        this->bConnectDirectTargetIP = true;
        emit (this->sigDirectTargetIPUpdated());
    }
}


void TSStreamer::onRequestFinished (void)
{
    this->qbaRequestResponse  = this->pclNetworkreply->readAll();
    this->qstrRequestResponse = QString (this->qbaRequestResponse);

    if (this->pclNetworkreply->error() != QNetworkReply::NetworkError::NoError)
    {
        #ifdef C_TSSTREAMER_DEBUG_OUTPUT
            qDebug () << __PRETTY_FUNCTION__ << "request failed!";
            qDebug () << this->pclNetworkreply->errorString();
        #endif
    }

    emit (this->sigRequestFinished());
}


void TSStreamer::onRangeRequestFinished (void)
{
    if (this->pclNetworkreply->error() != QNetworkReply::NetworkError::NoError)
    {
        #ifdef C_TSSTREAMER_DEBUG_OUTPUT
            qDebug () << __PRETTY_FUNCTION__ << "request failed!";
            qDebug () << this->pclNetworkreply->errorString();
        #endif
        this->bRangeRequestError = true;
    }

    emit (this->sigRequestFinished());
}


void TSStreamer::onRangeRequestReadyRead (void)
{
    this->qbaRequestResponse.append  (this->pclNetworkreply->readAll());
    this->qstrRequestResponse.append (QString (this->qbaRequestResponse));
}


QString TSStreamer::execHttpRequest (const QString qstrRequest, const bool bUseSSL)
{
    QEventLoop      clLoop;
    QNetworkRequest clRequest;

    this->qbaRequestResponse.clear();
    this->qstrRequestResponse.clear();

    clRequest.setUrl    (QUrl(qstrRequest));
    clRequest.setHeader (QNetworkRequest::ContentTypeHeader, "text");

    if (bUseSSL == true)
    {
        QSslConfiguration config    = clRequest.sslConfiguration();

        config.setCaCertificates(this->clHostCerts);
        if (this->clHostCerts.size() > 0)
        {
            config.setLocalCertificate  (this->clHostCerts.at(0));
            config.setPrivateKey        (this->clHostCerts.at(0).publicKey());
            config.setProtocol          (QSsl::AnyProtocol);
            config.setPeerVerifyMode    (QSslSocket::VerifyNone);
            clRequest.setSslConfiguration(config);
        }
        else
        {
            qDebug () << "no valid host certs found -> communication broken!";
        }

        QUrl clUrl = clRequest.url();
        clUrl.setUserName   (C_WEBSERVICE_USER);
        clUrl.setPassword   (C_WEBSERVICE_PASS);
        clRequest.setUrl    (clUrl);
    }

    this->pclNetworkreply = this->clNam.get(clRequest);

    clLoop.connect (this->pclNetworkreply, SIGNAL (finished()),           this,    SLOT(onRequestFinished()), Qt::DirectConnection);
    clLoop.connect (this,                  SIGNAL (sigRequestFinished()), &clLoop, SLOT(quit ()),             Qt::DirectConnection);

    clLoop.exec();

    clLoop.disconnect (this->pclNetworkreply, SIGNAL (finished()),           this,    SLOT(onRequestFinished()));
    clLoop.disconnect (this,                  SIGNAL (sigRequestFinished()), &clLoop, SLOT(quit ()));

    this->pclNetworkreply->deleteLater();
    this->pclNetworkreply = Q_NULLPTR;

    return (this->qstrRequestResponse);
}


void TSStreamer::execHttpRangeRequest (const QString qstrRequest, const unsigned int uiStartPos, const unsigned int uiNumBytes, const bool bUseSSL)
{
    QEventLoop*      pclLoop    = new QEventLoop;
    QNetworkRequest* pclRequest = new QNetworkRequest ();
    QString          qstrTemp;

    this->qbaRequestResponse.clear();
    this->qstrRequestResponse.clear();

    this->clUrl = QUrl::fromUserInput(qstrRequest);
    pclRequest->setUrl (this->clUrl);

    //qDebug () << __PRETTY_FUNCTION__ << "request:" << qstrRequest;

    qstrTemp = "bytes=" + QString::number(uiStartPos) + "-" +QString::number(uiStartPos + uiNumBytes);
    pclRequest->setRawHeader (QByteArray("Range"),      QByteArray(qstrTemp.toStdString().c_str()));
    pclRequest->setRawHeader (QByteArray("Keep-Alive"), QByteArray("10"));
    pclRequest->setRawHeader (QByteArray("timeout"),    QByteArray("5"));

    if (bUseSSL == true)
    {
        QSslConfiguration config    = pclRequest->sslConfiguration();

        config.setCaCertificates(this->clHostCerts);
        if (this->clHostCerts.size() > 0)
        {
            config.setLocalCertificate  (this->clHostCerts.at(0));
            config.setPrivateKey        (this->clHostCerts.at(0).publicKey());
            config.setProtocol          (QSsl::AnyProtocol);
            config.setPeerVerifyMode    (QSslSocket::VerifyNone);
            pclRequest->setSslConfiguration(config);
        }
        else
        {
            qDebug () << "no valid host certs found -> communication broken!";
        }

        QUrl clUrl = pclRequest->url();
        clUrl.setUserName   (C_WEBSERVICE_USER);
        clUrl.setPassword   (C_WEBSERVICE_PASS);
        pclRequest->setUrl  (clUrl);
    }

    this->pclNetworkreply = this->clNam.get(*pclRequest);

    pclLoop->connect (this->pclNetworkreply, &QNetworkReply::finished,      this, &TSStreamer::onRangeRequestFinished);
    pclLoop->connect (this->pclNetworkreply, &QIODevice::readyRead,         this, &TSStreamer::onRangeRequestReadyRead);
    pclLoop->connect (this,                  SIGNAL (sigRequestFinished()), pclLoop, SLOT(quit ()));

    pclLoop->exec();

    try
    {
        pclLoop->disconnect (this->pclNetworkreply, &QNetworkReply::finished,      this, &TSStreamer::onRangeRequestFinished);
        pclLoop->disconnect (this->pclNetworkreply, &QIODevice::readyRead,         this, &TSStreamer::onRangeRequestReadyRead);
        pclLoop->disconnect (this,                  SIGNAL (sigRequestFinished()), pclLoop, SLOT(quit ()));
    }
    catch (...)
    {
        // do nothing - next request will fix this
        qDebug () << "http request failed!";
    }

    delete (pclLoop);
    delete (pclRequest);
    delete (this->pclNetworkreply);
}


bool TSStreamer::readMeasInfoFromTarget (const QString qstrTargetIP)
{
    QString      qstrTemp;
    QString      qstrTemp2;
    int          iIndex;
    int          iCounter;
    ATSHeader*   pclHeader;
    QList<QObject*> qlMeasInfoTemp;

    qDeleteAll (this->qlMeasInfo);
    this->qlMeasInfo.clear();
    this->qstrTargetIPAddress = qstrTargetIP;

    // read meas_ directories to get top directories of all measurements on ADU
    if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
    {
        qstrTemp = "https://" + qstrTargetIP + "/data/";
        qstrTemp = this->execHttpRequest(qstrTemp, true);
    }
    else
    {
        qstrTemp = "http://" + qstrTargetIP + "/data/";
        qstrTemp = this->execHttpRequest(qstrTemp, false);
    }

    do
    {
        iIndex = (qstrTemp.indexOf("=\"meas_"));

        if (iIndex >= 0)
        {
            qstrTemp = qstrTemp.right(qstrTemp.size() - iIndex - 2);

            qlMeasInfoTemp.push_back((QObject*) new MeasInfo (qlMeasInfoTemp.size()));
            ((MeasInfo*)qlMeasInfoTemp.back())->qstrRootPath = qstrTemp.left(qstrTemp.indexOf("/"));
        }
    } while (iIndex >= 0);

    // now for all measurements search for ATS files and read meas-doc XML
    for (iCounter = 0; iCounter < qlMeasInfoTemp.size(); iCounter++)
    {
        if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
        {
            qstrTemp = "https://" + qstrTargetIP + "/data/" + ((MeasInfo*)qlMeasInfoTemp.at(iCounter))->qstrRootPath + "/";
            qstrTemp = this->execHttpRequest(qstrTemp, true);
        }
        else
        {
            qstrTemp = "http://" + qstrTargetIP + "/data/" + ((MeasInfo*)qlMeasInfoTemp.at(iCounter))->qstrRootPath + "/";
            qstrTemp = this->execHttpRequest(qstrTemp, false);
        }

        // meas-doc XML file name
        iIndex = (qstrTemp.indexOf(".xml"));
        if (iIndex >= 0)
        {
            qstrTemp2 = qstrTemp.left(iIndex + 4);
            qstrTemp2 = qstrTemp2.right(qstrTemp2.size() - qstrTemp2.lastIndexOf('"') - 1);
            ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrMeasDocXML = qstrTemp2;
        }

        if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
        {
            qstrTemp2 = "https://" + qstrTargetIP + "/data/" +  ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrRootPath + "/" +  ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrMeasDocXML;
            qstrTemp2 = this->execHttpRequest(qstrTemp2, true);
        }
        else
        {
            qstrTemp2 = "http://" + qstrTargetIP + "/data/" +  ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrRootPath + "/" +  ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrMeasDocXML;
            qstrTemp2 = this->execHttpRequest(qstrTemp2, false);
        }

        ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrMeasDocXML = qstrTemp2;


        qDebug () << qstrTemp;


        do
        {
            iIndex = (qstrTemp.indexOf(".ats"));
            if (iIndex >= 0)
            {
                qstrTemp2 = qstrTemp.left(iIndex + 4);
                qstrTemp2 = qstrTemp2.right(qstrTemp2.size() - qstrTemp2.lastIndexOf('"') - 1);

                if (qstrTemp2.at(0) != '>')
                {
                    ((MeasInfo*)qlMeasInfoTemp [iCounter])->qvecATSFiles.push_back(qstrTemp2);

                    // read ATSHeader from file
                    if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
                    {
                        qstrTemp2 = "https://" + qstrTargetIP + "/data/" + ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrRootPath + "/" + ((MeasInfo*)qlMeasInfoTemp [iCounter])->qvecATSFiles.back();
                        this->execHttpRangeRequest(qstrTemp2, 0, sizeof(ATSHeader), true);
                    }
                    else
                    {
                        qstrTemp2 = "http://" + qstrTargetIP + "/data/" + ((MeasInfo*)qlMeasInfoTemp [iCounter])->qstrRootPath + "/" + ((MeasInfo*)qlMeasInfoTemp [iCounter])->qvecATSFiles.back();
                        this->execHttpRangeRequest(qstrTemp2, 0, sizeof(ATSHeader), false);
                    }

                    pclHeader = (ATSHeader*)this->qbaRequestResponse.data();
                    ((MeasInfo*)qlMeasInfoTemp [iCounter])->qvecATSHeaders.push_back(*pclHeader);
                    pclHeader = nullptr;
                }
                qstrTemp = qstrTemp.right(qstrTemp.size() - iIndex - 4);
            }
        } while (iIndex >= 0);
    }

    for (iCounter = 0; iCounter < qlMeasInfoTemp.size(); iCounter++)
    {
        if (((MeasInfo*)qlMeasInfoTemp.at(iCounter))->qvecATSFiles.size() > 0)
        {
            if (this->qlMeasInfo.size() <= 0)
            {
                this->qlMeasInfo.push_back((MeasInfo*)qlMeasInfoTemp.at(iCounter));
            }
            else
            {
                for (int iCounter2 = 0; iCounter2 < this->qlMeasInfo.size(); iCounter2++)
                {
                    unsigned int uiStartDateNewEntry  = ((MeasInfo*)qlMeasInfoTemp.at(iCounter))->getStartDate();
                    unsigned int uiStartDateLastEntry = ((MeasInfo*)this->qlMeasInfo.at(iCounter2))->getStartDate();

                    if (uiStartDateNewEntry > uiStartDateLastEntry)
                    {
                        if (iCounter2 == 0)
                        {
                            this->qlMeasInfo.insert(iCounter2 - 1, (MeasInfo*)qlMeasInfoTemp.at(iCounter));
                        }
                        else
                        {
                            this->qlMeasInfo.insert(0, (MeasInfo*)qlMeasInfoTemp.at(iCounter));
                        }
                        break;
                    }
                }
            }
        }
    }

    for (iCounter = 0; iCounter < this->qlMeasInfo.size(); iCounter++)
    {
        ((MeasInfo*)this->qlMeasInfo [iCounter])->iMeasID = iCounter;
    }

    emit (this->sigMeasListUpdated());

    return (true);
}


bool TSStreamer::readMeasInfoFromTarget (void)
{
    return (this->readMeasInfoFromTarget(this->qstrTargetIPAddress));
}


bool TSStreamer::readData (const QString qstrTargetIP, const unsigned int uiMeasIndex, const unsigned int uiStartPos, const unsigned int uiNumBytes, const bool bShowSTSpectra, const eStackedSpectraMode_t eSTSpectraMode)
{
    bool        bRetValue = true;
    QString     qstrTemp;
    int         iCounter;
    int         iChCounter;
    int         iTemp;
    int         iNumSamples;
    double      dLSB;
    double      dOffset;
    QVector<double> qvecTSData;

    if (uiMeasIndex >= (unsigned int)this->qlMeasInfo.size())
    {
        #ifdef C_TSSTREAMER_DEBUG_OUTPUT
            qDebug () << __PRETTY_FUNCTION__ << "not a valid measurement index:" << uiMeasIndex;
        #endif
        bRetValue = false;
    }
    else
    {
        qvecTSData.resize(uiNumBytes / 4);

        this->bRangeRequestError = false;

        for (iChCounter = 0; iChCounter < ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles.size(); iChCounter++)
        {
            unsigned int uiChNum           = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).byChanNo;
            unsigned int uiBufferStartTime = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).iStartDateTime;

            dOffset  = ((uiStartPos - sizeof (ATSHeader))  / 4);
            dOffset /= ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq;
            uiBufferStartTime += (unsigned int) (floor (dOffset));

            if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
            {
                qstrTemp = "https://" +qstrTargetIP + "/data/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qstrRootPath + "/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles [iChCounter];
                this->qbaRequestResponse.clear();
                while (this->qbaRequestResponse.size() == 0)
                {
                    this->execHttpRangeRequest(qstrTemp, uiStartPos, uiNumBytes + 1, true);
                }
            }
            else
            {
                qstrTemp = "http://" +qstrTargetIP + "/data/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qstrRootPath + "/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles [iChCounter];
                this->qbaRequestResponse.clear();
                while (this->qbaRequestResponse.size() == 0)
                {
                    this->execHttpRangeRequest(qstrTemp, uiStartPos, uiNumBytes + 1, false);
                }
            }

            if (this->bRangeRequestError == true)
            {
                bRetValue = false;
                break;
            }
            else
            {

                dLSB        = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).dblLSBMV;
                iNumSamples = (this->qbaRequestResponse.size() / 4) - 1;
                if (qvecTSData.size() < iNumSamples)
                {
                    iNumSamples = qvecTSData.size();
                }
                iNumSamples = iNumSamples * 4;
                for (iCounter = 0; iCounter < iNumSamples; iCounter = iCounter + 4)
                {
                    iTemp =  ((unsigned char)this->qbaRequestResponse.at(iCounter + 3));
                    iTemp = iTemp << 8;
                    iTemp = iTemp | ((unsigned char)this->qbaRequestResponse.at(iCounter + 2));
                    iTemp = iTemp << 8;
                    iTemp = iTemp | ((unsigned char)this->qbaRequestResponse.at(iCounter + 1));
                    iTemp = iTemp << 8;
                    iTemp = iTemp | ((unsigned char)this->qbaRequestResponse.at(iCounter + 0));

                    qvecTSData[iCounter / 4] = (iTemp * dLSB);
                }

                unsigned int uiFullSecs =  ((this->uiTSActPosition - sizeof (ATSHeader)) / 4) / ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq;
                double       dMillis    = (((this->uiTSActPosition - sizeof (ATSHeader)) / 4) - ((uiFullSecs * ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq)));
                             dMillis    = dMillis / ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq;

                // for ADU-07e - map HF channels to LF channels
                if (uiChNum >= 5)
                {
                    uiChNum = uiChNum - 5;
                }
                if (bShowSTSpectra == false)
                {
                    //if (iChCounter < (((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles.size() - 1))
                    if ((unsigned int)iChCounter != this->uiLastVisiblePlot)
                    {
                        pclGlobalData->vecPlots[0][uiChNum]->showNewDataTS (qvecTSData,
                            ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                            uiBufferStartTime,
                            dMillis,
                            uiChNum, false);

                        pclGlobalData->vecPlots[1][uiChNum]->showNewDataSpectra(qvecTSData,
                            ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                            uiBufferStartTime,
                            uiChNum, false);
                    }
                    else
                    {
                        pclGlobalData->vecPlots[0][uiChNum]->showNewDataTS (qvecTSData,
                            ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                            uiBufferStartTime, dMillis, uiChNum, true);

                        pclGlobalData->vecPlots[1][uiChNum]->showNewDataSpectra (qvecTSData,
                            ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                            uiBufferStartTime, uiChNum, true);
                    }
                }
                else
                {
                    if (iChCounter < (((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles.size() - 1))
                    {
                        pclGlobalData->vecPlots[1][uiChNum]->showNewDataSTSpectra (qvecTSData, ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                                uiBufferStartTime, uiChNum, false, eSTSpectraMode);
                    }
                    else
                    {
                        pclGlobalData->vecPlots[1][uiChNum]->showNewDataSTSpectra (qvecTSData, ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.at(iChCounter).rSampleFreq,
                                uiBufferStartTime, uiChNum, true, eSTSpectraMode);
                    }
                }
            }
        }
    }

    return (bRetValue);
}


void TSStreamer::showNextTSBuffer (void)
{
    if (this->atomHttpAccessActive.load() == false)
    {
        this->atomHttpAccessActive.store(true);

        unsigned int uiNumSamples        = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders [0].iSamples;
        unsigned int uiLastDisplaySample = (((this->uiTSActPosition - sizeof(ATSHeader)) / 4) + this->uiWindowSize + this->uiWindowSize);

        // first check, if we are at the end of the file
        if (uiLastDisplaySample <= uiNumSamples)
        {
            this->uiTSActPosition += (this->uiWindowSize * 4) / 4;
            this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, this->uiTSActPosition, this->uiWindowSize * 4, false, STSpectraContBuffer);
        }

        emit (this->sigActSamplePosUpdated());

        this->atomHttpAccessActive.store(false);
    }
}


void TSStreamer::showPrevTSBuffer (void)
{
    if (this->atomHttpAccessActive.load() == false)
    {
        this->atomHttpAccessActive.store(true);


        int iTemp;

        iTemp  = this->uiTSActPosition;
        iTemp -= (this->uiWindowSize * 4) / 4;
        if ((iTemp < (int)sizeof (ATSHeader)) || (iTemp < 0))
        {
            iTemp = sizeof (ATSHeader);
        }
        this->uiTSActPosition = iTemp;
        this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, this->uiTSActPosition, this->uiWindowSize * 4, false, STSpectraContBuffer);

        emit (this->sigActSamplePosUpdated());

        this->atomHttpAccessActive.store(false);
    }
}


void TSStreamer::showTSBufferAtPos (unsigned int uiPos)
{
    if (this->atomHttpAccessActive.load() == false)
    {
        this->atomHttpAccessActive.store(true);


        if (this->qlMeasInfo.size() > 0)
        {
            // alsways fall back to a quarter of window size
            uiPos = uiPos / (this->uiWindowSize / 4);
            uiPos = uiPos * (this->uiWindowSize / 4);

            unsigned int uiNumSamples        = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders [0].iSamples;
            unsigned int uiLastDisplaySample = uiPos + this->uiWindowSize;

            // first check, if we are at the end of the file
            if (uiLastDisplaySample <= uiNumSamples)
            {
                this->uiTSActPosition = (uiPos * 4) + sizeof (ATSHeader);
                this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, this->uiTSActPosition, this->uiWindowSize * 4, false, STSpectraContBuffer);
            }

            emit (this->sigActSamplePosUpdated());
        }

        this->atomHttpAccessActive.store(false);
    }
}


void TSStreamer::showTSBufferScopeMode (void)
{
    if (this->atomHttpAccessActive.load() == false)
    {
        this->atomHttpAccessActive.store(true);

        int          iATSHeaderCounter;
        unsigned int uiPos;
        QString      qstrTemp2;
        ATSHeader*   pclHeader;
        int          iActMaxNumSamples;

        if (this->qlMeasInfo.size() >= this->uiActiveMeasID)
        {
            try
            {
                iActMaxNumSamples = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders[0].iSamples;
            }
            catch (...)
            {
                iActMaxNumSamples = -1;
            }
        }
        else
        {
            iActMaxNumSamples = -1;
        }

        if (iActMaxNumSamples <= 0)
        {
            qDebug () << "show buffer scope -> INVALID WINDOW SIZE OR NO MEAS SELECTED!";
        }
        else
        {
            qDebug () << "show buffer scope mode triggered ...";

            // first step: update ATS headers
            for (iATSHeaderCounter = 0; iATSHeaderCounter < ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles.size(); iATSHeaderCounter++)
            {
                // read ATSHeader from file
                if (((SystemInfo*) this->qlSystemInfo.at(this->uiActiveSystemID))->bUseHTTPS == true)
                {
                    qstrTemp2 = "https://" + this->qstrTargetIPAddress + "/data/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qstrRootPath + "/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles[iATSHeaderCounter];
                    this->execHttpRangeRequest(qstrTemp2, 0, sizeof(ATSHeader), true);
                }
                else
                {
                    qstrTemp2 = "http://" + this->qstrTargetIPAddress + "/data/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qstrRootPath + "/" + ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSFiles[iATSHeaderCounter];
                    this->execHttpRangeRequest(qstrTemp2, 0, sizeof(ATSHeader), false);
                }

                pclHeader = (ATSHeader*)this->qbaRequestResponse.data();
                ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders[iATSHeaderCounter] = *pclHeader;
                pclHeader = nullptr;
            }

            uiPos = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders[0].iSamples;
            qDebug () << "sample pos:" << uiPos;
            uiPos = uiPos - this->uiWindowSize;
            qDebug () << "sample pos:" << uiPos;

            this->atomHttpAccessActive.store(false);

            if ((uiPos < ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders[0].iSamples) &&
                (uiPos > 0))
            {
                this->showTSBufferAtPos(uiPos);
            }
        }
    }
}


void TSStreamer::showSTSpectra (void)
{
    int          iCounter;
    unsigned int uiActPos;
    unsigned int uiActSamplePos;
    unsigned int uiMaxNumSamples;
    MeasInfo*    pclActMeas;
    eStackedSpectraMode_t eSTSpectraMode;

    this->atomCancelShowSTSpectra.store(false);

    if (this->atomHttpAccessActive.load() == false)
    {
        this->atomHttpAccessActive.store(true);

        if (this->uiActiveMeasID < (unsigned int)this->qlMeasInfo.size())
        {
            pclActMeas = (MeasInfo*) this->qlMeasInfo.at(this->uiActiveMeasID);

            // get smallest ATS file size from all ATS files
            uiMaxNumSamples = 0xFFFFFFFF;
            for (iCounter = 0; iCounter < pclActMeas->qvecATSHeaders.size(); iCounter++)
            {
                if ((unsigned int)pclActMeas->qvecATSHeaders.at(iCounter).iSamples < uiMaxNumSamples)
                {
                    uiMaxNumSamples = pclActMeas->qvecATSHeaders.at(iCounter).iSamples;
                }
            }
            uiMaxNumSamples -= this->uiWindowSize;

            uiActSamplePos = 0;
            uiActPos       = sizeof (ATSHeader);
            while (uiActSamplePos < uiMaxNumSamples)
            {
                if (uiActSamplePos == 0)
                {
                    eSTSpectraMode = STSpectraFirstBuffer;
                }
                else
                {
                    if ((uiActSamplePos + this->uiWindowSize) >= uiMaxNumSamples)
                    {
                        eSTSpectraMode = STSpectraLastBuffer;
                    }
                    else
                    {
                        eSTSpectraMode = STSpectraContBuffer;
                    }
                }

                if (this->atomCancelShowSTSpectra.load() == true)
                {
                    this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, uiActPos, this->uiWindowSize * 4, true, STSpectraLastBuffer);
                    break;
                }
                else
                {
                    this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, uiActPos, this->uiWindowSize * 4, true, eSTSpectraMode);
                }

                uiActSamplePos += this->uiWindowSize;
                uiActPos       += this->uiWindowSize * 4;

                emit (this->sigShowProgress("Computing Stacked Spectra", ((double)uiActSamplePos / (double)uiMaxNumSamples)));
            }
        }

        emit (this->sigShowSTSpectraFinished());

        pclActMeas = NULL;

        this->atomHttpAccessActive.store(false);
    }
}


bool TSStreamer::scanForSystems (const QString qstrTargetIP)
{
    unsigned int uiIPCounter     = 0;
    unsigned int uiIPSubIndex    = 1;
    unsigned int uiSubCountMax   = 254;
    unsigned int uiSubCountStart = 1;
    double       dMaxIPAddresses;
    double       dIPAddressCount;
    QString      qstrIP;
    QString      qstrTemp = "Scanning For ADU Systems ...";
    QEventLoop   clLoop;
    QTimer       clTimer;
    QString      qstrTemp2;

    QList<QString> qlLocalIPAddresses;

    // scan for IP addresses of this host to avoid scanning address ranges that cannot be reached at all
    qlLocalIPAddresses.clear();
    if (qstrTargetIP.size() == 0)
    {
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
        {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            {
                 if (
                     (address.toString().contains("192.168.0")  == true) ||
                     (address.toString().contains("192.168.1")  == true) ||
                     (address.toString().contains("192.168.2")  == true) ||
                     (address.toString().contains("192.168.10") == true) ||
                     (address.toString().contains("192.168.11") == true) ||
                     (address.toString().contains("192.168.12") == true) ||
                     (address.toString().contains("192.168.20") == true) ||
                     (address.toString().contains("192.168.30") == true) ||
                     (address.toString().contains("192.168.40") == true) ||
                     (address.toString().contains("192.168.41") == true) ||
                     (address.toString().contains("192.168.50") == true) ||
                     (address.toString().contains("192.168.51") == true) ||
                     (address.toString().contains("192.168.60") == true) ||
                     (address.toString().contains("192.168.70") == true))
                 {
                     qstrIP = address.toString().left(address.toString().lastIndexOf("."));
                     if (qlLocalIPAddresses.contains(qstrIP) == false)
                     {
                        qlLocalIPAddresses.append(qstrIP);
                     }
                 }
            }
        }
    }
    else
    {
        qstrTemp2 = qstrTargetIP.right(qstrTargetIP.size() - (qstrTargetIP.lastIndexOf(".") + 1));
        uiSubCountStart = qstrTemp2.toUInt() - 1;
        if ((uiSubCountStart < 1) || (uiSubCountStart > 254))
        {
            uiSubCountStart = 1;
        }
        uiSubCountMax = qstrTemp2.toUInt() + 1;
        qlLocalIPAddresses.append(qstrTargetIP.left(qstrTargetIP.lastIndexOf(".")));
    }

    dMaxIPAddresses = qlLocalIPAddresses.size() * 253;
    dIPAddressCount = 0.0;

    qDebug () << "IP addresses to be scanned:" << qlLocalIPAddresses;

    clTimer.setInterval   (2);
    clTimer.setSingleShot (true);
    clLoop.connect        (&clTimer, SIGNAL(timeout()), &clLoop, SLOT(quit()));

    qDeleteAll (this->qlSystemInfo);
    this->qlSystemInfo.clear();

    // scan IP addresses to find ADU system
    for (uiIPCounter = 0; uiIPCounter < (unsigned int)qlLocalIPAddresses.size(); uiIPCounter++)
    {
        for (uiIPSubIndex = uiSubCountStart; uiIPSubIndex < uiSubCountMax; uiIPSubIndex++)
        {
            qstrIP = qlLocalIPAddresses.at(uiIPCounter) + "." + QString::number(uiIPSubIndex);

            if (qlLocalIPAddresses.at(uiIPCounter).contains(qstrIP) == false)
            {
                SystemInfo clSystemInfo;
                if (this->tryConnectSystem(qstrIP, clSystemInfo) == true)
                {
                    this->qlSystemInfo.push_back((QObject*)new SystemInfo ());
                    *((SystemInfo*)(this->qlSystemInfo.back())) = clSystemInfo;
                    ((SystemInfo*)(this->qlSystemInfo.back()))->uiIndex = this->qlSystemInfo.size() - 1;

                    qstrTemp = "Found ADU at: " + qstrIP;
                }
            }

            // only to have time for GUI to update
            clTimer.start ();
            clLoop.exec   ();

            dIPAddressCount += 1.0;

            emit (this->sigShowProgress(qstrTemp, dIPAddressCount / dMaxIPAddresses));
        }
    }

    emit (this->sigSystemListUpdated());

    return (true);
}


QVariant TSStreamer::getSystemList (void)
{
    return (QVariant::fromValue(this->qlSystemInfo));
}


QVariant TSStreamer::getMeasList (void)
{
    return (QVariant::fromValue(this->qlMeasInfo));
}


bool TSStreamer::selectMeas (const unsigned int uiMeasID)
{
    bool         bRetValue = true;
    unsigned int uiPlotCounter;
    unsigned int uiChCounter;
    unsigned int uiChNumberCorrected;

    if (this->bFirstTimeMeasSelected == true)
    {
        this->bFirstTimeMeasSelected = false;
        for (uiPlotCounter = 0; uiPlotCounter < pclGlobalData->vecPlots [0].size(); uiPlotCounter++)
        {
            connect (pclGlobalData->vecPlots [0] [uiPlotCounter], SIGNAL (sigUpdateCursor (const double, const enum ePlotType_t, const unsigned int)),
                    this,                                         SLOT   (updateCursor    (const double, const enum ePlotType_t, const unsigned int)));
            connect (pclGlobalData->vecPlots [1] [uiPlotCounter], SIGNAL (sigUpdateCursor (const double, const enum ePlotType_t, const unsigned int)),
                    this,                                         SLOT   (updateCursor    (const double, const enum ePlotType_t, const unsigned int)));
        }
    }


    if (uiMeasID >= (unsigned int)this->qlMeasInfo.size())
    {
        this->uiWindowSize    = 0;
        this->uiTSActPosition = 0;
        this->uiMaxSamples    = 0;
        bRetValue = false;
    }
    else
    {
        this->uiActiveMeasID = uiMeasID;

        this->uiWindowSize    = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders [0].rSampleFreq;
        if (this->uiWindowSize > 65536)
        {
            this->uiWindowSize = 65536;
        }

        this->uiTSActPosition = sizeof (ATSHeader);
        this->uiMaxSamples    = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders [0].iSamples;

        for (uiPlotCounter = 0; uiPlotCounter < pclGlobalData->vecPlots [0].size(); uiPlotCounter++)
        {
            this->setPlotVisible(uiPlotCounter, false);
            emit (this->sigSetPlotButtonState(uiPlotCounter, false));
        }

        for (uiChCounter = 0; uiChCounter < (unsigned int)((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders.size(); uiChCounter++)
        {
            uiChNumberCorrected = ((MeasInfo*)this->qlMeasInfo [this->uiActiveMeasID])->qvecATSHeaders [uiChCounter].byChanNo;
            // on ADU-07e HF channels are located on 5 .. 9 - map them down to 0 .. 4 as for all
            // other ADU systems
            if (uiChNumberCorrected >= 5)
            {
                uiChNumberCorrected = uiChNumberCorrected - 5;
            }
            if (uiChNumberCorrected < pclGlobalData->vecPlots [0].size())
            {
                this->setPlotVisible(uiChNumberCorrected, true);
                emit (this->sigSetPlotButtonState(uiChNumberCorrected, true));

                this->uiLastVisiblePlot = uiChCounter;
            }
        }
        this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, this->uiTSActPosition, this->uiWindowSize * 4, false, STSpectraContBuffer);

        this->setWindowSize(QString::number(this->uiWindowSize), true);
        emit (this->sigNewWindowSize(QString::number(this->uiWindowSize)));
    }

    emit (this->sigMaxSamplesUpdated());
    emit (this->sigActSamplePosUpdated());

    return (bRetValue);
}


unsigned int TSStreamer::getMaxSamples(void)
{
    return (this->uiMaxSamples - this->uiWindowSize);
}


unsigned int TSStreamer::getActSamplePos(void)
{
    unsigned int uiTemp = (this->uiTSActPosition - sizeof (ATSHeader)) / 4;

    return (uiTemp);
}


bool TSStreamer::directConnectToSystem (const QString qstrIPAddress)
{
    bool       bRetValue = true;
    SystemInfo clInfo;

    qDeleteAll (this->qlMeasInfo);
    this->qlMeasInfo.clear();

    if (this->tryConnectSystem(qstrIPAddress, clInfo) == true)
    {
        this->qlSystemInfo.push_back((QObject*)new SystemInfo ());
        *((SystemInfo*)(this->qlSystemInfo.back())) = clInfo;
         ((SystemInfo*)(this->qlSystemInfo.back()))->uiIndex = this->qlSystemInfo.size() - 1;
        this->uiActiveSystemID = ((SystemInfo*)(this->qlSystemInfo.back()))->uiIndex;
        this->readMeasInfoFromTarget(qstrIPAddress);
    }
    else
    {
        bRetValue = false;
        emit (this->sigMeasListUpdated());
    }

    emit (this->sigSystemListUpdated());

    return (bRetValue);
}


bool TSStreamer::tryConnectSystem (const QString qstrIPAddress, SystemInfo& clSystemInfo)
{
    QTcpSocket clSocket;
    QSslSocket clSSLSocket;
    QString    qstrTemp;
    bool       bRetValue = true;

    // tell GUI that we are trying to connect
    this->bConnecting = true;
    emit (this->sigConnectingUpdated());

    clSocket.connectToHost(qstrIPAddress, 80);
    if (clSocket.waitForConnected(100) == true)
    {
        clSocket.disconnectFromHost();

        // second try: try to load index.html and check for ADU-07 or ADU-08 strings
        qstrTemp = "http://" + qstrIPAddress + "/index.html";
        qstrTemp = this->execHttpRequest(qstrTemp, false);
        if (qstrTemp.contains("ADU-07") == true)
        {
            #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                qDebug () << __PRETTY_FUNCTION__ << "found ADU-07e system at" << qstrIPAddress;
            #endif
            clSystemInfo = SystemInfo ("ADU-07e", qstrIPAddress);
            clSystemInfo.bUseHTTPS = false;
        }
        else
        {
            #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                qDebug () << __PRETTY_FUNCTION__ << "no ADU-07e system at" << qstrIPAddress;
            #endif
            bRetValue = false;
        }
    }
    else
    {
        clSSLSocket.setPeerVerifyMode      (QSslSocket::QueryPeer);
        clSSLSocket.connectToHostEncrypted (qstrIPAddress, 443);
        if (clSSLSocket.waitForConnected(100) == true)
        {
            clSSLSocket.disconnectFromHost();
            if (this->getHostCert(this->clHostCerts, qstrIPAddress) == true)
            {
                qstrTemp = "https://" + qstrIPAddress + "/index.html";
                qstrTemp = this->execHttpRequest(qstrTemp, true);
                if (qstrTemp.contains("ADU-08") == true)
                {
                    #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                        qDebug () << __PRETTY_FUNCTION__ << "found ADU-08e system at" << qstrIPAddress;
                    #endif
                    clSystemInfo = SystemInfo ("ADU-08e", qstrIPAddress);
                    clSystemInfo.bUseHTTPS = true;
                }
                else if (qstrTemp.contains("ADU-07") == true)
                {
                    #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                        qDebug () << __PRETTY_FUNCTION__ << "found ADU-07e system at" << qstrIPAddress;
                    #endif
                    clSystemInfo = SystemInfo ("ADU-07e", qstrIPAddress);
                    clSystemInfo.bUseHTTPS = true;
                }
                else
                {
                    #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                        qDebug () << __PRETTY_FUNCTION__ << "failed to connect to system at" << qstrIPAddress << "(index.html)";
                    #endif
                    bRetValue = false;
                }
            }
            else
            {
                #ifdef C_TSSTREAMER_DEBUG_OUTPUT
                    qDebug () << __PRETTY_FUNCTION__ << "failed to connect to system at" << qstrIPAddress << "(host certs)";
                #endif
                bRetValue = false;
            }
        }
        else
        {
            bRetValue = false;
        }
    }

    // tell GUI that we are trying to connect
    this->bConnecting = false;
    emit (this->sigConnectingUpdated());

    return (bRetValue);
}


void TSStreamer::connectToSystem (const unsigned int uiSystemID)
{
    if (uiSystemID < (unsigned int)qlSystemInfo.size())
    {
        this->uiActiveSystemID = uiSystemID;
        this->readMeasInfoFromTarget(((SystemInfo*) this->qlSystemInfo.at(uiSystemID))->qstrIPAddress);
    }
}


bool TSStreamer::getHostCert (QList<QSslCertificate> &clCerts, const QString qstrUrl)
{
    bool        bRetValue = false;
    QSslSocket  clSocket;
    QString     qstrTemp;
    QList<QSslCertificate> clHostCertsTemp;

    qstrTemp = qstrUrl;
    qstrTemp = qstrTemp.right(qstrTemp.size() - qstrTemp.lastIndexOf("/") - 1);

    clCerts.clear();

    clSocket.setPeerVerifyMode      (QSslSocket::QueryPeer);
    clSocket.connectToHostEncrypted (qstrTemp, 443);
    clSocket.waitForConnected(2000);
    clSocket.waitForEncrypted(2000);
    if (clSocket.isValid() == true)
    {
        clHostCertsTemp = clSocket.peerCertificateChain();

        for (const QSslCertificate &cert : clHostCertsTemp)
        {
            qDebug() << cert.issuerInfo(QSslCertificate::Organization);
            qDebug() << cert.issuerInfo(QSslCertificate::OrganizationalUnitName);
            qDebug() << cert.issuerInfo(QSslCertificate::CountryName);

            if ((cert.issuerInfo(QSslCertificate::Organization).contains            (C_SSL_CERT_ORGANIZATION)           == true) &&
                (cert.issuerInfo(QSslCertificate::OrganizationalUnitName).contains  (C_SSL_CERT_ORGANIZATION_UNIT_NAME) == true))
            {
                clCerts.push_back(cert);
                bRetValue = true;
            }
        }
        clSocket.close();
    }
    else
    {
        bRetValue = false;
    }

    return (bRetValue);
}


void TSStreamer::setPlotVisible (const unsigned int uiCH, const bool bVisible)
{
    unsigned int uiCounter;
    unsigned int uiLastVisiblePlot;

    if (uiCH < pclGlobalData->vecPlots [0].size())
    {
        if (bVisible == true)
        {
            pclGlobalData->vecPlots [0] [uiCH]->showPlot();
        }
        else
        {
            pclGlobalData->vecPlots [0] [uiCH]->hidePlot();
        }
    }

    if (uiCH < pclGlobalData->vecPlots [1].size())
    {
        if (bVisible == true)
        {
            pclGlobalData->vecPlots [1] [uiCH]->showPlot();
        }
        else
        {
            pclGlobalData->vecPlots [1] [uiCH]->hidePlot();
        }
    }

    // search for last plot that is visible and activate x axis tickmarks
    uiLastVisiblePlot = 0xFFFFFFFF;
    for (uiCounter = 0; uiCounter < pclGlobalData->vecPlots [0].size(); uiCounter++)
    {
        if (pclGlobalData->vecPlots [0][uiCounter]->isVisible() == true)
        {
            pclGlobalData->vecPlots [0] [uiCounter]->showPlot(false);
            pclGlobalData->vecPlots [1] [uiCounter]->showPlot(false);
            uiLastVisiblePlot = uiCounter;
        }
    }

    if (uiLastVisiblePlot != 0xFFFFFFFF)
    {
        pclGlobalData->vecPlots [0] [uiLastVisiblePlot]->showPlot(true);
        pclGlobalData->vecPlots [1] [uiLastVisiblePlot]->showPlot(true);

        this->uiLastVisiblePlot = uiLastVisiblePlot;
    }
}


void TSStreamer::setWindowSize (const QString qstrWindowSize, const bool bForceUpdate)
{
    int          iCounter;
    bool         bConvOK = true;
    unsigned int uiNewWindowSize;

    uiNewWindowSize = qstrWindowSize.toUInt(&bConvOK);

    qDebug () << "new window size:" << qstrWindowSize << uiNewWindowSize;

    if (bConvOK == true)
    {
        if ((this->uiWindowSize != uiNewWindowSize) ||
            (bForceUpdate == true))
        {
            this->uiWindowSize = uiNewWindowSize;

            for (iCounter = 0; iCounter < (int)pclGlobalData->vecPlots [0].size(); iCounter++)
            {
                pclGlobalData->vecPlots [0] [iCounter]->slotNewWindowSize(this->uiWindowSize);
            }

            for (iCounter = 0; iCounter < (int)pclGlobalData->vecPlots [1].size(); iCounter++)
            {
                pclGlobalData->vecPlots [1] [iCounter]->slotNewWindowSize(this->uiWindowSize);
            }

            if (this->qstrTargetIPAddress.size() > 0)
            {
                this->readData(this->qstrTargetIPAddress, this->uiActiveMeasID, this->uiTSActPosition, this->uiWindowSize * 4, false, STSpectraContBuffer);
            }
        }
    }
}


void TSStreamer::updateCursor(const double dXAxisPos, const enum ePlotType_t ePlotType, const unsigned int uiPlotNum)
{
    double          dXValue = 0.0;
    double          dTemp;
    unsigned int    uiPlotCounter;
    unsigned int    uiBufferIndex;
    qint64          qiMsecs;
    QVector<double> qvecYValue;

    QString         qstrXAxis;
    QString         qstrCH0;
    QString         qstrCH1;
    QString         qstrCH2;
    QString         qstrCH3;
    QString         qstrCH4;

    qDebug () << "plot type:" << ePlotType << "x-axis pos:" << dXAxisPos;

    if (ePlotType == PlotTypeTS)
    {
        for (uiPlotCounter = 0; uiPlotCounter < pclGlobalData->vecPlots [0].size(); uiPlotCounter++)
        {
            qvecYValue.push_back(0.0);
            pclGlobalData->vecPlots [0][uiPlotCounter]->slotGetValue(dXAxisPos, dTemp, qvecYValue[qvecYValue.size() - 1]);
            if (pclGlobalData->vecPlots [0][uiPlotCounter]->isVisible() == true)
            {
                dXValue = dTemp;
            }
        }

        qiMsecs = (unsigned int) dXValue;
        qiMsecs = qiMsecs * 1000;
        qiMsecs = qiMsecs + ((dXValue - (qiMsecs / 1000.0)) * 1000.0);
        qstrXAxis = QDateTime::fromMSecsSinceEpoch(qiMsecs).toString("HH:mm:ss-zzz");

        qstrCH0 = "CH0: " + QString::number(qvecYValue [0], 'e', 3) + "mV";
        qstrCH1 = "CH1: " + QString::number(qvecYValue [1], 'e', 3) + "mV";
        qstrCH2 = "CH2: " + QString::number(qvecYValue [2], 'e', 3) + "mV";
        qstrCH3 = "CH3: " + QString::number(qvecYValue [3], 'e', 3) + "mV";
        qstrCH4 = "CH4: " + QString::number(qvecYValue [4], 'e', 3) + "mV";
    }
    else if (ePlotType == PlotTypeSpectra)
    {
        // search for nearby maximum and use that one
        uiBufferIndex = pclGlobalData->vecPlots [1][uiPlotNum]->slotGetValueMaxIndex((unsigned int)dXAxisPos);

        for (uiPlotCounter = 0; uiPlotCounter < pclGlobalData->vecPlots [1].size(); uiPlotCounter++)
        {
            qvecYValue.push_back(0.0);
            pclGlobalData->vecPlots [1][uiPlotCounter]->slotGetValue(uiBufferIndex, dTemp, qvecYValue[qvecYValue.size() - 1]);
            if (pclGlobalData->vecPlots [1][uiPlotCounter]->isVisible() == true)
            {
                dXValue = dTemp;
            }
        }

        qstrXAxis = QString::number(dXValue, 'g', 3) + "Hz";

        qstrCH0 = "CH0: " + QString::number(qvecYValue [0], 'e', 3) + "mV/√Hz";
        qstrCH1 = "CH1: " + QString::number(qvecYValue [1], 'e', 3) + "mV/√Hz";
        qstrCH2 = "CH2: " + QString::number(qvecYValue [2], 'e', 3) + "mV/√Hz";
        qstrCH3 = "CH3: " + QString::number(qvecYValue [3], 'e', 3) + "mV/√Hz";
        qstrCH4 = "CH4: " + QString::number(qvecYValue [4], 'e', 3) + "mV/√Hz";
    }

    emit (this->sigShowValues(qstrXAxis, qstrCH0, qstrCH1, qstrCH2, qstrCH3, qstrCH4));
}


void TSStreamer::cancelShowSTSpectra(void)
{
    this->atomCancelShowSTSpectra.store(true);
}

bool TSStreamer::getConnecting (void)
{
    return (this->bConnecting);
}

