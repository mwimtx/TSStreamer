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

#ifndef MEASINFO_H
#define MEASINFO_H

#include <QString>
#include <QVector>
#include <QObject>



typedef unsigned long long int  uint64;
typedef unsigned int            uint32;
typedef unsigned short          uint16;
typedef unsigned char           uint8;

typedef long long int           int64;
typedef int                     int32;
typedef short                   int16;
typedef char                    int8;

typedef float                   float32;
typedef double                  float64;


#define ADU07_MAX_NUM_ADB 10

/**
 * This defines the actial header version of the ATS files, that are created,
 * by this class.
 */
#ifdef C_MCP_CEA
    #define ATS_HEAD_VERS 1080
#else
    #define ATS_HEAD_VERS 80
#endif


/**
 * This defines the current ATM header version.
 */
#ifdef C_MCP_CEA
    #define ATM_HEAD_VERS   1010
#else
    #define ATM_HEAD_VERS   10
#endif


/**
 * In CEA mode this constant defines the maximum number of headers that can be stored
 * inside the begining of the file.<br>
 * <br>
 * 1024 header: 1 * ATS header + 1023 * sliced header
 */
#define C_ATS_CEA_NUM_HEADERS   1023     ///< add a maximum number of 1023 headers to the ATS file.


/**
 * This constants define the ADU system names, that are written into the ATS Headers.
 * They differ from the entries in the hardware database. Therefore they are redefined
 * here.
 */
#define ATS_HEAD_SYS_NAME_UNKNOWN   "unknown"   ///< unknown system type
#define ATS_HEAD_SYS_NAME_ADU06     "ADU06"     ///< system name for ADU-06 systems
#define ATS_HEAD_SYS_NAME_ADU07     "ADU07"     ///< system name for ADU-07 systems


/**
 * This constant defines the maximum number of samples in one single ATS file,
 * if the time series must be split up into several files to avoid the Linux
 * file system error "max filesize exceeded".
 */
#define ATS_MAX_NUM_SAMPLES_BY_FILE 534773760


/**
 * This constants define the ADC Type for the selection of the LSB values.
 */
typedef enum
{
    ADU07_ADC_INV         =  0,      ///< invalid/unknown ADC
    ADU07_ADC_LF          =  1,      ///< ADU-07 LF ADC
    ADU07_ADC_HF          =  2,      ///< ADU-07 HF ADC
    ADU07_ADC_MF_LF_Mode  =  3,      ///< ADU-07 MF ADC in LF mode
    ADU07_ADC_MF_HF_Mode  =  4,      ///< ADU-07 MF ADC in LF mode
    ADU08_ADC_LF          =  5,      ///< ADU-08e BB LF ADC
    ADU08_ADC_HF          =  6       ///< ADU-08e BB HF ADC
} ADC_Types;


/**
 * Substructure for the comments section of the ATS header.
 */
typedef struct ATSComments_s
{
    int8 achClient      [16];   // 000 h
    int8 achContractor  [16];   // 010 h
    int8 achArea        [16];   // 020 h
    int8 achSurveyID    [16];   // 030 h
    int8 achOperator    [16];   // 040 h
    int8 achReserved   [112];   // 050 h
    int8 achXmlHeader   [64];   // 0C0 h
    int8 achComments   [512];   // 100 h
} ATSComments;


/**
 * The real ATMHeader according to the definition from Metronix.
 */
typedef struct ATMHeader_s
{
    int16   siHeaderLength;             // 000h
    int16   siHeaderVers;               // 002h
    int32   iSamples;                   // 004h
} ATMHeader;


/**
 * Slice Header for CEA<br>
 * <br>
 * IMPORTANT: slice header must always be n * 4 byte (n * size of 1 sample)<br>
 */
typedef struct ATSSliceHeader_s
{
    int32   iSamples;                   ///< 0000h: number of samples for this slice
    int32   iStartDateTime;             ///< 0004h: startdate/time as UNIX timestamp (UTC)
    float64 dbDCOffsetCorrValue;        ///< 0008h: DC offset correction value in mV
    float32 rPreGain,                   ///< 0010h: originally used pre-gain (GainStage1) - LSB is the same for all slices
            rPostGain;                  ///< 0014h: originally used post-gain (GainStage2) - LSB is the same for all slices
    int8    byDCOffsetCorrOn;           ///< 0018h: DC offset correction was on/off for this slice
    int8    reserved [7];               ///< 0020h: reserved bytes to get to word / dword boundary
}ATSSliceHeader;


/**
 * The real ATSHeader according to the definition from Metronix.
 */
typedef struct ATSHeader_s
{
    int16   siHeaderLength;             // 000h
    int16   siHeaderVers;               // 002h

    // This information can be found in the ChannelTS datastructure
    int32   iSamples;                   // 004h
    float32 rSampleFreq;                // 008h
    int32   iStartDateTime;             // 00Ch
    float64 dblLSBMV;                   // 010h
    int32   iGMTOffset;                 // 018h
    float32 rOrigSampleFreq;            // 01Ch

    // The required data could probably found in the HardwareConfig
    int16   siADUSerNum,                // 020h
            siADCSerNum;                // 022h
    int8    byChanNo,                   // 024h
            byChopper;                  // 025h     // Chopper On/Off

    // Data from XML Job-specification
    int8    abyChanType   [2];          // 026h
    int8    abySensorType [6];          // 028h
    int16   siSensorSerNum;             // 02Eh

    float32 rPosX1,                     // 030h
            rPosY1,                     // 034h
            rPosZ1,                     // 038h
            rPosX2,                     // 03Ch
            rPosY2,                     // 040h
            rPosZ2,                     // 044h
            rDipLength,                 // 048h
            rAngle;                     // 04Ch

    // Data from Selftest ?
    float32 rProbeRes,                  // 050h
            rDCOffset,                  // 054h
            rPreGain,                   // 058h
            rPostGain;                  // 05Ch

    // Data from status information ?
    int32   iLatitude,                  // 060h
            iLongitude;                 // 064h
    int32   iElevation;                 // 068h
    int8    byLatLongType,              // 06Ch
            byAddCoordType;             // 06Dh
    int16   siRefMedian;                // 06Eh
    float64 dblXCoord,                  // 070h
            dblYCoord;                  // 078h
    int8    byGPSStat,                  // 080h
            byGPSAccuracy;              // 081h
    int16   iUTCOffset;                 // 082h
    int8    abySystemType[12];          // 084h

    // Data from XML-Job specification
    int8    abySurveyHeaderName [12];   // 090h
    int8    abyMeasType          [4];   // 09Ch

    //TODO[OKH]
    // Next three fields will not be supported any more.
    float64 dbDCOffsetCorrValue;        // 0A0h
    int8    byDCOffsetCorrOn;           // 0A8h
    int8    byInputDivOn;               // 0A9h
    int8    abyReserved6      [2];      // 0AAh
    int8    abySelfTestResult [2];      // 0ACh
    uint16  uiNumSlices;                // 0AEh --- number of slices in CEA mode - 0 for only main ATS header (1 slice) / if 0, now ATSSliceHeader after main ATS header

    //TODO[OKH]
    // Were the following fields ever used ?
    int16   siCalFreqs,                 // 0B0h
            siCalEntryLength,           // 0B2h
            siCalVersion,               // 0B4h
            siCalStartAddress;          // 0B6h

    int8    abyLFFilters [8];           // 0B8h

    int8    abyADU06CalFilename  [12];  // 0C0h
    int32   iADUCalTimeDate;            // 0CCh
    int8    abySensorCalFilename [12];  // 0D0h
    int32   iSensorCalTimeDate;         // 0DCh

    float32 rPowerlineFreq1,            // 0E0h
            rPowerlineFreq2;            // 0E4h
    int8    abyHFFilters[8];            // 0E8h

    float64 dblOriginalLSBMV;           // 0F0h
    float32 rExtGain;                   // 0F8h
    int8    abyADBBoardType[4];         // 0FCh

    // Comes from XML-Job spec.
    ATSComments tscComment;             // 100h
} ATSHeader;


/**
 * The following structure defines some information, which is organised form
 * the measurement documentation XML file. This is actually the hardware
 * dependent stuff, which is closely related to the ADU06
 */
typedef struct ATSHeaderGlobals_s
{
    // this is the adc type. it is set in the get_globals function.
    // according to this flag, the lsb, that is finally written to the
    // ATS-file is selected.
    int    iAdcType     [ADU07_MAX_NUM_ADB];
    //int iLF [ADU07_MAX_NUM_ADB];  // LF -> iLF = 1, HF -> iLF = 0;

    // Stuff from selftest
    double dblLSBMV_HF          [ADU07_MAX_NUM_ADB];
    double dblLSBMV_LF          [ADU07_MAX_NUM_ADB];
    double dblProbeRes          [ADU07_MAX_NUM_ADB];
    double dblDCOffset          [ADU07_MAX_NUM_ADB];
    double dblGainAmpl          [ADU07_MAX_NUM_ADB];
    double dblHwDivider         [ADU07_MAX_NUM_ADB];

    // GPS related information
    int iLatitude,
        iLongitude,
        iElevation;

    int iGPSStat,
        iGPSAccuracy,
        iUTCOffset;

    char szSystemType[12];
} ATSHeaderGlobals;






class MeasInfo : public QObject
{

    Q_PROPERTY(QString startDate  READ getStartDateString  NOTIFY startDateChanged)
    Q_PROPERTY(QString duration   READ getDurationString   NOTIFY durationChanged)
    Q_PROPERTY(QString sampleFreq READ getSampleFreqString NOTIFY sampleFreqChanged)
    Q_PROPERTY(int     measID     READ getMeasID           NOTIFY measIDChanged)

    Q_OBJECT

    public slots:

        QString getStartDateString (void);

        unsigned int getStartDate (void);

        QString getDurationString (void);

        QString getSampleFreqString (void);

        int getMeasID (void);

    Q_SIGNALS:

        void startDateChanged (void);

        void durationChanged (void);

        void sampleFreqChanged (void);

        void measIDChanged (void);

    public:

        MeasInfo (const int iIndexID);

        MeasInfo (const MeasInfo& clInit);

        QString qstrRootPath;

        QString qstrMeasDocXML;

        QVector<QString> qvecATSFiles;

        QVector<ATSHeader> qvecATSHeaders;

        int iMeasID;

        bool operator< (const MeasInfo& rhs);

        bool operator> (const MeasInfo& rhs);

        bool operator<=(const MeasInfo& rhs);

        bool operator>=(const MeasInfo& rhs);

        bool operator==(const MeasInfo& rhs);

        bool operator!=(const MeasInfo& rhs);

        MeasInfo& operator=(const MeasInfo & clInstanceToCopy);
};



#endif // MEASINFO_H
