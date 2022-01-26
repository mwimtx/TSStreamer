import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
//import QtQuick.Extras 1.4


import "GlobalIncludes.js" as GlobalIncludes

Item {

    Connections {
        target: tsStreamer
        onSigConnectingUpdated: {
            infoBusy.visible = tsStreamer.connecting
       }
    }

    Connections {
        target: tsStreamer
        onSigConnectDirectTargetIPUpdated: {
            teTargetIP.text = tsStreamer.directTargetIP
            infoNoScanDoneYet.visible = false
            tsStreamer.directConnectToSystem(teTargetIP.text)
            swipeView.setCurrentIndex(1)
            drawerUtils.open ();
        }
    }

    Timer {
        id: timerDirectConnect1
        interval: 250
        repeat: false
        running : true
        triggeredOnStart: false
        onTriggered: {
            if (tsStreamer.connectDirectTargetIP == true)
            {
                teTargetIP.text = tsStreamer.directTargetIP
                infoNoScanDoneYet.visible = false
                if (tsStreamer.directConnectToSystem(teTargetIP.text) == true)
                {
                    swipeView.setCurrentIndex(1)
                    drawerUtils.open ();
                }
                tsStreamer.connectDirectTargetIP = false
                timerDirectConnect2.start()
            }
        }
    }

    Timer {
        id: timerDirectConnect2
        interval: 250
        repeat: false
        running : false
        triggeredOnStart: false
        onTriggered: {
            tsStreamer.connectToSystem(0);
            swipeView.setCurrentIndex(1);
            drawerUtils.open ();
            tsStreamer.directOpenTSScopeMode = true;
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins:  GlobalIncludes.mtxMargin

        ColumnLayout {
            anchors.fill: parent
            Layout.margins: GlobalIncludes.mtxMargin

            Rectangle {
                id: rectDirectConnectADU
                Layout.fillWidth: true
                Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 4
                Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 4

                RowLayout {
                    anchors.fill: parent

                    MtxTextEdit {
                        id: teTargetIP
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        placeholderText: "Enter Target IP Address"

                        Timer {
                            id: timeCheckCarriagereturn
                            interval: 10
                            running: true
                            repeat: true
                            triggeredOnStart: true
                            onTriggered: {
                                if ((teTargetIP.text.charAt(teTargetIP.text.length - 1) == '\a') ||
                                    (teTargetIP.text.charAt(teTargetIP.text.length - 1) == '\n'))
                                {
                                    teTargetIP.text = teTargetIP.text.substring(0, teTargetIP.text.length - 1);
                                    infoNoScanDoneYet.visible = false;
                                    if (tsStreamer.directConnectToSystem(teTargetIP.text) == true)
                                    {
                                        swipeView.setCurrentIndex(1)
                                        drawerUtils.open ();
                                    }
                                }
                            }
                        }
                    }

                    MtxButton {
                        id: pbDirectConnectADU
                        Layout.fillHeight: true
                        Layout.maximumWidth: 80
                        text: "Connect"

                        onClicked: {
                            infoNoScanDoneYet.visible = false;
                            if (tsStreamer.directConnectToSystem(teTargetIP.text) == true)
                            {
                                swipeView.setCurrentIndex(1)
                                drawerUtils.open ();
                            }
                        }
                    }
                }
            }


            Rectangle {
                id: rectScanForADU
                Layout.fillWidth: true
                Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 3
                Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 3

                Timer {
                    id: timerStartScan
                    interval: 100
                    repeat: false
                    triggeredOnStart: false
                    onTriggered: {
                        tsStreamer.scanForSystems();
                    }
                }

                MtxButton {
                    id: pbScanForADUs
                    anchors.fill: parent
                    text: "Scan For ADU Systems"

                    onClicked: {
                        infoNoScanDoneYet.visible = false;
                        infoBusy.visible = true;
                        timerStartScan.start();
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.maximumHeight: GlobalIncludes.mtxBorderLineStrength
                Layout.minimumHeight: GlobalIncludes.mtxBorderLineStrength
                color: GlobalIncludes.mtxColorOrange
            }

            Rectangle {
                id: rectADUList
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: GlobalIncludes.mtxBorderLineStrength

                Text {
                    id: infoNoScanDoneYet
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    text: "Scan For ADU Systems\nOr Use Direct Connect With\nTarget IP Address!"
                    font.pointSize: GlobalIncludes.mtxFontSizeNormal
                    color: GlobalIncludes.mtxFontColor
                    visible: true
                }

                Image {
                    id: infoBusy
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors.horizontalCenter: rectADUList.horizontalCenter
                    anchors.verticalCenter: rectADUList.verticalCenter
                    height: rectADUList.height * 0.5
                    width: infoBusy.height
                    visible: false
                    source: "/pics/busy.png"
                    fillMode: Image.PreserveAspectFit
                    NumberAnimation on rotation { duration: 3000; from:0; to: 360; loops: Animation.Infinite}
                }

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: GlobalIncludes.mtxBorderLineStrength
                    ScrollBar.vertical.interactive: true
                    clip: true

                    ListView {
                        id: theListView
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.margins: GlobalIncludes.mtxMargin
                        model: tsStreamer.systemList


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

                            Component.onCompleted: {
                                infoBusy.visible = false;
                            }

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
                                            tsStreamer.connectToSystem(modelData.index);
                                            swipeView.setCurrentIndex(1);
                                            drawerUtils.open ();
                                        }
                                    }

                                    RowLayout {
                                        anchors.fill: parent
                                        Layout.margins: GlobalIncludes.mtxMargin
                                        anchors.centerIn: rectSystemInfos

                                        Text {
                                            Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                            Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 1.2
                                            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                                            text: "System Type: " + modelData.name + " / IP: " + modelData.ipAddress
                                            color: GlobalIncludes.mtxFontColor
                                            font.pointSize: GlobalIncludes.mtxFontSizeBig
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
