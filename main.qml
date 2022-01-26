import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import CustomPlot 1.0
//import QtQuick.Extras 1.4

import "GlobalIncludes.js" as GlobalIncludes

ApplicationWindow
{
    id: mainWindow
    visible: true
    width: 1024
    height: 768

    Connections {
        target: tsStreamer
        onSigNewWindowSize: {
            cbWindowSize.currentIndex = cbWindowSize.find(qstrWindowSize);
        }
    }

    Timer {
        id: timerCloseShowValues
        interval: 5000
        repeat: false
        triggeredOnStart: false
        onTriggered: {
            popupShowValues.close();
        }
    }

    Timer {
        id: timerCloseShowProgress
        interval: 1000
        repeat: false
        triggeredOnStart: false
        onTriggered: {
            popupShowProgress.close();
        }
    }

    Connections {
        target: tsStreamer
        onSigShowProgress: {
            rectProgress.width = (rectProgressFrame.width - (GlobalIncludes.mtxBorderLineStrength * 4)) * dProgress;
            labProgressText.text = qstrText;
            popupShowProgress.open();
            timerCloseShowProgress.restart();
        }
    }

    Connections {
        target: tsStreamer
        onSigShowSTSpectraFinished: {
            toggleShowStackedSpectra.checked = true;
        }
    }


    Connections {
        target: tsStreamer
        onSigShowValues: {
            popupShowValues.open();

            labShowValueX.text   = qstrXAxis;
            labShowValueCH0.text = qstrCH0;
            labShowValueCH1.text = qstrCH1;
            labShowValueCH2.text = qstrCH2;
            labShowValueCH3.text = qstrCH3;
            labShowValueCH4.text = qstrCH4;
            timerCloseShowValues.restart();

            labShowValueCH0.visible = buttToggleCH0.checked;
            labShowValueCH1.visible = buttToggleCH1.checked;
            labShowValueCH2.visible = buttToggleCH2.checked;
            labShowValueCH3.visible = buttToggleCH3.checked;
            labShowValueCH4.visible = buttToggleCH4.checked;
        }
    }

    Connections {
        target: tsStreamer
        onSigSetPlotButtonState: {

            switch (uiCH)
            {
                case 0:
                    if (bEnabled == true)
                    {
                        buttToggleCH0.checked = true
                        buttToggleCH0.enabled = true
                    }
                    else
                    {
                        buttToggleCH0.checked = false
                        buttToggleCH0.enabled = false
                    }
                    break;

                case 1:
                    if (bEnabled == true)
                    {
                        buttToggleCH1.checked = true
                        buttToggleCH1.enabled = true
                    }
                    else
                    {
                        buttToggleCH1.checked = false
                        buttToggleCH1.enabled = false
                    }
                    break;


                case 2:
                    if (bEnabled == true)
                    {
                        buttToggleCH2.checked = true
                        buttToggleCH2.enabled = true
                    }
                    else
                    {
                        buttToggleCH2.checked = false
                        buttToggleCH2.enabled = false
                    }
                    break;


                case 3:
                    if (bEnabled == true)
                    {
                        buttToggleCH3.checked = true
                        buttToggleCH3.enabled = true
                    }
                    else
                    {
                        buttToggleCH3.checked = false
                        buttToggleCH3.enabled = false
                    }
                    break;


                case 4:
                    if (bEnabled == true)
                    {
                        buttToggleCH4.checked = true
                        buttToggleCH4.enabled = true
                    }
                    else
                    {
                        buttToggleCH4.checked = false
                        buttToggleCH4.enabled = false
                    }
                    break;
            }
        }
    }


    Popup {
        id: popupShowProgress
        height: 100
        y: footer.y + footer.height - height
        width: footer.width
        visible: false
        padding: GlobalIncludes.mtxBorderLineStrength
        closePolicy: Popup.NoAutoClose

        Rectangle {
            anchors.fill: parent
            border.width: GlobalIncludes.mtxBorderLineStrength
            border.color: GlobalIncludes.mtxFontColor

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: (GlobalIncludes.mtxBorderLineStrength * 2)

                Text {
                    id: labProgressText
                    Layout.minimumHeight: GlobalIncludes.mtxFontSizeBig
                    Layout.maximumHeight: GlobalIncludes.mtxFontSizeBig
                    Layout.fillWidth: true
                    horizontalAlignment: Qt.AlignLeft
                    verticalAlignment: Qt.AlignVCenter
                    font.pointSize: GlobalIncludes.mtxFontSizeBig
                    color: GlobalIncludes.mtxFontColor
                    text: "---"
                }

                Rectangle {
                    id: rectProgressFrame
                    Layout.minimumHeight: GlobalIncludes.mtxFontSizeBig * 2
                    Layout.maximumHeight: GlobalIncludes.mtxFontSizeBig * 2
                    Layout.fillWidth: true
                    border.width: GlobalIncludes.mtxBorderLineStrength
                    border.color: GlobalIncludes.mtxFontColor
                    radius:  (GlobalIncludes.mtxBorderLineStrength * 2)

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: (GlobalIncludes.mtxBorderLineStrength * 2)

                        Rectangle {
                            id: rectProgress
                            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                            Layout.fillHeight: true
                            width: 1
                            radius: (GlobalIncludes.mtxBorderLineStrength * 2)
                            color: GlobalIncludes.mtxColorOrange
                        }
                    }
                }
            }
        }
    }


    Popup {
        id: popupShowValues
        height: 80
        y: footer.y + footer.height - height
        width: footer.width
        visible: false
        padding: GlobalIncludes.mtxBorderLineStrength
        closePolicy: Popup.NoAutoClose

        Rectangle {
            anchors.fill: parent
            border.width: GlobalIncludes.mtxBorderLineStrength
            border.color: GlobalIncludes.mtxFontColor

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.margins: GlobalIncludes.mtxBorderLineStrength

                    Text {
                        id: labShowValueX
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignBottom
                        font.pointSize: GlobalIncludes.mtxFontSizeNormal
                        color: GlobalIncludes.mtxFontColor
                        text: "X-Axis Value:"
                    }
                }

                RowLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.margins: GlobalIncludes.mtxBorderLineStrength

                    Text {
                        id: labShowValueCH0
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        font.pointSize: GlobalIncludes.mtxFontSizeSmall
                        color: GlobalIncludes.mtxFontColor
                        text: "CH0"
                    }
                    Text {
                        id: labShowValueCH1
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        font.pointSize: GlobalIncludes.mtxFontSizeSmall
                        color: GlobalIncludes.mtxFontColor
                        text: "CH1"
                    }
                    Text {
                        id: labShowValueCH2
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        font.pointSize: GlobalIncludes.mtxFontSizeSmall
                        color: GlobalIncludes.mtxFontColor
                        text: "CH2"
                    }
                    Text {
                        id: labShowValueCH3
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        font.pointSize: GlobalIncludes.mtxFontSizeSmall
                        color: GlobalIncludes.mtxFontColor
                        text: "CH3"
                    }
                    Text {
                        id: labShowValueCH4
                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        font.pointSize: GlobalIncludes.mtxFontSizeSmall
                        color: GlobalIncludes.mtxFontColor
                        text: "CH4"
                    }
                }
            }
        }
    }

    Drawer {
        // used to show available time series on ADU and other utilities
        id: drawerUtils
        width: mainWindow.width
        height: mainWindow.height * 0.9
        edge: Qt.BottomEdge
        dragMargin: GlobalIncludes.mtxFontSizeNormal * 1.5

        Timer {
            id: timerDirectConnectTSScopeMode1
            interval: 500
            repeat: true
            running : true
            triggeredOnStart: false
            onTriggered: {
                if (tsStreamer.directOpenTSScopeMode == true)
                {
                    tsStreamer.selectMeas(0);
                    toggleScopeMode.checked = Qt.Checked;
                    tsStreamer.directOpenTSScopeMode = false;
                    timerDirectConnectTSScopeMode2.start()
                }
            }
        }

        Timer {
            id: timerDirectConnectTSScopeMode2
            interval: 250
            repeat: false
            running : false
            triggeredOnStart: false
            onTriggered: {
                toggleScopeMode.checked = Qt.Unchecked;
                drawerUtils.close();
            }
        }

        ColumnLayout {
            anchors.fill: parent

            Rectangle {
                Layout.fillWidth: true
                Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal
                Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal
                color: "lightgrey"

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font.pointSize: GlobalIncludes.mtxFontSizeNormal
                    font.bold: true
                    color: GlobalIncludes.mtxFontColor
                    text: "="
                }
            }

            Rectangle {
                id: rectDrawerBase
                Layout.fillHeight: true
                Layout.fillWidth: true

                RowLayout {
                    anchors.fill: parent

                    Rectangle {
                        // shows measurement info
                        id: rectMasInfo
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        ColumnLayout {
                            anchors.fill: parent

                            MtxButton {
                                Layout.fillWidth: true
                                text: "Search For Measurements"

                                onClicked: {
                                    tsStreamer.readMeasInfoFromTarget();
                                }
                            }

                            ScrollView {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                ScrollBar.vertical.interactive: true
                                clip: true

                                ListView {
                                    id: lviewMeasInfo
                                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.margins: GlobalIncludes.mtxMargin
                                    model: tsStreamer.measList


                                    delegate: Rectangle {

                                        id: box
                                        height: GlobalIncludes.mtxFontSizeNormal * 5
                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                                        Layout.fillWidth: true
                                        anchors.margins: GlobalIncludes.mtxBorderLineStrength

                                        width: parent.width
                                        color: index % 2 == 0 ? GlobalIncludes.mtxListViewEvenItemColor : GlobalIncludes.mtxListViewOddItemColor
                                        border.width: 0
                                        border.color: GlobalIncludes.mtxBorderColor
                                        radius: 0

                                        RowLayout {
                                            anchors.fill: parent
                                            anchors.margins: GlobalIncludes.mtxBorderLineStrength
                                            Layout.margins:  GlobalIncludes.mtxBorderLineStrength

                                            Rectangle {
                                                id: rectSystemInfos
                                                Layout.fillHeight: true
                                                Layout.fillWidth: true
                                                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                                anchors.margins: GlobalIncludes.mtxBorderLineStrength

                                                color: index % 2 == 0 ? GlobalIncludes.mtxListViewEvenItemColor : GlobalIncludes.mtxListViewOddItemColor

                                                MouseArea {
                                                    anchors.fill: parent
                                                    onClicked: {
                                                        if (toggleScopeMode.checked == false)
                                                        {
                                                            toggleScopeMode.checked = true;
                                                            measChangedTimer.start();
                                                        }
                                                        else
                                                        {
                                                            drawerUtils.close ();
                                                            tsStreamer.selectMeas(modelData.measID);
                                                        }
                                                    }
                                                }

                                                Timer {
                                                   id: measChangedTimer
                                                   interval: 1200
                                                   repeat: false
                                                   running: false
                                                   triggeredOnStart: false
                                                   onTriggered: {
                                                       drawerUtils.close ();
                                                       tsStreamer.selectMeas(modelData.measID);
                                                       toggleScopeMode.checked = false
                                                   }
                                                }

                                                ColumnLayout {
                                                    anchors.fill: parent
                                                    Layout.margins: GlobalIncludes.mtxMargin
                                                    anchors.centerIn: rectSystemInfos

                                                    Text {
                                                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                                        text: modelData.startDate
                                                        color: GlobalIncludes.mtxFontColor
                                                        font.pointSize: GlobalIncludes.mtxFontSizeNormal
                                                    }

                                                    Text {
                                                        Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                                        Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                                        text: modelData.duration + " @ " + modelData.sampleFreq
                                                        color: GlobalIncludes.mtxFontColor
                                                        font.pointSize: GlobalIncludes.mtxFontSizeNormal
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.minimumWidth: GlobalIncludes.mtxBorderLineStrength
                        Layout.maximumWidth: GlobalIncludes.mtxBorderLineStrength
                        color: GlobalIncludes.mtxColorOrange
                    }

                    Rectangle {
                        // shows utils
                        id: rectUtils
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        ColumnLayout {
                            anchors.fill: parent

                            Rectangle {
                                id: rectChooseChannels
                                Layout.maximumHeight: 80
                                Layout.minimumHeight: 80
                                Layout.fillWidth: true

                                ColumnLayout {
                                    anchors.fill: parent

                                    Text {
                                        Layout.alignment: Qt.AlignHcenter | Qt.AlignVCenter
                                        horizontalAlignment: Qt.AlignHCenter
                                        verticalAlignment: Qt.AlignVCenter
                                        font.pointSize: GlobalIncludes.mtxFontSizeNormal
                                        color: GlobalIncludes.mtxFontColor
                                        text: qsTr("Choose Channels")
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true

                                        MtxToggleButton {
                                            id: buttToggleCH0
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "CH0"

                                            onCheckedChanged: {
                                                tsStreamer.setPlotVisible(0, buttToggleCH0.checked);
                                            }
                                        }

                                        MtxToggleButton {
                                            id: buttToggleCH1
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "CH1"

                                            onCheckedChanged: {
                                                tsStreamer.setPlotVisible(1, buttToggleCH1.checked);
                                            }
                                        }

                                        MtxToggleButton {
                                            id: buttToggleCH2
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "CH2"

                                            onCheckedChanged: {
                                                tsStreamer.setPlotVisible(2, buttToggleCH2.checked);
                                            }
                                        }

                                        MtxToggleButton {
                                            id: buttToggleCH3
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "CH3"

                                            onCheckedChanged: {
                                                tsStreamer.setPlotVisible(3, buttToggleCH3.checked);
                                            }
                                        }

                                        MtxToggleButton {
                                            id: buttToggleCH4
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: "CH4"

                                            onCheckedChanged: {
                                                tsStreamer.setPlotVisible(4, buttToggleCH4.checked);
                                            }
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.maximumHeight: GlobalIncludes.mtxBorderLineStrength
                                Layout.minimumHeight: GlobalIncludes.mtxBorderLineStrength
                                Layout.fillWidth: true
                                color: GlobalIncludes.mtxColorOrange
                            }

                            Rectangle {
                                Layout.maximumHeight: 40
                                Layout.minimumHeight: 40
                                Layout.fillWidth: true

                                RowLayout {
                                    anchors.fill: parent

                                    Text {
                                        id: labWindowSize
                                        Layout.fillHeight: true
                                        Layout.fillWidth: false
                                        Layout.alignment: Qt.AlignHcenter | Qt.AlignVCenter
                                        horizontalAlignment: Qt.AlignHCenter
                                        verticalAlignment: Qt.AlignVCenter
                                        font.pointSize: GlobalIncludes.mtxFontSizeNormal
                                        color: GlobalIncludes.mtxFontColor
                                        text: qsTr("Choose Window Size  ")
                                    }

                                    MtxComboBox {
                                        id: cbWindowSize
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true

                                        Timer {
                                           id: indexChangedTimer
                                           interval: 10;
                                           running: false
                                           triggeredOnStart: false
                                           onTriggered: {
                                               if (toggleScopeMode.checked == false)
                                               {
                                                   toggleScopeMode.checked = true
                                                   indexChangedTimer2.start()
                                               }
                                               else
                                               {
                                                   tsStreamer.setWindowSize(parent.currentText, false);
                                               }
                                           }
                                        }

                                        Timer {
                                           id: indexChangedTimer2
                                           interval: 1200
                                           repeat: false
                                           running: false
                                           triggeredOnStart: false
                                           onTriggered: {
                                               tsStreamer.setWindowSize(parent.currentText, false);
                                               toggleScopeMode.checked = false
                                           }
                                        }

                                        model: ListModel {
                                            ListElement { text: "256"}
                                            ListElement { text: "512"}
                                            ListElement { text: "1024"}
                                            ListElement { text: "2048"}
                                            ListElement { text: "4096"}
                                            ListElement { text: "8192"}
                                            ListElement { text: "16384"}
                                            ListElement { text: "32768"}
                                            ListElement { text: "65536"}
                                        }

                                        onCurrentIndexChanged: {
                                            indexChangedTimer.start()
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.maximumHeight: GlobalIncludes.mtxBorderLineStrength
                                Layout.minimumHeight: GlobalIncludes.mtxBorderLineStrength
                                Layout.fillWidth: true
                                color: GlobalIncludes.mtxColorOrange
                            }

                            MtxToggleButton {
                                id: toggleScopeMode
                                checked: true
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                text: "Scope Mode: Inactive"

                                Timer {
                                   id: timerUpdateScopeBuffer
                                   interval: 500;
                                   repeat: true
                                   running: false
                                   triggeredOnStart: false
                                   onTriggered: {
                                        tsStreamer.showTSBufferScopeMode();
                                   }
                                }

                                onCheckedChanged: {
                                    if (toggleScopeMode.checked == true)
                                    {
                                        toggleScopeMode.text = "Scope Mode: Inactive"
                                        timerUpdateScopeBuffer.stop ();
                                    }
                                    else
                                    {
                                        toggleScopeMode.text = "Scope Mode: Active"
                                        timerUpdateScopeBuffer.start();
                                    }
                                }
                            }

                            MtxToggleButton {
                                id: toggleShowStackedSpectra
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                checked: true
                                text: "Show Stacked Spectra"

                                Timer {
                                   id: timerStartShowStackedSpectra
                                   interval: 100;
                                   running: false
                                   triggeredOnStart: false
                                   onTriggered: {
                                       tsStreamer.showSTSpectra();
                                   }
                                }

                                onCheckedChanged: {
                                    if (toggleShowStackedSpectra.checked == true)
                                    {
                                        toggleShowStackedSpectra.text = "Show Stacked Spectra"
                                        tsStreamer.cancelShowSTSpectra();
                                    }
                                    else
                                    {
                                        toggleShowStackedSpectra.text = "CANCEL: \"Show Stacked Spectra\"";
                                        timerStartShowStackedSpectra.start()
                                    }
                                }
                            }

                            Rectangle {
                                id: rectSpacer
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }
        }
    }




    header: ToolBar {
        id: toolbar
        RowLayout {
           anchors.fill: parent

           ToolButton {
               Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
               Layout.maximumWidth: 70
               text: qsTr("≡")
               font.pointSize: GlobalIncludes.mtxFontSizeBig
               onClicked: stack.pop()
           }

           Image {
               id: logo
               Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
               Layout.maximumHeight: 55
               Layout.maximumWidth: 150
               fillMode: Image.PreserveAspectFit
               source: "/pics/metronix_Logo_einzeln_4c_09-2017.png"
           }

           ToolButton {
               Layout.maximumWidth: 70
               Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
               text: qsTr("⋮")
               font.pointSize: GlobalIncludes.mtxFontSizeBig
               font.bold: true
               onClicked: menu.open()
           }
       }
   }

    Rectangle{
        id: mainrect
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            SwipeView {
                id: swipeView
                Layout.fillHeight: true
                Layout.fillWidth: true
                currentIndex: tabBar.currentIndex
                clip: true
                interactive: false

                PageScan {
                }

                PagePlotTS {
                }

                PagePlotSpectra {
                }
            }

            Rectangle {
                Layout.maximumHeight: GlobalIncludes.mtxFontSizeBig * 1.5
                Layout.minimumHeight: GlobalIncludes.mtxFontSizeBig * 1.5
                Layout.fillWidth: true

                TabBar {
                    id: tabBar
                    currentIndex: swipeView.currentIndex

                    anchors.fill: parent

                    background: Rectangle {
                        color: GlobalIncludes.mtxColorOrange
                    }

                    TabButton {
                        background: Rectangle {
                            color: parent.checked ? GlobalIncludes.mtxColorOrange : GlobalIncludes.mtxFontColor
                        }

                        font.pointSize: GlobalIncludes.mtxFontSizeNormal

                        contentItem:
                        Text {
                           text: parent.text
                           font: parent.font

                           color: "white"

                           horizontalAlignment: Text.AlignHCenter
                           verticalAlignment: Text.AlignVCenter
                           elide: Text.ElideRight
                        }

                        onCheckedChanged: {
                            if (checked == true)
                            {
                                toolbar.visible = true;
                            }
                        }

                        text: qsTr("Scan For Systems")
                    }

                    TabButton {
                        background: Rectangle {
                            color: parent.checked ? GlobalIncludes.mtxColorOrange : GlobalIncludes.mtxFontColor
                        }

                        font.pointSize: GlobalIncludes.mtxFontSizeNormal

                        contentItem:
                        Text {
                           text: parent.text
                           font: parent.font

                           color: "white"

                           horizontalAlignment: Text.AlignHCenter
                           verticalAlignment: Text.AlignVCenter
                           elide: Text.ElideRight
                        }

                        onCheckedChanged: {
                            if (checked == true)
                            {
                                toolbar.visible = false
                            }
                        }

                        text: qsTr("Time Series")
                    }

                    TabButton {
                        background: Rectangle {
                            color: parent.checked ? GlobalIncludes.mtxColorOrange : GlobalIncludes.mtxFontColor
                        }

                        font.pointSize: GlobalIncludes.mtxFontSizeNormal

                        contentItem:
                        Text {
                           text: parent.text
                           font: parent.font

                           color: "white"

                           horizontalAlignment: Text.AlignHCenter
                           verticalAlignment: Text.AlignVCenter
                           elide: Text.ElideRight
                        }

                        onCheckedChanged: {
                            if (checked == true)
                            {
                                toolbar.visible = false
                            }
                        }

                        text: qsTr("Spectra")
                    }
                }
            }

            Rectangle {
                Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 1.5
                Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 1.5
                Layout.fillWidth: true
                border.color: GlobalIncludes.mtxBorderColor
                border.width: GlobalIncludes.mtxBorderLineStrength
                color: "lightgrey"

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font.pointSize: GlobalIncludes.mtxFontSizeNormal
                    font.bold: true
                    color: GlobalIncludes.mtxFontColor
                    text: "="
                }
            }
        }
    }

    footer : Rectangle {
        // no content - only to show popups for data readout
    }
}
