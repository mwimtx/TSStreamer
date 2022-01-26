import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.4

import "GlobalIncludes.js" as GlobalIncludes

Button {
    id: button
    property string textContent: ""
    property int fontPointSize: GlobalIncludes.mtxFontSizeNormal

    property int minimumheight: GlobalIncludes.mtxFontSizeNormal * 3
    property int maximumheight: GlobalIncludes.mtxFontSizeNormal * 3

    Layout.maximumHeight: maximumheight
    Layout.minimumHeight: minimumheight
    font.pointSize: fontPointSize
    text: textContent

    contentItem: Text {
       text: button.text
       font: button.font
       color: "white"
       horizontalAlignment: Text.AlignHCenter
       verticalAlignment: Text.AlignVCenter
       elide: Text.ElideRight
    }

    background: Rectangle {
        border.width: 0
        border.color: GlobalIncludes.mtxBorderColor
        color: button.pressed ? GlobalIncludes.mtxColorOrange : GlobalIncludes.mtxButtonColorActive
        radius: 1

        /*
        gradient: Gradient {
            GradientStop { position: -0.3 ; color: button.pressed ? GlobalIncludes.mtxButtonColorActive        : GlobalIncludes.mtxButtonColorActiveGradient }
            GradientStop { position: 1.3 ; color: button.pressed ? GlobalIncludes.mtxButtonColorActiveGradient : GlobalIncludes.mtxButtonColorActive }
        }
        */
    }
}
