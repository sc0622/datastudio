import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as T
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Styles.Flat 1.0 as Flat
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtQuick.Extras 1.4
import "gauge.js" as DataJs

Page {
    id: root
    clip: true
    leftPadding: 8
    topPadding: 2
    bottomPadding: 2
    Layout.minimumWidth: 40
    Layout.fillWidth: true
    Layout.fillHeight: true
    Behavior on implicitWidth { NumberAnimation { } }
    property alias value: gauge.value
    property alias minimumValue: gauge.minimumValue
    property alias maximumValue: gauge.maximumValue
    property string suffix: ""
    property real decimals: 0
    property int tickmarkCount: 10
    property int labelCount: 10
    property alias fontSize: labelValue.font.pointSize

    background: Rectangle {
        color: "#30888888"
        radius: 5
    }

    contentItem: ColumnLayout {
        spacing: 0
        Label {
            id: labelName
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignLeft
            height: text.length > 0 ? 30 : 0
            opacity: 0.7
            text: root.title
        }
        Item { Layout.fillHeight: true }
        CircularGauge {
            id: gauge
            implicitWidth: root.implicitWidth - 3   //TODO [warning]
            //width: root.implicitWidth - 3
            Layout.alignment: Qt.AlignHCenter
            implicitHeight: implicitWidth
            Behavior on value { NumberAnimation { } }
            style: CircularGaugeStyle {
                /*
                tickmarkInset: outerRadius * 0.06
                minorTickmarkInset: tickmarkInset
                labelInset: outerRadius * 0.23
*/
                tickmarkStepSize:(maximumValue - minimumValue) / root.tickmarkCount
                labelStepSize: (maximumValue - minimumValue) / root.labelCount
                minorTickmarkCount: 4

                background: Image {
                    antialiasing: true
                    source: "../../image/gauge/background.png"
                }
                tickmark: Rectangle {
                    visible: (styleData.value % tickmarkStepSize) == 0
                    antialiasing: true
                    implicitWidth: outerRadius * 0.03
                    implicitHeight: outerRadius * 0.09
                    radius: width / 2
                    //color: styleData.value > control.value ? "#aaa" : Material.accent
                    color: "#aaa"
                }

                minorTickmark: Rectangle {
                    antialiasing: true
                    implicitWidth: outerRadius * 0.03
                    implicitHeight: outerRadius * 0.04
                    radius: width / 2
                    //color: styleData.value > control.value ? "#aaa" : Material.accent
                    color: "#777"
                }
                tickmarkLabel: Label {
                    antialiasing: true
                    renderType: Text.QtRendering
                    font.pointSize: Math.min(12, Math.max(8, outerRadius * 0.04))
                    font.family: Flat.FlatStyle.fontFamily
                    font.weight: Font.Light
                    text: styleData.value
                    color: Material.foreground
                }
                /*
                foreground: Item {
                    Rectangle {
                        width: outerRadius * 0.1
                        height: width
                        radius: width / 2
                        color: "#aaa"
                        anchors.centerIn: parent
                    }
                }
                needle: Rectangle {
                    y: outerRadius * 0.1
                    implicitWidth: outerRadius * 0.015
                    implicitHeight: outerRadius * 1
                    antialiasing: true
                    color: "#aaa"
                    radius: width / 2
                }*/
                foreground: Item {
                    Image {
                        antialiasing: true
                        anchors.centerIn: parent
                        source: "../../image/gauge/center.png"
                        scale: (outerRadius * 0.25) / sourceSize.height
                    }
                }
                needle: Image {
                    id: needleImage
                    antialiasing: true
                    transformOrigin: Item.Bottom
                    source: "../../image/gauge/needle.png"
                    scale: {
                        var distanceFromLabelToRadius = labelInset / 2;
                        var idealHeight = outerRadius - distanceFromLabelToRadius;
                        var originalImageHeight = needleImage.sourceSize.height;
                        idealHeight / originalImageHeight;
                    }
                }
            }
        }
        RowLayout {
            Item { Layout.fillWidth: true }
            Label {
                id: labelValue
                Layout.topMargin: -3
                font.pointSize: 18
                font.weight: Font.Light
                font.family: Flat.FlatStyle.fontFamily
                renderType: Text.QtRendering
                Material.foreground: Material.accent
                text: value.toFixed(decimals).toString()
                Label {
                    id: labelSuffix
                    font.pointSize: fontSize / 2
                    font.weight: Font.Light
                    font.family: 'Noto Sans S Chinese Bold'
                    renderType: Text.QtRendering
                    Material.foreground: Material.accent
                    anchors {
                        left: parent.right
                        top: parent.top
                        leftMargin: 2
                    }
                    textFormat: Text.RichText
                    text: suffix
                }
            }
            Item { Layout.fillWidth: true }
        }
    }

    function autoScaled(from, to, intervals) {
        var scaleInfo = DataJs.autoScaled(from, to, intervals)
        root.minimumValue = scaleInfo[0]
        root.maximumValue = scaleInfo[1]
        root.tickmarkCount = scaleInfo[2]
        root.labelCount = scaleInfo[2]
    }
}
