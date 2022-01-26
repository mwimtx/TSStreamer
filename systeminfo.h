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

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>

class SystemInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString      name      READ getName       NOTIFY nameChanged)
    Q_PROPERTY(QString      ipAddress READ getIPAddress  NOTIFY ipAddressChanged)
    Q_PROPERTY(unsigned int index     READ getIndex      NOTIFY indexChanged)

    public:
        SystemInfo(void);

        SystemInfo(const QString qstrNewName, const QString qstrNewIPAddress);

        QString qstrName;

        QString qstrIPAddress;

        bool bUseHTTPS;

        unsigned int uiIndex;

        SystemInfo& operator=(const SystemInfo & clInstanceToCopy);

    public slots:

        QString getName() const;

        QString getIPAddress() const;

        unsigned int getIndex (void);

    Q_SIGNALS:

        void nameChanged (void);

        void ipAddressChanged (void);

        void indexChanged (void);

    private:
};

#endif // SYSTEMINFO_H
