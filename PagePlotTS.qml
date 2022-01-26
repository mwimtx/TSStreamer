import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import CustomPlot 1.0
//import QtQuick.Extras 1.4

import "GlobalIncludes.js" as GlobalIncludes

Item {

    Rectangle {
        id: rectPlotTSBase
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent

            Rectangle {
                // spacer to be able to use drawer
                Layout.fillWidth: true
                Layout.minimumHeight: 10
                Layout.maximumHeight: 10
            }

            Rectangle {
                //Layout.alignment:  Qt.AlignCenter
                Layout.fillHeight: true
                Layout.fillWidth:  true

                RowLayout {
                    anchors.fill: parent

                    Timer {
                       id: timerShowPrevBuffer
                       interval: 40;
                       triggeredOnStart: false
                       onTriggered: {
                           tsStreamer.showPrevTSBuffer();
                           buttonShowPrevBuffer.enabled = true
                       }
                    }

                    MtxButton {
                        id: buttonShowPrevBuffer
                        minimumheight: rectPlotTSBase.height * 0.5
                        maximumheight: rectPlotTSBase.height * 0.5
                        Layout.maximumWidth: 50
                        font.pointSize: 20
                        text: "<"

                        onClicked: {
                            if (timerShowPrevBuffer.running == false)
                            {
                                buttonShowPrevBuffer.enabled = false
                                timerShowPrevBuffer.restart();
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.margins: 0
                        anchors.margins: 0

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 0

                            CustomPlotItem {
                                //Layout.alignment:  Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth:  true
                                id: plotTs1
                                objectName: "plotTs1"
                                Component.onCompleted: initCustomPlot(0, 0)
                            }

                            CustomPlotItem {
                                //Layout.alignment:  Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth:  true
                                id: plotTs2
                                objectName: "plotTs2"
                                Component.onCompleted: initCustomPlot(0, 1)
                            }

                            CustomPlotItem {
                                //Layout.alignment:  Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth:  true
                                id: plotTs3
                                objectName: "plotTs3"
                                Component.onCompleted: initCustomPlot(0, 2)
                            }

                            CustomPlotItem {
                                //Layout.alignment:  Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth:  true
                                id: plotTs4
                                objectName: "plotTs4"
                                Component.onCompleted: initCustomPlot(0, 3)
                            }

                            CustomPlotItem {
                                //Layout.alignment:  Qt.AlignCenter
                                Layout.fillHeight: true
                                Layout.fillWidth:  true
                                id: plotTs5
                                objectName: "plotTs5"
                                Component.onCompleted: initCustomPlot(0, 4)
                            }

                            Slider {
                                id: slideTSPos
                                from: 0
                                to: tsStreamer.maxSamples
                                value: tsStreamer.actSamplePos

                                Layout.fillWidth: true
                                Layout.maximumHeight: 50
                                Layout.minimumHeight: 50

                                onMoved: {
                                    if (slideTimer.running == false)
                                    {
                                       slideTimer.restart();
                                    }
                                }
                            }
                        }

                        Timer {
                           id: slideTimer
                           interval: 250;
                           triggeredOnStart: false
                           onTriggered: {
                               tsStreamer.showTSBufferAtPos(slideTSPos.value);
                           }
                        }
                    }

                    Timer {
                       id: timerShowNextBuffer
                       interval: 40;
                       triggeredOnStart: false
                       onTriggered: {
                           tsStreamer.showNextTSBuffer();
                           buttonShowNextBuffer.enabled = true
                       }
                    }

                    MtxButton {
                        id: buttonShowNextBuffer
                        minimumheight: rectPlotTSBase.height * 0.5
                        maximumheight: rectPlotTSBase.height * 0.5
                        Layout.maximumWidth: 50
                        font.pointSize: 20
                        text: ">"

                        onClicked: {
                            if (timerShowNextBuffer.running == false)
                            {
                                buttonShowNextBuffer.enabled = false
                                timerShowNextBuffer.restart();
                            }
                        }
                    }
                }
            }
        }
    }
}
