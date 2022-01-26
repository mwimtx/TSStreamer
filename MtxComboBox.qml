import QtQuick 2.6
import QtQuick.Controls 2.1

import "GlobalIncludes.js" as GlobalIncludes

ComboBox {
    id: control
    model: ["First", "Second", "Third"]
    font.pointSize: GlobalIncludes.mtxFontSizeNormal

    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: modelData
            color: GlobalIncludes.mtxFontColor
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: GlobalIncludes.mtxFontSizeNormal * 1.6
        height: GlobalIncludes.mtxFontSizeNormal
        contextType: "2d"

        Connections {
            target: control
            onPressedChanged: canvas.requestPaint()
        }

        onPaint: {
            if (context != NULL)
            {
                context.reset();
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = "white";
                context.fill();
            }
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: "white"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        color: GlobalIncludes.mtxFontColor
        radius: 2
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: GlobalIncludes.mtxBorderLineStrength

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            border.color: GlobalIncludes.mtxFontColor
            border.width: GlobalIncludes.mtxBorderLineStrength
            radius: 2
        }
    }
}
