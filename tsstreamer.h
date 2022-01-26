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

#ifndef TSSTREAMER_H
#define TSSTREAMER_H

#include <QString>
#include <QVector>
#include <QNetworkReply>

#include <mutex>

#include "measinfo.h"
#include "systeminfo.h"
#include "qmlplot.h"


/**
 * defines the valid values for host certificates for organization
 * and OrganizationalUnitName.
 */
#define C_SSL_CERT_ORGANIZATION             "Metronix GmbH"
#define C_SSL_CERT_ORGANIZATION_UNIT_NAME   "Geophysics"


/**
 * defines username and password for webservice access
 */
#define C_WEBSERVICE_USER   "aduuser"
#define C_WEBSERVICE_PASS   "neptun"


class TSStreamer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant     systemList   READ getSystemList   NOTIFY sigSystemListUpdated)
    Q_PROPERTY(QVariant     measList     READ getMeasList     NOTIFY sigMeasListUpdated)
    Q_PROPERTY(unsigned int maxSamples   READ getMaxSamples   NOTIFY sigMaxSamplesUpdated)
    Q_PROPERTY(unsigned int actSamplePos READ getActSamplePos NOTIFY sigActSamplePosUpdated)
    Q_PROPERTY(bool         connecting   READ getConnecting   NOTIFY sigConnectingUpdated)

    Q_PROPERTY(QString directTargetIP        READ getDirectTargetIP          NOTIFY sigDirectTargetIPUpdated)
    Q_PROPERTY(bool    connectDirectTargetIP READ getConnectDirectTargetIP   WRITE  setConnectDirectTargetIP  NOTIFY sigConnectDirectTargetIPUpdated)

    Q_PROPERTY(bool    directOpenTSScopeMode READ getDirectOpenTSScopeMode   WRITE  setDirectOpenTSScopeMode  NOTIFY sigDirectOpenTSScopeMode)

    public:
        TSStreamer(const QString qstrTargetIP = "");


    private slots:

        void onRequestFinished (void);

        void onRangeRequestReadyRead (void);

        void onRangeRequestFinished  (void);


    public slots:

        QString getDirectTargetIP (void)
        {
            return (this->qstrDirectTargetIP);
        }

        bool getConnectDirectTargetIP (void)
        {
            return (this->bConnectDirectTargetIP);
        }

        void setConnectDirectTargetIP (const bool bValue)
        {
            this->bConnectDirectTargetIP = bValue;
        }

        bool getDirectOpenTSScopeMode (void)
        {
            return (this->bDirectOpenTSScopeMode);
        }

        void setDirectOpenTSScopeMode (const bool bValue)
        {
            this->bDirectOpenTSScopeMode = bValue;
        }

        bool readMeasInfoFromTarget (void);

        bool readMeasInfoFromTarget (const QString qstrTargetIP);

        bool readData (const QString qstrTargetIP, const unsigned int uiMeasIndex, const unsigned int uiStartPos, const unsigned int uiNumBytes,
                       const bool bShowSTSpectra, const eStackedSpectraMode_t eSTSpectraMode);

        bool scanForSystems (const QString qstrTargetIP = "");

        QVariant getSystemList (void);

        QVariant getMeasList (void);

        bool getConnecting (void);

        bool selectMeas (const unsigned int uiMeasID);

        void showNextTSBuffer (void);

        void showPrevTSBuffer (void);

        unsigned int getMaxSamples (void);

        unsigned int getActSamplePos (void);

        void showTSBufferAtPos (unsigned int uiPos);

        bool directConnectToSystem(const QString qstrIPAddress);

        void connectToSystem (const unsigned int uiSystemID);

        void setPlotVisible (const unsigned int uiCH, const bool bVisible);

        void setWindowSize (const QString qstrWindowSize, const bool bForceUpdate);

        void updateCursor(const double dXAxisPos, const enum ePlotType_t ePlotType, const unsigned int uiPlotNum);

        void showTSBufferScopeMode (void);

        void showSTSpectra (void);

        void cancelShowSTSpectra (void);



    signals:

        void sigDirectOpenTSScopeMode (void);

        void sigDirectTargetIPUpdated (void);

        void sigConnectDirectTargetIPUpdated (void);

        void sigRequestFinished (void);

        void sigSystemListUpdated (void);

        void sigMeasListUpdated (void);

        void sigMaxSamplesUpdated (void);

        void sigActSamplePosUpdated (void);

        void sigConnectingUpdated (void);

        void sigSetPlotButtonState (unsigned int uiCH, bool bEnabled);

        void sigNewWindowSize (QString qstrWindowSize);

        void sigShowValues (QString qstrXAxis, QString qstrCH0, QString qstrCH1,
                            QString qstrCH2,   QString qstrCH3, QString qstrCH4);

        void sigShowProgress (QString qstrText, double dProgress);

        void sigShowSTSpectraFinished (void);


    private:

        QString qstrDirectTargetIP     = "";
        bool    bConnectDirectTargetIP = false;

        bool bDirectOpenTSScopeMode = false;

        QString qstrRequestResponse;

        QByteArray qbaRequestResponse;

        QNetworkReply* pclNetworkreply;

        QNetworkAccessManager clNam;

        QUrl clUrl;

        QList<QObject*> qlSystemInfo;

        QList<QObject*> qlMeasInfo;

        QString qstrTargetIPAddress;

        QString execHttpRequest (const QString qstrRequest, const bool bUseSSL);

        unsigned int uiActiveMeasID;

        unsigned int uiActiveSystemID;

        unsigned int uiTSActPosition;

        unsigned int uiWindowSize;

        unsigned int uiMaxSamples;

        unsigned int uiLastVisiblePlot;

        QList<QSslCertificate> clHostCerts;

        bool bFirstTimeMeasSelected;

        bool bRangeRequestError;

        std::atomic_bool atomHttpAccessActive;

        std::atomic_bool atomCancelShowSTSpectra;

        bool bConnecting;

        bool getHostCert (QList<QSslCertificate> &clCerts, const QString qstrUrl);

        void execHttpRangeRequest(const QString qstrRequest, const unsigned int uiStartPos, const unsigned int uiNumBytes, const bool bUseSSL);

        bool tryConnectSystem (const QString qstrIPAddress, SystemInfo &clSystemInfo);


};

#endif // TSSTREAMER_H
