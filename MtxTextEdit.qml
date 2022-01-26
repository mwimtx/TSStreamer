import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import "GlobalIncludes.js" as GlobalIncludes

TextArea {
    id: textEdit
    placeholderText: ""
    Layout.maximumHeight: GlobalIncludes.mtxFontSizeNormal * 3
    Layout.minimumHeight: GlobalIncludes.mtxFontSizeNormal * 3
    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
    horizontalAlignment: Qt.AlignLeft
    verticalAlignment: Qt.AlignVCenter
    font.pointSize: GlobalIncludes.mtxFontSizeNormal
    color: "white"
    clip: true

    background: Rectangle {
        border.width: 0
        border.color: GlobalIncludes.mtxBorderColor
        color: GlobalIncludes.mtxButtonColorActive
        radius: 0
    }

}
